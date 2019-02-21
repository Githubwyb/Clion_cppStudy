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

int main() {
    pRoot = make_shared<Node>();
    pRoot->value = 50;
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
    tmp1.value = 40;
    addNode(pRoot, tmp1);

    clearNode(tmp1);
    tmp1.value = 30;
    addNode(pRoot, tmp1);

    clearNode(tmp1);
    tmp1.value = 10;
    addNode(pRoot, tmp1);

    clearNode(tmp1);
    tmp1.value = 20;
    addNode(pRoot->pChild[0], tmp1);

    clearNode(tmp1);
    tmp1.value = 70;
    addNode(pRoot->pChild[0], tmp1);

    clearNode(tmp1);
    tmp1.value = 15;
    addNode(pRoot->pChild[0]->pChild[1], tmp1);

    clearNode(tmp1);
    tmp1.value = 25;
    addNode(pRoot->pChild[0]->pChild[1], tmp1);
}
