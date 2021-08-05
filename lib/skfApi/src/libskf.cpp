/**
 * @file libskf.cpp
 * @brief 库函数
 * @author wangyubo
 * @version v2.0.0
 * @date 2020-12-16
 */

#include "libskf/libskf.h"

// #include <openssl/engine.h>
// #include <openssl/err.h>
// #include <openssl/md5.h>
#include <assert.h>
#include <stdio.h>

#include "internal/platform.h"
#include "log.hpp"

typedef struct Usbkey {
    std::string certPwd;
    DEVHANDLE devHandle;
    HAPPLICATION appHandle;
    HCONTAINER conHandle;

    Usbkey() : certPwd(""), devHandle(NULL), appHandle(NULL), conHandle(NULL) {}
} Usbkey;

// 给openssl调用的api接口
static SKFApi s_skfApi = SKFApi();
// 动态库储存的usbkey的handle
static Usbkey s_usbKey;

static void closeUsbkey(const SKFApi &api, Usbkey &usbKey) {
    if (usbKey.conHandle != NULL) {
        api.closeContainer(usbKey.conHandle);
        usbKey.conHandle = NULL;
    }

    if (usbKey.appHandle != NULL) {
        api.closeApp(usbKey.appHandle);
        usbKey.appHandle = NULL;
    }

    if (usbKey.devHandle != NULL) {
        api.disConnectDev(usbKey.devHandle);
        usbKey.devHandle = NULL;
    }
}

/**
 * @brief 打开一个usbkey
 *
 * @param devName [IN]设备名称
 * @param appName [IN]app名称
 * @param conName [IN]container名称
 * @param certPwd [IN]pin，为NULL将不校验pin
 * @param api [IN]初始化好的api
 * @param usbKey [OUT]各种打开后的句柄
 * @return int SAR错误码，0成功，其他失败
 */
static int openUsbkey(LPSTR devName, LPSTR appName, LPSTR conName,
                      const char *certPwd, const SKFApi &api, Usbkey &usbKey) {
    const char *operation = "openUsbkey";
    LOG_DEBUG("[%s] devName %s, appName %s, conName %s", operation, devName,
              appName, conName);
    ULONG rv;
    // 关掉上一次的usbkey
    closeUsbkey(api, usbKey);
    do {
        rv = api.connectDev(devName, usbKey.devHandle);
        if (rv != SAR_OK) {
            LOG_ERROR("connectDev failed, err 0x%x, dev %s", rv, devName);
            break;
        }
        rv = api.openApp(appName, usbKey.devHandle, usbKey.appHandle);
        if (rv != SAR_OK) {
            LOG_ERROR("openApp failed, err 0x%x, app %s", rv, appName);
            break;
        }

        // 某些情况可以不需要输入pin就可以执行
        if (certPwd != NULL) {
            rv = api.verifyPin(usbKey.appHandle, certPwd);
            if (rv != SAR_OK) {
                LOG_ERROR("verify failed, err 0x%x", rv);
                break;
            }
            usbKey.certPwd = certPwd;
        }
        rv = api.openContainer(conName, usbKey.appHandle, usbKey.conHandle);
        if (rv != SAR_OK) {
            LOG_ERROR("openContainer failed, err 0x%x, conName %s", rv,
                      conName);
            break;
        }
        return SAR_OK;
    } while (false);

    closeUsbkey(api, usbKey);
    return rv;
}

int LibSkf::initApi(const char *libPath) {
    LOG_DEBUG("initApi, libPath %s", libPath);
    closeUsbkey(s_skfApi, s_usbKey);
    return s_skfApi.initApi(libPath);
}

int LibSkf::initEngine(LPSTR devName, LPSTR appName, LPSTR conName,
                       const char *certPwd) {
    LOG_DEBUG("initEngine, devName %s, appName %s, conName %s", devName,
              appName, conName);
    return openUsbkey(devName, appName, conName, certPwd, s_skfApi, s_usbKey);
}

void LibSkf::uninitEngine() {
    LOG_DEBUG("uninitEngine");
    closeUsbkey(s_skfApi, s_usbKey);
}

