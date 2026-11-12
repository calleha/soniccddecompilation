#include <SDL3/SDL.h>
#include "attract.h"
#include "dll.h"
#include "ending.h"
#include "graphics.h"
#include "globals.h"
#include "movie_play.h"
#include "planet.h"
#include "round.h"
#include "score.h"
#include "sound.h"
#include "special.h"
#include "ta.h"
#include "thanks.h"
#include "title.h"
#include "../title/common/engine_dll.h"

/* The values the menu modules return from their DLLMain, exactly as
   WINEXE/WINMAIN.C's changeMenuState() and menuOrMovieLoop() read them.  A
   handful of them are shared: the opening hands out the menu entries, the
   visual mode hands out the movies, and the stage select hands out the chosen
   stage. */
enum {
    TITLE_RESULT_NEW_GAME = 1,
    TITLE_RESULT_CONTINUE = 2,
    TITLE_RESULT_TIME_ATTACK = 3,
    TITLE_RESULT_SAVE_DATA = 4,
    TITLE_RESULT_PLANET = 5,
    TITLE_RESULT_VISUAL_MODE = 6,
    TITLE_RESULT_ATTRACT = 7,
    TITLE_RESULT_OPENING_MOVIE = 8,
    TITLE_RESULT_OPENING = 9,
    TITLE_RESULT_GOOD_ENDING = 10, /* AVIGOOD module: movies 4 and 5 */
    TITLE_RESULT_BAD_ENDING = 11,  /* AVIBAD module: movies 2 and 3 */
    TITLE_RESULT_PENCIL_TEST = 12,
    TITLE_RESULT_SOUND_TEST = 13,
    TITLE_RESULT_STAGE_SELECT = 14,
    TITLE_RESULT_STAGE_SELECTED = 15,
    /* 16 and 17 are what the reference's AVIGOOD and AVIBAD movie modules
       answer when their movies are over ("show the ending message, type 1/2").
       The port plays the endings itself and hands over to the thanks screen
       from src/frontend/ending.c, so no menu module returns them here. */
    TITLE_RESULT_TA_SPECIAL_STAGE = 18,
    TITLE_RESULT_DEBUG_FLAG = 19,
    TITLE_RESULT_BEST_TIME = 20,
    TITLE_RESULT_EXIT = 21,
};

static Uint32 fullscreen;
static Uint32 visualMode;
static Uint16 selectedStage;
static Uint16 userKeys[5];
static Uint32 firstTitle = 1;
static Uint32 debugFlag;

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
    Graphics_TitleSnapshotUnfadedPalette();
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
    Graphics_TitleSnapshotUnfadedPalette();
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

/* The sub menus (save data, sound test, stage select, best times, visual mode)
   are all title style modules: swapping the loaded DLL is all it takes. */
static frontend_state LoadMenuDLL(const char *path) {
    if (!Title_SwitchDLL(path)) {
        return FRONTEND_STATE_EXIT;
    }
    return FRONTEND_STATE_TITLE;
}

/* The visual mode menu owns the movie entries, so it has to flag itself the way
   loadVisualmd() does in the reference: the flag decides whether a "back to
   the menu" answer goes to the visual mode or to the opening. */
frontend_state Title_VisualMode(void) {
    frontend_state state = LoadMenuDLL("bin/libvisualmd.so");
    visualMode = (state == FRONTEND_STATE_TITLE);
    return state;
}

static frontend_state LoadPlanet(void) {
    Title_Unload();
    visualMode = 0;
    if (!Planet_Load()) {
        return FRONTEND_STATE_EXIT;
    }
    return FRONTEND_STATE_PLANET;
}

static frontend_state LoadThanks(Uint32 messageType) {
    Title_Unload();
    visualMode = 0;
    /* The reference adds 128 in windowed mode; only the low bits are read. */
    gameInfo.EndingMesType = (Uint8)messageType;
    if (!Thanks_Load()) {
        return FRONTEND_STATE_EXIT;
    }
    return FRONTEND_STATE_THANKS;
}

