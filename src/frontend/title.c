#include <SDL3/SDL.h>
#include "dll.h"
#include "graphics.h"
#include "globals.h"
#include "round.h"
#include "score.h"
#include "sound.h"
#include "ta.h"
#include "title.h"
#include "../title/common/engine_dll.h"

enum {
    TITLE_RESULT_NEW_GAME = 1,
    TITLE_RESULT_CONTINUE = 2,
    TITLE_RESULT_TIME_ATTACK = 3,
    TITLE_RESULT_SAVE_DATA = 4,
    TITLE_RESULT_OPENING = 9,
    TITLE_RESULT_EXIT = 21,
};

static Uint32 fullscreen;
static Uint32 visualMode;
static Uint16 selectedStage;
static Uint16 userKeys[5];
static Uint32 firstTitle = 1;

engine_dll titleInfo = {
    .lpColorwk = colorwk,
    .lpbFullScreen = &fullscreen,
    .lpbVisualmode = &visualMode,
    .lpCrntScorData = &scoreData,
    .lpSelectedStage = &selectedStage,
    .ReadScoreData = (void *)ReadScoreData,
    .WriteScoreData = (void *)WriteScoreData,
    .OpenScoreData = (void *)OpenScoreData,
    .CloseScoreData = (void *)CloseScoreData,
    .CreateScoreData = (void *)CreateScoreData,
    .ReadScoreIndx = (void *)ReadScoreIndx,
    .WriteScoreIndx = (void *)WriteScoreIndx,
    .CDPlay = (void *)CDPlay,
    .CDPause = (void *)CDPause,
    .WaveRequest = (void *)WaveRequest,
    .lpUserKey = userKeys,
};

static void Title_PrepareInfo(void) {
    Sint32 scoreIndex = ReadScoreIndx(0);

    titleInfo.lpCrntScorData = &scoreData;
    titleInfo.selectIndx = scoreIndex >= 0 ? (Uint32)scoreIndex : 0;
    titleInfo.bFirstTitle = firstTitle;
}

int Title_LoadDLL(const char *path) {
    if (!DLL_LoadTitle(path)) {
        SDL_Log("Couldn't load title DLL");
        return 0;
    }

    Title_PrepareInfo();
    Title_DLLInit(&titleInfo, memoryTbl, functionTbl);
    firstTitle = 0;
    return 1;
}

static int Title_SwitchDLL(const char *path) {
    Title_Unload();
    if (!DLL_LoadTitle(path)) {
        SDL_Log("Couldn't load title DLL");
        return 0;
    }

    Title_PrepareInfo();
    Title_DLLInit(&titleInfo, memoryTbl, functionTbl);
    firstTitle = 0;
    return 1;
}

static frontend_state StartNewGame(void) {
    if (!NewScoreData()) {
        return FRONTEND_STATE_TITLE;
    }
    Title_Unload();
    gameInfo.play_start = 0;
    gameInfo.plflag = 0;
    gameInfo.clrspflg_save = 0;
    gameInfo.stagenm = 0;
    gameInfo.emie3end = 0;
    if (!Round_LoadStageByMenu(ROUND_STAGE_R11A)) {
        return FRONTEND_STATE_EXIT;
    }
    return FRONTEND_STATE_ROUND;
}

static frontend_state ContinueGame(void) {
    Sint32 scoreIndex = ReadScoreIndx(0);
    Uint32 stageMenuId;

    if (scoreIndex < 0 || !ReadScoreData(scoreIndex, &scoreData, 0) ||
        scoreData.saved == 0) {
        return FRONTEND_STATE_TITLE;
    }

    Title_Unload();
    gameInfo.play_start = 0;
    gameInfo.plflag = 0;
    gameInfo.clrspflg_save = scoreData.clrspflg_save;
    gameInfo.stagenm = scoreData.stagenm;
    gameInfo.emie3end = 0;

    if (scoreData.roundNo < 7) {
        stageMenuId = ROUND_STAGE_R11A + scoreData.roundNo * 10;
    } else {
        stageMenuId = ROUND_STAGE_R81A;
    }

    if (!Round_LoadStageByMenu(stageMenuId)) {
        return FRONTEND_STATE_EXIT;
    }
    return FRONTEND_STATE_ROUND;
}

static frontend_state LoadSaveManager(void) {
    if (!Title_SwitchDLL("bin/libsavedata.so")) {
        return FRONTEND_STATE_EXIT;
    }
    return FRONTEND_STATE_TITLE;
}

static frontend_state LoadTimeAttack(void) {
    Title_Unload();
    if (!TimeAttack_Load(1)) {
        return FRONTEND_STATE_EXIT;
    }
    return FRONTEND_STATE_TIME_ATTACK;
}

static frontend_state LoadOpening(void) {
    if (!Title_SwitchDLL("bin/libopening.so")) {
        return FRONTEND_STATE_EXIT;
    }
    return FRONTEND_STATE_TITLE;
}

frontend_state Title_Run(void) {
    if (Title_DLLMain) {
        Sint32 gameResult = Title_DLLMain();
        if (gameResult) {
            SDL_Log("opening result %d", gameResult);
        }
        switch (gameResult) {
        case 0:
            return FRONTEND_STATE_TITLE;
        case TITLE_RESULT_NEW_GAME:
            return StartNewGame();
        case TITLE_RESULT_CONTINUE:
            return ContinueGame();
        case TITLE_RESULT_TIME_ATTACK:
            return LoadTimeAttack();
        case TITLE_RESULT_SAVE_DATA:
            return LoadSaveManager();
        case TITLE_RESULT_OPENING:
            return LoadOpening();
        case TITLE_RESULT_EXIT:
            Title_Unload();
            return FRONTEND_STATE_EXIT;
        default:
            SDL_Log("unhandled opening result %d", gameResult);
            return FRONTEND_STATE_TITLE;
        }
    }
    return FRONTEND_STATE_EXIT;
}

void Title_Unload(void) {
    if (Title_DLLEnd) {
        Title_DLLEnd();
    }
    DLL_Unload();
}
