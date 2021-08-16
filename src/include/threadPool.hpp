#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

/**
 * 用法示例

class testPool : public threadPool, public BaseInstance<testPool> {
   public:
    // 线程池取名字
    void init(int threadNum) { threadPool::init(threadNum, "testPool"); }
};

void main() {
    // 获取单例
    auto &pool = testPool::getInstance();
    // 初始化5个线程
    pool.init(5);
    // 定义返回值
    vector<future<int>> ret;

    for (int i = 0; i < 20; i++) {
        // 插入任务函数
        ret.emplace_back(pool.commit(xxxx));
    }
    for (auto &tmp : ret) {
        // 等待结果返回
        tmp.wait();
    }
}

 **/

using Task = std::function<void(void)>;
class threadPool {
   private:
    std::atomic<int> m_idleThreadNum;  //空闲线程数量
    std::atomic<bool> m_stoped;        //是否停止线程
    std::mutex m_lock;                 //线程池锁
    std::queue<Task> m_tasks;          //待执行任务
    std::condition_variable m_cv;      //线程控制
    int m_threadNum = 0;               //线程总数
    std::string m_poolName;            //线程池名称
    //线程执行函数
    void run();

   public:
    std::vector<std::thread> m_pool;   //线程池
    //构造函数
    threadPool() : m_stoped(true) {}
    ~threadPool();

    //添加线程函数
    template <class F, class... Args>
    auto commit(F &&f, Args &&... args) -> std::future<decltype(f(args...))> {
        using RetType = decltype(f(args...));

        if (m_stoped) {
            return std::future<RetType>();
        }

        auto task = std::make_shared<std::packaged_task<RetType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        auto ret = task->get_future();
        {
            std::lock_guard<std::mutex> lock(m_lock);
            m_tasks.emplace([task] { (*task)(); });
        }
        m_cv.notify_one();
        return ret;
    }

    // 初始化
    virtual void init(int threadNum, std::string name = "ThreadPool");
};

#endif
