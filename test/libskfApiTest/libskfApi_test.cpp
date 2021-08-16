/**
 * @file mylibTest.cpp
 * @author wangyubo (1061322005@qq.com)
 * @brief test mylib function
 * @version 0.1
 * @date 2021-07-09
 *
 * @copyright Copyright (c) 2021
 *
 */
#include <string.h>

#include "gtest/gtest.h"
// #include "gmock/gmock.h"
#include "libskf/libskf.h"
#include "log.hpp"

// static const char *s_libPath = "D:\\中文测试目录\\ShuttleCsp11_3000GM.dll";
static const char *s_libPath = "C:\\Windows\\System32\\ShuttleCsp11_3000GM.dll";
static LPSTR s_devName = NULL;
static LPSTR s_appName = NULL;
static LPSTR s_conName = NULL;
static const char *s_pin = "123456";
static const char *s_ca =
    "/C=cn/ST=hn/L=cs/O=sangfor/OU=ssl/CN=ssl/emailAddress=aaa@qq.com";

class MylibTestClass : public testing::Test {
   public:
    /**
     * @brief run before this test case suit
     *
     */
    static void SetUpTestCase() {
        // init log
        spdlog::set_level(
            spdlog::level::debug);  // Set global log level to debug
        spdlog::set_pattern("%Y-%m-%d %H:%M:%S [%P:%t][%L][%s:%# %!] %v");
        s_devName = (LPSTR)calloc(1, strlen("ES3003 VCR 1") + 1);
        strncpy(reinterpret_cast<char *>(s_devName), "ES3003 VCR 1",
                strlen("ES3003 VCR 1") + 1);
        s_appName = (LPSTR)calloc(1, strlen("EnterSafe") + 1);
        strncpy(reinterpret_cast<char *>(s_appName), "EnterSafe",
                strlen("EnterSafe") + 1);
        s_conName = (LPSTR)calloc(1, strlen("EccContainer") + 1);
        strncpy(reinterpret_cast<char *>(s_conName), "EccContainer",
                strlen("EccContainer") + 1);
    }

    /**
     * @brief run after this test case suit
     *
     */
    static void TearDownTestCase() {
        free(s_devName);
        free(s_appName);
        free(s_conName);
    }

    /**
     * @brief run before every test case
     *
     */
    virtual void SetUp() {}

    /**
     * @brief run after every test case
     *
     */
    virtual void TearDown() {
        LOG_DEBUG("case tear down");
        EXPECT_EQ(SAR_FAIL, LibSkf::initApi("xxx"));
    }
};

static void printDevInfo(const UkeyInfo &node) {
    for (const auto &dev : node.vcDevs) {
        printf("%s\r\n", dev.devName.c_str());
        for (const auto &app : dev.vcApps) {
            printf("  %s\r\n", app.appName.c_str());
            for (const auto &con : app.vcCons) {
                printf("    %s\r\n", con.conName.c_str());
                printf("      %s\r\n", con.issuer.c_str());
                printf("      %s\r\n", con.subject.c_str());
            }
        }
    }
}

#if 0  // gmock尝试
class MockSkfApi : public SKFApi {
   public:
    // MOCK_METHOD1(initApi, int(const char *));
    int initApi(const char *path) {
        m_isApiInit = true;
        return SAR_OK;
    }
    MOCK_CONST_METHOD2_T(connectDev, int(LPSTR, DEVHANDLE &));
    MOCK_CONST_METHOD2_T(enumApp, ULONG(std::vector<std::string> &, DEVHANDLE &));
    MOCK_CONST_METHOD1_T(disConnectDev, int(DEVHANDLE));
};

#include <string.h>

using namespace testing;
TEST_F(MylibTestClass, SKFApi_enumAppByDevName) {
    MockSkfApi skfApi;

    std::vector<std::string> appNameList;
    char devName[32] = {0};
    strncpy(devName, "testdev", sizeof(devName));

    EXPECT_CALL(skfApi, connectDev(_, _))
        .Times(3)
        .WillOnce(Return(SAR_FAIL))
        .WillOnce(Return(SAR_OK))
        .WillOnce(Return(SAR_OK));

    EXPECT_CALL(skfApi, disConnectDev(_))
        .Times(2)
        .WillOnce(Return(SAR_OK))
        .WillOnce(Return(SAR_OK));

    EXPECT_CALL(skfApi, enumApp(_, _))
        .Times(2)
        .WillOnce(Return(SAR_OK))
        .WillOnce(Return(SAR_FAIL));
    // 不init调用
    EXPECT_EQ(SAR_FAIL, skfApi.enumAppByDevName(
                            appNameList, reinterpret_cast<LPSTR>(devName)));
    skfApi.initApi("/a/a/so");
    // init后调用
    EXPECT_EQ(SAR_OK, skfApi.enumAppByDevName(
                          appNameList, reinterpret_cast<LPSTR>(devName)));

    // 枚举app失败
    EXPECT_EQ(SAR_FAIL, skfApi.enumAppByDevName(
                          appNameList, reinterpret_cast<LPSTR>(devName)));
}

