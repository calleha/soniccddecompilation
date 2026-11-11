#include <SDL3/SDL.h>
#include <string.h>
#include "dll.h"
#include "frontend_state.h"
#include "globals.h"
#include "graphics.h"
#include "round.h"
#include "score.h"
#include "sound.h"
#include "special.h"
#include "ta.h"
#include "title.h"

static int timeAttackLoaded;

int TimeAttack_Load(Sint32 newSession) {
    Sint32 scoreIndex;

    memset(hscrollbuff, 0, sizeof(hscrollbuff));
    scoreIndex = ReadScoreIndx(0);
    if (scoreIndex >= 0) {
        ReadScoreData(scoreIndex, &scoreData, 0);
    }

    if (!DLL_Load("bin/libta.so")) {
        SDL_Log("Couldn't load time attack DLL");
        return 0;
    }

    DLL_meminit(memoryTbl, functionTbl);
    fade_flag = 0;
    gameInfo.ta_flag = 1;
    if (newSession) {
        gameInfo.ta_time = (Uint32)-2;
    }

    if (game_init) {
        game_init();
    }
    if (game) {
        game();
    }

    timeAttackLoaded = 1;
    return 1;
}

frontend_state TimeAttack_Run(SDL_Renderer *renderer) {
    Sint32 gameResult = 0;

    (void)renderer;
    if (fade_flag && FadeProc) {
        if (FadeProc()) {
            fade_flag = 0;
        }
    } else if (game) {
        gameResult = game();
    }

    if (!gameResult) {
        return FRONTEND_STATE_TIME_ATTACK;
    }

    SDL_Log("time attack result %d", gameResult);
    TimeAttack_Unload();
    if (gameResult == -1) {
        gameInfo.ta_flag = 0;
        if (!Title_LoadDLL("bin/libopening.so")) {
            return FRONTEND_STATE_EXIT;
        }
        return FRONTEND_STATE_TITLE;
    }

    gameInfo.play_start = 0;
    gameInfo.ta_flag = 1;
    if (gameResult >= ROUND_STAGE_R11A && gameResult <= ROUND_STAGE_R83D) {
        if (!Round_LoadStageByMenu((Uint32)gameResult)) {
            return FRONTEND_STATE_EXIT;
        }
        return FRONTEND_STATE_ROUND;
    }

    if (!Special_LoadStage((Uint32)gameResult)) {
        return FRONTEND_STATE_EXIT;
    }
    return FRONTEND_STATE_SPECIAL;
}

void TimeAttack_Unload(void) {
    if (!timeAttackLoaded) {
        return;
    }
    if (DLL_memfree) {
        DLL_memfree();
    }
    DLL_Unload();
    timeAttackLoaded = 0;
}
