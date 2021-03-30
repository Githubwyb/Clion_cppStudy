/**
 * @file solution.hpp
 * @brief 算法头文件
 * @author wangyubo
 * @version v0.0.1
 * @date 2021-03-17
 */

#include <algorithm>
#include <list>
#include <vector>

using namespace std;

class Solution {
   public:
    /**
     *
     * @param numbers int整型vector
     * @param target int整型
     * @return int整型vector
     */
    vector<int> twoSum(vector<int>& numbers, int target) {
        // write code here
        int len = numbers.size();
        for (int i = 0; i < len - 1; ++i) {
            int tg = target - numbers[i];
            for (int j = i + 1; j < len; ++j) {
                if (tg == numbers[j]) {
                    return {i + 1, j + 1};
                }
            }
        }
        return {};
    }
};
