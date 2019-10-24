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
    int Fibonacci(int n) {
        if (n == 0 || m_result[n] != 0) {
            return m_result[n];
        }

        if (n == 1) {
            m_result[n] = 1;
        } else {
            m_result[n] = Fibonacci(n - 1) + Fibonacci(n - 2);
        }

        return m_result[n];
    }

private:
    int m_result[40] = {0};
};

int main(int argC, char *arg[]) {
    Solution a;
    int n = 0;
    LOG_DEBUG("n %d, result %d", n, a.Fibonacci(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.Fibonacci(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.Fibonacci(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.Fibonacci(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.Fibonacci(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.Fibonacci(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.Fibonacci(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.Fibonacci(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.Fibonacci(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.Fibonacci(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.Fibonacci(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.Fibonacci(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.Fibonacci(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.Fibonacci(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.Fibonacci(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.Fibonacci(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.Fibonacci(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.Fibonacci(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.Fibonacci(n));
    n++;
    LOG_DEBUG("n %d, result %d", n, a.Fibonacci(n));
    n++;
    return 0;
}
