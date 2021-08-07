/**
 * @file skf_win.h
 * @author wangyubo (18433@sangfor.com)
 * @brief skf windows接口
 * @version 0.1
 * @date 2021-07-20
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifdef __linux__

#include "internal/skf_linux.h"

#include <dlfcn.h>
#include <errno.h>
#include <assert.h>

#include "log.hpp"

#define loadFunc(funcName)                                \
    m_apiHandle.pFun_##funcName =                         \
        (PFun_##funcName)dlsym(m_libHandle, #funcName);   \
    if (m_apiHandle.pFun_##funcName == nullptr) {         \
        LOG_ERROR("Func %s not found in lib", #funcName); \
        return SAR_FAIL;                                  \
    }

SKFApiLinux::~SKFApiLinux() {
    if (m_libHandle != NULL) {
        dlclose(m_libHandle);
        m_libHandle = NULL;
    }
    LOG_DEBUG("~SKFApiLinux");
}

int SKFApiLinux::initApi(const char *libPath) {
    assert(libPath != NULL);
    int rv = SAR_FAIL;
    m_isApiInit = false;
    // 如果已经加载了库，断开重新加载
    if (m_libHandle != NULL) {
        dlclose(m_libHandle);
        m_libHandle = NULL;
        memset(&m_apiHandle, 0, sizeof(m_apiHandle));
    }
    m_libHandle = dlopen(libPath, RTLD_LAZY);
    if (m_libHandle == NULL) {
        LOG_ERROR("Load library failed, err %s, path %s", dlerror(), libPath);
        return -1;
    }
    do {
        loadFunc(SKF_EnumDev);
        loadFunc(SKF_ConnectDev);
        loadFunc(SKF_DisConnectDev);
        // loadFunc(SKF_ClearSecureState);
        // loadFunc(SKF_RSASignData);
        loadFunc(SKF_VerifyPIN);
        loadFunc(SKF_OpenApplication);
        loadFunc(SKF_EnumApplication);
        loadFunc(SKF_CloseApplication);
        // loadFunc(SKF_GenRandom);

        // 容器管理接口
        // loadFunc(SKF_CreateContainer);
        // loadFunc(SKF_DeleteContainer);
        loadFunc(SKF_OpenContainer);
        loadFunc(SKF_EnumContainer);
        loadFunc(SKF_CloseContainer);
        // loadFunc(SKF_GetContainerType);
        // loadFunc(SKF_ImportCertificate);
        loadFunc(SKF_ExportCertificate);
        loadFunc(SKF_ExportPublicKey);

        // sm3 杂凑算法
        // loadFunc(SKF_DigestInit);
        // loadFunc(SKF_Digest);
        // loadFunc(SKF_DigestUpdate);
        // loadFunc(SKF_DigestFinal);

        // sm2非对阵椭圆算法
        // loadFunc(SKF_GenECCKeyPair);
        // loadFunc(SKF_ImportECCKeyPair);
        loadFunc(SKF_ECCSignData);
        loadFunc(SKF_ECCVerify);
        // loadFunc(SKF_ECCExportSessionKey);
        // loadFunc(SKF_RSAExportSessionKeyEx);
        // loadFunc(SKF_ExtECCEncrypt);
        // loadFunc(SKF_ExtECCDecrypt);
        // loadFunc(SKF_ExtECCSign);
        // loadFunc(SKF_ExtECCVerify);
        // loadFunc(SKF_GenerateAgreementDataWithECC);
        // loadFunc(SKF_GenerateAgreementDataAndKeyWithECC);
        // loadFunc(SKF_GenerateKeyWithECC);
        m_isApiInit = true;
        return SAR_OK;
    } while (false);

    // 失败清理
    memset(&m_apiHandle, 0, sizeof(m_apiHandle));
    if (m_libHandle != NULL) {
        dlclose(m_libHandle);
        m_libHandle = NULL;
    }
    return SAR_FAIL;
}

#endif
