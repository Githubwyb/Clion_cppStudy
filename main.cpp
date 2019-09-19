/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include "log.hpp"
#include <vector>
#include <cstring>

static int b64_decode_table[256] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* 00-0F */
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* 10-1F */
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,  /* 20-2F */
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,  /* 30-3F */
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,  /* 40-4F */
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,  /* 50-5F */
        -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,  /* 60-6F */
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,  /* 70-7F */
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* 80-8F */
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* 90-9F */
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* A0-AF */
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* B0-BF */
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* C0-CF */
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* D0-DF */
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* E0-EF */
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1   /* F0-FF */
};

int Base64_Decode(const unsigned char *in, int inlen, unsigned char *space, int size) {
    const char *cp;
    int space_idx, phase;
    int d, prev_d = 0;
    unsigned char c;
    int i = 0;

    if (inlen < 0 || size < 0) {
        return -1;
    } else if (0 == inlen) {
        return 0;
    }

    space_idx = 0;
    phase = 0;
    /* for ( cp = (char*)in; *cp != '\0'; ++cp ) */
    for (cp = (char *) in; i < inlen; ++cp, ++i) {
        d = b64_decode_table[(int) *cp];
        if (d != -1) {
            switch (phase) {
                case 0:
                    ++phase;
                    break;
                case 1:
                    c = (unsigned char) ((prev_d << 2) | ((d & 0x30) >> 4));
                    if (space_idx < size)
                        space[space_idx++] = c;
                    else
                        return -1;
                    ++phase;
                    break;
                case 2:
                    c = (unsigned char) (((prev_d & 0xf) << 4) | ((d & 0x3c) >> 2));
                    if (space_idx < size)
                        space[space_idx++] = c;
                    else
                        return -1;
                    ++phase;
                    break;
                case 3:
                    c = (unsigned char) (((prev_d & 0x03) << 6) | d);
                    if (space_idx < size)
                        space[space_idx++] = c;
                    else
                        return -1;
                    phase = 0;
                    break;
            }
            prev_d = d;
        }
    }
    return space_idx;
}

const char *base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int base64_decode(const char *base64, unsigned char *bindata) {
    int i, j;
    unsigned char k;
    unsigned char temp[4];
    for (i = 0, j = 0; base64[i] != '\0'; i += 4) {
        memset(temp, 0xFF, sizeof(temp));
        for (k = 0; k < 64; k++) {
            if (base64char[k] == base64[i])
                temp[0] = k;
        }
        for (k = 0; k < 64; k++) {
            if (base64char[k] == base64[i + 1])
                temp[1] = k;
        }
        for (k = 0; k < 64; k++) {
            if (base64char[k] == base64[i + 2])
                temp[2] = k;
        }
        for (k = 0; k < 64; k++) {
            if (base64char[k] == base64[i + 3])
                temp[3] = k;
        }

        bindata[j++] = ((unsigned char) (((unsigned char) (temp[0] << 2)) & 0xFC)) |
                       ((unsigned char) ((unsigned char) (temp[1] >> 4) & 0x03));
        if (base64[i + 2] == '=')
            break;

        bindata[j++] = ((unsigned char) (((unsigned char) (temp[1] << 4)) & 0xF0)) |
                       ((unsigned char) ((unsigned char) (temp[2] >> 2) & 0x0F));
        if (base64[i + 3] == '=')
            break;

        bindata[j++] = ((unsigned char) (((unsigned char) (temp[2] << 6)) & 0xF0)) |
                       ((unsigned char) (temp[3] & 0x3F));
    }
    return j;
}

