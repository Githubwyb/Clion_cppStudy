/**
 * @file ReuseUdpSender.cpp
 * @brief udp复用cket发送，生命周期由外部维护，析构就不会再进行接受
 *
 *  开始/停止状态维护：
 *      1. 首次发送会开始接收
 *      2. 主动取消会触发接受回调停止接收
 *      3. 析构会停止接收
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

#ifndef REUSEUDPSENDER_HPP
#define REUSEUDPSENDER_HPP

#include <memory>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>

#include <ensure/ensure.h>
#include <net/target_address.h>

#include <Log/Log.hpp>
#include <utilities/AsioUdpHelper.hpp>
#include <utilities/ec_to_string.hpp>

namespace asio {
class ReuseUdpSender;
using ReuseUdpSenderPtr = std::shared_ptr<ReuseUdpSender>;
class ReuseUdpSender : public enable_shared_from_this<ReuseUdpSender> {
public:
    using IOContext    = boost::asio::io_context;
    using IOContextPtr = std::shared_ptr<IOContext>;
    using UdpSock      = boost::asio::ip::udp::socket;
    using UDPEndpoint  = boost::asio::ip::udp::endpoint;
    using DataBuffer   = std::vector<uint8_t>;
    // 发送完成回调
    using SendDone =
        std::function<void(std::size_t length, const std::string &connStr, const boost::system::error_code &ec)>;
    // 接受回调
    using RecvCallback = std::function<void(const uint8_t                   *pData,
                                            std::size_t                      length,
                                            const std::string               &connStr,
                                            const boost::system::error_code &ec)>;

    static ReuseUdpSenderPtr create(const target_address &dst, IOContext &ioContext, const RecvCallback &recvCallback) {
        return std::shared_ptr<ReuseUdpSender>(new ReuseUdpSender(dst, ioContext, recvCallback));
    }

    ~ReuseUdpSender() {
        cancel();
        boost::system::error_code ignoreEC;
        m_udpSock.shutdown(boost::asio::socket_base::shutdown_type::shutdown_both, ignoreEC);
        m_udpSock.close(ignoreEC);
    }

    /**
     * @brief 取消接收
     */
    void cancel() {
        boost::system::error_code ignoreEC;
        m_udpSock.cancel(ignoreEC);
    }

    /**
     * @brief 异步发送数据，调用之后内部会接管生命周期，需要显式调用取消否则会造成内存泄漏
     * @param data
     * @param dst 目的地址
     * @param ctx 包上下文
     * @param sendDone 发送完成回调
     */
    void asyncSend(const DataBuffer &data, const SendDone &sendDone) {
        if (!m_udpSock.is_open()) {
            ENSURE_DEBUG(false).msg("socket is not open");
            return;
        }

        std::weak_ptr<ReuseUdpSender> weakPtr = shared_from_this();
        auto sendDoneCallback = [weakPtr, sendDone](const boost::system::error_code &ec, std::size_t length) {
            auto self = weakPtr.lock();
            if (!self) return;

            self->m_connStr = asio::getConnStr(self->m_udpSock, self->m_dstEP);
            sendDone(length, self->m_connStr, ec);

            if (!self->m_beginRecv) {
                self->m_beginRecv = true;
                self->asyncRecv();
            }
        };

        m_udpSock.async_send_to(boost::asio::buffer(data.data(), data.size()), m_dstEP, sendDoneCallback);
    }

    UdpSock       &getSocket() { return m_udpSock; }
    const UdpSock &getSocket() const { return m_udpSock; }

private:
    /**
     * @brief construct a ReuseUdpSender object
     * @param dst 目的地址
     * @param ioContext ioContext
     */
    ReuseUdpSender(const target_address &dst, IOContext &ioContext, const RecvCallback &recvCallback)
        : m_dstAddr(dst),
          m_dstEP(boost::asio::ip::address::from_string(targetAddress::toStringWithoutPort(dst)), ntoh16(dst.port)),
          m_udpSock(ioContext),
          m_recvCallback(recvCallback),
          m_beginRecv(false) {
        boost::system::error_code ec;
        m_udpSock.open(m_dstEP.protocol(), ec);
        // open失败直接抛异常，内部无法处理
        if (ec) throw ec;
        m_connStr = asio::getConnStr(m_udpSock, m_dstEP);
    }

    void asyncRecv() {
        std::weak_ptr<ReuseUdpSender> weakPtr      = shared_from_this();
        auto                             recvCallback = [weakPtr](boost::system::error_code ec, std::size_t length) {
            auto self = weakPtr.lock();
            if (!self) return;

            if (ec.value() == boost::asio::error::operation_aborted) {
                // 手动取消就结束
                self->m_beginRecv = false;
                return;
            }

            std::string tag = "reuse udp sender receive callback " + self->m_connStr;
            do {
                // 容错处理
                if (ec || length == 0) {
                    LOGW(WHAT("{} receive error, ignore it", tag),
                         REASON("get ec {}, length {}", std::to_string(ec), length));
                    break;
                }

                self->m_recvCallback(self->m_data.data(), length, self->m_connStr, ec);
            } while (false);
            // 继续接收
            self->asyncRecv();
        };

        m_udpSock.async_receive_from(boost::asio::buffer(m_data), m_dstEP, recvCallback);
    }

    UdpSock m_udpSock;
    bool m_beginRecv;  // 是否开始接收数据，复用socket时，只有第一次发送数据时才开始接收数据

    std::array<uint8_t, 1024> m_data;  ///< recv buffer
    UDPEndpoint               m_dstEP;
    target_address            m_dstAddr;
    std::string               m_connStr;  ///< fd xxx x.x.x.x:123 => x.x.x.x:456
    std::string               m_src;      ///< xxx x.x.x.x:123
    const RecvCallback        m_recvCallback;
};

}  // namespace asio

#endif  // UDPSENDER_HPP