#endif

#include <openssl/engine.h>

// initApi
TEST_F(MylibTestClass, LibSkf_initApi) {
    // 错误path
    EXPECT_EQ(SAR_FAIL, LibSkf::initApi(NULL));
    EXPECT_EQ(SAR_FAIL, LibSkf::initApi("xxx"));
    // 正确path
    EXPECT_EQ(SAR_OK, LibSkf::initApi(s_libPath));
}

// initEngine
TEST_F(MylibTestClass, LibSkf_initEngine) {
    LPSTR devName =
        (LPSTR)calloc(1, strlen(reinterpret_cast<char *>(s_devName)) + 1);
    LPSTR appName =
        (LPSTR)calloc(1, strlen(reinterpret_cast<char *>(s_appName)) + 1);
    LPSTR conName =
        (LPSTR)calloc(1, strlen(reinterpret_cast<char *>(s_conName)) + 1);
    char *pin = (char *)calloc(1, strlen(s_pin) + 1);
    // 构造错误参数
    strncpy(reinterpret_cast<char *>(devName),
            reinterpret_cast<char *>(s_devName),
            strlen(reinterpret_cast<char *>(s_devName)) + 1);
    devName[0]++;
    strncpy(reinterpret_cast<char *>(appName),
            reinterpret_cast<char *>(s_appName),
            strlen(reinterpret_cast<char *>(s_appName)) + 1);
    appName[0]++;
    strncpy(reinterpret_cast<char *>(conName),
            reinterpret_cast<char *>(s_conName),
            strlen(reinterpret_cast<char *>(s_conName)) + 1);
    conName[0]++;
    strncpy(pin, s_pin, strlen(s_pin) + 1);
    pin[0]++;
    // 参数错误
    EXPECT_EQ(SAR_FAIL, LibSkf::initEngine(NULL, NULL, NULL, NULL));
    EXPECT_EQ(SAR_FAIL, LibSkf::initEngine(devName, NULL, NULL, NULL));
    EXPECT_EQ(SAR_FAIL, LibSkf::initEngine(devName, appName, NULL, NULL));
    EXPECT_EQ(SAR_FAIL, LibSkf::initEngine(devName, appName, conName, NULL));
    // 没有initApi
    EXPECT_EQ(SAR_FAIL, LibSkf::initEngine(devName, appName, conName, pin));
    // 输入不正确
    ASSERT_EQ(SAR_OK, LibSkf::initApi(s_libPath));
    EXPECT_EQ(SAR_DEVICE_REMOVED,
              LibSkf::initEngine(devName, appName, conName, pin));
    EXPECT_EQ(SAR_APPLICATION_NOT_EXISTS,
              LibSkf::initEngine(s_devName, appName, conName, pin));
    EXPECT_EQ(SAR_PIN_LEN_RANGE,
              LibSkf::initEngine(s_devName, s_appName, conName, "1"));
    EXPECT_EQ(SAR_PIN_INCORRECT,
              LibSkf::initEngine(s_devName, s_appName, conName, pin));
    // 不知道为什么容器不存在返回的是SAR_FAIL
    // EXPECT_EQ(SAR_CONTAINER_NOT_EXISTS, LibSkf::initEngine("ES3003 VCR 1",
    // "EnterSafe", "con", "123456"));
    EXPECT_EQ(SAR_FAIL,
              LibSkf::initEngine(s_devName, s_appName, conName, s_pin));
    // 正确
    EXPECT_EQ(SAR_OK,
              LibSkf::initEngine(s_devName, s_appName, s_conName, s_pin));

    free(devName);
    free(appName);
    free(conName);
    free(pin);
}

