#include "file.h"
#include "globals.h"
#include "graphics.h"
#include "hmx.h"
#include "movie.h"
#include "score.h"
#include "sound.h"
#include "utilities.h"

static SDL_Renderer *renderer;
static SDL_Window *window;

static Uint16 *pMapwk = mapwk;
static PALETTEENTRY *pColorwk = colorwk;
static PALETTEENTRY *pColorwk2 = colorwk2;
static PALETTEENTRY *pColorwk3 = colorwk3;
static PALETTEENTRY *pColorwk4 = colorwk4;
static int_union *pHscrollbuff = hscrollbuff;

game_info gameInfo;
score_data scoreData;

static void *hmxEnvironment;
static void *hmxLoaderEnvironment;

char **memoryTbl[13] = {
    (char **)&pMapwk,
    (char **)&pColorwk,
    (char **)&pColorwk2,
    (char **)&pColorwk3,
    (char **)&pColorwk4,
    (char **)&pHscrollbuff,
    (char **)&fade_flag,
    (char **)&gameInfo,
    (char **)&renderer,
    (char **)&window,
    (char **)&scoreData,
    (char **)&hmxEnvironment,
    (char **)&hmxLoaderEnvironment,
};

void *functionTbl[64] = {
    (void *)SetGrid,
    (void *)EAsprset,
    (void *)ClrSpriteDebug,
    (void *)WaveRequest,
    (void *)CDPlay,
    (void *)CDPause,
    (void *)ChangeTileBmp,
    (void *)ReadScoreIndx,
    (void *)WriteScoreData,
    (void *)SetScoreDate,
    (void *)WaveAllStop,
    (void *)sMemAlloc,
    (void *)sMemFree,
    (void *)sMemSet,
    (void *)sMemCpy,
    (void *)sMemCmp,
    (void *)sRandom,
    (void *)sStrcpy,
    (void *)sStrncpy,
    (void *)sStrncmp,
    (void *)sPrintf,
    (void *)sOutputDebugString,
    (void *)sOpenFile,
    (void *)sReadFile,
    (void *)sCloseFile,
    (void *)sGetFileSize,
    (void *)get_draw_context,
    (void *)FlipToScreen,
    (void *)ld_bitmap_4to8,
    (void *)ld_load_cmpfile,
    (void *)hmx_background_set_background,
    (void *)hmx_bitmap_create,
    (void *)hmx_bitmap_release,
    (void *)hmx_bitmap_get_scan0,
    (void *)hmx_bitmap_set_transparency,
    (void *)hmx_free,
    (void *)hmx_grid_create,
    (void *)hmx_grid_release,
    (void *)hmx_grid_base,
    (void *)hmx_grid_set_position,
    (void *)hmx_grid_set_view,
    (void *)hmx_grid_set_tile,
    (void *)hmx_grid_set_horz_offsets,
    (void *)hmx_renderer_context_add,
    (void *)hmx_renderer_context_clear,
    (void *)hmx_renderer_context_draw,
    (void *)hmx_sprite_base,
    (void *)hmx_sprite_set_position,
    (void *)hmx_sprite_set_bitmap,
    (void *)hmx_sprite_set_flags,
    (void *)hmx_ddagrid_create,
    (void *)hmx_ddagrid_release,
    (void *)hmx_ddagrid_set_scan,
    (void *)hmx_ddagrid_set_tile,
    /* The movie DLLs (TITLE/AVIOPEN, TITLE/AVIGOOD) skip one slot and then take
       the three movie hooks, so the order here is part of the ABI. */
    0,
    (void *)Movie_Play,
    (void *)Movie_Stop,
    (void *)Movie_Stat,
};

void Globals_Init(SDL_Window *w, SDL_Renderer *r) {
    window = w;
    hmxEnvironment = hmx_get_environment();
    renderer = r;
    hmxLoaderEnvironment = hmx_get_loader_environment();
    gameInfo.TimeWarp = 1;
}
