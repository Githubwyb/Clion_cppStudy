/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include "log.hpp"
#include "sqlite3.h"
#include <string>
#include <string.h>
#include <cassert>

static int get_IOS_cert_callback(void *data, int cols, char **p_values, char **p_heads) {
    assert(data != NULL && p_values != NULL);
    if (p_values[0] != NULL) {
        const char *nameStart = strstr(p_values[0], ": ");
        if (nameStart == nullptr) {
            nameStart = p_values[0];
        } else {
            nameStart += 2;
        }

        LOG_HEX(nameStart, strlen(nameStart));
        strcpy((char *) data, nameStart);
    } else {
        memset(data, 0, 1);
    }

    return 0;
}


//从数据库读取企业签名证书
static int get_IOS_CorpSignCert(void *cert) {
    // 构造sql语句
    char buf_sql[] = "select subject from emm_cert_infos where cert_type=1";

    // 从数据库中读取数据
    sqlite3 *db;
    int ret_sql = sqlite3_open("../sslvpn.sq3", &db);
    if (ret_sql != SQLITE_OK) {
        LOG_ERROR("open db error");
        return -1;
    } else {
        LOG_DEBUG("open db success");
    }

    char *errorMsg = nullptr;
    ret_sql = sqlite3_exec(db, buf_sql, get_IOS_cert_callback, cert, &errorMsg);
    if (ret_sql != SQLITE_OK) {
        LOG_ERROR("sqlite3_exec error, %s", errorMsg);
        // 关闭数据库连接
        sqlite3_close(db);
        return -1;
    } else {
        LOG_DEBUG("sqlite3_exec success");
    }
    // 关闭数据库连接
    sqlite3_close(db);
    return 0;
}


int main(int argC, char *arg[]) {
    char cert[128] = {0};

    if (get_IOS_CorpSignCert(cert) != 0) {
        LOG_ERROR("Get ios corpSignCert error");
        return -1;
    }

    LOG_DEBUG("cert: %s", cert);
    return 0;
}