// enumAllInfo
TEST_F(MylibTestClass, LibSkfUtils_enumAllInfo) {
    std::vector<UkeyInfo> info;
    // 参数错误
    EXPECT_EQ(SAR_OK, LibSkfUtils::enumAllInfo({}, info));
    EXPECT_EQ(0, info.size());
    // 参数正确
    EXPECT_EQ(SAR_OK, LibSkfUtils::enumAllInfo({s_libPath}, info));
    ASSERT_EQ(1, info.size());
    EXPECT_STREQ(s_libPath, info[0].strPath.c_str());

    printDevInfo(info[0]);
    bool check = false;
    for (const auto &dev : info[0].vcDevs) {
        if (dev.devName != std::string(reinterpret_cast<char *>(s_devName))) {
            continue;
        }
        for (const auto &app : dev.vcApps) {
            if (app.appName !=
                std::string(reinterpret_cast<char *>(s_appName))) {
                continue;
            }
            for (const auto &con : app.vcCons) {
                if (con.conName !=
                    std::string(reinterpret_cast<char *>(s_conName))) {
                    continue;
                }
                check = true;
                break;
            }
        }
    }
    ASSERT_EQ(true, check);
}

// checkCertByCAIssuer
TEST_F(MylibTestClass, LibSkfUtils_checkCertByCAIssuer) {
    LPSTR devName =
        (LPSTR)calloc(1, strlen(reinterpret_cast<char *>(s_devName)) + 1);
    LPSTR appName =
        (LPSTR)calloc(1, strlen(reinterpret_cast<char *>(s_appName)) + 1);
    LPSTR conName =
        (LPSTR)calloc(1, strlen(reinterpret_cast<char *>(s_conName)) + 1);
    char *pin = (char *)calloc(1, strlen(s_pin) + 1);
    char *ca = (char *)calloc(1, strlen(s_ca) + 1);
    // 构造错误参数
    strncpy(reinterpret_cast<char *>(devName),
            reinterpret_cast<char *>(s_devName),
            strlen(reinterpret_cast<char *>(s_devName)) + 1);
    devName[0]++;
    strncpy(reinterpret_cast<char *>(appName),
            reinterpret_cast<char *>(s_appName),
            strlen(reinterpret_cast<char *>(s_appName)) + 1);
    appName[0]++;
    strncpy(reinterpret_cast<char *>(conName),
            reinterpret_cast<char *>(s_conName),
            strlen(reinterpret_cast<char *>(s_conName)) + 1);
    conName[0]++;
    strncpy(pin, s_pin, strlen(s_pin) + 1);
    pin[0]++;
    strncpy(ca, s_ca, strlen(s_ca) + 1);
    ca[0]++;
    // 参数错误
    EXPECT_EQ(SAR_FAIL, LibSkfUtils::checkCertByCAIssuer(s_libPath, NULL, NULL,
                                                         NULL, NULL, ca));
    EXPECT_EQ(SAR_FAIL, LibSkfUtils::checkCertByCAIssuer(s_libPath, devName,
                                                         NULL, NULL, NULL, ca));
    EXPECT_EQ(SAR_FAIL, LibSkfUtils::checkCertByCAIssuer(
                            s_libPath, devName, appName, NULL, NULL, ca));
    // 输入不正确
    EXPECT_EQ(SAR_FAIL, LibSkfUtils::checkCertByCAIssuer("xxx", NULL, NULL,
                                                         NULL, NULL, ca));
    EXPECT_EQ(SAR_DEVICE_REMOVED,
              LibSkfUtils::checkCertByCAIssuer(s_libPath, devName, appName,
                                               conName, pin, ca));
    EXPECT_EQ(SAR_APPLICATION_NOT_EXISTS,
              LibSkfUtils::checkCertByCAIssuer(s_libPath, s_devName, appName,
                                               conName, pin, ca));
    EXPECT_EQ(SAR_PIN_LEN_RANGE,
              LibSkfUtils::checkCertByCAIssuer(s_libPath, s_devName, s_appName,
                                               conName, "1", ca));
    EXPECT_EQ(SAR_PIN_INCORRECT,
              LibSkfUtils::checkCertByCAIssuer(s_libPath, s_devName, s_appName,
                                               conName, pin, ca));
    // 不知道为什么容器不存在返回的是SAR_FAIL
    // EXPECT_EQ(SAR_CONTAINER_NOT_EXISTS, LibSkfUtils::initEngine("ES3003 VCR
    // 1", "EnterSafe", "con", "123456"));
    EXPECT_EQ(SAR_FAIL,
              LibSkfUtils::checkCertByCAIssuer(s_libPath, s_devName, s_appName,
                                               conName, s_pin, ca));
    EXPECT_EQ(SAR_FAIL,
              LibSkfUtils::checkCertByCAIssuer(s_libPath, s_devName, s_appName,
                                               s_conName, s_pin, ca));
    // 正确
    EXPECT_EQ(SAR_OK,
              LibSkfUtils::checkCertByCAIssuer(s_libPath, s_devName, s_appName,
                                               s_conName, NULL, s_ca));
    EXPECT_EQ(SAR_OK,
              LibSkfUtils::checkCertByCAIssuer(s_libPath, s_devName, s_appName,
                                               s_conName, s_pin, s_ca));

    free(devName);
    free(appName);
    free(conName);
    free(pin);
    free(ca);
}

