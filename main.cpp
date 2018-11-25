/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include "log.hpp"

#include <iostream>
#include <vector>
#include <memory>
#include <math.h>

#define FPGA_SYSTEM_TICK 200000000.0
#define FPGA_SYSTEM_TICK_1 202000000.0

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef enum
{
    F_FAILED = -1,
    F_SUCCESS = 0,
} FUNCTION_RETURN;

typedef enum
{
    L_FALSE = 0,
    L_TRUE = 1,
} LOGICAL_ENUM;

typedef enum
{
    FPGA_MODE_LINE = 0x00,
    FPGA_MODE_LOG = 0x01,
    FPGA_MODE_NUM
} FPGA_MODE_ENUM;

typedef struct
{
    u8 flag;
    u8 ignoreNum;
    u8 averageNum;
    u8 mode;
    u8 currentIndex;
    u8 haveMeasuredNum;
    u16 sum;
    float R;
    u32 startFre;
    u32 endFre;
} SETTING;

typedef struct
{
    u8 isScram;     //急停键
    u8 isStart;
    u8 recievedFlag;
    u8 isFirst;
    u8 isIgnore;
    u8 getIndex;
    u8 fpgaFreChoose;
    double curFre;
    u32 curRealFre;
    u32 divFre;
    u32 lastDiv;
} DATA;

static DATA data;
static SETTING setting;

int get_fre(u8 mode);
int get_freLine(void);
int get_divLine(double fre);
int get_freLog(void);
int get_divLog(double fre);

DATA *data_getData() {
    return &data;
}

SETTING *data_getSetting() {
    return &setting;
}

using namespace std;
int main() {
    LOG_INFO("Hello, main");

    DATA *pData = data_getData();
    SETTING *pSetting = data_getSetting();
    float error = 0;

    pSetting->sum = 501;
    pSetting->mode = FPGA_MODE_LINE;
    pSetting->startFre = 10;
    pSetting->endFre = 1000000;

    pData->curFre = pSetting->startFre;
    pData->divFre = 1;
    pData->isStart = L_TRUE;
    pData->isFirst = L_TRUE;
    pData->fpgaFreChoose = 0;

    int i = 1;
    while (true) {
        if(0 != get_fre(0))
        {
            pData->isStart = L_FALSE;
            break;
        }

        if (pData->fpgaFreChoose == 1) {
            error = FPGA_SYSTEM_TICK / 32 / pData->divFre / pData->curRealFre * 10 - 1;
        } else {
            error = FPGA_SYSTEM_TICK_1 / 32 / pData->divFre / pData->curRealFre * 10 - 1;
        }
        LOG_DEBUG("index %d current frequency %.1f, divFre %d, error %.4f%%", i++, pData->curRealFre / 10.0, pData->divFre, error * 100);
    }
    return 0;
}

int get_div(double fre)
{
    DATA *pData = NULL;

    pData = data_getData();

    if (fre < 1000)
    {
        pData->curRealFre = fre * 10;
        pData->divFre = FPGA_SYSTEM_TICK / 32 / pData->curRealFre * 10;
    }
    else if (fre < 1000000 * 1.06)
    {
        pData->divFre = FPGA_SYSTEM_TICK / 32 / fre;
        pData->curRealFre = FPGA_SYSTEM_TICK / 32 / pData->divFre * 10;
    }
    else
    {
        LOG_ERROR("frequency max is 1000000");
        return F_FAILED;
    }

    return F_SUCCESS;
}

int get_fre(u8 mode)
{
    DATA *pData = NULL;
    SETTING *pSetting = NULL;

    pData = data_getData();
    pSetting = data_getSetting();

    if (mode == 1)
    {
        return get_div(pData->curFre);
    }
    else
    {
        switch (pSetting->mode)
        {
            case FPGA_MODE_LINE:
                return get_freLine();

            case FPGA_MODE_LOG:
                return get_freLog();

            default:
                return F_FAILED;
        }
    }

    //    return F_FAILED;
}

int get_divLine(double fre)
{
    DATA *pData = NULL;

    pData = data_getData();

    if (fre < 1000)
    {
        pData->curRealFre = fre * 10;
        pData->divFre = FPGA_SYSTEM_TICK / 32 / pData->curRealFre * 10;
    }
    else if (pData->curFre > 10000)
    {
        if (pData->fpgaFreChoose == 1) {
            pData->curRealFre = FPGA_SYSTEM_TICK / 32 / pData->divFre * 10;
        } else {
            pData->curRealFre = FPGA_SYSTEM_TICK_1 / 32 / pData->divFre * 10;
        }
    }
    else
    {
        pData->divFre = FPGA_SYSTEM_TICK / 32 / fre;
        pData->curRealFre = FPGA_SYSTEM_TICK / 32 / pData->divFre * 10;
    }

    return F_SUCCESS;
}

