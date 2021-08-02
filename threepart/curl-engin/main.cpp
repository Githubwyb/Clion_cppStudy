#include <curl/curl.h>
#include <openssl/engine.h>
#include <openssl/ossl_typ.h>
#include <openssl/ssl.h>
#include <stdlib.h>
#include <string.h>

#include "libskf/libskf.h"

size_t dataSize = 0;

int my_trace(CURL *handle, curl_infotype type, char *data, size_t size,
             void *userp);

size_t curlWriteFunction(void *ptr, size_t size /*always==1*/, size_t nmemb,
                         void *userdata);

#define LIB_NAME "ShuttleCsp11_3000GM.dll"
#define LIB_PATH "C:\\Windows\\System32\\" LIB_NAME
static int initLibSkf() {
    LibSkfUtils skfUtils;
    std::vector<StrTreeNode> info;
    skfUtils.enumAllInfo({LIB_PATH}, info);
    LibSkf::initApi(LIB_PATH);
    LibSkf::initEngine(const_cast<LPSTR>(info[0].val.c_str()),
                       const_cast<LPSTR>(info[0].child[0].val.c_str()),
                       const_cast<LPSTR>(info[0].child[0].child[0].val.c_str()),
                       "123456");
    return 0;
}

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
        if (!SSL_CTX_set_client_cert_engine(ctx, ssl_client_engine)) {
            fprintf(stderr, "Error setting client auth engine\n");
            break;
        }
        return CURLE_OK;
    } while (false);
    ENGINE_free(ssl_client_engine);

    return CURLE_FAILED_INIT;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stdout, "Usage: CURL-ENGINE.exe <URL>\n");
        fprintf(stdout, "\texample: CURL-ENGINE.exe https://199.200.2.142\n");
        return 0;
    }
    initLibSkf();
    char *data = 0;
    CURL *const curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to init curl");
        return 1;
    }
    curl_easy_setopt(curl, CURLOPT_URL, argv[1]);

    curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_MAX_TLSv1_1);
    curl_easy_setopt(curl, CURLOPT_SSLENGINE, "skf");

    curl_easy_setopt(curl, CURLOPT_SSL_CTX_FUNCTION, sslctxfun);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, 3 * 1000);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 3 * 1000);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curlWriteFunction);

    CURLcode mc = curl_easy_perform(curl);
    if (mc != CURLE_OK) {
        fprintf(stderr, "Failed to get web page, curl error: %s\n",
                curl_easy_strerror(mc));
        return 1;
    }
    curl_easy_cleanup(curl);

    if (!data) {
        fprintf(stderr, "Got no data\n");
        return 1;
    }

    printf("Page data:\n\n%s\n", data);
    free(data);
}

size_t curlWriteFunction(void *ptr, size_t size /*always==1*/, size_t nmemb,
                         void *userdata) {
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

    fprintf(stream, "%s, %10.10ld bytes (0x%8.8lx)\n", text, (long)size,
            (long)size);

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
            char x =
                (ptr[i + c] >= 0x20 && ptr[i + c] < 0x80) ? ptr[i + c] : '.';
            fputc(x, stream);
        }

        fputc('\n', stream); /* newline */
    }
}

int my_trace(CURL *handle, curl_infotype type, char *data, size_t size,
             void *userp) {
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
