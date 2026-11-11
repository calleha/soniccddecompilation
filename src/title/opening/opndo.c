#include "../../types.h"
#include "../common/score_data_types.h"
#include "../common/hmx_types.h"
#include "opndo.h"
#include "../common/grid.h"
#include "../common/palt.h"
#include "../common/hmx_oeeactl.h"
#include "../../impfuncs.h"
#include "opnentry.h"

static void OEUpdateHand(void);
static void OEUpdateMayu(void);
static void OEUpdatePlanet(void);
static void OEUpdateMizu(void);
static void OEUpdateKumo(void);
static void GetEnabeMenu(Uint32 *menuTable);
static Sint32 GetNextMenu(Uint32 *menuTable, Sint32 kind, Uint32 bNext);
static void OEUpdateMsg(void);

typedef struct {
    Uint16 wrap;
    void *pSmorphArray;
} smorph;

typedef struct {
    Sint32 StartX;
    Sint32 StartY;
    Sint32 DeltaX;
    Sint32 DeltaY;
} smorph_data;

typedef struct {
    Sint32 time;
    POINT point;
} time_point;

typedef struct {
    Sint32 time;
    Uint16 timingPatNo;
} time_pat;

static const time_point ptPlnt[8] = {
    {16, {240, -33}},  {32, {240, -34}},  {48, {240, -35}},  {84, {240, -34}},
    {100, {240, -33}}, {116, {240, -32}}, {128, {240, -32}}, {-1, {0, 0}}};
static time_pat timeHand[16] = {
    {0, 11},  {18, 10}, {19, 9}, {20, 8}, {21, 8}, {22, 9}, {23, 10}, {24, 11},
    {25, 11}, {26, 10}, {27, 9}, {28, 8}, {29, 8}, {30, 9}, {31, 10}, {32, 11}};
static Sint32 timeMayu[2] = {18, 20};
static Sint32 posiLeftArrow[3] = {90, 88, 86};
static Sint32 posiRightArrow[3] = {210, 212, 214};
extern Uint16 nSequenceNum;
extern Sint32 nTimerCunt;
extern void (*hmx_sprite_set_position_module)(hmx_sprite *, Sint32, Sint32);
extern draw_context *s_ctx;
extern sprite_bmp infoSprtBmp[];
extern void (*hmx_grid_set_horz_offsets_module)(hmx_grid *, Sint32 *);
extern grid_bmp infoGridBmp[];
extern void (*hmx_ddagrid_set_scan_module)(hmx_ddagrid *, Sint32, Sint32,
                                           Sint32, Sint32, Sint32);
extern score_data crntScorData;
extern Uint32 selectIndx;
extern Uint16 swData1;
extern PALETTEENTRY tblPal2[];
extern PALETTEENTRY tblPal2a[];
extern PALETTEENTRY tblPal4[];
extern PALETTEENTRY tblPal3[];
extern PALETTEENTRY tblPal1[];
extern Uint32 bFirstTitle;
extern PALETTEENTRY tblPal1a[];
static char KeyState[256];
Uint32 hFx0;
Uint32 hFx1;
static Sint32 nFadoTime;
static Sint32 nFadoValu;
static Uint32 bFadePaletDisable[4];
static Sint32 nFadoSeqNum;
static Uint16 nMenuKind;
static PALETTEENTRY workPalet[4][16];
static Uint32 bDrawDisable;
static Uint32 *testBmp;
static Uint32 testSpr;

void OEDraw(void) {
    if (nSequenceNum > 1 && bDrawDisable == 0) {

        srfDraw();
    }
}

static void OEUpdateHand(void) {
    static Sint32 num;

    if (nTimerCunt == 0) {

        num = 0;
    }
    if ((Uint32)num < 16 && nTimerCunt == timeHand[num].time) {

        if (num != 0)
            OESprCreate(timeHand[num].timingPatNo);
        ++num;
    } else if (num != 0)
        OESprCreate(timeHand[num - 1].timingPatNo);
    else {
        OESprCreate(timeHand[num].timingPatNo);
    }
}

static void OEUpdateMayu(void) {
    static Sint32 num;

    if (nTimerCunt == 0) {

        num = 0;
    }
    if ((Uint32)num < 2 && nTimerCunt == timeMayu[num]) {

        if (num % 2 == 0)
            OESprCreate(12);
        ++num;
    } else if (num % 2 == 0 && (Uint32)num < 2) {
        OESprCreate(12);
    }
}

