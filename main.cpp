/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <queue>
#include <stack>
#include <vector>

#include "log.hpp"

using namespace std;

class Solution {
   public:
    static bool IsPopOrder(vector<int> pushV, vector<int> popV) {
        auto size = pushV.size();
        if (size == 0 || size != popV.size()) {
            return false;
        }

        stack<int> test;
        unsigned int j = 0;
        unsigned int i = 0;
        while (i < size) {
            test.push(pushV[i++]);
            while (!test.empty() && test.top() == popV[j]) {
                test.pop();
                j++;
            }
        }

        return test.empty();
    }
};

int main(int argC, char *arg[]) {
    LOG_DEBUG("%d", Solution::IsPopOrder({1, 2, 3, 4, 5}, {4, 5, 3, 2, 1}));
    return 0;
}
