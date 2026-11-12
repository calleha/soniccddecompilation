#include "../../types.h"
#include "lpl_types.h"
#include "spm_equ.h"
#include "lplmain.h"
#include "actm.h"
#include "asciiset.h"
#include "flash.h"
#include "kaiten.h"
#include "pls.h"

static void ram_clear(void);
static void cgdata_unlze(void);
static void kaiten_req(void);
static void m2copy(void);
static void cgchange_chk(void);
static void lp_makeup(void);
static Sint32 init_dsp(void);
static void end_proc(void);
static void prg_init(void);
static void patchg_init(void);
static void prgend_chk(void);
static void cgdata_change(void);
static void SubCpuMain(void);
static void cgmwrt_a(void);
static void cgmwrt_b(void);
static void cd_call(void);

static char *ScrAMapFileName[3] = {"../TITLE/PLANET/MAP/LP.MAP",
                                   "../TITLE/PLANET/MAP/LPG.MAP",
                                   "../TITLE/PLANET/MAP/LPB.MAP"};
/* 26 is what the reference declares for "TITLE\PLANET\MAP\LPBS.MAP"; the port
   needs the "../" prefix, so the array has to be longer or the name is cut. */
static char ScrBMapFileName[32] = "../TITLE/PLANET/MAP/LPBS.MAP";
static Uint32 bGameInit = 1;
extern PALETTEENTRY init_col[];
extern Sint16 *funka_map_tbl[];
extern Sint16 funka_timetbl[];
extern PALETTEENTRY ascii_col[];
extern PALETTEENTRY sprite_col1[];
extern PALETTEENTRY **lp_coltbl[];
extern Sint16 lpbs_timer[];
extern PALETTEENTRY *sprite_color[];
static Sint16 DATable[39] = {3,  36, 4,  5,  6,  37, 7,  8,  9,  38,
                             10, 11, 12, 39, 13, 14, 15, 40, 16, 17,
                             18, 41, 19, 20, 21, 42, 22, 23, 25, 33,
                             31, 28, 24, 30, 29, 26, 32, 34, 35};
dlink_export ExportedFunctions = {&game_init,
                                  (void (*)(void)) & game,
                                  &DLL_meminit,
                                  &DLL_memfree,
                                  (void (*)(Sint16, Sint16)) & SWdataSet,
                                  0,
                                  0,
                                  0,
                                  &FadeProc,
                                  0,
                                  0,
                                  0};
static Sint32 MakeupMode;
static Sint32 InitMode;
static Uint32 hWordRAM;
static game_info *lpKeepWork;
static Sint16 ScrBMap[704];
static Uint32 hSmAdr;
void (*sOutputDebugString)(char *);
void (*sPrintf)(char *, char *);
Sint32 (*sRandom)(void);
void *(*sMemAlloc)(Sint32);
void (*sMemFree)(void *);
void (*sMemSet)(void *, Uint8, Sint32);
void (*sMemCpy)(void *, void *, Sint32);
Sint32 (*sOpenFile)(char *);
Sint32 (*sReadFile)(Sint32, void *, Sint32);
Sint32 (*sGetFileSize)(Sint32);
void (*sCloseFile)(Sint32);
Sint32 (*SetGrid)(Sint32, Sint32, Sint32, Sint32, Sint32);
void (*EAsprset)(Sint16, Sint16, Uint16, Uint16, Uint16);
void (*ClrSpriteDebug)(void);
void (*WaveRequest)(Sint16);
void (*CDPlay)(Sint16);
void (*ChangeTileBmp)(Sint32, Sint32);
void (*WaveAllStop)(void);
pad_status *(*sPeriPadGet)(Uint32);