static void OEUpdatePlanet(void) {
    static Sint32 num;
    static Sint32 staTime;

    if (*infoSprtBmp[14].lphSpr != 0) {

        if (nTimerCunt == 0) {

            num = 0;
            staTime = 0;
        }

        hmx_sprite_set_position_module(
            s_ctx->sprites[*infoSprtBmp[14].lphSpr], ptPlnt[num].point.x,
            ptPlnt[num].point.y);
        if (ptPlnt[num].time == nTimerCunt - staTime) {

            if ((Uint32)++num > 7) {

                num = 0;
            }
        } else if (ptPlnt[num].time == -1) {

            num = 0;
            staTime = nTimerCunt;

            hmx_sprite_set_position_module(
                s_ctx->sprites[*infoSprtBmp[14].lphSpr], ptPlnt[num].point.x,
                ptPlnt[num].point.y);
        }
    }
}

static void OEUpdateMizu(void) {
    Sint32 inc;
    Sint32 i;
    static Sint32 nHorizShearArray[64], fHorizShearArray[64];

    if (nTimerCunt == 0) {

        for (i = 0; i < 64; ++i) {

            fHorizShearArray[i] = 0;
            nHorizShearArray[i] = 0;
        }

        hmx_grid_set_horz_offsets_module(s_ctx->grids[1], nHorizShearArray);
    }

    if (*infoGridBmp[1].lphGrid != 0) {

        if (nTimerCunt % 2 == 0) {

            for (i = 0; i < 64; ++i) {

                inc = i * 10 + 40;
                fHorizShearArray[i] += inc;
                if (fHorizShearArray[i] > 32000)
                    fHorizShearArray[i] -= 32000;
                nHorizShearArray[i] = -(fHorizShearArray[i] / 100);
            }
            hmx_grid_set_horz_offsets_module(s_ctx->grids[1], nHorizShearArray);
        }
    }
}

static void OEUpdateKumo(void) {
    Sint32 i;
    static smorph_data RotateMorphValues[128];
    static Uint8 offsetY[92] = {
        0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,
        14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,
        28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,
        42,  43,  44,  45,  46,  47,  48,  49,  50,  52,  54,  56,  58,  60,
        62,  64,  66,  68,  70,  72,  74,  76,  78,  80,  84,  88,  92,  96,
        100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 142, 148, 154, 160,
        170, 180, 190, 200, 210, 220, 236, 255};
    static smorph RotateMorph;
    static Uint16 incY;

    if (nTimerCunt == 0) {

        for (i = 0; i < 92; ++i) {

            RotateMorphValues[i].StartX =
                (i * -64000 / 92 + 32767 + 31233) * 65;
            RotateMorphValues[i].StartY = i * 65536;
            RotateMorphValues[i].DeltaX = (i * 400 / 92 + 400) * 65;
            RotateMorphValues[i].DeltaY = 0;
        }
        for (i = 92; i < 128; ++i) {

            RotateMorphValues[i].StartX = 65536;
            RotateMorphValues[i].StartY = 65536;
            RotateMorphValues[i].DeltaX = 0;
            RotateMorphValues[i].DeltaY = 0;
        }
        RotateMorph.wrap = 0;
        RotateMorph.pSmorphArray = RotateMorphValues;

        incY = 0;
    }
    for (i = 0; i < 92; ++i) {

        RotateMorphValues[i].StartY = (offsetY[i] + incY) % 128 * 65536;
        hmx_ddagrid_set_scan_module(
            (hmx_ddagrid *)s_ctx->grids[2], i, RotateMorphValues[i].StartX,
            RotateMorphValues[i].DeltaX, RotateMorphValues[i].StartY,
            RotateMorphValues[i].DeltaY);
    }
    if (++incY >= 128)
        incY = 0;
}

