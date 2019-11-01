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
#include <algorithm>
#include <cstring>
#include "log.hpp"

using namespace std;

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
#include <algorithm>
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
    static ListNode *Merge(ListNode *pHead1, ListNode *pHead2) {
        if (pHead1 == nullptr) {
            return pHead2;
        }

        if (pHead2 == nullptr) {
            return pHead1;
        }

        ListNode* result = nullptr;
        if (pHead1->val < pHead2->val) {
            result = pHead1;
            pHead1 = pHead1->next;
        } else {
            result = pHead2;
            pHead2 = pHead2->next;
        }

        ListNode* pTmp = result;
        while (true) {
            if (pHead1 == nullptr) {
                pTmp->next = pHead2;
                break;
            }

            if (pHead2 == nullptr) {
                pTmp->next = pHead1;
                break;
            }

            if (pHead1->val < pHead2->val) {
                pTmp->next = pHead1;
                pHead1 = pHead1->next;
            } else {
                pTmp->next = pHead2;
                pHead2 = pHead2->next;
            }
            pTmp = pTmp->next;
        }

        return result;
    }
};

int main(int argC, char *arg[]) {
    return 0;
}


int main(int argC, char *arg[]) {
    return 0;
}
