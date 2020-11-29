/*
 * @Author WangYubo
 * @Date 11/25/2020
 * @Description 配置管理器
 */
#include "configManager.hpp"

#include <unistd.h>

#include <fstream>
#include <iostream>

#include "iniReader/INIReader.h"
#include "log.hpp"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "utils.hpp"

using namespace std;
using namespace rapidjson;

int configManager::loadINIConf(const string &path) {
    if (m_confPath == "") {
        m_confPath = path;
    }

    auto iniReader = INIReader(m_confPath);
    auto ret = iniReader.ParseError();
    if (ret != 0) {
        LOG_ERROR("file %s open failed, ret %d", m_confPath.c_str(), ret);
        return FILE_OPEN_ERROR;
    }

    // 请求服务器配置路径，参数解析到此参数就不用global.ini中的参数
    if (m_serverConfPath == "") {
        m_serverConfPath = iniReader.GetString(
            "server", "confPath",
            utils::getProgramPath() + "/config/server.json");
        if (m_serverConfPath[0] != '/') {
            // 不是绝对路径，拼接程序目录
            m_serverConfPath = utils::getProgramPath() + "/" + m_serverConfPath;
        }
    }
    // 解析服务器配置文件
    ret = parseServerConf(m_serverConfPath);
    if (ret != SUCCESS) {
        LOG_ERROR("file %s parse failed, ret %d", m_serverConfPath.c_str(),
                  ret);
        return ret;
    }

    // 解析库的路径，参数解析到此参数就不用global.ini中的参数
    if (m_parserLibPath == "") {
        m_parserLibPath = iniReader.GetString(
            "parser", "libPath", utils::getProgramPath() + "/parser/");
        if (m_parserLibPath[0] != '/') {
            // 不是绝对路径，拼接程序目录
            m_parserLibPath = utils::getProgramPath() + "/" + m_parserLibPath;
        }
    }

    return SUCCESS;
}

void configManager::showConf() {
    LOG_INFO("Show conf:");
    PRINT("====================================================\n");
    PRINT("confPath:            '%s'\n", m_confPath.c_str());
    PRINT("parserLibPath:       '%s'\n", m_parserLibPath.c_str());
    PRINT("inputFilePath:       '%s'\n", m_inputFile.c_str());
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
    PRINT("input:\n");
    for (auto &item : m_vInput) {
        PRINT("    %s\n", item.c_str());
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
        return FILE_OPEN_ERROR;
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

    return m_vQueryServer.size() == 0 ? SERVER_CONF_EMPTY : SUCCESS;
}

int configManager::getCmdLineParam(int argC, char *argV[]) {
    char ch;
    while ((ch = getopt(argC, argV, "c:s:p:f:")) != EOF) {
        switch (ch) {
            case 'c':
                // global.ini
                m_confPath = optarg;
                break;

            case 's':
                // server.json
                m_serverConfPath = optarg;
                break;

            case 'p':
                // parserLibPath/
                m_parserLibPath = optarg;
                // 防止输入未加最后的/
                if (*(m_parserLibPath.end() - 1) != '/') {
                    m_parserLibPath += "/";
                }
                break;

            case 'f':
                // 输入文件
                m_inputFile = optarg;
                break;

            default:
                usage();
                return PARAM_PARSE_ERROR;
        }
    }

    argC -= optind;
    argV += optind;

    // 没参数输入
    if (m_inputFile == "" && argC == 0) {
        LOG_INFO("Need input a domain");
        usage();
        return PARAM_PARSE_ERROR;
    }

    m_vInput.clear();
    // 解析文件输入
    if (m_inputFile != "" && parseInputFile(m_inputFile) != SUCCESS) {
        LOG_INFO("input file parse error, please check -f");
        usage();
        return PARAM_PARSE_ERROR;
    }

    // 命令行输入
    for (int index = 0; index < argC; ++index) {
        m_vInput.emplace_back(argV[index]);
    }

    return SUCCESS;
}

void configManager::usage(void) {
    cout << "Usage: dcq [options...] <domain|file>" << endl
         << "    -c <filePath> System config file, global.ini" << endl
         << "                  default (program path)/global.ini" << endl
         << "    -s <filePath> Server config to parse domain, server.json"
         << endl
         << "                  default parse from global.ini::server.confPath"
         << endl
         << "    -p <dirPath>  Parser so lib path, parserLibPath/" << endl
         << "                  default parse from global.ini::parser.libPath"
         << endl
         << "                  use parserLibPath/lib(type).so to parse domain "
            "which type defined in server.json"
         << endl
         << endl;
}

int configManager::parseInputFile(const std::string &path) {
    //读txt文件
    ifstream infile;             //定义文件变量
    infile.open(path, ios::in);  //打开txt
    if (!infile) {
        LOG_ERROR("Read file %s failed", path.c_str());
        return FILE_OPEN_ERROR;
    }

    string temp;
    //读取一行，直到所有行读完
    while (getline(infile, temp)) {
        // 去除换行符和空格
        int endIndex = temp.size();
        auto rpos = temp.find('\r');
        auto spos = temp.find(' ');
        // 先赋值\r的，空格覆盖此值
        if (rpos != temp.npos) {
            endIndex = rpos;
        }
        if (spos != temp.npos) {
            endIndex = spos;
        }

        if (endIndex != 0) {
            m_vInput.emplace_back(temp.substr(0, endIndex));
        }
    }
    infile.close();
    return SUCCESS;
}