/**
 * @file main.cpp
 * @brief 启动函数所在位置
 * @author wangyubo
 * @version v2.0.0
 * @date 2020-12-16
 */
#include <arpa/inet.h>
#include <curl/curl.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <openssl/bio.h>
#include <openssl/engine.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <resolv.h>
#include <sys/inotify.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <algorithm>
#include <bitset>
#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/multi_index/key_extractors.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/stacktrace.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <future>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <system_error>
#include <thread>
#include <vector>

#include "FileMonitor.hpp"
#include "baseInstance.hpp"
#include "bugReport.hpp"
#include "common.hpp"
#include "ec_to_string.hpp"
#include "iocontextutils.hpp"
#include "ip_address.hpp"
#include "log.hpp"
#include "normalize_log.hpp"
#include "socketHelper.hpp"
#include "target_address.h"
#include "threadPool.hpp"

using namespace std;

size_t dataSize = 0;

int my_trace(CURL *handle, curl_infotype type, char *data, size_t size, void *userp);

size_t curlWriteFunction(void *ptr, size_t size /*always==1*/, size_t nmemb, void *userdata);

/* The SSL initialisation callback. The callback sets:
- a private key and certificate
- a trusted ca certificate
- a preferred cipherlist
- an application verification callback (the function above)
*/
static CURLcode sslctxfun(CURL *curl, void *sslctx, void *parm) {
    SSL_CTX *ctx = (SSL_CTX *)sslctx;
    ENGINE *ssl_client_engine = NULL;

    do {
        ssl_client_engine = ENGINE_by_id("skf");
        if (ssl_client_engine == NULL) {
            fprintf(stderr, "get engine failed\n");
            break;
        }
        // if (!SSL_CTX_set_client_cert_engine(ctx, ssl_client_engine)) {
        //     fprintf(stderr, "Error setting client auth engine\n");
        //     break;
        // }
        return CURLE_OK;
    } while (false);
    ENGINE_free(ssl_client_engine);

    return CURLE_FAILED_INIT;
}

int access_curl(bool use_engine) {
    char *data = 0;
    CURL *const curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to init curl");
        return 1;
    }
    curl_easy_setopt(curl, CURLOPT_URL, "https://127.0.0.1:7777");

    curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    // curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_MAX_TLSv1_1);
    if (use_engine) {
        curl_easy_setopt(curl, CURLOPT_SSLENGINE, "skf");
        // curl_easy_setopt(curl, CURLOPT_SSLENGINE_DEFAULT, 1L);
    }

    // curl_easy_setopt(curl, CURLOPT_SSL_CTX_FUNCTION, sslctxfun);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, 3 * 1000);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 3 * 1000);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curlWriteFunction);

    int ret = 0;
    CURLcode mc = curl_easy_perform(curl);
    if (mc != CURLE_OK) {
        fprintf(stderr, "Failed to get web page, curl error: %s\n", curl_easy_strerror(mc));
        ret = 1;
        goto exit;
    }

    if (!data) {
        fprintf(stderr, "Got no data\n");
        ret = 1;
        goto exit;
    }
    printf("Page data:\n\n%s\n", data);

exit:
    curl_easy_cleanup(curl);
    if (data) free(data);
    return ret;
}

int main(int argc, char *argv[]) {
    (void)BugReportRegister("run", ".", nullptr, nullptr);
    spdlog::set_level(spdlog::level::debug);  // Set global log level to debug
    // spdlog::set_pattern("%Y-%m-%d %H:%M:%S [%P:%t][%L][%@ %!] %v");
    spdlog::set_pattern("%Y-%m-%d %H:%M:%S.%e [%P:%t][%L][%s:%# %!] %v");
    // LOGI(WHAT("Hello, main { }"));
    // LOGD(WHAT("aaaa {}", sizeof(unsigned long)));
    // LOGI(WHAT("aaaa {}", 1234));
    // LOGI(WHAT("aaaa {:#06x}", 1234));
    // LOGE(WHAT("get_svpn_rand"), REASON("hhh {}", 1), WILL("aaaaa"));
    // LOGW(WHAT("get_svpn_rand"), REASON("hhh {}", 0x1234), NO_WILL);
    // LOGI(WHAT("long {}", sizeof(long)));
    // LOGI(WHAT("why {}", 4 + sizeof(size_t) - 1));
    // LOGI(WHAT("why {}", (4 + sizeof(size_t) - 1) & (size_t) ~(sizeof(size_t) - 1)));
    LOG_INFO("use engine !!!!!!!!!!!!!!!!!!!!!!!!");
    access_curl(true);
    LOG_INFO("not use engine !!!!!!!!!!!!!!!!!!!!!!!!");
    access_curl(false);
    this_thread::sleep_for(std::chrono::seconds(3));
    LOG_INFO("exit main");
    return 0;
}

size_t curlWriteFunction(void *ptr, size_t size /*always==1*/, size_t nmemb, void *userdata) {
    char **stringToWrite = (char **)userdata;
    const char *input = (const char *)ptr;
    if (nmemb == 0) return 0;
    if (!*stringToWrite)
        *stringToWrite = (char *)malloc(nmemb + 1);
    else
        *stringToWrite = (char *)realloc(*stringToWrite, dataSize + nmemb + 1);
    memcpy(*stringToWrite + dataSize, input, nmemb);
    dataSize += nmemb;
    (*stringToWrite)[dataSize] = '\0';
    return nmemb;
}

void dump(const char *text, FILE *stream, unsigned char *ptr, size_t size) {
    size_t i;
    size_t c;
    unsigned int width = 0x10;

    fprintf(stream, "%s, %10.10ld bytes (0x%8.8lx)\n", text, (long)size, (long)size);

    for (i = 0; i < size; i += width) {
        fprintf(stream, "%4.4lx: ", (long)i);

        /* show hex to the left */
        for (c = 0; c < width; c++) {
            if (i + c < size)
                fprintf(stream, "%02x ", ptr[i + c]);
            else
                fputs("   ", stream);
        }

        /* show data on the right */
        for (c = 0; (c < width) && (i + c < size); c++) {
            char x = (ptr[i + c] >= 0x20 && ptr[i + c] < 0x80) ? ptr[i + c] : '.';
            fputc(x, stream);
        }

        fputc('\n', stream); /* newline */
    }
}

int my_trace(CURL *handle, curl_infotype type, char *data, size_t size, void *userp) {
    const char *text;
    (void)handle; /* prevent compiler warning */
    (void)userp;

    switch (type) {
        case CURLINFO_TEXT:
            fprintf(stderr, "== Info: %s", data);
        default: /* in case a new one is introduced to shock us */
            return 0;

        case CURLINFO_HEADER_OUT:
            text = "=> Send header";
            break;
        case CURLINFO_DATA_OUT:
            text = "=> Send data";
            break;
        case CURLINFO_SSL_DATA_OUT:
            text = "=> Send SSL data";
            break;
        case CURLINFO_HEADER_IN:
            text = "<= Recv header";
            break;
        case CURLINFO_DATA_IN:
            text = "<= Recv data";
            break;
        case CURLINFO_SSL_DATA_IN:
            text = "<= Recv SSL data";
            break;
    }

    //   dump(text, stderr, (unsigned char *)data, size);
    return 0;
}

