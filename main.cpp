/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <algorithm>
#include <string>
#include <vector>

#include "log.hpp"
//#include "bugReport.hpp"

using namespace std;

typedef struct ListNode {
    int val;
    struct ListNode *next;
    ListNode(int x) :
            val(x), next(NULL) {
    }

    ListNode(int x, ListNode *p) : val(x), next(p) {}
} ListNode;

class Solution {
   public:
    // 最骚解法，最多两次遍历
    ListNode *FindFirstCommonNode(ListNode *pHead1, ListNode *pHead2) {
        auto p1 = pHead1;
        auto p2 = pHead2;
        // 要么相等，要么同时为nullptr
        while (p1 != p2) {
            p1 = (p1 == nullptr ? pHead2 : p1->next);
            p2 = (p2 == nullptr ? pHead1 : p2->next);
        }
        return p1;
    }

    // 暴力破解
    ListNode *FindFirstCommonNode1(ListNode *pHead1, ListNode *pHead2) {
        auto p1 = pHead1;
        auto p2 = pHead2;
        // 要么相等，要么同时为nullptr
        while (p1 != p2) {
            // p1到尾部，p2后移
            p2 = (p1 == nullptr ? p2 : p2->next);
            // p1到尾部，从头再来
            p1 = (p1 == nullptr ? pHead1 : p1->next);
        }
        return p1;
    }
};

int main() {
   // (void)BugReportRegister("run", ".", nullptr, nullptr);
    auto common = new ListNode(6, new ListNode(7, nullptr));
    ListNode test1 = ListNode(1, new ListNode(2, new ListNode(3, common)));
    ListNode test2 = ListNode(4, new ListNode(5, common));
    LOG_DEBUG("%d", common == Solution::FindFirstCommonNode(&test1, &test2));
    return 0;
}
