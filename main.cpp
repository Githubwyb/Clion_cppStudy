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
#include "log.hpp"

using namespace std;

/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <memory>
#include <iostream>
#include <algorithm>
#include <cstring>
#include "log.hpp"

using namespace std;


struct TreeNode {
	int val;
	struct TreeNode *left;
	struct TreeNode *right;
	explicit TreeNode(int x) :
			val(x), left(nullptr), right(nullptr) {
	}
};
class Solution {
public:
    //递归
    static void Mirror1(TreeNode *pRoot) {
        if (pRoot == nullptr) {
            return;
        }

        TreeNode *pTmp = pRoot->right;
        pRoot->right = pRoot->left;
        pRoot->left = pTmp;

        Mirror1(pRoot->right);
        Mirror1(pRoot->left);
    }

    //非递归
    static void Mirror(TreeNode *pRoot) {
        if (pRoot == nullptr) {
            return;
        }

        queue<TreeNode *> myQueue;
        myQueue.push(pRoot);
        while (myQueue.empty()) {
            TreeNode *pTmp = myQueue.front();
            myQueue.pop();
            if (pTmp == nullptr) {
                continue;
            }

            TreeNode *pTmp1 = pTmp->right;
            pTmp->right = pTmp->left;
            pTmp->left = pTmp1;

            myQueue.push(pTmp->right);
            myQueue.push(pTmp->left);
        }
    }
};

int main(int argC, char *arg[]) {
    return 0;
}
