/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include "log.hpp"

#include <iostream>

int main() {
    LOG_DEBUG("Hello, world");
    LOG_WARN("Hello, world");
    LOG_INFO("Hello, world");
    LOG_ERROR("Hello, world");
    LOG_FATAL("Hello, world");
    LOG_PRINT("Hello, world");
    PRINT("Hello, world\r\n");
    LOG_HEX("Hello, world", std::string("Hello, world").size());
    LOG_BIN("Hello, world5555555555555555555555555555555555", std::string("Hello, world5555555555555555555555555555555555").size());
    return 0;
}
