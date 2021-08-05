/**
 * @file types.hpp
 * @author 王钰博18433
 * @brief libutils的类型头文件
 * @version 0.1
 * @date 2020-12-18
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef LIBUTILS_TYPES_HPP
#define LIBUTILS_TYPES_HPP

namespace libutils {

enum ERROR_CODE {
    SUCCESS,       // 成功
    OUT_OF_RANGE,  // 超出范围
    KEY_ERROR      // 键值错误
};
}

#endif /* LIBUTILS_TYPES_HPP */

