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


struct ListNode {
	int val;
	struct ListNode *next;
	explicit ListNode(int x) :
			val(x), next(nullptr) {
	}
};
class Solution {
public:
    static ListNode* FindKthToTail(ListNode* pListHead, unsigned int k) {
        if (k == 0) {
            return nullptr;
        }

        ListNode *p1 = pListHead;   //遍历指针
        ListNode *p2 = pListHead;   //比p1提前k个节点的指针

        int i = 0;
        for (i = 1; p1 != nullptr; ++i) {
            //走了k步后，p2移动跟上
            if (i > k) {
                p2 = p2->next;
            }
            p1 = p1->next;
        }

        //没走够k步返回空
        return (i > k ? p2 : nullptr);
    }
};

int main(int argC, char *arg[]) {
    return 0;
}
