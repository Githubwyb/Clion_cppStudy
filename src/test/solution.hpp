/**
 * @file solution.hpp
 * @brief 算法头文件
 * @author wangyubo
 * @version v0.0.1
 * @date 2021-03-17
 */

#include <list>
#include <unordered_map>
#include <vector>

using namespace std;

class lru_cache {
   public:
    lru_cache(int k) : cache_size(k) {
        m_lruList.clear();
        m_cache.clear();
    }
    void set(int key, int value) {
        // 先查找value
        auto it = m_cache.find(key);
        if (it != m_cache.end()) {
            // 找到修改值
            (*it->second).second = value;
            // 找到，将缓存插入到最前面
            m_lruList.splice(m_lruList.begin(), m_lruList, it->second);
            return;
        }

        // 没找到，插入
        // 判断是否超过最大个数，删除最后一个，map也要删除
        if (m_lruList.size() >= cache_size) {
            auto delIt = m_lruList.back();
            m_cache.erase(delIt.first);
            m_lruList.pop_back();
        }

        m_lruList.emplace_front(make_pair(key, value));
        m_cache[key] = m_lruList.begin();
    }

    int get(int key) {
        // 先查找value
        auto it = m_cache.find(key);
        if (it == m_cache.end()) {
            // 没找到返回-1
            return -1;
        }

        // 找到，将缓存插入到最前面
        m_lruList.splice(m_lruList.begin(), m_lruList, it->second);
        // 这里返回值
        return (*it->second).second;
    }

   private:
    int cache_size;                  // cache最大大小
    list<pair<int, int>> m_lruList;  // 用于存放lru的链表，值为value
    unordered_map<int, list<pair<int, int>>::iterator>
        m_cache;  // 用于O(1)查找的map
};

class Solution {
   public:
    /**
     * lru design
     * @param operators int整型vector<vector<>> the ops
     * @param k int整型 the k
     * @return int整型vector
     */
    vector<int> LRU(vector<vector<int>>& operators, int k) {
        // write code here
        lru_cache lru(k);
        vector<int> result;
        for (auto& item : operators) {
            if (item[0] == 1) {
                lru.set(item[1], item[2]);
            } else if (item[0] == 2) {
                result.emplace_back(lru.get(item[1]));
            }
        }
        return result;
    }
};
