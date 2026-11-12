#include <SDL3/SDL.h>
#include <string.h>

#include "dll.h"
#include "globals.h"
#include "graphics.h"
#include "planet.h"
#include "sound.h"
#include "title.h"

#define PLANET_SPRITE_COUNT 700

static int planetLoaded;

int Planet_Load(void) {
    memset(hscrollbuff, 0, sizeof(hscrollbuff));

    if (!DLL_Load("bin/libplanet.so")) {
        SDL_Log("Couldn't load planet DLL");
        return 0;
    }

    /* DLL_meminit both wires the tables and hands the module its keep work;
       the rotate map it builds (lpKeepWork->sm_adr0) is what the frontend
       needs for the rotating Little Planet. */
    DLL_meminit(memoryTbl, functionTbl);
    fade_flag = 0;
    if (SetDebugFlag) {
        SetDebugFlag(Title_DebugFlag());
    }

    if (!Graphics_LoadPlanet(gameInfo.sm_adr0, (bmp_info *)gameInfo.pSprBmp,
                             PLANET_SPRITE_COUNT)) {
        SDL_Log("Couldn't load planet graphics");
        return 0;
    }

    if (game_init) {
        game_init();
    }

    planetLoaded = 1;
    return 1;
}

frontend_state Planet_Run(SDL_Renderer *renderer) {
    Sint32 gameResult = 0;

    if (fade_flag && FadeProc) {
        if (FadeProc()) {
            fade_flag = 0;
        }
    } else if (game) {
        gameResult = game();
    }

    Graphics_DrawPlanet(renderer, &gameInfo);

    if (gameResult) {
        SDL_Log("planet result %d", gameResult);
        Planet_Unload();
        if (!Title_LoadDLL("bin/libopening.so")) {
            return FRONTEND_STATE_EXIT;
        }
        return FRONTEND_STATE_TITLE;
    }
    return FRONTEND_STATE_PLANET;
}

void Planet_Unload(void) {
    if (!planetLoaded) {
        return;
    }
    if (DLL_memfree) {
        DLL_memfree();
    }
    DLL_Unload();
    planetLoaded = 0;
}
