/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <algorithm>
#include <map>
#include <queue>
#include <stack>
#include <vector>

#include "log.hpp"

using namespace std;

class Solution {
   public:
    static bool VerifySquenceOfBST(vector<int> sequence) {
        return !sequence.empty() &&
               VerifySquenceOfBST(sequence.begin().base(), sequence.size());
    }

   private:
    static bool VerifySquenceOfBST(const int arr[], int length) {
        if (length <= 0) {
            return true;
        }

        bool flag = false;
        int result = length;
        int i = 0;
        for (i = 0; i < length - 1; i++) {
            //异或，当false时，最后一位要比前面大，否则进判断；true时最后一位要比前面小，否则进逻辑
            if (flag ^ (arr[i] > arr[length - 1])) {
                if (flag) {
                    //找到中间点后，出现小的，返回false
                    return false;
                }
                //中间点转判断
                flag = true;
                result = i;
            }
        }
        return (i == length - 1) ||
               (VerifySquenceOfBST(arr, result) &&
                VerifySquenceOfBST(arr + result, length - result - 1));
    }
};

int main(int argC, char *arg[]) {
    LOG_DEBUG("%d", Solution::VerifySquenceOfBST({1, 2, 3, 5, 6, 4}));
    return 0;
}
