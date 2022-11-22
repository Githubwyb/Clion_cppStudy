/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description log file
 */

#ifndef CLION_CPPSTUDY_LOG_HPP
#define CLION_CPPSTUDY_LOG_HPP

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define NORMALIZE_LOG_ENABLE

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
        log_print("%02x  ", i / 16);
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

#ifdef __cplusplus
#include <iomanip>
#include <sstream>
static const std::string getHexDumpStr(const void *data, uint32_t data_len) {
    static const int firstColLen = 5;
    static const int hexColLen = 3;
    static const int asciiColLen = 1;
    static const int middleColLen = 4;

    auto pData = static_cast<const uint8_t *>(data);
    std::stringstream ss;

    // 首行
    ss << std::setw(firstColLen) << ""
       << " ";
    ss << std::uppercase << std::hex;
    ss << std::left;
    for (int i = 0; i < 16; i++) {
        ss << std::setw(hexColLen) << i;
    }
    ss << std::right;
    ss << std::setw(middleColLen) << "";
    for (int i = 0; i < 16; i++) {
        ss << std::setw(asciiColLen) << i;
    }
    ss << std::nouppercase;
    ss << std::endl;

    // 数据段
    for (uint32_t i = 0; i < data_len; i += 16) {
        // 前导地址
        ss << std::hex << std::setfill('0');
        ss << std::setw(firstColLen) << i << ":";

        // hex段
        uint32_t j = 0;
        for (j = i; j < i + 16 && j < data_len; j++) {
            ss << std::setw(2) << static_cast<uint16_t>(pData[j]) << " ";
        }
        // 不足补空格
        ss << std::setfill(' ');
        for (; j < i + 16; j++) {
            ss << std::setw(hexColLen) << "";
        }

        // 分隔列
        ss << std::setw(middleColLen) << "";

        // ascii段
        for (j = i; j < i + 16 && j < data_len; j++) {
            if (pData[j] < 32 || pData[j] >= 127) {
                ss << ".";
            } else {
                ss << static_cast<char>(pData[j]);
            }
        }
        ss << std::endl;
    }

    return ss.str();
}
#endif  // __cplusplus

/**
 * 以二进制显示数据
 * @param data 数据首地址
 * @param length 数据长度
 */
static void log_binary(const void *data, int length) {
#define MAX_PRINTED_BIN_LENGTH (32)
    const unsigned char *point = NULL;
    char out_str[128] = {0};
    int index = 0;
    int i = 0;

    point = (const unsigned char *)data;

    for (i = 0; i < length && i < MAX_PRINTED_BIN_LENGTH; i++) {
        snprintf(out_str + index, 4, "%02x ", point[i]);
        index += 3;
    }

    if (length > MAX_PRINTED_BIN_LENGTH) {
        snprintf(out_str + index, 7, "......");
        index += 6;
    }
    snprintf(out_str + index, 3, "\r\n");

    log_print("%s", out_str);
}

#define LOG_HEX(data, length) log_hex(data, length)
#define LOG_BIN(data, length) log_binary(data, length)

enum LOG_LEVEL { TRACE_LEVEL = 0, DBG_LEVEL = 1, INFO_LEVEL = 2, WARN_LEVEL = 3, ERR_LEVEL = 4 };

#ifdef USE_SPDLOG

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

// 引入第三方库 spdlog
#include "spdlog/spdlog.h"

enum level_enum {
    trace = SPDLOG_LEVEL_TRACE,
    debug = SPDLOG_LEVEL_DEBUG,
    info = SPDLOG_LEVEL_INFO,
    warn = SPDLOG_LEVEL_WARN,
    err = SPDLOG_LEVEL_ERROR,
    critical = SPDLOG_LEVEL_CRITICAL,
    off = SPDLOG_LEVEL_OFF,
};

#ifdef NORMALIZE_LOG_ENABLE

#define NORMALIZE_LOG_API(level, ...) SPDLOG_LOGGER_CALL(spdlog::default_logger_raw(), level, __VA_ARGS__)
#define NORMALIZE_LOG_WITHCTX_API

#include "normalize_log.hpp"

#define LOGD(what) NORMALIZE_LOG_1(spdlog::level::debug, what);
#define LOGI(what) NORMALIZE_LOG_1(spdlog::level::info, what);
#define LOGW(what, reason, will) NORMALIZE_LOG_3(spdlog::level::warn, what, reason, will);
#define LOGE(what, reason, will) NORMALIZE_LOG_3(spdlog::level::err, what, reason, will);

#endif

