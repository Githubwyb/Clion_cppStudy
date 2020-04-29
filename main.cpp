/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <algorithm>
#include <string>
#include <vector>

#include "log.hpp"
#include "bugReport.hpp"

using namespace std;

class Solution {
   public:
    vector<string> Permutation(string str) {
        int len = str.length();
        if (len == 0) {
            return {};
        }
        m_result.clear();
        Permutation(str, 0, len);
        sort(m_result.begin(), m_result.end());
        m_result.erase(unique(m_result.begin(), m_result.end()),
                       m_result.end());
        return m_result;
    }

   private:
    void Permutation(string str, int index, int len) {
        if (len == index) {
            m_result.emplace_back(str);
            return;
        }

        Permutation(str, index + 1, len);
        for (int i = index + 1; i < len; i++) {
            swap(str[index], str[i]);
            Permutation(str, index + 1, len);
            swap(str[index], str[i]);
        }
    }

    vector<string> m_result;
};

int main() {
    (void)BugReportRegister("run", ".", nullptr, nullptr);
    Solution so;
    auto tmpV = so.Permutation("abbsd");
    for (auto &tmp : tmpV) {
        LOG_DEBUG("%s", tmp.c_str());
    }
    return 0;
}
