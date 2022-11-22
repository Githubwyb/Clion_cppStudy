/**
 * @file udpSender.cpp
 * @brief udp单包单socket发送，生命周期自己维护，外部不需要关心
 *        可以重复调用，但是前一个没有发送接受完成，后一个会发送不会生效
 *
 *  开始/停止状态维护：
 *      1. 调用asyncSend函数，除非前一次没停止返回错误，否则将发送并设置开始状态
 *      2. 发送失败，设置停止
 *      3. 接受失败，设置停止
 *      4. 接受超时，定时器会调用取消，接受回调会调用，设置停止
 *      5. 外部主动取消，接受回调会调用，设置停止
 *
 *  生命周期维护：
 *      1. 调用asyncSend，生命周期归sendDone托管
 *      2. sendDone失败会减引用计数，sendDone完成托管给定时器
 *      3. 接受失败，取消定时器
 *      4. 接受超时，定时器会调用cancel，接受回调会调用，这时由接受回调管理生命周期
 *      5. 外部主动取消，取消定时器
 *
 *  sendDone回调调用：
 *      1. 调用asyncSend失败调用sendDone
 *      2. 发送出去boost库给错误调用
 *      3. 发送正常调用
 *
 *  recvCallback回调调用：
 *      1. 超时取消，调用给ec
 *      2. 主动取消，调用给ec
 *      3. 接受失败，调用给ec
 *      4. 接受成功，正常调用
 *
 *  定时器回调调用：
 *      1. 定时到期
 *      2. 成功接受，调用取消定时器，获得ec
 *      3. 主动取消，调用取消定时器，获得ec
 * @author wangyubo
 * @version v0.1
 * @date 2022-11-04
 */

#pragma once

#include <array>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <system_error>
#include <type_traits>
#include <utility>

#include <boost/asio.hpp>

#include <ensure/ensure.h>
#include <net/dns_types.h>

#include <Log/Log.hpp>
#include <utilities/AsioUdpHelper.hpp>
#include <utilities/ec_to_string.hpp>

namespace sangfor {
namespace asio {

// Class to manage the memory to be used for handler-based custom allocation.
// It contains a single block of memory which may be returned for allocation
// requests. If the memory is in use when an allocation request is made, the
// allocator delegates allocation to the global heap.
class handler_memory {
public:
    handler_memory() : m_in_use(false) {}

    handler_memory(const handler_memory&)            = delete;
    handler_memory& operator=(const handler_memory&) = delete;

    void* allocate(std::size_t size) {
        if (!m_in_use && size < sizeof(m_storage)) {
            m_in_use = true;
            return &m_storage;
        } else {
            return ::operator new(size);
        }
    }

    void deallocate(void* pointer) {
        if (pointer == &m_storage) {
            m_in_use = false;
        } else {
            ::operator delete(pointer);
        }
    }

private:
    // Storage space used for handler-based custom memory allocation.
    typename std::aligned_storage<1024>::type m_storage;

    // Whether the handler-based custom allocation storage has been used.
    bool m_in_use;
};

// The allocator to be associated with the handler objects. This allocator only
// needs to satisfy the C++11 minimal allocator requirements.
template <typename T>
class handler_allocator {
public:
    using value_type = T;

    explicit handler_allocator(handler_memory& mem) : m_memory(mem) {}

    template <typename U>
    handler_allocator(const handler_allocator<U>& other) noexcept : m_memory(other.m_memory) {}

    bool operator==(const handler_allocator& other) const noexcept { return &m_memory == &other.m_memory; }

    bool operator!=(const handler_allocator& other) const noexcept { return &m_memory != &other.m_memory; }

    T* allocate(std::size_t n) const { return static_cast<T*>(m_memory.allocate(sizeof(T) * n)); }

    void deallocate(T* p, std::size_t /*n*/) const { return m_memory.deallocate(p); }

private:
    template <typename>
    friend class handler_allocator;

    // The underlying memory.
    handler_memory& m_memory;
};

// Wrapper class template for handler objects to allow handler memory
// allocation to be customised. The allocator_type type and get_allocator()
// member function are used by the asynchronous operations to obtain the
// allocator. Calls to operator() are forwarded to the encapsulated handler.
template <typename Handler>
class custom_alloc_handler {
public:
    using allocator_type = handler_allocator<Handler>;

    custom_alloc_handler(handler_memory& m, Handler h) : m_memory(m), m_handler(h) {}

    allocator_type get_allocator() const noexcept { return allocator_type(m_memory); }

    template <typename... Args>
    void operator()(Args&&... args) {
        m_handler(std::forward<Args>(args)...);
    }

private:
    handler_memory& m_memory;
    Handler         m_handler;
};

// Helper function to wrap a handler object to add custom allocation.
template <typename Handler>
inline custom_alloc_handler<Handler> make_custom_alloc_handler(handler_memory& m, Handler h) {
    return custom_alloc_handler<Handler>(m, h);
}

/**
 * @brief udp client
 *
 */
class UdpSender;
using UdpSenderPtr = std::shared_ptr<UdpSender>;
class UdpSender : public std::enable_shared_from_this<UdpSender> {
public:
    using IOContext    = boost::asio::io_context;
    using IOContextPtr = std::shared_ptr<IOContext>;
    using UdpSock      = boost::asio::ip::udp::socket;
    using DataBuffer   = std::vector<uint8_t>;
    // 发送完成回调
    using SendDone =
        std::function<void(std::size_t length, const std::string& connStr, const boost::system::error_code& ec)>;
    // 接受回调
    using RecvCallback = std::function<bool(const uint8_t*                   pData,
                                            std::size_t                      length,
                                            const std::string&               connStr,
                                            const boost::system::error_code& ec)>;

