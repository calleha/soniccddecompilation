#include "../../types.h"
#include "../common/score_data_types.h"
#include "thanks_types.h"
#include "game.h"
#include "bm_m.h"
#include "etc_m.h"
#include "io.h"
#include "spm.h"

static PALETTEENTRY endingcol[64] = {
    {0, 0, 0, 1},       {0, 0, 0, 1},       {0, 0, 0, 1},
    {0, 0, 224, 1},     {96, 96, 224, 1},   {128, 128, 224, 1},
    {224, 224, 224, 1}, {160, 160, 160, 1}, {128, 128, 128, 1},
    {64, 64, 64, 1},    {0, 0, 0, 1},       {0, 0, 0, 1},
    {0, 0, 0, 1},       {0, 0, 0, 1},       {0, 0, 0, 1},
    {224, 224, 0, 1},   {0, 0, 0, 1},       {0, 0, 0, 1},
    {0, 0, 0, 1},       {0, 0, 0, 1},       {0, 0, 0, 1},
    {0, 0, 0, 1},       {0, 0, 0, 1},       {0, 0, 0, 1},
    {0, 0, 0, 1},       {0, 0, 0, 1},       {0, 0, 0, 1},
    {0, 0, 0, 1},       {0, 0, 0, 1},       {0, 0, 0, 1},
    {0, 0, 0, 1},       {0, 0, 0, 1},       {0, 0, 0, 1},
    {0, 0, 0, 1},       {0, 0, 0, 1},       {0, 0, 0, 1},
    {0, 0, 0, 1},       {0, 0, 0, 1},       {0, 0, 0, 1},
    {0, 0, 0, 1},       {0, 0, 0, 1},       {0, 0, 0, 1},
    {0, 0, 0, 1},       {0, 0, 0, 1},       {0, 0, 0, 1},
    {0, 0, 0, 1},       {0, 0, 0, 1},       {0, 0, 0, 1},
    {0, 0, 0, 1},       {0, 0, 0, 1},       {0, 0, 0, 1},
    {0, 0, 0, 1},       {0, 0, 0, 1},       {0, 0, 0, 1},
    {0, 0, 0, 1},       {0, 0, 0, 1},       {0, 0, 0, 1},
    {0, 0, 0, 1},       {0, 0, 0, 1},       {0, 0, 0, 1},
    {0, 0, 0, 1},       {0, 0, 0, 1},       {0, 0, 0, 1},
    {0, 0, 0, 1}};
static char MapFileA[] = "../TITLE/THANKS/MAP/VA_MAP.BIN";
static char MapErrorA[] = "Map Load Error [va_map.bin]\n";
static char MapFileB[] = "../TITLE/THANKS/MAP/VB_MAP.BIN";
static char MapErrorB[] = "Map Load Error [vb_map.bin]\n";
static Uint8 SeToWavTbl[80] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 20, 21, 22, 22, 23, 23, 24, 25, 26, 27,
    28, 29, 30, 31, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54,
    55, 56, 57, 58, 58, 59, 60, 61, 62, 63, 64, 65, 67, 68, 69, 70,
    71, 71, 72, 73, 74, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42};
extern bmp_info SprBmp[700];
extern Sint32 *lpFadeFlag;
extern int_union *lphscrollbuff;
extern PALETTEENTRY *lpcolorwk3;
extern PALETTEENTRY *lpcolorwk2;
extern PALETTEENTRY *lpcolorwk1;
extern PALETTEENTRY *lpcolorwk0;
extern short_union swdata2;
extern short_union swdata1;
extern Sint16 end_timer;
extern Sint16 heart_tm;
extern Sint16 posetimer;
extern Sint16 colorno2;
extern Sint16 coltimer2;
extern Sint16 colorno;
extern Sint16 coltimer;
extern Uint16 lpbs_timer[32];
extern Uint8 quit_flg;
extern PALETTEENTRY *asc_coltbl[2];
extern PALETTEENTRY *lpbs_good[32];
extern Sint16 animal_timer;
extern Uint8 animal_flg;
extern sprite_status_thanks actwk[66];
extern Uint16 *asc_maptbl[];
dlink_export ExportedFunctions = {&game_init,
                                  (void (*)(void)) & game,
                                  &DLL_meminit,
                                  &DLL_memfree,
                                  (void (*)(Sint16, Sint16)) & SWdataSet,
                                  0,
                                  0,
                                  0,
                                  0,
                                  0,
                                  0,
                                  0};
