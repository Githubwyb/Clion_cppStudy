/**
 * @file dcq.hpp
 * @author wangyubo (1061322005@qq.com)
 * @brief 公共类型定义头文件
 * @version 0.1
 * @date 2020-12-12
 *
 * @copyright Copyright (c) 2020
 *
 */
#ifndef DCQ_HPP
#define DCQ_HPP

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <stack>
#include <string>

#include "configManager.hpp"
#include "libdcq.hpp"
#include "parserManager.hpp"

namespace libdcq {

class parserPool;
class dcqReal {
   public:
    dcqReal() : m_inited(true) {}

    /**
     * @brief 初识化dcq
     *
     * @param confPath  (可选)const string & ini配置文件路径
     * @return ERROR_CODE 错误码，错误信息从getErrorMsg获取
     */
    ERROR_CODE init(const std::string &confPath);

    /**
     * @brief 获取错误信息字符串，同时会清空错误信息
     *
     * @return const char* 错误字符串
     */
    const char *getErrorMsg();

    /**
     * @brief 解析一个域名
     *
     * @param domain const std::string 要解析的域名
     * @return ERROR_CODE 错误码，错误信息从getErrorMsg获取
     **/
    ERROR_CODE parseOne(const std::string &, KeyValueMap &);

    /**
     * @brief 批量解析域名
     *
     * @param vDomain const std::vector<std::string> 要解析的域名列表
     * @param vResult std::vector<std::shared_ptr<KeyValueMap>> & 返回的数据
     * @return int 解析成功的数量
     */
    int parseBatch(const std::vector<std::string> &,
                          std::vector<std::shared_ptr<KeyValueMap>> &);

   private:
    std::stack<std::string> m_errMsg;         // 错误信息栈
    std::atomic<bool> m_inited;               // 是否初始化
    std::mutex m_lock;                        // 锁变量
    std::shared_ptr<configManager> m_conf;    // 配置管理指针
    std::shared_ptr<parserManager> m_parser;  // 解析器管理指针
    std::shared_ptr<parserPool> m_parserPool;  // 解析器线程池指针
    std::map<std::string, std::shared_ptr<KeyValueMap>>
        m_mParseCache;  // 域名解析缓存

    /**
     * @brief 设置错误信息
     *
     * @param const std::string & 错误信息字符串
     **/
    void setErrorMsg(const char *fmt, ...);

    /**
     * @brief 任务函数，静态成员函数
     *
     * @param p dcqReal * 类指针，为了调用parserOne方法
     * @param domain const std::string & 域名
     * @param result KeyValueMap & 结果储存的map
     * @return ERROR_CODE 错误码，错误信息从getErrorMsg获取
     */
    static ERROR_CODE run(dcqReal *p, const std::string &domain, KeyValueMap &result);
};

}  // namespace libdcq

#endif /* DCQ_HPP */
