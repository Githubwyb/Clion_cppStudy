/**
 * @file solution.hpp
 * @brief 算法头文件
 * @author wangyubo
 * @version v0.0.1
 * @date 2021-03-17
 */

#include <algorithm>
#include <vector>
#include <unordered_map>
#include <string>

using namespace std;

struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(NULL) {}
};

class Solution {
   public:
    ListNode *detectCycle(ListNode *head) {
        /*
         * thinking 1
         * destroy the list, make every node point to some standard node,
         * if one point point to standard node, it's the circle head
         */
#if 0
        ListNode *pStand = new ListNode(0);
        while (head != nullptr) {
            if (head->next == pStand) {
                return head;
            }
            ListNode *tmp = head->next;
            // make every node point to -1
            head->next = pStand;
            head = tmp;
        }
        return nullptr;
#endif
        /*
         * thinking 2
         * math think, fast slow point
         * find the meet point, make tow point run from begin and meet point
         * when they meet, that's the circle head
         */
        ListNode *fast = head;
        ListNode *slow = head;
        while (fast != nullptr && fast->next != nullptr) {
            fast = fast->next->next;        // move two step
            slow = slow->next;
            if (fast == slow) {
                break;
            }
        }
        if (fast == nullptr || fast->next == nullptr) {
            // find the tail, must have no circle
            return nullptr;
        }

        // if meet, one run from head, one from meet
        fast = head;
        while (fast != slow) {
            fast = fast->next;
            slow = slow->next;
        }
        return fast;
    }
};
