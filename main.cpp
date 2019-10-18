/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <string>
#include <vector>
#include <iostream>
#include <cstring>
#include "log.hpp"

using namespace std;

class Solution {
public:
    bool Find(int target, vector<vector<int>> &array) {
        int row = array.size();
        int line = array[0].size();

        int j = line - 1;
        int i = 0;
        int state = 0;
        while (i < row && j >= 0) {
            LOG_DEBUG("i %d j %d", i, j);
            if (target == array[i][j])
                return true;

            switch (state) {
                //横向排查
                case 0:
                    //从右向左找小于target的索引
                    if (target >= array[i][j]) {
                        state = 1;
                        break;
                    }
                    --j;
                    break;

                //纵向排查
                case 1:
                    //从上向下找大于target的索引
                    if (target <= array[i][j]) {
                        state = 0;
                        break;
                    }
                    ++i;
                    break;

                default:
                    printf("End, unbelievable error");
                    break;
            }
        }

        return false;
    }
};

int main(int argC, char *arg[])
{
    vector<vector<int>> array = {{0, 3, 7, 9},
                                 {1, 4, 8, 10},
                                 {2, 5, 9, 11}};
    Solution a;
    bool b = a.Find(6, array);
    LOG_DEBUG("%d", b);
    return 0;
}
