/**
 * @file test_algorithm.cpp
 * @brief 算法测试程序
 * @author wangyubo
 * @version v0.0.1
 * @date 2021-03-17
 */

#include <vector>

#include "log.hpp"
#include "solution.hpp"
#include "xtest/xtest.h"

using namespace std;

//  完成使用场景的测试
TEST(test, scene) {
    Solution so;
    EXPECT_EQ(so.isValid("{{(([[]]))}}"), true);
}

