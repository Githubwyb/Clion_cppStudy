/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <algorithm>
#include <iostream>
#include <vector>
#include <queue>
#include "log.hpp"

using namespace std;

typedef struct TreeNode {
    int val;
    struct TreeNode *left;
    struct TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
    TreeNode(int x, TreeNode *pLeft, TreeNode *pRight)
        : val(x), left(pLeft), right(pRight) {}
} TreeNode;

class Solution {
   public:
    // 递归实现
    static int TreeDepth1(TreeNode *pRoot) {
        if (pRoot == nullptr) {
            return 0;
        }
        return max(TreeDepth1(pRoot->left) + 1, TreeDepth1(pRoot->right) + 1);
    }

    // 非递归实现，广度优先遍历
    static int TreeDepth2(TreeNode *pRoot) {
        if (pRoot == nullptr) {
            return 0;
        }

        int maxDeep = 0;
        queue<TreeNode *> traversal;
        traversal.push(pRoot);
        while(!traversal.empty()) {
            int length = traversal.size();
            for (int i = 0; i < length; i++)
            {
                auto &tmp = traversal.front();
                if (tmp->left != nullptr) traversal.push(tmp->left);
                if (tmp->right != nullptr) traversal.push(tmp->right);
                traversal.pop();
            }

            maxDeep++;
        }
        return maxDeep;
    }
};

int main() {
    TreeNode root(10);
    LOG_DEBUG("%d", Solution::TreeDepth2(&root));
}
