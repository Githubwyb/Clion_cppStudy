/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <string>
#include <vector>

#include "log.hpp"
#include "bugReport.hpp"

using namespace std;

class Coins {
   public:
    static int countWays(int n) {
        int coins[] = {1, 5, 10, 25};
        // 初始化为1，只有1种硬币时，所有都只有一种方案
        vector<int> dp(n + 1, 1);
        // 从2种硬币开始
        for (unsigned int i = 1; i < sizeof(coins) / sizeof(coins[0]); i++) {
            // dp[i][j] = dp[i - 1][j];
            for (int j = coins[i]; j <= n; j++) {
                dp[j] += dp[j - coins[i]];
                dp[j] %= 1000000007;
            }
        }
        return dp[n];
    }
};

int main() {
    (void)BugReportRegister("run", ".", nullptr, nullptr);
    for (int i = 0; i < 15; i++) {
        LOG_DEBUG("%d %d", i, Coins::countWays(i));
    }
    int num = 35837;
    LOG_DEBUG("%d %d", num, Coins::countWays(num));
    return 0;
}

