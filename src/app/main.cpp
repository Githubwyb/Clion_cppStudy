/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <atomic>
#include <string>
#include <vector>

#include "configManager.hpp"
#include "log.hpp"
#include "threadPool.hpp"
#include "utils.hpp"
#include "ensure/ensure.h"

using namespace std;

int main(int argC, char *arg[]) {
    LOG_DEBUG("Hello dcq");

    // 读取全局配置，此配置为固定文件，程序目录（非运行目录）同级的global.ini
    string confPath = utils::getProgramPath() + "/global.ini";
    auto &config = configManager::getInstance();
    auto ret = config.loadINIConf(confPath);
    ENSURE(ret == configManager::SUCCESS)(ret).msg("loadINIConf must be success");

    config.showConf();

    LOG_DEBUG("%d", ret);
    return 0;
}
