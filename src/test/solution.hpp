/**
 * @file solution.hpp
 * @brief 算法头文件
 * @author wangyubo
 * @version v0.0.1
 * @date 2021-03-17
 */

#include <algorithm>
#include <vector>
#include <unordered_map>
#include <string>

using namespace std;

class Solution {
   public:
    int getLongestPalindrome(string A, int n) {
        // write code here
        int result = 0;
        for (int i = 0; i < n; i++) {
            // think every char as mid, caculate the same len
            // 1234321
            int j = 0;
            for (j = 1; j < (i + 1) && j < (n - i); ++j) {
                if (A[i + j] != A[i - j]) {
                    int len = (j - 1) * 2 + 1;
                    result = result > len ? result : len;
                    break;
                }
            }
            if (j == (i + 1) || j == (n - i)) {
                int len = (j - 1) * 2 + 1;
                result = result > len ? result : len;
            }
            // think every char as left first, caculate the same len
            // 123321
            for (j = 0; j < (i + 1) && j < (n - i - 1); ++j) {
                if (A[i + j + 1] != A[i - j]) {
                    int len = j * 2;
                    result = result > len ? result : len;
                    break;
                }
            }
            if (j == (i + 1) || j == (n - i - 1)) {
                int len = j * 2;
                result = result > len ? result : len;
            }
        }
        return result;
    }
};