void DLL_meminit(char ***pBufTbl, void **pFuncTbl) {
    Sint32 x, y;
    Uint32 hf;
    Sint16 s;
    Sint16 *pSmap;

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
    WaveAllStop = *pFuncTbl++;

    sMemAlloc = *pFuncTbl++;
    sMemFree = *pFuncTbl++;
    sMemSet = *pFuncTbl++;
    sMemCpy = *pFuncTbl++;
    ++pFuncTbl;
    sRandom = *pFuncTbl++;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    sPrintf = *pFuncTbl++;
    sOutputDebugString = *pFuncTbl++;
    sOpenFile = *pFuncTbl++;
    sReadFile = *pFuncTbl++;
    sCloseFile = *pFuncTbl++;
    ++pFuncTbl;

    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;

    sPeriPadGet = *pFuncTbl++;

    ++pBufTbl;
    lpcolorwk0 = (PALETTEENTRY *)**pBufTbl++;
    lpcolorwk1 = (PALETTEENTRY *)**pBufTbl++;
    lpcolorwk2 = (PALETTEENTRY *)**pBufTbl++;
    lpcolorwk3 = (PALETTEENTRY *)**pBufTbl++;
    lphscrollbuff = (int_union *)**pBufTbl++;
    lpFadeFlag = (Sint32 *)*pBufTbl++;
    lpKeepWork = (game_info *)*pBufTbl++;

    lpKeepWork->pSprBmp = SprBmp;
    lpKeepWork->ptv_adr = (Uint16 *)tv_adr;

    MakeupMode = 0;
    InitMode = 0;

    wordram.wordram = sMemAlloc(0x40000);

    lpKeepWork->sm_adr0 = sMemAlloc(32768);

    if ((hf = sOpenFile(ScrAMapFileName[0])) != -1) {
        pSmap = (Sint16 *)lpKeepWork->sm_adr0;
        for (y = 0; y < 7; ++y) {
            for (x = 0; x < 8; ++x) {
                if (sReadFile(hf, &s, sizeof(s)) != 2) {
                    sCloseFile(hf);
                    return;
                }
                *pSmap++ = s;
            }
            while (x < 10)
                *pSmap++ = 0, ++x;
        }
        sCloseFile(hf);
    }
}

void DLL_memfree(void) {

    sMemFree(wordram.wordram);
    sMemFree(lpKeepWork->sm_adr0);
}

void SWdataSet(ushort_union sw1, ushort_union sw2) {
    swdata1.w = sw1.w;
    swdata2.w = sw2.w;
}

void game_init(void) {
    PALETTEENTRY *lpSrc, *lpDst;
    Sint32 i;
    Uint32 ProcEnd;
    Uint32 hf;

    sMemSet(ScrBMap, 0, sizeof(ScrBMap));
    if ((hf = sOpenFile(ScrBMapFileName)) != -1) {
        (sReadFile(hf, ScrBMap, sizeof(ScrBMap)) ^ sizeof(ScrBMap)) > 0;

        sCloseFile(hf);
    }

    ProcEnd = 0;
    do {
        switch (InitMode) {

        case 0:
            WorkRamClear();

            comdata_m0 = 0;
            comdata_m1 = 0;
            comdata_m2 = 0;
            comdata_m3 = 0;
            comdata_m4 = 0;
            comdata_m5 = 0;
            comdata_m6 = 0;
            comdata_m7 = 0;
            swdata1.w = 32768;
            main_lpcnt = 1;

            comdata_s0 = 0;
            comdata_s1 = 0;
            comdata_s2 = 0;
            comdata_s3 = 0;
            comdata_s4 = 0;
            comdata_s5 = 0;
            comdata_s6 = 0;
            comdata_s7 = 0;
            comflag_s &= 3;
            ram_clear();
            kaiten_init();
            cgdata_unlze();

            patchg_init();

            cgmwrt_a();
            cgmwrt_b();

            flash_flg = 1;
        case 1:
        case 2:
        case 3:
        case 4:
            if (init_dsp() == 0)
                break;
            comflag_m = 0;
            ProcEnd = 1;

            break;

        case 5:
            prg_init();
            lpSrc = init_col;
            lpDst = lpcolorwk1;
            for (i = 0; i < 64; ++i)
                *lpDst++ = *lpSrc++;
            fadein();

        default:
            ProcEnd = 1;
        }

        v_int();
    } while (ProcEnd == 0);
    CDPlay(33);
    bGameInit = 0;
}

