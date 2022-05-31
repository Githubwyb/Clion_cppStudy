/**
 * @file main.cpp
 * @brief 启动函数所在位置
 * @author wangyubo
 * @version v2.0.0
 * @date 2020-12-16
 */
#include <unistd.h>

#include <boost/asio.hpp>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "baseInstance.hpp"
// #include "bugReport.hpp"
#include "common.hpp"
#include "log.hpp"
#include "threadPool.hpp"

using namespace std;

std::future<void> testFunc() {
    boost::asio::io_context ioc;
    boost::asio::io_context::work worker(ioc);

    auto f1 = std::async(std::launch::async, [&ioc]() {
        ioc.run();
        LOGI(WHAT("ioc exit"));
        std::this_thread::sleep_for(std::chrono::seconds(5));
    });
    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::promise<void> p;
    // auto f = p.get_future();
    int testValue = 0;
    LOGI(WHAT("push task"));
    ioc.post([&testValue, &p, &ioc]() {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        testValue = 1;
        p.set_value();
    });
    p.get_future().wait();
    LOGI(WHAT("testValue {}", testValue));
    return f1;
}

int main(int argC, char *argV[]) {
    // (void)BugReportRegister("run", ".", nullptr, nullptr);
    spdlog::set_level(spdlog::level::debug);  // Set global log level to debug
    // spdlog::set_pattern("%Y-%m-%d %H:%M:%S [%P:%t][%L][%@ %!] %v");
    // spdlog::set_pattern("%Y-%m-%d %H:%M:%S [%P:%t][%L][%s:%# %!] %v");
    LOGI(WHAT("Hello, main"));
    LOGD(WHAT("aaaa {}", 1234));
    LOGI(WHAT("aaaa {}", 1234));
    LOGI(WHAT("aaaa {:#06x}", 1234));
    LOGE(WHAT("get_svpn_rand"), REASON("hhh {}", 1), WILL("aaaaa"));
    LOGW(WHAT("get_svpn_rand"), REASON("hhh {}", 0x1234), NO_WILL);

    std::multimap<int, string> testMap;
    testMap.insert(std::make_pair(6, "12"));
    testMap.insert(std::make_pair(5, "123"));
    testMap.insert(std::make_pair(5, "123"));
    testMap.insert(std::make_pair(5, "1235"));
    testMap.insert(std::make_pair(5, "12"));
    testMap.insert(std::make_pair(4, "12"));
    testMap.insert(std::make_pair(5, "1245"));

    auto np = testMap.equal_range(5);
    for (auto iter = np.first; iter != np.second; iter++) {
        LOGI(WHAT("{} {}", iter->first, iter->second));
    }

    // auto f1 = testFunc();
    // LOGI(WHAT("main end1"));
    // std::future<void> f2 = std::move(f1);
    // f2.wait();
    // LOGI(WHAT("async end"));
    return 0;
}
