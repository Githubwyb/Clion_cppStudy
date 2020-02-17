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
    // map统计
    static int MoreThanHalfNum_Solution1(vector<int> numbers) {
        int size = numbers.size();
        map<int, int> count;
        for (auto &tmp : numbers) {
            if (count.count(tmp) == 0) {
                count[tmp] = 1;
                continue;
            }
            count[tmp]++;
        }

        for (auto it = count.begin(); it != count.end(); it++) {
            if (it->second > size / 2) {
                return it->first;
            }
        }
        return 0;
    }

    //巧解，排除相同的数，留下的即为所求
    static int MoreThanHalfNum_Solution2(vector<int> numbers) {
        int size = numbers.size();

        int times = 0;
        int result = 0;
        for (auto &tmp : numbers) {
            if (times == 0) {
                result = tmp;
            }

            if (result == tmp) {
                times++;
                continue;
            }

            times--;
        }

        if (count(numbers.begin(), numbers.end(), result) > size / 2) {
            return result;
        }

        return 0;
    }

    //排序，查中间
    static int MoreThanHalfNum_Solution3(vector<int> numbers) {
        sort(numbers.begin(), numbers.end());
        int size = numbers.size();
        if (count(numbers.begin(), numbers.end(), numbers[size / 2]) >
            size / 2) {
            return numbers[size / 2];
        }
        return 0;
    }
};

int main(int argC, char *arg[]) {
    LOG_DEBUG("%d",
              Solution::MoreThanHalfNum_Solution1({1, 2, 3, 2, 2, 2, 5, 4, 2}));
    LOG_DEBUG("%d",
              Solution::MoreThanHalfNum_Solution2({1, 2, 3, 2, 2, 2, 5, 4, 2}));
    LOG_DEBUG("%d",
              Solution::MoreThanHalfNum_Solution3({1, 2, 3, 2, 2, 2, 5, 4, 2}));
    return 0;
}
