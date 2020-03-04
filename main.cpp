/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;
int main() {
    int num;
    while (cin >> num) {
        vector<int> input;
        for (int i = 0; i < num; i++) {
            int tmp = 0;
            cin >> tmp;
            input.push_back(tmp);
        }

        sort(input.begin(), input.end());
        input.erase(unique(input.begin(), input.end()), input.end());
        for (auto &tmp : input) {
            cout << tmp << endl;
        }
    }
}