/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <algorithm>
#include <map>
#include <queue>
#include <stack>
#include <vector>

#include "log.hpp"

using namespace std;

class Solution {
   public:
    // 最小堆解问题，C++标准接口
    static vector<int> GetLeastNumbers_Solution3(vector<int> input, int k) {
        int inputSize = input.size();
        if (k == 0 || k > inputSize) {
            return {};
        }

        vector<int> result(k + 1);
        // 插入前k个值并构造最大堆
        for (int i = 0; i < k; i++) {
            result[i] = input[i];
            push_heap(result.begin(), result.begin() + i + 1);
        }

        // 判断，如果大于最大节点，替换并重构最大堆
        for (int i = k; i < inputSize; i++) {
            if (input[i] < result[0]) {
                result[k] = input[i];
                pop_heap(result.begin(), result.end());
            }
        }

        // 多余一个删掉
        result.pop_back();
        return result;
    }

    // 最小堆解问题
    static vector<int> GetLeastNumbers_Solution2(vector<int> input, int k) {
        int inputSize = input.size();
        if (k == 0 || k > inputSize) {
            return {};
        }

        vector<int> result(k);
        // 插入前k个值并构造最大堆
        for (int i = 0; i < k; i++) {
            result[i] = input[i];
            sortTailNodeOfMaxHeap(result, i + 1);
        }

        // 判断，如果大于最大节点，替换并排序
        for (int i = k; i < inputSize; i++) {
            if (input[i] < result[0]) {
                result[0] = input[i];
                sortHeadNodeOfMaxHeap(result, k);
            }
        }

        return result;
    }

    // 遍历，找到最小的k个值
    static vector<int> GetLeastNumbers_Solution1(vector<int> input, int k) {
        int inputSize = input.size();
        if (k == 0 || k > inputSize) {
            return {};
        }
        vector<int> result(k);
        int size = 1;
        result[0] = input[0];
        // 遍历数组
        for (int i = 1; i < inputSize; i++) {
            // 不到k，且大于最后一位，直接放到最后一位
            if (size < k && input[i] > result[size - 1]) {
                result[size] = input[i];
                size++;
                continue;
            }

            // 数组个数不到k，或者tmp小于数组最大值
            LOG_DEBUG("size %d, result[size - 1] %d, input[i] %d", size,
                      result[size - 1], input[i]);
            if (size < k || input[i] < result[size - 1]) {
                if (size < k) {
                    // 不到k，最后一位在上面判断了肯定大于输入
                    result[size] = result[size - 1];
                    size++;
                }
                // 从最后一位开始，如果前一位大于tmp，后移，直到不大于跳出
                int j = size - 1;
                for (; j > 0 && result[j - 1] > input[i]; --j) {
                    LOG_DEBUG("j %d, result[j - 1] %d", j, result[j - 1]);
                    result[j] = result[j - 1];
                }
                // 跳出后，j指向要放的位置
                result[j] = input[i];
            }
        }
        return result;
    }

   private:
    // 将尾节点排序到最大堆中
    static void sortTailNodeOfMaxHeap(vector<int> &heap, int length) {
        int index = length;
        while (index != 1 && heap[index - 1] > heap[index / 2 - 1]) {
            int tmp = heap[index - 1];
            heap[index - 1] = heap[index / 2 - 1];
            heap[index / 2 - 1] = tmp;
            index = index / 2;
        }
    }

    // 将头节点排序到最大堆中
    static void sortHeadNodeOfMaxHeap(vector<int> &heap, int length) {
        int father = 1;
        for (int child = father * 2; child <= length; child = father * 2) {
            // 左孩子大于父亲，且没有右孩子或者左孩子大于右孩子，交换左孩子和父亲
            if (heap[child - 1] > heap[father - 1] &&
                (child == length || heap[child - 1] > heap[child])) {
                int tmp = heap[child - 1];
                heap[child - 1] = heap[father - 1];
                heap[father - 1] = tmp;
                father = child;
                continue;
            }

            // 有右孩子且右孩子大于父亲，交换右孩子和父亲
            if (child != length && heap[child] > heap[father - 1]) {
                int tmp = heap[child];
                heap[child] = heap[father - 1];
                heap[father - 1] = tmp;
                father = child + 1;
                continue;
            }
            break;
        }
    }
};

int main(int argC, char *arg[]) {
    vector<int> tmp =
        Solution::GetLeastNumbers_Solution3({4, 5, 1, 6, 2, 7, 3, 8}, 4);
    for (auto &tmp1 : tmp) {
        LOG_DEBUG("%d", tmp1);
    }
    return 0;
}