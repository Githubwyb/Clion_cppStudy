/*
 * @Author WangYubo
 * @Date 11/26/2020
 * @Description json类型解析库
 */

#include <iostream>
#include <string>
#include <vector>

#include "parseBase.hpp"
#include "rapidjson/document.h"

DEFINE_CLASS(parserJson)
DEFINE_FUNC(parserJson)

using namespace std;
using namespace rapidjson;

//字符串分割函数
vector<string> split(string str, const string &pattern) {
    string::size_type pos;
    vector<string> result;
    str += pattern;  //扩展字符串以方便操作
    int size = str.size();
    for (int i = 0; i < size; i++) {
        pos = str.find(pattern, i);
        if (pos < size) {
            std::string s = str.substr(i, pos - i);
            result.push_back(s);
            i = pos + pattern.size() - 1;
        }
    }
    return result;
}

int parserJson::parse(const KeyValueMap &rules, string &input,
                      KeyValueMap &result) {
    // 解析json文件
    Document d;
    d.Parse(input.c_str());
    if (d.HasParseError()) {
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

        Value::ConstMemberIterator iter = d.FindMember(vKeys[0].c_str());
        if (iter == d.MemberEnd()) {
            cout << "Can't find key" << it->second << " in json " << input
                 << ", " << it->first << endl;
            continue;
        }

        auto len = vKeys.size();
        size_t i = 1;
        for (; i < len; ++i) {
            if (!iter->value.IsObject()) {
                if (i == len - 1) {
                    continue;
                }
                break;
            }

            Value::ConstMemberIterator tmp = iter->value.FindMember(vKeys[i].c_str());
            if (tmp == iter->value.MemberEnd()) {
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

        if (!iter->value.IsString()) {
            cout << "key " << it->second << " in " << input << " is not string, " << it->first << endl;
            continue;
        }

        // 匹配到取第一个
        result[it->first.c_str()] = iter->value.GetString();
    }

    return 0;
}

