/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <string>
#include <vector>
#include <stack>
#include <memory>
#include <iostream>
#include <cstring>
#include "log.hpp"

using namespace std;

class Solution {
public:
    static int rectCover(int number) {
        if (number == 0) {
            return 0;
        }
        int f = 1;
        int g = 1;
        for (int i = 0; i < number; ++i) {
            g += f;
            f = g - f;
        }

        return f;
    }
};

int main(int argC, char *arg[]) {
    int n = 0;
    LOG_DEBUG("n %d, result %d", n, Solution::rectCover(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, Solution::rectCover(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, Solution::rectCover(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, Solution::rectCover(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, Solution::rectCover(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, Solution::rectCover(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, Solution::rectCover(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, Solution::rectCover(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, Solution::rectCover(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, Solution::rectCover(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, Solution::rectCover(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, Solution::rectCover(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, Solution::rectCover(n));
    n++;
    return 0;
}
