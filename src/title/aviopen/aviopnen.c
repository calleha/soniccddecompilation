#include "../../types.h"
#include "../common/score_data_types.h"
#include "../common/engine_dll.h"
#include "../common/hmx_types.h"
#include "aviopnen.h"
#include "aviopndo.h"

hmx_environment *g_env_module;
hmx_environment *g_loader_module;
#if defined(AVIOPEN)
/* The reference sizes this for "TITLE\\BMP\\OPNAVI\\OPWP.BMP"; the port's
   "../" prefix needs one byte more for the terminator. */
char fileName[29] = "../TITLE/BMP/OPNAVI/OPWP.BMP";
#endif
#if defined(AVIPEN)
char fileName[29] = "./TITLE/BMP/OPNAVI/PENBG.BMP";
#endif
Sint32 nTimerCunt;
Uint16 nSequenceNum;
Uint16 wMCIDeviceID[1];
draw_context *s_ctx;
draw_context *(*get_draw_context_module)(void);
Sint32 (*sMovieStat)(void);
Sint32 (*sMovieStop)(void);
Sint32 (*sMoviePlay)(Sint32);
void (*hmx_ddagrid_set_tile_module)(hmx_ddagrid *, Sint32, Sint32, hmx_bitmap *,
                                    Sint32);
void (*hmx_ddagrid_set_scan_module)(hmx_ddagrid *, Sint32, Sint32, Sint32,
                                    Sint32, Sint32);
void (*hmx_ddagrid_release_module)(hmx_environment *, hmx_ddagrid *);
hmx_ddagrid *(*hmx_ddagrid_create_module)(hmx_environment *, Sint32, Sint32,
                                          Sint32, Sint32, Sint32, Sint32);
void (*hmx_sprite_set_flags_module)(hmx_sprite *, Sint32);
void (*hmx_sprite_set_bitmap_module)(hmx_sprite *, hmx_bitmap *);
void (*hmx_sprite_set_position_module)(hmx_sprite *, Sint32, Sint32);
hmx_renderer_base *(*hmx_sprite_base_module)(hmx_sprite *);
void (*hmx_renderer_context_draw_module)(hmx_renderer_context *, hmx_surface *);
void (*hmx_renderer_context_clear_module)(hmx_renderer_context *);
void (*hmx_renderer_context_add_module)(hmx_renderer_context *, Sint32,
                                        hmx_renderer_base *);
void (*hmx_grid_set_horz_offsets_module)(hmx_grid *, Sint32 *);
void (*hmx_grid_set_tile_module)(hmx_grid *, Sint32, Sint32, hmx_bitmap *,
                                 Sint32);
void (*hmx_grid_set_view_module)(hmx_grid *, Sint32, Sint32, Sint32, Sint32);
void (*hmx_grid_set_position_module)(hmx_grid *, Sint32, Sint32);
hmx_renderer_base *(*hmx_grid_base_module)(hmx_grid *);
void (*hmx_grid_release_module)(hmx_environment *, hmx_grid *);
hmx_grid *(*hmx_grid_create_module)(hmx_environment *, Sint32, Sint32, Sint32,
                                    Sint32);
void (*hmx_free_module)(hmx_environment *, void *);
void (*hmx_bitmap_set_transparency_module)(hmx_bitmap *, Sint32);
void *(*hmx_bitmap_get_scan0_module)(hmx_bitmap *);
void (*hmx_bitmap_release_module)(hmx_environment *, hmx_bitmap *);
hmx_bitmap *(*hmx_bitmap_create_module)(hmx_environment *, Sint32, Sint32);
void (*hmx_background_set_background_module)(hmx_background *, Sint32);
void *(*ld_load_cmpfile_module)(hmx_environment *, char *);
void (*ld_bitmap_4to8_module)(void *, void *, Sint32, Sint32, Sint32, Sint32,
                              Sint32);
void (*FlipToScreen_module)(void);
Sint32 (*sGetFileSize)(Sint32);
void (*sCloseFile)(Sint32);
Sint32 (*sReadFile)(Sint32, void *, Sint32);
Sint32 (*sOpenFile)(char *);
void (*sOutputDebugString)(char *);
void (*sPrintf)(char *, const char *, ...);
void (*sMemCpy)(void *, void *, Sint32);
void (*sMemFree)(void *);
void *(*sMemAlloc)(Sint32);
Uint16 swData2;
Uint16 swData1;
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
Uint32 (*WriteIndx)(Sint32, Uint32);
Sint32 (*ReadIndx)(Uint32);
Uint32 (*CreateScore)(void);
Uint32 (*CloseScore)(Uint32);
Uint32 (*OpenScore)(Sint32);
Uint32 (*WriteScore)(Sint32, char *, Uint32);
Uint32 (*ReadScore)(Sint32, char *, Uint32);
void (*WaveRequest)(Sint16);
void (*CDPause)(void);
void (*CDPlay)(Sint16);
Uint32 *lpbFullScreen;
UintPtr hWnd;

void OESetSequenceNum(Uint16 num) {
    nSequenceNum = num;
    nTimerCunt = -1;
}

void DLLInit(engine_dll *lpDllIn, char ***pBufTbl, void **pFuncTbl) {
    Sint32 n;

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

    ++pFuncTbl;
    sMoviePlay = *pFuncTbl++;
    sMovieStop = *pFuncTbl++;
    sMovieStat = *pFuncTbl++;

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

    s_ctx = get_draw_context_module();

    ReadDIB(fileName);
    nSequenceNum = 1;
    nTimerCunt = 0;

    for (n = 0; n <= 0; ++n) {
        wMCIDeviceID[n] = 65535;
    }
}

void DLLEnd(void) {
    FreeDIB();
    AVIEnd();
}

void DLLPaint(Uint32 hdc) { AVIPaint(hdc); }

Sint32 DLLNotify(Uint32 WPARAM, long LPARAM) { return 0; }

void DLLAVISizeChange(void) {}

void DLLAVIRealize(void) {}

void AVIPause(void) {}

void AVIResume(void) {}

void SWdataSet(ushort_union sw1, ushort_union sw2) {
    swData1 = sw1.w;
    swData2 = sw2.w;
}

Sint32 DLLMain(void) {
    Sint32 ret = 0;

    switch (nSequenceNum) {

    case 1:
        AVIOpeningMove();
        break;
    }
    if (nTimerCunt == 0x7FFFFFFF)
        nTimerCunt = 5184000;
    else
        ++nTimerCunt;

#if defined(AVIOPEN)
    if (swData1 & 240) {
        ret = 9;
    }
#endif

    return ret;
}
