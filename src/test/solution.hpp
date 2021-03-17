/**
 * @file solution.hpp
 * @brief 算法头文件
 * @author wangyubo
 * @version v0.0.1
 * @date 2021-03-17
 */

#include <stack>

struct ListNode {
    int val;
    struct ListNode *next;
};

using namespace std;

class Solution {
   public:
    /**
     *
     * @param head ListNode类
     * @param k int整型
     * @return ListNode类
     */
    ListNode *reverseKGroup(ListNode *head, int k) {
        if (head == nullptr || head->next == nullptr || k < 2) return head;

        stack<ListNode *> node_stack;
        int n = 0;
        ListNode *last_node = nullptr;
        ListNode *result = head;
        ListNode *current_node = head;
        while (current_node != nullptr) {
            n++;
            node_stack.emplace(current_node);
            current_node = current_node->next;
            if (n < k) {
                continue;
            }
            // 这里执行翻转逻辑
            // 先出第一个，没有上一次，说明为第一个k
            if (last_node != nullptr) {
                // 有上一次，上一次接这一跳
                last_node->next = node_stack.top();
                last_node = last_node->next;
            } else {
                result = node_stack.top();
                last_node = result;
            }
            node_stack.pop();
            while (--n) {
                last_node->next = node_stack.top();
                node_stack.pop();
                last_node = last_node->next;
            }
            // 这里栈已经没元素了
            last_node->next = current_node;
        }
        return result;
    }
};
