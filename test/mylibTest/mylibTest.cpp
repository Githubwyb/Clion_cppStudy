/**
 * @file mylibTest.cpp
 * @author wangyubo (1061322005@qq.com)
 * @brief test mylib function
 * @version 0.1
 * @date 2021-07-09
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "gtest/gtest.h"
#include "log.hpp"

class MylibTestClass : public testing::Test {
   public:
    /**
     * @brief run before this test case suit
     *
     */
    static void SetUpTestCase() {
        // init log
        spdlog::set_level(
            spdlog::level::debug);  // Set global log level to debug
        spdlog::set_pattern("%Y-%m-%d %H:%M:%S [%P:%t][%L][%s:%# %!] %v");
    }

    /**
     * @brief run after this test case suit
     *
     */
    static void TearDownTestCase() {}

    /**
     * @brief run before every test case
     *
     */
    virtual void SetUp() {}

    /**
     * @brief run after every test case
     *
     */
    virtual void TearDown() {}
};

extern void init_mylib(void *pFunc);

TEST_F(MylibTestClass, test_add) { init_mylib(nullptr); }

TEST_F(MylibTestClass, test_sub) { EXPECT_TRUE(-1 == -1); }
