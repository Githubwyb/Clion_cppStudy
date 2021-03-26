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
     * 代码中的类名、方法名、参数名已经指定，请勿修改，直接返回方法规定的值即可
     *
     * 如果目标值存在返回下标，否则返回 -1
     * @param nums int整型vector
     * @param target int整型
     * @return int整型
     */
    int search(vector<int>& nums, int target) {
        // write code here
        int len = nums.size();
        int left = 0;
        int right = len - 1;

        // check array boundary
        if (len == 0 || target < nums.front() || target > nums[right])
            return -1;
        if (target == nums.front()) return 0;
        // left move faster than mid and mid is more close to left, finally left
        // will be equal or bigger than right
        while (left < right) {
            int mid = (right + left) >> 1;  // 0-3 mid 1, more close to left
            // fast convergence, mid +- 1
            if (target > nums[mid])
                left = mid + 1;  // left - 1 must less than target
            else
                right = mid - 1;  // right + 1 maybe equal target
        }
        // if left bigger than right, judge left only
        // if left == right, judge left and right + 1
        if (nums[left] == target) return left;
        if (left == right && nums[++left] == target) return left;
        return -1;
    }
};
