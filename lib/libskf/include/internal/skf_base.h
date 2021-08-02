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

#ifndef pFun_SKF_BASE_H
#define pFun_SKF_BASE_H

#include <string>
#include <vector>

#include "baseInstance.hpp"
#include "libskf/libskf.h"
#include "libskf/skf.h"

// 定义skf标准接口的函数类型，用于dll库加载使用
typedef ULONG(DEVAPI *PFun_SKF_EnumDev)(BOOL, LPSTR szNameList, ULONG *pulSize);
typedef ULONG(DEVAPI *PFun_SKF_ConnectDev)(LPSTR szName, DEVHANDLE *phDev);
typedef ULONG(DEVAPI *PFun_SKF_DisConnectDev)(DEVHANDLE hDev);
typedef LONG(DEVAPI *PFun_SKF_CloseContainer)(HCONTAINER hContainer);
typedef ULONG(DEVAPI *PFun_SKF_ClearSecureState)(HAPPLICATION hApplication);
typedef ULONG(DEVAPI *PFun_SKF_CloseApplication)(HAPPLICATION hApplication);
typedef ULONG(DEVAPI *PFun_SKF_ExportPublicKey)(HCONTAINER hContainer,
                                                BOOL bSignFlag, BYTE *pbBlob,
                                                ULONG *pulBlobLen);
typedef ULONG(DEVAPI *PFun_SKF_ExportCertificate)(HCONTAINER hContainer,
                                                  BOOL bSignFlag, BYTE *pbCert,
                                                  ULONG *pulCertLen);
typedef ULONG(DEVAPI *PFun_SKF_ECCSignData)(HCONTAINER hContainer,
                                            BYTE *pbDigest, ULONG ulDigestLen,
                                            ECCSIGNATUREBLOB *pSignature);
typedef ULONG(DEVAPI *PFun_SKF_RSASignData)(HCONTAINER hContainer, BYTE *pbData,
                                            ULONG ulDataLen, BYTE *pbSignature,
                                            ULONG *pulSignLen);
typedef ULONG(DEVAPI *PFun_SKF_VerifyPIN)(HAPPLICATION hApplication,
                                          ULONG ulPINType, LPSTR szPIN,
                                          ULONG *pulRetryCount);
typedef ULONG(DEVAPI *PFun_SKF_OpenApplication)(DEVHANDLE hDev, LPSTR szAppName,
                                                HAPPLICATION *phApplication);
typedef ULONG(DEVAPI *PFun_SKF_EnumApplication)(DEVHANDLE hDev, LPSTR szAppName,
                                                ULONG *pulSize);
typedef ULONG(DEVAPI *PFun_SKF_OpenContainer)(HAPPLICATION hApplication,
                                              LPSTR szContainerName,
                                              HCONTAINER *phContainer);
typedef ULONG(DEVAPI *PFun_SKF_EnumContainer)(HAPPLICATION hApplication,
                                              LPSTR szContainerName,
                                              ULONG *pulSize);
typedef ULONG(DEVAPI *PFun_SKF_GenRandom)(DEVHANDLE hDev, BYTE *pbRandom,
                                          ULONG ulRandomLen);

// 动态库api结构体
typedef struct DynamicApi {
    PFun_SKF_EnumDev pFun_SKF_EnumDev;
    PFun_SKF_ConnectDev pFun_SKF_ConnectDev;
    PFun_SKF_DisConnectDev pFun_SKF_DisConnectDev;
    PFun_SKF_CloseContainer pFun_SKF_CloseContainer;
    PFun_SKF_ClearSecureState pFun_SKF_ClearSecureState;
    PFun_SKF_CloseApplication pFun_SKF_CloseApplication;
    PFun_SKF_ExportPublicKey pFun_SKF_ExportPublicKey;
    PFun_SKF_ExportCertificate pFun_SKF_ExportCertificate;
    PFun_SKF_ECCSignData pFun_SKF_ECCSignData;
    PFun_SKF_RSASignData pFun_SKF_RSASignData;
    PFun_SKF_VerifyPIN pFun_SKF_VerifyPIN;
    PFun_SKF_OpenApplication pFun_SKF_OpenApplication;
    PFun_SKF_EnumApplication pFun_SKF_EnumApplication;
    PFun_SKF_OpenContainer pFun_SKF_OpenContainer;
    PFun_SKF_EnumContainer pFun_SKF_EnumContainer;
    PFun_SKF_GenRandom pFun_SKF_GenRandom;
} DynamicApi;

typedef enum CertType {
    CertType_SIGN,     // 加密证书
    CertType_ENCRYPT,  // 加密证书
} CertType;

// skf基类，用于定义一些通用接口
class SKFApiBase {
   public:
    /**
     * @brief lib库初始化，加载内部需要的skf函数，需要不同平台自己实现
     *
     * @param libPath const char * lib库路径
     * @return int SAR错误码，0成功，其他失败
     */
    virtual int initApi(const char *libPath) = 0;

    /**
     * @brief 连接设备
     *
     * @param devName [IN]设备名称
     * @param devHandle [OUT]连接成功的句柄
     * @return int SAR错误码，0成功，其他失败
     */
    int connectDev(LPSTR devName, DEVHANDLE &devHandle) const;

    /**
     * @brief 断开设备连接
     *
     * @param devHandle [IN]设备连接句柄
     * @return int SAR错误码，0成功，其他失败
     */
    int disConnectDev(DEVHANDLE devHandle) const;

    /**
     * @brief 打开app，根据设备句柄和名称
     *
     * @param appName [IN]app的名称
     * @param devHandle [IN]设备连接句柄
     * @param appHandle [OUT]app连接句柄
     * @return int SAR错误码，0成功，其他失败
     */
    int openApp(LPSTR appName, DEVHANDLE devHandle,
                HAPPLICATION &appHandle) const;

