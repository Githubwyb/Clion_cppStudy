/**
 * @file mylib.cpp
 * @brief 库函数
 * @author wangyubo
 * @version v2.0.0
 * @date 2020-12-16
 */

#include <openssl/md5.h>
#include <stdio.h>

#include "libcommon.hpp"
#include "log.hpp"

EXPORT_API
void init_mylib(void *pFunc) {
    LOG_INFO("This is origin init func, %p", pFunc);
    MD5_CTX ctx;
    unsigned char outmd[16] = {0};
    int i = 0;

    MD5_Init(&ctx);
    MD5_Update(&ctx, "hel", 3);
    MD5_Update(&ctx, "lo\n", 3);
    MD5_Final(outmd, &ctx);
    char md5Value[33] = {0};
    for (i = 0; i < 16; i < i++) {
        sprintf(md5Value + i * 2, "%02X", outmd[i]);
    }
    LOG_INFO("%s", md5Value);
}
