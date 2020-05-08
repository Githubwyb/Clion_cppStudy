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
    // Parameters:
    //        numbers:     an array of integers
    //        length:      the length of array numbers
    //        duplication: (Output) the duplicated number in the array number
    // Return value:       true if the input is valid, and there are some
    // duplications in the array number
    //                     otherwise false
    static bool duplicate(int numbers[], int length, int* duplication) {
        for (int i = 0; i < length; i++) {
            // 获取原始数据
            auto tmp = numbers[i] < length ? numbers[i] : numbers[i] - length;
            // 如果对应位小于length证明没有被访问过
            if (numbers[tmp] < length) {
                // 访问它，增加length
                numbers[tmp] += length;
                continue;
            }
            // 访问过，就是它了
            *duplication = tmp;
            return true;
        }
        return false;
    }

    // Parameters:
    //        numbers:     an array of integers
    //        length:      the length of array numbers
    //        duplication: (Output) the duplicated number in the array number
    // Return value:       true if the input is valid, and there are some
    // duplications in the array number
    //                     otherwise false
    static bool duplicate1(int numbers[], int length, int* duplication) {
        vector<bool> count(length, false);
        for (int i = 0; i < length; i++) {
            if (count[numbers[i]]) {
                *duplication = numbers[i];
                return true;
            }
            count[numbers[i]] = true;
        }
        return false;
    }
};

int main() {
    // (void)BugReportRegister("run", ".", nullptr, nullptr);
    int test[] = {2, 4, 3, 1, 4};
    int p = -1;
    auto tmp = Solution::duplicate(test, sizeof(test) / sizeof(test[0]), &p);
    LOG_DEBUG("%d %d", tmp, p);
    return 0;
}
