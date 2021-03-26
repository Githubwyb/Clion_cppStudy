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
     * @brief 转递归函数实现，a传入指针方便递归
     *
     * @param a 头指针
     * @param n 数组长度
     * @param K K值
     *
     * @return 返回第K大的值
     */
    int findKth(int *a, int n, int K) {
        assert(K > 0 && n > 0);
        assert(n >= K);
        // use the first item to be the axis
        int axis = a[0];
        int left = 0;
        int right = n - 1;
        // First, the right run, when right > axis, left = right, then left run
        // when left < axis, right = left, then right run
        // equal won't handle
        // with this way, we can avoid swap item too frequently
        bool isRightRun = true;
        while (left != right) {
            // right run
            if (isRightRun) {
                // right less than axis
                if (a[right] <= axis) {
                    right--;
                    continue;
                }
                a[left] = a[right];
                isRightRun = false;
                continue;
            }
            // left run
            // left bigger than axis
            if (a[left] >= axis) {
                left++;
                continue;
            }
            a[right] = a[left];
            isRightRun = true;
        }
        a[left] = axis;
        // if left is K
        if ((left + 1) == K) return axis;
        // left bigger than K, use right part to sort
        if ((left + 1) < K) return findKth(a + left + 1, n - 1 - left, K - left - 1);
        // left bigger than K, use right part to sort
        return findKth(a, left, K);
    }

    int findKth(vector<int> a, int n, int K) {
        // write code here
        /* 思路1
         * 小根堆实现
         *
         * 思路2（当前）
         * 快排思想
         */
        return findKth(&a[0], n, K);
    }
};
