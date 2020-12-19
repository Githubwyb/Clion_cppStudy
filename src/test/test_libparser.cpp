#include <assert.h>
#include <errno.h>
#include <stdio.h>

#include <iostream>

// #define DEBUG

#include "libdcq.hpp"
#include "log.hpp"
#include "xtest/xtest.h"

using namespace std;
using namespace libdcq;

static const char *defaultGlobalIni =
    "/mnt/e/work/github_files/Clion_cppStudy/build/global.ini";
static const vector<string> vDomain = {"baidu.com", "sina.com", "hustwyb.cn"};

// 单个域名测试
TEST(test, single) {
    auto testDcq = dcq();
    EXPECT_EQ(SUCCESS, testDcq.init(defaultGlobalIni));

    KeyValueMap result;
    EXPECT_EQ(SUCCESS, testDcq.parseOne(vDomain[0], result));
    EXPECT_TRUE(result.size() > 0);
}

// 批量场景测试
TEST(test, batch) {
    auto testDcq = dcq();
    EXPECT_EQ(SUCCESS, testDcq.init(defaultGlobalIni));

    vector<shared_ptr<KeyValueMap>> vResult;
    auto count = testDcq.parseBatch(vDomain, vResult);
    EXPECT_EQ(vDomain.size(), count);
    for (size_t i = 0; i < vResult.size(); i++) {
        EXPECT_TRUE(vResult[i]->size() > 0);
    }
}

#include <sys/time.h>

static suseconds_t getIntevel(struct timeval &startTime,
                              struct timeval &endTime) {
    return ((endTime.tv_sec - startTime.tv_sec) * 1000000 + endTime.tv_usec -
            startTime.tv_usec);
}

// 异步单个场景测试
TEST(test, asyn_single) {
    auto testDcq = dcq();
    EXPECT_EQ(SUCCESS, testDcq.init(defaultGlobalIni));

    struct timeval startTime;
    gettimeofday(&startTime, nullptr);

    KeyValueMap result;
    auto ret = testDcq.asynParseOne(vDomain[0], result);

    struct timeval endTime;
    gettimeofday(&endTime, nullptr);

    // 异步要求0.001秒内返回
    EXPECT_TRUE(getIntevel(startTime, endTime) < 1000);

    ret.wait();
    EXPECT_EQ(SUCCESS, ret.get());
    EXPECT_TRUE(result.size() > 0);
}

// 异步批量场景测试
TEST(test, asyn_batch) {
    auto testDcq = dcq();
    EXPECT_EQ(SUCCESS, testDcq.init(defaultGlobalIni));

    vector<shared_ptr<KeyValueMap>> vResult;

    struct timeval startTime;
    gettimeofday(&startTime, nullptr);

    auto vRet = testDcq.asynParseBatch(vDomain, vResult);

    struct timeval endTime;
    gettimeofday(&endTime, nullptr);

    // 异步要求0.001秒内返回
    EXPECT_TRUE(getIntevel(startTime, endTime) < 1000);

    for (size_t i = 0; i < vDomain.size(); i++) {
        EXPECT_EQ(SUCCESS, vRet[i].get());
        EXPECT_TRUE(vResult[i]->size() > 0);
    }
}

static vector<bool> vFlag;
/**
 * @brief 回调函数测试
 *
 * @param domain const std::string & 域名
 * @param result KeyValueMap & 结果字符串
 */
static void callback(const std::string &domain, KeyValueMap &result) {
    LOG_DEBUG("Domain %s, result.size %d", domain.c_str(), result.size());
    for (size_t i = 0; i < vDomain.size(); i++) {
        if (domain == vDomain[i]) {
            LOG_DEBUG("check domain %s", domain.c_str());
            if (result.size() > 0) {
                // 域名查到有结果，设置为true
                vFlag[i] = true;
            }
            break;
        }
    }
}

// 异步回调批量场景测试
TEST(test, asyn_single_cb) {
    auto testDcq = dcq();
    EXPECT_EQ(SUCCESS, testDcq.init(defaultGlobalIni));

    vFlag.clear();
    for (auto &item : vDomain) {
        vFlag.emplace_back(false);
    }
    struct timeval startTime;
    gettimeofday(&startTime, nullptr);

    auto ret = testDcq.asynParseOneCb(vDomain[0], callback);

    struct timeval endTime;
    gettimeofday(&endTime, nullptr);

    // 异步要求0.001秒内返回，不阻塞
    EXPECT_TRUE(getIntevel(startTime, endTime) < 1000);

    ret.wait();
    EXPECT_EQ(SUCCESS, ret.get());
    EXPECT_TRUE(vFlag[0]);
}

// 异步回调批量场景测试
TEST(test, asyn_batch_cb) {
    auto testDcq = dcq();
    EXPECT_EQ(SUCCESS, testDcq.init(defaultGlobalIni));

    vFlag.clear();
    for (auto &item : vDomain) {
        vFlag.emplace_back(false);
    }

    struct timeval startTime;
    gettimeofday(&startTime, nullptr);

    auto vRet = testDcq.asynParseBatchCb(vDomain, callback);

    struct timeval endTime;
    gettimeofday(&endTime, nullptr);

    // 异步要求0.001秒内返回
    EXPECT_TRUE(getIntevel(startTime, endTime) < 1000);
    for (size_t i = 0; i < vDomain.size(); i++) {
        EXPECT_EQ(SUCCESS, vRet[i].get());
        LOG_DEBUG("vFlag[%d] = %d", i, vFlag[i]);
        EXPECT_TRUE(vFlag[i]);
    }
}

// 未初始化场景测试
TEST(test, uninited) {
    auto testDcq = dcq();

    // 单个
    KeyValueMap result;
    EXPECT_EQ(FAILED_UNINITED, testDcq.parseOne(vDomain[0], result));
    // 批量
    vector<shared_ptr<KeyValueMap>> vResult;
    EXPECT_EQ(0, testDcq.parseBatch(vDomain, vResult));
    // 异步单个
    EXPECT_EQ(FAILED_UNINITED, testDcq.asynParseOne(vDomain[0], result).get());
    // 异步批量
    auto vRet = testDcq.asynParseBatch(vDomain, vResult);
    for (size_t i = 0; i < vDomain.size(); i++) {
        EXPECT_EQ(FAILED_UNINITED, vRet[i].get());
    }

    // 异步回调单个
    EXPECT_EQ(FAILED_UNINITED,
              testDcq.asynParseOneCb(vDomain[0], nullptr).get());
    // 异步回调批量
    vRet = testDcq.asynParseBatchCb(vDomain, nullptr);
    for (size_t i = 0; i < vDomain.size(); i++) {
        EXPECT_EQ(FAILED_UNINITED, vRet[i].get());
    }
}

int main(int argc, char **argv) { return xtest_start_test(argc, argv); }
