/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include "log.hpp"

#include <iostream>
#include <string>

using namespace std;

bool check(string &str) {
    int state = 0;
    char tmp = 0;
    for (auto it = str.begin(); it != str.end(); it++) {
        switch (state) {
            case 0:
                if (*it == tmp) {
                    state = 1;
                } else {
                    tmp = *it;
                }
                break;

            case 1:
                if (*it == tmp) {
                    str.erase(it);
                    LOG_DEBUG("%s", str.data());
                    return true;
                } else {
                    tmp = *it;
                    state = 2;
                }
                break;

            case 2:
                if (*it == tmp) {
                    str.erase(it);
                    return true;
                } else {
                    tmp = *it;
                    state = 0;
                }
                break;

            default:
                break;
        }
    }
    return false;
}

int main() {
    int N = 0;
    cin >> N;

    for (int i = 0; i < N; ++i) {
        string words;
        cin >> words;
        while (check(words)) {}
        cout << words << endl;
    }

    return 0;
}
