/*
 * @Author WangYubo
 * @Date 11/26/2020
 * @Description json类型解析库
 */

#include <iostream>
#include <string>
#include <vector>

#include "json.hpp"
#include "parseBase.hpp"

DEFINE_CLASS(parserJson)
DEFINE_FUNC(parserJson)

using namespace std;

//字符串分割函数
vector<string> split(string str, const string &pattern) {
    size_t pos;
    vector<string> result;
    str += pattern;  //扩展字符串以方便操作
    auto size = str.size();
    for (size_t i = 0; i < size; i++) {
        pos = str.find(pattern, i);
        if (pos < size) {
            std::string s = str.substr(i, pos - i);
            result.push_back(s);
            i = pos + pattern.size() - 1;
        }
    }
    return result;
}

int parserJson::parse(const libdcq::KeyValueMap &rules, string &input,
                      libdcq::KeyValueMap &result) {
    nlohmann::json j;
    // json文件解析
    try {
        j = nlohmann::json::parse(input);
    } catch (...) {
        cout << "parse error, input " << input << endl;
        return -1;
    }

    result.clear();
    for (auto it = rules.begin(); it != rules.end(); ++it) {
        auto vKeys = split(it->second, ".");
        if (vKeys.size() == 0) {
            cout << "value format error " << it->first << ": " << it->second
                 << endl;
            continue;
        }

        // 取第一个key
        auto iter = j[vKeys[0]];
        if (iter.is_null()) {
            cout << "Can't find key" << it->second << " in json " << input
                 << ", " << it->first << endl;
            continue;
        }

        auto len = vKeys.size();
        size_t i = 1;
        for (; i < len; ++i) {
            if (!iter.is_object()) {
                if (i == len - 1) {
                    continue;
                }
                break;
            }

            auto tmp = iter[vKeys[i]];
            if (tmp.is_null()) {
                cout << "Can't find key" << it->second << " in json " << input
                     << ", " << it->first << endl;
                iter = tmp;
                break;
            }
            iter = tmp;
        }

        if (i != len) {
            cout << "Can't find key " << it->second << " in json " << input
                 << ", " << it->first << endl;
            continue;
        }

        if (!iter.is_string()) {
            cout << "key " << it->second << " in " << input
                 << " is not string, " << it->first << endl;
            continue;
        }

        // 匹配到取第一个
        result[it->first.c_str()] = iter;
    }

    return 0;
}
