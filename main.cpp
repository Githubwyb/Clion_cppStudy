/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include "log.hpp"

#include <iostream>
#include <vector>

using namespace std;

int main() {
    int n, m, c;
    cin >> n >> m >> c;

    int last[51] = {0};     //上一次出现位置
    int first[51] = {0};    //第一次出现位置
    bool colorNo[51] = {false};

    for (int i = 0; i < n; ++i) {
        int colorN = 0;
        cin >> colorN;
        for (int j = 0; j < colorN; ++j) {
            int color = 0;
            cin >> color;
            if (first[color] == 0) {
                first[color] = i;
            } else if (i - last[color] < m) {
                colorNo[color] = true;
            }
            last[color] = i;
        }
    }

    for (int k = 1; k < 51; ++k) {
        if (first[k] != 0 && last[k] + m - 1 - n >= first[k]) {
            colorNo[k] = true;
        }
    }

    int result = 0;
    for (int l = 0; l < 51; ++l) {
        if (colorNo[l]) {
            result++;
        }
    }
    cout << result;

    return 0;
}
