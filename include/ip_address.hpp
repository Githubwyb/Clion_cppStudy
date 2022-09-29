/**
 * @file ip_address.hpp
 * @author 王钰博18433 (18433@sangfor.com)
 * @brief 定义ip类型和相关操作符
 * @version 0.1
 * @date 2022-09-01
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __CLION_CPPSTUDY_IPADDRESS_HPP__
#define __CLION_CPPSTUDY_IPADDRESS_HPP__

#ifdef __WIN32
#include <WinSock2.h>
#else
#include <netinet/in.h>
#endif

#include <string.h>

#include <iomanip>
#include <sstream>

/**
 * @brief IP地址类型
 */
enum class ip_type {
    ipv4,  ///< ipv4地址
    ipv6,  ///< ipv6地址
};

struct ip_address {
    ip_type type;  ///< ip地址类型 @ref ip_type
    union {
        char v4[4];             ///< ipv4地址
        unsigned char uv4[4];   ///< ipv4地址,unsigned char
        uint32_t v4_addr;       ///< ipv4地址,网络字节序
        uint32_t reserved[4];   ///< 用来存储临时数据
        char v6[16];            ///< ipv6地址
        unsigned char uv6[16];  ///< ipv6地址
    } ip;                       ///< ip地址

    ip_address() = default;

    // 使用uint32进行构造，传入主机字节序即可
    ip_address(uint32_t ipv4) {
        type = ip_type::ipv4;
        ip.v4_addr = htonl(ipv4);
    }

    // 使用uint32进行赋值，传入主机字节序即可
    ip_address& operator=(uint32_t ipv4) {
        type = ip_type::ipv4;
        ip.v4_addr = htonl(ipv4);
        return *this;
    }

    ip_address(const uint8_t* buf, uint32_t len) {
        if (len == 4) {
            type = ip_type::ipv4;
            memcpy(ip.uv4, buf, 4);
        } else if (len == 16) {
            type = ip_type::ipv6;
            memcpy(ip.uv6, buf, 16);
        }
    }
};

// 定义各种类型操作符
static bool operator==(const ip_address& lhs, const ip_address& rhs) {
    if (lhs.type != rhs.type) return false;
    return !memcmp(&lhs.ip, &rhs.ip, (lhs.type == ip_type::ipv4) ? sizeof(rhs.ip.v4) : sizeof(rhs.ip.v6));
}
static bool operator<(const ip_address& A, const ip_address& B) {
    // 类型不等，ipv4 < ipv6
    if (A.type != B.type) {
        if (A.type == ip_type::ipv4) {
            return true;
        } else {
            return false;
        }
    }

    if (A.type == ip_type::ipv4) {
        // 转主机字节序对比
        return ntohl(A.ip.v4_addr) < ntohl(B.ip.v4_addr);
    }

    // ipv6，从头部开始对比
    for (auto i = sizeof(A.ip.uv6) - 1; i >= 0; --i) {
        if (A.ip.uv6[i] < B.ip.uv6[i]) {
            return true;
        }
    }
    return false;
}
static bool operator!=(const ip_address& lhs, const ip_address& rhs) { return !(lhs == rhs); }
static bool operator<=(const ip_address& lhs, const ip_address& rhs) { return (lhs < rhs) || (lhs == rhs); }
static bool operator>(const ip_address& lhs, const ip_address& rhs) { return !(lhs <= rhs); }
static bool operator>=(const ip_address& lhs, const ip_address& rhs) { return !(lhs < rhs); }

// 定义 ++ip 的操作， ip++不会调用此操作符
static ip_address& operator++(ip_address& lhs) {
    if (lhs.type == ip_type::ipv6) {
        for (int i = sizeof(lhs.ip.v6) - 1; i >= 0; i--) {
            if (lhs.ip.uv6[i] != 0xff) {
                lhs.ip.uv6[i]++;
                break;
            }
            lhs.ip.uv6[i] = 0;
        }
    } else if (lhs.type == ip_type::ipv4) {
        // 转主机字节序直接++
        lhs.ip.v4_addr = htonl(ntohl(lhs.ip.v4_addr) + 1);
    }

    return lhs;
}
static ip_address& operator--(ip_address& lhs) {
    if (lhs.type == ip_type::ipv6) {
        for (int i = sizeof(lhs.ip.v6) - 1; i >= 0; i--) {
            if (lhs.ip.uv6[i] != 0x00) {
                lhs.ip.uv6[i]--;
                break;
            }
            lhs.ip.uv6[i] = 0xff;
        }
    } else if (lhs.type == ip_type::ipv4) {
        // 转主机字节序直接--
        lhs.ip.v4_addr = htonl(ntohl(lhs.ip.v4_addr) - 1);
    }

    return lhs;
}

// ip = ip + 1
static ip_address operator+(ip_address ip, int value) {
    if (value > 0) {
        for (auto i = 0; i < value; ++i) {
            ++ip;
        }
    } else {
        for (auto i = 0; i > value; --i) {
            --ip;
        }
    }
    return ip;
}
static ip_address operator-(ip_address ip, int value) { return ip + (-value); }

// ip += 1
static ip_address operator+=(ip_address& ip, int value) {
    if (value > 0) {
        for (auto i = 0; i < value; ++i) {
            ++ip;
        }
    } else {
        for (auto i = 0; i > value; --i) {
            --ip;
        }
    }
    return ip;
}
static ip_address operator-=(ip_address& ip, int value) {
    ip += (-value);
    return ip;
}

// 定义 ip++ 的行为
static ip_address operator++(ip_address& ip, int) {
    auto tmp = ip;
    ++ip;
    return tmp;
}
static ip_address operator--(ip_address& ip, int) {
    auto tmp = ip;
    --ip;
    return tmp;
}

// std::cout << ip 和 std::cout << ip << std::endl
static std::ostream& operator<<(std::ostream& out, const ip_address& ip) {
    if (ip.type == ip_type::ipv4) {
        out << (std::to_string(ip.ip.uv4[0]) + "." + std::to_string(ip.ip.uv4[1]) + "." + std::to_string(ip.ip.uv4[2]) +
                "." + std::to_string(ip.ip.uv4[3]));
        return out;
    }
    std::stringstream ss;
    ss << std::hex;
    for (auto i = 0; i < sizeof(ip.ip.uv6); i += 2) {
        if (i > 0) {
            ss << ":";
        }
        ss << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(ip.ip.uv6[i]);
        ss << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(ip.ip.uv6[i + 1]);
    }
    out << ss.str();
    return out;
}

// ip << 0xc0a80001
static void operator<<(ip_address& ip, uint32_t ipv4) { ip = ipv4; }

#endif  // __CLION_CPPSTUDY_IPADDRESS_HPP__
