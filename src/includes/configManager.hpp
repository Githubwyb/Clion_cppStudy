/*
 * @Author WangYubo
 * @Date 11/25/2020
 * @Description 配置管理器
 */

#include <memory>
#include <string>
#include <vector>

#include "baseInstance.hpp"
#include "common.hpp"

class configManager : public BaseInstance<configManager> {
   public:
    enum {
        SUCCESS = 0,          // 成功
        FAILED = -1,          // 统一失败返回
        FILE_OPEN_ERROR = -2  // 文件打开失败
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
    std::string m_confPath;        // 配置路径
    std::string m_serverConfPath;  // 请求服务器配置文件路径
    std::vector<std::shared_ptr<QueryServer>> m_vQueryServer;

    /**
     * 加载请求服务器配置
     * @param path string 配置路径
     * @return int 错误码
     **/
    int parseServerConf(const std::string &path);
};
