#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

class threadPool {
    using Task = std::function<void(void)>;

   private:
    std::vector<std::thread> m_pool;   //线程池
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