int get_freLine(void)
{
    static float add = 0;
    static int threshold1 = 0;
    static int threshold2 = 0;
    static int n1 = 0;
    static int n2 = 0;
    static DATA *pData = data_getData();
    static SETTING *pSetting = data_getSetting();
    int temp = 0;
    int temp1 = 0;
    int temp2 = 0;
    if (L_TRUE == pData->isFirst)
    {
        pSetting = data_getSetting();

        pData->lastDiv = 0xffffffff;

        temp = pSetting->sum / 5;
        add = 90.0 / temp;

        temp1 = (int)(FPGA_SYSTEM_TICK / 32 / 10000) - (int)(FPGA_SYSTEM_TICK / 32 / 100000) - 1;
        n1 = temp1 / (temp - 1);
        temp2 = temp1 % (temp - 1);
        threshold1 = (int)(FPGA_SYSTEM_TICK / 32 / 10000) - (n1 + 1) * temp2 + 1;

        temp1 = (int)(FPGA_SYSTEM_TICK / 32 / 100000) - (int)(FPGA_SYSTEM_TICK / 32 / 1000000);
        // LOG_DEBUG("temp1: %d", temp1);
        if (temp1 < temp)
        {
            n2 = 0;
            threshold2 = 0;
        }
        else
        {
            n2 = temp1 / (temp - 1);
            temp2 = temp1 % (temp - 1);
            threshold2 = (int)(FPGA_SYSTEM_TICK / 32 / 100000) - (n2 + 1) * temp2 + 1;
        }

        LOG_DEBUG("n1: %d threshold1: %d n2: %d threshold2: %d", n1, threshold1, n2, threshold2);
    }

    if (pData->curFre < pSetting->startFre)
    {
        LOG_DEBUG("%f %d", pData->curFre, pSetting->startFre);
        pData->isFirst = L_TRUE;
        return F_FAILED;
    }

    if (pData->curFre - pSetting->startFre < 0.1 && L_TRUE == pData->isFirst)
    {
        pData->isFirst = L_FALSE;
    }
    else if (pData->curFre < 50)
    {
        pData->curFre += add;
        if (50 - pData->curFre < 0.2)
        {
            pData->curFre = 50;
        }
    }
    else if (pData->curFre < 100)
    {
        pData->curFre += add;
        if (100 - pData->curFre < 0.2)
        {
            pData->curFre = 100;
        }
    }
    else if (pData->curFre < 1000)
    {
        pData->curFre += add * 10;
        if (1000 - pData->curFre < 2)
        {
            pData->curFre = 1000;
        }
    }
    else if (pData->curFre < 10000)
    {
        pData->curFre += add * 100;
        if (10000 - pData->curFre < 20)
        {
            pData->curFre = 10000;
            pData->divFre = FPGA_SYSTEM_TICK / 32 / pData->curFre;
        }
    }
    else if (pData->curFre < 100000 && pData->divFre > threshold1)
    {
        pData->divFre -= n1 + 1;
        pData->curFre = FPGA_SYSTEM_TICK / 32 / pData->divFre;
    }
    else if (pData->curFre < 100000 && pData->divFre <= threshold1)
    {
        pData->divFre -= n1;
        pData->curFre = FPGA_SYSTEM_TICK / 32 / pData->divFre;
        if (pData->curFre > 100000)
        {
            pData->curFre = 100000;
            pData->divFre = FPGA_SYSTEM_TICK / 32 / pData->curFre;
        }
    }
    else {
        switch (pSetting->sum) {
            case 251:
                if (pData->divFre > threshold2) {
                    pData->divFre -= n2 + 1;
                    pData->curFre = FPGA_SYSTEM_TICK / 32 / pData->divFre;
                }
                else if (pData->divFre < threshold2)
                {
                    pData->divFre -= n2;
                    pData->curFre = FPGA_SYSTEM_TICK / 32 / pData->divFre;
                }
                break;

            case 501: {
                if (pData->fpgaFreChoose == 0) {
                    if (pData->divFre > 53) {
                        pData->divFre -= 2;
                        pData->curFre = FPGA_SYSTEM_TICK / 32 / pData->divFre;
                    }
                    else
                    {
                        pData->fpgaFreChoose = 1;
                        pData->divFre -= 1;
                        pData->curFre = FPGA_SYSTEM_TICK / 32 / pData->divFre;
                    }
                } else {
                    pData->fpgaFreChoose = 0;
                    pData->curFre = FPGA_SYSTEM_TICK_1 / 32 / pData->divFre;
                }
            }
                break;

            case 1001: {
                if (pData->fpgaFreChoose == 0) {
                    pData->fpgaFreChoose = 1;
                    pData->divFre -= 1;
                    pData->curFre = FPGA_SYSTEM_TICK / 32 / pData->divFre;
                } else {
                    pData->fpgaFreChoose = 0;
                    pData->curFre = FPGA_SYSTEM_TICK_1 / 32 / pData->divFre;
                }
            }
                break;

            default:
                break;
        }
    }

    get_divLine(pData->curFre);

    if (pData->curRealFre > pSetting->endFre * 10.6)
    {
        pData->isFirst = L_TRUE;
        return F_FAILED;
    }

    return F_SUCCESS;
}