const char plist_base64[] = "PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiPz4NCjwhRE9DVFlQRSBwbGlzdCBQVUJMSUMgIi0vL0FwcGxlLy9EVEQgUExJU1QgMS4wLy9FTiIgImh0dHA6Ly93d3cuYXBwbGUuY29tL0RURHMvUHJvcGVydHlMaXN0LTEuMC5kdGQiPg0KPHBsaXN0IHZlcnNpb249IjEuMCI+DQogIDxkaWN0Pg0KICAgIDxrZXk+aXRlbXM8L2tleT4NCiAgICA8YXJyYXk+DQogICAgICA8ZGljdD4NCiAgICAgICAgPGtleT5hc3NldHM8L2tleT4NCiAgICAgICAgPGFycmF5Pg0KICAgICAgICAgIDxkaWN0Pg0KICAgICAgICAgICAgPGtleT5raW5kPC9rZXk+DQogICAgICAgICAgICA8c3RyaW5nPnNvZnR3YXJlLXBhY2thZ2U8L3N0cmluZz4NCiAgICAgICAgICAgIDxrZXk+dXJsPC9rZXk+DQogICAgICAgICAgICA8c3RyaW5nPjwhW0NEQVRBW2h0dHA6Ly8xMjcuMC4wLjE6NDkxODYvNzY4NjE4MDc5NjM3ODQzOTY4LmlwYV1dPjwvc3RyaW5nPg0KICAgICAgICAgIDwvZGljdD4NCiAgICAgICAgICA8ZGljdD4NCiAgICAgICAgICAgIDxrZXk+a2luZDwva2V5Pg0KICAgICAgICAgICAgPHN0cmluZz5kaXNwbGF5LWltYWdlPC9zdHJpbmc+DQogICAgICAgICAgICA8a2V5PnVybDwva2V5Pg0KICAgICAgICAgICAgPHN0cmluZz48IVtDREFUQVtodHRwczovL21vc3BvYy56aGl6aGFuZ3lpLmNvbTo5MDcwL3V1c2FmZS9tb3MvZmlsZXMvc2h1YW5neXUvMjAxOC8xMS8yOS82ODg0Njk3NDIyMjIxNTk4NzIucG5nXV0+PC9zdHJpbmc+DQogICAgICAgICAgPC9kaWN0Pg0KICAgICAgICAgIDxkaWN0Pg0KICAgICAgICAgICAgPGtleT5raW5kPC9rZXk+DQogICAgICAgICAgICA8c3RyaW5nPmZ1bGwtc2l6ZS1pbWFnZTwvc3RyaW5nPg0KICAgICAgICAgICAgPGtleT51cmw8L2tleT4NCiAgICAgICAgICAgIDxzdHJpbmc+PCFbQ0RBVEFbaHR0cHM6Ly9tb3Nwb2Muemhpemhhbmd5aS5jb206OTA3MC91dXNhZmUvbW9zL2ZpbGVzL3NodWFuZ3l1LzIwMTgvMTEvMjkvNjg4NDY5NzQyMjIyMTU5ODcyLnBuZ11dPjwvc3RyaW5nPg0KICAgICAgICAgIDwvZGljdD4NCiAgICAgICAgPC9hcnJheT4NCiAgICAgICAgPGtleT5tZXRhZGF0YTwva2V5Pg0KICAgICAgICA8ZGljdD4NCiAgICAgICAgICA8a2V5PmJ1bmRsZS1pZGVudGlmaWVyPC9rZXk+DQogICAgICAgICAgPHN0cmluZz5jb20uc2t5am9zLmZpbGVleHBsb3JlcmZyZWU8L3N0cmluZz4NCiAgICAgICAgICA8a2V5PmJ1bmRsZS12ZXJzaW9uPC9rZXk+DQogICAgICAgICAgPHN0cmluZz44LjEuMjwvc3RyaW5nPg0KICAgICAgICAgIDxrZXk+a2luZDwva2V5Pg0KICAgICAgICAgIDxzdHJpbmc+c29mdHdhcmU8L3N0cmluZz4NCiAgICAgICAgICA8a2V5PnRpdGxlPC9rZXk+DQogICAgICAgICAgPHN0cmluZz5GaWxlRXhwbG9yZXI8L3N0cmluZz4NCiAgICAgICAgPC9kaWN0Pg0KICAgICAgPC9kaWN0Pg0KICAgIDwvYXJyYXk+DQogIDwvZGljdD4NCjwvcGxpc3Q+DQo=";

int main(int argC, char *arg[]) {
    int plistLen = strlen(plist_base64);
    int plistRawLen = plistLen * 0.75 + 2;
    std::vector<char> plist_raw(plistRawLen, 0);
    Base64_Decode((const unsigned char *)plist_base64, plistLen, (unsigned char *)plist_raw.begin().base(), plist_raw.size());
//    base64_decode(plist_base64, (unsigned char *)plist_raw.begin().base());
//    LOG_HEX(plist_raw.begin().base(), plist_raw.size());
    LOG_DEBUG("\n%s", plist_raw.begin().base());
    return 0;
}
