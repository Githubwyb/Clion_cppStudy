#include <assert.h>
#include <errno.h>
#include <stdio.h>

#include <iostream>

#include "configManager.hpp"
#include "log.hpp"
#include "outputManager.hpp"
#include "parserManager.hpp"
#include "utils.hpp"
#include "xtest/xtest.h"

using namespace std;

//  完成使用场景的测试
TEST(test, scene) {
    auto &config = configManager::getInstance();
    char str1[] = "./";
    char str2[] = "www.baidu.com";
    char str3[] = "-d";
    char *argV[3] = {str1, str2, str3};
    int argC = sizeof(argV) / sizeof(argV[0]);
    // 解析命令行参数，将用命令行参数替换全局配置
    EXPECT_EQ(configManager::SUCCESS, config.getCmdLineParam(argC, argV));
    // 读取全局配置，此配置为固定文件，程序目录（非运行目录）同级的global.ini
    string confPath = utils::getProgramPath() + "/global.ini";
    EXPECT_EQ(configManager::SUCCESS, config.loadINIConf(confPath));

    auto &parser = parserManager::getInstance();
    // 解析域名
    for (auto &input : config.getInput()) {
        auto &result = parser.parseOne(input);
        ASSERT_TRUE(result.size() > 0);
    }
}

int main(int argc, char **argv) { return xtest_start_test(argc, argv); }
