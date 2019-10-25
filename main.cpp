/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <string>
#include <vector>
#include <iostream>
#include <cstring>
#include "log.hpp"

using namespace std;

class Solution {
public:
    int jumpFloor(int n) {
        if (n == 0) {
            return 0;
        }

        int f = 1, g = 1;
        while(n--) {
            g += f;     //从前一次到当前
            f = g - f;  //从前两次算到前一次
        }
        return f;
    }
};

int main(int argC, char *arg[]) {
    Solution a;
    int n = 0;
    LOG_DEBUG("n %d, result %d", n, a.jumpFloor(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.jumpFloor(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.jumpFloor(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.jumpFloor(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.jumpFloor(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.jumpFloor(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.jumpFloor(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.jumpFloor(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.jumpFloor(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.jumpFloor(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.jumpFloor(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.jumpFloor(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.jumpFloor(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.jumpFloor(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.jumpFloor(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.jumpFloor(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.jumpFloor(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.jumpFloor(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.jumpFloor(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.jumpFloor(n));
    n++;
    return 0;
}
