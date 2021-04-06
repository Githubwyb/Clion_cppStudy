/**
 * @file solution.hpp
 * @brief 算法头文件
 * @author wangyubo
 * @version v0.0.1
 * @date 2021-03-17
 */

#include <algorithm>
#include <vector>
#include <unordered_map>

using namespace std;

class Solution {
   public:
    /**
     *
     * @param arr int整型vector the array
     * @return int整型
     */
    int maxLength(vector<int>& arr) {
        // write code here
        unordered_map<int, int> um_map;
        int len = arr.size();
        int result = 0;
        for (int i = 0; i < len; i++) {
            // if value in map, map size may be the result, then i = last common value + 1
            auto it = um_map.find(arr[i]);
            if (it != um_map.end()) {
                int mapLen = um_map.size();
                result = result > mapLen ? result : mapLen;
                i = it->second + 1;
                um_map.clear();
            }
            um_map[arr[i]] = i;
        }
        int mapLen = um_map.size();
        result = result > mapLen ? result : mapLen;
        return result;
    }
};
