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
    vector<vector<int>> tv = {{1,3,5,9}, {8,1,3,4}, {5,0,6,1}, {8,8,4,0}};
    int tmp = so.minPathSum(tv);
    LOG_INFO("tmp %d");
    EXPECT_EQ(tmp, 12);
//    EXPECT_EQ(so.getLongestPalindrome("baabccc",7), 4);
//    EXPECT_EQ(so.getLongestPalindrome("ccbcabaabba",11), 4);
}

