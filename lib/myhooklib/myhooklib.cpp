/**
 * @file myhooklib.cpp
 * @brief hook前一个hook
 * @author wangyubo
 * @version v0.1.0
 * @date 2021-02-26
 */

#include "log.hpp"
#include "localFunc.hpp"

void testFunc(int a) {
    LOG_INFO("hook func, a %d", a);
}
