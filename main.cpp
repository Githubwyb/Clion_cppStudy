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
    cin >> n;
    cin >> m;
    cin >> c;

    int count[51] = {0};
    bool colorNo[51] = {false};
    vector<vector<int>> line;
    for (int i = 0; i < n; ++i) {
        int colorN = 0;
        cin >> colorN;
        vector<int> one;
        if (i >= m) {
            for (auto tmp : line[i - m]) {
                count[tmp]--;
            }
        }
        for (int j = 0; j < colorN; ++j) {
            int color = 0;
            cin >> color;
            if (count[color] == 1) {
                colorNo[color] = true;
            }
            count[color]++;
            one.emplace_back(color);
        }
        line.emplace_back(one);
    }

    for (int k = 0; k < m; ++k) {
        for (auto tmp : line[n + k - m]) {
            count[tmp]--;
        }
        for (auto tmp : line[k]) {
            if (count[tmp] == 1) {
                colorNo[tmp] = true;
            }
            count[tmp]++;
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
