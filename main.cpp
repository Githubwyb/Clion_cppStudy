/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include "log.hpp"

#include <iostream>
#include <memory>
#include <vector>
#include <fstream>
#include <map>

#define InputFileName "../input.txt"

using namespace std;

//按行返回数据
int readFile(string &input) {
    input = "";
    static ifstream inFile(InputFileName);
    if (!inFile) {
        LOG_ERROR("Failed to open file %s", InputFileName);
        return -1;
    }

    while (!inFile.eof()) {
        char buf[128];
        inFile.getline(buf, 128);
        input = string(buf);
        return 0;
    }

    return -1;
}

//将字母转成数字
int getIndex(char a) {
    switch (a) {
        case 'A':
            return 0;

        case 'B':
            return 1;

        case 'C':
            return 2;

        case 'D':
            return 3;

        case 'E':
            return 4;

        case 'F':
            return 5;

        default:
            return -1;
    }
}

//将数字转成字母
char getChar(int a) {
    switch (a) {
        case 0:
            return 'A';

        case 1:
            return 'B';

        case 2:
            return 'C';

        case 3:
            return 'D';

        case 4:
            return 'E';

        case 5:
            return 'F';

        default:
            return -1;
    }
}

//用于判断U集合中是否还有点
#define over (u[0] || u[1] || u[2] || u[3] || u[4] || u[5])

int main() {
    //由于已知点数量，第一行废掉
    string inputStr = "";
    if (readFile(inputStr) != 0) {
        LOG_ERROR("Read file error");
        return -1;
    }

    //定义距离矩阵
    int distance[6][6] = {0};
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 6; ++j) {
            //初始化所有距离为-1
            distance[i][j] = -1;
        }
        //自己到自己为0
        distance[i][i] = 0;
    }

    map<char, string> road;
    road['A'] = "AA";   //自己到自己
    while (readFile(inputStr) == 0) {
        int a = getIndex(inputStr[0]);      //第一个点
        int b = getIndex(inputStr[2]);      //第二个点
        int c = stoi(inputStr.substr(3));   //距离
        //赋值给距离矩阵
        LOG_INFO("%c--%c: %d", getChar(a), getChar(b), c);
        distance[a][b] = c;
        distance[b][a] = c;
    }

    //U集初始化为true
    bool u[6] = {true, true, true, true, true, true};
    while (over) {
        for (int i = 0; i < 6; ++i) {
            //可达A点的U集合中的点i
            if (u[i] && distance[i][0] != -1) {
                u[i] = false;
                //将直达的路径加进去
                if (road.count(getChar(i)) == 0) {
                    string roadStr = "A";
                    road[getChar(i)] = roadStr + getChar(i);
                }

                for (int j = 0; j < 6; ++j) {
                    //筛选出i点可到达的j点
                    if (distance[j][i] != -1) {
                        //如果j点不可到达A点或者当前距离大于A从i到达j的距离
                        if (distance[j][0] == -1 ||
                            distance[j][0] > (distance[j][i] + distance[i][0])) {
                            //更新距离表
                            distance[j][0] = distance[j][i] + distance[i][0];
                            distance[0][j] = distance[j][i] + distance[i][0];
                            //更新路径表
                            string tmpStr = road[getChar(i)];
                            road[getChar(j)] = tmpStr + getChar(j);
                        }
                    }
                }
            }
        }
    }

    //打印结果
    PRINT("\r\n");
    LOG_INFO("Result:");
    for (int m = 0; m < 6; ++m) {
        LOG_INFO("%c %s: %d", getChar(m), road[getChar(m)].c_str(), distance[m][0]);
    }
}
