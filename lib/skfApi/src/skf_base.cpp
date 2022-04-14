/**
 * @file skf_base.cpp
 * @author wangyubo (18433@sangfor.com)
 * @brief skf基本库代码
 * @version 0.1
 * @date 2021-07-20
 *
 * @copyright Copyright (c) 2021
 *
 */

#include <algorithm>

#include "internal/skf_base.h"

#include <assert.h>
#include <openssl/x509.h>
#include <string.h>

#include "log.hpp"

typedef std::vector<std::string> StrList;

static const char *Tag = "SKFApiBase";

static StrList getNameListFromLPSTR(const LPSTR nameList, const ULONG size) {
    assert(nameList != NULL);
    StrList result;

    ULONG index = 0;  // 标识当前字符串头部
    while (index < size) {
        std::string tmp     = reinterpret_cast<char *>(nameList) + index;
        auto        nameLen = tmp.length();
        if (nameLen == 0) {
            // 如果没有名字，说明到最后一个了，break掉
            break;
        }
        result.emplace_back(tmp);
        // 使用\0隔开name，需要向后多偏移一个
        index += nameLen + 1;
    }

    return result;
}

#define check0_f(x) ((x >= '0' && x <= '9') || (x >= 'a' && x <= 'f') || (x >= 'A' && x <= 'F'))
/**
 * @brief 将"\\xe9\\xe8"转成"\xe9\xe8"
 *
 * @param str 输入字符串
 * @return std::string 输出字符串，如果不符合格式将只输出符合的前几位
 */
std::string convertStrToByte(const std::string &str) {
    std::string result;
    size_t      index  = 0;
    size_t      strLen = str.length();
    while (index < strLen) {
        char tmp[2] = {0};
        // 满足条件转义，不满足条件，追加
        if (str[index] == '\\' && index + 3 < strLen && str[index + 1] == 'x' && check0_f(str[index + 2]) &&
            check0_f(str[index + 3]) && sscanf(&(str.front()) + index, "\\x%02hhx", &tmp) == 1) {
            result += tmp;
            index += 4;
            continue;
        }
        result += str[index++];
    }
    return result;
}

/*
 * @brief 字符串替换函数
 * @param strSource 原始字符串
 * @param strToReplace 需要替换的字符串内容
 * @param strReplaceAs 替换后的字符串内容
 * @return 返回完成的字符串
 */
std::string &stringReplace(std::string &strSource, const std::string &strToReplace, const std::string &strReplaceAs) {
    for (std::string::size_type pos(0); pos != std::string::npos; pos += strReplaceAs.length()) {
        if ((pos = strSource.find(strToReplace, pos)) != std::string::npos)
            strSource.replace(pos, strToReplace.length(), strReplaceAs);
        else
            break;
    }
    return strSource;
}

static int check_cert_issuer(const BYTE *certInfo, ULONG ulCertLen, const char *certIssuer) {
    assert(certInfo != NULL);
    X509      *pUsrCert      = NULL;
    X509_NAME *pCertIssuer   = NULL;
    BIO       *pBio          = NULL;
    char      *pszIssuerName = NULL;
    BOOL       bRet          = FALSE;
    if (certInfo == NULL || ulCertLen == 0)  //|| certIssuer == NULL)
    {
        return 0;
    }

    do {
        pBio = BIO_new(BIO_s_mem());
        if (pBio == NULL) {
            fprintf(stderr, "BIO_new failed \n");
            break;
        }
        BIO_write(pBio, certInfo, ulCertLen);
        pUsrCert = d2i_X509_bio(pBio, NULL);
        if (pUsrCert == NULL) {
            fprintf(stderr, "d2i_X509_bio failed \n");
            break;
        }
        pCertIssuer = X509_get_issuer_name(pUsrCert);
        if (!pCertIssuer) {
            fprintf(stderr, "X509_get_issuer failed \n");
            break;
        }
        pszIssuerName = X509_NAME_oneline(pCertIssuer, NULL, 0);
        if (!pszIssuerName) {
            fprintf(stderr, "X509_NAME_oneline failed \n");
            break;
        }

        std::string tmpIssuer = convertStrToByte(pszIssuerName);
        stringReplace(tmpIssuer, ", ", "/");
        if (strcmp(certIssuer, tmpIssuer.c_str()) != 0) {
            LOG_WARN("issuer not match, ukey %s, input %s", certIssuer, tmpIssuer.c_str());
            bRet = FALSE;
        } else {
            bRet = TRUE;
        }
    } while (0);

    if (pBio != NULL) {
        BIO_free(pBio);
    }
    if (pUsrCert != NULL) {
        X509_free(pUsrCert);
    }
    return bRet;
}

