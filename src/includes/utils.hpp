/*
 * @Author WangYubo
 * @Date 11/25/2020
 * @Description 存放一些工具函数
 */

#include <string>

class utils {
   public:
    /**
     * 返回程序所在目录，非当前目录
     * @return string 目录
     **/
    static const std::string &getProgramPath();
};
