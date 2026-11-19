#include "../../types.h"
#include "../common/score_data_types.h"
#include "../common/hmx_types.h"
#include "ta.h"
#include "fadein.h"
#include "tacolor.h"
#include "taeactrl.h"

Sint32 gMove = 0;
Sint32 gRankY = 0;
Sint32 gRankX = 0;
static Sint32 bExit = 0;
hmx_environment *g_env_module = 0;
hmx_environment *g_loader_module = 0;
static Uint16 KeyRepTimer = 0;
draw_context *s_ctx = 0;
static Sint32 gRet = 0;
score_data *lpScoreData = 0;
Sint32 gNewRankX = 0;
Uint32 gTimer = 0;
Sint32 gNewMenuZone = 0;
Sint32 gMenuZone = 0;
Sint32 gNewMenuRound = 0;
Sint32 gMenuRound = 0;
Sint32 gNewMenu2 = 0;
Sint32 gMenu2 = 0;
Sint32 gNewMenu1 = 0;
Sint32 gMenu1 = 0;
ushort_union swdata2 = {0};
ushort_union swdata1 = {0};
UintPtr ghWnd = 0;
UintPtr hSurf = 0;
void (*hmx_sprite_set_bitmap_module)(hmx_sprite *, hmx_bitmap *) = 0;
void (*hmx_sprite_set_position_module)(hmx_sprite *, Sint32, Sint32) = 0;
hmx_renderer_base *(*hmx_sprite_base_module)(hmx_sprite *) = 0;
void (*hmx_renderer_context_draw_module)(hmx_renderer_context *,
                                         hmx_surface *) = 0;
void (*hmx_renderer_context_clear_module)(hmx_renderer_context *) = 0;
void (*hmx_renderer_context_add_module)(hmx_renderer_context *, Sint32,
                                        hmx_renderer_base *) = 0;
void (*hmx_grid_set_tile_module)(hmx_grid *, Sint32, Sint32, hmx_bitmap *,
                                 Sint32) = 0;
void (*hmx_grid_set_view_module)(hmx_grid *, Sint32, Sint32, Sint32,
                                 Sint32) = 0;
void (*hmx_grid_set_position_module)(hmx_grid *, Sint32, Sint32) = 0;
hmx_renderer_base *(*hmx_grid_base_module)(hmx_grid *) = 0;
void (*hmx_grid_release_module)(hmx_environment *, hmx_grid *) = 0;
hmx_grid *(*hmx_grid_create_module)(hmx_environment *, Sint32, Sint32, Sint32,
                                    Sint32) = 0;
void (*hmx_free_module)(hmx_environment *, void *) = 0;
void (*hmx_bitmap_set_transparency_module)(hmx_bitmap *, Sint32) = 0;
void *(*hmx_bitmap_get_scan0_module)(hmx_bitmap *) = 0;
void (*hmx_bitmap_release_module)(hmx_environment *, hmx_bitmap *) = 0;
hmx_bitmap *(*hmx_bitmap_create_module)(hmx_environment *, Sint32, Sint32) = 0;
void (*hmx_background_set_background_module)(hmx_background *, Sint32) = 0;
void (*ld_bitmap_4to8_module)(void *, void *, Sint32, Sint32, Sint32, Sint32,
                              Sint32) = 0;
void *(*ld_load_cmpfile_module)(hmx_environment *, char *) = 0;
void (*FlipToScreen_module)(void) = 0;
draw_context *(*get_draw_context_module)(void) = 0;
void (*sOutputDebugString)(char *) = 0;
void (*sPrintf)(char *, const char *, ...) = 0;
void (*sCloseFile)(Sint32) = 0;
Sint32 (*sReadFile)(Sint32, void *, Sint32) = 0;
Sint32 (*sOpenFile)(char *) = 0;
Sint32 (*sStrncmp)(char *, char *, Sint32) = 0;
char *(*sStrncpy)(char *, char *, Sint32) = 0;
void (*sMemFree)(void *) = 0;
void *(*sMemAlloc)(Sint32) = 0;
void (*SetScoreDate2)(score_data *) = 0;
Uint32 (*WriteScoreData2)(Sint32, char *, Uint32) = 0;
Sint32 (*ReadScoreIndx2)(Uint32) = 0;
void (*CDPause)(Sint16) = 0;
void (*CDPlay)(Sint16) = 0;
extern game_info *lpKeepWork;
extern Sint32 *lpFadeFlag;
extern int_union *lphscrollbuff;
extern Uint16 *pmapwk;
extern PALETTEENTRY *lpcolorwk4;
extern PALETTEENTRY *lpcolorwk3;
extern PALETTEENTRY *lpcolorwk2;
extern PALETTEENTRY *lpcolorwk;
static char aNameDefault[4] = "AAA";
static char aNamePlayer[4] = "YOU";
dlink_export ExportedFunctions = {&game_init,
                                  (void (*)(void)) & game,
                                  &DLL_meminit,
                                  &DLL_memfree,
                                  (void (*)(Sint16, Sint16)) & SWdataSet,
                                  0,
                                  0,
                                  0,
                                  FadeProc,
                                  0,
                                  0,
                                  0};