int LibSkfUtils::enumAllInfo(const std::vector<std::string> &libPath,
                             std::vector<UkeyInfo> &ukeyInfos) {
    SKFApi skfApi = SKFApi();
    ukeyInfos.clear();
    for (const auto &lib : libPath) {
        UkeyInfo ukey;
        ukey.strPath = lib;

        skfApi.initApi(lib.c_str());
        skfApi.enumAllInfo(ukey.vcDevs);

        if (ukey.vcDevs.size() > 0) {
            ukeyInfos.emplace_back(ukey);
        }
    }
    return SAR_OK;
}

int LibSkfUtils::checkCertByCAIssuer(const std::string &libPath, LPSTR devName,
                                     LPSTR appName, LPSTR conName,
                                     const char *certPwd,
                                     const std::string &issuer) {
    const char *operation = "checkCertByCAIssuer";
    LOG_DEBUG("[%s] devName %s, appName %s, conName %s", operation, devName,
              appName, conName);
    ULONG rv;
    Usbkey usbkey;
    SKFApi skfApi = SKFApi();
    do {
        rv = skfApi.initApi(libPath.c_str());
        if (rv != SAR_OK) {
            LOG_ERROR("[%s] initapi failed", operation);
            break;
        }

        rv = openUsbkey(devName, appName, conName, certPwd, skfApi, usbkey);
        if (rv != SAR_OK) {
            LOG_ERROR("[%s] openUsbkey failed", operation);
            break;
        }

        rv = skfApi.checkCertByCAIssuer(usbkey.conHandle, issuer.c_str());
        LOG_DEBUG("[%s] check ret %d", operation, rv);
    } while (false);
    closeUsbkey(skfApi, usbkey);
    return rv;
}

int isEngineInit() {
    // const char *operation = "isEngineInit";
    // LOG_DEBUG("%s", operation);
    return s_usbKey.devHandle != NULL && s_usbKey.appHandle != NULL &&
           s_usbKey.conHandle != NULL;
}

//对一个摘要数据做签名
int sm2DoSign(const unsigned char *dgst, int dgst_len, unsigned char r[32],
              unsigned char s[32]) {
    const char *operation = "sm2DoSign";
    LOG_DEBUG("%s", operation);
    if (!isEngineInit()) {
        LOG_ERROR("sm2DoSign failed, engine not init");
        return 0;
    }
    ULONG ulDataLen;
    ULONG rv = SAR_FAIL;
    ULONG ulRetryCount = 0;
    ECCSIGNATUREBLOB signature;

    ulDataLen = dgst_len;

    do {
        // ulDataLen长度为32字节的摘要才是正确的
        rv = s_skfApi.ECCSignData(s_usbKey.conHandle, const_cast<BYTE *>(dgst),
                                  ulDataLen, &signature);
        if (rv != SAR_OK) {
            LOG_ERROR("[sm2_do_sign] SKF_ECCSignData error 0x%x", rv);
            break;
        }
        memcpy(&r[0], &signature.r[32], 32);
        memcpy(&s[0], &signature.s[32], 32);
    } while (false);

    return (rv == SAR_OK);
}

int sm2Verify(const unsigned char *dgst, int dgst_len, unsigned char x[32],
              unsigned char y[32], unsigned char r[32], unsigned char s[32]) {
    const char *operation = "sm2Verify";
    LOG_DEBUG("%s", operation);
    ULONG rv;
    ECCPUBLICKEYBLOB PubKey;
    ECCSIGNATUREBLOB signature;
    ULONG ulDataLen = dgst_len;

    memset(&PubKey, 0x00, sizeof(PubKey));
    memset(&signature, 0x00, sizeof(ECCSIGNATUREBLOB));

    PubKey.BitLen = 256;
    memcpy(&PubKey.XCoordinate[32], &x[0], 32);
    memcpy(&PubKey.YCoordinate[32], &y[0], 32);

    memcpy(&signature.r[32], &r[0], 32);
    memcpy(&signature.s[32], &s[0], 32);

    rv = s_skfApi.ECCVerify(s_usbKey.devHandle, &PubKey, (BYTE *)dgst,
                            ulDataLen, &signature);
    if (rv != SAR_OK) {
        fprintf(stderr, "[sm2_do_verify] SKF_ECCVerify failed %d \n", rv);
        goto __end;
    }

__end:

    return (rv == SAR_OK);
}

