/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include "log.hpp"

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    int N, M;
    cin >> N >> M;

    vector<unsigned long long> len;
    unsigned long long maxLen = 0;
    for (int i = 0; i < N; ++i) {
        unsigned long long tmp;
        cin >> tmp;
        len.emplace_back(tmp * 100);
        maxLen = max(maxLen, tmp * 100);
    }
    sort(len.begin(), len.end());

    if (M <= N) {
        cout << len[N - M];
        return 0;
    }

    unsigned long long left = 0;
    unsigned long long right = maxLen;
    while ((right - left) > 1) {
        int count = 0;
        unsigned long long medium = (left + right) / 2;
        cout << left << medium << right << endl;
        for (auto tmp : len) {
            count += tmp / medium;
        }

        if (count >= M) {
            left = medium;
        } else {
            right = medium;
        }
    }

    printf("%.2f", left / 100.0);

    return 0;
}
