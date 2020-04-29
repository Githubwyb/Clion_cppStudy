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
    static int Sum_Solution(int n) {
        int sum = 0;
        int num = 1 + n;
        unsigned int bitNum = 0x01;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        (n & bitNum) && (sum += num); bitNum <<= 1; num <<= 1;
        return sum >> 1;
    }
};

int main() {
    //(void)BugReportRegister("run", ".", nullptr, nullptr);
    int num = 10;
    LOG_DEBUG("%d %d", num , Solution::Sum_Solution(num));
    num = 100;
    LOG_DEBUG("%d %d", num, Solution::Sum_Solution(num));
    return 0;
}
