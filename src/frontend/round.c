#include <SDL3/SDL.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "dll.h"
#include "ending.h"
#include "globals.h"
#include "graphics.h"
#include "round.h"
#include "score.h"
#include "sound.h"
#include "special.h"
#include "ta.h"
#include "title.h"
#include "warp.h"

#define ROUND_STAGE_MENU_BASE 400
#define ROUND_STAGE_ID_LEN 3
#define ROUND_SPRITE_COUNT 700

static const Uint16 tileChangeMask[8] = {
    0,
    0x0B00,
    0x8000,
    0xFF30,
    0xFF30,
    0,
    0xBF30,
    0,
};

static int stageIdInitialized;

/* Whether the stage that is loaded ran in a good future - the reference's
   DAT_0042570c, set from the "?3C" stage id when a stage is loaded. */
static int roundStageIsGoodFuture;

static void Round_ReadStageString(char *str, size_t strSize) {
    Uint8 round;
    Uint8 zone;

    if (strSize < ROUND_STAGE_ID_LEN + 1) {
        return;
    }

    round = (Uint8)(gameInfo.stageno.b.h + 1);
    if (round > 1) {
        ++round;
    }
    zone = (Uint8)(gameInfo.stageno.b.l + 1);

    str[0] = (char)('0' + round);
    str[1] = (char)('0' + zone);
    switch (gameInfo.time_flag & 0x7F) {
    case 0:
        str[2] = 'B';
        break;
    case 1:
        str[2] = 'A';
        break;
    case 2:
        str[2] = gameInfo.generate_flag ? 'C' : 'D';
        break;
    default:
        str[2] = 'A';
        break;
    }
    str[3] = '\0';
}

static void Round_SetCurrentStage(void) {
    if (gameInfo.time_flag & 0x80) {
        gameInfo.time_flag &= 0x7F;
    } else if (gameInfo.stageno.b.l == 0) {
        gameInfo.generate_flag = 0;
        gameInfo.gf_flag = 0;
        gameInfo.projector_flag = 0;
    }
    stageIdInitialized = 1;
}

static void Round_SetStageByMenu(Uint32 stageMenuId) {
    int round;
    int level;
    int zone;

    if (stageMenuId == 0) {
        Round_SetCurrentStage();
        return;
    }

    round = (int)(stageMenuId - ROUND_STAGE_MENU_BASE) / 10;
    level = (int)(stageMenuId - ROUND_STAGE_MENU_BASE) % 10;
    zone = level / 4;

    gameInfo.stageno.b.h = (Sint8)round;
    gameInfo.stageno.b.l = (Sint8)zone;

    if (level >= 8 || level % 4 >= 2) {
        gameInfo.time_flag = 2;
    } else if (level % 4 == 1) {
        gameInfo.time_flag = 0;
    } else {
        gameInfo.time_flag = 1;
    }

    gameInfo.generate_flag = 0;
    gameInfo.gf_flag = 0;
    if (level == 8 || level % 4 == 2) {
        gameInfo.generate_flag = 1;
    } else if (level == 9 || level % 4 == 3) {
        gameInfo.generate_flag = 0;
    }
    stageIdInitialized = 1;
}

static int Round_SetStageByName(const char *stageId) {
    int round;
    int zone;
    char period;

    if (!stageId || strlen(stageId) != ROUND_STAGE_ID_LEN) {
        return 0;
    }
    if (!isdigit((unsigned char)stageId[0]) || !isdigit((unsigned char)stageId[1])) {
        return 0;
    }

    round = stageId[0] - '0';
    zone = stageId[1] - '0';
    period = (char)toupper((unsigned char)stageId[2]);
    if (round == 2 || round < 1 || round > 8 || zone < 1 || zone > 3) {
        return 0;
    }

    gameInfo.stageno.b.h = (Sint8)(round == 1 ? 0 : round - 2);
    gameInfo.stageno.b.l = (Sint8)(zone - 1);
    gameInfo.generate_flag = 0;
    gameInfo.gf_flag = 0;

    switch (period) {
    case 'A':
        gameInfo.time_flag = 1;
        break;
    case 'B':
        gameInfo.time_flag = 0;
        break;
    case 'C':
        gameInfo.time_flag = 2;
        gameInfo.generate_flag = 1;
        break;
    case 'D':
        gameInfo.time_flag = 2;
        break;
    default:
        return 0;
    }

    stageIdInitialized = 1;
    return 1;
}

