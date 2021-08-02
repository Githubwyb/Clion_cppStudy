/**
 * @file shmManager.hpp
 * @author wangyubo
 * @brief 共享内存处理类
 * @version 0.1
 * @date 2020-12-23
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef SHM_MANAGER_HPP
#define SHM_MANAGER_HPP

#define MAX_SHM_FILENAME_LEN 255
#define FTOK_PROJECT_ID 0x01

#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#include <string>

#include "log.hpp"

class shmManager {
   public:
    shmManager(std::string fileName)
        : m_ftokFileName(fileName), m_key(-1), m_addr(NULL), m_shmid(-1) {}

    /**
     * @brief 创建或者连接共享内存
     *
     * @param size 共享内存大小
     * @return void * 大小不对，返回空；否则返回地址
     */
    void *openShm(size_t size) {
        size_t shmSize = getShmSize();
        // 确保ftok没问题
        if (m_key == -1) {
            LOG_ERROR("ftok failed, %s", strerror(errno));
            return NULL;
        }
        LOG_DEBUG("size %d", shmSize);
        // 和外部传入size不相等
        if (shmSize > 0 && shmSize < size) {
            return NULL;
        }
        // 已经连接成功
        if (shmSize == 0) {
            // 创建共享内存
            m_shmid = shmget(m_key, size, IPC_CREAT | 0666);
            if (m_shmid == -1) {
                // 创建失败
                return NULL;
            }
        }

        return attachShm(true);
    }

    /**
     * @brief 连接共享内存
     *
     * @param writable 是否可写
     * @return void * 共享内存地址
     */
    void *connectShm(bool writable = false) {
        // 获取size，size为0，返回空值
        if (getShmSize() == 0) {
            return NULL;
        }

        return attachShm(writable);
    }

    /**
     * @brief 获取共享内存大小
     *
     * @return size_t 大小，0代表没有共享内存
     */
    size_t getShmSize() {
        if (getFtokKey() == -1) {
            return 0;
        }
        if (m_shmid == -1) {
            // 尝试连接共享内存
            m_shmid = shmget(m_key, 0, 0666);
            if (m_shmid == -1) {
                // 不存在返回0
                return 0;
            }
        }

        // 获取共享内存大小
        struct shmid_ds buf;
        int ret = shmctl(m_shmid, IPC_STAT, &buf);
        if (ret == -1) {
            return 0;
        }

        return buf.shm_segsz;
    }

    /**
     * @brief 获取共享内存地址
     *
     */
    void *getShmAddr() { return m_addr; }

    /**
     * @brief 删除共享内存
     *
     */
    void removeShm() {
        unattachShm();
        if (m_shmid != -1) {
            shmctl(m_shmid, IPC_RMID, NULL);
        }
        m_shmid = -1;
        m_addr = NULL;
    }

    /**
     * @brief 断开共享内存连接
     *
     */
    void unattachShm() {
        if (m_addr != NULL) {
            shmdt(m_addr);
        }
        m_addr = NULL;
    }

   private:
    /**
     * @brief 获取ftokKey
     *
     * @return key_t ftokKey
     */
    key_t getFtokKey() {
        if (m_key != -1) {
            return m_key;
        }
        // 防止文件没有被创建
        if (access(m_ftokFileName.c_str(), F_OK) != 0) {
            int fd = open(m_ftokFileName.c_str(), O_CREAT | O_RDWR, 0666);
            if (fd == -1) {
                return -1;
            } else {
                close(fd);
                (void)chmod(m_ftokFileName.c_str(), 0666);
            }
        }
        m_key = ftok(m_ftokFileName.c_str(), FTOK_PROJECT_ID);
        return m_key;
    }

    /**
     * @brief 连接共享内存
     *
     * @param writable 是否可写
     * @return void * 共享内存地址
     */
    void *attachShm(bool writable) {
        // 已经连接上了，这里直接返回
        if (m_addr) {
            return m_addr;
        }
        assert(m_key != -1 && m_shmid != -1);
        m_addr = shmat(m_shmid, NULL, (writable ? 0 : SHM_RDONLY));
        if (m_addr == NULL) {
            // DBG(shmat);
            return NULL;
        }

        return m_addr;
    }

    std::string m_ftokFileName;  // 用于获取ftokId
    key_t m_key;                 // ftokKey
    void *m_addr;                // 内存映射地址
    int m_shmid;                 // 共享内存id
};

#endif  // SHM_MANAGER_HPP

