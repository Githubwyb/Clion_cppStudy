/**
 * @file ec_to_string.hpp
 * @author 王钰博18433 (18433@sangfor.com)
 * @brief
 * @version 0.1
 * @date 2022-09-08
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <sstream>
#include <system_error>

namespace std {
static string to_string(const error_code &ec) {
    stringstream ss;
    ss << "{";
    ss << R"("code":)" << ec.value();
    ss << R"(,"category":")" << ec.category().name() << "\"";
    ss << R"(,"msg":)" << ec.message() << "\"";
    ss << "}";
    return ss.str();
}
}  // namespace std