static void Round_NormalizeStageId(char *dst, size_t dstSize, const char *src, int lowerCase) {
    size_t i;
    if (dstSize == 0) {
        return;
    }
    for (i = 0; i + 1 < dstSize && src[i]; ++i) {
        dst[i] = lowerCase ? (char)tolower((unsigned char)src[i]) : (char)toupper((unsigned char)src[i]);
    }
    dst[i] = '\0';
}

static int Round_UsesTileChange(void) {
    int stage;
    Uint8 round;

    round = (Uint8)gameInfo.stageno.b.h;
    if (round >= SDL_arraysize(tileChangeMask)) {
        return 0;
    }

    stage = gameInfo.stageno.b.l * 4;
    if (gameInfo.time_flag == 0) {
        ++stage;
    } else if (gameInfo.time_flag == 2) {
        stage += 2;
        if (gameInfo.generate_flag == 0) {
            ++stage;
        }
    }

    return (tileChangeMask[round] & (0x8000 >> stage)) != 0;
}

static int Round_LoadGraphics(void) {
    char stageId[ROUND_STAGE_ID_LEN + 1] = "";
    char tilePath[64];
    char spritePath[64];
    char tileChangePath[64];
    bmp_info *spriteInfo;

    Graphics_BeginRound();
    if (GetRoundStr) {
        GetRoundStr(gameInfo.stageno.w, gameInfo.time_flag, stageId);
    } else {
        Round_ReadStageString(stageId, sizeof(stageId));
    }

    snprintf(tilePath, sizeof(tilePath), "../R%c/%s/TCMP%s.CM_", stageId[0], stageId, stageId);
    if (!Graphics_LoadTiles(tilePath)) {
        SDL_Log("Couldn't load round tiles: %s", tilePath);
        return 0;
    }

    if (Round_UsesTileChange()) {
        snprintf(tileChangePath, sizeof(tileChangePath), "../R%c/%s/TCHG%s.CM_", stageId[0], stageId, stageId);
        if (!Graphics_LoadChangeTiles(tileChangePath)) {
            SDL_Log("Couldn't load round tile changes: %s", tileChangePath);
            return 0;
        }
    }

    spriteInfo = (bmp_info *)gameInfo.pSprBmp;
    if (!spriteInfo) {
        SDL_Log("Round DLL did not provide a sprite info table");
        return 0;
    }
    snprintf(spritePath, sizeof(spritePath), "../R%c/%s/SCMP%s.CM_", stageId[0], stageId, stageId);
    if (!Graphics_LoadSprites(spritePath, spriteInfo, ROUND_SPRITE_COUNT)) {
        SDL_Log("Couldn't load round sprites: %s", spritePath);
        return 0;
    }

    return 1;
}

static int Round_LoadStagePath(const char *path) {
    char stageId[ROUND_STAGE_ID_LEN + 1];

    memset(hscrollbuff, 0, sizeof(hscrollbuff));

    /* The reference notes the good future here, before the DLL is loaded, and
       uses it when the next zone's act 1 starts. */
    stageId[0] = '\0';
    Round_ReadStageString(stageId, sizeof(stageId));
    roundStageIsGoodFuture = (stageId[1] == '3' && stageId[2] == 'C');

    if (!DLL_Load(path)) {
        SDL_Log("Couldn't load round DLL: %s", path);
        return 0;
    }

    DLL_meminit(memoryTbl, functionTbl);
    fade_flag = 0;
    if (SetDebugFlag) {
        SetDebugFlag(0);
    }

    if (!Round_LoadGraphics()) {
        return 0;
    }

    if (game_init) {
        game_init();
    }

    return 1;
}

int Round_LoadDLL(const char *path) {
    if (!stageIdInitialized) {
        Round_SetStageByMenu(ROUND_STAGE_R11A);
    }
    return Round_LoadStagePath(path);
}

int Round_LoadStageByMenu(Uint32 stageMenuId) {
    char stageId[ROUND_STAGE_ID_LEN + 1];
    char dllName[ROUND_STAGE_ID_LEN + 1];
    char dllPath[64];

    Round_SetStageByMenu(stageMenuId);
    Round_ReadStageString(stageId, sizeof(stageId));
    Round_NormalizeStageId(dllName, sizeof(dllName), stageId, 1);
    snprintf(dllPath, sizeof(dllPath), "bin/libr%s.so", dllName);

    return Round_LoadStagePath(dllPath);
}

