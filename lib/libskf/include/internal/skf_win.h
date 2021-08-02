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

#ifndef SKF_WIN_H
#define SKF_WIN_H

#include <windows.h>

#include "log.hpp"
#include "skf_base.h"

INHERIT_SKF_BASE(Win, HMODULE);

#define loadFunc(funcName)                                             \
    m_apiHandle.pFun_##funcName =                                     \
        (PFun_##funcName)GetProcAddress(m_libHandle, #funcName);       \
    if (m_apiHandle.pFun_##funcName == nullptr) {                     \
        LOG_ERROR("SKFApiWin", "Func %s not found in lib", #funcName); \
        break;                                                         \
    }

SKFApiWin::~SKFApiWin() {
    if (m_libHandle != NULL) {
        FreeLibrary(m_libHandle);
        m_libHandle = NULL;
    }
    LOG_DEBUG("SKFApiWin", "~SKFApiWin");
}

int SKFApiWin::initApi(const char *libPath) {
    int rv = SAR_FAIL;
    // 如果已经加载了库，断开重新加载
    if (m_libHandle != NULL) {
        FreeLibrary(m_libHandle);
        m_libHandle = NULL;
        memset(&m_apiHandle, 0, sizeof(m_apiHandle));
    }

    m_libHandle = LoadLibrary(libPath);
    if (m_libHandle == NULL) {
        LOG_ERROR("SKFApiWin", "Load library failed, errCode %d, path %s",
                  GetLastError(), libPath);
        return -1;
    }
    do {
        loadFunc(SKF_EnumDev);
        loadFunc(SKF_ConnectDev);
        loadFunc(SKF_DisConnectDev);
        loadFunc(SKF_CloseContainer);
        loadFunc(SKF_ClearSecureState);
        loadFunc(SKF_CloseApplication);
        loadFunc(SKF_ExportPublicKey);
        loadFunc(SKF_ExportCertificate);
        loadFunc(SKF_ECCSignData);
        loadFunc(SKF_RSASignData);
        loadFunc(SKF_VerifyPIN);
        loadFunc(SKF_OpenApplication);
        loadFunc(SKF_EnumApplication);
        loadFunc(SKF_OpenContainer);
        loadFunc(SKF_EnumContainer);
        loadFunc(SKF_GenRandom);
        m_isApiInit = true;
        return SAR_OK;
    } while (false);

    // 失败清理
    memset(&m_apiHandle, 0, sizeof(m_apiHandle));
    if (m_libHandle != NULL) {
        FreeLibrary(m_libHandle);
        m_libHandle = NULL;
    }
    return SAR_FAIL;
}

#endif /* SKF_WIN_H */
