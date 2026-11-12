#include "../../types.h"
#include "../common/score_data_types.h"
#include "../common/engine_dll.h"
#include "../common/hmx_types.h"
#include "opnentry.h"
#include "../common/hmx_oeeactl.h"
#include "../common/palt.h"
#include "opndo.h"

extern PALETTEENTRY tblPal4[];
extern PALETTEENTRY tblPal3[];
extern PALETTEENTRY tblPal2[];
extern PALETTEENTRY tblPal1[];
hmx_environment *g_env_module = 0;
hmx_environment *g_loader_module = 0;
void (*sOutputDebugString)(char *) = 0;
void (*sPrintf)(char *, char *) = 0;
void (*sCloseFile)(Sint32) = 0;
Sint32 (*sReadFile)(Sint32, void *, Sint32) = 0;
Sint32 (*sOpenFile)(char *) = 0;
Sint32 (*sGetFileSize)(Sint32) = 0;
void (*sMemCpy)(void *, void *, Sint32) = 0;
void (*sMemFree)(void *) = 0;
void *(*sMemAlloc)(Sint32) = 0;
void (*hmx_ddagrid_set_tile_module)(hmx_ddagrid *, Sint32, Sint32, hmx_bitmap *,
                                    Sint32) = 0;
void (*hmx_ddagrid_set_scan_module)(hmx_ddagrid *, Sint32, Sint32, Sint32,
                                    Sint32, Sint32) = 0;
void (*hmx_ddagrid_release_module)(hmx_environment *, hmx_ddagrid *) = 0;
hmx_ddagrid *(*hmx_ddagrid_create_module)(hmx_environment *, Sint32, Sint32,
                                          Sint32, Sint32, Sint32, Sint32) = 0;
void (*hmx_sprite_set_flags_module)(hmx_sprite *, Sint32) = 0;
void (*hmx_sprite_set_bitmap_module)(hmx_sprite *, hmx_bitmap *) = 0;
void (*hmx_sprite_set_position_module)(hmx_sprite *, Sint32, Sint32) = 0;
hmx_renderer_base *(*hmx_sprite_base_module)(hmx_sprite *) = 0;
void (*hmx_renderer_context_draw_module)(hmx_renderer_context *,
                                         hmx_surface *) = 0;
void (*hmx_renderer_context_clear_module)(hmx_renderer_context *) = 0;
void (*hmx_renderer_context_add_module)(hmx_renderer_context *, Sint32,
                                        hmx_renderer_base *) = 0;
void (*hmx_grid_set_horz_offsets_module)(hmx_grid *, Sint32 *) = 0;
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
Uint32 (*WriteIndx)(Sint32, Uint32) = 0;
Sint32 (*ReadIndx)(Uint32) = 0;
Uint32 (*CreateScore)(void) = 0;
Uint32 (*CloseScore)(Uint32) = 0;
Uint32 (*OpenScore)(Sint32) = 0;
Uint32 (*WriteScore)(Sint32, char *, Uint32) = 0;
Uint32 (*ReadScore)(Sint32, char *, Uint32) = 0;
void (*WaveRequest)(Sint16) = 0;
void (*CDPause)(void) = 0;
void (*CDPlay)(Sint16) = 0;
draw_context *s_ctx = 0;
Uint16 *lpUserKey = 0;
Uint32 *lpbFullScreen = 0;
Uint32 bFirstTitle = 0;
Uint16 swData2 = 0;
Uint16 swData1 = 0;
Uint32 selectIndx = 0;
score_data crntScorData = {0};
PALETTEENTRY *lpColorwk = 0;
UintPtr hWnd = 0;
UintPtr hSurf = 0;
Sint32 nTimerCunt = 0;
Uint16 nSequenceNum = 0;
dlink_export ExportedFunctions = {(void (*)(void)) & DLLInit,
                                  (void (*)(void)) & DLLMain,
                                  (void (*)(char ***, void **)) & DLLEnd,
                                  (void (*)(void)) & SWdataSet,
                                  0,
                                  0,
                                  0,
                                  0,
                                  0,
                                  0,
                                  0,
                                  0};

void OESetSequenceNum(Uint16 num) {
    nSequenceNum = num;
    nTimerCunt = -1;
}

