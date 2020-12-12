/*
 * @Author WangYubo
 * @Date 11/25/2020
 * @Description 配置管理器
 */

#include "outputManager.hpp"

#include <iostream>

#include "log.hpp"

using namespace std;
using namespace libdcq;

void outputManager::printResult(const string &input,
                                const KeyValueMap &result) {
    if (result.size() == 0) {
        cout << "[Error] Can't parse domain: " << input << endl;
        return;
    }
    // 只有一个值就返回 域名: xxx
    if (result.size() == 1) {
        for (auto &item : result) {
            cout << input << ": " << item.second << endl;
        }
        return;
    }
    // 有多个值就返回 key: xxx
    for (auto &item : result) {
        cout << item.first << ": " << item.second << endl;
    }
}