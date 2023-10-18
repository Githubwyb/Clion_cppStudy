/**
 * @file iocontextutils.hpp
 * @author 王钰博18433 (18433@sangfor.com)
 * @brief 封装ioContext的部分接口
 * @version 0.1
 * @date 2023-05-13
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <errno.h>

#include <boost/algorithm/string.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/stacktrace.hpp>
#include <boost/thread.hpp>
#include <functional>
#include <future>
#include <string>
#include <thread>

#include "ec_to_string.hpp"
#include "log.hpp"

using IOContext = boost::asio::io_context;
using IOContextPtr = std::shared_ptr<IOContext>;
using IOContextWorker = boost::asio::io_context::work;
using IOContextWorkerPtr = std::unique_ptr<IOContextWorker>;
#define IOCONTEXTUTILS_CATCH_COMMON(tag, ec)                                                  \
    catch (const std::exception &e) {                                                         \
        std::string exceptionStr = boost::diagnostic_information(e);                          \
        boost::algorithm::replace_all(exceptionStr, "\r\n", "");                              \
        boost::algorithm::replace_all(exceptionStr, "\n", "");                                \
        (ec).assign(errno, std::system_category());                                           \
        LOGE(WHAT("{}, get exception", (tag)), REASON("{}", exceptionStr), NO_WILL);          \
    }                                                                                         \
    catch (const std::error_code &error) {                                                    \
        (ec) = error;                                                                         \
        LOGE(WHAT("{}, get exception", (tag)), REASON("ec {}", std::to_string(ec)), NO_WILL); \
    }                                                                                         \
    catch (...) {                                                                             \
        std::string exceptionStr = boost::current_exception_diagnostic_information();         \
        boost::algorithm::replace_all(exceptionStr, "\r\n", "");                              \
        boost::algorithm::replace_all(exceptionStr, "\n", "");                                \
        (ec).assign(errno, std::system_category());                                           \
        LOGE(WHAT("{}, get unknown exception", (tag)), REASON("{}", exceptionStr), NO_WILL);  \
    }

class IOContextUtils {
public:
    /**
     * @brief 异步执行线程
     *
     * @param[in] pIOContext 要执行的ioContext
     * @param[in] func
     * @param[out] ec 错误信息
     * @return bool 成功还是失败，失败由ec给出错误信息
     */
    static bool runAsync(IOContext &ioContext, std::function<void()> func, std::error_code &ec) {
        const std::string tag = "run task async";
        if (!checkIOContext(tag, ioContext, ec)) return false;

        ioContext.post([func, tag]() {
            std::error_code ec;
            try {
                func();
            }
            IOCONTEXTUTILS_CATCH_COMMON(tag, ec);
        });
        return true;
    }

    /**
     * @brief 同步执行一个函数
     *
     * @param[in] pIOContext 要执行的ioContext
     * @param[in] func
     * @param[out] ec 错误信息
     * @return bool 成功还是失败，失败由ec给出错误信息
     */
    static bool runSync(IOContext &ioContext, std::function<void()> func, std::error_code &ec) {
        const std::string tag = "run task sync";
        if (!checkIOContext(tag, ioContext, ec)) return false;
        ec.clear();

        std::promise<void> taskPromise;
        auto taskFuture = taskPromise.get_future();
        // 使用dispatch防止在当前线程等待任务造成循环阻塞
        ioContext.dispatch([&taskPromise, &func, &tag, &ec]() {
            try {
                func();
            }
            IOCONTEXTUTILS_CATCH_COMMON(tag, ec);
            taskPromise.set_value();
        });
        taskFuture.wait();
        return !ec;
    }

    /**
     * @brief 同步执行一个函数，带超时，当前和ioContext同一个线程，超时无效
     *
     * @param[in] pIOContext 要执行的ioContext
     * @param[in] func
     * @param[in] timeout ms级超时，不允许为0，存在runSync不限制时间
     * @param[out] ec 错误信息
     * @return bool 成功还是失败，失败由ec给出错误信息
     */
    static bool runSyncTimeout(IOContext &ioContext, std::function<void()> func, int timeout, std::error_code &ec) {
        const std::string tag = "run task sync with timeout " + std::to_string(timeout) + "ms";
        if (!checkIOContext(tag, ioContext, ec)) return false;

        // 使用共享指针防止超时后被释放导致运行的地方崩溃
        auto pTaskPromise = std::make_shared<std::promise<std::error_code>>();
        auto taskFuture = pTaskPromise->get_future();
        // 使用dispatch防止在当前线程等待任务造成循环阻塞
        ioContext.dispatch([pTaskPromise, func, tag]() {
            std::error_code errorCode;
            try {
                func();
            }
            IOCONTEXTUTILS_CATCH_COMMON(tag, errorCode);
            pTaskPromise->set_value(errorCode);
        });

        // 防止线程没起来，这里循环等待一下
        while (taskFuture.wait_for(std::chrono::milliseconds(timeout)) == std::future_status::deferred)
            ;

        // 超时设置错误
        if (taskFuture.wait_for(std::chrono::milliseconds(timeout)) != std::future_status::ready) {
            LOGW(WHAT("{} failed, will return but the task will be executed asynchronously", tag),
                 REASON("task has been executed for more than {}ms", timeout), NO_WILL);
            ec.assign(errno, std::system_category());
            return false;
        }
        ec = taskFuture.get();
        return !ec;
    }

    /**
     * @brief 检查ioContext是否存在，并且是否可用
     *
     * @param tag 日志的tag
     * @param pIOContext 要检查的ioContext
     * @param ec 错误信息
     * @return bool 成功还是失败，失败由ec给出错误信息
     */
    static bool checkIOContext(const std::string &tag, const IOContext &ioContext, std::error_code &ec) {
        if (ioContext.stopped()) {
            LOGW(WHAT("{} failed", tag), REASON("io context is stopped"), NO_WILL);
            ec.assign(errno, std::system_category());
            return false;
        }
        return true;
    }

    static std::future<void> startIOContextThread(const std::string &threadName, IOContextPtr &pIOContext) {
        auto p = std::make_shared<std::promise<void>>();
        auto retFuture = std::async(std::launch::async, [p, threadName, pIOContext]() {
            LOGI(WHAT("{} begin", threadName));
            p->set_value();
            while (true) {
                try {
                    if (pIOContext->stopped()) pIOContext->restart();
                    IOContextWorker worker(*pIOContext);
                    pIOContext->run();
                    break;
                } catch (...) {
                    std::stringstream ss;
                    ss << boost::stacktrace::stacktrace();
                    auto errorStr = boost::current_exception_diagnostic_information();
                    auto errorStack = ss.str();
                    LOGE(WHAT("{} get exception, will restart thread after 5 second", threadName),
                         REASON("exception: {}, stack: \n{}", errorStr, errorStack), NO_WILL);
                }
                boost::this_thread::sleep_for(boost::chrono::seconds(5));
            }
            LOGI(WHAT("{} stop", threadName));
        });
        auto f = p->get_future();
        f.wait();
        return retFuture;
    }
};