// int genRandom(unsigned char *buf, int num) {
//     const char *operation = "genRandom";
//     LOG_DEBUG("%s", operation);
//     if (!isEngineInit()) {
//         LOG_ERROR("genRandom failed, engine not init");
//         return 0;
//     }

//     if (buf == NULL) {
//         return 0;
//     }

//     char *buffer = new char[num];
//     ULONG rv = s_skfApi.GenRandom(s_usbKey.devHandle,
//                                   reinterpret_cast<BYTE *>(buffer), num);
//     if (rv != SAR_OK) {
//         LOG_ERROR("SKF_GenRandom failed, err=0x%x", rv);
//     } else {
//         // num的值一般会小于32位
//         memcpy(buf, buffer, num);
//     }
//     delete[] buffer;
//     LOG_HEX(buf, num);
//     memset(buf, 1, num);
//     // return rv == SAR_OK;
//     return 1;
// }

int getSm2SignPubkey(unsigned char ecc_x[32], unsigned char ecc_y[32]) {
    const char *operation = "getSm2SignPubkey";
    LOG_DEBUG("%s", operation);
    if (!isEngineInit()) {
        LOG_ERROR("%s failed, engine not init", operation);
        return 0;
    }

    ECCPUBLICKEYBLOB PubKey;
    ULONG ulBlobLen = sizeof(ECCPUBLICKEYBLOB);
    ULONG rv;

    rv = s_skfApi.ExportPublicKey(s_usbKey.conHandle, TRUE,
                                  (unsigned char *)&PubKey, &ulBlobLen);
    if (rv != SAR_OK) {
        LOG_ERROR("SKF_ExportPublicKey error %d size %d", rv, ulBlobLen);
    } else {
        memcpy(&ecc_x[0], &PubKey.XCoordinate[32], 32);
        memcpy(&ecc_y[0], &PubKey.YCoordinate[32], 32);
    }

    return (rv == SAR_OK);
}

int exportCertificate(BOOL type, unsigned char *cert, ULONG &len) {
    assert(cert != NULL);
    const char *operation = "exportCertificate";
    LOG_DEBUG("%s", operation);
    if (!isEngineInit()) {
        LOG_ERROR("%s failed, engine not init", operation);
        return 0;
    }
    int rv = s_skfApi.ExportCertificate(s_usbKey.conHandle, type, NULL, &len);
    if (rv != SAR_OK) {
        LOG_ERROR(
            "[epass_load_ssl_client_cert], SKF_ExportCertificate failed, "
            "err=%d",
            rv);
        return 0;
    }
    LOG_DEBUG("certContetBITS: len %d", len);

    rv = s_skfApi.ExportCertificate(s_usbKey.conHandle, type, cert, &len);
    if (rv != SAR_OK) {
        LOG_ERROR(
            "[epass_load_ssl_client_cert], SKF_ExportCertificate failed, "
            "err=%d",
            rv);
        return 0;
    }
    return 1;
}

#define LIB_NAME "ShuttleCsp11_3000GM.dll"
#define LIB_PATH "C:\\Windows\\System32\\" LIB_NAME
int test_init() {
    if (isEngineInit()) {
        return 1;
    }
    LibSkfUtils skfUtils;
    std::vector<UkeyInfo> info;
    skfUtils.enumAllInfo({LIB_PATH}, info);
    LibSkf::initApi(LIB_PATH);
    LibSkf::initEngine(
        reinterpret_cast<LPSTR>(
            const_cast<char *>(info[0].vcDevs[0].devName.c_str())),
        reinterpret_cast<LPSTR>(
            const_cast<char *>(info[0].vcDevs[0].vcApps[0].appName.c_str())),
        reinterpret_cast<LPSTR>(const_cast<char *>(
            info[0].vcDevs[0].vcApps[0].vcCons[0].conName.c_str())),
        "123456");
    return 1;
}
