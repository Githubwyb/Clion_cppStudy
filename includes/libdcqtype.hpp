/**
 * @file libdcqtype.hpp
 * @author wangyubo (1061322005@qq.com)
 * @brief 对外类型头文件
 * @version 0.1
 * @date 2020/12/13
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef LIGDCQ_TYPE_HPP
#define LIGDCQ_TYPE_HPP

#include <map>
#include <string>

#define LIBDCQ_API __attribute__((visibility("default")))

#ifndef MAX_ERROR_MSG_LEN
#define MAX_ERROR_MSG_LEN 255
#endif

namespace libdcq {
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

// 定义输出函数，外部可自制
using OutputFunc = void (*)(const std::string &, KeyValueMap &);

enum ERROR_CODE {
    SUCCESS = 0,  // 成功
    FAILED,       // 一般失败
    // 配置错误码
    FAILED_INI_PARSE,   // ini配置文件解析失败
    FAILED_UNINITED,  // 未初始化
    // 解析器错误码
    FAILED_NO_PARSER,  // 没有parser解析成功
    FAILED_SO_OPEN,    // so库打开失败
    FAILED_SO_PARSE,   // so库找不到parser符号

    UNKOWN_FAILED  // 未定义错误码
};

#define DEFAULT_CONF_PATH "/etc/libdcq/global.ini"
#define DEFAULT_SERVERCONF_PATH "/etc/libdcq/config/server.json"
#define DEFAULT_PARSER_DIR "/etc/libdcq/parser/"
#define DEFAULT_LOG_DIR "/var/log/libdcq/"

// 线程最大数量
#define MAX_THREAD_NUM 50
// 线程最小数量
#define MIN_THREAD_NUM 1

}  // namespace libdcq

#endif