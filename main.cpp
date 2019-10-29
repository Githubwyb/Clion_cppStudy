/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <string>
#include <vector>
#include <stack>
#include <memory>
#include <iostream>
#include <cstring>
#include "log.hpp"

using namespace std;

 struct TreeNode {
     int val;
     TreeNode *left;
     TreeNode *right;
     explicit TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};
class Solution {
public:
    static TreeNode* reConstructBinaryTree(vector<int> pre,vector<int> vin) {
        if (pre.size() != vin.size()) {
            return nullptr;
        }
        return reConstructBinaryTree(pre.begin().base(), vin.begin().base(), pre.size());
    }

private:
    static TreeNode* reConstructBinaryTree(const int *pre, const int *vin, int size) {
        if (size == 0) {
            return nullptr;
        }

        //头结点从pre第一个取
        auto head = new TreeNode(pre[0]);
        //找到和vin中和pre[0]相等的点，左边为左子树，右边为右子树
        for (int i = 0; i < size; ++i) {
            if (vin[i] == pre[0]) {
                head->left = reConstructBinaryTree(pre + 1, vin, i);
                head->right = reConstructBinaryTree(pre + i + 1, vin + i + 1, size - i - 1);
                break;
            }
        }

        return head;
    }
};

void in_order_traversal(TreeNode *root) {
    PRINT("%d, ", root->val);
    if (root->left != nullptr)
        in_order_traversal(root->left);
    if (root->right != nullptr)
        in_order_traversal(root->right);
    delete root;
}

int main(int argC, char *arg[]) {
    in_order_traversal(Solution::reConstructBinaryTree({1, 2, 4, 7, 3, 5, 6, 8}, {4, 7, 2, 1, 5, 3, 8, 6}));
    return 0;
}
