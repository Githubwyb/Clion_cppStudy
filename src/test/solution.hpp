/**
 * @file solution.hpp
 * @brief 算法头文件
 * @author wangyubo
 * @version v0.0.1
 * @date 2021-03-17
 */

#include <stack>
#include <string>
#include <vector>

using namespace std;

struct TreeNode {
    int val;
    struct TreeNode *left;
    struct TreeNode *right;
};

class Solution {
   public:
    /**
     *
     * @param root TreeNodeÀà
     * @return int整型vector<vector<>>
     */
    vector<vector<int>> zigzagLevelOrder(TreeNode *root) {
        // write code here
        if (root == nullptr) return {};
        vector<vector<int>> result;
        stack<TreeNode *> order;    // 顺序层
        stack<TreeNode *> reverse;  // 倒序层
        // 先放一个根节点
        order.emplace(root);
        while (!order.empty()) {
            vector<int> current;
            // 顺序层插入，左节点开始
            int stackSize = order.size();
            while (stackSize--) {
                TreeNode *tmp = order.top();
                order.pop();
                // 当前层插入
                current.emplace_back(tmp->val);
                if (tmp->left) reverse.emplace(tmp->left);
                if (tmp->right) reverse.emplace(tmp->right);
            }
            if (!current.empty()) result.emplace_back(current);
            current.clear();
            stackSize = reverse.size();
            while (stackSize--) {
                TreeNode *tmp = reverse.top();
                reverse.pop();
                // 当前层插入
                current.emplace_back(tmp->val);
                if (tmp->right) order.emplace(tmp->right);
                if (tmp->left)  order.emplace(tmp->left);
            }
            if (!current.empty()) result.emplace_back(current);
        }
        return result;
    }
};
