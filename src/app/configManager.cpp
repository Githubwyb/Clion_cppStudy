/*
 * @Author WangYubo
 * @Date 11/25/2020
 * @Description 配置管理器
 */
#include "configManager.hpp"

#include <unistd.h>

#include <fstream>
#include <iostream>

#include "libhv/include/hv/iniparser.h"
#include "libhv/include/hv/json.hpp"
#include "log.hpp"
#include "utils.hpp"

using namespace std;

int configManager::loadINIConf(const string &path) {
    if (m_confPath == "") {
        m_confPath = path;
    }

    auto iniReader = IniParser();
    // auto iniReader = INIReader(m_confPath);
    auto ret = iniReader.LoadFromFile(path.c_str());
    if (ret != 0) {
        cout << "[Error] file " << m_confPath << " open failed, ret " << to_string(ret)
             << endl;
        return FILE_OPEN_ERROR;
    }

    // 解析日志相关参数
    m_logFilePath = iniReader.GetValue("log", "logPath");
    if (m_logFilePath == "") {
        m_logFilePath = utils::getProgramPath() + "/logs/";
    }
    if (m_logFilePath[0] != '/') {
        // 不是绝对路径，拼接程序目录
        m_logFilePath = utils::getProgramPath() + "/" + m_logFilePath;
    }
    m_logFileSize = iniReader.Get("log", "logFileSize", 5000);
    m_logFileRotating = iniReader.Get("log", "logFileRotating", 3);
    initLog();

    // 请求服务器配置路径，参数解析到此参数就不用global.ini中的参数
    if (m_serverConfPath == "") {
        m_serverConfPath = iniReader.GetValue("server", "confPath");
        if (m_serverConfPath == "") {
            m_serverConfPath = utils::getProgramPath() + "/config/server.json";
        }
        if (m_serverConfPath[0] != '/') {
            // 不是绝对路径，拼接程序目录
            m_serverConfPath = utils::getProgramPath() + "/" + m_serverConfPath;
        }
    }
    // 解析服务器配置文件
    ret = parseServerConf(m_serverConfPath);
    if (ret != SUCCESS) {
        cout << "[Error] file " << m_serverConfPath << " parse failed, ret "
             << to_string(ret) << endl;
        return ret;
    }

    // 解析库的路径，参数解析到此参数就不用global.ini中的参数
    if (m_parserLibPath == "") {
        m_parserLibPath = iniReader.GetValue("parser", "libPath");
        if (m_parserLibPath == "") {
            m_parserLibPath = utils::getProgramPath() + "/parser/";
        }
        if (m_parserLibPath[0] != '/') {
            // 不是绝对路径，拼接程序目录
            m_parserLibPath = utils::getProgramPath() + "/" + m_parserLibPath;
        }
    }

    showConf();
    return SUCCESS;
}

void configManager::showConf() {
    // 仅调试输出
    if (!m_logDebug) return;

    string printStr = "====================================================\n";
    printStr += "confPath:            '" + m_confPath + "'\n";
    printStr += "parserLibPath:       '" + m_parserLibPath + "'\n";
    printStr += "logFilePath:         '" + m_logFilePath + "'\n";
    printStr += "logFileSize:         " + to_string(m_logFileSize) + "\n";
    printStr += "logFileRotating:     " + to_string(m_logFileRotating) + "\n";
    printStr += "inputFilePath:       '" + m_inputFile + "'\n";
    printStr += "serverConfPath:      '" + m_serverConfPath + "'\n";
    // 服务器配置
    printStr += "serverConf:\n";
    for (auto &pItem : m_vQueryServer) {
        printStr += "    server:\n";
        printStr += "        url:         '" + pItem->url + "'\n";
        printStr += "        type:        '" + pItem->type + "'\n";
        printStr += "        requestType: '" + pItem->requestType + "'\n";
        printStr += "        param:       '" + pItem->param + "'\n";
        printStr += "        result:\n";
        // 打印期望结果
        for (auto it = pItem->result.begin(); it != pItem->result.end(); ++it) {
            printStr +=
                "            '" + it->first + "': '" + it->second + "'\n";
        }
    }
    printStr += "input:\n";
    for (auto &item : m_vInput) {
        printStr += "    '" + item + "'\n";
    }

    printStr += "====================================================\n";
    LOG_DEBUG("Show conf:\n{}", printStr);
}

