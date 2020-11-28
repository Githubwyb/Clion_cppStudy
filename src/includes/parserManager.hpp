/*
 * @Author WangYubo
 * @Date 11/28/2020
 * @Description 解析器管理
 */
#ifndef PARSER_MANAGER_HPP
#define PARSER_MANAGER_HPP

#include <memory>
#include <string>
#include <vector>

#include "baseInstance.hpp"
#include "common.hpp"

class parserManager : public BaseInstance<parserManager> {
   public:
    /**
     * 析构函数
     **/
    ~parserManager();

    /**
     * 解析一个域名
     * @param domain const std::string 要解析的域名
     * @return KeyValueMap 解析结果
     **/
    const KeyValueMap parseOne(const std::string &);

   private:
    std::vector<void *> m_vLibHandler;            // 动态库的句柄
    std::map<std::string, ParserFunc> m_mParser;  // 动态库的解析函数
    std::map<std::string, std::shared_ptr<KeyValueMap>>
        m_mParseCache;  // 域名解析缓存

    /**
     * 获取parser函数
     * @param type const std::string & parser类型
     * @return ParserFunc 根据类型加载的解析函数
     **/
    ParserFunc getParserFunc(const std::string &);

    /**
     * 获取网络请求结果
     * @param server const QueryServer & 服务器配置
     * @param domain const std::string & 要解析的域名
     * @return std::string & 请求的结果
     **/
    std::string getRequestResult(const QueryServer &, const std::string &);
};

#endif /* PARSER_MANAGER_HPP */