Sint32 game(void) {
    Sint32 RetMode;

    RetMode = 0;
    switch (ProcMode) {
    case 0:
        if (main_lpcnt < 0) {
            main_lpcnt = 1;
            intmode = 0;
        }
        kaiten_req();
        sprascii();
        action();
        SubCpuMain();
        ++ProcMode;
        break;

    case 1:
        ++intmode;
        sprascii();
        action();
        ++ProcMode;
        break;

    case 2:
        set_asciicg();
        m2copy();
        cgchange_chk();
        if (comflag_s & 32) {
            MakeupMode = 0;
            lp_makeup();
        } else {
            ++ProcMode;
        }
        ++ProcMode;
        break;

    case 3:
        lp_makeup();
        if (!(comflag_m & 32)) {
            comflag_s &= 223;
            comflag_s |= 16;
        }
        if (MakeupMode < 0)
            ++ProcMode;
        break;

    case 4:
        if (comflag_s & 64) {
            prgend_chk();
            ++ProcMode;
        } else {
            --main_lpcnt;
            ++intmode;
            ProcMode = 0;
        }
        break;

    case 5:
        comflag_m = 0;
        comflag_s = 0;
        RetMode = -1;
        break;
    }

    v_int();
    return RetMode;
}

static void ram_clear(void) {}

static void cgdata_unlze(void) {}

static void kaiten_req(void) {}

static void m2copy(void) {}

static void cgchange_chk(void) {
    Sint16 fPatnoWk, *pMaptbl, *pWkadr, wk, funka_off;
    Sint32 i, j;

    switch (lplanet_no) {
    case 0:
    case 1:
        funka_off = 620;
        break;

    case 2:
        funka_off = 720;
        break;

    default:
        return;
    }
    if (funka_pctimer == 0) {
        if (funka_patno >= 12)
            funka_patno = 0;
        fPatnoWk = funka_patno;
        funka_pctimer = funka_timetbl[fPatnoWk];
        ++funka_patno;
        pMaptbl = funka_map_tbl[fPatnoWk];
        pWkadr = (Sint16 *)lpKeepWork->sm_adr0 + 6;
        for (i = 0; i < 2; ++i) {
            for (j = 0; j < 2; ++j) {
                if ((wk = *pMaptbl++) != 0)
                    wk += funka_off;
                *pWkadr++ = wk;
                SetGrid(-1, j + 6, i, wk, 0);
            }
            pWkadr += 126;
        }
    }
}

static void lp_makeup(void) {
    switch (MakeupMode) {
    case 0:
        flashout();
        ++MakeupMode;
        break;

    case 1:
        sMemSet(actwk, 0, 960);
        prg_init();
        comflag_m &= 223;
        kaiten_req();
        m2copy();
        SubCpuMain();
        if (comflag_s & 64) {
            MakeupMode = -1;
        } else
            ++MakeupMode;
        break;

    case 2:
        intmode = 0;
        kaiten_req();
        m2copy();
        SubCpuMain();
        if (comflag_s & 64) {
            MakeupMode = -1;
        } else
            ++MakeupMode;
        break;

    case 3:
        intmode = 1;
        if (comflag_s & 64) {
            MakeupMode = -1;
        } else
            ++MakeupMode;
        break;

    case 4:
        intmode = 2;
        kaiten_req();
        m2copy();
        SubCpuMain();
        if (comflag_s & 64) {
            MakeupMode = -1;
        } else
            ++MakeupMode;
        break;

    case 5:
        intmode = 3;
        if (comflag_s & 64) {
            MakeupMode = -1;
        } else
            ++MakeupMode;
        break;

    case 6:
        music_selflg &= 254;
        actset_flg = 0;
        flashin();
        ++MakeupMode;
        break;

    case 7:
        main_lpcnt = 0;
        intmode = -1;
        MakeupMode = -1;
        break;

    default:
        MakeupMode = -1;
    }
}