void DLLInit(engine_dll *lpDllIn, char ***pBufTbl, void **pFuncTbl) {

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

    sMemAlloc = *pFuncTbl++;
    sMemFree = *pFuncTbl++;
    ++pFuncTbl;
    sMemCpy = *pFuncTbl++;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    sPrintf = *pFuncTbl++;
    sOutputDebugString = *pFuncTbl++;
    sOpenFile = *pFuncTbl++;
    sReadFile = *pFuncTbl++;
    sCloseFile = *pFuncTbl++;
    sGetFileSize = *pFuncTbl++;

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
    hmx_grid_set_horz_offsets_module = *pFuncTbl++;
    hmx_renderer_context_add_module = *pFuncTbl++;
    hmx_renderer_context_clear_module = *pFuncTbl++;
    hmx_renderer_context_draw_module = *pFuncTbl++;
    hmx_sprite_base_module = *pFuncTbl++;
    hmx_sprite_set_position_module = *pFuncTbl++;
    hmx_sprite_set_bitmap_module = *pFuncTbl++;
    hmx_sprite_set_flags_module = *pFuncTbl++;

    hmx_ddagrid_create_module = *pFuncTbl++;
    hmx_ddagrid_release_module = *pFuncTbl++;
    hmx_ddagrid_set_scan_module = *pFuncTbl++;
    hmx_ddagrid_set_tile_module = *pFuncTbl++;

    ++pBufTbl;
    ++pBufTbl;
    ++pBufTbl;
    ++pBufTbl;
    ++pBufTbl;
    ++pBufTbl;
    ++pBufTbl;
    ++pBufTbl;
    ++pBufTbl;
    ++pBufTbl;
    ++pBufTbl;

    g_env_module = (hmx_environment *)**pBufTbl++;
    g_loader_module = (hmx_environment *)**pBufTbl++;

    hSurf = lpDllIn->hSurf;
    lpColorwk = lpDllIn->lpColorwk;
    lpbFullScreen = lpDllIn->lpbFullScreen;

    sMemCpy(&crntScorData, lpDllIn->lpCrntScorData, sizeof(crntScorData));
    selectIndx = lpDllIn->selectIndx;

    CDPlay = lpDllIn->CDPlay;
    CDPause = lpDllIn->CDPause;
    WaveRequest = lpDllIn->WaveRequest;

    ReadScore = lpDllIn->ReadScoreData;
    WriteScore = lpDllIn->WriteScoreData;
    OpenScore = lpDllIn->OpenScoreData;
    CloseScore = lpDllIn->CloseScoreData;
    CreateScore = lpDllIn->CreateScoreData;
    ReadIndx = lpDllIn->ReadScoreIndx;
    WriteIndx = lpDllIn->WriteScoreIndx;
    bFirstTitle = lpDllIn->bFirstTitle;
    lpUserKey = lpDllIn->lpUserKey;

    s_ctx = get_draw_context_module();

    OEClrset(tblPal1, 0);
    OEClrset(tblPal2, 1);
    OEClrset(tblPal3, 2);
    OEClrset(tblPal4, 3);

    OECreateEA();
    OEFlashSpritesCreate();
    nSequenceNum = 1;
    nTimerCunt = 0;
}

void DLLEnd(void) { OEDeleteEA(); }

void SWdataSet(ushort_union sw1, ushort_union sw2) {
    swData1 = sw1.w;
    swData2 = sw2.w;
}

Sint32 DLLMain(void) {
    Sint32 ret = 0;

    switch (nSequenceNum) {

    case 1:
        OEOpenSta();
        break;

    case 2:
        OESegaLogo();
        break;

    case 3:
        OEDark();
        break;

    case 5:
        OESonic();
        break;

    case 6:
        OETitleSta();
        break;

    case 7:
        OETitle();
        break;

    case 8:
        OEFadeOut();
        break;

    case 9:
        OEFadeIn();
        break;

    case 999:
        ret = OEEnd();
        break;
    }
    if (nTimerCunt == 0x7FFFFFFF)
        nTimerCunt = 5184000;
    else
        ++nTimerCunt;

    OEDraw();

    return ret;
}