void SWdataSet(ushort_union sw1, ushort_union sw2) {
    swdata1.w = sw1.w;
    swdata2.w = sw2.w;
}

Uint32 SpecialPlayChk(void) {
    if (lpScoreData->total < 109269 && lpScoreData->roundNo >= 7) {

        return 1;
    }
    return 0;
}

Sint32 GetPlayRound(void) {
    Sint32 ret;

    ret = 1;

    if (lpScoreData->roundNo != 0) {
        ret = 1 + lpScoreData->roundNo - 1;
        if (ret > 7)
            ret = 7;
    }

    return ret;
}

Sint32 game(void) {
    char c;
    Sint32 index;

    ++gTimer;
    if (lpKeepWork->bRestart) {
        lpKeepWork->bRestart = 0;
        bExit = 1;
        gRet = -1;
    }
    if (bExit != 0) {

        DeleteEA();

        CDPause(2);
        return gRet;
    }
    if (gMenu1 == 1 && gMenuRound == 6 && gMove == 0) {
        if (gTimer % 4 == 0) {
            TAColorChgSP6();
        }
    }

    if (gMove == 0) {

        if (gMenu2 == 0) {

            if (swdata1.b.l & 64) {
                if (gMenuRound == 0) {

                    bExit = 1;
                    gRet = -1;
                    clwout();
                    return 0;
                }

                if (gMenu1 == 0) {
                    gNewMenu2 = 2;
                    gMove = 1;
                } else {
                    gNewMenu2 = 1;
                    gMove = 2;
                }
                gNewMenuZone = gMenuZone = 0;

            } else if (swdata1.b.h & 4) {
                if (gMenu1 == 0 && SpecialPlayChk() == 1) {

                    gMove = 5;
                    gNewMenu1 = 1;
                }
            } else if (swdata1.b.h & 8) {
                if (gMenu1 == 1) {

                    gMove = 6;
                    gNewMenu1 = 0;
                }
            } else if (swdata1.b.h & 1) {

                gMove = 7;
                gNewMenuRound = gMenuRound - 1;
                if (gNewMenuRound < 0)
                    gNewMenuRound = GetPlayRound();
            } else if (swdata1.b.h & 2) {

                gMove = 7;
                gNewMenuRound = gMenuRound + 1;
                if (gNewMenuRound > GetPlayRound())
                    gNewMenuRound = 0;
            }
        }

        else if (gMenu2 == 4) {

            if (swdata1.b.l) {
                gNewMenu2 = 5;
                gMove = 11;
            }
        }

        else if (gMenu2 == 5) {

            if (swdata1.b.l & 64) {
                if (gRankX == 2) {
                    gNewMenu2 = 3;
                    gMove = 12;

                    index = ReadScoreIndx2(0);
                    if (index != -1) {
                        SetScoreDate2(0);
                        WriteScoreData2(index, 0, 0);
                    }
                } else {

                    swdata1.b.l |= 8;
                }
            }
            if (gMove == 0 && swdata1.b.l & 4) {
                if (gRankX != 0) {
                    gNewRankX = gRankX - 1;
                    gMove = 12;
                }
            } else if (gMove == 0 && swdata1.b.l & 8) {
                if (gRankX < 2) {
                    gNewRankX = gRankX + 1;
                    gMove = 12;
                }
            } else if (gMove == 0) {
                if (swdata1.b.h & 3) {
                    ++KeyRepTimer;
                } else if (swdata1.b.l & 3) {
                    KeyRepTimer = 15;
                }

                if (KeyRepTimer > 15) {
                    KeyRepTimer = 0;
                    if (gMenu1 == 0) {
                        c = lpScoreData
                                ->timeattack[gMenuRound - 1][gMenuZone][gRankY]
                                .name[gRankX];
                    } else {
                        c = lpScoreData->special[gMenuRound - 1][gRankY]
                                .name[gRankX];
                    }
                    if (swdata1.b.h & 2)
                        ++c;
                    else
                        --c;

                    if (c < 48)
                        c = 90;
                    else if (c > 90)
                        c = 48;
                    else if (c == 58)
                        c = 65;
                    else if (c == 64)
                        c = 57;

                    if (gMenu1 == 0) {
                        lpScoreData
                            ->timeattack[gMenuRound - 1][gMenuZone][gRankY]
                            .name[gRankX] = c;
                    } else {
                        lpScoreData->special[gMenuRound - 1][gRankY]
                            .name[gRankX] = c;
                    }
                    gMove = 12;
                }
            }
        } else {

            if (gMenu2 == 2) {
                if (swdata1.b.h & 1) {
                    if (gMenu1 == 0) {

                        gMove = 8;
                        if (--gNewMenuZone < 0) {
                            gNewMenuZone = 2;
                        }
                    }
                } else if ((swdata1.b.h & 2) || swdata1.b.h & 64) {

                    if (gMenu1 == 0) {

                        gMove = 9;
                        if (++gNewMenuZone > 2) {
                            gNewMenuZone = 0;
                        }
                    }
                }
            } else if (swdata1.b.l & 64) {
                if (gMenu2 == 1) {

                    if (gMenu1 == 0)
                        gMove = 3;
                    else
                        gMove = 4;
                    gNewMenu2 = gMenu2 = 0;
                } else if (gMenu2 == 3) {

                    if (gMenu1 == 0) {

                        gRet = (gMenuRound - 1) * 10 + gMenuZone * 4 + 400;
                        if (gMenuZone == 2) {
                            ++gRet;
                        }
                    } else {
                        gRet = gMenuRound + 469;
                    }
                    bExit = 1;
                    lpKeepWork->ta_Menu1 = gMenu1;
                    lpKeepWork->ta_Round = gMenuRound;
                    lpKeepWork->ta_Zone = gMenuZone;
                    clwout();
                    return 0;
                }
            }
            if (gMove == 0) {
                if (swdata1.b.l & 4) {
                    if (gMenu1 == 0) {
                        if (gMenu2 == 2)
                            gNewMenu2 = 1;
                        else if (gMenu2 == 3)
                            gNewMenu2 = 2;
                    } else {
                        if (gMenu2 == 1)
                            gNewMenu2 = 3;
                    }
                    gMove = 10;
                } else if (swdata1.b.l & 8) {
                    if (gMenu1 == 0) {
                        if (gMenu2 == 1)
                            gNewMenu2 = 2;
                        else if (gMenu2 == 2)
                            gNewMenu2 = 3;
                    } else {
                        if (gNewMenu2 == 3)
                            gNewMenu2 = 1;
                    }
                    gMove = 10;
                }
            }
        }
    }

    if (gMove != 0) {
        TAMove();
    } else {
        Tenmetu();
    }

    DrawTA();
    return 0;
}

