/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

#include "log.hpp"
#include "bugReport.hpp"

using namespace std;

class Solution {
   public:
    static int countWays(int n) {
        int coins[4] = {1, 5, 10, 25};
        // 初始化4 x n的数组，初始化为1
        vector<vector<int>> data(4, vector<int>(n + 1, 1));
        // 从i = 0开始
        for (int i = 1; i < 4; i++) {
            // sum累加
            for (int j = 0; j <= n; j++) {
                // d[n][sum] & = d[n - 1][sum - 0 * coins[n]] +
                //               d[n - 1][sum - 1 * coins[n]] +
                //               ... +
                //               d[n - 1][sum - sum / coins[n] * coins[n]]
                data[i][j] = 0;
                for (int k = 0; k <= j / coins[i]; k++) {
                    data[i][j] += data[i - 1][j - k * coins[i]];
                    data[i][j] %= 1000000007;
                }
            }
        }
        return data[3][n];
    }
};

int main() {
    (void)BugReportRegister("run", ".", nullptr, nullptr);
    //for (int i = 0; i < 10; i++) {
    LOG_DEBUG("%d %d", 11746, Solution::countWays(11746));
    //}
    return 0;
}
