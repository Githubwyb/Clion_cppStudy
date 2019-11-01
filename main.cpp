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
#include <algorithm>
#include <cstring>
#include "log.hpp"

using namespace std;

class Solution {
public:
    static double Power(double base, int exponent) {
        unsigned int tmp = abs(exponent);
        double r = 1.0;
        while (tmp) {
            if (tmp & (unsigned) 1) r *= base;
            base *= base;
            tmp >>= (unsigned) 1;
        }
        return exponent < 0 ? 1 / r : r;
    }
};

int main(int argC, char *arg[]) {
    return 0;
}
