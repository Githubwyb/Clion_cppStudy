/*
 * @Author WangYubo
 * @Date 11/25/2020
 * @Description 存放一些工具函数
 */

#include "utils.hpp"
#include "log.hpp"

#include <libgen.h>
#include <unistd.h>

using namespace std;

// 程序所在目录的最大长度
#define MAX_PROGRAM_PATH_LEN 255

const string &utils::getProgramPath() {
    static string path = "";
    if (path == "") {
        // 获取程序所在目录
        char programPath[MAX_PROGRAM_PATH_LEN] = {0};
        readlink("/proc/self/exe", programPath, MAX_PROGRAM_PATH_LEN);
        // 取路径
        dirname(programPath);
        path = programPath;
        LOG_DEBUG("Get path %s", path.c_str());
    }
    return path;
}
