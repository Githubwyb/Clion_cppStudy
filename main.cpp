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

class Solution {
   public:
    static int FindGreatestSumOfSubArray(vector<int> array) {
        int maxTail = -1;
        int max = 0x80000000;
        for (auto &tmp : array) {
            maxTail = (maxTail + tmp) > tmp ? (maxTail + tmp) : tmp;
            max = max < maxTail ? maxTail : max;
        }
        return max;
    }
};

int main() {
    LOG_DEBUG("%d", Solution::FindGreatestSumOfSubArray(
                        {1, -2, 3, 10, -4, 7, 2, -5}));
}