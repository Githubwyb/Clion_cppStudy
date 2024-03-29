/**
 * @file libcommon.hpp
 * @brief 公共头文件
 * @author wangyubo
 * @version v0.1.0
 * @date 2020-12-14
 */

#ifndef LIB_COMMON_HPP
#define LIB_COMMON_HPP

#if WIN32
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API __attribute__((visibility("default")))
#endif

#endif /* LIB_COMMON_HPP */

