/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <string>
#include <vector>
#include <cmath>
#include "log.hpp"

using namespace std;

class Solution {
public:
    int jumpFloorII(int number) {
        if (number == 0) {
            return 0;
        }

        return 1 << (number - 1);
    }
};

int main(int argC, char *arg[]) {
    Solution a;
    for (int i = 0; i < 10; ++i) {
        LOG_DEBUG("i %d, num %d", i, a.jumpFloorII(i));
    }
    return 0;
}
