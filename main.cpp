/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <queue>
#include <stack>
#include <vector>

#include "log.hpp"

using namespace std;

class Solution {
   public:
    void push(int value) {
        if (m_minData.empty() || value <= m_minData.top()) {
            m_minData.push(value);
        }

        m_data.push(value);
    }

    void pop() {
        if (m_minData.top() == m_data.top()) {
            m_minData.pop();
        }
        m_data.pop();
    }

    int top() { return m_data.top(); }
    int min() {
        if (m_minData.empty()) {
            return -1;
        }
        return m_minData.top();
    }

   private:
    stack<int> m_data;
    stack<int> m_minData;
};

int main(int argC, char *arg[]) {
    Solution a;
    a.push(3);
    LOG_DEBUG("%d", a.min());
    a.push(4);
    LOG_DEBUG("%d", a.min());
    a.push(2);
    LOG_DEBUG("%d", a.min());
    a.push(3);
    LOG_DEBUG("%d", a.min());
    a.pop();
    LOG_DEBUG("%d", a.min());
    a.pop();
    LOG_DEBUG("%d", a.min());
    a.pop();
    LOG_DEBUG("%d", a.min());
    a.push(0);
    LOG_DEBUG("%d", a.min());
    return 0;
}
