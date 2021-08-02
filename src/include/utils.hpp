/*
 * @Author WangYubo
 * @Date 11/25/2020
 * @Description 存放一些工具函数
 */
#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

#include "common.hpp"

//#include "libhv/include/hv/httpdef.h"

class utils {
   public:
    /**
     * 返回程序所在目录，非当前目录
     * @return string 目录
     **/
    static const std::string &getProgramPath();

    /**
     * 获取网络请求结果
     * @param url const std::string & 要请求的url
     * @param method http_method 请求类型，具体见 enum http_metod
     * @return std::string & 请求的结果
     **/
//    static std::string getRequestResult(const std::string &, http_method method = HTTP_GET);
};

#endif /* UTILS_HPP */
