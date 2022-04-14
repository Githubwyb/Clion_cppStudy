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

#define NEED_ENGINE(operation, ret)                                            \
    if (!isEngineInit()) {                                                     \
        LOG_ERROR("%s failed, engine not init, init engine first", operation); \
        return ret;                                                            \
    }

// 给openssl调用的api接口
static SKFApi s_skfApi = SKFApi();
// 动态库储存的usbkey的handle
static Usbkey s_usbKey;

int LibSkf::initApi(const char *libPath) {
    if (libPath == NULL) {
        LOG_ERROR("libPath is null");
        return SAR_FAIL;
    }
    LOG_DEBUG("initApi, libPath %s", libPath);
    s_skfApi.closeUsbkey(s_usbKey);
    return s_skfApi.initApi(libPath);
}

int LibSkf::initEngine(LPSTR devName, LPSTR appName, LPSTR conName, const char *certPwd) {
    if (devName == NULL || appName == NULL || conName == NULL || certPwd == NULL) {
        LOG_ERROR("param invalid, please check param");
        return SAR_FAIL;
    }
    LOG_DEBUG("initEngine, devName %s, appName %s, conName %s", devName, appName, conName);
    return s_skfApi.openUsbkey(devName, appName, conName, certPwd, s_usbKey);
}

void LibSkf::uninitEngine() {
    LOG_DEBUG("uninitEngine");
    s_skfApi.closeUsbkey(s_usbKey);
}

int LibSkfUtils::enumAllInfo(const std::vector<std::string> &libPath, std::vector<UkeyInfo> &ukeyInfos) {
    SKFApi skfApi = SKFApi();
    ukeyInfos.clear();

    int iRet = SAR_FAIL;
    for (const auto &lib : libPath) {
        UkeyInfo ukey;
        ukey.strPath = lib;

        if (skfApi.initApi(lib.c_str()) == SAR_OK) {
            // 只要有一个路径init成功，返回SAR_OK
            iRet = SAR_OK;
            skfApi.enumAllInfo(ukey.vcDevs);

            if (ukey.vcDevs.size() > 0) {
                ukeyInfos.emplace_back(ukey);
            }
        }
    }
    return iRet;
}

int LibSkfUtils::checkCertByCAIssuer(const std::string &libPath,
                                     LPSTR              devName,
                                     LPSTR              appName,
                                     LPSTR              conName,
                                     const char        *certPwd,
                                     const std::string &issuer) {
    const char *operation = "checkCertByCAIssuer";
    LOG_DEBUG("[%s] devName %s, appName %s, conName %s", operation, devName, appName, conName);
    if (devName == NULL || appName == NULL || conName == NULL) {
        LOG_ERROR("param invalid, please check param");
        return SAR_FAIL;
    }
    ULONG  rv;
    Usbkey usbkey;
    SKFApi skfApi = SKFApi();
    do {
        rv = skfApi.initApi(libPath.c_str());
        if (rv != SAR_OK) {
            LOG_ERROR("[%s] initapi failed, Check libPath %s", operation, libPath.c_str());
            break;
        }

        rv = skfApi.openUsbkey(devName, appName, conName, certPwd, usbkey);
        if (rv != SAR_OK) {
            LOG_ERROR("[%s] openUsbkey failed", operation);
            break;
        }

        rv = skfApi.checkCertByCAIssuer(usbkey.conHandle, issuer.c_str());
        LOG_DEBUG("[%s] check ret %#x", operation, rv);
    } while (false);
    skfApi.closeUsbkey(usbkey);
    return rv;
}

int isEngineInit() {
    // const char *operation = "isEngineInit";
    // LOG_DEBUG("%s", operation);
    return s_usbKey.devHandle != NULL && s_usbKey.appHandle != NULL && s_usbKey.conHandle != NULL;
}

//对一个摘要数据做签名
int sm2DoSign(const unsigned char *dgst, int dgst_len, unsigned char r[32], unsigned char s[32]) {
    const char *operation = "sm2DoSign";
    LOG_DEBUG("%s", operation);

    NEED_ENGINE(operation, 0)

    ULONG            ulDataLen;
    ULONG            rv           = SAR_FAIL;
    ULONG            ulRetryCount = 0;
    ECCSIGNATUREBLOB signature;

    ulDataLen = dgst_len;

    do {
        // ulDataLen长度为32字节的摘要才是正确的
        rv = s_skfApi.ECCSignData(s_usbKey.conHandle, const_cast<BYTE *>(dgst), ulDataLen, &signature);
        if (rv != SAR_OK) {
            LOG_ERROR("[sm2_do_sign] SKF_ECCSignData error %#x", rv);
            break;
        }
        memcpy(&r[0], &signature.r[32], 32);
        memcpy(&s[0], &signature.s[32], 32);
    } while (false);

    return (rv == SAR_OK);
}

