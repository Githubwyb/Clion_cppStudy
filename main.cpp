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

// 定义一个复杂产品，暂时先所有都为public
class Product {
   public:
    string part1;
    string part2;
    string part3;
    void show() {
        LOG_DEBUG("part 1 %s", part1.c_str());
        LOG_DEBUG("part 2 %s", part2.c_str());
        LOG_DEBUG("part 3 %s", part3.c_str());
    }
};

// 定义工人，也就是建造者
class Builder {
   public:
    Builder(Product *pProduct) : m_product(pProduct) {}
    Builder() = delete;
    Builder(const Builder&) = delete;
    virtual void buildPart1() = 0;
    virtual void buildPart2() = 0;
    virtual void buildPart3() = 0;
    Product *getProduct() { return m_product; }

   protected:
    Product *m_product = nullptr;
};

// 定义实际工人继承
class ProABuilder : public Builder {
   public:
    using Builder::Builder;
    void buildPart1() { m_product->part1 = "A part1"; }
    void buildPart2() { m_product->part2 = "A part2"; }
    void buildPart3() { m_product->part3 = "A part3"; }
};

// 定义设计师，指挥工人做事
class Designer {
   public:
    void creatProductA(Builder *pBuilder) {
        pBuilder->buildPart1();
        pBuilder->buildPart2();
        pBuilder->buildPart3();
    }
};

int main() {
    (void)BugReportRegister("run", ".", nullptr, nullptr);
    // 定义产品
    Product a;
    // 招募工人
    ProABuilder builder(&a);
    // 招募设计师
    Designer designer;

    // 设计师指挥工人干活
    designer.creatProductA(&builder);

    // 获取产品
    auto tmp = builder.getProduct();

    tmp->show();
    return 0;
}
