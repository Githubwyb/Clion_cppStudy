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
        std::string tmp = reinterpret_cast<char *>(nameList) + index;
        auto nameLen = tmp.length();
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

static int check_cert_issuer(const BYTE *certInfo, ULONG ulCertLen,
                             const char *certIssuer) {
    assert(certInfo != NULL);
    X509 *pUsrCert = NULL;
    X509_NAME *pCertIssuer = NULL;
    BIO *pBio = NULL;
    char *pszIssuerName = NULL;
    BOOL bRet = FALSE;
    if (certInfo == NULL || ulCertLen == 0)  //|| certIssuer == NULL)
    {
        return 0;
    }

    pBio = BIO_new(BIO_s_mem());
    if (pBio == NULL) {
        fprintf(stderr, "BIO_new failed \n");
        goto END;
    }
    BIO_write(pBio, certInfo, ulCertLen);
    pUsrCert = d2i_X509_bio(pBio, NULL);
    if (pUsrCert == NULL) {
        fprintf(stderr, "d2i_X509_bio failed \n");
        goto END;
    }
    pCertIssuer = X509_get_issuer_name(pUsrCert);
    if (!pCertIssuer) {
        fprintf(stderr, "X509_get_issuer failed \n");
        goto END;
    }
    pszIssuerName = X509_NAME_oneline(pCertIssuer, NULL, 0);
    if (!pszIssuerName) {
        fprintf(stderr, "X509_NAME_oneline failed \n");
        goto END;
    }

    if (strcmp(certIssuer, pszIssuerName) != 0) {
        LOG_ERROR("issuer not match");
        LOG_DEBUG("ukey %s, input %s", certIssuer, pszIssuerName);
        bRet = FALSE;
    } else {
        bRet = TRUE;
    }
END:
    if (pBio != NULL) {
        BIO_free(pBio);
    }
    if (pUsrCert != NULL) {
        X509_free(pUsrCert);
    }
    return bRet;
}

static int getCertSubject(const std::vector<BYTE> &certInfo,
                          std::string &subject) {
    X509 *pUsrCert = NULL;
    X509_NAME *pSubjectName = NULL;
    BIO *pBio = NULL;
    char *pszSubjectName = NULL;
    BOOL bRet = FALSE;

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
        bRet = TRUE;
    } while (false);
    if (pBio != NULL) {
        BIO_free(pBio);
    }
    if (pUsrCert != NULL) {
        X509_free(pUsrCert);
    }
    return bRet;
}

SKFApiBase::~SKFApiBase() { LOG_DEBUG("~SKFApiBase"); }

int SKFApiBase::verifyPin(HAPPLICATION appHandle,
                          const std::string &pin) const {
    assert(appHandle != NULL);
    ULONG retry;
    // 传入string，实际只要LPSTR，需要转换
    char *szPin = new char[pin.length() + 1];
    strncpy(szPin, pin.c_str(), pin.length() + 1);
    ULONG rv = m_apiHandle.pFun_SKF_VerifyPIN(
        appHandle, USER_TYPE, reinterpret_cast<LPSTR>(szPin), &retry);
    delete[] szPin;
    if (rv != SAR_OK) {
        LOG_ERROR("verify_pin SKF_VerifyPIN, error 0x%x, retry = %u", rv,
                  retry);
    }

    return rv;
}

ULONG SKFApiBase::enumAppByDevName(StrList &appNameList, LPSTR devName) {
    assert(devName != NULL);
    DEVHANDLE devHandle;
    ULONG rv = connectDev(devName, devHandle);
    if (rv != SAR_OK) {
        LOG_ERROR("connectDev failed, err 0x%x, dev %s", rv, devName);
        return rv;
    }
    rv = enumApp(appNameList, devHandle);
    disConnectDev(devHandle);
    if (rv != SAR_OK) {
        LOG_ERROR("enumApp by devHandle failed, err 0x%x, dev %s", rv, devName);
        return rv;
    }
    return rv;
}

