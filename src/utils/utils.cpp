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
#if 0
#include "libhv/include/hv/http_client.h"

string utils::getRequestResult(const string &url, http_method method) {
    LOG_DEBUG("Begin, url {}", url);
    HttpRequest req;
    req.method = method;
    req.url = url;
    HttpResponse res;
    auto ret = http_client_send(&req, &res);
    string result = res.Dump(false, true);
    if (ret != 0 || res.status_code != 200) {
        LOG_DEBUG("{}", result);
        LOG_WARN("* Failed, error {}, ret {}, status_code {}",
                 http_client_strerror(ret), ret, res.status_code);
        return "";
    }
    return result;
}
#endif
