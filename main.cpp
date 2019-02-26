/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include "log.hpp"

#include <iostream>
#include <vector>

using namespace std;

int main() {
    int n = 0;
    cin >> n;
    vector<int> user;
    for (int i = 0; i < n; ++i) {
        int like = 0;
        cin >> like;
        user.emplace_back(like);
    }

    int q = 0;
    cin >> q;
    for (int j = 0; j < q; ++j) {
        int l = 0;
        cin >> l;
        int r = 0;
        cin >> r;
        int k = 0;
        cin >> k;

        int result = 0;
        for (int i = l - 1; i < r; ++i) {
            if (user[i] == k) {
                result++;
            }
        }
        cout << result << endl;
    }
    return 0;
}
