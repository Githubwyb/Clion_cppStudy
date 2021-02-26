/**
 * @file localFunc.cpp
 * @brief 测试本地函数能否hook
 * @author wangyubo
 * @version v0.1.0
 * @date 2021-02-26
 */

#include "log.hpp"
#include "localFunc.hpp"

void testFunc(int a) {
    LOG_INFO("I'm origin func, a %d", a);
}