static int getCertIssuer(const std::vector<BYTE> &certInfo, std::string &issuer) {
    X509      *pUsrCert      = NULL;
    X509_NAME *pIssuerName   = NULL;
    BIO       *pBio          = NULL;
    char      *pszIssuerName = NULL;
    BOOL       bRet          = FALSE;

    do {
        pBio = BIO_new(BIO_s_mem());
        if (pBio == NULL) {
            LOG_ERROR("BIO_new failed");
            break;
        }
        BIO_write(pBio, &(certInfo.front()), certInfo.size());
        pUsrCert = d2i_X509_bio(pBio, NULL);
        if (pUsrCert == NULL) {
            LOG_ERROR("d2i_X509_bio failed \n");
            break;
        }
        pIssuerName = X509_get_issuer_name(pUsrCert);
        if (!pIssuerName) {
            LOG_ERROR("X509_get_issuer failed \n");
            break;
        }
        pszIssuerName = X509_NAME_oneline(pIssuerName, NULL, 0);
        if (!pszIssuerName) {
            LOG_ERROR("X509_NAME_oneline failed \n");
            break;
        }

        issuer = pszIssuerName;
        bRet   = TRUE;
    } while (false);
    if (pBio != NULL) {
        BIO_free(pBio);
    }
    if (pUsrCert != NULL) {
        X509_free(pUsrCert);
    }
    return bRet;
}

static int getCertSubject(const std::vector<BYTE> &certInfo, std::string &subject) {
    X509      *pUsrCert       = NULL;
    X509_NAME *pSubjectName   = NULL;
    BIO       *pBio           = NULL;
    char      *pszSubjectName = NULL;
    BOOL       bRet           = FALSE;

    do {
        pBio = BIO_new(BIO_s_mem());
        if (pBio == NULL) {
            LOG_ERROR("BIO_new failed");
            break;
        }
        BIO_write(pBio, &(certInfo.front()), certInfo.size());
        pUsrCert = d2i_X509_bio(pBio, NULL);
        if (pUsrCert == NULL) {
            LOG_ERROR("d2i_X509_bio failed \n");
            break;
        }
        pSubjectName = X509_get_subject_name(pUsrCert);
        if (!pSubjectName) {
            LOG_ERROR("X509_get_issuer failed \n");
            break;
        }
        pszSubjectName = X509_NAME_oneline(pSubjectName, NULL, 0);
        if (!pszSubjectName) {
            LOG_ERROR("X509_NAME_oneline failed \n");
            break;
        }

        subject = pszSubjectName;
        bRet    = TRUE;
    } while (false);

    if (pBio != NULL) BIO_free(pBio);

    if (pUsrCert != NULL) X509_free(pUsrCert);
    return bRet;
}

