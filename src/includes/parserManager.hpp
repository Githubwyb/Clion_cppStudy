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

#include "libdcqtype.hpp"

namespace libdcq {

class parserManager {
   public:
    /**
     * 析构函数
     **/
    ~parserManager();

    /**
     * 获取parser函数
     * @param type const std::string & parser类型
     * @return ParserFunc 根据类型加载的解析函数
     **/
    ParserFunc getParserFunc(const std::string &);

    /**
     * 加载所有so库到内存
     * @param parserDir const std::string & parser库的目录路径
     * @return ERROR_CODE 错误码
     **/
    ERROR_CODE loadParsers(const std::string &parserDir);

    /**
     * 加载一个so库到内存
     * @param type const std::string & parser类型
     * @param parserPath const std::string & parser库的文件路径
     * @return ERROR_CODE 错误码
     **/
    ERROR_CODE loadOneParser(const std::string &, const std::string &parserPath);

   private:
    std::vector<void *> m_vLibHandler;            // 动态库的句柄
    std::map<std::string, ParserFunc> m_mParser;  // 动态库的解析函数
};
}

#endif /* PARSER_MANAGER_HPP */
