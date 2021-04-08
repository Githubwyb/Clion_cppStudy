/**
 * @file solution.hpp
 * @brief 算法头文件
 * @author wangyubo
 * @version v0.0.1
 * @date 2021-03-17
 */

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

class Solution {
   public:
    void printV(vector<vector<int>> &data) {
        for (auto &tmp : data) {
            for (auto &item : tmp) {
                printf("%d ", item);
            }
            printf("\n");
        }
    }
    /**
     *
     * @param matrix int整型vector<vector<>> the matrix
     * @return int整型
     */
    int minPathSum(vector<vector<int>> &matrix) {
        // write code here
        int row = matrix.size();
        if (row == 0) return 0;
        int column = matrix[0].size();
        if (column == 0) return 0;
        // row traverse
        vector<vector<int>> distance(row, vector<int>(column, 0));
        distance[0][0] = matrix[0][0];
        for (int i = 1; i < row; ++i) {
            distance[i][0] = distance[i - 1][0] + matrix[i][0];
        }
        for (int j = 1; j < column; ++j) {
            distance[0][j] = distance[0][j - 1] + matrix[0][j];
        }
        for (int i = 1; i < row; ++i) {
            for (int j = 1; j < column; ++j) {
                distance[i][j] = (distance[i - 1][j] < distance[i][j - 1]
                                      ? distance[i - 1][j]
                                      : distance[i][j - 1]) +
                                 matrix[i][j];
            }
        }
        printV(matrix);
        printf("\n");
        printV(distance);
        return distance.back().back();
    }
};