static void GetEnabeMenu(Uint32 *menuTable) {
    score_data dummyData;
    Uint32 bDataExist;

    if (ReadScore(selectIndx, (char *)&dummyData, 0) == 0) {

        bDataExist = 0;
    } else {
        bDataExist = 1;
        selectIndx = ReadIndx(0);
        ReadScore(selectIndx, (char *)&crntScorData, 0);
    }

    menuTable[0] = 1;

    menuTable[6] = 1;

    if (bDataExist != 0)
        if (ChkNewSlot() == 0)
            menuTable[0] = 0;

    if (crntScorData.saved == 1)
        menuTable[1] = 1;
    else
        menuTable[1] = 0;

    if (bDataExist != 0)
        menuTable[3] = 1;
    else
        menuTable[3] = 0;

    if (crntScorData.roundNo != 0)
        menuTable[2] = 1;
    else
        menuTable[2] = 0;

    if (crntScorData.roundNo >= 7 && crntScorData.total < 134734)
        menuTable[4] = 1;
    else
        menuTable[4] = 0;

    if (crntScorData.roundNo >= 7 && crntScorData.total < 92767)
        menuTable[5] = 1;
    else
        menuTable[5] = 0;
}

static Sint32 GetNextMenu(Uint32 *menuTable, Sint32 kind, Uint32 bNext) {
    Sint32 indx;
    Sint32 n;

    if (kind == -1) {

        if (menuTable[1] != 0)
            return 1;
        else
            return 0;
    }

    indx = kind;

    if (bNext != 0) {

        for (n = 0; n < 7; ++n) {

            ++indx;
            if (indx > 6)
                indx = 0;
            if (menuTable[indx] != 0)
                return indx;
        }
    } else {

        for (n = 0; n < 7; ++n) {

            --indx;
            if (indx < 0)
                indx = 6;
            if (menuTable[indx] != 0)
                return indx;
        }
    }
}

Uint32 ChkNewSlot(void) {
    Sint32 i;
    score_data dummyData;

    for (i = 0; i < 6; ++i) {
        if (ReadScore(i, (char *)&dummyData, 0) == 0)
            break;

        if (dummyData.saved == 0)
            break;
    }
    if (i == 6) {

        return 0;
    }
    return 1;
}

