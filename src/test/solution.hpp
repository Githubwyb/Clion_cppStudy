/**
 * @file solution.hpp
 * @brief 算法头文件
 * @author wangyubo
 * @version v0.0.1
 * @date 2021-03-17
 */

#include <stack>
#include <string>
using namespace std;

class Solution {
   public:
    void threeOrders1(TreeNode *root, vector<vector<int>> &result) {
        if (root == nullptr) return;
        result[0].emplace_back(root->val);
        threeOrders1(root->left, result);
        result[1].emplace_back(root->val);
        threeOrders1(root->right, result);
        result[2].emplace_back(root->val);
    }
    /**
     *
     * @param root TreeNode类 the root of binary tree
     * @return int整型vector<vector<>>
     */
    vector<vector<int>> threeOrders(TreeNode *root) {
        // write code here
        vector<vector<int>> result = {{}, {}, {}};
        threeOrders1(root, result);
        return result;
    }
};
