/**
 * @file solution.hpp
 * @brief 算法头文件
 * @author wangyubo
 * @version v0.0.1
 * @date 2021-03-17
 */

#include <list>
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
        list<TreeNode *> floor;  // 每一层
        bool isReverse = false;
        // 先放一个根节点
        floor.emplace_front(root);
        while (!floor.empty()) {
            vector<int> current;
            int listSize = floor.size();
            while (listSize--) {
                if (!isReverse) {
                    // 顺序层从前向后遍历
                    TreeNode *tmp = floor.front();
                    floor.pop_front();
                    current.emplace_back(tmp->val);
                    // 插入从后插入，左节点开始
                    if (tmp->left) floor.emplace_back(tmp->left);
                    if (tmp->right) floor.emplace_back(tmp->right);
                } else {
                    // 倒序层从后向前遍历
                    TreeNode *tmp = floor.back();
                    floor.pop_back();
                    current.emplace_back(tmp->val);
                    // 插入从前插入，右节点开始
                    if (tmp->right) floor.emplace_front(tmp->right);
                    if (tmp->left) floor.emplace_front(tmp->left);
                }
            }
            if (!current.empty()) result.emplace_back(current);
            isReverse = !isReverse;
        }
        return result;
    }
};
