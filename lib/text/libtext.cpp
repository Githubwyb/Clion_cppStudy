/*
 * @Author WangYubo
 * @Date 11/26/2020
 * @Description text类型解析库
 */

#include <iostream>
#include <regex>

#include "parseBase.hpp"

DEFINE_CLASS(parserText)
DEFINE_FUNC(parserText)

using namespace std;

int parserText::parse(const KeyValueMap &rules, string &input,
                      KeyValueMap &result) {
    result.clear();
    for (auto it = rules.begin(); it != rules.end(); ++it) {
        // 正则匹配字段，放到result里面
        regex e(it->second);
        smatch sm;
        regex_search(input, sm, e);
        // 没匹配到跳过
        if (sm.size() <= 1) {
            // cout << "Can't parse " << it->first << ": " << it->second << endl;
            continue;
        }
        // 匹配到取第一个
        result[it->first] = sm[1];
    }

    return result.size() == 0 ? -1 : 0;
}
