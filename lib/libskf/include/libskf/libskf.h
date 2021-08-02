/**
 * @file libskf.h
 * @author wangyubo (18433@sangfor.com)
 * @brief libskf库对外提供的头文件
 * @version 0.1
 * @date 2021-07-20
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef LIBSKF_H
#define LIBSKF_H

#include <string>
#include <vector>

#include "skf.h"

#if WIN32
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API __attribute__((visibility("default")))
#endif

typedef struct StrTreeNode {
    std::string val;
    std::vector<StrTreeNode> child;

    StrTreeNode();
    StrTreeNode(std::string ival, std::vector<StrTreeNode> ichild)
        : val(ival), child(ichild) {}
} StrTreeNode;

// openssl引擎配置类，可通过此类配置引擎
class EXPORT_API LibSkf {
   public:
    /**
     * @brief 初始化api接口，需要传入库路径
     *
     * @param libPath lib库路径
     * @return int SAR错误码，0成功，其他失败
     */
    static int initApi(const char *libPath);

    /**
     * @brief 初始化引擎，调用openssl前需要执行，执行一次后就会一直保存
     *
     * @param devName 设备名称
     * @param appName app名称
     * @param conName container名称
     * @param certPwd pin密码
     * @return int SAR错误码，0成功，其他失败
     */
    static int initEngine(LPSTR devName, LPSTR appName, LPSTR conName,
                          const char *certPwd);

    /**
     * @brief 关闭usbkey，使引擎不可用
     *
     */
    static void uninitEngine();
};

// 枚举设备使用的类，生命周期由外部接管
class EXPORT_API LibSkfUtils {
   public:
    /**
     * @brief 枚举所有设备信息
     *
     * @param libPath [IN]支持动态库路径
     * @param devInfoList [OUT]设备信息树
     * @return int SAR错误码，0成功，其他失败
     */
    static int enumAllInfo(const std::vector<std::string> &libPath,
                           std::vector<StrTreeNode> &devInfoList);

    /**
     * @brief 检查证书是否匹配ca
     *
     * @param libPath [IN]支持动态库路径
     * @param devName [IN]设备名称
     * @param appName [IN]app名称
     * @param conName [IN]container名称
     * @param issuer [IN]CA颁发者
     * @return int SAR错误码，0成功，其他失败
     */
    static int checkCertByCAIssuer(const std::string &libPath, LPSTR devName,
                                   LPSTR appName, LPSTR conName,
                                   const char *certPwd,
                                   const std::string &issuer);
};

// openssl引擎调用的函数，使用C函数传递出去
extern "C" EXPORT_API int sm2DoSign(const unsigned char *dgst, int dgst_len,
                                    unsigned char r[32], unsigned char s[32]);
// extern "C" EXPORT_API int genRandom(unsigned char *buf, int num);
extern "C" EXPORT_API int isEngineInit();
extern "C" EXPORT_API int getSm2SignPubkey(unsigned char ecc_x[32],
                                           unsigned char ecc_y[32]);
extern "C" EXPORT_API int exportCertificate(BOOL type, unsigned char *cert,
                                            ULONG &len);
extern "C" EXPORT_API int test_init();

#endif /* LIBSKF_H */