    /**
     * @brief 关闭app
     *
     * @param appHandle [IN]app连接句柄
     * @return int SAR错误码，0成功，其他失败
     */
    int closeApp(HAPPLICATION appHandle) const;

    /**
     * @brief 打开container
     *
     * @param conName [IN]container名字
     * @param appHandle [IN]app句柄
     * @param conHandle [OUT]container连接句柄
     * @return int SAR错误码，0成功，其他失败
     */
    int openContainer(LPSTR conName, HAPPLICATION appHandle,
                      HCONTAINER &conHandle) const;

    /**
     * @brief 关闭container
     *
     * @param conHandle [IN]container连接句柄
     * @return int SAR错误码，0成功，其他失败
     */
    int closeContainer(HCONTAINER conHandle) const;

    /**
     * @brief 校验pin
     *
     * @param appHandle [IN]app句柄
     * @param pin [IN]pin密码
     * @return int SAR错误码，0成功，其他失败
     */
    int verifyPin(HAPPLICATION appHandle, const std::string &pin) const;

    /**
     * @brief 枚举所有的app
     *
     * @param szList
     * @param devName
     * @return ULONG
     */
    ULONG enumAppByDevName(std::vector<std::string> &szList, LPSTR devName);

    /**
     * @brief 枚举所有的Container
     *
     * @param pszList
     * @return ULONG
     */
    ULONG enumContainer(std::vector<std::string> &pszList);

    int loadSm2Pubkey(unsigned char *sm2_x, unsigned char *sm2_y);
    int loadRsaPubkey(unsigned char *Modulus, unsigned char *PublicExponent);

    int loadCert(unsigned char *cert, ULONG *len);
    int doSign(const unsigned char *dgst, int dgst_len, unsigned char r[32],
               unsigned char s[32]);
    int doRsaSign(const unsigned char *dgst, int dgst_len, unsigned char *sign,
                  unsigned int *signLen);

    int enumAllInfo(std::vector<StrTreeNode> &devInfoList);

    int ECCSignData(HCONTAINER conHandle, BYTE *pbData, ULONG ulDataLen,
                    PECCSIGNATUREBLOB pSignature);
    // int GenRandom(DEVHANDLE hDev, BYTE *pbRandom, ULONG ulRandomLen);
    int ExportPublicKey(HCONTAINER hContainer, BOOL bSignFlag, BYTE *pbBlob,
                        ULONG *pulBlobLen);
    int ExportCertificate(HCONTAINER hContainer, BOOL bSignFlag, BYTE *pbCert,
                          ULONG *pulCertLen);

    int checkCertByCAIssuer(HCONTAINER hContainer, const char *Issuer);

   protected:
    /**
     * @brief Destroy the SKFApiBase object
     *
     */
    ~SKFApiBase();

    DynamicApi m_apiHandle;    // api接口定义结构体
    bool m_isApiInit = false;  // 标识api是否已经初始化

   private:
    /**
     * @brief 枚举所有设备
     *
     * @param devNameList [OUT]设备名称列表
     * @return int SAR错误码，0成功，其他失败
     */
    int enumDev(std::vector<std::string> &devNameList);

    /**
     * @brief 枚举所有的app
     *
     * @param szList
     * @param devHandle
     * @return ULONG
     */
    ULONG enumApp(std::vector<std::string> &szList, DEVHANDLE &devHandle);

    /**
     * @brief
     *
     * @param appName
     * @param devHandle
     * @param conNameList
     * @return int SAR错误码，0成功，其他失败
     */
    int enumContainerByAppName(LPSTR appName, DEVHANDLE devHandle,
                               std::vector<std::string> &conNameList);
    /**
     * @brief 枚举所有容器
     *
     * @param appHandle
     * @param conNameList
     * @return int SAR错误码，0成功，其他失败
     */
    int enumContainer(HAPPLICATION appHandle,
                      std::vector<std::string> &conNameList);

    /**
     * @brief Get the Cert By Container Name
     *
     * @param conName [IN]contianer名字
     * @param appHandle [IN]app连接句柄
     * @param type [IN]证书类型
     * @param cert [OUT]证书内容
     * @return int SAR错误码，0成功，其他失败
     */
    int getCertByContainerName(LPSTR conName, HAPPLICATION appHandle,
                               CertType type, std::vector<BYTE> &cert);

    /**
     * @brief Get the Cert By Container
     *
     * @param conHandle [IN]con连接句柄
     * @param type [IN]证书类型
     * @param cert [OUT]证书内容
     * @return int SAR错误码，0成功，其他失败
     */
    int getCertByContainer(HCONTAINER conHandle, CertType type,
                           std::vector<BYTE> &cert);
};

#if 0
// 新加的平台，使用此宏定义继承关系
#define INHERIT_SKF_BASE(name)                               \
    class SKFApi##name : public SKFApiBase,                  \
                         public BaseInstance<SKFApi##name> { \
       public:                                               \
        ~SKFApi##name();                                     \
        int initApi(const char *);                           \
                                                             \
       private:                                              \
        libHandleType m_libHandle;                           \
    };                                                       \
    typedef SKFApi##name SKFApi
#else
// 新加的平台，使用此宏定义继承关系
#define INHERIT_SKF_BASE(name, libHandleType) \
    class SKFApi##name : public SKFApiBase {  \
       public:                                \
        ~SKFApi##name();                      \
        int initApi(const char *);            \
                                              \
       private:                               \
        libHandleType m_libHandle;            \
    };                                        \
    typedef SKFApi##name SKFApi
#endif

#endif /* pFun_SKF_BASE_H */