static void OEUpdateMsg(void) {
    POINT arrowPoint;
    Uint32 kPosi;
    Uint32 lPosi;
    Uint32 rPosi;
    static Uint32 bDelete;
    static Sint32 kind;
    static Sint32 nSeqNo;
    static Sint32 nSeqNoNext;
    static POINT point;
    static Sint32 timeOld;
    static Sint32 arrowKind;
    static Uint32 menuEnable[7];
    static Sint32 testCheckMode;
    static Uint32 LArrow;
    static Uint32 RArrow;

    rPosi = lPosi = kPosi = 0;

    if (nTimerCunt == 0) {

        kind = 24;
        bDelete = 0;
        timeOld = 0;
        arrowKind = 0;
        testCheckMode = 1;
        GetEnabeMenu(menuEnable);
        nSeqNo = 1;
    }
    if (nTimerCunt - timeOld == 900) {

        nMenuKind = 7;
        nFadoTime = 21;
        nFadoValu = 32;
        nFadoSeqNum = 999;
        OESetSequenceNum(8);
    }

    if (nSeqNo == 1) {

        if (testCheckMode != 0) {

            if (swData1 & 24320) {

                switch (testCheckMode) {

                case 1:
                    if (swData1 & 512)
                        ++testCheckMode;
                    else if (swData1 & 256)
                        testCheckMode = 12;
                    else if (swData1 & 2048)
                        testCheckMode = 20;
                    else
                        testCheckMode = 0;
                    break;
                case 2:
                    if (swData1 & 512)
                        ++testCheckMode;
                    else
                        testCheckMode = 0;
                    break;
                case 3:
                    if (swData1 & 512)
                        ++testCheckMode;
                    else
                        testCheckMode = 0;
                    break;
                case 4:
                    if (swData1 & 1024)
                        ++testCheckMode;
                    else
                        testCheckMode = 0;
                    break;
                case 5:
                    if (swData1 & 2048)
                        ++testCheckMode;
                    else
                        testCheckMode = 0;
                    break;
                case 6:
                    if (swData1 & 16384) {

                        nMenuKind = 8;
                        nFadoTime = 21;
                        nFadoValu = 32;
                        nFadoSeqNum = 999;
                        OESetSequenceNum(8);
                    } else
                        testCheckMode = 0;
                    break;
                case 12:
                    if (swData1 & 512)
                        ++testCheckMode;
                    else
                        testCheckMode = 0;
                    break;
                case 13:
                    if (swData1 & 512)
                        ++testCheckMode;
                    else
                        testCheckMode = 0;
                    break;
                case 14:
                    if (swData1 & 1024)
                        ++testCheckMode;
                    else
                        testCheckMode = 0;
                    break;
                case 15:
                    if (swData1 & 2048)
                        ++testCheckMode;
                    else
                        testCheckMode = 0;
                    break;
                case 16:
                    if (swData1 & 16384) {

                        nMenuKind = 9;
                        nFadoTime = 21;
                        nFadoValu = 32;
                        nFadoSeqNum = 999;
                        OESetSequenceNum(8);
                    } else
                        testCheckMode = 0;
                    break;

                case 20:
                    if (swData1 & 2048)
                        ++testCheckMode;
                    else
                        testCheckMode = 0;
                    break;
                case 21:
                    if (swData1 & 256)
                        ++testCheckMode;
                    else
                        testCheckMode = 0;
                    break;
                case 22:
                    if (swData1 & 256)
                        ++testCheckMode;
                    else
                        testCheckMode = 0;
                    break;
                case 23:
                    if (swData1 & 512)
                        ++testCheckMode;
                    else
                        testCheckMode = 0;
                    break;
                case 24:
                    if (swData1 & 16384) {

                        nMenuKind = 10;
                        nFadoTime = 21;
                        nFadoValu = 32;
                        nFadoSeqNum = 999;
                        OESetSequenceNum(8);
                    } else
                        testCheckMode = 0;
                    break;
                }

                timeOld = nTimerCunt;
                nSeqNoNext = 1;
                nSeqNo = 7;
            }
        }
        if (nTimerCunt % 20 == 0) {

            bDelete = bDelete ? 0 : 1;
        }

        if (bDelete == 0)
            OESprCreateIndx(kind);

        if (swData1 & 53248) {

            point = infoSprtBmp[kind].point;
            if (bDelete == 0)
                OESprCreateIndx(kind);
            OEClrset(tblPal2a, 1);
            nSeqNo = 2;
        }
    } else if (nSeqNo == 2) {

        RArrow = LArrow = 0;
        OEClrset(tblPal2a, 1);
        point.x += 32;
        if (point.x >= 320) {

            if (kind == 24) {

                kind = GetNextMenu(menuEnable, -1, 0) + 25;
            } else
                kind = GetNextMenu(menuEnable, kind - 25, 0) + 25;
            point = infoSprtBmp[kind].point;
            point.x = -infoSprtBmp[kind].w;
            OEClrset(tblPal2a, 1);
            nSeqNo = 3;
        }
        kPosi = 1;
    } else if (nSeqNo == 3) {

        OEClrset(tblPal2a, 1);
        point.x += 32;
        if (point.x >= infoSprtBmp[kind].point.x) {

            if (GetNextMenu(menuEnable, kind - 25, 0) + 25 < kind)
                LArrow = 1;
            if (GetNextMenu(menuEnable, kind - 25, 1) + 25 > kind)
                RArrow = 1;
            point = infoSprtBmp[kind].point;
            OEClrset(tblPal2, 1);
            nSeqNo = 6;
            timeOld = nTimerCunt;
        }
        kPosi = 1;
    } else if (nSeqNo == 4) {

        RArrow = LArrow = 0;
        OEClrset(tblPal2a, 1);
        point.x -= 32;
        if (point.x < -infoSprtBmp[kind].w) {

            kind = GetNextMenu(menuEnable, kind - 25, 1) + 25;
            point = infoSprtBmp[kind].point;
            point.x = 320;
            OEClrset(tblPal2a, 1);
            nSeqNo = 5;
        }
        kPosi = 1;
    } else if (nSeqNo == 5) {

        OEClrset(tblPal2a, 1);
        point.x -= 32;
        if (point.x < infoSprtBmp[kind].point.x) {

            if (GetNextMenu(menuEnable, kind - 25, 0) + 25 < kind)
                LArrow = 1;
            if (GetNextMenu(menuEnable, kind - 25, 1) + 25 > kind)
                RArrow = 1;
            point = infoSprtBmp[kind].point;
            OEClrset(tblPal2, 1);
            nSeqNo = 6;
            timeOld = nTimerCunt;
        }
        kPosi = 1;
    } else if (nSeqNo == 6) {

        if ((nTimerCunt - timeOld) % 16 == 0) {

            if (*infoSprtBmp[32].lphSpr != 0) {

                arrowPoint.x = posiLeftArrow[arrowKind];
                arrowPoint.y = infoSprtBmp[32].point.y;
                lPosi = 1;
            }
            if (*infoSprtBmp[33].lphSpr != 0) {

                arrowPoint.x = posiRightArrow[arrowKind];
                arrowPoint.y = infoSprtBmp[33].point.y;
                rPosi = 1;
            }
            if (++arrowKind > 2)
                arrowKind = 0;
        }

        if (swData1 & 20480) {

            if (kind - 25 == 0) {
                if (ChkNewSlot() == 0) {
                    WaveRequest(28);
                    return;
                }
            }

            nFadoTime = 21;
            nFadoValu = 32;
            nFadoSeqNum = 999;
            OESetSequenceNum(8);
            nMenuKind = kind - 25;
        } else if (swData1 & 1024 &&
                   GetNextMenu(menuEnable, kind - 25, 0) + 25 < kind) {

            timeOld = nTimerCunt;
            nSeqNo = 7;
            nSeqNoNext = 2;
        } else if (swData1 & 2048 &&
                   GetNextMenu(menuEnable, kind - 25, 1) + 25 > kind) {

            timeOld = nTimerCunt;
            nSeqNo = 7;
            nSeqNoNext = 4;
        }
    } else if (nSeqNo == 7) {

        if (nTimerCunt - timeOld > 6) {

            nSeqNo = nSeqNoNext;
        }
    }
    if (LArrow != 0)
        OESprCreateIndx(32);
    if (RArrow != 0)
        OESprCreateIndx(33);
    if (nSeqNo != 1)
        OESprCreateIndx(kind);

    if (kPosi != 0)
        hmx_sprite_set_position_module(
            s_ctx->sprites[*infoSprtBmp[kind].lphSpr], point.x, point.y);
    if (lPosi != 0)
        hmx_sprite_set_position_module(s_ctx->sprites[*infoSprtBmp[32].lphSpr],
                                       arrowPoint.x, arrowPoint.y);
    if (rPosi != 0)
        hmx_sprite_set_position_module(s_ctx->sprites[*infoSprtBmp[33].lphSpr],
                                       arrowPoint.x, arrowPoint.y);
}

