/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include "log.hpp"

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    string originStr;
    int m = 0;
    cin >> originStr >> m;

    int result = 0;
    for (char item = 'a'; item <= 'z'; item++) {
        vector<int> pos;
        //提取位置
        for (int i = 0; i < originStr.length(); ++i) {
            if (originStr[i] == item) {
                pos.emplace_back(i);
            }
        }

        vector<vector<int>> dp(pos.size(), vector<int>(pos.size(), 0));
        int maxLen = 0;
        for (int len = 2; len <= pos.size(); ++len) {
            for (int i = 0; i + len - 1 < pos.size(); ++i) {
                dp[i][i + len - 1] = dp[i + 1][i + len - 2] + pos[i + len - 1] - pos[i] - len + 1;
                if (dp[i][i + len - 1] <= m) {
                    maxLen = len;
                }
            }
        }
        result = max(result, maxLen);
    }
    cout << result << endl;

    return 0;
}
