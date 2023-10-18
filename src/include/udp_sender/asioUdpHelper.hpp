//
// Created by wangyubo on 11/5/22.
//
#pragma once

#include <boost/asio.hpp>
#include <sstream>
#include <string>

namespace asio {
static std::string getDstStr(const boost::asio::ip::udp::endpoint& dstEP) {
    std::string destStr = dstEP.address().to_string() + ":" + std::to_string(dstEP.port());
    return destStr;
}

static inline void ignoreCancel(boost::asio::steady_timer& timer) {
    boost::system::error_code ignoreEC;
    timer.cancel(ignoreEC);
}

static inline std::string getSrcAddr(const boost::asio::ip::udp::socket& sock) {
    // 这里跨平台保持一致，linux上没有获取到源地址默认赋值成0.0.0.0:0，windows会抛异常
    std::string result = "0.0.0.0:0";
    if (!sock.is_open()) {
        return result;
    }

    try {
        auto srcStr = sock.local_endpoint().address().to_string() + ":" + std::to_string(sock.local_endpoint().port());
        result = srcStr;
    } catch (...) {
        // 这里可能是没有连接，不存在源地址，不报错
    }

    return result;
}

static std::string getDstStr(const boost::asio::ip::udp::socket& sock) {
    // 这里跨平台保持一致，linux上没有获取到源地址默认赋值成0.0.0.0:0，windows会抛异常
    std::string result = "0.0.0.0:0";
    if (!sock.is_open()) {
        return result;
    }

    try {
        auto srcStr =
            sock.remote_endpoint().address().to_string() + ":" + std::to_string(sock.remote_endpoint().port());
        result = srcStr;
    } catch (...) {
        // 这里可能是没有连接，不存在地址，不报错
    }

    return result;
}

static inline std::string getConnStr(boost::asio::ip::udp::socket& sock) {
    std::stringstream ss;
    ss << getSrcAddr(sock) << " => " << getDstStr(sock) << " fd " << sock.native_handle();
    return ss.str();
}

static inline std::string getConnStr(boost::asio::ip::udp::socket& sock, const boost::asio::ip::udp::endpoint& dstEP) {
    std::stringstream ss;
    ss << getSrcAddr(sock) << " => " << getDstStr(dstEP) << " fd " << sock.native_handle();
    return ss.str();
}

static inline std::string getConnStr(boost::asio::ip::udp::socket& sock, const std::string& dst) {
    std::stringstream ss;
    ss << getSrcAddr(sock) << " => " << dst << " fd " << sock.native_handle();
    return ss.str();
}

}  // namespace asio