static bool getCertUserNameAndIssuer(const std::vector<BYTE> &certInfo, std::string &userName, std::string &issuer) {
    BIO       *pBio           = NULL;
    X509      *pUsrCert       = NULL;
    X509_NAME *pSubjectName   = NULL;
    char      *pszSubjectName = NULL;
    X509_NAME *pIssuerName    = NULL;
    char      *pszIssuerName  = NULL;
    ASN1_TIME *pAfterTime     = NULL;
    bool       bRet           = false;

    do {
        pBio = BIO_new(BIO_s_mem());
        if (pBio == NULL) {
            LOG_ERROR("BIO_new failed");
            break;
        }

        BIO_write(pBio, &(certInfo.front()), certInfo.size());
        pUsrCert = d2i_X509_bio(pBio, NULL);
        if (pUsrCert == NULL) {
            LOG_ERROR("d2i_X509_bio failed \n");
            break;
        }

        // 判断证书是否过期
        pAfterTime = X509_get_notAfter(pUsrCert);
        if (pAfterTime) {
            int dayDiff, secDiff = 0;
            if (ASN1_TIME_diff(&dayDiff, &secDiff, NULL, pAfterTime) != 0) {
                if (dayDiff < 1 && secDiff < 1) {
                    LOG_ERROR("The cert is out deadline.\n");
                    break;
                }
            }
        }

        // 获取颁发者
        pIssuerName = X509_get_issuer_name(pUsrCert);
        if (!pIssuerName) {
            LOG_ERROR("X509_get_issuer failed \n");
            break;
        }
        pszIssuerName = X509_NAME_oneline(pIssuerName, NULL, 0);
        if (!pszIssuerName) {
            LOG_ERROR("X509_NAME_oneline failed \n");
            break;
        }
        issuer = pszIssuerName;
        issuer = convertStrToByte(issuer);

        // 获取用户名
        pSubjectName = X509_get_subject_name(pUsrCert);
        if (!pSubjectName) {
            LOG_ERROR("X509_get_issuer failed \n");
            break;
        }
        pszSubjectName = X509_NAME_oneline(pSubjectName, NULL, 0);
        if (!pszSubjectName) {
            LOG_ERROR("X509_NAME_oneline failed \n");
            break;
        }

        const char  userNameSep[] = "CN=";
        std::string allSubject(pszSubjectName);
        std::string tempSubject(pszSubjectName);
        // 忽略USERNAME_SEP的大小写
        transform(tempSubject.begin(), tempSubject.end(), tempSubject.begin(), ::toupper);
        int iStart = tempSubject.find(userNameSep);
        if (iStart != std::string::npos) {
            iStart += strlen(userNameSep);
            int iEnd = allSubject.find("/", iStart);
            if (iEnd != std::string::npos)
                userName = allSubject.substr(iStart, iEnd - iStart);
            else
                userName = allSubject.substr(iStart, allSubject.size() - iStart);
        }
        if (userName.empty()) userName = pszSubjectName;

        // 中文情况下为\\xe9的方式，需要转成\xe9
        userName = convertStrToByte(userName);

        issuer = convertStrToByte(issuer);
        // 针对可以设置的内容信息，对内容中的(, ）替换成(/)
        stringReplace(issuer, ", ", "/");
        bRet = true;
    } while (false);

    if (pBio != NULL) BIO_free(pBio);

    if (pUsrCert != NULL) X509_free(pUsrCert);
    return bRet;
}

SKFApiBase::~SKFApiBase() { LOG_DEBUG("~SKFApiBase"); }

int SKFApiBase::verifyPin(HAPPLICATION appHandle, const std::string &pin) const {
    assert(appHandle != NULL);
    ULONG retry;
    // 传入string，实际只要LPSTR，需要转换
    char *szPin = new char[pin.length() + 1];
    strncpy(szPin, pin.c_str(), pin.length() + 1);
    ULONG rv = m_apiHandle.pFun_SKF_VerifyPIN(appHandle, USER_TYPE, reinterpret_cast<LPSTR>(szPin), &retry);
    delete[] szPin;
    if (rv != SAR_OK) {
        LOG_ERROR("verify_pin SKF_VerifyPIN, error %#x, retry = %u", rv, retry);
    }

    return rv;
}

