/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description log file
 */

#ifndef CLION_CPPSTUDY_LOG_HPP
#define CLION_CPPSTUDY_LOG_HPP

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

/**
 * 相当于printf的打印
 * @param fmt 格式化的数据
 * @param ... 不定变量
 */
static void log_print(const char *fmt, ...) {
    va_list arg;
    va_start(arg, fmt);
    vprintf(fmt, arg);
    va_end(arg);
}

/**
 * 以网络结构显示数据
 * @param data 数据首地址
 * @param length 数据长度
 */
static void log_hex(const void *data, int length) {
    int i = 0, j = 0;
    const char *pData = (const char *)data;

    log_print("    ");
    for (i = 0; i < 16; i++) {
        log_print("%X  ", i);
    }
    log_print("    ");
    for (i = 0; i < 16; i++) {
        log_print("%X", i);
    }

    log_print("\r\n");

    for (i = 0; i < length; i += 16) {
        log_print("%02d  ", i / 16 + 1);
        for (j = i; j < i + 16 && j < length; j++) {
            log_print("%02x ", pData[j] & 0xff);
        }
        if (j == length && length % 16) {
            for (j = 0; j < (16 - length % 16); j++) {
                log_print("   ");
            }
        }
        log_print("    ");
        for (j = i; j < i + 16 && j < length; j++) {
            if (pData[j] < 32 || pData[j] >= 127) {
                log_print(".");
            } else {
                log_print("%c", pData[j] & 0xff);
            }
        }

        log_print("\r\n");
    }
}

/**
 * 以二进制显示数据
 * @param data 数据首地址
 * @param length 数据长度
 */
static void log_binary(const void *data, int length) {
#define MAX_PRINTED_BIN_LENGTH (32)
    const char *point = nullptr;
    int i = 0;

    point = (const char *)data;

    for (i = 0; i < length && i < MAX_PRINTED_BIN_LENGTH; i++) {
        log_print("%02x ", point[i]);
    }

    if (i >= MAX_PRINTED_BIN_LENGTH) {
        log_print("......");
    }

    log_print("\r\n");
}

#define LOG_HEX(data, length) log_hex(data, length)
#define LOG_BIN(data, length) log_binary(data, length)

#ifdef USE_SPDLOG

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

// 引入第三方库 spdlog
#include "spdlog/spdlog.h"
#define LOG_DEBUG SPDLOG_DEBUG
#define LOG_INFO SPDLOG_INFO
#define LOG_WARN SPDLOG_WARN
#define LOG_ERROR SPDLOG_ERROR
#define LOG_FATAL SPDLOG_CRITICAL

#else

/**
 * 切分文件名，将前面的斜杠去除
 * @param fileName 文件名
 */
static const char *splitFileName(const char *fileName) {
    const char *pChar = fileName;
    pChar = (strrchr(pChar, '/')
                 ? strrchr(pChar, '/') + 1
                 : (strrchr(pChar, '\\') ? strrchr(pChar, '\\') + 1 : pChar));
    return pChar;
}

/**
 * 打印当前时间
 */
static void print_currentTime() {
    time_t currentTime = time(nullptr);
    tm *currentTm = localtime(&currentTime);
    log_print("%4d-%02d-%02d %02d:%02d:%02d", currentTm->tm_year + 1900,
              currentTm->tm_mon + 1, currentTm->tm_mday, currentTm->tm_hour,
              currentTm->tm_min, currentTm->tm_sec);
}

#define FileName(str) splitFileName(str)

#define LOG_PRINT(fmt, ...)        \
    log_print(fmt, ##__VA_ARGS__); \
    log_print("\r\n")
#define LOG_DEBUG(fmt, ...)                                                  \
    print_currentTime();                                                     \
    log_print(" [D][%s:%d %s] " fmt, FileName(__FILE__), __LINE__, __FUNCTION__, \
              ##__VA_ARGS__);                                                \
    log_print("\r\n")
#define LOG_INFO(fmt, ...)                                                   \
    print_currentTime();                                                     \
    log_print(" [I][%s:%d %s] " fmt, FileName(__FILE__), __LINE__, __FUNCTION__, \
              ##__VA_ARGS__);                                                \
    log_print("\r\n")
#define LOG_WARN(fmt, ...)                                                   \
    print_currentTime();                                                     \
    log_print(" [W][%s:%d %s] " fmt, FileName(__FILE__), __LINE__, __FUNCTION__, \
              ##__VA_ARGS__);                                                \
    log_print("\r\n")
#define LOG_ERROR(fmt, ...)                                                  \
    print_currentTime();                                                     \
    log_print(" [E][%s:%d %s] " fmt, FileName(__FILE__), __LINE__, __FUNCTION__, \
              ##__VA_ARGS__);                                                \
    log_print("\r\n")
#define LOG_FATAL(fmt, ...)                                                  \
    print_currentTime();                                                     \
    log_print(" [F][%s:%d %s] " fmt, FileName(__FILE__), __LINE__, __FUNCTION__, \
              ##__VA_ARGS__);                                                \
    log_print("\r\n")

#endif

#endif  // CLION_CPPSTUDY_LOG_HPP
