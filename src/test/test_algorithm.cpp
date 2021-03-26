/**
 * @file test_algorithm.cpp
 * @brief 算法测试程序
 * @author wangyubo
 * @version v0.0.1
 * @date 2021-03-17
 */

#include "log.hpp"
#include "solution.hpp"
#include "xtest/xtest.h"

using namespace std;

//  完成使用场景的测试
TEST(test, scene) {
    Solution so;
    vector<int> test1 = {0, 1, 2, 3, 4};
    EXPECT_EQ(so.search(test1, 2), 2);
}

