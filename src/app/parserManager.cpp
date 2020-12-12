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

using namespace libdcq;

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
    // 从内存查找parser
    auto iter = m_mParser.find(type);
    if (iter != m_mParser.end()) {
        return iter->second;
    }
    return nullptr;
}

#include <dirent.h>
#include <string.h>

#define MAX_TYPE_LEN 10

ERROR_CODE parserManager::loadParsers(const std::string &parserDir) {
    LOG_DEBUG("Begin, parserDir {}", parserDir);

    auto dirHandler = opendir(parserDir.c_str());
    if (dirHandler == nullptr) {
        LOG_ERROR("Open {} failed", parserDir);
        return FAILED_NO_PARSER;
    }

    struct dirent *entry;
    bool isSuccess = false;  // 有一个加载成功就返回成功
    // 遍历目录
    while ((entry = readdir(dirHandler)) != nullptr) {
        LOG_DEBUG("Get {}, type {}", entry->d_name, entry->d_type);
        // 为目录则跳过
        if (entry->d_type & DT_DIR) {
            LOG_DEBUG("{} is a dir", entry->d_name);
            continue;
        }
        std::string fileName = entry->d_name;
        auto strLen = fileName.length();
        if (strLen < 6 || fileName.substr(0, 3) != "lib" ||
            fileName.substr(strLen - 3, 3) != ".so") {
            // 非lib(type).so的跳过
            LOG_DEBUG("{} is not an expect so file", entry->d_name);
            continue;
        }
        auto filePath =
            (parserDir.back() == '/' ? parserDir : parserDir + "/") +
            entry->d_name;
        if (SUCCESS == loadOneParser(fileName.substr(3, strLen - 6), filePath)) {
            isSuccess = true;
        }
    }
    closedir(dirHandler);

    return isSuccess ? SUCCESS : FAILED_NO_PARSER;
}

ERROR_CODE parserManager::loadOneParser(const std::string &type,
                                        const std::string &parserPath) {
    LOG_DEBUG("Begin, type {}, parserPath {}", type, parserPath);
    // 打开so库，句柄存缓存
    auto handler = dlopen(parserPath.c_str(), RTLD_NOW);
    if (handler == nullptr) {
        LOG_WARN("Can't open lib {}", parserPath.c_str());
        return FAILED_SO_OPEN;
    }

    // 加载解析函数
    auto parser = (ParserFunc)dlsym(handler, "parser");
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
        dlclose(handler);
        LOG_WARN("Cannot load symbol 'parser': {}", dlsym_error);
        return FAILED_SO_PARSE;
    }

    // 成功后存到缓存中
    m_vLibHandler.emplace_back(handler);
    m_mParser[type] = parser;

    return SUCCESS;
}
