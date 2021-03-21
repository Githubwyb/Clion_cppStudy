/**
 * @file solution.hpp
 * @brief 算法头文件
 * @author wangyubo
 * @version v0.0.1
 * @date 2021-03-17
 */

#include <stack>
#include <string>
using namespace std;

class Solution {
   public:
    void merge(int A[], int m, int B[], int n) {
        /* 思路
         * 从尾到头遍历，最小拷贝次数
         */
        int posA = m - 1;
        int posB = n - 1;
        int posCurrent = m + n;
        while (posCurrent > 0) {
            if (posA < 0 || (posB >= 0 && A[posA] < B[posB])) {
                A[--posCurrent] = B[posB--];
            } else {
                A[--posCurrent] = A[posA--];
            }
        }
    }
};
