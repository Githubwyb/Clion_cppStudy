/**
 * @file main.cpp
 * @brief 启动函数所在位置
 * @author wangyubo
 * @version v2.0.0
 * @date 2020-12-16
 */
#include <iostream>
#include <string>
#include <vector>

#include "baseInstance.hpp"
//#include "bugReport.hpp"
#include "common.hpp"
#include "log.hpp"
#include "threadPool.hpp"

using namespace std;

class testPool : public threadPool, public BaseInstance<testPool> {
   public:
    void init(int threadNum) { threadPool::init(threadNum, "testPool"); }
    void testHHH(int a) { LOG_INFO("a {}", a); }
};

class testClass {
   public:
    int testFunc(char *buf, int size, const char *pStrIp, testPool *b) {
        LOG_INFO("get param buf: {:p}, size: {}", buf, size);
        b->testHHH(size);
        return 0;
    }
};

int run(char *buf, int size, const char *pStrIp, testPool *b) {
    testClass obj;
    return obj.testFunc(buf, size, pStrIp, b);
}

extern "C" void init_mylib(void *);

int main(int argC, char *argV[]) {
//    (void)BugReportRegister("run", ".", nullptr, nullptr);
//    spdlog::set_level(spdlog::level::debug);  // Set global log level to debug
//    spdlog::set_pattern("%Y-%m-%d %H:%M:%S [%P:%t][%L][%@ %!] %v");
    auto pFunc = (void *)&testClass::testFunc;
    init_mylib(pFunc);
    return 0;
    LOG_INFO("start, want hook func {:p}", pFunc);
    auto &pool = testPool::getInstance();
    char test[] = "tessss";
    pool.init(5);
    // 定义返回值
    vector<future<int>> ret;

    for (int i = 0; i < 20; i++) {
        // 插入任务函数
        ret.emplace_back(pool.commit(run, test, 6, "abdc", &pool));
    }
    for (auto &tmp : ret) {
        // 等待结果返回
        tmp.wait();
    }
    return 0;
}

