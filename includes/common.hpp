/*
 * @Author WangYubo
 * @Date 11/26/2020
 * @Description 公共类型定义头文件
 */
#ifndef COMMON_HPP
#define COMMON_HPP

#include <map>
#include <string>

using KeyValueMap = std::map<std::string, std::string>;

struct QueryServer {
    std::string url;
    std::string type;
    std::string requestType;
    std::string param;
    KeyValueMap result;
};

using QueryServer = struct QueryServer;

using ParserFunc = int (*)(const KeyValueMap &, std::string &, KeyValueMap &);

#endif /* COMMON_HPP */
