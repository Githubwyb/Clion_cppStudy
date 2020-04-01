/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <vector>
#include <assert.h>

#include "log.hpp"
#include "bugReport.hpp"

using namespace std;

class Solution {
   public:
    int FirstNotRepeatingChar(string str) {
        vector<int> count(128, 0);
        for (auto &tmp : str) {
            count[tmp]++;
        }

        int i = 0;
        for (auto &tmp : str) {
            if (count[tmp] == 1) return i;
            i++;
        }
        return -1;
    }
};

int main(int argC, char* arg[]) {
    (void)BugReportRegister("run", ".", nullptr, nullptr);
    std::string input;
    getline(cin, input);
    char findChr;
    scanf("%c", &findChr);

    int result = 0;
    result += count(input.begin(), input.end(), findChr);
    if (findChr >= 'A' && findChr <= 'Z') {
        result += count(input.begin(), input.end(), findChr - 'A' + 'a');
    }

    if (findChr >= 'a' && findChr <= 'z') {
        result += count(input.begin(), input.end(), findChr - 'a' + 'A');
    }
    cout << result;
    return 0;
}
