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
// #include "bugReport.hpp"
#include "common.hpp"
#include "log.hpp"
#include "threadPool.hpp"

using namespace std;

class A {
   public:
    virtual void test1() {
        LOG_INFO("OK");
        test2();
    }
    virtual void test2() { LOG_INFO("OK A"); }
};

class B : public A {
   public:
    void test2() { LOG_INFO("OK B"); }
};

int main(int argC, char *argV[]) {
    // (void)BugReportRegister("run", ".", nullptr, nullptr);
    spdlog::set_level(spdlog::level::debug);  // Set global log level to debug
    // spdlog::set_pattern("%Y-%m-%d %H:%M:%S [%P:%t][%L][%@ %!] %v");
    spdlog::set_pattern("%Y-%m-%d %H:%M:%S [%P:%t][%L][%s:%# %!] %v");
    LOG_INFO("Hello, main");
    B b;
    b.test1();
    return 0;
}