void OETitleSta(void) {
    OEAllSprDelete();
    if (nTimerCunt == 0) {

        OEGridCreate(1);
        bDrawDisable = 1;
        nFadoTime = 21;
        nFadoValu = 32;
        nFadoSeqNum = 7;
        bFadePaletDisable[3] = 1;
        OESetSequenceNum(9);
    }
    OESprCreate(7);
    OESprCreate(11);
}

void OETitle(void) {
    OEAllSprDelete();
    OESprCreate(7);
    if (nTimerCunt == 0) {

        OEGridCreate(2);
    }
    OEUpdateHand();
    OEUpdateMayu();
    OEUpdatePlanet();
    OEUpdateMizu();
    OEUpdateKumo();
    OEUpdateMsg();
}

void OEFadeOut(void) {
    Sint32 i, j;
    static Sint32 colorNum;

    if (nTimerCunt == 0) {

        sMemCpy(workPalet[0], tblPal1, sizeof(workPalet[0]));
        sMemCpy(workPalet[1], tblPal2, sizeof(workPalet[1]));
        sMemCpy(workPalet[2], tblPal3, sizeof(workPalet[2]));
        sMemCpy(workPalet[3], tblPal4, sizeof(workPalet[3]));
        colorNum = 0;
    } else if (nTimerCunt == nFadoTime) {

        bFadePaletDisable[0] = bFadePaletDisable[1] = bFadePaletDisable[2] =
            bFadePaletDisable[3] = 0;
        OESetSequenceNum(nFadoSeqNum);
    } else {

        for (i = 0; i < 4; ++i) {

            for (j = 0; j < 16; ++j) {

                if (workPalet[i][j].peRed > nFadoValu)
                    workPalet[i][j].peRed -= nFadoValu;
                else
                    workPalet[i][j].peRed = 0;
                if (workPalet[i][j].peRed == 0) {

                    if (workPalet[i][j].peGreen > nFadoValu)
                        workPalet[i][j].peGreen -= nFadoValu;
                    else
                        workPalet[i][j].peGreen = 0;
                    if (workPalet[i][j].peGreen == 0) {

                        if (workPalet[i][j].peBlue > nFadoValu)
                            workPalet[i][j].peBlue -= nFadoValu;
                        else
                            workPalet[i][j].peBlue = 0;
                    }
                }
            }
            if (bFadePaletDisable[i] == 0)
                OEClrset(workPalet[i], i);
        }
    }
}

