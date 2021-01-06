/**
 * @file circleList.hpp
 * @author wangyubo
 * @brief 循环队列实现（无锁队列）
 * @version 0.1
 * @date 2020-12-22
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef LIBUTILS_CIRCLELIST_HPP
#define LIBUTILS_CIRCLELIST_HPP

// #define DEBUG
#ifdef DEBUG
#include <stdio.h>

#define LOG(fmt, ...) \
    printf("[%d %s] " fmt "\r\n", __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define LOG(fmt, ...)
#endif

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

typedef struct sa_access {
    uint8_t type;   // SA_RULE_TYPE_TCP or SA_RULE_TYPE_L3VPN
    uint8_t proto;  // SA_RULE_PROTO_TCP SA_RULE_PROTO_UDP SA_RULE_PROTO_ICMP
                    // SA_RULE_PROTO_OTHER
    uint16_t port;  // Host Order   TCP/UDP/other: [0, 65535]  ICMP: 0
    uint32_t ip;    // Host Order
} sa_access_t;

typedef sa_access_t value_type;

#include <stddef.h>
#include <string.h>
namespace libutils {

typedef bool (*func_cmp)(const value_type &, const value_type &);

struct circleList {
    struct circleList_t {
        unsigned char headPos;
        value_type data[];
    };

    circleList_t *m_pData;    // 数据段地址
    unsigned int m_dataSize;  // 内存总长度
    unsigned int maxLength;   // 数据段总长度
    func_cmp m_cmpFunc;       // 比较函数

    /**
     * @brief 内置比较函数，返回两个元素是否相等
     *
     * @param a const value_type & a
     * @param b const value_type & b
     * @return true
     * @return false
     */
    bool cmp(const value_type &a, const value_type &b) {
        return a.ip == b.ip && a.port == b.port && a.proto == b.proto &&
               a.type == b.type;
    }

    /**
     * @brief 构造函数，初始化一个队列
     *
     * @param pData 内存空间头指针
     * @param dataSize 内存大小
     * @param pFunc 比较函数，不传使用默认的
     */
    circleList(void *pData, unsigned int dataSize, func_cmp pFunc = NULL)
        : m_pData((circleList_t *)pData),
          m_dataSize(dataSize),
          m_cmpFunc(pFunc) {
        maxLength = (m_dataSize - ((unsigned char *)(m_pData->data) -
                                   (unsigned char *)m_pData)) /
                    sizeof(value_type);
    }

    /**
     * @brief 检查value是否存在于缓存中
     *
     * @param value
     * @return unsigned int 查询次数，0为没有查到
     */
    unsigned int checkValueExist(const value_type &value) {
        unsigned char pos = m_pData->headPos;
        // 容错防止头指针飞了
        if (pos > maxLength) pos = 0;

        // 最多遍历maxLength遍，防止死循环
        unsigned int i = 0;
        for (; i < maxLength; ++i) {
            // ip不可能为0，这里用于判断是否为空数据
            if (m_pData->data[pos].ip == 0) {
                return 0;
            }
            // 查到value退出
            if (m_cmpFunc != NULL ? m_cmpFunc(value, m_pData->data[pos])
                                  : cmp(value, m_pData->data[pos])) {
                break;
            }
            pos = (pos == 0 ? maxLength - 1 : pos - 1);
        }

        // 遍历完了，没找到，返回0
        if (i == maxLength) return 0;

        // 返回查找次数
        return i + 1;
    }

    /**
     * @brief 插入一个数据，只用于单进程单线程操作
     *
     * @param value 待插入数据
     */
    void insert(const value_type &value) {
        unsigned char pos = m_pData->headPos;
        // 容错防止头指针飞了
        if (pos > maxLength) pos = 0;
        // ip不可能为0，这里用于判断是否为空数据
        if (pos == 0 && m_pData->data[pos].ip == 0) {
            // 空数据直接拷贝，头指针不修改
            memcpy(m_pData->data, &value, sizeof(value_type));
            return;
        }
        if (m_cmpFunc != NULL ? m_cmpFunc(value, m_pData->data[pos])
                              : cmp(value, m_pData->data[pos])) {
            // 容错，相等说明插入过了，直接返回
            return;
        }
        pos = (pos == maxLength - 1 ? 0 : pos + 1);
        memcpy(m_pData->data + pos, &value, sizeof(value_type));
        m_pData->headPos = pos;
    }

    /**
     * @brief 清理内存
     *
     */
    void clear() { memset(m_pData, 0, m_dataSize); }

    /**
     * @brief 返回有效缓存个数
     *
     * @return unsigned int 缓存个数
     */
    unsigned int size() {
        unsigned char pos = m_pData->headPos;
        // 容错防止头指针飞了
        if (pos > maxLength) pos = 0;

        // 最多遍历maxLength遍，防止死循环
        unsigned int i = 0;
        for (; i < maxLength && m_pData->data[pos].ip != 0; ++i) {
            pos = (pos == 0 ? maxLength - 1 : pos - 1);
        }

        // 返回查找次数
        return i;
    }
};
}  // namespace libutils

#endif  // LIBUTILS_CIRCLELIST_HPP