static Sint32 init_dsp(void) {
    switch (InitMode) {
    case 0:
        kaiten_req();
        m2copy();
        SubCpuMain();
        cgchange_chk();
        ++InitMode;
        break;

    case 1:
        intmode = 1;
        if (comflag_s & 64)
            end_proc();
        ++InitMode;
        break;

    case 2:
        kaiten_req();
        m2copy();
        SubCpuMain();
        cgchange_chk();
    case 3:
        ++intmode;
        ++InitMode;
        break;

    case 4:
        if (comflag_s & 64)
            end_proc();
        else
            ++InitMode;
    }

    return 0;
}

static void end_proc(void) {
    flash_flg = 1;
    fadeout();
}

static void prg_init(void) {
    PALETTEENTRY *lpPlCol, *lpBkgCol, *lpDst;
    Sint32 i;

    lpPlCol = *lp_coltbl[lplanet_no * 2];
    lpBkgCol = *lp_coltbl[lplanet_no * 2 + 1];
    lpDst = lpcolorwk1;
    for (i = 0; i < 16; ++i)
        *lpDst++ = *lpBkgCol++;
    for (i = 0; i < 16; ++i)
        *lpDst++ = *lpPlCol++;
    lpPlCol = sprite_col1;
    for (i = 0; i < 16; ++i)
        *lpDst++ = *lpPlCol++;
    lpPlCol = ascii_col;
    for (i = 0; i < 16; ++i)
        *lpDst++ = *lpPlCol++;
    patchg_init();
}

static void patchg_init(void) {
    Sint16 *pMapwk, *pMapadr, funka_off;
    Sint32 i, wk;

    colorno = 0;
    coltimer = lpbs_timer[0];
    funka_patno = 0;

    funka_pctimer = 0;
    pMapwk = funka_map_tbl[0];
    pMapadr = (Sint16 *)lpKeepWork->sm_adr0 + 6;
    switch (lplanet_no) {
    case 0:
    case 1:
        funka_off = 620;
        break;

    case 2:
        funka_off = 720;
        break;

    default:
        funka_off = 0;
    }
    for (i = 0; i < 2; ++i) {
        wk = funka_off + *pMapwk++;
        *pMapadr++ = wk;
        SetGrid(-1, 6, i, wk, 0);
        wk = funka_off + *pMapwk++;
        *pMapadr++ = wk;
        SetGrid(-1, 7, i, wk, 0);
        pMapadr += 127;
    }
    spr_coltimer = 5;
    spr_colno = 0;
}

static void prgend_chk(void) {
    flash_flg = 1;
    fadeout();
}

static void cgdata_change(void) {
    Uint16 *pA0, *pA1, *pA2, *pSmap, s, offs;
    Sint32 i, j, x, y;
    Uint32 hf;

    if (comdata_m5 != 3) {

        if ((hf = sOpenFile(ScrAMapFileName[comdata_m5])) != -1) {
            switch (comdata_m5) {
            case 0:
                offs = 0;
                break;

            case 1:
                offs = 200;
                break;

            case 2:
                offs = 416;
                break;

            default:
                offs = 0;
                break;
            }
            pSmap = lpKeepWork->sm_adr0;
            for (y = 0; y < 7; ++y) {
                for (x = 0; x < 8; ++x) {
                    if (sReadFile(hf, &s, sizeof(s)) != sizeof(s)) {
                        sCloseFile(hf);
                        return;
                    }
                    if (s != 0)
                        s += offs;
                    *pSmap++ = s;
                }
                while (x < 10)
                    *pSmap++ = 0, ++x;
            }
            sCloseFile(hf);
        }

        switch (lplanet_no) {
        case 0:
        case 1:
            offs = 620;
            break;

        case 2:
            offs = 720;
            break;

        default:
            offs = 0;
        }
        pA0 = (Uint16 *)funka_map_tbl[0];
        pA2 = lpKeepWork->sm_adr0 + 6;
        for (i = 1; i >= 0; --i) {
            pA1 = pA2;
            for (j = 1; j >= 0; --j) {
                s = *pA0++;
                if (s != 0)
                    s += offs;
                *pA1++ = s;
            }
            pA2 += 128;
        }
        pSmap = lpKeepWork->sm_adr0;
        for (y = 0; y < 7; ++y) {
            for (x = 0; x < 10; ++x) {
                SetGrid(-1, x, y, *pSmap, 0);
                ++pSmap;
            }
        }
    }
}

