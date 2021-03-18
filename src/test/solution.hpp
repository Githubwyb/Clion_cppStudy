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
    bool hasCycle(ListNode *head) {
		/* 思路1
         * 想象两个人在捉迷藏，一个人先走到一个地方，另一个人去找
         * 如果找到他的时候和他走的路程一样长，那就是原路找的
         * 如果比他路程还短，说明另外一个人绕了一圈，也就是有环路
         *
         * 思路2（当前实现代码）
         * 两个人追逐，一个走的快（一次2步），一个走的慢（一次一步）
         * 如果相遇，说明有环路
         *
         * 思路3
         * 破坏链表，遍历的同时，让前一个节点指向头指针
         * 遍历如果到了头指针就说明有环路
		 */
        ListNode *pFast = head;     // 跑的快的指针
        ListNode *pSlow = head;     // 跑得慢的指针
        while (pFast != nullptr && pFast->next != nullptr) {
            pSlow = pSlow->next;
            pFast = pFast->next->next;
            if (pSlow == pFast) return true;
        }
        return false;
    }
};
