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
    void replaceSpace(char *str,int length) {
        int srcI = 0;
        int desI = 0;
        string desStr;
        while (str != nullptr) {
            char *find = strstr(str + srcI, " ");
            if (find == nullptr)
                break;
            desStr.append(str + srcI, find);
            srcI = find - str + 1;
            desStr.append("%20");
        }
        desStr.append(str + srcI, length - srcI);
        memcpy(str, desStr.c_str(), desStr.size());
    }
};

int main(int argC, char *arg[])
{
    char test[255] = " i  l k lkj kk  kjl";
    Solution a;
    a.replaceSpace(test, strlen(test));
    LOG_DEBUG("%s", test);
    return 0;
}
