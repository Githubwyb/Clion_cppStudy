/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include <string>
#include <cstring>
#include "log.hpp"

#define PREVIEW_AWORK_SETTING "setting.json"
#define PREVIEW_AWORKNAME_PREFIX "\"tmp_app_name\":\""
#define PREVIEW_AWORKNAME_SUFIX "\""
#define TWF_LOGE LOG_ERROR
#define TWF_LOG(DebugLog, fmt, ...) LOG_DEBUG(fmt, ##__VA_ARGS__)

//从json文件内获取awork试用版名字
static int get_previewAworkName(char *const aworkName, int size)
{
    FILE *file = fopen(PREVIEW_AWORK_SETTING, "rt");

    if (nullptr == file)
    {
        TWF_LOG(DebugLog, "No file " PREVIEW_AWORK_SETTING ", maybe there is no previewAwork");
        return -1;
    }

    char line[1024];
    int state = 0;
    char *previewAworkNameStart = nullptr;
    char *previewAworkNameEnd = nullptr;
    while (nullptr != fgets(line, sizeof(line), file))
    {
        //初始状态
        if (state == 0)
        {
            //查找试用版awork起始位置
            previewAworkNameStart = strstr(line, PREVIEW_AWORKNAME_PREFIX);
            if (previewAworkNameStart != nullptr)
            {
                previewAworkNameStart += sizeof(PREVIEW_AWORKNAME_PREFIX);
                state = 1;
            }
            else
            {
                continue;
            }
        }

        //查找到起始位置
        if (state == 1)
        {
            //查找试用版awork结束位置
            previewAworkNameEnd = strstr(line, PREVIEW_AWORKNAME_SUFIX);
            if (previewAworkNameEnd != nullptr)
            {
                int cpLen = (previewAworkNameEnd - previewAworkNameStart > size) ? size : previewAworkNameEnd - previewAworkNameStart;
                memcpy(aworkName, previewAworkNameStart, cpLen);
                state = 2; //成功
            }
            else
            {
                //没有读到结束符，报错
                break;
            }
        }
    }

    //非读完状态的推出
    if (ferror(file) != 0)
    {
        TWF_LOGE("WHAT(\"File read failed\"),    \
                     REASON(\"fileName %s\", PREVIEW_AWORK_SETTING), \
                     WILL(\"Can't get previewAworkName\"),          \
                     NO_HOW_TO, NO_CAUSED_BY");
        fclose(file);
        return -1;
    }

    //state不对，读有误
    if (state != 2)
    {
        TWF_LOGE("WHAT(\"File read failed\"),    \
                     REASON(\"Can't find " PREVIEW_AWORKNAME_PREFIX "in file\"), \
                     WILL(\"Can't get previewAworkName\"),          \
                     NO_HOW_TO, NO_CAUSED_BY");
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

int main(int argC, char *arg[])
{
    char name[128] = {0};
    int ret = get_previewAworkName(name, 127);
    LOG_DEBUG("ret %d, name %s", ret, name);

    return 0;
}
