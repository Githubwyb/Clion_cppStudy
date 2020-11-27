/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <curl/curl.h>

#include <atomic>
#include <string>
#include <vector>

#include "bugReport.hpp"
#include "configManager.hpp"
#include "log.hpp"
#include "threadPool.hpp"
#include "utils.hpp"

using namespace std;

#define MAX_URL_LEN 255
#include <iostream>
#include <regex>

#include "libhv/include/hv/http_client.h"
KeyValueMap parse(KeyValueMap &rules, string &input) {
    KeyValueMap result;
    for (auto it = rules.begin(); it != rules.end(); ++it) {
        // LOG_DEBUG("%s", it->second.c_str());
        regex e(it->second);
        smatch sm;
        regex_search(input, sm, e);

        if (sm.size() <= 1) {
            LOG_WARN("Can't parse %s: %s", it->first.c_str(), it->second.c_str());
            continue;
        }

        result[it->first] = sm[1];
    }
    return result;
}

int main(int argC, char *arg[]) {
    LOG_DEBUG("Hello dcq");
    (void)BugReportRegister("dcq", ".", nullptr, nullptr);

    // 读取全局配置，此配置为固定文件，程序目录（非运行目录）同级的global.ini
    string confPath = utils::getProgramPath() + "/global.ini";
    auto &config = configManager::getInstance();
    auto ret = config.loadINIConf(confPath);
    if (ret != configManager::SUCCESS) {
        LOG_ERROR("Open %s failed, ret %d", confPath.c_str(), ret);
        return -1;
    }

    auto vQueryServer = config.getQueryServer();
    for (auto &server : vQueryServer) {
        HttpRequest req;
        req.method = HTTP_GET;
        char url[MAX_URL_LEN + 1] = {0};
        string format = server->url + server->param;
        snprintf(url, MAX_URL_LEN, format.c_str(), "sina.com");
        req.url = url;
        LOG_DEBUG("url %s", req.url.c_str());
        // req.body = "hello,world!";
        HttpResponse res;
        ret = http_client_send(&req, &res);
        // printf("%s\n", req.Dump(true,true).c_str());
        if (ret != 0) {
            printf("* Failed:%s:%d\n", http_client_strerror(ret), ret);
            return ret;
        }

        string resultBody = res.Dump(false, true);
        auto result = parse(server->result, resultBody);
        for (auto it = result.begin(); it != result.end(); ++it) {
            LOG_DEBUG("%s: %s", it->first.c_str(), it->second.c_str());
        }
    }

    config.showConf();

    LOG_DEBUG("%d", ret);
    return 0;
}
