/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <algorithm>
#include <iostream>
#include <vector>

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
    static vector<vector<int>> FindPath(TreeNode *root, int expectNumber) {
        if (root == nullptr) {
            return {};
        }
        vector<vector<int>> result;
        vector<int> road;
        FindPath(root, expectNumber, result, road);
        sort(result.begin(), result.end(), comp);
        return result;
    }

   private:
    static bool comp(vector<int> a, vector<int> b) {
        return a.size() > b.size();
    }

    static void FindPath(TreeNode *root, int expectNumber,
                         vector<vector<int>> &result, vector<int> &road) {
        if (root == nullptr) {
            return;
        }

        // sum增加，road放入此节点
        expectNumber -= root->val;
        road.emplace_back(root->val);

        if (root->right == nullptr && root->left == nullptr &&
            expectNumber == 0) {
            // 查到根部，判断是否相等，相等则存入结果
            result.emplace_back(road);
        }

        // 递归判断子节点
        FindPath(root->right, expectNumber, result, road);
        FindPath(root->left, expectNumber, result, road);

        // 出递归需要还原路径
        road.pop_back();
    }
};

int main() {
    TreeNode root(10);
    vector<vector<int>> result = Solution::FindPath(&root, 10);
    for (auto &vTmp : result) {
        for (auto &tmp : vTmp) {
            PRINT("%d ", tmp);
        }
        PRINT("\r\n");
    }
}