void OEFadeIn(void) {
    Sint32 n, j;
    PALETTEENTRY *lpPal;

    OEAllSprDelete();
    if (nFadoSeqNum == 7) {
        OESprCreate(7);
        OESprCreate(11);
    }

    if (nTimerCunt == nFadoTime) {

        OEClrset(tblPal1, 0);
        OEClrset(tblPal2, 1);
        OEClrset(tblPal3, 2);
        OEClrset(tblPal4, 3);
        bFadePaletDisable[0] = bFadePaletDisable[1] = bFadePaletDisable[2] =
            bFadePaletDisable[3] = 0;
        OESetSequenceNum(nFadoSeqNum);
    } else {

        bDrawDisable = 0;
        if (nFadoValu * nTimerCunt < 256) {

            for (n = 0; n < 4; ++n) {

                switch (n) {

                case 0:
                    lpPal = tblPal1;
                    break;
                case 1:
                    lpPal = tblPal2;
                    break;
                case 2:
                    lpPal = tblPal3;
                    break;
                case 3:
                    lpPal = tblPal4;
                    break;
                }
                for (j = 0; j < 16; ++j) {

                    workPalet[n][j].peRed =
                        lpPal[j].peRed > 224 - nFadoValu * nTimerCunt
                            ? lpPal[j].peRed
                            : 224 - nFadoValu * nTimerCunt;
                    workPalet[n][j].peGreen =
                        lpPal[j].peGreen > 224 - nFadoValu * nTimerCunt
                            ? lpPal[j].peGreen
                            : 224 - nFadoValu * nTimerCunt;
                    workPalet[n][j].peBlue =
                        lpPal[j].peBlue > 224 - nFadoValu * nTimerCunt
                            ? lpPal[j].peBlue
                            : 224 - nFadoValu * nTimerCunt;
                    workPalet[n][j].peFlags = lpPal[j].peFlags;
                }
                if (bFadePaletDisable[n] == 0)
                    OEClrset(workPalet[n], n);
            }
        }
    }
}

void OEOpenSta(void) {
    if (nTimerCunt == 0) {

        OEGridCreate(0);
        if (bFirstTitle != 0)
            OESetSequenceNum(2);
        else
            OESetSequenceNum(5);
    }
}

void OESegaLogo(void) {
    OEAllSprDelete();
    if (nTimerCunt == 0) {

        OEClrset(tblPal1a, 0);
    } else if (nTimerCunt == 90) {

        OESprDelete(1);
        OESetSequenceNum(3);
        OEClrset(tblPal1, 0);
    } else
        OESprCreate(1);
}

void OEDark(void) {
    if (nTimerCunt == 30) {

        OESetSequenceNum(5);
    }
}

void OESonic(void) {
    OEAllSprDelete();
    if (nTimerCunt == 0) {
        CDPlay(26);
    } else if (nTimerCunt < 30) {

        OESprCreate(2);
    } else if (nTimerCunt < 32) {

        OESprCreate(3);
    } else if (nTimerCunt < 35) {

        OESprCreate(4);
    } else if (nTimerCunt < 38) {

        OESprCreate(5);
    } else {

        OESetSequenceNum(6);
    }
}

Sint32 OEEnd(void) {
    Sint32 ret = 0;

    if (nTimerCunt == 0) {

        CDPause(2);
        switch (nMenuKind) {

        case 0:
            ret = 1;
            break;
        case 1:
            ret = 2;
            break;
        case 2:
            ret = 3;
            break;
        case 3:
            ret = 4;
            break;
        case 4:
            ret = 5;
            break;
        case 5:
            ret = 6;
            break;
        case 7:
            ret = 7;
            break;
        case 8:
            ret = 13;
            break;
        case 9:
            ret = 14;
            break;
        case 10:
            ret = 20;
            break;

        case 6:
            ret = 21;
            break;
        }
    }

    return ret;
}