ULONG SKFApiBase::enumAppByDevName(LPSTR devName, StrList &appNameList) const {
    assert(devName != NULL);
    DEVHANDLE devHandle;
    ULONG     rv = connectDev(devName, devHandle);
    if (rv != SAR_OK) {
        LOG_ERROR("connectDev failed, err %#x, dev %s", rv, devName);
        return rv;
    }
    rv = enumApp(devHandle, appNameList);
    disConnectDev(devHandle);
    if (rv != SAR_OK) {
        LOG_ERROR("enumApp by devHandle failed, err %#x, dev %s", rv, devName);
        return rv;
    }
    return rv;
}

// 枚举应用
ULONG SKFApiBase::enumApp(DEVHANDLE devHandle, StrList &appNameList) const {
    ULONG len     = 0;
    LPSTR appName = NULL;

    // 同样分为两步，第一步获取长度，第二步获取内容
    ULONG rv = m_apiHandle.pFun_SKF_EnumApplication(devHandle, appName, &len);
    if (rv != SAR_OK) {
        LOG_ERROR("enumApp SKF_EnumApplication error %#x", rv);
        return rv;
    }

    do {
        appName = (LPSTR)calloc(1, len);
        if (appName == NULL) {
            LOG_ERROR("calloc failed, maybe memory not enough");
            return SAR_FAIL;
        }
        rv = m_apiHandle.pFun_SKF_EnumApplication(devHandle, appName, &len);
        if (rv != SAR_OK) {
            LOG_ERROR("SKF_EnumApplication error %#x", rv);
            break;
        }

        appNameList = getNameListFromLPSTR(appName, len);
    } while (false);

    if (appName != NULL) {
        free(appName);
        appName = NULL;
    }
    return rv;
}

int SKFApiBase::enumContainerByAppName(LPSTR appName, DEVHANDLE devHandle, StrList &conNameList) const {
    assert(appName != NULL);
    assert(devHandle != NULL);
    HAPPLICATION appHandle;
    ULONG        rv = openApp(appName, devHandle, appHandle);
    if (rv != SAR_OK) {
        LOG_ERROR("openApp failed, err %#x, app %s", rv, appName);
        return rv;
    }
    rv = enumContainer(appHandle, conNameList);
    closeApp(appHandle);
    if (rv != SAR_OK) {
        LOG_ERROR("enumContainer by appHandle failed, err %#x, app %s", rv, appName);
        return rv;
    }
    return rv;
}

int SKFApiBase::openContainer(LPSTR conName, HAPPLICATION appHandle, HCONTAINER &conHandle) const {
    assert(conName != NULL);
    assert(appHandle != NULL);
    // 所有接口先判断是否初始化api
    if (!m_isApiInit) {
        LOG_ERROR("openContainer failed, api doesn't init");
        return SAR_FAIL;
    }

    ULONG rv = m_apiHandle.pFun_SKF_OpenContainer(appHandle, conName, &conHandle);
    if (rv != SAR_OK) {
        LOG_ERROR("SKF_OpenContainer, error, errCode: %#x, con %s", rv, conName);
    }
    return rv;
}

int SKFApiBase::closeContainer(HCONTAINER conHandle) const {
    // 所有接口先判断是否初始化api
    if (!m_isApiInit) {
        LOG_ERROR("closeContainer failed, api doesn't init");
        return SAR_FAIL;
    }

    if (conHandle == NULL) {
        return SAR_OK;
    }

    ULONG rv = m_apiHandle.pFun_SKF_CloseContainer(conHandle);
    if (rv != SAR_OK) {
        LOG_ERROR("SKF_CloseContainer, error, errCode: %#x", rv);
    }
    return rv;
}

