/*
 * @Author WangYubo
 * @Date 11/26/2020
 * @Description 编写so需要用到的基类头文件
 */

#include "common.hpp"

class parserBase {
   public:
    /**
     * 解析函数
     * @param rules const KeyValueMap & 解析规则
     * @param rules std::string & 解析目标字符串
     * @param result KeyValueMap 解析结果
     * @return KeyValueMap 解析结果
     **/
    virtual int parse(const KeyValueMap &, std::string &, KeyValueMap &) = 0;
};

// 声明一个外部可用的标识符
#define DEFINE_FUNC(child)                                      \
    extern "C" int parser(const KeyValueMap &a, std::string &b, \
                          KeyValueMap &c) {                     \
        child parser;                                           \
        return parser.parse(a, b, c);                           \
    }

// 声明子类
#define DEFINE_CLASS(child)                                            \
    class child : public parserBase {                                 \
       public:                                                        \
        int parse(const KeyValueMap &, std::string &, KeyValueMap &); \
    };
