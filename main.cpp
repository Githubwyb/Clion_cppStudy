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

    map<char, string> road;                 //路由表定义
    while (readFile(inputStr) == 0) {
        int a = getIndex(inputStr[0]);      //第一个点
        int b = getIndex(inputStr[2]);      //第二个点
        int c = stoi(inputStr.substr(3));   //距离
        //赋值给距离矩阵
        LOG_INFO("%c--%c: %d", getChar(a), getChar(b), c);
        distance[a][b] = c;
    }

    while (true) {
        bool change = false;
        for (int i = 0; i < 6; ++i) {
            //U集合中A点可到达的点i
            if (distance[0][i] != -1) {
                //将路径加进去
                if (road.count(getChar(i)) == 0) {
                    string roadStr = "A";
                    road[getChar(i)] = roadStr + getChar(i);
                }

                for (int j = 0; j < 6; ++j) {
                    //筛选出i点可到达的j点
                    if (distance[i][j] != -1) {
                        //A点不可到达j点或者当前已计算的距离大于A从i到达j的距离
                        if (distance[0][j] == -1 ||
                            distance[0][j] > (distance[0][i] + distance[i][j])) {
                            change = true;
                            //更新距离表
                            distance[0][j] = distance[0][i] + distance[i][j];
                            //更新路由表
                            string tmpStr = road[getChar(i)];
                            road[getChar(j)] = tmpStr + getChar(j);
                        }
                    }
                }
            }
        }
        //一轮没有改变则证明已经计算完毕
        if (!change) {
            break;
        }
    }
    //打印距离表
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 6; ++j) {
            PRINT("%d\t", distance[i][j]);
        }
        PRINT("\r\n");
    }
    //打印路由表
    PRINT("\r\n");
    LOG_INFO("Result:");
    for (int m = 0; m < 6; ++m) {
        LOG_INFO("%c %s: %d", getChar(m), road[getChar(m)].c_str(), distance[0][m]);
    }
}
