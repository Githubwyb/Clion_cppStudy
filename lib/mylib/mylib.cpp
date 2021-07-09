/**
* @file mylib.cpp
* @brief 库函数
* @author wangyubo
* @version v2.0.0
* @date 2020-12-16
*/

#include <stdio.h>

#include "libcommon.hpp"
#include "log.hpp"

EXPORT_API
void init_mylib(void *pFunc) {
    LOG_INFO("This is origin init func, %p", pFunc);
}

