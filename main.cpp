/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <string>
#include <vector>
#include <stack>
#include <memory>
#include <iostream>
#include <cstring>
#include "log.hpp"

using namespace std;

class Solution
{
public:
    void push(int node) {
        stack1.push(node);
    }

    int pop() {
        if (stack2.empty()) {
            while (!stack1.empty()) {
                stack2.push(stack1.top());
                stack1.pop();
            }
        }

        if (stack2.empty()) {
            return -1;
        }

        int result = stack2.top();
        stack2.pop();
        return result;
    }

private:
    stack<int> stack1;  //入队列
    stack<int> stack2;  //出队列
};

int main(int argC, char *arg[]) {
    Solution queue;

    queue.push(1);
    queue.push(2);
    queue.push(3);
    queue.push(4);
    queue.push(5);

    LOG_DEBUG("%d", queue.pop());
    LOG_DEBUG("%d", queue.pop());
    LOG_DEBUG("%d", queue.pop());
    LOG_DEBUG("%d", queue.pop());
    LOG_DEBUG("%d", queue.pop());
    LOG_DEBUG("%d", queue.pop());
    return 0;
}