int SKFApiBase::enumContainer(HAPPLICATION appHandle, StrList &conNameList) const {
    assert(appHandle != NULL);
    LPSTR szList = NULL;
    ULONG rv     = SAR_FAIL;
    ULONG len;

    do {
        rv = m_apiHandle.pFun_SKF_EnumContainer(appHandle, NULL, &len);
        if (rv != SAR_OK) {
            LOG_ERROR("SKF_EnumContainer error, errCode %#x", rv);
            break;
        }
        szList = (LPSTR)calloc(1, len);
        if (szList == NULL) {
            LOG_ERROR("enumContainer: calloc failed, maybe memory not enough");
            rv = SAR_FAIL;
            break;
        }
        rv = m_apiHandle.pFun_SKF_EnumContainer(appHandle, szList, &len);
        if (rv != SAR_OK) {
            LOG_ERROR("SKF_EnumContainer error, errCode: %#x", rv);
            break;
        }
        conNameList = getNameListFromLPSTR(szList, len);
    } while (false);

    if (szList != NULL) {
        free(szList);
        szList = NULL;
    }
    return rv;
}

//对一个摘要数据做签名
int SKFApiBase::ECCSignData(HCONTAINER conHandle, BYTE *pbData, ULONG ulDataLen, PECCSIGNATUREBLOB pSignature) const {
    assert(conHandle != NULL);
    return m_apiHandle.pFun_SKF_ECCSignData(conHandle, pbData, ulDataLen, pSignature);
}

int SKFApiBase::ECCVerify(DEVHANDLE         hDev,
                          ECCPUBLICKEYBLOB *pECCPubKeyBlob,
                          BYTE             *pbData,
                          ULONG             ulDataLen,
                          ECCSIGNATUREBLOB *pSignature) const {
    assert(hDev != NULL);
    return m_apiHandle.pFun_SKF_ECCVerify(hDev, pECCPubKeyBlob, pbData, ulDataLen, pSignature);
}

int SKFApiBase::RSASignData(HCONTAINER hContainer,
                            BYTE      *pbData,
                            ULONG      ulDataLen,
                            BYTE      *pbSignature,
                            ULONG     *pulSignLen) const {
    assert(hContainer != NULL);
    return m_apiHandle.pFun_SKF_RSASignData(hContainer, pbData, ulDataLen, pbSignature, pulSignLen);
}

// int SKFApiBase::GenRandom(DEVHANDLE hDev, BYTE *pbRandom, ULONG ulRandomLen)
// {
//     assert(hDev != NULL);
//     assert(pbRandom != NULL);
//     return m_apiHandle.pFun_SKF_GenRandom(hDev, pbRandom, ulRandomLen);
// }

int SKFApiBase::ExportPublicKey(HCONTAINER hContainer, BOOL bSignFlag, BYTE *pbBlob, ULONG *pulBlobLen) const {
    assert(hContainer != NULL);
    return m_apiHandle.pFun_SKF_ExportPublicKey(hContainer, bSignFlag, pbBlob, pulBlobLen);
}

int SKFApiBase::ExportCertificate(HCONTAINER hContainer, BOOL bSignFlag, BYTE *pbCert, ULONG *pulCertLen) const {
    assert(hContainer != NULL);
    return m_apiHandle.pFun_SKF_ExportCertificate(hContainer, bSignFlag, pbCert, pulCertLen);
}

int SKFApiBase::GetContainerType(HCONTAINER hContainer, ULONG *pulContainerType) const {
    assert(hContainer != NULL);
    return m_apiHandle.pFun_SKF_GetContainerType(hContainer, pulContainerType);
}

