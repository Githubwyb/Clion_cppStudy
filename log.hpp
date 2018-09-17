//
// Created by 10613 on 09/16/2018.
//

#ifndef PROJECT_FILES_LOG_HPP
#define PROJECT_FILES_LOG_HPP

#include <stdio.h>

#define DEBUG

#ifdef DEBUG

#include <string>
#include <c++/cstring>

__inline const char *splitFileName(std::string x) {
    const char *pChar = x.c_str();
    pChar = (strrchr(pChar, '/') ? strrchr(pChar, '/') + 1 : (strrchr(pChar, '\\') ? strrchr(pChar, '\\') + 1 : pChar));
    return pChar;
}

#define LOG_INFO(fmt, ...)                                                            \
    printf("[%lu:%lu][I][%s:%d %s] " fmt, (unsigned long int)getpid(), (unsigned long int)pthread_self(), splitFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    printf("\r\n")
#define LOG_DEBUG(fmt, ...)                                                           \
    printf("[%lu:%lu][D][%s:%d %s] " fmt, (unsigned long int)getpid(), (unsigned long int)pthread_self(), splitFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    printf("\r\n")
#define LOG_ERROR(fmt, ...)                                                           \
    printf("[%lu:%lu][E][%s:%d %s] " fmt, (unsigned long int)getpid(), (unsigned long int)pthread_self(), splitFileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    printf("\r\n")

#else

#define LOG_INFO(fmt, ...)
#define LOG_DEBUG(fmt, ...)
#define LOG_ERROR(fmt, ...)

#endif

class log {

};

#endif //PROJECT_FILES_LOG_HPP
