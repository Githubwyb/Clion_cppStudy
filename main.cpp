/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <algorithm>
#include <string>
#include <vector>

#include "log.hpp"
//#include "bugReport.hpp"

using namespace std;

class Solution {
   public:
    static void FindNumsAppearOnce(vector<int> data, int* num1, int* num2) {
        int nor = 0;
        // 先把所有数字异或，结果为两个单独出现的数字的异或
        for (auto &tmp : data) {
            nor ^= tmp;
        }

        int bitNum = 0;
        // 找到第一个不同的位
        for (int i = 0; i < 32; ++i) {
            if (nor & 0x01) {
                bitNum = i;
                break;
            }
            nor >>= 1;
        }
        bitNum = 1 << bitNum;
        
        // 根据此位将所有数字分成两组进行异或，最终得到结果
        *num1 = 0;
        *num2 = 0;
        for (auto &tmp : data) {
            if (tmp & bitNum) {
                *num1 ^= tmp;
                continue;
            }

            *num2 ^= tmp;
        }
    }
};

int main() {
    // (void)BugReportRegister("run", ".", nullptr, nullptr);
    int a = -1;
    int b = -1;
    Solution::FindNumsAppearOnce({2, 4, 3, 6, 3, 2, 5, 5}, &a, &b);
    LOG_DEBUG("%d %d", a, b);
    return 0;
}
