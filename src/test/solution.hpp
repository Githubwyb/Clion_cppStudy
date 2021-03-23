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
        stack<TreeNode *> order;  // 顺序层
        bool isReverse = false;
        // 先放一个根节点
        order.emplace(root);
        while (!order.empty()) {
            vector<TreeNode *> current;
            // 顺序层插入，左节点开始
            int stackSize = order.size();
            while (stackSize--) {
                TreeNode *tmp = order.top();
                order.pop();
                // 当前层插入
                current.emplace_back(tmp);
            }
            vector<int> floor;
            for (auto &tmp : current) {
                if (isReverse) {
                    if (tmp->right) order.emplace(tmp->right);
                    if (tmp->left) order.emplace(tmp->left);
                } else {
                    if (tmp->left) order.emplace(tmp->left);
                    if (tmp->right) order.emplace(tmp->right);
                }
                floor.emplace_back(tmp->val);
            }
            isReverse = !isReverse;
            if (!floor.empty()) result.emplace_back(floor);
        }
        return result;
    }
};