int SKFApiBase::enumAllInfo(std::vector<UkeyDev> &devInfoList) const {
    int rv = SAR_FAIL;
    // 所有接口先判断是否初始化api
    if (!m_isApiInit) {
        LOG_ERROR("enumAllInfo failed, api doesn't init");
        return -1;
    }

    do {
        StrList devNameList;
        rv = enumDev(devNameList);
        if (rv != SAR_OK) {
            LOG_ERROR("EnumDev failed, err %#x", rv);
            break;
        }

        devInfoList.clear();
        for (const auto &dev : devNameList) {
            UkeyDev   tempDev;
            DEVHANDLE devHandle;

            StrList appNameList;
            tempDev.devName = dev;
            // 传入string，实际只要LPSTR，需要转换
            char *devName = new char[dev.length() + 1];
            strncpy(devName, dev.c_str(), dev.length() + 1);
            ULONG rv = connectDev(reinterpret_cast<LPSTR>(devName), devHandle);
            delete[] devName;
            if (rv != SAR_OK) {
                LOG_WARN("connectDev failed, continue, err %#x, dev %s", rv, devName);
                continue;
            }
            rv = enumApp(devHandle, appNameList);
            if (rv != SAR_OK) {
                LOG_WARN("enumApp by devHandle failed, continue, err %#x, dev %s", rv, devName);
                disConnectDev(devHandle);
                devHandle = NULL;
                continue;
            }

            // 遍历app，得到container
            for (const auto &app : appNameList) {
                UkeyApp      tempApp;
                HAPPLICATION appHandle;

                tempApp.appName = app;
                // 传入string，实际只要LPSTR，需要转换
                StrList conNameList;
                char   *appName = new char[app.length() + 1];
                strncpy(appName, app.c_str(), app.length() + 1);
                ULONG rv = openApp(reinterpret_cast<LPSTR>(appName), devHandle, appHandle);
                delete[] appName;
                if (rv != SAR_OK) {
                    LOG_WARN("openApp failed, continue, err %#x, app %s", rv, appName);
                    continue;
                }

                rv = enumContainer(appHandle, conNameList);
                if (rv != SAR_OK) {
                    LOG_WARN(
                        "enumContainer by appHandle failed, continue, err "
                        "%#x, app %s",
                        rv,
                        appName);
                    closeApp(appHandle);
                    continue;
                }

                // 遍历container，得到cert
                for (const auto &con : conNameList) {
                    UkeyCon           tempCon;
                    std::vector<BYTE> cert;
                    tempCon.conName = con;
                    // 传入string，实际只要LPSTR，需要转换
                    char *conName = new char[con.length() + 1];
                    strncpy(conName, con.c_str(), con.length() + 1);
                    rv = getCertByContainerName(reinterpret_cast<LPSTR>(conName), appHandle, CertType_ENCRYPT, cert);
                    delete[] conName;
                    if (cert.size() == 0) {
                        continue;
                    }

                    if (getCertUserNameAndIssuer(cert, tempCon.subject, tempCon.issuer))
                        tempApp.vcCons.emplace_back(tempCon);
                }
                closeApp(appHandle);
                tempDev.vcApps.emplace_back(tempApp);
            }
            disConnectDev(devHandle);
            devInfoList.emplace_back(tempDev);
        }

        rv = SAR_OK;
    } while (false);

    return rv;
}

int SKFApiBase::enumDev(StrList &devNameList) const {
    ULONG rv             = SAR_FAIL;
    LPSTR pszDevNameList = NULL;
    ULONG ulSize         = 0;

    // 所有接口先判断是否初始化api
    if (!m_isApiInit) {
        LOG_ERROR("openUsbKey failed, api doesn't init");
        return rv;
    }

    do {
        // 需要两步，第一步获取名字列表的长度，根据长度申请内存
        // 第二步获取真实的名称列表
        rv = m_apiHandle.pFun_SKF_EnumDev(TRUE, pszDevNameList, &ulSize);
        if (rv != SAR_OK) {
            LOG_ERROR("SKF_EnumDev step 1 error: %#x", rv);
            break;
        }

        if (ulSize <= 0) {
            LOG_ERROR("Dev size should be greater than zero, but its value is: %d", ulSize);
            rv = SAR_FAIL;
            break;
        }

        pszDevNameList = (LPSTR)calloc(1, ulSize);
        if (pszDevNameList == NULL) {
            LOG_ERROR("openUsbKey: calloc failed, maybe memory not enough");
            rv = SAR_FAIL;
            break;
        }

        rv = m_apiHandle.pFun_SKF_EnumDev(TRUE, pszDevNameList, &ulSize);
        if (rv != SAR_OK) {
            LOG_ERROR("SKF_EnumDev step 2 error: %x", rv);
            break;
        }

        // LOG_DEBUG( "DevName: ");
        // LOG_HEX(pszDevNameList, ulSize);
        devNameList = getNameListFromLPSTR(pszDevNameList, ulSize);
    } while (false);

    if (pszDevNameList != NULL) {
        free(pszDevNameList);
        pszDevNameList = NULL;
    }
    return SAR_OK;
}

