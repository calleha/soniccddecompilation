#include <SDL3/SDL.h>
#include <string.h>
#include "dll.h"
#include "frontend_state.h"
#include "globals.h"
#include "graphics.h"
#include "round.h"
#include "title.h"
#include "warp.h"

#define WARP_SPRITE_COUNT 700

static int warpLoaded;

int Warp_LoadDLL(const char *path) {
    memset(hscrollbuff, 0, sizeof(hscrollbuff));

    if (!DLL_Load(path)) {
        SDL_Log("Couldn't load warp DLL: %s", path);
        return 0;
    }

    DLL_meminit(memoryTbl, functionTbl);
    fade_flag = 0;
    if (SetDebugFlag) {
        SetDebugFlag(Title_DebugFlag());
    }

    if (!Graphics_LoadWarp((bmp_info *)gameInfo.pSprBmp, WARP_SPRITE_COUNT)) {
        SDL_Log("Couldn't load warp graphics");
        return 0;
    }

    if (game_init) {
        game_init();
    }

    warpLoaded = 1;
    return 1;
}

int Warp_Load(void) {
    return Warp_LoadDLL("bin/libwarp.so");
}

frontend_state Warp_Run(SDL_Renderer *renderer) {
    Sint32 gameResult = 0;
    Sint32 vscroll = 0;
    Sint32 scraHPosiw = 0;
    Sint32 scrbHPosiw = 0;

    if (fade_flag && FadeProc) {
        if (FadeProc()) {
            fade_flag = 0;
        }
    } else if (game) {
        gameInfo.GamePass = 0;
        gameResult = game();
    }

    if (Get_vscroll) {
        vscroll = Get_vscroll();
    }
    if (Get_scra_h_posiw) {
        scraHPosiw = Get_scra_h_posiw();
    }
    if (Get_scrb_h_posiw) {
        scrbHPosiw = Get_scrb_h_posiw();
    }

    Graphics_DrawWarp(renderer, scraHPosiw, scrbHPosiw, vscroll);
    if (gameResult) {
        SDL_Log("warp result %d", gameResult);
        Warp_Unload();
        if (!Round_LoadStageByMenu(0)) {
            return FRONTEND_STATE_EXIT;
        }
        return FRONTEND_STATE_ROUND;
    }
    return FRONTEND_STATE_WARP;
}

void Warp_Unload(void) {
    if (!warpLoaded) {
        return;
    }
    if (DLL_memfree) {
        DLL_memfree();
    }
    DLL_Unload();
    warpLoaded = 0;
}