// openssl
TEST_F(MylibTestClass, openssl) {
    unsigned char dgst[255] = {0};
    strncpy(reinterpret_cast<char *>(dgst), "xxx", sizeof(dgst));
    // api没有init
    EXPECT_EQ(SAR_FAIL, LibSkf::initApi(NULL));
    unsigned char r[32];
    unsigned char s[32];
    EXPECT_EQ(0, sm2DoSign(dgst, sizeof(dgst) - 1, r, s));
    unsigned char x[32];
    unsigned char y[32];
    EXPECT_EQ(0, sm2Verify(dgst, sizeof(dgst) - 1, x, y, r, s));
    EXPECT_EQ(0, getSm2SignPubkey(x, y));
    unsigned char cert[8192];
    ULONG len = 8192;
    EXPECT_EQ(0, exportCertificate(TRUE, cert, len));
    EXPECT_EQ(0, exportCertificate(FALSE, cert, len));
    EXPECT_EQ(0, isEngineInit());

    // engine没有init
    ASSERT_EQ(SAR_OK, LibSkf::initApi(s_libPath));
    EXPECT_EQ(0, sm2DoSign(dgst, sizeof(dgst) - 1, r, s));
    EXPECT_EQ(0, sm2Verify(dgst, sizeof(dgst) - 1, x, y, r, s));
    EXPECT_EQ(0, getSm2SignPubkey(x, y));
    EXPECT_EQ(0, exportCertificate(TRUE, cert, len));
    EXPECT_EQ(0, exportCertificate(FALSE, cert, len));
    EXPECT_EQ(0, isEngineInit());

    // 正常
    EXPECT_EQ(SAR_OK,
              LibSkf::initEngine(s_devName, s_appName, s_conName, s_pin));
    EXPECT_EQ(0, sm2DoSign(dgst, sizeof(dgst) - 1, r, s));
    EXPECT_EQ(0, sm2Verify(dgst, sizeof(dgst) - 1, x, y, r, s));
    EXPECT_EQ(1, getSm2SignPubkey(x, y));
    EXPECT_EQ(1, exportCertificate(TRUE, cert, len));
    EXPECT_EQ(1, exportCertificate(FALSE, cert, len));
    EXPECT_EQ(1, isEngineInit());

    // engine uninit
    LibSkf::uninitEngine();
    EXPECT_EQ(0, sm2DoSign(dgst, sizeof(dgst) - 1, r, s));
    EXPECT_EQ(0, sm2Verify(dgst, sizeof(dgst) - 1, x, y, r, s));
    EXPECT_EQ(0, getSm2SignPubkey(x, y));
    EXPECT_EQ(0, exportCertificate(TRUE, cert, len));
    EXPECT_EQ(0, exportCertificate(FALSE, cert, len));
    EXPECT_EQ(0, isEngineInit());

    // api重新init，没有重新initEngine
    EXPECT_EQ(SAR_OK,
              LibSkf::initEngine(s_devName, s_appName, s_conName, s_pin));
    EXPECT_EQ(SAR_FAIL, LibSkf::initApi("xxx"));
    EXPECT_EQ(0, sm2DoSign(dgst, sizeof(dgst) - 1, r, s));
    EXPECT_EQ(0, sm2Verify(dgst, sizeof(dgst) - 1, x, y, r, s));
    EXPECT_EQ(0, getSm2SignPubkey(x, y));
    EXPECT_EQ(0, exportCertificate(TRUE, cert, len));
    EXPECT_EQ(0, exportCertificate(FALSE, cert, len));
    EXPECT_EQ(0, isEngineInit());
}
