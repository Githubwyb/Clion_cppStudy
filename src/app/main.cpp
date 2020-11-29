/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <string>
#include <vector>
#include <iostream>

#include "bugReport.hpp"
#include "configManager.hpp"
#include "log.hpp"
#include "parserManager.hpp"
#include "utils.hpp"

using namespace std;

int main(int argC, char *argV[]) {
    LOG_DEBUG("Hello dcq");
    (void)BugReportRegister("dcq", ".", nullptr, nullptr);

    auto &config = configManager::getInstance();
    // 解析命令行参数，将用命令行参数替换全局配置
    int ret = config.getCmdLineParam(argC, argV);
    if (ret != configManager::SUCCESS) {
        LOG_ERROR("parse cmd line param failed, ret %d", ret);
        return -1;
    }
    // 读取全局配置，此配置为固定文件，程序目录（非运行目录）同级的global.ini
    string confPath = utils::getProgramPath() + "/global.ini";
    ret = config.loadINIConf(confPath);
    if (ret != configManager::SUCCESS) {
        LOG_ERROR("Open %s failed, ret %d", confPath.c_str(), ret);
        return -1;
    }

    for (auto &input : config.getInput()) {
        auto &result = parserManager::getInstance().parseOne(input.c_str());
        for (auto &item : result) {
            cout << input << ": " << item.second << endl;
        }
    }
    return 0;
}
