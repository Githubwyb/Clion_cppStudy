/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <atomic>
#include <string>
#include <vector>

#include "bugReport.hpp"
#include "configManager.hpp"
#include "log.hpp"
#include "threadPool.hpp"
#include "utils.hpp"

using namespace std;

#include <regex>
#include <iostream>
KeyValueMap parse(KeyValueMap &rules, string &input) {
    KeyValueMap result;
    for (auto it = rules.begin(); it != rules.end(); ++it)
    {
        // LOG_DEBUG("%s, %s", it->second.c_str(), input.c_str());
        regex e(it->second);
        smatch sm;
        regex_search(input, sm, e);

        cout << "sm.prefix: " << sm.prefix() << endl;
        // LOG_DEBUG("%d", sm.size());
        for (int i = 0; i < sm.size(); ++i) {
            cout << "sm[" << i << "]: " << sm[i] << endl;
        }
        cout << "sm.suffix: " << sm.suffix() << endl;
    }
    return result;
}

int main(int argC, char *arg[]) {
    LOG_DEBUG("Hello dcq");
    (void)BugReportRegister("dcq", ".", nullptr, nullptr);

    KeyValueMap rules;
    rules["test"] = "(\\w+)\\W+(\\w+)";
    string inputStr = "@abc def--";

    auto result = parse(rules, inputStr);
    for (auto it = rules.begin(); it != rules.end(); ++it)
    {
        LOG_DEBUG("%s: %s", it->first.c_str(), it->second.c_str());
    }

    // // 读取全局配置，此配置为固定文件，程序目录（非运行目录）同级的global.ini
    // string confPath = utils::getProgramPath() + "/global.ini";
    // auto &config = configManager::getInstance();
    // auto ret = config.loadINIConf(confPath);
    // if (ret != configManager::SUCCESS) {
    //     LOG_ERROR("Open %s failed, ret %d", confPath.c_str(), ret);
    //     return -1;
    // }

    // config.showConf();

    // LOG_DEBUG("%d", ret);
    return 0;
}
