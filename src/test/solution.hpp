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
     * @param prices int整型vector
     * @return int整型
     */
    int maxProfit(vector<int>& prices) {
        // write code here
		int result = 0;
        int nMin = prices.front();   // 前n个数中最小的
        // 以每个点为卖出点遍历
        for (auto &tmp : prices) {
            // 找到卖出点前最小的作为买入点
            nMin = tmp < nMin ? tmp : nMin;
            int earnings = tmp - nMin;
            result = result < earnings ? earnings : result;
        }
        return result;
    }
};
