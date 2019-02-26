/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include "log.hpp"

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

int main() {
    int n = 0;
    cin >> n;
    map<int, vector<int>> likeMap;
    for (int i = 0; i < n; ++i) {
        int like = 0;
        cin >> like;
        likeMap[like].emplace_back(i + 1);
    }

    int q = 0;
    cin >> q;
    for (int j = 0; j < q; ++j) {
        int l, r, k;
        cin >> l >> r >> k;

        auto ln = lower_bound(likeMap[k].begin(), likeMap[k].end(), l);
        auto rn = upper_bound(likeMap[k].begin(), likeMap[k].end(), r);
        cout << rn - ln << endl;
    }

    return 0;
}
