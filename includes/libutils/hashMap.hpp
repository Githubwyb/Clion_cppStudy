/**
 * @file hashMap.hpp
 * @author wangyubo
 * @brief 自写hashmap，根据第三方库修改，指定内存地址建立hashmap
 * @version 0.1
 * @date 2020-12-17
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef LIBUTILS_HASHMAP_HPP
#define LIBUTILS_HASHMAP_HPP

#include <stddef.h>
#include <string.h>

#include "types.hpp"

// #define DEBUG
#ifdef DEBUG
#include <stdio.h>

#define LOG(fmt, ...) \
    printf("[%d %s] " fmt "\r\n", __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define LOG(fmt, ...)
#endif

namespace libutils {

template <class _Key, class _Tp, class const_key = const _Key,
          class const_value = const _Tp,
          class func_hash = unsigned int (*)(const_key),
          class func_cmp = bool (*)(const_key, const_key)>
class hashMap {
   public:
    struct item {
        bool isUsed;
        unsigned int nextPos;
        _Key key;
        _Tp value;
    };

    /**
     * @brief Construct a new hash Map object
     *
     * @param pData void * 申请的内存地址
     * @param size int 内存地址大小
     * @param pCmpFunc func_cmp  （可选）比较函数指针，当前支持int和char *对比
     * @param pHashFunc func_hash  （可选）hash算法指针，内置支持int和char *
     */
    hashMap(void *pData, int size, func_cmp pCmpFunc = NULL,
            func_hash pHashFunc = NULL)
        : m_pData((item *)pData),
          m_size(size),
          m_hashFunc(pHashFunc),
          m_cmpFunc(pCmpFunc) {
        m_bucketCount = m_size / sizeof(item);
        // 缩小一倍，前一半是hash，后一半是链表
        m_maxDataCount = (m_bucketCount + 1) >> 1;

        // 把所有内存块清理一下，由于类还没初始化，不调用内部方法
        for (int i = 0; i < m_bucketCount; i++) {
            m_pData[i].isUsed = false;
            // map肯定有首地址，所以next不可能链到首地址，这里初始化为0
            m_pData[i].nextPos = 0;
        }
    }

    /**
     * @brief 添加一个值到hashmap中
     *
     * @param key _Key 键
     * @param value _Tp 值
     * @return int 错误码
     */
    int put(const_key key, const_value value) {
        unsigned int index =
            (m_hashFunc != NULL ? m_hashFunc(key) : hash(key)) % m_maxDataCount;
        item *pData = m_pData + index;

        // 内存块被用了且key不相等，说明是前一个块，走链表查找，找下一个块；
        // 内存块没被用或者key相等，说明要放到这个块里面
        while (pData->isUsed && (m_cmpFunc != NULL ? !m_cmpFunc(key, pData->key)
                                                   : !cmp(key, pData->key))) {
            if (pData->nextPos != 0) {
                // 不为空，链表查找
                pData = m_pData + pData->nextPos;
            }
            // nextPos是空，说明没有下一个块，需要查找到可用的内存块
            index = m_maxDataCount;
            // 找链表段的第一个没有被用到的块
            for (; index < m_bucketCount && m_pData[index].isUsed; ++index)
                ;
            // 遍历到尾部，说明没有空间了，返回错误
            if (index == m_bucketCount) {
                return OUT_OF_RANGE;
            }

            pData->nextPos = index;
            pData = m_pData + index;
            break;
        }

        if (!pData->isUsed) {
            // 返回了一个内存块没有被用，需要把key赋值进去；
            // 返回被用的内存块，肯定是key相等，不用赋值key，省性能
            assigned(pData->key, key);
            pData->isUsed = true;
        }
        assigned(pData->value, value);
        return SUCCESS;
    }

    /**
     * @brief 根据key获取值
     *
     * @param key 值
     * @return _Tp* 值指针
     */
    _Tp *get(const_key key) {
        item *pData = getPoint(key);
        if (pData == NULL || !pData->isUsed) {
            return NULL;
        }
        return &(pData->value);
    }

    /**
     * @brief
     *
     * @param key
     * @return item*
     */
    item *find(const_key key) {
        item *pData = getPoint(key);
        if (pData == NULL || !pData->isUsed) {
            return NULL;
        }
        return pData;
    }

    void clear() {
        for (int i = 0; i < m_bucketCount; i++) {
            m_pData[i].isUsed = false;
            // map肯定有首地址，所以next不可能链到首地址，这里初始化为0
            m_pData[i].nextPos = 0;
        }
    }

    void setCmpFunc(func_cmp pFunc) { m_cmpFunc = pFunc; }

    void setHashFunc(func_hash pFunc) { m_hashFunc = pFunc; }

    unsigned int getBucketCount() { return m_bucketCount; }
    unsigned int getMaxDataCount() { return m_maxDataCount; }

   private:
    item *m_pData;                // 数据首地址
    unsigned int m_size;          // 数据总大小
    unsigned int m_bucketCount;   // 桶大小
    unsigned int m_maxDataCount;  // 最多存放数据个数
    func_hash m_hashFunc;         // hash函数
    func_cmp m_cmpFunc;           // 比较函数
    item *getPoint(const_key key) {
        unsigned int index =
            (m_hashFunc != NULL ? m_hashFunc(key) : hash(key)) % m_maxDataCount;
        item *pData = m_pData + index;
        // 内存块被用了且key不相等，说明是前一个块，走链表查找，找下一个块
        while (pData->isUsed && (m_cmpFunc != NULL ? !m_cmpFunc(key, pData->key)
                                                   : !cmp(key, pData->key))) {
            if (pData->nextPos == 0) {
                // 下一个块为空，直接返回空指针
                return NULL;
            }

            // 链表下一个块
            pData = m_pData + pData->nextPos;
        }
        return pData;
    }
    void assigned(int &target, const int &value) { target = value; }
    void assigned(char *target, const char *value) { strcpy(target, value); }
    template <class T>
    void assigned(T &target, const T &value) {
        memcpy(&target, &value, sizeof(T));
    }

    bool cmp(const int &target, const int &value) { return target == value; }
    bool cmp(const char *target, const char *value) {
        return strcmp(target, value) == 0;
    }
    template <class T>
    bool cmp(const T &target, const T &value) {
        return memcmp(&target, &value, sizeof(T)) == 0;
    }

    unsigned int hash(const char *key) {
        const unsigned char *str = (const unsigned char *)key;
        unsigned int hashValue = 0;

        for (int i = 0; str[i] != '\0'; i++) {
            hashValue = hashValue * 37 + str[i];
        }

        return hashValue;
    }

    template <class T>
    unsigned int hash(const T &key) {
        const unsigned char *str = (const unsigned char *)&key;
        unsigned int hashValue = 0;

        for (int i = 0; i < sizeof(T); i++) {
            hashValue = hashValue * 37 + str[i];
        }

        return hashValue;
    }
};  // namespace libutils
}  // namespace libutils

#endif /* LIBUTILS_HASHMAP_HPP */

