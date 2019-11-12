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
#include <algorithm>
#include <cstring>
#include <queue>
#include "log.hpp"

using namespace std;

struct TreeNode {
	int val;
	struct TreeNode *left;
	struct TreeNode *right;
	explicit TreeNode(int x) :
			val(x), left(nullptr), right(nullptr) {
	}

    explicit TreeNode(int x, TreeNode *pLeft, TreeNode *pRight) :
    val(x), left(pLeft), right(pRight) {}
};

class Solution {
public:
    static bool HasSubtree(TreeNode* pRoot1, TreeNode* pRoot2)
    {
        //排除有一个为空的情况
        if (pRoot1 == nullptr || pRoot2 == nullptr) {
            return false;
        }

        return HasSubtree1(pRoot1, pRoot2);
    }

private:
    static bool HasSubtree1(TreeNode* pRoot1, TreeNode* pRoot2)
    {
        //有一个为空时
        if (pRoot1 == nullptr || pRoot2 == nullptr) {
            //子结构为空证明遍历完了，否则不匹配
            return pRoot2 == nullptr;
        }

        //当有值匹配，判断左右是否也匹配
        if (pRoot1->val == pRoot2->val &&
            HasSubtree1(pRoot1->right, pRoot2->right) &&
            HasSubtree1(pRoot1->left, pRoot2->left)) {
            return true;
        }

        //当上述不匹配，判断左右子树是否和子结构匹配
        return HasSubtree1(pRoot1->right, pRoot2) || HasSubtree1(pRoot1->left, pRoot2);
    }
};

//广度优先打印节点
void printNodesWidthFirst(TreeNode *node) {
    queue<TreeNode *> myQueue;
    myQueue.push(node);
    while (myQueue.size() > 0) {
        TreeNode *pTmp = myQueue.front();
        myQueue.pop();
        PRINT("%d ", pTmp->val);

        myQueue.push(pTmp->left);
        myQueue.push(pTmp->right);
    }
    PRINT("\r\n");
}

int main(int argC, char *arg[]) {
    LOG_DEBUG("Start");
    TreeNode a(8, new TreeNode(8, new TreeNode(9, nullptr, nullptr), new TreeNode(2, nullptr, nullptr)),
                    new TreeNode(7, new TreeNode(4, nullptr, nullptr), new TreeNode(7, nullptr, nullptr)));
    TreeNode b(8, new TreeNode(9, nullptr, nullptr), new TreeNode(2, nullptr, nullptr));
    auto result = Solution::HasSubtree(&a, &b);
    LOG_DEBUG("%d", result);
    return 0;
}