int sm2Verify(const unsigned char *dgst,
              int                  dgst_len,
              unsigned char        x[32],
              unsigned char        y[32],
              unsigned char        r[32],
              unsigned char        s[32]) {
    const char *operation = "sm2Verify";
    LOG_DEBUG("%s", operation);

    NEED_ENGINE(operation, 0)

    ULONG            rv;
    ECCPUBLICKEYBLOB PubKey;
    ECCSIGNATUREBLOB signature;
    ULONG            ulDataLen = dgst_len;

    memset(&PubKey, 0x00, sizeof(PubKey));
    memset(&signature, 0x00, sizeof(ECCSIGNATUREBLOB));

    PubKey.BitLen = 256;
    memcpy(&PubKey.XCoordinate[32], &x[0], 32);
    memcpy(&PubKey.YCoordinate[32], &y[0], 32);

    memcpy(&signature.r[32], &r[0], 32);
    memcpy(&signature.s[32], &s[0], 32);

    rv = s_skfApi.ECCVerify(s_usbKey.devHandle, &PubKey, (BYTE *)dgst, ulDataLen, &signature);
    if (rv != SAR_OK) {
        LOG_ERROR("[sm2_do_verify] SKF_ECCVerify failed 0x%x", rv);
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

int rsaDoSign(const unsigned char *data, unsigned int data_len, unsigned char *sig, unsigned int sig_len) {
    int         ret       = -1;
    const char *operation = "rsa sign data";
    LOG_DEBUG("%s", operation);

    NEED_ENGINE(operation, -1)

    do {
        // 先获取长度，检查sigLen是否符合大小
        ULONG buf_len;
        ULONG rv = s_skfApi.RSASignData(s_usbKey.conHandle, const_cast<BYTE *>(data), data_len, NULL, &buf_len);
        if (rv != SAR_OK) {
            LOG_ERROR("%s failed, RSASignData error %#x", operation, rv);
            break;
        }
        // 长度不够直接返回错误
        if (sig_len < buf_len) {
            LOG_ERROR("%s failed, need len %d, but input %d", operation, buf_len, sig_len);
            break;
        }

        rv = s_skfApi.RSASignData(s_usbKey.conHandle, const_cast<BYTE *>(data), data_len, sig, &buf_len);
        if (rv != SAR_OK) {
            LOG_ERROR("%s failed, RSASignData error %#x", operation, rv);
            break;
        }
        ret = buf_len;
    } while (false);

    return ret;
}

int getSm2SignPubkey(unsigned char ecc_x[32], unsigned char ecc_y[32]) {
    const char *operation = "getSm2SignPubkey";
    LOG_DEBUG("%s", operation);

    NEED_ENGINE(operation, 0)

    ECCPUBLICKEYBLOB PubKey;
    ULONG            ulBlobLen = sizeof(ECCPUBLICKEYBLOB);
    ULONG            rv;

    rv = s_skfApi.ExportPublicKey(s_usbKey.conHandle, TRUE, (unsigned char *)&PubKey, &ulBlobLen);
    if (rv != SAR_OK) {
        LOG_ERROR("SKF_ExportPublicKey error %#x size %d", rv, ulBlobLen);
    } else {
        memcpy(&ecc_x[0], &PubKey.XCoordinate[32], 32);
        memcpy(&ecc_y[0], &PubKey.YCoordinate[32], 32);
    }

    return (rv == SAR_OK);
}

int exportPublicKey(BOOL type, unsigned char *out, unsigned int out_len) {
    int         ret       = -1;
    const char *operation = "export public key";
    LOG_DEBUG("%s", operation);

    NEED_ENGINE(operation, -1)

    do {
        // 先获取公钥的长度
        ULONG buf_len = 0;
        ULONG rv      = s_skfApi.ExportPublicKey(s_usbKey.conHandle, type, NULL, &buf_len);
        if (rv != SAR_OK) {
            LOG_ERROR("%s failed, SKF_ExportPublicKey error %#x, type %d", operation, rv, type);
            break;
        }
        // 输入buf长度不够，返回错误
        if (out_len < buf_len) {
            LOG_ERROR("%s failed, Need len %d, but input %d", operation, buf_len, out_len);
            break;
        }

        rv = s_skfApi.ExportPublicKey(s_usbKey.conHandle, type, out, &buf_len);
        if (rv != SAR_OK) {
            LOG_ERROR("%s failed, SKF_ExportPublicKey error %#x, type %d", operation, rv, type);
            break;
        }
        ret = buf_len;
    } while (false);

    return ret;
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
            "err=%#x",
            rv);
        return 0;
    }
    LOG_DEBUG("certContetBITS: len %d", len);

    rv = s_skfApi.ExportCertificate(s_usbKey.conHandle, type, cert, &len);
    if (rv != SAR_OK) {
        LOG_ERROR(
            "[epass_load_ssl_client_cert], SKF_ExportCertificate failed, "
            "err=%#x",
            rv);
        return 0;
    }
    return 1;
}

int getContainerType() {
    const char *operation = "get container type";
    LOG_DEBUG("%s", operation);

    NEED_ENGINE(operation, -1)

    ULONG type;
    int   rv = s_skfApi.GetContainerType(s_usbKey.conHandle, &type);
    if (rv != SAR_OK) {
        LOG_ERROR("%s failed, err %#x", operation, rv);
        return -1;
    }
    return type;
}

/*
#define LIB_NAME "libes_3000gm.so.1.0.0"
#define LIB_PATH
"/home/pc/Downloads/temp/ePass3000GM-linux-20190117/ePass3000GM-linux-20190117/20190117/x86_64/release/" LIB_NAME int
test_init() {

    LOGD(WHAT("test_init start"));
    if (isEngineInit()) {
        return 1;
    }
    LibSkfUtils skfUtils;
    std::vector<UkeyInfo> info;
    skfUtils.enumAllInfo({LIB_PATH}, info);
    LibSkf::initApi(LIB_PATH);
    LibSkf::initEngine(const_cast<LPSTR>((LPSTR)info.front().vcDevs.front().devName.c_str()),
                       const_cast<LPSTR>((LPSTR)info.front().vcDevs.front().vcApps.front().appName.c_str()),
                       const_cast<LPSTR>((LPSTR)info.front().vcDevs.front().vcApps.front().vcCons.front().conName.c_str()),
                       "111111");
    LOGD(WHAT("test_init end"));
    return 1;
}
*/
