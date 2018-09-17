/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include "log.hpp"
#include <stdio.h>
#include <time.h>
#include <string>
#include <c++/cstdarg>
#include <c++/cstring>

void Log::log_print(const char *fmt, ...) {
    va_list arg;
    char data[1024] = {0};

    va_start(arg, fmt);
    vsnprintf((char *) data, 1024, fmt, arg);
    va_end(arg);

    printf("%s", data);
}

/*
 * The hex log is in the following format:
 *
 *     0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F      0123456789ABCDEF
 * 01  aa 55 01 00 00 00 25 00 38 36 35 30 36 37 30 32     .U....%.86506702
 * 02  30 34 39 30 31 36 38 30 00 00 00 00 00 00 00 00     04901680........
 * 03  00 00 00 00 00 00 00 00 00 00 00 00                 ............
 *
 */
void Log::log_hex(const void *data, int length) {
    int i = 0, j = 0;
    const char *pData = (const char *) data;

    printf("    ");
    for (i = 0; i < 16; i++) {
        printf("%X  ", i);
    }
    printf("    ");
    for (i = 0; i < 16; i++) {
        printf("%X", i);
    }

    printf("\r\n");

    for (i = 0; i < length; i += 16) {
        printf("%02d  ", i / 16 + 1);
        for (j = i; j < i + 16 && j < length; j++) {
            printf("%02x ", pData[j] & 0xff);
        }
        if (j == length && length % 16) {
            for (j = 0; j < (16 - length % 16); j++) {
                printf("   ");
            }
        }
        printf("    ");
        for (j = i; j < i + 16 && j < length; j++) {
            if (pData[j] < 32 || pData[j] >= 127) {
                printf(".");
            } else {
                printf("%c", pData[j] & 0xff);
            }
        }

        printf("\r\n");
    }
}

/*
 * 打印二进制数据接口
 * 当前仅支持打印前32个字节的二进制数据
 */
#define MAX_PRINTED_BIN_LENGTH (32)

void Log::log_binary(const void *data, int length) {
    char buf[MAX_PRINTED_BIN_LENGTH * 3 + 2] = {0};
    const char *point = nullptr;
    int i = 0;

    point = (const char *) data;

    for (i = 0; i < length && i < MAX_PRINTED_BIN_LENGTH; i++) {
        sprintf(buf + i * 3, "%02x", point[i]);
        buf[i * 3 + 2] = ' ';   //because sprintf will add a terminating null character at the end
    }

    if (i >= MAX_PRINTED_BIN_LENGTH) {
        sprintf(buf + i * 3, "%s", "......");
    }

    printf("%s", buf);
    printf("\r\n");
}

void Log::log_header(LOG_LEVEL level) {
    print_currentTime();
    printf("[%lu:%lu][%s]",
           (unsigned long int) getpid(),
           (unsigned long int) pthread_self(),
           getLevelString(level));
}

const char *Log::splitFileName(const std::string &x) {
    const char *pChar = (char *)x.c_str();
    pChar = (strrchr(pChar, '/') ? strrchr(pChar, '/') + 1 : (strrchr(pChar, '\\') ? strrchr(pChar, '\\') + 1 : pChar));
    return pChar;
}

const char *Log::getLevelString(LOG_LEVEL level) {
    switch (level) {
        case LOG_LEVEL_DEBUG:
            return "D";

        case LOG_LEVEL_WARN:
            return "W";

        case LOG_LEVEL_INFO:
            return "I";

        case LOG_LEVEL_ERROR:
            return "E";

        case LOG_LEVEL_FATAL:
            return "F";

        default:
            return "";
    }
}

void Log::print_currentTime() {
    time_t currentTime = time(nullptr);
    tm *currentTm = localtime(&currentTime);
    printf("%4d-%02d-%02d %02d:%02d:%02d",
           currentTm->tm_year + 1900,
           currentTm->tm_mon,
           currentTm->tm_mday,
           currentTm->tm_hour,
           currentTm->tm_min,
           currentTm->tm_sec);
}
