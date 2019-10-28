/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <string>
#include <vector>
#include <stack>
#include <memory>
#include <iostream>
#include <cstring>
#include "log.hpp"

using namespace std;

class Solution {
public:
    int minNumberInRotateArray(vector<int> rotateArray) {
        if (rotateArray.empty()) {
            return 0;
        }

        int left = 0;
        int right = rotateArray.size() - 1;

        //另类的二分查找，比左边的大，左边等，比右边的小，右边等
        while (true) {
            int tmp = (left + right) / 2;

            if (tmp == left) {
                break;
            }

            if (rotateArray[tmp] >= rotateArray[left]) {
                left = tmp;
            } else {
                right = tmp;
            }
        }

        if (rotateArray[left] < rotateArray[right]) {
            return rotateArray[left];
        }

        return rotateArray[right];
    }
};

int main(int argC, char *arg[]) {
    Solution a;
    vector<int> arr = {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 7, 8, 9, 1, 2, 3, 4, 5};
    LOG_DEBUG("%d", a.minNumberInRotateArray(arr));
    return 0;
}
