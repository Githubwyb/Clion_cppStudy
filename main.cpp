/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <queue>
#include <stack>
#include <vector>

#include "log.hpp"

using namespace std;

class Solution {
   public:
    static vector<int> printMatrix(vector<vector<int> > matrix) {
        if (matrix.empty()) {
            return {};
        }
        int index[4] = {0};
        index[2] = matrix[0].size() - 1;
        index[3] = matrix.size() - 1;
        char state = 0;
        vector<int> result;
        while (index[0] <= index[2] && index[1] <= index[3]) {
            switch (state) {
                //第一行
                case 0:
                    for (int i = index[0]; i <= index[2]; i++) {
                        result.push_back(matrix[index[1]][i]);
                    }
                    index[1]++;
                    state++;
                    break;

                //最后一列
                case 1:
                    for (int i = index[1]; i <= index[3]; i++) {
                        result.push_back(matrix[i][index[2]]);
                    }
                    index[2]--;
                    state++;
                    break;

                //最后一行
                case 2:
                    for (int i = index[2]; i >= index[0]; i--) {
                        result.push_back(matrix[index[3]][i]);
                    }
                    index[3]--;
                    state++;
                    break;

                //第一列
                default:
                    for (int i = index[3]; i >= index[1]; i--) {
                        result.push_back(matrix[i][index[0]]);
                    }
                    index[0]++;
                    state = 0;
                    break;
            }
        }
        return result;
    }
};

void printVector(const vector<int> &data) {
    LOG_DEBUG("result:");
    for (auto tmp : data) {
        PRINT("%d ", tmp);
    }
    PRINT("\r\n");
}

int main(int argC, char *arg[]) {
    printVector(Solution::printMatrix(
        {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}}));
    return 0;
}
