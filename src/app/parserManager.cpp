/*
 * @Author WangYubo
 * @Date 11/28/2020
 * @Description 解析器管理
 */

#include "parserManager.hpp"

#include <dlfcn.h>

#include "configManager.hpp"
#include "log.hpp"
#include "utils.hpp"

parserManager::~parserManager() {
    // 关闭加载的所有动态链接库
    for (auto &item : m_vLibHandler) {
        if (item != nullptr) {
            dlclose(item);
        }
    }
}

ParserFunc parserManager::getParserFunc(const std::string &type) {
    LOG_DEBUG("Begin, type {}", type);
    auto iter = m_mParser.find(type);
    if (iter != m_mParser.end()) {
        return iter->second;
    }

    // 根据类型加载动态库
    auto &config = configManager::getInstance();
    auto libPath = config.getParserLibPath() + "lib" + type + ".so";
    auto handler = dlopen(libPath.c_str(), RTLD_NOW);
    if (handler == nullptr) {
        LOG_WARN("Can't open lib {}", libPath.c_str());
        // 保存，下次报错也不用加载
        m_mParser[type] = nullptr;
        return nullptr;
    }
    m_vLibHandler.emplace_back(handler);

    // 加载解析函数
    auto parser = (ParserFunc)dlsym(handler, "parser");
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
        LOG_WARN("Cannot load symbol 'parser': {}", dlsym_error);
        dlclose(handler);
        // 保存，下次报错也不用加载
        m_mParser[type] = nullptr;
        return nullptr;
    }
    m_mParser[type] = parser;

    return parser;
}

#define MAX_URL_LEN 255

const KeyValueMap parserManager::parseOne(const std::string &domain) {
    LOG_DEBUG("Begin, domain {}", domain);
    auto iter = m_mParseCache.find(domain);
    if (iter != m_mParseCache.end()) {
        // 找到返回缓存数据
        return *(iter->second);
    }

    auto &config = configManager::getInstance();
    auto vQueryServer = config.getQueryServer();
    for (auto &server : vQueryServer) {
        LOG_DEBUG("{}", server->url);
        // 获取解析函数
        auto parser = getParserFunc(server->type);
        if (parser == nullptr) {
            LOG_WARN("Can't load server parser, url {}, type {}", server->url,
                     server->type);
            continue;
        }

        // 发送网络请求，对结果进行解析
        char url[MAX_URL_LEN + 1] = {0};
        std::string format = server->url + server->param;
        snprintf(url, MAX_URL_LEN, format.c_str(), domain.c_str());
        auto reqResult = utils::getRequestResult(
            url, server->requestType == "GET" ? HTTP_GET : HTTP_POST);
        if (reqResult == "") {
            LOG_WARN("reqResult is empty, url {}, type {}", server->url,
                     server->type);
            continue;
        }

        KeyValueMap result;
        // 获取结果
        auto ret = parser(server->result, reqResult, result);
        if (ret != 0) {
            LOG_WARN("parse failed, ret {}, reqResult {}", ret, reqResult);
            continue;
        }

        m_mParseCache[domain] = std::make_shared<KeyValueMap>(result);
        return *m_mParseCache[domain];
    }
    return {};
}
