#include <assert.h>
#include <errno.h>
#include <stdio.h>

#include <iostream>

#include "libdcq.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include "xtest/xtest.h"

using namespace std;
using namespace libdcq;

static std::shared_ptr<spdlog::logger> getConsoleLogger() {
    auto logger = spdlog::get("console");
    if (logger != nullptr) {
        return logger;
    }
    logger = spdlog::stdout_color_mt("console");
    logger->set_level(spdlog::level::debug);  // Set global log level to debug
    logger->set_pattern("%Y-%m-%d %H:%M:%S [%P][%L][%@ %!] %v");
    return logger;
}

// 单个域名测试
TEST(test, single) {
    // Create and return a shared_ptr to a multithreaded console logger.
    auto logger = getConsoleLogger();
    auto testDcq = dcq();
    EXPECT_EQ(SUCCESS,
              testDcq.init(
                  "/mnt/e/work/github_files/Clion_cppStudy/build/global.ini"));

    KeyValueMap result;
    EXPECT_EQ(SUCCESS, testDcq.parseOne("www.baidu.com", result));
    EXPECT_TRUE(result.size() > 0);
}

// 批量场景测试
TEST(test, batch) {
    // Create and return a shared_ptr to a multithreaded console logger.
    auto logger = getConsoleLogger();
    auto testDcq = dcq();
    EXPECT_EQ(SUCCESS,
              testDcq.init(
                  "/mnt/e/work/github_files/Clion_cppStudy/build/global.ini"));

    vector<shared_ptr<KeyValueMap>> vResult;
    vector<string> vDomain = {"baidu.com", "sina.com", "baidu.com",
                              "hustwyb.cn"};
    auto count = testDcq.parseBatch(vDomain, vResult);
    EXPECT_EQ(vDomain.size(), count);
    for (size_t i = 0; i < vResult.size(); i++) {
        EXPECT_TRUE(vResult[i]->size() > 0);
    }
}

#include <sys/time.h>

suseconds_t getIntevel(struct timeval &startTime, struct timeval &endTime) {
    return ((endTime.tv_sec - startTime.tv_sec) * 1000000 + endTime.tv_usec -
            startTime.tv_usec);
}

// 异步单个场景测试
TEST(test, asyn_single) {
    // Create and return a shared_ptr to a multithreaded console logger.
    auto logger = getConsoleLogger();
    auto testDcq = dcq();
    EXPECT_EQ(SUCCESS,
              testDcq.init(
                  "/mnt/e/work/github_files/Clion_cppStudy/build/global.ini"));

    struct timeval startTime;
    gettimeofday(&startTime, nullptr);

    KeyValueMap result;
    auto ret = testDcq.asynParseOne("www.baidu.com", result);

    struct timeval endTime;
    gettimeofday(&endTime, nullptr);

    // 异步要求0.001秒内返回
    EXPECT_TRUE(getIntevel(startTime, endTime) < 1000);

    ret.wait();
    EXPECT_EQ(SUCCESS, ret.get());
    EXPECT_TRUE(result.size() > 0);
}

// 异步批量场景测试
TEST(test, asyn_batch) {
    // Create and return a shared_ptr to a multithreaded console logger.
    auto logger = getConsoleLogger();
    auto testDcq = dcq();
    EXPECT_EQ(SUCCESS,
              testDcq.init(
                  "/mnt/e/work/github_files/Clion_cppStudy/build/global.ini"));

    vector<shared_ptr<KeyValueMap>> vResult;
    vector<string> vDomain = {"baidu.com", "sina.com", "baidu.com",
                              "hustwyb.cn"};

    struct timeval startTime;
    gettimeofday(&startTime, nullptr);

    auto vRet = testDcq.asynParseBatch(vDomain, vResult);

    struct timeval endTime;
    gettimeofday(&endTime, nullptr);

    // 异步要求0.001秒内返回
    EXPECT_TRUE(getIntevel(startTime, endTime) < 1000);
    for (size_t i = 0; i < vDomain.size(); i++) {
        EXPECT_EQ(SUCCESS, vRet[i].get());
        EXPECT_TRUE(vResult[i]->size() > 0);
    }
}

// 未初始化场景测试
TEST(test, uninited) {
    // Create and return a shared_ptr to a multithreaded console logger.
    auto logger = getConsoleLogger();
    auto testDcq = dcq();

    // 单个
    KeyValueMap result;
    EXPECT_EQ(FAILED_UNINITED, testDcq.parseOne("www.baidu.com", result));
    // 批量
    vector<shared_ptr<KeyValueMap>> vResult;
    vector<string> vDomain = {"baidu.com", "sina.com", "baidu.com",
                              "hustwyb.cn"};
    EXPECT_EQ(0, testDcq.parseBatch(vDomain, vResult));
    // 异步单个
    EXPECT_EQ(FAILED_UNINITED,
              testDcq.asynParseOne("www.baidu.com", result).get());
    // 异步批量
    auto vRet = testDcq.asynParseBatch(vDomain, vResult);
    for (size_t i = 0; i < vDomain.size(); i++) {
        EXPECT_EQ(FAILED_UNINITED, vRet[i].get());
    }
}

int main(int argc, char **argv) { return xtest_start_test(argc, argv); }
