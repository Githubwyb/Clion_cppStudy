/**
* @file mylib.cpp
* @brief 库函数
* @author wangyubo
* @version v2.0.0
* @date 2020-12-16
*/

#include <stdio.h>

#include "localFunc.hpp"
#include "libcommon.hpp"
#include "log.hpp"

extern "C" MYLIB_API
void init_mylib(void *pFunc) {
    testFunc(5);
    LOG_INFO("This is origin init func, %p", pFunc);
}

