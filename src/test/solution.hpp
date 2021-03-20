/**
 * @file solution.hpp
 * @brief 算法头文件
 * @author wangyubo
 * @version v0.0.1
 * @date 2021-03-17
 */

#include <stack>
#include <string>
using namespace std;

class Solution {
   public:
    char getOpsite(char a) {
        switch (a) {
            case '{':
                return '}';
            case '[':
                return ']';
            case '(':
                return ')';
            default:
                return ' ';
        }
    }
    /**
     *
     * @param s string字符串
     * @return bool布尔型
     */
    bool isValid(string s) {
        // write code here
        stack<char> sCheck;
        for (auto &item : s) {
            switch (item) {
                case '(':
                case '[':
                case '{':
                    sCheck.push(getOps(item));
                    break;

                default:
                    if (sCheck.size() == 0 || item != sCheck.top()) {
                        return false;
                    }
                    sCheck.pop();
            }
        }
        return sCheck.size() == 0;
    }
};