void game_init(void) {
    Sint32 i, j, k, l;
    Sint32 round;
    Sint32 zone;
    Uint32 Time;

    s_ctx = get_draw_context_module();

    if (lpKeepWork->ta_time == -2) {

        gMenu1 = gNewMenu1 = 0;
        gMenu2 = gNewMenu2 = 0;
        gMenuRound = gNewMenuRound = 1;
        gMenuZone = gNewMenuZone = 0;
    } else {

        gMenu1 = gNewMenu1 = (Uint16)lpKeepWork->ta_Menu1;
        gMenu2 = gNewMenu2 = 3;
        gMenuRound = gNewMenuRound = (Uint16)lpKeepWork->ta_Round;
        gMenuZone = gNewMenuZone = (Uint16)lpKeepWork->ta_Zone;

        if (lpKeepWork->ta_time != -1) {

            round = gMenuRound - 1;
            zone = gMenuZone;
            for (i = 0; i < 3; ++i) {
                if (gMenu1 == 0) {
                    if (lpScoreData->timeattack[round][zone][i].time >=
                        lpKeepWork->ta_time)
                        break;

                } else {
                    if (lpScoreData->special[round][i].time >=
                        lpKeepWork->ta_time)
                        break;
                }
            }
            if (i != 3) {

                gRankY = i;
                gNewRankX = gRankX = 0;
                gMenu2 = gNewMenu2 = 4;
                for (j = 2; j > i; --j) {

                    if (gMenu1 == 0) {
                        lpScoreData->timeattack[round][zone][j].time =
                            lpScoreData->timeattack[round][zone][j - 1].time;
                        sStrncpy(
                            lpScoreData->timeattack[round][zone][j].name,
                            lpScoreData->timeattack[round][zone][j - 1].name,
                            3);
                    } else {
                        lpScoreData->special[round][j].time =
                            lpScoreData->special[round][j - 1].time;
                        sStrncpy(lpScoreData->special[round][j].name,
                                 lpScoreData->special[round][j - 1].name, 3);
                    }
                }

                if (gMenu1 == 0) {
                    lpScoreData->timeattack[round][zone][i].time =
                        lpKeepWork->ta_time;
                    if (sStrncmp(lpScoreData->timeattack[round][zone][i].name,
                                 aNameDefault, 3) == 0) {
                        sStrncpy(lpScoreData->timeattack[round][zone][i].name,
                                 aNamePlayer, 3);
                    }

                    Time = 0;
                    for (k = 0; k < 7; ++k) {
                        for (l = 0; l < 3; ++l)
                            Time += lpScoreData->timeattack[k][l][0].time;
                    }
                    lpScoreData->total = Time;
                    Time < 108001;

                } else {
                    lpScoreData->special[round][i].time = lpKeepWork->ta_time;
                    if (sStrncmp(lpScoreData->special[round][i].name,
                                 aNameDefault, 3) == 0) {
                        sStrncpy(lpScoreData->special[round][i].name,
                                 aNamePlayer, 3);
                    }
                }
            }
        }
    }

    TAColorSet(0);
    TA_EACreate();
    clwin_init();
    clwin();
    /* Track 27, not the reference's 33.  CD96's disc carried the US
       soundtrack, where track 33 was the separate copy of the title music the
       Time Attack menu used.  This port plays the Japanese soundtrack from
       assets/Music, whose Time Attack music is TimeAttack.ogg at track 27
       (track 33 of that pack is DAGarden.ogg).  See docs/music_tracks.md. */
    CDPlay(27);
}

