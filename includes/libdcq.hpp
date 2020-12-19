/**
 * @file libdcq.hpp
 * @author wangyubo (1061322005@qq.com)
 * @brief 对外接口头文件
 * @version 0.1
 * @date 2020/12/12
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef LIBDCQ_HPP
#define LIBDCQ_HPP

#include <future>
#include <memory>
#include <vector>

#include "libdcqtype.hpp"

namespace libdcq {

// 真实类
class dcqReal;
// 代理类，只暴露几个供外部调用的接口
// 使用LIBDCQ_API只显示接口标签，其余隐藏，防止接口冲突
class dcq {
   public:
    LIBDCQ_API dcq();
    LIBDCQ_API ~dcq();
    /**
     * @brief 初识化dcq
     *
     * @param confPath (可选)const string & ini配置文件路径
     * @return ERROR_CODE 错误码，错误信息从getErrorMsg获取
     */
    LIBDCQ_API ERROR_CODE init(const std::string &confPath = DEFAULT_CONF_PATH);

    /**
     * @brief 获取错误信息字符串，同时会清空错误信息
     *
     * @return const char * 错误字符串
     */
    LIBDCQ_API const char *getErrorMsg();

    /**
     * @brief 解析一个域名
     *
     * @param domain const std::string & 要解析的域名
     * @param result KeyValueMap & 要解析的域名
     * @return ERROR_CODE 错误码，错误信息从getErrorMsg获取
     */
    LIBDCQ_API ERROR_CODE parseOne(const std::string &, KeyValueMap &);

    /**
     * @brief 异步解析一个域名
     *
     * @param domain const std::string & 要解析的域名
     * @param result KeyValueMap & 结果储存位置
     * @return std::future<ERROR_CODE> 错误码，错误信息从getErrorMsg获取
     */
    LIBDCQ_API std::future<ERROR_CODE> asynParseOne(const std::string &,
                                                    KeyValueMap &);

    /**
     * @brief 异步解析一个域名，带回调函数
     *
     * @param domain const std::string & 要解析的域名
     * @param callback OutputFunc 回调函数
     * @return std::future<ERROR_CODE> 错误码，错误信息从getErrorMsg获取
     */
    LIBDCQ_API std::future<ERROR_CODE> asynParseOneCb(const std::string &domain,
                                                      OutputFunc callback);

    /**
     * @brief 批量解析域名
     *
     * @param vDomain const std::vector<std::string> 要解析的域名列表
     * @param vResult std::vector<std::shared_ptr<KeyValueMap>> & 返回的数据
     * @return int 解析成功的数量
     */
    LIBDCQ_API int parseBatch(const std::vector<std::string> &,
                              std::vector<std::shared_ptr<KeyValueMap>> &);

    /**
     * @brief 异步批量解析域名
     *
     * @param vDomain const std::vector<std::string> 要解析的域名列表
     * @param vResult std::vector<std::shared_ptr<KeyValueMap>> & 返回的数据
     * @return std::vector<std::future<ERROR_CODE>> 解析错误码
     */
    LIBDCQ_API std::vector<std::future<ERROR_CODE>> asynParseBatch(
        const std::vector<std::string> &,
        std::vector<std::shared_ptr<KeyValueMap>> &);

    /**
     * @brief 异步批量解析域名
     *
     * @param vDomain const std::vector<std::string> 要解析的域名列表
     * @param callback OutputFunc 回调函数
     * @return std::vector<std::future<ERROR_CODE>> 解析错误码
     */
    LIBDCQ_API std::vector<std::future<ERROR_CODE>> asynParseBatchCb(
        const std::vector<std::string> &vDomain, OutputFunc callback);

   private:
    // 此类为代理类，这里定义真实类
    dcqReal *m_pDcq;
};

}  // namespace libdcq

#endif /* LIBDCQ_HPP */
