/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <iostream>
#include <map>
#include <string>
#include <vector>

//#include "bugReport.hpp"
#include "log.hpp"

using namespace std;

class ProductA {
   public:
    virtual void show() = 0;
    virtual ~ProductA() = default;
};

class ProductA1 : public ProductA {
   public:
    void show() { LOG_DEBUG("I'm product A1"); }
};

class ProductA2 : public ProductA {
   public:
    void show() { LOG_DEBUG("I'm product A2"); }
};

class ProductB {
   public:
    virtual void show() = 0;
    virtual ~ProductB() = default;
};

class ProductB1 : public ProductB {
   public:
    void show() { LOG_DEBUG("I'm product B1"); }
};

class ProductB2 : public ProductB {
   public:
    void show() { LOG_DEBUG("I'm product B2"); }
};

class Factory {
   public:
    virtual ProductA *createProductA() = 0;
    virtual ProductB *createProductB() = 0;
};

class Factory1 : public Factory {
   public:
    ProductA *createProductA() { return new ProductA1(); }
    ProductB *createProductB() { return new ProductB1(); }
};

class Factory2 : public Factory {
   public:
    ProductA *createProductA() { return new ProductA2(); }
    ProductB *createProductB() { return new ProductB2(); }
};

int main() {
    //(void)BugReportRegister("run", ".", nullptr, nullptr);
    auto f1 = new Factory1();
    auto f2 = new Factory2();
    while (true) {
        /* code */
        int value;
        cin >> value;
        ProductA *tmpa = nullptr;
        ProductB *tmpb = nullptr;
        switch (value) {
            case 0:
                tmpa = f1->createProductA();
                tmpa->show();
                break;

            case 1:
                tmpb = f1->createProductB();
                tmpb->show();
                break;

            case 2:
                tmpa = f2->createProductA();
                tmpa->show();
                break;

            case 3:
                tmpb = f2->createProductB();
                tmpb->show();
                break;

            default:
                break;
        }

        if (!tmpa) {
            delete tmpa;
        }

        if (!tmpb) {
            delete tmpb;
        }
    }

    return 0;
}