int SKFApiBase::connectDev(LPSTR devName, DEVHANDLE &devHandle) const {
    assert(devName != NULL);
    // 所有接口先判断是否初始化api
    if (!m_isApiInit) {
        LOG_ERROR("connectDev failed, api doesn't init");
        return SAR_FAIL;
    }

    ULONG rv = m_apiHandle.pFun_SKF_ConnectDev(devName, &devHandle);
    if (rv != SAR_OK) {
        LOG_ERROR("SKF_ConnectDev error, errCode: %#x", rv);
    }
    return rv;
}

int SKFApiBase::disConnectDev(DEVHANDLE devHandle) const {
    // 所有接口先判断是否初始化api
    if (!m_isApiInit) {
        LOG_ERROR("disConnectDev failed, api doesn't init");
        return SAR_FAIL;
    }

    if (devHandle == NULL) {
        return SAR_OK;
    }

    ULONG rv = m_apiHandle.pFun_SKF_DisConnectDev(devHandle);
    if (rv != SAR_OK) {
        LOG_ERROR("SKF_DisConnectDev error, errCode: %#x", rv);
    }
    return rv;
}

int SKFApiBase::openApp(LPSTR appName, DEVHANDLE devHandle, HAPPLICATION &appHandle) const {
    assert(appName != NULL);
    assert(devHandle != NULL);
    // 所有接口先判断是否初始化api
    if (!m_isApiInit) {
        LOG_ERROR("openApp failed, api doesn't init");
        return SAR_FAIL;
    }

    ULONG rv = m_apiHandle.pFun_SKF_OpenApplication(devHandle, appName, &appHandle);
    if (rv != SAR_OK) {
        LOG_INFO("SKF_OpenApplication, error %#x", rv);
    }

    return rv;
}

int SKFApiBase::closeApp(HAPPLICATION appHandle) const {
    // 所有接口先判断是否初始化api
    if (!m_isApiInit) {
        LOG_ERROR("closeApp failed, api doesn't init");
        return SAR_FAIL;
    }

    if (appHandle == NULL) {
        return SAR_OK;
    }

    ULONG rv = m_apiHandle.pFun_SKF_CloseApplication(appHandle);
    if (rv != SAR_OK) {
        LOG_ERROR("SKF_CloseApplication error, errCode: %#x", rv);
    }
    return rv;
}

int SKFApiBase::getCertByContainerName(LPSTR              conName,
                                       HAPPLICATION       appHandle,
                                       CertType           type,
                                       std::vector<BYTE> &cert) const {
    assert(conName != NULL);
    assert(appHandle != NULL);
    HCONTAINER conHandle;
    ULONG      rv = openContainer(conName, appHandle, conHandle);
    if (rv != SAR_OK) {
        LOG_ERROR("openContianer by name failed, err %#x, con %s", rv, conName);
        return rv;
    }
    rv = getCertByContainer(conHandle, type, cert);
    closeContainer(conHandle);
    if (rv != SAR_OK) {
        LOG_ERROR("getCertByContianer by conHandle failed, err %#x, con %s", rv, conName);
    }
    return rv;
}