// 枚举应用
ULONG SKFApiBase::enumApp(StrList &appNameList, DEVHANDLE &devHandle) {
    ULONG len = 0;
    LPSTR appName = NULL;

    // 同样分为两步，第一步获取长度，第二步获取内容
    ULONG rv = m_apiHandle.pFun_SKF_EnumApplication(devHandle, appName, &len);
    if (rv != SAR_OK) {
        LOG_ERROR("enumApp SKF_EnumApplication error 0x%x", rv);
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
            LOG_ERROR("SKF_EnumApplication error 0x%x", rv);
            break;
        }

        appNameList = getNameListFromLPSTR(appName, len);
        // LOG_DEBUG( "appName: ");
        // LOG_HEX(appName, len);
    } while (false);

    if (appName != NULL) {
        free(appName);
        appName = NULL;
    }
    return rv;
}

int SKFApiBase::enumContainerByAppName(LPSTR appName, DEVHANDLE devHandle,
                                       StrList &conNameList) {
    assert(appName != NULL);
    assert(devHandle != NULL);
    HAPPLICATION appHandle;
    ULONG rv = openApp(appName, devHandle, appHandle);
    if (rv != SAR_OK) {
        LOG_ERROR("openApp failed, err 0x%x, app %s", rv, appName);
        return rv;
    }
    rv = enumContainer(appHandle, conNameList);
    closeApp(appHandle);
    if (rv != SAR_OK) {
        LOG_ERROR("enumContainer by appHandle failed, err 0x%x, app %s", rv,
                  appName);
        return rv;
    }
    return rv;
}

int SKFApiBase::openContainer(LPSTR conName, HAPPLICATION appHandle,
                              HCONTAINER &conHandle) const {
    assert(conName != NULL);
    assert(appHandle != NULL);
    // 所有接口先判断是否初始化api
    if (!m_isApiInit) {
        LOG_ERROR("openContainer failed, api doesn't init");
        return SAR_FAIL;
    }

    ULONG rv =
        m_apiHandle.pFun_SKF_OpenContainer(appHandle, conName, &conHandle);
    if (rv != SAR_OK) {
        LOG_ERROR("SKF_OpenContainer, error, errCode: 0x%x, con %s", rv,
                  conName);
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
        LOG_ERROR("SKF_CloseContainer, error, errCode: 0x%x", rv);
    }
    return rv;
}