    static UdpSenderPtr create(const std::string& dstIP, uint16_t dstPort, IOContext& ioContext, int timeout) {
        return std::shared_ptr<UdpSender>(new UdpSender(dstIP, dstPort, ioContext, timeout));
    }

    ~UdpSender() {
        cancel();
        boost::system::error_code ignoreEC;
        m_udpSock.shutdown(boost::asio::socket_base::shutdown_type::shutdown_both, ignoreEC);
        m_udpSock.close(ignoreEC);
    }

    /**
     * @brief 取消发送，会立即调用recvCallback并给出ec
     */
    void cancel() {
        boost::system::error_code ignoreEC;
        m_udpSock.cancel(ignoreEC);
    }

    /**
     * @brief 发送udp包，单次仅允许调用一次，前一个未调用callback不允许再次调用
     *
     * @param[in] data
     * @param[in] len
     * @param[in] sendDone send done callback
     * @param[in] callback recv done callback
     */
    void asyncSend(const uint8_t* data, int len, const SendDone& sendDone, const RecvCallback& callback) {
        if (m_running) {
            ENSURE_DEBUG(false).msg("last receive is not done");
            return;
        }

        auto self                 = shared_from_this();
        auto asyncSendDoneHandler = [self, sendDone, callback](boost::system::error_code ec, std::size_t length) {
            self->m_connStr = getConnStr(self->m_udpSock, self->m_dstEP);
            std::string tag = "udp sender " + self->m_connStr + " send done";

            sendDone(length, self->m_connStr, ec);
            if (ec) {
                self->m_running = false;
                return;
            }
            LOGD(WHAT("{} send ok", tag));

            if (self->m_timeout > 0) {
                self->m_timeoutTimer.expires_after(std::chrono::milliseconds(self->m_timeout));
                // 由定时器管理self生命周期
                self->m_timeoutTimer.async_wait([self](const boost::system::error_code& ec) {
                    if (ec) {
                        // 成功接收会调用取消
                        // 外部主动cancel会由recv回调调用cancel
                        // 由于持有self的shared指针，不会由析构进入
                        LOGD(WHAT("udp sender timer get error, maybe cancel, ec {}", std::to_string(ec)));
                        return;
                    }
                    if (self->m_running) self->cancel();
                });
            }

            self->asyncRecv(callback);
        };
        m_running = true;
        m_udpSock.async_send_to(boost::asio::buffer(data, len),
                                m_dstEP,
                                make_custom_alloc_handler(m_handler_memory, asyncSendDoneHandler));
    }

    UdpSock&       getSocket() { return m_udpSock; }
    const UdpSock& getSocket() const { return m_udpSock; }

private:
    /**
     * @brief Construct a new Udp Sender object
     *
     * @param dstIP
     * @param dstPort
     * @param ioContext 用于创建定时器和socket，允许传入多线程的ioContext
     * @param timeout 定时器超时时间，单位ms，设置timeout将会自动管理生命周期，否则需要外部管理
     */
    UdpSender(const std::string& dstIP, uint16_t dstPort, IOContext& ioContext, int timeout)
        : m_dstEP(boost::asio::ip::address::from_string(dstIP), dstPort),
          m_running(false),
          m_timeoutTimer(ioContext),
          m_timeout(timeout),
          m_udpSock(ioContext) {
        boost::system::error_code ec;
        m_udpSock.open(m_dstEP.protocol(), ec);
        // open失败直接抛异常，内部无法处理
        if (ec) throw ec;
        m_connStr = getConnStr(m_udpSock, m_dstEP);
    }

    /**
     * @brief callde by do_write
     *
     * @param callback
     */
    void asyncRecv(const RecvCallback& callback) {
        auto self                 = shared_from_this();
        auto asyncRecvDoneHandler = [self, callback](boost::system::error_code ec, std::size_t length) {
            std::string tag = "udp sender " + self->m_connStr + " receive done";
            if (!self->m_running) return;

            if (ec.value() == boost::asio::error::operation_aborted) {
                // 这里属于调用socket的cancel进入
                // 定时器超时和外部主动会调用cancel进入
                // 由于持有self的shared指针，不会由析构进入
                self->m_running = false;
                ignoreCancel(self->m_timeoutTimer);
                callback(nullptr, 0, self->m_connStr, ec);
                return;
            }

            if (callback((const uint8_t*)self->m_data.data(), length, self->m_connStr, ec)) {
                self->m_running = false;
                ignoreCancel(self->m_timeoutTimer);
                return;
            }

            LOGI(WHAT("{} receive again", tag));
            self->asyncRecv(callback);
        };

        m_udpSock.async_receive_from(boost::asio::buffer(m_data),
                                     m_dstEP,
                                     make_custom_alloc_handler(m_handler_memory, asyncRecvDoneHandler));
    }

private:
    std::atomic<bool>              m_running;  ///< 是否开始了发送
    boost::asio::ip::udp::endpoint m_dstEP;    ///< dest addr

    UdpSock                m_udpSock;  ///< 内部用于发送数据的socket
    std::string            m_connStr;  ///< 连接字符串: xxx:123 => xxx:345 fd xx
    std::array<char, 1024> m_data;     ///< recv buffer

    // The memory to use for handler-based custom memory allocation.
    handler_memory m_handler_memory;

    boost::asio::steady_timer m_timeoutTimer;  ///< 用于超时使用，到点取消socket

    int m_timeout;  ///< 超时时间，ms级别，小于等于0就会一直等待收到数据
};
}  // namespace asio
}  // namespace sangfor
