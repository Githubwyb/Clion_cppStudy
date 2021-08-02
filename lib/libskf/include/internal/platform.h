/**
 * @file platform.h
 * @author wangyubo (18433@sangfor.com)
 * @brief 平台兼容头文件
 * @version 0.1
 * @date 2021-07-20
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN32
#include "skf_win.h"
#elif __linux__
#include "skf_linux.h"
#endif



#endif /* PLATFORM_H */
