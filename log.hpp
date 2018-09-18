/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description log file
 */

#ifndef CLION_CPPSTUDY_LOG_HPP
#define CLION_CPPSTUDY_LOG_HPP

#define DEBUG

#define LOG_HEX(data, length) Log::log_hex(data, length)
#define LOG_BIN(data, length) Log::log_binary(data, length)
#define PRINT(fmt, ...) Log::log_print(fmt, ##__VA_ARGS__)

#ifdef DEBUG

#define FileName(str) Log::splitFileName(str)

#define LOG_DEBUG(fmt, ...)                                                                       \
    Log::log_header(LOG_LEVEL_DEBUG);                                                             \
    Log::log_print("[%s:%d %s] " fmt, FileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    Log::log_print("\r\n")
#define LOG_WARN(fmt, ...)                                                                        \
    Log::log_header(LOG_LEVEL_WARN);                                                              \
    Log::log_print("[%s:%d %s] " fmt, FileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    Log::log_print("\r\n")
#define LOG_INFO(fmt, ...)                                                                        \
    Log::log_header(LOG_LEVEL_INFO);                                                              \
    Log::log_print("[%s:%d %s] " fmt, FileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    Log::log_print("\r\n")
#define LOG_ERROR(fmt, ...)                                                                       \
    Log::log_header(LOG_LEVEL_ERROR);                                                             \
    Log::log_print("[%s:%d %s] " fmt, FileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    Log::log_print("\r\n")
#define LOG_FATAL(fmt, ...)                                                                       \
    Log::log_header(LOG_LEVEL_FATAL);                                                             \
    Log::log_print("[%s:%d %s] " fmt, FileName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    Log::log_print("\r\n")
#define LOG_PRINT(fmt, ...)             \
    Log::log_print(fmt, ##__VA_ARGS__); \
    Log::log_print("\r\n")

#else

#define LOG_DEBUG(fmt, ...)
#define LOG_WARN(fmt, ...)
#define LOG_INFO(fmt, ...)
#define LOG_ERROR(fmt, ...)
#define LOG_FATAL(fmt, ...)
#define LOG_PRINT(fmt, ...)

#endif

typedef enum LOG_LEVEL {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
} LOG_LEVEL;

class Log {

public:
    /*
     * @function 以网络结构显示数据
     * @param data 数据首地址
     * @param length 数据长度
     */
    static void log_hex(const void *data, int length);

    /*
     * @function 以二进制显示数据
     * @param data 数据首地址
     * @param length 数据长度
     */
    static void log_binary(const void *data, int length);

    /*
     * @function 相当于printf的打印
     * @param fmt 格式化的数据
     * @param ... 不定变量
     */
    static void log_print(const char *fmt, ...);

    /*
     * @function 打印日志头部信息
     * @param level 日志等级
     */
    static void log_header(LOG_LEVEL level);

    /*
     * @function 切分文件名，将前面的斜杠去除
     * @param fileName 文件名
     */
    static const char *splitFileName(const char *fileName);

private:
    /*
     * @function 获得level的打印名
     * @param level level级别
     */
    static const char *getLevelString(LOG_LEVEL level);

    /*
     * @function 打印当前时间
     */
    static void print_currentTime();
};

#endif //CLION_CPPSTUDY_LOG_HPP
