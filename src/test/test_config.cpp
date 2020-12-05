#include <assert.h>
#include <errno.h>
#include <stdio.h>

#include <iostream>

#include "log.hpp"
#include "utils.hpp"
#include "xtest/xtest.h"

using namespace std;

//  完成使用场景的测试
TEST(test, scene) {
    // 解析命令行参数，将用命令行参数替换全局配置
    EXPECT_EQ("/mnt/e/work/github_files/Clion_cppStudy/build", utils::getProgramPath());
}

int main(int argc, char **argv) { return xtest_start_test(argc, argv); }
