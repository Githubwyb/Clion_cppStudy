/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include "baseInstance.hpp"
#include "log.hpp"

using namespace std;
using Task = function<void(void)>;
static const int s_threadNum = 4;  //线程数量

atomic<int> addNum;
atomic<int> result;

class threadPool : public BaseInstance<threadPool> {
   private:
    vector<thread> m_pool;        //线程池
    atomic<int> m_idleThreadNum;  //空闲线程数量
    atomic<bool> m_stoped;        //是否停止线程
    mutex m_lock;                 //线程池锁
    queue<Task> m_tasks;          //待执行任务
    condition_variable m_cv;      //线程控制
    //线程执行函数
    void run();

   public:
    //构造函数
    threadPool();

    ~threadPool();

    //添加线程函数
    void commit(function<void(string)>, const string &param);
};

threadPool::threadPool() {
    m_stoped = false;
    for (size_t i = 0; i < s_threadNum; i++) {
        m_pool.emplace_back(thread(&threadPool::run, this));
    }
}

threadPool::~threadPool() {
    m_stoped = true;
    m_cv.notify_all();
    for (auto &tmp : m_pool) {
        if (tmp.joinable()) tmp.join();
    }
    LOG_DEBUG("result %d, addNum %d", result.load(), addNum.load());
}

void threadPool::commit(function<void(string)> task, const string &param) {
    if (m_stoped) {
        LOG_INFO("ThreadPool has been stoped");
        return;
    }

    {
        lock_guard<mutex> lock(m_lock);
        m_tasks.emplace([task, param] { task(param); });
    }
    m_cv.notify_one();
}

void threadPool::run() {
    while (true) {
        m_idleThreadNum--;
        Task task;
        {
            unique_lock<mutex> lock(m_lock);
            m_cv.wait(lock,
                      [this] { return this->m_stoped || !(m_tasks.empty()); });
            if (m_tasks.empty()) {
                return;
            }
            task = move(m_tasks.front());
            m_tasks.pop();
        }
        task();
        m_idleThreadNum++;
    }
}

void test(string str) {
    int add = 0;
    while (true) {
        add = addNum++;
        if (add > 10000) {
            return;
        }
        result += add;
    }
}

int main(int argC, char *arg[]) {
    LOG_DEBUG("Hello");
    addNum = 0;
    result = 0;
    threadPool &pool = threadPool::getInstance();
    for (int i = 0; i < 20; i++) {
        char str[50] = {0};
        sprintf(str, "task id %d", i);
        string a = str;
        pool.commit(test, a);
    }
    LOG_DEBUG("End");
    return 0;
}
