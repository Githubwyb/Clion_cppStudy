/*
 * @Author WangYubo
 * @Date 11/25/2020
 * @Description 配置管理器
 */

#include <string>

#include "baseInstance.hpp"

class configManager : public BaseInstance<configManager> {
   public:
    enum {
        SUCCESS = 0,            // 成功
        FAILED = -1,            // 统一失败返回
        FILE_OPEN_ERROR = -2    // 文件打开失败
    };

    /**
     * 加载INI配置
     * @param path string 配置路径
     * @return int 错误码
     **/
    int loadINIConf(const std::string &path);

    /**
     * 打印所有配置项
     **/
    void showConf(void);

   private:
    std::string confPath;       // 配置路径
    std::string testStr;        // 测试变量
    int testInt;                // 测试变量
};
