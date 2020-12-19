/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include "dcq.hpp"

#include <iostream>

#include "log.hpp"
#include "utils.hpp"

using namespace libdcq;
using namespace std;

#define SET_ERRMSG(fmt, ...)                                                 \
    setErrorMsg("[%s:%d %s] " fmt, utils::splitFileName(__FILE__), __LINE__, \
                __FUNCTION__, ##__VA_ARGS__)

const char *dcqReal::getErrorMsg() {
    auto errLen = m_errMsg.size();
    if (errLen == 0) {
        return "No error msg here";
    }

    // 静态用于输出用
    static string errMsg;
    // 只有一个错误就直接输出
    if (errLen) {
        errMsg = m_errMsg.top();
        m_errMsg.pop();
        return errMsg.c_str();
    }

    errMsg = "";
    // 拼接错误信息输出，多条加个索引
    for (size_t i = 0; i < errLen; ++i) {
        errMsg += to_string(i) + " " + m_errMsg.top() + "\n";
        m_errMsg.pop();
    }
    return errMsg.c_str();
}

#include "threadPool.hpp"
class libdcq::parserPool : public threadPool {
   public:
    // 线程池取名字
    void init(int threadNum) { threadPool::init(threadNum, "parserPool"); }
};

ERROR_CODE dcqReal::init(const string &confPath) {
    if (m_inited) {
        LOG_INFO("lib has been inited");
        return SUCCESS;
    }
    auto retCode = SUCCESS;
    do {
        unique_lock<mutex> lock(m_lock);
        if (m_inited) {
            LOG_INFO("lib has been inited");
            break;
        }

        // 配置管理器加载配置
        m_conf = make_shared<configManager>();
        auto ret = m_conf->loadINIConf(confPath);
        if (ret != configManager::SUCCESS) {
            retCode = FAILED_INI_PARSE;
            SET_ERRMSG("Failed to parse %s, ret %d", confPath.c_str(), ret);
            break;
        }

        // 解析管理器加载配置
        m_parser = make_shared<parserManager>();
        ret = m_parser->loadParsers(m_conf->getParserLibPath());
        if (ret != configManager::SUCCESS) {
            retCode = FAILED_NO_PARSER;
            SET_ERRMSG("Failed to load so, path %s, ret %d",
                       m_conf->getParserLibPath().c_str(), ret);
            break;
        }

        // 初始化解析线程池
        m_parserPool = make_shared<parserPool>();
        m_parserPool->init(m_conf->getThreadNum());

        // init结束
        m_inited = true;
    } while (false);

    return retCode;
}

#define MAX_URL_LEN 255

ERROR_CODE dcqReal::parseOne(const string &domain, KeyValueMap &result) {
    LOG_DEBUG("Begin, domain {}", domain);
    if (!m_inited) {
        // 为初始化直接返回错误码
        LOG_ERROR("Hasn't been inited");
        SET_ERRMSG("Hasn't been inited");
        return FAILED_UNINITED;
    }
    {
        // 缓存读写要加锁
        unique_lock<mutex> lock(m_lock);
        auto iter = m_mParseCache.find(domain);
        if (iter != m_mParseCache.end()) {
            LOG_DEBUG("Use cache data");
            // 找到返回缓存数据
            result = *(iter->second);
            return SUCCESS;
        }
    }

    // 根据请求服务器信息进行查询解析
    auto vQueryServer = m_conf->getQueryServer();
    for (auto &server : vQueryServer) {
        LOG_DEBUG("{}", server->url);
        // 获取解析函数
        auto parser = m_parser->getParserFunc(server->type);
        if (parser == nullptr) {
            LOG_WARN("Can't load server parser, url {}, type {}", server->url,
                     server->type);
            continue;
        }

        // 发送网络请求，对结果进行解析
        char url[MAX_URL_LEN + 1] = {0};
        string format = server->url + server->param;
        snprintf(url, MAX_URL_LEN, format.c_str(), domain.c_str());
        auto reqResult = utils::getRequestResult(
            url, server->requestType == "GET" ? HTTP_GET : HTTP_POST);
        if (reqResult == "") {
            LOG_WARN("reqResult is empty, url {}, type {}", server->url,
                     server->type);
            continue;
        }

        // 获取结果
        auto ret = parser(server->result, reqResult, result);
        if (ret != 0) {
            LOG_WARN("parse failed, ret {}, reqResult {}", ret, reqResult);
            continue;
        }

        {
            // 缓存读写要加锁
            unique_lock<mutex> lock(m_lock);
            m_mParseCache[domain] = make_shared<KeyValueMap>(result);
        }
        return SUCCESS;
    }
    return FAILED;
}

// 任务函数，封装一层，非静态成员函数无法当做任务函数，并且结果使用指针传递，不能用引用
ERROR_CODE dcqReal::run(dcqReal *p, const string &domain, KeyValueMap *result) {
    LOG_DEBUG("Begin, domain {}", domain);
    return p->parseOne(domain, *result);
}

ERROR_CODE dcqReal::runCb(dcqReal *p, const std::string &domain,
                          OutputFunc callback) {
    LOG_DEBUG("Begin, domain {}", domain);
    KeyValueMap result;
    auto ret = p->parseOne(domain, result);
    LOG_INFO("run(cb) parseOne ret {}", ret);
    if (ret == SUCCESS && callback) {
        LOG_DEBUG("Call callback");
        callback(domain, result);
    }
    return ret;
}

int dcqReal::parseBatch(const vector<string> &vDomain,
                        vector<shared_ptr<KeyValueMap>> &vResult) {
    if (!m_inited) {
        // 为初始化直接返回错误码
        LOG_ERROR("Hasn't been inited");
        SET_ERRMSG("Hasn't been inited");
        vResult = vector<shared_ptr<KeyValueMap>>(vDomain.size(),
                                                  make_shared<KeyValueMap>());
        return 0;
    }
    vResult.clear();

    // 定义返回值，使用线程池跑解析，加快解析速度
    auto ret = asynParseBatch(vDomain, vResult);

    int count = 0;
    for (size_t i = 0; i < vDomain.size(); ++i) {
        // 等待结果返回
        if (ret[i].get() == SUCCESS) {
            count++;
        } else {
            LOG_ERROR("Parse failed, ret {}, domain {}", ret[i].get(),
                      vDomain[i]);
        }
    }
    return count;
}

future<ERROR_CODE> dcqReal::asynParseOne(const string &domain,
                                         KeyValueMap &result) {
    if (!m_inited) {
        // 为初始化直接返回错误码
        LOG_ERROR("Hasn't been inited");
        SET_ERRMSG("Hasn't been inited");
        return async([] { return FAILED_UNINITED; });
    }
    return m_parserPool->commit(run, this, domain, &result);
}

std::future<ERROR_CODE> dcqReal::asynParseOneCb(const std::string &domain,
                                                OutputFunc callback) {
    if (!m_inited) {
        // 为初始化直接返回错误码
        LOG_ERROR("Hasn't been inited");
        SET_ERRMSG("Hasn't been inited");
        return async([] { return FAILED_UNINITED; });
    }
    return m_parserPool->commit(runCb, this, domain, callback);
}

vector<future<ERROR_CODE>> dcqReal::asynParseBatch(
    const vector<string> &vDomain, vector<shared_ptr<KeyValueMap>> &vResult) {
    vResult.clear();
    // 定义返回值
    vector<future<ERROR_CODE>> vRet;
    if (!m_inited) {
        // 为初始化直接返回错误码
        LOG_ERROR("Hasn't been inited");
        SET_ERRMSG("Hasn't been inited");
        for (size_t i = 0; i < vDomain.size(); ++i) {
            vRet.emplace_back(async([] { return FAILED_UNINITED; }));
            vResult.emplace_back(make_shared<KeyValueMap>());
        }
        return vRet;
    }

    for (auto &item : vDomain) {
        // 外部申请结果内存
        auto pResult = make_shared<KeyValueMap>();
        // 插入任务函数
        vRet.emplace_back(asynParseOne(item, *pResult));
        // 仅仅放入指针，具体指针指向的内存改动，由任务函数完成
        vResult.emplace_back(pResult);
    }
    return vRet;
}

std::vector<std::future<ERROR_CODE>> dcqReal::asynParseBatchCb(
    const std::vector<std::string> &vDomain, OutputFunc callback) {
    // 定义返回值
    vector<future<ERROR_CODE>> vRet;
    if (!m_inited) {
        // 为初始化直接返回错误码
        LOG_ERROR("Hasn't been inited");
        SET_ERRMSG("Hasn't been inited");
        for (size_t i = 0; i < vDomain.size(); ++i) {
            vRet.emplace_back(async([] { return FAILED_UNINITED; }));
        }
        return vRet;
    }

    for (auto &item : vDomain) {
        // 插入任务函数
        vRet.emplace_back(asynParseOneCb(item, callback));
    }
    return vRet;
}

#include <stdarg.h>
void dcqReal::setErrorMsg(const char *fmt, ...) {
    char errMsg[MAX_ERROR_MSG_LEN + 1];
    va_list arg;
    va_start(arg, fmt);
    vsnprintf(errMsg, MAX_ERROR_MSG_LEN, fmt, arg);
    va_end(arg);
    m_errMsg.push(errMsg);
}