static Uint16 gEndingMesTimer = 0;
void (*ChangeTileBmp)(Sint32, Sint32) = 0;
void (*CDPlay)(Sint16) = 0;
void (*WaveRequest)(Sint16) = 0;
void (*ClrSpriteDebug)(void) = 0;
void (*EAsprset)(Sint16, Sint16, Uint16, Uint16, Uint16) = 0;
Sint32 (*SetGrid)(Sint32, Sint32, Sint32, Sint32, Sint32) = 0;
char *(*sStrcpy)(char *, char *) = 0;
void (*sCloseFile)(Sint32) = 0;
Sint32 (*sGetFileSize)(Sint32) = 0;
Sint32 (*sReadFile)(Sint32, void *, Sint32) = 0;
Sint32 (*sOpenFile)(char *) = 0;
Sint32 (*sMemCmp)(void *, void *, Sint32) = 0;
void (*sMemCpy)(void *, void *, Sint32) = 0;
void (*sMemSet)(void *, Uint8, Sint32) = 0;
Sint32 (*sRandom)(void) = 0;
void (*sPrintf)(char *, char *) = 0;
void (*sOutputDebugString)(char *) = 0;
static game_info *lpKeepWork = 0;
score_data *lpScorData = 0;
Uint32 *lpghWnd = 0;
Uint32 *lphSurf = 0;

void DLL_meminit(char ***pBufTbl, void **pFuncTbl) {

    SetGrid = *pFuncTbl++;
    EAsprset = *pFuncTbl++;
    ClrSpriteDebug = *pFuncTbl++;
    WaveRequest = *pFuncTbl++;
    CDPlay = *pFuncTbl++;
    ++pFuncTbl;
    ChangeTileBmp = *pFuncTbl++;

    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;

    ++pFuncTbl;
    ++pFuncTbl;
    sMemSet = *pFuncTbl++;
    sMemCpy = *pFuncTbl++;
    sMemCmp = *pFuncTbl++;
    sRandom = *pFuncTbl++;
    sStrcpy = *pFuncTbl++;
    ++pFuncTbl;
    ++pFuncTbl;
    sPrintf = *pFuncTbl++;
    sOutputDebugString = *pFuncTbl++;
    sOpenFile = *pFuncTbl++;
    sReadFile = *pFuncTbl++;
    sCloseFile = *pFuncTbl++;

    ++pBufTbl;
    lpcolorwk0 = (PALETTEENTRY *)**pBufTbl++;
    lpcolorwk1 = (PALETTEENTRY *)**pBufTbl++;
    lpcolorwk2 = (PALETTEENTRY *)**pBufTbl++;
    lpcolorwk3 = (PALETTEENTRY *)**pBufTbl++;
    lphscrollbuff = (int_union *)**pBufTbl++;
    lpFadeFlag = (Sint32 *)*pBufTbl++;
    lpKeepWork = (game_info *)*pBufTbl++;
    lphSurf = (Uint32 *)*pBufTbl++;
    lpghWnd = (Uint32 *)*pBufTbl++;
    lpScorData = (score_data *)*pBufTbl++;

    lpKeepWork->pSprBmp = SprBmp;
}

void DLL_memfree(void) {}

void SWdataSet(ushort_union sw1, ushort_union sw2) {
    swdata1.w = sw1.w;
    swdata2.w = sw2.w;
}

void game_init(void) {
    WorkRamClear();

    swdata1.w = -4096;

    cgmwrt_a();
    cgmwrt_b();

    EndingMesColor();

    coltimer = lpbs_timer[0];
    colorno = 0;

    coltimer2 = 20;
    colorno2 = 1;

    posetimer = 1536;
    heart_tm = 1;

    end_timer = 10800;
}

Sint32 game(void) {

    if (EndingMes() != 0)
        return 0;
    if (lpKeepWork->EndingMesType & 2) {
        return -1;
    }

    mainloop();

    v_int();

    if (quit_flg)
        return -1;
    return 0;
}

void cgmwrt_a(void) {
    Sint32 x, y, TileNo;
    Sint16 aMap[2048], *pMap;
    Uint32 hf;

    sMemSet(aMap, 0, sizeof(aMap));
    if ((hf = sOpenFile(MapFileA)) == -1) {

        sOutputDebugString(MapErrorA);
    } else {

        sReadFile(hf, aMap, sizeof(aMap));
        sCloseFile(hf);
    }
    for (y = 0; y < 28; ++y) {

        pMap = &aMap[y * 64];
        for (x = 0; x < 40; ++x) {

            TileNo = *pMap++;
            if (TileNo & 1023)
                TileNo += 174;
            if ((TileNo & 1023) >= 461)
                TileNo = 0;

            SetGrid(0, x, y, TileNo, 0);
        }
    }
}

