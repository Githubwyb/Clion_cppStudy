/**
 * @file solution.hpp
 * @brief 算法头文件
 * @author wangyubo
 * @version v0.0.1
 * @date 2021-03-17
 */

#include <list>
#include <vector>
#include <algorithm>

using namespace std;

class Solution {
   public:
    int findKth(vector<int> a, int n, int K) {
        // write code here
        /* 思路
         * 小根堆实现
         */
        // 构建小根堆
        auto cmp = [] (int &a1, int &b) { return a1 > b; };
        make_heap(a.begin(), a.begin() + K, cmp);
        for (int i = K; i < n; ++i) {
            // 如果第n个值大于堆顶，堆顶移出，插入新值
            if (a[i] > a.front()) {
                swap(a[K], a[i]);
                pop_heap(a.begin(), a.begin() + K + 1, cmp);
            }
        }
        return a.front();
    }
};
