#include <assert.h>
#include <errno.h>
#include <stdio.h>

#include <iostream>

#include "libdcq.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
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

//  完成使用场景的测试
TEST(test, single) {
    //Create and return a shared_ptr to a multithreaded console logger.
    auto logger = getConsoleLogger();
    auto testDcq = dcq();
    EXPECT_EQ(SUCCESS, testDcq.init("/mnt/e/work/github_files/Clion_cppStudy/build/global.ini"));

    KeyValueMap result;
    EXPECT_EQ(SUCCESS, testDcq.parseOne("www.baidu.com", result));
    EXPECT_TRUE(result.size() > 0);
}

//  完成使用场景的测试
TEST(test, batch) {
    //Create and return a shared_ptr to a multithreaded console logger.
    auto logger = getConsoleLogger();
    auto testDcq = dcq();
    EXPECT_EQ(SUCCESS, testDcq.init("/mnt/e/work/github_files/Clion_cppStudy/build/global.ini"));

    vector<shared_ptr<KeyValueMap>> vResult;
    vector<string> vDomain = {"baidu.com", "sina.com", "baidu.com", "hustwyb.cn"};
    auto count = testDcq.parseBatch(vDomain, vResult);
    EXPECT_EQ(vDomain.size(), count);
    SPDLOG_LOGGER_INFO(logger, "{} {}", vDomain.size(), count);
}

int main(int argc, char **argv) { return xtest_start_test(argc, argv); }
