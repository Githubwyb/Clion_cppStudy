//
// Created by User on 2019/9/28.
//

#include "threadPool.hpp"
#include "log.hpp"

using namespace std;


threadPool::~threadPool() {
    m_stoped = true;
    m_cv.notify_all();
    for (auto &tmp : m_pool) {
        if (tmp.joinable()) tmp.join();
    }
}

void threadPool::init(int threadNum, string name) {
    if (!m_stoped) {
        LOG_INFO("%s has been started, thread num %d", m_poolName.c_str(),
                 m_threadNum);
        return;
    }

    {
        unique_lock<mutex> lock(m_lock);
        if (!m_stoped) {
            return;
        }
        m_stoped = false;
        // 清理旧的线程，初始化新的线程
        m_pool.clear();

        for (int i = 0; i < threadNum; i++) {
            m_pool.emplace_back(thread(&threadPool::run, this));
        }
        m_threadNum = threadNum;
        m_poolName = name;
    }
    LOG_INFO("%s start thread num %d", m_poolName.c_str(), m_threadNum);
}

void threadPool::run() {
    while (true) {
        m_idleThreadNum--;
        Task task;
        {
            unique_lock<mutex> lock(m_lock);
            m_cv.wait(lock, [this] { return m_stoped || !(m_tasks.empty()); });
            if (m_tasks.empty()) {
                return;
            }
            task = move(m_tasks.front());
            m_tasks.pop();
        }
        LOG_DEBUG("Handle one task");
        task();
        m_idleThreadNum++;
    }
}
