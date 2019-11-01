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
#include <algorithm>
#include <cstring>
#include "log.hpp"

using namespace std;

class Solution {
public:
    static void reOrderArray(vector<int> &array) {
        stable_partition(array.begin(), array.end(),
                         [](const int &value) { return (value % 2 == 1); });
        return;
        vector<int> Odd;     //偶数队列
        int size = array.size();
        int j = 0;              //保存最后待放偶数的位置
        for (int i = 0; i < size; ++i) {
            if ((array[i] % 2) == 0) {
                //LOG_DEBUG("push %d", array[i]);
                Odd.emplace_back(array[i]);
                continue;
            }

            if (i != j) {
                //LOG_DEBUG("change %d to %d, value %d", j, i, array[i]);
                array[j] = array[i];
            }

            ++j;
        }

        memcpy(array.begin().base() + j, Odd.begin().base(), Odd.size() * sizeof(int));
    }
};

int main(int argC, char *arg[]) {
    vector<int> arr = {1, 2, 3, 3, 5, 6, 5, 4, 45, 54, 64};
    Solution::reOrderArray(arr);

    for (auto tmp : arr) {
        PRINT("%d,  ", tmp);
    }
    return 0;
}
