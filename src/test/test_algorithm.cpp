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
    TreeNode root;
    root.val = 1;
    TreeNode mid1;
    mid1.val = 2;
    TreeNode mid2;
    mid2.val = 3;
    TreeNode last1;
    last1.val = 4;
    TreeNode last2;
    last2.val = 5;

    root.left = &mid1;
    root.right = &mid2;

    mid1.left = &last1;
    mid1.right = nullptr;
    mid2.left = nullptr;
    mid2.right = &last2;

    last1.right = nullptr;
    last1.left = nullptr;
    last2.right = nullptr;
    last2.left = nullptr;

    auto result = so.zigzagLevelOrder(&root);
    EXPECT_TRUE(result.size() == 3);
    EXPECT_TRUE(result[0].size() == 1);
    EXPECT_TRUE(result[1].size() == 2);
    EXPECT_TRUE(result[2].size() == 2);

    EXPECT_TRUE(result[0][0] == 1);
    EXPECT_TRUE(result[1][0] == 3);
    EXPECT_TRUE(result[1][1] == 2);
    EXPECT_TRUE(result[2][0] == 4);
    EXPECT_TRUE(result[2][1] == 5);
}