void cgmwrt_b(void) {
    Sint32 x, y, TileNo;
    Sint16 bMap[2048], *pMap;
    Uint32 hf;

    sMemSet(bMap, 0, sizeof(bMap));
    if ((hf = sOpenFile(MapFileB)) == -1) {

        sOutputDebugString(MapErrorB);
    } else {

        sReadFile(hf, bMap, sizeof(bMap));
        sCloseFile(hf);
    }
    for (y = 0; y < 28; ++y) {

        pMap = &bMap[y * 64];
        for (x = 0; x < 40; ++x) {

            TileNo = *pMap++;
            if (TileNo & 1023)
                TileNo -= 154;
            if ((TileNo & 1023) >= 329)
                TileNo = 0;
            SetGrid(1, x, y, TileNo, 0);
        }
    }
}

void v_int(void) {
    PALETTEENTRY *lpDst, *lpSpr;
    int_union ld0;
    Sint16 i;
    Sint16 d0;

    if (animal_flg == 2) {

        if (--animal_timer <= 0) {

            animal_flg = 0;
        }
    }
    if (--posetimer <= 0) {

        if (actwk[0].ACT_NO) {

            animal_flg = 1;
        }
        animal_timer = 900;
        ld0.l = random();
        ld0.l &= 32767;
        ld0.w.l = (Sint32)ld0.w.l % 4096;
        ld0.w.l += 8192;
        posetimer = ld0.w.l;
    }

    --coltimer;
    if (coltimer <= 0) {

        if (colorno >= 31) {

            colorno = 0;
        }
        d0 = colorno;
        coltimer = lpbs_timer[d0];
        ++colorno;

        lpSpr = lpbs_good[d0];
        lpDst = lpcolorwk0 + 16;
        for (i = 0; i < 16; ++i) {

            *lpDst++ = *lpSpr++;
        }
    }

    --coltimer2;
    if (coltimer2 <= 0) {

        if (colorno2 >= 2) {

            colorno2 = 0;
        }
        coltimer2 = 20;
        d0 = colorno2;
        ++colorno2;

        lpSpr = asc_coltbl[d0];
        lpDst = lpcolorwk0 + 40;
        for (i = 0; i < 8; ++i) {

            *lpDst++ = *lpSpr++;
        }
    }

    if (--end_timer <= 0) {

        quit_flg = 1;
    }
}

void PutAscii(Uint16 c, Uint16 XPos, Uint16 YPos) {
    Sint32 vcellno, i;
    Uint16 *pMap;

    pMap = asc_maptbl[c];
    if (c == 27) {
        vcellno = 4;
    } else
        vcellno = 3;

    for (i = 0; i < vcellno; ++i) {
        SetGrid(0, XPos, YPos + i, *pMap++ + 32767 + 330, 0);
        SetGrid(0, XPos + 1, YPos + i, *pMap++ + 32767 + 330, 0);
    }
}

void soundset(Sint16 ReqNo) {
    if (ReqNo == 171)
        return;

    ReqNo -= 144;
    ReqNo = (Uint16)SeToWavTbl[ReqNo];

    WaveRequest(ReqNo);
}

void EndingMesColor(void) {
    PALETTEENTRY *lpPeSrc, *lpPeDest;
    Sint32 i;

    lpPeSrc = endingcol;
    lpPeDest = lpcolorwk0;
    for (i = 0; i < 64; ++i) {
        *lpPeDest++ = *lpPeSrc++;
    }
}

Sint32 EndingMes(void) {
    Sint32 i;

    ++gEndingMesTimer;
    /* The ending cutscenes this port plays already end with the "TRY AGAIN" /
       "YOU'RE TOO COOL!" line (sprites 75 and 76), so the ticks that would draw
       it again are skipped and the screen starts on "PRESENTED BY SEGA"
       (sprite 77).  The reference draws both, one after the other. */
    if (gEndingMesTimer < 180) {
        gEndingMesTimer = 180;
    }
    if (gEndingMesTimer > 360) {
        gEndingMesTimer = 360;
        return 0;
    }

    if (lpKeepWork->EndingMesType & 1) {
        switch (gEndingMesTimer) {

        case 1:
            EAsprset(224, 288, 76, 0, 0);
            break;

        case 180:
            EAsprset(208, 228, 77, 0, 0);
            break;
        case 360:
            if ((Uint8)swdata1.b.l & 240) {
                vdpinit();
            } else {
                --gEndingMesTimer;
            }
            break;
        }
    } else if (lpKeepWork->EndingMesType & 2) {
        switch (gEndingMesTimer) {

        case 1:
            EAsprset(252, 288, 75, 0, 0);
            break;

        case 180:
            EAsprset(208, 228, 77, 0, 0);
            break;
        case 360:
            if ((Uint8)swdata1.b.l & 240) {

                for (i = 0; i < 64; ++i) {
                    lpcolorwk0[i].peRed = 0;
                    lpcolorwk0[i].peGreen = 0;
                    lpcolorwk0[i].peBlue = 0;
                }
            } else {
                --gEndingMesTimer;
            }
        }
    }

    return 1;
}
