/**
 * @file fileMonitor.cpp
 * @author 王钰博18433 (18433@sangfor.com)
 * @brief 文件变化监控
 * @version 0.1
 * @date 2022-09-08
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifdef __linux__

#include <resolv.h>
#include <sys/inotify.h>

#include <functional>
#include <future>
#include <string>
#include <vector>

#include "ec_to_string.hpp"
#include "log.hpp"

class FileMonitor final {
public:
    explicit FileMonitor(std::string path) : m_filePath(std::move(path)) {}

    ~FileMonitor() { stop(); }

    void start() {
        stop();
        std::promise<void> p;
        m_monitorFuture = std::make_shared<std::future<void>>(std::async(std::launch::async, [&p, this]() {
            p.set_value();
            std::string tag = "file(" + m_filePath + ") monitor loop";
            LOGI(WHAT("{} begin", tag));

            {
                std::lock_guard<std::mutex> lock(m_fdMutex);
                // 设置为非阻塞方式
                m_monitorFD = inotify_init1(IN_NONBLOCK);
                if (m_monitorFD < 0) {
                    LOGE(WHAT("{} failed", tag),
                         REASON("inotify_init failed, ec: {}",
                                std::to_string(std::error_code(errno, std::system_category()))),
                         WILL("exit thread and won't watch file"));
                    return;
                }
            }

            // 添加文件监听的函数，返回false说明总的文件描述符被关闭
            if (!addFileWatch()) {
                LOGI(WHAT("{}, stop monitor", tag));
                return;
            }

            // 开始监听文件
            while (true) {
                char buf[sizeof(struct inotify_event) + m_filePath.size() + 1] = {0};
                ssize_t len;
                {
                    std::lock_guard<std::mutex> lock(m_fdMutex);
                    if (m_monitorFD < 0 || m_watchFD < 0) {
                        // 这里说明外部关闭了监听fd，也就是析构了，退出就好
                        break;
                    }
                    // 非阻塞读取event事件
                    len = read(m_monitorFD, buf, sizeof(buf));
                }
                if (len < 0) {
                    auto err = errno;
                    // 非阻塞的读不到事件返回EAGAIN，直接continue，其他的报错continue
                    if (err != EAGAIN) {
                        LOGW(WHAT("{}, read get err", tag),
                             REASON("get error {}", std::to_string(std::error_code(err, std::system_category()))),
                             WILL("retry after 10 ms"));
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    continue;
                }

                if (len < sizeof(struct inotify_event)) {
                    LOGW(WHAT("{}, read len error", tag), REASON("len {}, need {}", len, sizeof(struct inotify_event)),
                         WILL("retry after 10 ms"));
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    continue;
                }

                int index = 0;
                bool isChanged = false;
                // 这里怕读到多个事件，使用while处理
                do {
                    auto event = reinterpret_cast<struct inotify_event *>(buf + index);
                    // index向后移动evnet和name的长度
                    index += sizeof(struct inotify_event) + event->len;
                    LOGD(WHAT("event wd {}, mask {}, cookie {}, len {}, name {}", event->wd, event->mask, event->cookie,
                              event->len, event->name));
                    {
                        std::lock_guard<std::mutex> lock(m_fdMutex);
                        if (event->wd != m_watchFD) {
                            continue;
                        }
                    }
                    // 老的句柄被移除后，这里read会有一次ignore
                    if (event->mask & (IN_IGNORED)) {
                        continue;
                    }

                    // 到这里说明文件存在改动
                    isChanged = true;

                    // 这个文件被删除或重命名，需要重新进行添加
                    if (event->mask & (IN_DELETE_SELF | IN_MOVE_SELF)) {
                        LOGI(WHAT("{}, file {} was renamed or deleted, retry open", tag, m_filePath));
                        if (!addFileWatch()) {
                            LOGI(WHAT("{}, stop monitor", tag));
                            return;
                        }
                    }

                } while (index < len);

                if (isChanged) {
                    // 文件发生变化
                    LOGI(WHAT("{}, file change", tag));
                    notifyChangeCallback();
                }
            }

            LOGI(WHAT("{} end", tag));
            {
                std::lock_guard<std::mutex> lock(m_fdMutex);
                if (m_monitorFD >= 0) {
                    if (m_watchFD >= 0) {
                        inotify_rm_watch(m_monitorFD, m_watchFD);
                        m_watchFD = -1;
                    }
                    close(m_monitorFD);
                    m_monitorFD = -1;
                }
            }
        }));
        p.get_future().wait();
    }

    void stop() {
        if (m_monitorFuture == nullptr || !m_monitorFuture->valid()) {
            return;
        }
        do {
            // 这里将总的fd关闭，将线程从read阻塞释放出来，然后将watch移除
            std::lock_guard<std::mutex> lock(m_fdMutex);
            if (m_monitorFD >= 0) {
                if (m_watchFD >= 0) {
                    inotify_rm_watch(m_monitorFD, m_watchFD);
                    m_watchFD = -1;
                }
                close(m_monitorFD);
                m_monitorFD = -1;
            }
            // 可能还没打开就关闭了，这里确保一下future是正常的，防止死锁
        } while (m_monitorFuture->wait_for(std::chrono::milliseconds(1)) != std::future_status::ready);
        m_monitorFuture = nullptr;
    }

    /**
     * @brief 是否正在监控
     * @return
     */
    bool isRunning() { return m_monitorFuture != nullptr; }

    /**
     * @brief 添加文件变化回调
     * @param callback
     */
    void registerChangeCallback(const std::function<void()> &callback) {
        std::lock_guard<std::mutex> lock(m_callbackMutex);
        m_callbacks.emplace_back(callback);
    }

    /**
     * @brief 移除文件变化回调
     * @param callback
     */
    void unregisterChangeCallback(std::function<void()> callback) {
        std::lock_guard<std::mutex> lock(m_callbackMutex);
        for (auto it = m_callbacks.begin(); it != m_callbacks.end(); ++it) {
            if (it->target<void *>() == callback.target<void *>()) {
                m_callbacks.erase(it);
                break;
            }
        }
    }

