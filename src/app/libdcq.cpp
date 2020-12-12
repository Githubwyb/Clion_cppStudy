/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description 代理类实现
 */

#include "libdcq.hpp"

#include "dcq.hpp"

using namespace libdcq;
using namespace std;

dcq::dcq() { m_pDcq = new dcqReal(); }

dcq::~dcq() { delete m_pDcq; }

const char *dcq::getErrorMsg() { return m_pDcq->getErrorMsg(); }

ERROR_CODE dcq::init(const string &confPath) { return m_pDcq->init(confPath); }

ERROR_CODE dcq::parseOne(const string &domain, KeyValueMap &result) {
    return m_pDcq->parseOne(domain, result);
}

std::future<ERROR_CODE> dcq::asynParseOne(const std::string &domain,
                                          KeyValueMap &result) {
    return m_pDcq->asynParseOne(domain, result);
}

int dcq::parseBatch(const std::vector<std::string> &vDomain,
                    std::vector<std::shared_ptr<KeyValueMap>> &vResult) {
    return m_pDcq->parseBatch(vDomain, vResult);
}

std::vector<std::future<ERROR_CODE>> dcq::asynParseBatch(
    const std::vector<std::string> &vDomain,
    std::vector<std::shared_ptr<KeyValueMap>> &vResult) {
    return m_pDcq->asynParseBatch(vDomain, vResult);
}
