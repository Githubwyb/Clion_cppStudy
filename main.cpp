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
    static ListNode* ReverseList(ListNode* pHead) {
        ListNode *p1 = nullptr;     //跟在后面翻转next的指针
        ListNode *p2 = nullptr;     //保存前一次

        while (pHead != nullptr) {
            p2 = p1;
            p1 = pHead;
            pHead = pHead->next;
            p1->next = p2;
        }
        
        return p1;
    }
};

int main(int argC, char *arg[]) {
    return 0;
}
