/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <atomic>
#include <string>
#include <vector>

#include "baseInstance.hpp"
#include "threadPool.hpp"
#include "log.hpp"
using namespace std;

class testPool : public threadPool, public BaseInstance<testPool> {
   public:
    void init(int threadNum) { threadPool::init(threadNum, "testPool"); }
};


void fun_2(int &a,int &b)
{
    a++;
    b++;
	LOG_DEBUG("print a = %d, b = %d",a, b);
}

int main(int argC, char *arg[]) {
    LOG_DEBUG("Hello");
    int m = 2;
    int n = 3;
    auto f4 = std::bind(fun_2, n, placeholders::_1); //表示绑定fun_2的第一个参数为n, fun_2的第二个参数由调用f4的第一个参数（_1）指定。
    f4(m); //print: m=3,n=4
    LOG_DEBUG("m %d", m);
    LOG_DEBUG("n %d", n);
    return 0;
}

