/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "bugReport.hpp"
#include "log.hpp"

using namespace std;

class Product {
   public:
    virtual void show() = 0;
    virtual ~Product() = default;
};

class ProductA : public Product {
   public:
    void show() { LOG_DEBUG("I'm product A"); }
};

class ProductB : public Product {
   public:
    void show() { LOG_DEBUG("I'm product B"); }
};

class ProductC : public Product {
   public:
    void show() { LOG_DEBUG("I'm product C"); }
};

class Factory {
   public:
    static Product *createProduct(int type) {
        switch (type) {
            case 0:
                return new ProductA();
            case 1:
                return new ProductB();
            case 2:
                return new ProductC();

            default:
                return nullptr;
        }
    }
};

int main() {
    (void)BugReportRegister("run", ".", nullptr, nullptr);

    while (true) {
        /* code */
        int value;
        cin >> value;
        auto tmp = Factory::createProduct(value);
        if (tmp != nullptr) {
            tmp->show();
            delete tmp;
        }
    }

    return 0;
}
