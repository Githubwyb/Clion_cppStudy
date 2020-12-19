/*
 * @Author WangYubo
 * @Date 11/25/2020
 * @Description 配置管理器
 */
#ifndef CONFIG_MANAGER_HPP
#define CONFIG_MANAGER_HPP

#include <memory>
#include <string>
#include <vector>

// #include "baseInstance.hpp"
#include "libdcqtype.hpp"

namespace libdcq {

class configManager {
   public:
    enum {
        SUCCESS = 0,             // 成功
        FAILED = -1,             // 统一失败返回
        FILE_OPEN_ERROR = -2,    // 文件打开失败
        SERVER_CONF_EMPTY = -3,  // 没有服务器配置项
        PARAM_PARSE_ERROR = -4,  // 参数解析失败
        CONFIG_PARSE_ERROR = -5  // 配置文件解析失败
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

    /**
     * 获取所有请求服务器
     * @return std::vector<std::shared_ptr<QueryServer>> 服务器配置
     **/
    const std::vector<std::shared_ptr<QueryServer>> &getQueryServer(void) {
        return m_vQueryServer;
    }

    /**
     * 获取解析库的路径
     * @return std::string 解析库的路径
     **/
    const std::string &getParserLibPath(void) { return m_parserLibPath; }

    /**
     * @brief 获取线程池数量
     *
     * @return int 线程池数量
     */
    int getThreadNum(void) { return m_threadNum; }

    /**
     * 获取输入的域名列表
     * @return const std::vector<std::string> & 输入的域名列表
     **/
    const std::vector<std::string> &getInput(void) { return m_vInput; }

   private:
    std::string m_confPath;        // 配置路径
    std::string m_serverConfPath;  // 请求服务器配置文件路径
    std::vector<std::shared_ptr<QueryServer>> m_vQueryServer;
    std::string m_parserLibPath;        // 解析库的路径
    std::vector<std::string> m_vInput;  // 输入的要解析的字段
    std::string m_inputFile;            // 输入文件路径
    std::string m_logFilePath;          // 日志输出路径
    size_t m_logFileSize;               // 单个文件日志大小
    size_t m_logFileRotating;           // 保留几个日志文件
    bool m_logDebug = false;            // 是否开启调试
    int m_threadNum;                    // 线程池线程数量

    /**
     * 加载请求服务器配置
     * @param path string 配置路径
     * @return int 错误码
     **/
    int parseServerConf(const std::string &path);

    /**
     * 加载输入文件
     * @param path string 文件路径
     * @return int 错误码
     **/
    int parseInputFile(const std::string &path);

    /**
     * 日志初始化
     **/
    void initLog(void);
};
}  // namespace libdcq
#endif /* CONFIG_MANAGER_HPP */