int SKFApiBase::enumContainer(HAPPLICATION appHandle, StrList &conNameList) {
    assert(appHandle != NULL);
    LPSTR szList = NULL;
    ULONG rv = SAR_FAIL;
    ULONG len;

    do {
        rv = m_apiHandle.pFun_SKF_EnumContainer(appHandle, NULL, &len);
        if (rv != SAR_OK) {
            LOG_ERROR("SKF_EnumContainer error, errCode 0x%x", rv);
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
            LOG_ERROR("SKF_EnumContainer error, errCode: 0x%x", rv);
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
int SKFApiBase::ECCSignData(HCONTAINER conHandle, BYTE *pbData, ULONG ulDataLen,
                            PECCSIGNATUREBLOB pSignature) {
    assert(conHandle != NULL);
    return m_apiHandle.pFun_SKF_ECCSignData(conHandle, pbData, ulDataLen,
                                            pSignature);
}

// int SKFApiBase::GenRandom(DEVHANDLE hDev, BYTE *pbRandom, ULONG ulRandomLen) {
//     assert(hDev != NULL);
//     assert(pbRandom != NULL);
//     return m_apiHandle.pFun_SKF_GenRandom(hDev, pbRandom, ulRandomLen);
// }

int SKFApiBase::ExportPublicKey(HCONTAINER hContainer, BOOL bSignFlag,
                                BYTE *pbBlob, ULONG *pulBlobLen) {
    assert(hContainer != NULL);
    assert(pbBlob != NULL);
    return m_apiHandle.pFun_SKF_ExportPublicKey(hContainer, bSignFlag, pbBlob,
                                                pulBlobLen);
}

int SKFApiBase::ExportCertificate(HCONTAINER hContainer, BOOL bSignFlag,
                                  BYTE *pbCert, ULONG *pulCertLen) {
    assert(hContainer != NULL);
    // assert(pbCert != NULL);
    return m_apiHandle.pFun_SKF_ExportCertificate(hContainer, bSignFlag, pbCert,
                                                  pulCertLen);
}

/*
int SKFApiBase::doRsaSign(const unsigned char *dgst, int dgst_len,
                          unsigned char *sign, unsigned int *signLen) {
    ULONG rv = SAR_FAIL;

    rv = openUsbKey();
    if (rv != SAR_OK) {
        LOG_ERROR( "openUsbKey failed, errCode: 0x%x", rv);
        return rv;
    }

    if (m_conHandle == NULL) {
        LOG_ERROR( "doSign m_conHandle is NULL");
        rv = SAR_FAIL;
        return rv;
    }
    //初始大小不能小于密钥长度，否则会报参数错误a000006.
    *signLen = 1024;
    rv = m_apiHandle.pFun_SKF_RSASignData(m_conHandle, (BYTE *)dgst, dgst_len,
                                           (BYTE *)sign, (ULONG *)signLen);
    if (rv != SAR_OK) {
        LOG_ERROR( "SKF_ECCSignData failed, errCode: 0x%x", rv);
        return rv;
    }
    return rv;
}
*/

int SKFApiBase::enumAllInfo(std::vector<StrTreeNode> &devInfoList) {
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
            LOG_ERROR("EnumDev failed, err 0x%x", rv);
            break;
        }

        devInfoList.clear();
        for (const auto &dev : devNameList) {
            DEVHANDLE devHandle;

            StrList appNameList;
            // 传入string，实际只要LPSTR，需要转换
            char *devName = new char[dev.length() + 1];
            strncpy(devName, dev.c_str(), dev.length() + 1);
            ULONG rv = connectDev(reinterpret_cast<LPSTR>(devName), devHandle);
            delete[] devName;
            if (rv != SAR_OK) {
                LOG_WARN("connectDev failed, continue, err 0x%x, dev %s", rv,
                         devName);
                continue;
            }
            rv = enumApp(appNameList, devHandle);
            if (rv != SAR_OK) {
                LOG_WARN(

                    "enumApp by devHandle failed, continue, err 0x%x, dev %s",
                    rv, devName);
                disConnectDev(devHandle);
                devHandle = NULL;
                continue;
            }

            // 遍历app，得到container
            StrTreeNode devNode = {dev, {}};
            for (const auto &app : appNameList) {
                HAPPLICATION appHandle;

                // 传入string，实际只要LPSTR，需要转换
                StrList conNameList;
                char *appName = new char[app.length() + 1];
                strncpy(appName, app.c_str(), app.length() + 1);
                ULONG rv = openApp(reinterpret_cast<LPSTR>(appName), devHandle,
                                   appHandle);
                delete[] appName;
                if (rv != SAR_OK) {
                    LOG_WARN("openApp failed, continue, err 0x%x, app %s", rv,
                             appName);
                    continue;
                }
                rv = enumContainer(appHandle, conNameList);
                if (rv != SAR_OK) {
                    LOG_WARN(
                        "enumContainer by appHandle failed, continue, err "
                        "0x%x, app %s",
                        rv, appName);
                    closeApp(appHandle);
                    continue;
                }

                // 遍历container，得到cert
                StrTreeNode appNode = {app, {}};
                for (const auto &con : conNameList) {
                    std::vector<BYTE> cert;
                    // 传入string，实际只要LPSTR，需要转换
                    char *conName = new char[con.length() + 1];
                    strncpy(conName, con.c_str(), con.length() + 1);
                    rv = getCertByContainerName(
                        reinterpret_cast<LPSTR>(conName), appHandle,
                        CertType_ENCRYPT, cert);
                    delete[] conName;
                    // check_cert_issuer(&(cert.front()), cert.size(), "test");
                    std::string subject;
                    getCertSubject(cert, subject);
                    StrTreeNode conNode = {con, {StrTreeNode(subject, {})}};
                    appNode.child.emplace_back(conNode);
                }
                closeApp(appHandle);
                devNode.child.emplace_back(appNode);
            }
            disConnectDev(devHandle);
            devInfoList.emplace_back(devNode);
        }

        rv = SAR_OK;
    } while (false);

    return rv;
}

int SKFApiBase::enumDev(StrList &devNameList) {
    ULONG rv = SAR_FAIL;
    LPSTR pszDevNameList = NULL;
    ULONG ulSize = 0;

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
            LOG_ERROR("SKF_EnumDev step 1 error: 0x%x", rv);
            break;
        }

        if (ulSize <= 0) {
            LOG_ERROR(

                "Dev size should be greater than zero, but its value is: %d",
                ulSize);
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
        LOG_ERROR("SKF_ConnectDev error, errCode: 0x%x", rv);
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
        LOG_ERROR("SKF_DisConnectDev error, errCode: 0x%x", rv);
    }
    return rv;
}

