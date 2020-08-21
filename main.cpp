/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include "baseInstance.hpp"
#include "threadPool.hpp"
#include "log.hpp"

#include <atomic>
#include <vector>
#include <future>

using namespace std;

class testPool : public threadPool, public BaseInstance<testPool> {
   public:
    void init(int threadNum) { threadPool::init(threadNum, "testPool"); }
};

atomic<int> addNum;
atomic<int> result;
int test() {
    int add = 0;
    while (true) {
        add = addNum++;
        if (add > 10000) {
            return add;
        }
        result += add;
    }
}

int main(int argC, char *arg[]) {
    LOG_DEBUG("Hello");
    testPool &pool = testPool::getInstance();
    pool.init(5);
    vector<future<int>> ret;

    for (int i = 0; i < 20; i++) {
        ret.emplace_back(pool.commit(test));
    }
    for (auto &tmp : ret) {
        tmp.wait();
    }

    LOG_DEBUG("End, result %d", result.load());
    return 0;
}
