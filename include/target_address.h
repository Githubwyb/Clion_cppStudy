/**
 * @file ip_address.h
 * @author dailin (dailin1989ok@163.com)
 * @brief
 * @version 1.0
 * @date 2019-03-15
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef __SANGFOR_SDP_TARGET_ADDRESS_H__
#define __SANGFOR_SDP_TARGET_ADDRESS_H__

#include "ip_address.hpp"
#define MAX_DOMAIN_LENGTH 256
/**
 * @biref 目标类型
 */
typedef enum _target_type { TARGET_IP = 0, TARGET_DOMAIN } target_type;

/**
 * @biref 目标地址和端口
 * TCP/IP协议规定，网络数据流均采用大端，即低地址处放数据的高位。如下的内容都是网络字节序
 */
typedef struct _target_address {
    target_type type;  ///< 目标地址类型
    union {
        ip_address ip;                   ///< ip
        char domain[MAX_DOMAIN_LENGTH];  ///< 域名
    } target;                            ///< 目标地址
    uint16_t port;                       ///< 目标端口
} target_address;

#ifdef __cplusplus

#include <string>

/**
 * @brief 比较目标地址是否相同，不比较端口
 *
 * @return true 目标地址类型和目标地址相同
 * @return false 目标地址类型不同，或目标地址不同
 */
static bool hasSameHost(const target_address& lhs, const target_address& rhs) {
    if ((lhs.type != rhs.type)) return false;
    return ((lhs.type == target_type::TARGET_DOMAIN) && (!strcmp(lhs.target.domain, rhs.target.domain))) ||
           ((lhs.type == target_type::TARGET_IP) && (lhs.target.ip == rhs.target.ip));
}

/**
 * @brief 比较目标地址是否相同，比较端口
 *
 * @return true 目标地址类型和目标地址相同
 * @return false 目标地址类型不同，或目标地址不同
 */
static bool operator==(const target_address& lhs, const target_address& rhs) {
    if ((lhs.type != rhs.type) || (lhs.port != rhs.port)) return false;
    return ((lhs.type == target_type::TARGET_DOMAIN) && (!strcmp(lhs.target.domain, rhs.target.domain))) ||
           ((lhs.type == target_type::TARGET_IP) && (lhs.target.ip == rhs.target.ip));
}

namespace targetAddress {
static std::string toString(const target_address& target) {
    std::string addrStr;
    if (target.type == TARGET_IP) {
        addrStr << target.target.ip;
        if (target.target.ip.type == ip_type::ipv4) {
            addrStr += ":" + std::to_string(ntohs(target.port));
        } else {
            addrStr.insert(0, "[");
            addrStr += "]:" + std::to_string(ntohs(target.port));
        }
    } else {
        addrStr = target.target.domain;
        addrStr += ":" + std::to_string(ntohs(target.port));
    }

    return addrStr;
}

static std::string toStringWithoutPort(const target_address& target) {
    std::string addrStr;
    if (target.type == TARGET_IP) {
        addrStr << target.target.ip;
    } else {
        addrStr = target.target.domain;
    }

    return addrStr;
}
}  // namespace targetAddress

namespace std {
static inline string to_string(const target_address& target) { return targetAddress::toString(target); }

static target_address to_target(const ip_address& ip, uint16_t port) {
    target_address target;
    target.type = TARGET_IP;
    target.port = port;
    target.target.ip = ip;
    return target;
}

}  // namespace std

#endif  // __cplusplus

#endif  // __SANGFOR_SDP_TARGET_ADDRESS_H__
