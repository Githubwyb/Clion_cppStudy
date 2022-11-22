/**
 * @file main.cpp
 * @brief 启动函数所在位置
 * @author wangyubo
 * @version v2.0.0
 * @date 2020-12-16
 */
#include <errno.h>
#include <resolv.h>
#include <sys/inotify.h>
#include <time.h>
#include <unistd.h>

#include <algorithm>
#include <bitset>
#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/multi_index/key_extractors.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/stacktrace.hpp>
#include <fstream>
#include <future>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <system_error>
#include <thread>
#include <vector>

#include "FileMonitor.hpp"
#include "UdpSender.hpp"
#include "baseInstance.hpp"
#include "bugReport.hpp"
#include "common.hpp"
#include "ec_to_string.hpp"
#include "ip_address.hpp"
#include "log.hpp"
#include "socketHelper.hpp"
#include "target_address.h"
#include "threadPool.hpp"

using namespace sangfor;
using UdpSock = boost::asio::ip::udp::socket;
using UdpSockPtr = std::shared_ptr<UdpSock>;
using IOContextWorker = boost::asio::io_context::work;
using IOContextWorkerPtr = std::unique_ptr<IOContextWorker>;

using DataBuffer = std::vector<uint8_t>;
using DataBufferPtr = std::shared_ptr<std::vector<uint8_t>>;

/**
 * @brief udp socket struct
 *
 */
struct UdpSockStruct {
    std::string dstAddr;
    UdpSockPtr sockPtr = nullptr;  // socket ptr
};
using UdpSockStructPtr = std::shared_ptr<UdpSockStruct>;

class IOContextThreadTest {
public:
    IOContextThreadTest() : m_pIOContext(std::make_shared<IOContext>()) {
        m_pIOContextWorker = std::make_unique<IOContextWorker>(*m_pIOContext);
    }

    ~IOContextThreadTest() { stop(); }

    void start() {
        stop();

        std::promise<void> p;
        m_future = std::async(std::launch::async, [&p, this]() {
            if (m_pIOContext->stopped()) m_pIOContext->restart();
            p.set_value();
            m_pIOContext->run();
        });
        // 等待线程起来再退出
        p.get_future().wait();
    }

    void stop() {
        if (m_pIOContext && !m_pIOContext->stopped()) {
            m_pIOContext->stop();
        }

        if (m_future.valid()) m_future.wait();
    }

    IOContextPtr m_pIOContext;
    IOContextWorkerPtr m_pIOContextWorker;
    std::future<void> m_future;
};

static IOContextThreadTest ioTest;

// udp发包的回调函数
using SendDoneCallback = std::function<void(UdpSockPtr sock, const std::error_code&)>;
using RecvCallback1 = std::function<void(DataBuffer data, const std::error_code& ec)>;

void triggerOnce(const DataBuffer& data, const target_address& dst, const SendDoneCallback& sendDone,
                 const RecvCallback1& recvCallback, IOContext& ioContext, std::error_code& ec) {
    static int id = 0;
    // 一个请求一个socket模式，直接使用sender发送即可
    auto dstStr = targetAddress::toStringWithoutPort(dst);
    auto udpSenderPtr = std::make_shared<UdpSender>(dstStr, ntohs(dst.port), *ioTest.m_pIOContext, 3000, id++);
    auto udpSendDone = [sendDone](std::size_t, const UdpSockPtr& sockPtr, const boost::system::error_code& ec) {
        sendDone(sockPtr, ec);
    };

    auto udpSenderCallback = [udpSenderPtr, recvCallback](const uint8_t* data, std::size_t size,
                                                          const boost::system::error_code& ec) -> bool {
        DataBuffer buf(data, data + size);
        recvCallback(std::move(buf), ec);
        return true;
    };

    udpSenderPtr->asyncSend(data.data(), data.size(), udpSendDone, udpSenderCallback, ec);
}

uint8_t s_data[] = {
    0x39, 0xb5, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x77, 0x77, 0x77,
    0x06, 0x67, 0x6f, 0x6f, 0x67, 0x6c, 0x65, 0x03, 0x63, 0x6f, 0x6d, 0x00, 0x00, 0x1c, 0x00, 0x01,
};

class A {
    public:
    int func() const {

    }
    int func1(int a) {
        b = a;
    }

    int b;
};

int func(const A &a) {
    auto funcTest = [a](){
        a.func1(1);    // std::string  const std::string
    };
    funcTest();
}

int main(int argc, char* argv[]) {
    (void)BugReportRegister("run", ".", nullptr, nullptr);
    spdlog::set_level(spdlog::level::debug);  // Set global log level to debug
    // spdlog::set_pattern("%Y-%m-%d %H:%M:%S [%P:%t][%L][%@ %!] %v");
    spdlog::set_pattern("%Y-%m-%d %H:%M:%S [%P:%t][%L][%s:%# %!] %v");
    LOGI(WHAT("Hello, main { }"));
    LOGD(WHAT("aaaa {}", sizeof(unsigned long)));
    LOGI(WHAT("aaaa {}", 1234));
    LOGI(WHAT("aaaa {:#06x}", 1234));
    LOGE(WHAT("get_svpn_rand"), REASON("hhh {}", 1), WILL("aaaaa"));
    LOGW(WHAT("get_svpn_rand"), REASON("hhh {}", 0x1234), NO_WILL);
    LOGI(WHAT("long {}", sizeof(long)));
    LOGI(WHAT("why {}", 4 + sizeof(size_t) - 1));
    LOGI(WHAT("why {}", (4 + sizeof(size_t) - 1) & (size_t) ~(sizeof(size_t) - 1)));

    return 0;
}
