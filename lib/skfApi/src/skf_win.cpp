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

#ifdef _WIN32

#include "internal/skf_win.h"

#include <assert.h>
#include <atlconv.h>

#include <map>
#include <string>

#include "log.hpp"

static std::map<std::string, HMODULE> s_mapCache;

#define loadFunc(funcName)                                       \
    m_apiHandle.pFun_##funcName =                                \
        (PFun_##funcName)GetProcAddress(m_libHandle, #funcName); \
    if (m_apiHandle.pFun_##funcName == nullptr) {                \
        LOG_ERROR("Func %s not found in lib", #funcName);        \
        break;                                                   \
    }

SKFApiWin::~SKFApiWin() {
    if (m_libHandle != NULL) {
        // 会崩溃
        // FreeLibrary(m_libHandle);
        // 概率性崩溃，概率性阻塞
        // FreeLibraryAndExitThread(m_libHandle, 0);
        m_libHandle = NULL;
    }
    LOG_DEBUG("~SKFApiWin");
}

static std::string UTF8ToGBK(const std::string &strUTF8) {
    int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
    wchar_t *wszGBK = new wchar_t[len + 1];
    wmemset(wszGBK, 0, len + 1);
    MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUTF8.c_str(), -1, wszGBK, len);

    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
    char *szGBK = new char[len + 1];
    memset(szGBK, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
    // strUTF8 = szGBK;
    std::string strTemp(szGBK);
    delete[] szGBK;
    delete[] wszGBK;
    return strTemp;
}

int SKFApiWin::initApi(const char *libPath) {
    assert(libPath != NULL);
    // windows下需要将utf8转成gbk使用
    auto libPathUTF8 = UTF8ToGBK(libPath);
    bool bNeedBackup = false;
    int rv = SAR_FAIL;
    m_isApiInit = false;
    // 如果已经加载了库，断开重新加载
    if (m_libHandle != NULL) {
        // FreeLibrary(m_libHandle);
        m_libHandle = NULL;
        memset(&m_apiHandle, 0, sizeof(m_apiHandle));
    }

    auto iterFind = s_mapCache.find(libPathUTF8);
    if (iterFind != s_mapCache.end()) {
        m_libHandle = iterFind->second;
    } else {
        m_libHandle = LoadLibrary(libPathUTF8.c_str());
        bNeedBackup = true;
    }

    if (m_libHandle == NULL) {
        LOG_ERROR("Load library failed, errCode %d, libpath %s",
                  GetLastError(), libPathUTF8.c_str());
        return SAR_FAIL;
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
        if (bNeedBackup) {
            s_mapCache.insert(
                std::pair<std::string, HMODULE>(libPathUTF8, m_libHandle));
            LOG_INFO("Load library succ, addr=0x%p", (uint64_t)m_libHandle);
        }

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

#endif
