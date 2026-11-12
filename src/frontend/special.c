#include <SDL3/SDL.h>
#include <string.h>
#include "dll.h"
#include "frontend_state.h"
#include "globals.h"
#include "graphics.h"
#include "round.h"
#include "sound.h"
#include "special.h"
#include "ta.h"
#include "title.h"

#define SPECIAL_SPRITE_COUNT 700

static int specialLoaded;
static int clearScreenLoaded;

static void Special_SelectStage(Uint32 stageMenuId) {
    int i;
    if (stageMenuId) {
        if (stageMenuId >= SPECIAL_STAGE_MENU_BASE) {
            gameInfo.stagenm = (Uint8)(stageMenuId - SPECIAL_STAGE_MENU_BASE);
        } else {
            gameInfo.stagenm = (Uint8)stageMenuId;
        }
        return;
    }

    for (i = 0; i < 7; ++i) {
        if (!(gameInfo.clrspflg_save & (1 << gameInfo.stagenm))) {
            break;
        }
        ++gameInfo.stagenm;
        if (gameInfo.stagenm > 6) {
            gameInfo.stagenm = 0;
        }
    }
}

static void Special_UpdateGroundTiles(void) {
    Uint16 hane1;
    Uint16 hane2;
    Uint16 dmg1;
    Uint16 dmg2;

    if (!Special_block_chg) {
        return;
    }
    Special_block_chg(&hane1, &hane2, &dmg1, &dmg2);
    Graphics_UpdateSpecialGroundTiles(hane1, hane2, dmg1, dmg2);
}

int Special_LoadDLL(const char *path) {
    memset(hscrollbuff, 0, sizeof(hscrollbuff));

    if (!DLL_Load(path)) {
        SDL_Log("Couldn't load special DLL: %s", path);
        return 0;
    }

    DLL_meminit(memoryTbl, functionTbl);
    fade_flag = 0;
    if (SetDebugFlag) {
        SetDebugFlag(Title_DebugFlag());
    }

    if (!Graphics_LoadSpecialStage(gameInfo.stagenm, gameInfo.sm_adr0, (bmp_info *)gameInfo.pSprBmp, SPECIAL_SPRITE_COUNT)) {
        SDL_Log("Couldn't load special graphics");
        return 0;
    }

    if (game_init) {
        game_init();
    }
    if (game) {
        game();
    }
    Special_UpdateGroundTiles();

    specialLoaded = 1;
    clearScreenLoaded = 0;
    return 1;
}

int Special_LoadStage(Uint32 stageMenuId) {
    Special_SelectStage(stageMenuId);
    gameInfo.SPEMode = 0;
    return Special_LoadDLL("bin/libspecial.so");
}

frontend_state Special_Run(SDL_Renderer *renderer) {
    Sint32 gameResult = 0;

    if (gameInfo.SPEMode == 1 && !clearScreenLoaded) {
        CDPause(2);
        if (!Graphics_LoadSpecialClearScreen()) {
            return FRONTEND_STATE_EXIT;
        }
        clearScreenLoaded = 1;
    }

    if (fade_flag && FadeProc) {
        if (FadeProc()) {
            fade_flag = 0;
        }
    } else if (game) {
        gameResult = game();
    }

    Special_UpdateGroundTiles();
    Graphics_DrawSpecial(renderer, &gameInfo);

    if (gameResult) {
        SDL_Log("special result %d", gameResult);
    }

    if (gameInfo.SPEMode != 0 && gameInfo.stagenm == 7) {
        Special_Unload();
        gameInfo.ta_flag = 0;
        if (!Title_LoadDLL("bin/libopening.so")) {
            return FRONTEND_STATE_EXIT;
        }
        return FRONTEND_STATE_TITLE;
    }

    if (gameInfo.SPEMode == 4 || gameInfo.SPEMode == 6) {
        Special_Unload();
        if (!gameInfo.ta_flag) {
            ++gameInfo.stagenm;
            if (gameInfo.stagenm > 6) {
                gameInfo.stagenm = 0;
            }
            scoreData.clrspflg_save = gameInfo.clrspflg_save;
            scoreData.stagenm = gameInfo.stagenm;
            if (!Round_LoadStageByMenu(0)) {
                return FRONTEND_STATE_EXIT;
            }
            return FRONTEND_STATE_ROUND;
        }
        if (!TimeAttack_Load(0)) {
            return FRONTEND_STATE_EXIT;
        }
        return FRONTEND_STATE_TIME_ATTACK;
    }

    return FRONTEND_STATE_SPECIAL;
}

void Special_Unload(void) {
    if (!specialLoaded) {
        return;
    }
    if (DLL_memfree) {
        DLL_memfree();
    }
    DLL_Unload();
    specialLoaded = 0;
    clearScreenLoaded = 0;
}
