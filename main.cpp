/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include "log.hpp"

#include <iostream>
#include <memory>
#include <vector>

using namespace std;

typedef struct Node {
    int value;
    vector<shared_ptr<Node>> pChild;
    weak_ptr<Node> pParent;
} Node;

shared_ptr<Node> pRoot = nullptr;

void addNodes();

#include <stack>

//深度优先
void printNodesDeepFirst(const shared_ptr<Node> &node) {
    stack<shared_ptr<Node>> myStack;
    myStack.push(node);
    while (myStack.size() > 0) {
        shared_ptr<Node> pTmp = myStack.top();
        myStack.pop();
        PRINT("%d ", pTmp->value);

        //由于栈的后进先出特性，需要倒序使左节点先出
        for (int i = pTmp->pChild.size(); i != 0; --i) {
            myStack.push(pTmp->pChild[i - 1]);
        }
    }
    PRINT("\r\n");
}

#include <queue>

//广度优先
void printNodesWidthFirst(const shared_ptr<Node> &node) {
    queue<shared_ptr<Node>> myQueue;
    myQueue.push(node);
    while (myQueue.size() > 0) {
        shared_ptr<Node> pTmp = myQueue.front();
        myQueue.pop();
        PRINT("%d ", pTmp->value);

        for (auto tmp : pTmp->pChild) {
            myQueue.push(tmp);
        }
    }
    PRINT("\r\n");
}

int main() {
    pRoot = make_shared<Node>();
    pRoot->value = 1;

    addNodes();

    printNodesDeepFirst(pRoot);
    printNodesWidthFirst(pRoot);
}

void addNode(const shared_ptr<Node> &pParent, Node &pChild) {
    pChild.pParent = pParent;
    pParent->pChild.push_back(make_shared<Node>(pChild));
}

void clearNode(Node &node) {
    node.pChild.clear();
    node.pParent.reset();
}

void addNodes() {
    Node tmp1;

    clearNode(tmp1);
    tmp1.value = 2;
    addNode(pRoot, tmp1);

    clearNode(tmp1);
    tmp1.value = 3;
    addNode(pRoot, tmp1);

    clearNode(tmp1);
    tmp1.value = 4;
    addNode(pRoot->pChild[0], tmp1);

    clearNode(tmp1);
    tmp1.value = 5;
    addNode(pRoot->pChild[0], tmp1);

    clearNode(tmp1);
    tmp1.value = 6;
    addNode(pRoot->pChild[1], tmp1);

    clearNode(tmp1);
    tmp1.value = 7;
    addNode(pRoot->pChild[1], tmp1);

    clearNode(tmp1);
    tmp1.value = 8;
    addNode(pRoot->pChild[0]->pChild[0], tmp1);

    clearNode(tmp1);
    tmp1.value = 9;
    addNode(pRoot->pChild[0]->pChild[0], tmp1);

    clearNode(tmp1);
    tmp1.value = 10;
    addNode(pRoot->pChild[0]->pChild[1], tmp1);
}
