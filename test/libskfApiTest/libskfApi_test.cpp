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
#include "libskf/libskf.h"

#include "gtest/gtest.h"
#include "log.hpp"

static const char *Tag = "libskfTest";

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

        LOG_INFO("Test case setup, {:x}", 0xab);
    }

    /**
     * @brief run after this test case suit
     *
     */
    static void TearDownTestCase() {}

    /**
     * @brief run before every test case
     *
     */
    virtual void SetUp() {}

    /**
     * @brief run after every test case
     *
     */
    virtual void TearDown() {}
};

#define LIB_NAME "ShuttleCsp11_3000GM.dll"
#define LIB_PATH "C:\\Windows\\System32\\" LIB_NAME

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

#include <openssl/engine.h>

// 正常流程测试
TEST_F(MylibTestClass, normal) {
    std::vector<UkeyInfo> info;
    LibSkfUtils::enumAllInfo({LIB_PATH}, info);
    for (const auto &tmp : info) {
        printDevInfo(tmp);
    }

    LibSkf::initApi(LIB_PATH);
    LibSkf::initEngine(
        reinterpret_cast<LPSTR>(
            const_cast<char *>(info[0].vcDevs[0].devName.c_str())),
        reinterpret_cast<LPSTR>(
            const_cast<char *>(info[0].vcDevs[0].vcApps[0].appName.c_str())),
        reinterpret_cast<LPSTR>(const_cast<char *>(
            info[0].vcDevs[0].vcApps[0].vcCons[0].conName.c_str())),
        "123456");

    ENGINE *eng = NULL;
    const char *eng_id = "skf";
    ENGINE_load_dynamic();
    eng = ENGINE_by_id(eng_id);
    LOG_DEBUG("{}", (void *)eng);
}

// 正常流程测试
TEST_F(MylibTestClass, normal_issuer) {
    std::vector<UkeyInfo> info;
    LibSkfUtils::enumAllInfo({LIB_PATH}, info);
    LOG_DEBUG("All info:");
    for (const auto &tmp : info) {
        printDevInfo(tmp);
    }

    EXPECT_EQ(SAR_OK,
              LibSkfUtils::checkCertByCAIssuer(
                  LIB_PATH,
                  reinterpret_cast<LPSTR>(
                      const_cast<char *>(info[0].vcDevs[0].devName.c_str())),
                  reinterpret_cast<LPSTR>(const_cast<char *>(
                      info[0].vcDevs[0].vcApps[0].appName.c_str())),
                  reinterpret_cast<LPSTR>(const_cast<char *>(
                      info[0].vcDevs[0].vcApps[0].vcCons[0].conName.c_str())),
                  NULL,
                  "/C=cn/ST=hn/L=cs/O=sangfor/OU=ssl/CN=ssl/"
                  "emailAddress=aaa@qq.com"));
}
