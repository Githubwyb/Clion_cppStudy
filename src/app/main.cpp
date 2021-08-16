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

#define check0_f(x) \
    ((x >= '0' && x <= '9') || (x >= 'a' && x <= 'f') || (x >= 'A' && x <= 'F'))

std::string convertStrToByte(const std::string &str) {
    string result;
    size_t index = 0;
    size_t strLen = str.length();
    while (index < strLen) {
        char tmp[2] = {0};
        // bool parseOK = false;
        if (str[index] == '\\' && index + 3 < strLen && str[index + 1] == 'x' &&
            check0_f(str[index + 2]) && check0_f(str[index + 3]) &&
            sscanf(&(str.front()) + index, "\\x%02hhx", &tmp) == 1) {
            result += tmp;
            index += 4;
            continue;
        }
        result += str[index++];
    }
    return result;
}

int main(int argC, char *argV[]) {
    // (void)BugReportRegister("run", ".", nullptr, nullptr);
    spdlog::set_level(spdlog::level::debug);  // Set global log level to debug
    // spdlog::set_pattern("%Y-%m-%d %H:%M:%S [%P:%t][%L][%@ %!] %v");
    spdlog::set_pattern("%Y-%m-%d %H:%M:%S [%P:%t][%L][%s:%# %!] %v");
    LOG_INFO("Hello, main");
    string str = "\\xE6\\xB5\\x8B\\xE8\\xAF\\x95\\xE9\\x83\\xA8";
    LOG_DEBUG("{}", str);
    LOG_DEBUG("{}", convertStrToByte(str));
    str = "\\xEr\\xB5\\x8B\\xEz\\xA..\\xe9\\x03\\xA8";
    LOG_DEBUG("{}", str);
    auto result = convertStrToByte(str);
    LOG_HEX(result.c_str(), result.length());
    return 0;
}
