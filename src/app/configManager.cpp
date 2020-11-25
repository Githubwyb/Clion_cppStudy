/*
 * @Author WangYubo
 * @Date 11/25/2020
 * @Description 配置管理器
 */

#include "configManager.hpp"

#include <string>

#include "iniReader/INIReader.h"
#include "log.hpp"

using namespace std;

int configManager::loadINIConf(const string &path) {
    auto iniReader = INIReader(path);
    auto ret = iniReader.ParseError();
    if (ret != 0) {
        LOG_ERROR("file %s open failed, ret %d", path.c_str(), ret);
        return FILE_OPEN_ERROR;
    }

    // 赋值各个配置项
    confPath = path;
    testStr = iniReader.GetString("test", "str", "aaa");
    testInt = iniReader.GetInteger("test", "str", 5);

    return SUCCESS;
}

void configManager::showConf() {
    LOG_INFO("Show conf:");
    PRINT("==============================\n");
    PRINT("    confPath:    '%s'\n", confPath.c_str());
    PRINT("    testStr:     '%s'\n", testStr.c_str());
    PRINT("    testInt:     %d\n", testInt);
    PRINT("==============================\n");
}
