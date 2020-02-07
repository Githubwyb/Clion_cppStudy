/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <vector>
#include <queue>

#include "log.hpp"

using namespace std;

typedef struct TreeNode {
    int val;
    struct TreeNode *left;
    struct TreeNode *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *leftNode, TreeNode *rightNode)
        : val(x), left(leftNode), right(rightNode) {}
} TreeNode;

class Solution {
   public:
    static vector<int> PrintFromTopToBottom(TreeNode *root) {
        if (root == nullptr) {
            return {};
        }

        queue<TreeNode *> qTmp;
        qTmp.push(root);
        vector<int> result;
        while (!qTmp.empty())
        {
            auto tmp = qTmp.front();
            qTmp.pop();
            result.push_back(tmp->val);

            if (tmp->left != nullptr) {
                qTmp.push(tmp->left);
            }

            if (tmp->right != nullptr) {
                qTmp.push(tmp->right);
            }
        }
        return result;
    }
};

void print_vector(const vector<int> &result) {
    LOG_DEBUG("vector is");
    for (auto &tmp : result) {
        PRINT("%d ", tmp);
    }
    PRINT("\r\n");
}

int main(int argC, char *arg[]) {
    TreeNode testTree(
        1,
        new TreeNode(2, new TreeNode(4, new TreeNode(8), new TreeNode(9)),
                     new TreeNode(5, new TreeNode(10), nullptr)),
        new TreeNode(3, new TreeNode(6), new TreeNode(7)));
    vector<int> result = Solution::PrintFromTopToBottom(&testTree);
    print_vector(result);
    return 0;
}