void DLL_meminit(char ***pBufTbl, void **pFuncTbl) {

    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    CDPlay = *pFuncTbl++;
    CDPause = *pFuncTbl++;
    ++pFuncTbl;
    ReadScoreIndx2 = *pFuncTbl++;
    WriteScoreData2 = *pFuncTbl++;
    SetScoreDate2 = *pFuncTbl++;
    ++pFuncTbl;

    sMemAlloc = *pFuncTbl++;
    sMemFree = *pFuncTbl++;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    sStrncpy = *pFuncTbl++;
    sStrncmp = *pFuncTbl++;
    sPrintf = *pFuncTbl++;
    sOutputDebugString = *pFuncTbl++;
    sOpenFile = *pFuncTbl++;
    sReadFile = *pFuncTbl++;
    sCloseFile = *pFuncTbl++;
    ++pFuncTbl;

    get_draw_context_module = *pFuncTbl++;
    FlipToScreen_module = *pFuncTbl++;
    ld_bitmap_4to8_module = *pFuncTbl++;
    ld_load_cmpfile_module = *pFuncTbl++;
    hmx_background_set_background_module = *pFuncTbl++;
    hmx_bitmap_create_module = *pFuncTbl++;
    hmx_bitmap_release_module = *pFuncTbl++;
    hmx_bitmap_get_scan0_module = *pFuncTbl++;
    hmx_bitmap_set_transparency_module = *pFuncTbl++;
    hmx_free_module = *pFuncTbl++;
    hmx_grid_create_module = *pFuncTbl++;
    hmx_grid_release_module = *pFuncTbl++;
    hmx_grid_base_module = *pFuncTbl++;
    hmx_grid_set_position_module = *pFuncTbl++;
    hmx_grid_set_view_module = *pFuncTbl++;
    hmx_grid_set_tile_module = *pFuncTbl++;
    ++pFuncTbl;
    hmx_renderer_context_add_module = *pFuncTbl++;
    hmx_renderer_context_clear_module = *pFuncTbl++;
    hmx_renderer_context_draw_module = *pFuncTbl++;
    hmx_sprite_base_module = *pFuncTbl++;
    hmx_sprite_set_position_module = *pFuncTbl++;
    hmx_sprite_set_bitmap_module = *pFuncTbl++;

    pmapwk = (Uint16 *)**pBufTbl++;
    lpcolorwk = (PALETTEENTRY *)**pBufTbl++;
    lpcolorwk2 = (PALETTEENTRY *)**pBufTbl++;
    lpcolorwk3 = (PALETTEENTRY *)**pBufTbl++;
    lpcolorwk4 = (PALETTEENTRY *)**pBufTbl++;
    lphscrollbuff = (int_union *)**pBufTbl++;
    lpFadeFlag = (Sint32 *)*pBufTbl++;
    lpKeepWork = (game_info *)*pBufTbl++;

    ++pBufTbl;
    ++pBufTbl;

    lpScoreData = (score_data *)*pBufTbl++;

    g_env_module = (hmx_environment *)**pBufTbl++;
    g_loader_module = (hmx_environment *)**pBufTbl++;
}

void DLL_memfree(void) {}