private:
    std::string m_filePath;
    std::shared_ptr<std::future<void>> m_monitorFuture;
    std::mutex m_fdMutex;
    int m_monitorFD = -1;                            // inotify的句柄
    int m_watchFD = -1;                              // 被添加的文件句柄
    std::mutex m_callbackMutex;                      // 对回调函数加锁
    std::vector<std::function<void()>> m_callbacks;  // 回调函数

    /**
     * @brief 通知变更的观察者，此文件变更
     */
    void notifyChangeCallback() {
        std::vector<std::function<void()>> callbacks;
        {
            std::lock_guard<std::mutex> lock(m_callbackMutex);
            callbacks = m_callbacks;
        }
        for (auto &callback : callbacks) {
            callback();
        }
    }

    /**
     * @brief 添加一个文件到列表中
     *
     * @return 添加成功还是失败
     */
    bool addFileWatch() {
        std::string tag = "add file(" + m_filePath + ") to watch";
        {
            std::lock_guard<std::mutex> lock(m_fdMutex);
            if (m_monitorFD < 0) {
                return false;
            }
            // 关闭上一次的句柄
            if (m_watchFD >= 0) {
                LOGI(WHAT("{}, remove watch fd {}", tag, m_watchFD));
                inotify_rm_watch(m_monitorFD, m_watchFD);
                m_watchFD = -1;
            }
        }
        uint32_t failedCount = 0;
        // 最大重试32位最大值的次数
        while (++failedCount) {
            // 重新加锁判断是否monitor已经被关了
            {
                std::lock_guard<std::mutex> lock(m_fdMutex);
                if (m_monitorFD < 0) {
                    // 这里说明外部关闭了监听fd，也就是析构了，退出就好
                    return false;
                }
                // 监听所有事件，除了访问，打开和关闭，主要监听修改删除，添加成功直接返回
                m_watchFD = inotify_add_watch(m_monitorFD, m_filePath.c_str(),
                                              IN_ALL_EVENTS ^ (IN_ACCESS | IN_OPEN | IN_CLOSE));
                if (m_watchFD > 0) {
                    LOGI(WHAT("{}, add watch success, fd {}", tag, m_watchFD));
                    return true;
                }
            }
            if ((failedCount % 100) == 1) {
                LOGW(WHAT("{}, inotify_add_watch failed", tag),
                     REASON("failed {} times, ec: {}", failedCount,
                            std::to_string(std::error_code(errno, std::system_category()))),
                     WILL("retry"));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        LOGE(WHAT("{} failed", tag),
             REASON("reach max failed count {}, ec: {}", failedCount - 1, m_filePath,
                    std::to_string(std::error_code(errno, std::system_category()))),
             WILL("stop watch file"));
        return false;
    }
};

#endif