#define LOG_DEBUG SPDLOG_DEBUG
#define LOG_INFO SPDLOG_INFO
#define LOG_WARN SPDLOG_WARN
#define LOG_ERROR SPDLOG_ERROR
#define LOG_FATAL SPDLOG_CRITICAL

#else

#define LOG_PRINT(fmt, ...)        \
    log_print(fmt, ##__VA_ARGS__); \
    log_print("\r\n")

/**
 * 切分文件名，将前面的斜杠去除
 * @param fileName 文件名
 */
static const char *splitFileName(const char *fileName) {
    const char *pChar = fileName;
    pChar = (strrchr(pChar, '/') ? strrchr(pChar, '/') + 1 : (strrchr(pChar, '\\') ? strrchr(pChar, '\\') + 1 : pChar));
    return pChar;
}

/**
 * 打印当前时间
 */
static void print_currentTime() {
    time_t currentTime = time(NULL);
    tm *currentTm = localtime(&currentTime);
    log_print("%4d-%02d-%02d %02d:%02d:%02d", currentTm->tm_year + 1900, currentTm->tm_mon + 1, currentTm->tm_mday,
              currentTm->tm_hour, currentTm->tm_min, currentTm->tm_sec);
}

#define FileName(str) splitFileName(str)

#ifdef NORMALIZE_LOG_ENABLE

#define NORMALIZE_LOG_API(level, fmt, ...) log_print(fmt, ##__VA_ARGS__)
#define NORMALIZE_LOG_WITHCTX_API

#include "normalize_log.hpp"

#define LOGD(what)                                                            \
    print_currentTime();                                                      \
    log_print(" [D][%s:%d %s] ", FileName(__FILE__), __LINE__, __FUNCTION__); \
    NORMALIZE_LOG_1(INFO_LEVEL, what);                                        \
    log_print("\r\n")
#define LOGI(what)                                                            \
    print_currentTime();                                                      \
    log_print(" [I][%s:%d %s] ", FileName(__FILE__), __LINE__, __FUNCTION__); \
    NORMALIZE_LOG_1(INFO_LEVEL, what);                                        \
    log_print("\r\n")
#define LOGW(what, reason, will)                                              \
    print_currentTime();                                                      \
    log_print(" [W][%s:%d %s] ", FileName(__FILE__), __LINE__, __FUNCTION__); \
    NORMALIZE_LOG_3(INFO_LEVEL, what, reason, will);                          \
    log_print("\r\n")
#define LOGE(what, reason, will)                                              \
    print_currentTime();                                                      \
    log_print(" [E][%s:%d %s] ", FileName(__FILE__), __LINE__, __FUNCTION__); \
    NORMALIZE_LOG_3(INFO_LEVEL, what, reason, will);                          \
    log_print("\r\n")

#endif

#define DEBUG_LOG

#ifdef DEBUG_LOG

#define LOG_DEBUG(fmt, ...)                                                                      \
    print_currentTime();                                                                         \
    log_print(" [D][%s:%d %s] " fmt, FileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    log_print("\r\n")
#define LOG_INFO(fmt, ...)                                                                       \
    print_currentTime();                                                                         \
    log_print(" [I][%s:%d %s] " fmt, FileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    log_print("\r\n")
#define LOG_WARN(fmt, ...)                                                                       \
    print_currentTime();                                                                         \
    log_print(" [W][%s:%d %s] " fmt, FileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    log_print("\r\n")
#define LOG_ERROR(fmt, ...)                                                                      \
    print_currentTime();                                                                         \
    log_print(" [E][%s:%d %s] " fmt, FileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    log_print("\r\n")
#define LOG_FATAL(fmt, ...)                                                                      \
    print_currentTime();                                                                         \
    log_print(" [F][%s:%d %s] " fmt, FileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    log_print("\r\n")

#else

#define LOG_PRINT(fmt, ...)
#define LOG_DEBUG(fmt, ...)
#define LOG_INFO(fmt, ...)                                                                       \
    print_currentTime();                                                                         \
    log_print(" [I][%s:%d %s] " fmt, FileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    log_print("\r\n")
#define LOG_WARN(fmt, ...)                                                                       \
    print_currentTime();                                                                         \
    log_print(" [W][%s:%d %s] " fmt, FileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    log_print("\r\n")
#define LOG_ERROR(fmt, ...)                                                                      \
    print_currentTime();                                                                         \
    log_print(" [E][%s:%d %s] " fmt, FileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    log_print("\r\n")
#define LOG_FATAL(fmt, ...)                                                                      \
    print_currentTime();                                                                         \
    log_print(" [F][%s:%d %s] " fmt, FileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    log_print("\r\n")

#endif /* DEBUG_LOG */

#endif

#endif  // CLION_CPPSTUDY_LOG_HPP
