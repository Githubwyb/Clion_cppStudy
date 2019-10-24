/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <string>
#include <vector>
#include <iostream>
#include <cstring>
#include "log.hpp"

using namespace std;


typedef struct ListNode {
    int val;
    struct ListNode *next;

    ListNode(int x) : val(x), next(NULL) {
    }
} ListNode;

class Solution {
public:
    vector<int> printListFromTailToHead(ListNode *head) {
        vector<int> result;
        //遍历查找最后一个元素，过程中倒序list
        ListNode *pTmp1 = head;     //保存最开始的节点
        ListNode *pTmp2 = nullptr;  //保存第二个
        ListNode *pTmp3 = nullptr;  //保存第三个
        if (pTmp1 == nullptr) {
            return move(result);
        }
        //循环列表
        while (pTmp1->next != nullptr) {
            //顺序后延一位
            pTmp3 = pTmp2;
            pTmp2 = pTmp1;
            pTmp1 = pTmp1->next;

            //第二个节点不为空的时候
            if (pTmp2 != nullptr) {
                pTmp2->next = pTmp3;
            }
        }

        result.emplace_back(pTmp1->val);

        while (pTmp2 != nullptr) {
            result.emplace_back(pTmp2->val);
            pTmp2 = pTmp2->next;
        }

        return move(result);
    }
};

void print_vector(const vector<int> &inV) {
    LOG_DEBUG("Print");
    for (auto tmp : inV) {
        PRINT("%d ", tmp);
    }
    PRINT("\r\n");
}

int main(int argC, char *arg[]) {
    ListNode test1(5);
    ListNode test2(4);
    ListNode test3(3);
    ListNode test4(2);
    ListNode test5(1);

    Solution a;
//    print_vector(a.printListFromTailToHead(&test1));
    test1.next = &test2;
//    print_vector(a.printListFromTailToHead(&test1));
    test2.next = &test3;
//    print_vector(a.printListFromTailToHead(&test1));
    test3.next = &test4;
//    print_vector(a.printListFromTailToHead(&test1));
    test4.next = &test5;
    print_vector(a.printListFromTailToHead(&test1));
    return 0;
}
