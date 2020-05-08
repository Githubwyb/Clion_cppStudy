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
    static int GetNumberOfK(vector<int> data, int k) {
        auto tmp =
            equal_range(data.begin(), data.end(), k,
                        [](int a, int b) { return (a > b); });
        return tmp.second - tmp.first;
    }
};

int main() {
    // (void)BugReportRegister("run", ".", nullptr, nullptr);
    auto tmp = Solution::GetNumberOfK({5, 4, 3, 3, 3, 2, 1}, 3);
    LOG_DEBUG("%d", tmp);
    return 0;
}