int get_divLog(double fre)
{
    DATA *pData = data_getData();
    SETTING *pSetting = data_getSetting();

    if (fre < 1000)
    {
        pData->curRealFre = fre * 10;
        pData->divFre = FPGA_SYSTEM_TICK / 32 / pData->curRealFre * 10;
    }
    else
    {
        pData->divFre = FPGA_SYSTEM_TICK / 32 / fre;
        pData->curRealFre = FPGA_SYSTEM_TICK / 32 / pData->divFre * 10;
    }

    switch (pSetting->sum) {
        case 251:
            if (pData->divFre >= pData->lastDiv)
            {
                pData->divFre = pData->lastDiv - 1;
                pData->curRealFre = FPGA_SYSTEM_TICK / 32 / pData->divFre * 10;
            }
            break;

        case 501: {
            if (pData->fpgaFreChoose == 0 && pData->divFre >= pData->lastDiv) {
                pData->fpgaFreChoose = 1;
                //200
                pData->divFre = pData->lastDiv - 1;
                pData->curRealFre = FPGA_SYSTEM_TICK / 32 / pData->divFre * 10;
            } else if (pData->fpgaFreChoose == 1) {
                pData->fpgaFreChoose = 0;
                pData->divFre = pData->lastDiv;
                //202
                pData->curRealFre = FPGA_SYSTEM_TICK_1 / 32 / pData->divFre * 10;
            }
        }
            break;

        case 1001: {
            if (pData->fpgaFreChoose == 0 && pData->divFre >= pData->lastDiv) {
                pData->fpgaFreChoose = 1;
                //200
                pData->divFre = pData->lastDiv - 1;
                pData->curRealFre = FPGA_SYSTEM_TICK / 32 / pData->divFre * 10;
            } else if (pData->fpgaFreChoose == 1) {
                pData->fpgaFreChoose = 0;
                pData->divFre = pData->lastDiv;
                //202
                pData->curRealFre = FPGA_SYSTEM_TICK_1 / 32 / pData->divFre * 10;
            }
        }
            break;

        default:
            break;
    }

    pData->lastDiv = pData->divFre;
    return F_SUCCESS;
}

int get_freLog(void)
{
    static double times = 0;
    static DATA *pData = data_getData();
    static SETTING *pSetting = data_getSetting();
    double temp = 0;

    if (L_TRUE == pData->isFirst)
    {
        pData->lastDiv = 0xffffffff;

        temp = log10(pSetting->endFre) - log10(pSetting->startFre);
        temp /= pSetting->sum;
        times = pow(10, temp);

        LOG_DEBUG("times: %.4f", times);
        // return F_FAILED;
    }

    if (pData->curFre < pSetting->startFre)
    {
        pData->isFirst = L_TRUE;
        return F_FAILED;
    }

    if (pData->curFre - pSetting->startFre < 0.01 && L_TRUE == pData->isFirst)
    {
        pData->isFirst = L_FALSE;
    }
    else if (pData->curFre < 50)
    {
        pData->curFre = pData->curFre * times;
        if (50 - pData->curFre < 0.2)
        {
            pData->curFre = 50;
        }
    }
    else
    {
        pData->curFre = pData->curFre * times;
    }

    get_divLog(pData->curFre);

    if (pData->curRealFre > pSetting->endFre * 10.6)
    {
        pData->isFirst = L_TRUE;
        return F_FAILED;
    }

    return F_SUCCESS;
}