static frontend_state PlayMovie(Sint32 id) {
    /* The movie modules answer 9 when they are done, which reloads the visual
       mode menu if the player came from there and the opening otherwise. */
    Title_Unload();
    MoviePlay_Begin(id, visualMode != 0);
    return FRONTEND_STATE_MOVIE;
}

static frontend_state PlayEnding(int good) {
    Title_Unload();
    visualMode = 0;
    Ending_Play(good);
    return FRONTEND_STATE_ENDING;
}

static frontend_state LoadSelectedStage(void) {
    Title_Unload();
    visualMode = 0;
    /* The reference remembers that a stage was picked from the stage select, so
       clearing it returns to the title instead of the next act. */
    Round_MarkStageSelectUsed();
    if (!Round_LoadStageByMenu(selectedStage)) {
        return FRONTEND_STATE_EXIT;
    }
    return FRONTEND_STATE_ROUND;
}

static frontend_state LoadTimeAttackSpecialStage(void) {
    Title_Unload();
    visualMode = 0;
    gameInfo.ta_flag = 1;
    /* ID_STAGE 477 is the dedicated time attack special stage (stagenm 7). */
    if (!Special_LoadStage(477)) {
        return FRONTEND_STATE_EXIT;
    }
    return FRONTEND_STATE_SPECIAL;
}

static frontend_state ToggleDebugFlag(void) {
    debugFlag ^= 1;
    SDL_Log("debug flag %u", debugFlag);
    return LoadOpening();
}

Uint32 Title_DebugFlag(void) { return debugFlag; }

/* Restarts the title sequence from the beginning - used after the attract mode
   (opening movie / stage demos) is over. */
frontend_state Title_Restart(void) {
    visualMode = 0;
    return LoadOpening();
}

frontend_state Title_Run(void) {
    if (Title_DLLMain) {
        Sint32 gameResult = Title_DLLMain();
        if (gameResult) {
            SDL_Log("menu result %d", gameResult);
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
        case TITLE_RESULT_PLANET:
            return LoadPlanet();
        case TITLE_RESULT_VISUAL_MODE:
            return Title_VisualMode();
        case TITLE_RESULT_ATTRACT:
            Attract_Begin();
            return FRONTEND_STATE_ATTRACT;
        case TITLE_RESULT_OPENING_MOVIE:
            return PlayMovie(0);
        case TITLE_RESULT_OPENING:
            return LoadOpening();
        case TITLE_RESULT_GOOD_ENDING:
            return PlayEnding(1);
        case TITLE_RESULT_BAD_ENDING:
            return PlayEnding(0);
        case TITLE_RESULT_PENCIL_TEST:
            return PlayMovie(1);
        case TITLE_RESULT_SOUND_TEST:
            return LoadMenuDLL("bin/libsoundtst.so");
        case TITLE_RESULT_STAGE_SELECT:
            return LoadMenuDLL("bin/libstagetst.so");
        case TITLE_RESULT_STAGE_SELECTED:
            return LoadSelectedStage();
        case TITLE_RESULT_TA_SPECIAL_STAGE:
            return LoadTimeAttackSpecialStage();
        case TITLE_RESULT_DEBUG_FLAG:
            return ToggleDebugFlag();
        case TITLE_RESULT_BEST_TIME:
            return LoadMenuDLL("bin/libbesttime.so");
        case TITLE_RESULT_EXIT:
            Title_Unload();
            return FRONTEND_STATE_EXIT;
        default:
            SDL_Log("unhandled menu result %d", gameResult);
            return FRONTEND_STATE_TITLE;
        }
    }
    return FRONTEND_STATE_EXIT;
}

void Title_Unload(void) {
    /* DLL_Unload() runs the title DLL's DLLEnd while it is still mapped. */
    DLL_Unload();
}
