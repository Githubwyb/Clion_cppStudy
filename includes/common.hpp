/*
 * @Author WangYubo
 * @Date 11/26/2020
 * @Description 公共类型定义头文件
 */

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