int configManager::parseServerConf(const string &path) {
    LOG_DEBUG("Begin, path {}", path);
    // 读取json文件
    ifstream ifs(path);
    nlohmann::json j;

    // json文件解析
    try {
        ifs >> j;
    } catch (...) {
        LOG_ERROR("ParseError path {}", path);
        return FILE_OPEN_ERROR;
    }

    if (!j.is_array()) {
        LOG_ERROR("ParseError need json type array path {}", path);
        return FILE_OPEN_ERROR;
    }

    // 遍历获取服务器配置
    for (auto it = j.begin(); it != j.end(); ++it) {
        if (it.value().type() != nlohmann::detail::value_t::object) {
            LOG_WARN("item {} is not a object, but {}", *it, it.value().type_name());
            continue;
        }
        auto serverConf = it.value();

        // 将读取到的属性存到服务器配置中
        QueryServer queryServer;
        // 请求url
        auto tmp = serverConf["url"];
        if (!tmp.is_string()) {
            LOG_WARN("item({}).url is not a string, but {}", *it,
                     tmp.type_name());
            continue;
        }
        queryServer.url = tmp.get<string>();

        // 结果类型
        tmp = serverConf["type"];
        if (!tmp.is_string()) {
            LOG_WARN("item({}).type is not a string, but {}", *it,
                     tmp.type_name());
            continue;
        }
        queryServer.type = tmp.get<string>();

        // 请求类型
        tmp = serverConf["requestType"];
        if (!tmp.is_string()) {
            LOG_WARN("item({}).requestType is not a string, but {}", *it,
                     tmp.type_name());
            continue;
        }
        queryServer.requestType = tmp.get<string>();

        // 请求参数结构
        tmp = serverConf["param"];
        if (!tmp.is_string()) {
            LOG_WARN("item({}).param is not a string, but {}", *it,
                     tmp.type_name());
            continue;
        }
        queryServer.param = tmp.get<string>();

        // 期望返回结果
        tmp = serverConf["result"];
        if (!tmp.is_object()) {
            LOG_WARN("item({}).result is not a object, but {}", *it,
                     tmp.type_name());
            continue;
        }
        auto result = tmp;
        for (auto itr = result.begin(); itr != result.end(); ++itr) {
            if (!itr.value().is_string()) {
                LOG_WARN("item({}).result.'{}' is not a string, but {}", *it,
                         itr.value(), itr.value().type_name());
                continue;
            }

            queryServer.result[itr.key()] = itr.value();
        }

        // 将配置储存到内部变量
        m_vQueryServer.emplace_back(make_shared<QueryServer>(queryServer));
    }

    return m_vQueryServer.size() == 0 ? SERVER_CONF_EMPTY : SUCCESS;
}

int configManager::getCmdLineParam(int argC, char *argV[]) {
    char ch;
    while ((ch = getopt(argC, argV, "dc:s:p:f:")) != EOF) {
        switch (ch) {
            case 'd':
                // debug mode
                m_logDebug = true;
                break;

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
        cout << "[Error] Need input a domain" << endl;
        usage();
        return PARAM_PARSE_ERROR;
    }

    m_vInput.clear();
    // 解析文件输入
    if (m_inputFile != "" && parseInputFile(m_inputFile) != SUCCESS) {
        cout << "[Error] input file parse error, please check -f" << endl;
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
    cout << "Usage: dcq [options...] <domain|-f file>" << endl
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
        LOG_ERROR("Read file {} failed", path.c_str());
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

#include "spdlog/sinks/rotating_file_sink.h"
void configManager::initLog(void) {
    auto logger =
        spdlog::rotating_logger_mt("mylog", m_logFilePath + "dcq.log",
                                   m_logFileSize * 1024, m_logFileRotating);
    spdlog::set_default_logger(logger);
    if (m_logDebug) {
        spdlog::set_level(
            spdlog::level::debug);  // Set global log level to debug
        spdlog::set_pattern("%Y-%m-%d %H:%M:%S [%P][%L][%@ %!] %v");
    } else {
        spdlog::set_pattern("%Y-%m-%d %H:%M:%S [%P][%L] %v");
    }
    LOG_INFO("log init, level {}", spdlog::get_level());
}