int SKFApiBase::openApp(LPSTR appName, DEVHANDLE devHandle,
                        HAPPLICATION &appHandle) const {
    assert(appName != NULL);
    assert(devHandle != NULL);
    // 所有接口先判断是否初始化api
    if (!m_isApiInit) {
        LOG_ERROR("openApp failed, api doesn't init");
        return SAR_FAIL;
    }

    ULONG rv =
        m_apiHandle.pFun_SKF_OpenApplication(devHandle, appName, &appHandle);
    if (rv != SAR_OK) {
        LOG_INFO("SKF_OpenApplication, error 0x%x", rv);
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
        LOG_ERROR("SKF_CloseApplication error, errCode: 0x%x", rv);
    }
    return rv;
}

int SKFApiBase::getCertByContainerName(LPSTR conName, HAPPLICATION appHandle,
                                       CertType type, std::vector<BYTE> &cert) {
    assert(conName != NULL);
    assert(appHandle != NULL);
    HCONTAINER conHandle;
    ULONG rv = openContainer(conName, appHandle, conHandle);
    if (rv != SAR_OK) {
        LOG_ERROR("openContianer by name failed, err 0x%x, con %s", rv,
                  conName);
        return rv;
    }
    rv = getCertByContainer(conHandle, type, cert);
    closeContainer(conHandle);
    if (rv != SAR_OK) {
        LOG_ERROR("getCertByContianer by conHandle failed, err 0x%x, con %s",
                  rv, conName);
    }
    return rv;
}

int SKFApiBase::getCertByContainer(HCONTAINER conHandle, CertType type,
                                   std::vector<BYTE> &cert) {
    assert(conHandle != NULL);
    ULONG len = 0;
    BYTE *pbCert = NULL;

    // 同样分为两步，第一步获取长度，第二步获取内容
    ULONG rv = m_apiHandle.pFun_SKF_ExportCertificate(
        conHandle, type == CertType_SIGN ? TRUE : FALSE, pbCert, &len);
    if (rv != SAR_OK) {
        LOG_ERROR("SKF_ExportCertificate error 0x%x", rv);
        return rv;
    }
    do {
        pbCert = (BYTE *)calloc(1, len + 1);
        if (pbCert == NULL) {
            LOG_ERROR("calloc failed, maybe memory not enough");
            return SAR_FAIL;
        }

        ULONG rv = m_apiHandle.pFun_SKF_ExportCertificate(
            conHandle, type == CertType_SIGN ? TRUE : FALSE, pbCert, &len);
        if (rv != SAR_OK) {
            LOG_ERROR("SKF_EnumApplication error 0x%x", rv);
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

int SKFApiBase::checkCertByCAIssuer(HCONTAINER hContainer, const char *Issuer) {
    std::vector<BYTE> cert;
    ULONG rv = getCertByContainer(hContainer, CertType_ENCRYPT, cert);
    if (rv != SAR_OK) {
        LOG_ERROR("SKF_EnumApplication error 0x%x", rv);
        return rv;
    }

    return check_cert_issuer(&(cert.front()), cert.size(), Issuer) == TRUE
               ? SAR_OK
               : SAR_FAIL;
}
