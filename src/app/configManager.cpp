/*
 * @Author WangYubo
 * @Date 11/25/2020
 * @Description 配置管理器
 */
#include "configManager.hpp"

#include <fstream>

#include "iniReader/INIReader.h"
#include "log.hpp"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "utils.hpp"

using namespace std;
using namespace rapidjson;

int configManager::loadINIConf(const string &path) {
    auto iniReader = INIReader(path);
    auto ret = iniReader.ParseError();
    if (ret != 0) {
        LOG_ERROR("file %s open failed, ret %d", path.c_str(), ret);
        return FILE_OPEN_ERROR;
    }

    // 赋值各个配置项
    m_confPath = path;
    // 请求服务器配置路径
    m_serverConfPath = iniReader.GetString(
        "server", "confPath", utils::getProgramPath() + "/config/server.json");
    if (m_serverConfPath[0] != '/') {
        // 不是绝对路径，拼接程序目录
        m_serverConfPath = utils::getProgramPath() + "/" + m_serverConfPath;
    }
    // 解析服务器配置文件
    parseServerConf(m_serverConfPath);

    return SUCCESS;
}

void configManager::showConf() {
    LOG_INFO("Show conf:");
    PRINT("====================================================\n");
    PRINT("confPath:            '%s'\n", m_confPath.c_str());
    PRINT("serverConfPath:      '%s'\n", m_serverConfPath.c_str());
    // 服务器配置
    PRINT("serverConf:\n");
    for (auto &pItem : m_vQueryServer) {
        PRINT("    server:\n");
        PRINT("        url:         '%s'\n", pItem->url.c_str());
        PRINT("        type:        '%s'\n", pItem->type.c_str());
        PRINT("        requestType: '%s'\n", pItem->requestType.c_str());
        PRINT("        param:       '%s'\n", pItem->param.c_str());
        PRINT("        result:\n");
        // 打印期望结果
        for (auto it = pItem->result.begin(); it != pItem->result.end(); ++it) {
            PRINT("            '%s': '%s'\n", it->first.c_str(),
                  it->second.c_str());
        }
    }

    PRINT("====================================================\n");
}

int configManager::parseServerConf(const string &path) {
    ifstream ifs(path);
    IStreamWrapper isw(ifs);
    // 解析json文件
    Document d;
    d.ParseStream(isw);
    if (d.HasParseError()) {
        LOG_ERROR("ParseError %d", d.GetParseError());
        return -1;
    }

    // 遍历获取服务器配置
    for (Value::ConstMemberIterator itr = d.MemberBegin(); itr != d.MemberEnd();
         itr++) {
        auto key = itr->name.GetString();
        LOG_DEBUG("server: %s", key);
        if (!itr->value.IsObject()) {
            LOG_WARN("key '%s' is not a object", key);
            continue;
        }
        auto serverConf = itr->value.GetObject();

        // 将读取到的属性存到服务器配置中
        QueryServer queryServer;
        // 请求url
        queryServer.url = key;
        // 结果类型
        if (!serverConf["type"].IsString()) {
            LOG_WARN("server(%s).type is not a string", key);
            continue;
        }
        queryServer.type = serverConf["type"].GetString();

        // 请求类型
        if (!serverConf["requestType"].IsString()) {
            LOG_WARN("server(%s).requestType is not a string", key);
            continue;
        }
        queryServer.requestType = serverConf["requestType"].GetString();

        // 请求参数结构
        if (!serverConf["param"].IsString()) {
            LOG_WARN("server(%s).param is not a string", key);
            continue;
        }
        queryServer.param = serverConf["param"].GetString();

        // 期望返回结果
        if (!serverConf["result"].IsObject()) {
            LOG_WARN("server(%s).result is not a object", key);
            continue;
        }
        auto result = serverConf["result"].GetObject();
        for (Value::ConstMemberIterator it = result.MemberBegin();
             it != result.MemberEnd(); ++it) {
            if (!it->value.IsString()) {
                LOG_WARN("server(%s).result.'%s' is not a string", key,
                         it->name.GetString());
                continue;
            }

            queryServer.result[it->name.GetString()] = it->value.GetString();
        }

        // 将配置储存到内部变量
        m_vQueryServer.emplace_back(make_shared<QueryServer>(queryServer));
    }
}
