#include <SDL3/SDL.h>
#include <string.h>

#include "dll.h"
#include "globals.h"
#include "graphics.h"
#include "sound.h"
#include "thanks.h"
#include "title.h"

#define THANKS_SPRITE_COUNT 700

static int thanksLoaded;

int Thanks_Load(void) {
    memset(hscrollbuff, 0, sizeof(hscrollbuff));

    SDL_Log("thanks: message type %u", (unsigned)gameInfo.EndingMesType);

    if (!DLL_Load("bin/libthanks.so")) {
        SDL_Log("Couldn't load thanks DLL");
        return 0;
    }

    DLL_meminit(memoryTbl, functionTbl);
    fade_flag = 0;
    if (SetDebugFlag) {
        SetDebugFlag(Title_DebugFlag());
    }

    if (!Graphics_LoadThanks((bmp_info *)gameInfo.pSprBmp, THANKS_SPRITE_COUNT)) {
        SDL_Log("Couldn't load thanks graphics");
        return 0;
    }

    if (game_init) {
        game_init();
    }

    thanksLoaded = 1;
    return 1;
}

frontend_state Thanks_Run(SDL_Renderer *renderer) {
    Sint32 gameResult = 0;

    if (fade_flag && FadeProc) {
        if (FadeProc()) {
            fade_flag = 0;
        }
    } else if (game) {
        gameResult = game();
    }

    Graphics_DrawThanks(renderer, &gameInfo);

    if (gameResult) {
        SDL_Log("thanks result %d", gameResult);
        Thanks_Unload();
        if (!Title_LoadDLL("bin/libopening.so")) {
            return FRONTEND_STATE_EXIT;
        }
        return FRONTEND_STATE_TITLE;
    }
    return FRONTEND_STATE_THANKS;
}

void Thanks_Unload(void) {
    if (!thanksLoaded) {
        return;
    }
    if (DLL_memfree) {
        DLL_memfree();
    }
    DLL_Unload();
    thanksLoaded = 0;
}
