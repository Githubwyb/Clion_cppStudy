/**
 * @file main.cpp
 * @brief 启动函数所在位置
 * @author wangyubo
 * @version v2.0.0
 * @date 2020-12-16
 */

#include <iostream>

#include "bugReport.hpp"
#include "libutils/circleList.hpp"
#include "log.hpp"
#include "shmManager.hpp"
#include "epollWorker.hpp"

using namespace std;

int main(int argC, char *argV[]) {
    (void)BugReportRegister("run", ".", nullptr, nullptr);
    spdlog::set_level(spdlog::level::debug);  // Set global log level to debug
    spdlog::set_pattern("%Y-%m-%d %H:%M:%S [%P:%t][%L][%@ %!] %v");

    if (argC > 1 && strcmp(argV[1], "client") == 0) {
        LOG_DEBUG("Client mod");
        clientRun();
    } else {
        LOG_DEBUG("Server mod");
        serverRun();
    }

    return 0;
}

