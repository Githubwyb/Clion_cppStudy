/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include "log.hpp"

#include <iostream>
#include <memory>

using namespace std;

int *minNum;
int coin[] = {1, 4, 16, 64};

int fun(int num) {
    for (int j = 1; j <= num; ++j) {
        int minValue = 99999;
        for (int i = 0; i < 4; ++i) {
            int tmp = 999999;
            if (j >= coin[i]) {
                tmp = minNum[j - coin[i]] + 1;
            }
            minValue = min(tmp, minValue);
        }
        minNum[j] = minValue;
    }
    return minNum[num];
}

int main() {
    int N = 0;
    cin >> N;

    minNum = new int[1025];
    minNum[0] = 0;
    fun(1024 - N);
    for (int i = 0; i < 1025 - N; i++) {
        LOG_DEBUG("%d: %d", i, minNum[i]);
    }
    delete minNum;
    return 0;
}
