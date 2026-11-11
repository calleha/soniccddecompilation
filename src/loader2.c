#include "equ.h"
#include "loader2.h"
#include "impfuncs.h"

static Uint32 SE_DashReq;

void GetRoundStr(Uint16 StageNo, Uint8 Time_Flag, char *buf) {
    Uint8 stage, round;

    round = (Uint8)(StageNo >> 8) + 1;
    if (round > 1)
        ++round;
    stage = (Uint8)StageNo + 1;

    *buf++ = (Uint8)(round + 48);
    *buf++ = (Uint8)(stage + 48);
    switch (Time_Flag & 127) {
    case 0:
        *buf++ = 66;
        break;
    case 1:
        *buf++ = 65;
        break;
    case 2:
        if (generate_flag)
            *buf++ = 67;
        else
            *buf++ = 68;
    }

    *buf++ = 0;
}

void ReadBlockMap(void) {
    Sint32 hf;
    char buf[80];
    char fn[80];

    GetRoundStr(stageno.w, time_flag, buf);

    sPrintf(fn, "../R%c/%3s/T%3s.MAP", buf[0], buf, buf);

    sMemSet(blockwk, 0, sizeof(blockwk));
    hf = sOpenFile(fn);
    if (hf == -1) {

        sPrintf(buf, "Block Map Read Error:%s\n", fn);
        sOutputDebugString(buf);

    } else {
        sReadFile(hf, blockwk, sizeof(blockwk));
        sCloseFile(hf);

        sPrintf(buf, "Block Map Read:%s\n", fn);
        sOutputDebugString(buf);
    }
}

void ReadScrolMap(void) {
    Sint32 hf;
    Uint16 *p;
    Sint32 i;
    char buf[80];
    char fn[80];

    GetRoundStr(stageno.w, time_flag, buf);

    sPrintf(fn, "../R%c/%3s/MAP%3s.BIN", buf[0], buf, buf);

    p = pmapwk;
    for (i = 0; i < 128; ++i) {
        sMemSet(pmapwk, 0, 512);
        p += 256;
    }

    hf = sOpenFile(fn);
    if (hf == -1) {

        sPrintf(buf, "Scroll Map Read Error:%s\n", fn);
        sOutputDebugString(buf);

    } else {
        p = pmapwk;
        for (i = 0; i < 128; ++i) {

            if (sReadFile(hf, p, 512) == -1)
                break;

            p += 256;
        }
        sCloseFile(hf);

        sPrintf(buf, "Scroll Map Read:%s\n", fn);
        sOutputDebugString(buf);
    }
}

static Uint8 SeToWavTbl[80] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 20, 21, 22, 22, 23, 23, 24, 25, 26, 27,
    28, 29, 30, 31, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54,
    55, 56, 57, 58, 58, 59, 60, 61, 62, 63, 64, 65, 67, 68, 69, 70,
    71, 71, 72, 73, 74, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42};

void soundset(Sint16 ReqNo) {
    if (ReqNo == 171)
        return;

    ReqNo -= 144;
    ReqNo = (Uint16)SeToWavTbl[ReqNo];

    WaveRequest(ReqNo);
}

void sub_sync(Sint16 ReqNo) {
    if (ReqNo == 103) {
        boss_sound = 1;
    }

    if (ReqNo >= 118 && ReqNo < 129) {
        WaveRequest(ReqNo - 43);
        return;
    }
    if ((ReqNo >= 15 && ReqNo <= 34) || (ReqNo >= 48 && ReqNo < 55)) {
        CDPlay(ReqNo - 12);
    } else if (ReqNo >= 102 && ReqNo < 113) {
        CDPlay(ReqNo - 79);
    } else if (ReqNo == 113) {
        CDPlay(2);
    } else if (ReqNo >= 114 && ReqNo < 116) {
        CDPlay(ReqNo - 80);
    } else if (ReqNo == 146) {
        WaveRequest(85);
    }
}
