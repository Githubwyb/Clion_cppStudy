/*
 * @Author WangYubo
 * @Date 11/28/2020
 * @Description 解析器管理
 */

#include "parserManager.hpp"

#include <dlfcn.h>

#include "configManager.hpp"
#include "libhv/include/hv/http_client.h"
#include "log.hpp"

parserManager::~parserManager() {
    // 关闭加载的所有动态链接库
    for (auto &item : m_vLibHandler) {
        if (item != nullptr) {
            dlclose(item);
        }
    }
}

ParserFunc parserManager::getParserFunc(const std::string &type) {
    auto iter = m_mParser.find(type);
    if (iter != m_mParser.end()) {
        return iter->second;
    }

    // 根据类型加载动态库
    auto &config = configManager::getInstance();
    auto libPath = config.getParserLibPath() + "lib" + type + ".so";
    auto handler = dlopen(libPath.c_str(), RTLD_NOW);
    if (handler == nullptr) {
        LOG_WARN("Can't open lib %s", libPath.c_str());
        // 保存，下次报错也不用加载
        m_mParser[type] = nullptr;
        return nullptr;
    }
    m_vLibHandler.emplace_back(handler);

    // 加载解析函数
    auto parser = (ParserFunc)dlsym(handler, "parser");
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
        LOG_WARN("Cannot load symbol 'parser': %s", dlsym_error);
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
    auto iter = m_mParseCache.find(domain);
    if (iter != m_mParseCache.end()) {
        // 找到返回缓存数据
        return *(iter->second);
    }

    auto &config = configManager::getInstance();
    auto vQueryServer = config.getQueryServer();
    for (auto &server : vQueryServer) {
        LOG_DEBUG("%s", server->url.c_str());
        // 获取解析函数
        auto reqResult = getRequestResult(*server, domain);
        if (reqResult == "") {
            continue;
        }

        // 解析
        auto parser = getParserFunc(server->type);
        KeyValueMap result;
        // 获取结果
        auto ret = parser(server->result, reqResult, result);
        if (ret != 0) {
            LOG_WARN("parse failed, reqResult %s", reqResult.c_str());
            continue;
        }

        m_mParseCache[domain] = std::make_shared<KeyValueMap>(result);
        return *m_mParseCache[domain];
    }
    return {};
}

std::string parserManager::getRequestResult(const QueryServer &queryServer,
                                            const std::string &domain) {
    HttpRequest req;
    req.method = HTTP_GET;
    char url[MAX_URL_LEN + 1] = {0};
    string format = queryServer.url + queryServer.param;
    snprintf(url, MAX_URL_LEN, format.c_str(), "sina.com");
    req.url = url;
    LOG_DEBUG("url %s", req.url.c_str());
    HttpResponse res;
    auto ret = http_client_send(&req, &res);
    if (ret != 0) {
        printf("* Failed:%s:%d\n", http_client_strerror(ret), ret);
        return "";
    }

    return std::move(res.Dump(false, true));
}
