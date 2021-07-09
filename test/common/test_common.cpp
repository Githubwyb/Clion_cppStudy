/**
 * @file test_common.cpp
 * @author wangyubo (1061322005@qq.com)
 * @brief 单测总的setup和teardown
 * @version 0.1
 * @date 2021-07-09
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "gtest/gtest.h"
#include "log.hpp"
#include <iostream>

class GlobalEvent : public testing::Environment {
   public:
    virtual void SetUp() {
    }

    virtual void TearDown() {}
};
