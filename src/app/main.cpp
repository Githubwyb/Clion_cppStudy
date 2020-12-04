/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <iostream>
#include <string>
#include <vector>

#include "bugReport.hpp"
#include "log.hpp"
#include "utils.hpp"

using namespace std;

extern void init_mylib(void);

int main(int argC, char *argV[]) {
    (void)BugReportRegister("run", ".", nullptr, nullptr);
    spdlog::set_level(spdlog::level::debug);  // Set global log level to debug
    spdlog::set_pattern("%Y-%m-%d %H:%M:%S [%P][%L][%@ %!] %v");
    init_mylib();
    LOG_INFO("test");
    return 0;
}