int Round_LoadStageByName(const char *stageId) {
    char normalizedStageId[ROUND_STAGE_ID_LEN + 1];
    char dllName[ROUND_STAGE_ID_LEN + 1];
    char dllPath[64];

    Round_NormalizeStageId(normalizedStageId, sizeof(normalizedStageId), stageId, 0);
    if (!Round_SetStageByName(normalizedStageId)) {
        SDL_Log("Invalid round stage id: %s", stageId ? stageId : "(null)");
        return 0;
    }

    Round_NormalizeStageId(dllName, sizeof(dllName), normalizedStageId, 1);
    snprintf(dllPath, sizeof(dllPath), "bin/libr%s.so", dllName);

    return Round_LoadStagePath(dllPath);
}

static frontend_state Round_LoadNextStage(void) {
    /* WINEXE/WINMAIN.C credits a zone's good future when the next zone's act 1
       starts, if the act 3 that was just played ran in a good future (the
       reference keeps this in DAT_0042570c, set from the "?3C" stage id at
       load time).  clrgood is what decides the good ending later. */
    if (gameInfo.stageno.b.l == 0) {
        if (roundStageIsGoodFuture && gameInfo.stageno.b.h >= 1 &&
            gameInfo.stageno.b.h <= 7) {
            scoreData.clrgood |= (Uint8)(1 << (gameInfo.stageno.b.h - 1));
        }
        scoreData.roundNo = gameInfo.stageno.b.h;
        SetScoreDate(&scoreData);
    }

    if (gameInfo.stageno.w == 0x0700) {
        /* Last act of R8 cleared: the game is over, save the record and play
           the ending movie (the reference's gEndingMovieFlag path).  The
           opening DLL is loaded first because the main loop feeds the pad
           through the loaded module while the movie runs. */
        Round_Unload();
        scoreData.roundNo = 7;
        WriteScoreData(ReadScoreIndx(0), &scoreData, 0);
        if (!Title_LoadDLL("bin/libopening.so")) {
            return FRONTEND_STATE_EXIT;
        }
        Ending_Begin();
        return FRONTEND_STATE_ENDING;
    }

    WriteScoreData(ReadScoreIndx(0), &scoreData, 0);
    Round_Unload();
    if (!Round_LoadStageByMenu(0)) {
        return FRONTEND_STATE_EXIT;
    }
    return FRONTEND_STATE_ROUND;
}

static frontend_state Round_HandleClear(void) {
    gameInfo.plflag = 0;
    gameInfo.projector_flag = 0;

    if (gameInfo.ta_flag != 0) {
        Round_Unload();
        if (!TimeAttack_Load(0)) {
            return FRONTEND_STATE_EXIT;
        }
        return FRONTEND_STATE_TIME_ATTACK;
    }

    if (gameInfo.special_flag != 0) {
        Round_Unload();
        if (!Special_LoadStage(0)) {
            return FRONTEND_STATE_EXIT;
        }
        return FRONTEND_STATE_SPECIAL;
    }

    return Round_LoadNextStage();
}

static frontend_state Round_HandlePlayerDeath(void) {
    if (gameInfo.ta_flag != 0) {
        Round_Unload();
        if (!TimeAttack_Load(0)) {
            return FRONTEND_STATE_EXIT;
        }
        return FRONTEND_STATE_TIME_ATTACK;
    }

    if (gameInfo.pl_suu != 0) {
        CDPause(2);
        if (game_init) {
            game_init();
        }
        return FRONTEND_STATE_ROUND;
    }

    Round_Unload();
    if (!Title_LoadDLL("bin/libopening.so")) {
        return FRONTEND_STATE_EXIT;
    }
    return FRONTEND_STATE_TITLE;
}

frontend_state Round_Run(SDL_Renderer *renderer) {
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

    Graphics_Draw(renderer, scraHPosiw, scrbHPosiw, vscroll, &gameInfo);
    if (gameResult) {
        SDL_Log("round result %d", gameResult);
    }
    if (gameResult & 2) {
        return Round_HandleClear();
    }
    if (gameResult & 1) {
        return Round_HandlePlayerDeath();
    }
    if (gameResult & 0x100) {
        CDPause(2);
        Round_Unload();
        if (!Warp_Load()) {
            return FRONTEND_STATE_EXIT;
        }
        gameInfo.plflag = 2;
        gameInfo.time_flag |= 0x80;
        return FRONTEND_STATE_WARP;
    }
    return FRONTEND_STATE_ROUND;
}

void Round_Unload(void) {
    if (DLL_memfree) {
        DLL_memfree();
    }
    DLL_Unload();
}
