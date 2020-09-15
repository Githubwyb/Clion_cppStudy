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
};

// 定义工人，也就是建造者
class Builder {
   public:
    Builder(Product *pProduct) : m_product(pProduct) {}
    Builder() = delete;
    virtual void buildPart1() = 0;
    virtual void buildPart2() = 0;
    virtual void buildPart3() = 0;
    Product *getProduct() { return m_product; }

   protected:
    Product *m_product = nullptr;
};

// 定义实际工人继承
class proABuilder : public Builder {
   public:
    void buildPart1() { m_product->part1 = "A part1"; }
    void buildPart2() { m_product->part2 = "A part2"; }
    void buildPart3() { m_product->part3 = "A part3"; }
};

int main() {
    (void)BugReportRegister("run", ".", nullptr, nullptr);

    return 0;
}
