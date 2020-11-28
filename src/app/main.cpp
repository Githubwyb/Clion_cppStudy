/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <string>
#include <vector>

#include "bugReport.hpp"
#include "configManager.hpp"
#include "parserManager.hpp"
#include "log.hpp"
#include "utils.hpp"

using namespace std;

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

    parserManager::getInstance().parseOne("sina.com");
    LOG_DEBUG("parse again");
    auto &result = parserManager::getInstance().parseOne("sina.com");

    for (auto &item : result) {
        LOG_DEBUG("%s: '%s'", item.first.c_str(), item.second.c_str());
    }
    return 0;
}
