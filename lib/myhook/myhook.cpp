/**
 * @file myhook.cpp
 * @author 王钰博18433 (18433@sangfor.com)
 * @brief hook函数
 * @version 0.1
 * @date 2020-12-16
 *
 * @copyright Copyright (c) 2020
 *
 */
#include <dlfcn.h>

#include "libcommon.hpp"
#include "log.hpp"
#include "subhook/subhook.h"
typedef int (*func_type)(void *, char *, int, const char *, void *);

subhook_t foo_hook;

int my_run(void *p, char *buf, int size, const char *pStrIp, void *pV) {
    LOG_INFO(
        "This is hook func, get param p: %p, buf(addr %p), size %d, pStrIp %s, "
        "pV: "
        "%p",
        p, buf, size, pStrIp, pV);

    auto originFunc = (func_type)subhook_get_trampoline(foo_hook);
    LOG_HEX((void *)originFunc, 50);
    int ret = originFunc(p, buf, size, pStrIp, pV);
    LOG_INFO("call origin func, ret %d", ret);
    return ret;
}

typedef void (*func_mylib)(void *);
extern "C" MYLIB_API void init_mylib(void *pFunc) {
    LOG_INFO("Hook init_mylib, %p", pFunc);
    foo_hook = subhook_new(pFunc, (void *)my_run, SUBHOOK_64BIT_OFFSET);
    LOG_INFO("do hook");
    subhook_install(foo_hook);
    LOG_INFO("call origin init, %p", pFunc);
    auto origin_func = (func_mylib)dlsym(RTLD_NEXT, "init_mylib");
    origin_func(pFunc);
}

