/*
 * @Author WangYubo
 * @Date 11/28/2020
 * @Description 输出管理器
 */
#ifndef OUTPUT_MANAGER_HPP
#define OUTPUT_MANAGER_HPP

#include <string>

#include "baseInstance.hpp"
#include "libdcq.hpp"

namespace libdcq {
class outputManager : public BaseInstance<outputManager> {
   public:
    /**
     * 打印输出结果
     * @param domain const std::string & 要解析的域名
     * @param result const KeyValueMap & 解析的结果
     **/
    void printResult(const std::string &domain, const KeyValueMap &result);
};
}

#endif /* OUTPUT_MANAGER_HPP */