static void SubCpuMain(void) {
    if (bGameInit == 0)
        player();

    if (comflag_s & 16) {
        cd_call();
        cgdata_change();
        comflag_s &= 239;
    }
    kaiten0();
    comflag_s &= 223;
    if (comflag_m & 32)
        comflag_s |= 32;
    else
        comflag_s &= 223;
}

void v_int(void) {
    PALETTEENTRY *lpPlanet, *lpBkg, *lpDst, *lpSpr;
    Sint32 i;

    switch (intmode) {
    case 0:

    case 1:

    case 2:

    case 3:

    default:
        if (flash_flg == 0) {
            if (--coltimer == 0) {
                if (colorno >= 31)
                    colorno = 0;
                coltimer = lpbs_timer[colorno];
                lpPlanet = lp_coltbl[lplanet_no * 2][colorno];
                lpBkg = lp_coltbl[lplanet_no * 2 + 1][colorno];
                ++colorno;
                lpDst = lpcolorwk0;
                for (i = 0; i < 16; ++i)
                    *lpDst++ = *lpBkg++;
                for (i = 0; i < 16; ++i)
                    *lpDst++ = *lpPlanet++;
            }
            if (--spr_coltimer <= 0) {
                lpSpr = sprite_color[spr_colno / 2];
                lpDst = lpcolorwk0 + 32;
                for (i = 0; i < 16; ++i)
                    *lpDst++ = *lpSpr++;
                if (spr_colno == 0)
                    spr_colno = 2;
                else
                    spr_colno = 0;
                spr_coltimer = 5;
            }
        }
    }
    if (funka_pctimer > 0)
        --funka_pctimer;
    if (gmtime0 != 0)
        --gmtime0;
    ++gmtime1;
}

static void cgmwrt_a(void) {
    Sint32 x, y;

    for (y = 0; y < 3; ++y) {
        for (x = 0; x < 40; ++x) {
            SetGrid(0, x, y, 32818, 0);
        }
    }
    for (y += 22; y < 28; ++y) {
        for (x = 0; x < 40; ++x) {
            SetGrid(0, x, y, 32818, 0);
        }
    }
}

static void cgmwrt_b(void) {
    Sint16 *pA1;
    Sint32 x, y, i;

    for (y = 0; y < 3; ++y) {
        for (x = 0; x < 40; ++x) {
            SetGrid(1, x, y, 49, 0);
        }
    }
    pA1 = ScrBMap;
    for (; y < 25; ++y) {
        for (x = 0; x < 32; ++x) {
            SetGrid(0, x, y, *pA1, 0);
            ++pA1;
        }
        pA1 -= 11;
        for (i = 1; i <= 8; ++i) {
            SetGrid(0, x, y, pA1[i], 0);
            ++x;
        }
        pA1 += 11;
    }
    for (; y < 28; ++y) {
        for (x = 0; x < 40; ++x) {
            SetGrid(1, x, y, 49, 0);
        }
    }
}

static void cd_call(void) { CDPlay(DATable[comdata_m4]); }