int SKFApiBase::getCertByContainer(HCONTAINER conHandle, CertType type, std::vector<BYTE> &cert) const {
    assert(conHandle != NULL);
    ULONG len    = 0;
    BYTE *pbCert = NULL;

    // 同样分为两步，第一步获取长度，第二步获取内容
    ULONG rv = m_apiHandle.pFun_SKF_ExportCertificate(conHandle, type == CertType_SIGN ? TRUE : FALSE, pbCert, &len);
    if (rv != SAR_OK) {
        LOG_ERROR("SKF_ExportCertificate error %#x", rv);
        return rv;
    }
    do {
        pbCert = (BYTE *)calloc(1, len + 1);
        if (pbCert == NULL) {
            LOG_ERROR("calloc failed, maybe memory not enough");
            return SAR_FAIL;
        }

        ULONG rv =
            m_apiHandle.pFun_SKF_ExportCertificate(conHandle, type == CertType_SIGN ? TRUE : FALSE, pbCert, &len);
        if (rv != SAR_OK) {
            LOG_ERROR("SKF_EnumApplication error %#x", rv);
            break;
        }

        cert.assign(pbCert, pbCert + len);
        // LOG_DEBUG( "cert:");
        // LOG_HEX(&(*cert.begin()), cert.size());
    } while (false);

    if (pbCert != NULL) {
        free(pbCert);
        pbCert = NULL;
    }
    return rv;
}

int SKFApiBase::checkCertByCAIssuer(HCONTAINER hContainer, const char *Issuer) const {
    assert(Issuer != NULL);
    const char *operation = "checkCertByCAIssuer";
    LOG_DEBUG("[%s] Issuer %s", operation, Issuer);
    std::vector<BYTE> cert;
    ULONG             rv = getCertByContainer(hContainer, CertType_ENCRYPT, cert);
    if (rv != SAR_OK) {
        LOG_ERROR("getCertByContainer error %#x", rv);
        return rv;
    }

    return check_cert_issuer(&(cert.front()), cert.size(), Issuer) == TRUE ? SAR_OK : SAR_FAIL;
}

int SKFApiBase::openUsbkey(LPSTR devName, LPSTR appName, LPSTR conName, const char *certPwd, Usbkey &usbKey) const {
    assert(devName != NULL);
    assert(appName != NULL);
    assert(conName != NULL);
    const char *operation = "openUsbkey";
    LOG_DEBUG("[%s] devName %s, appName %s, conName %s", operation, devName, appName, conName);
    ULONG rv;
    // 关掉上一次的usbkey
    closeUsbkey(usbKey);
    do {
        rv = connectDev(devName, usbKey.devHandle);
        if (rv != SAR_OK) {
            LOG_ERROR("connectDev failed, err %#x, dev %s", rv, devName);
            break;
        }
        rv = openApp(appName, usbKey.devHandle, usbKey.appHandle);
        if (rv != SAR_OK) {
            LOG_ERROR("openApp failed, err %#x, app %s", rv, appName);
            break;
        }

        // 某些情况可以不需要输入pin就可以执行
        if (certPwd != NULL) {
            rv = verifyPin(usbKey.appHandle, certPwd);
            if (rv != SAR_OK) {
                LOG_ERROR("verifyPin failed, err %#x", rv);
                break;
            }
            usbKey.certPwd = certPwd;
        }
        rv = openContainer(conName, usbKey.appHandle, usbKey.conHandle);
        if (rv != SAR_OK) {
            LOG_ERROR("openContainer failed, err %#x, conName %s", rv, conName);
            break;
        }
        return SAR_OK;
    } while (false);

    closeUsbkey(usbKey);
    return rv;
}

void SKFApiBase::closeUsbkey(Usbkey &usbKey) const {
    if (usbKey.conHandle != NULL) {
        closeContainer(usbKey.conHandle);
        usbKey.conHandle = NULL;
    }

    if (usbKey.appHandle != NULL) {
        closeApp(usbKey.appHandle);
        usbKey.appHandle = NULL;
    }

    if (usbKey.devHandle != NULL) {
        disConnectDev(usbKey.devHandle);
        usbKey.devHandle = NULL;
    }
}
