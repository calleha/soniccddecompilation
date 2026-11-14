#include <SDL3/SDL.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "attract.h"
#include "dll.h"
#include "ending.h"
#include "file.h"
#include "frontend_state.h"
#include "globals.h"
#include "graphics.h"
#include "hmx.h"
#include "movie.h"
#include "movie_play.h"
#include "planet.h"
#include "round.h"
#include "score.h"
#include "sound.h"
#include "special.h"
#include "ta.h"
#include "thanks.h"
#include "title.h"
#include "warp.h"
#include "utilities.h"

#define MAX_SPRITES 256
#define FUNC_TABLE_SIZE 64

SDL_Renderer *renderer;
SDL_Window *window;
ushort_union joy1;
ushort_union joy2;
Uint32 fullScreen;
Uint32 visualMode;
Uint16 selectedStage;
Uint16 userKeys[5];

typedef struct {
    Uint32 On;
    Uint32 Press;
    Uint32 Release;
    Sint16 X1;
    Sint16 Y1;
    Sint16 X2;
    Sint16 Y2;
} frontend_pad_status;

static frontend_pad_status padStatus;

/* Handed to the modules through functionTbl slot 54 (the planet module's
   sPeriPadGet).  This port has a single pad, so both numbers give the same
   struct. */
void *FrontendPadGet(Uint32 padNo) {
    (void)padNo;
    return &padStatus;
}

int main(int argc, char **argv) {
    frontend_state state = FRONTEND_STATE_TITLE;
    int i;

    /* --scanlines turns on the soft scanline filter (see
       Graphics_SetScanlines()), --fullscreen starts in fullscreen and
       --timewarp-noskip brings back the time warp transition, which is skipped
       by default.  All three are switches rather than sub-commands, so they are
       taken out of the argument list here and can be given anywhere on the
       command line, e.g. "./frontend --fullscreen" or
       "./frontend round 11A --scanlines". */
    {
        int kept = 1;

        for (i = 1; i < argc; ++i) {
            if (strcmp(argv[i], "--scanlines") == 0) {
                Graphics_SetScanlines(1);
            } else if (strcmp(argv[i], "--fullscreen") == 0) {
                Graphics_SetFullscreen(1);
            } else if (strcmp(argv[i], "--timewarp-noskip") == 0) {
                Round_SetSkipTimeWarp(0);
            } else {
                argv[kept++] = argv[i];
            }
        }
        argc = kept;
        argv[argc] = NULL;
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Couldn't init SDL: %s\n", SDL_GetError());
        return -1;
    }

    if (!Graphics_Init(&window, &renderer)) {
        fprintf(stderr, "Graphics_Init failed\n");
        return -1;
    }
    Globals_Init(window, renderer);

    if (!Sound_Init()) {
        fprintf(stderr, "Sound_Init failed\n");
        return -1;
    }

    if (argc > 1 && strcmp(argv[1], "round") == 0) {
        state = FRONTEND_STATE_ROUND;
        if (argc > 2) {
            char *end = NULL;
            unsigned long stageMenuId = strtoul(argv[2], &end, 10);
            if (end && *end == '\0') {
                if (!Round_LoadStageByMenu((Uint32)stageMenuId)) {
                    return -1;
                }
            } else if (!Round_LoadStageByName(argv[2])) {
                return -1;
            }
        } else if (!Round_LoadStageByMenu(ROUND_STAGE_R11A)) {
            return -1;
        }
    } else if (argc > 1 && strcmp(argv[1], "warp") == 0) {
        state = FRONTEND_STATE_WARP;
        if (!Warp_Load()) {
            return -1;
        }
    } else if (argc > 1 && strcmp(argv[1], "special") == 0) {
        state = FRONTEND_STATE_SPECIAL;
        if (argc > 2) {
            char *end = NULL;
            unsigned long stageMenuId = strtoul(argv[2], &end, 10);
            if (end && *end == '\0') {
                if (!Special_LoadStage((Uint32)stageMenuId)) {
                    return -1;
                }
            } else {
                return -1;
            }
        } else if (!Special_LoadStage(0)) {
            return -1;
        }
    } else if (argc > 1 && strcmp(argv[1], "ta") == 0) {
        state = FRONTEND_STATE_TIME_ATTACK;
        if (!TimeAttack_Load(1)) {
            return -1;
        }
    } else if (argc > 1 && strcmp(argv[1], "attract") == 0) {
        /* Starting point of the attract rotation: 0 opening movie, 1..5 the
           stage demos in the reference's order.  The opening DLL provides the
           pad bridge (SWdataSet) the main loop needs; a demo entry replaces it
           with its own stage DLL. */
        if (!Title_LoadDLL("bin/libopening.so")) {
            return -1;
        }
        state = FRONTEND_STATE_ATTRACT;
        if (argc > 2) {
            char *end = NULL;
            unsigned long entry = strtoul(argv[2], &end, 10);
            if (!end || *end != '\0') {
                return -1;
            }
            Attract_Select((int)entry);
        }
        Attract_Begin();
    } else if (argc > 1 && strcmp(argv[1], "ending") == 0) {
        /* Plays the ending directly: `ending good` or `ending bad` forces the
           save condition, with no argument the current save decides.  The
           opening DLL provides the pad bridge the main loop needs. */
        if (!Title_LoadDLL("bin/libopening.so")) {
            return -1;
        }
        if (argc > 2) {
            if (strcmp(argv[2], "good") == 0) {
                gameInfo.clrspflg_save = 127;
            } else if (strcmp(argv[2], "bad") == 0) {
                gameInfo.clrspflg_save = 0;
                scoreData.clrgood = 0;
            } else {
                return -1;
            }
        }
        Ending_Begin();
        state = FRONTEND_STATE_ENDING;
    } else if (argc > 1 && strcmp(argv[1], "planet") == 0) {
        /* D.A. Garden, the Little Planet screen. */
        if (!Title_LoadDLL("bin/libopening.so")) {
            return -1;
        }
        if (!Planet_Load()) {
            return -1;
        }
        state = FRONTEND_STATE_PLANET;
    } else if (argc > 1 && strcmp(argv[1], "thanks") == 0) {
        /* The ending message screen: `thanks good` (type 1) or `thanks bad`
           (type 2); no argument uses the current save like the ending does. */
        if (!Title_LoadDLL("bin/libopening.so")) {
            return -1;
        }
        gameInfo.EndingMesType = 1;
        if (argc > 2) {
            if (strcmp(argv[2], "bad") == 0) {
                gameInfo.EndingMesType = 2;
            } else if (strcmp(argv[2], "good") != 0) {
                return -1;
            }
        }
        if (!Thanks_Load()) {
            return -1;
        }
        state = FRONTEND_STATE_THANKS;
    } else if (argc > 1 && strcmp(argv[1], "movie") == 0) {
        /* One movie by id (see movie.h): `movie 1` is the pencil test. */
        Sint32 movieId = 0;

        if (!Title_LoadDLL("bin/libopening.so")) {
            return -1;
        }
        if (argc > 2) {
            char *end = NULL;
            unsigned long id = strtoul(argv[2], &end, 10);
            if (!end || *end != '\0') {
                return -1;
            }
            movieId = (Sint32)id;
        }
        MoviePlay_Begin(movieId, 0);
        state = FRONTEND_STATE_MOVIE;
    } else if (argc > 1 && strcmp(argv[1], "menu") == 0) {
        /* Loads a menu module directly: savedata, soundtst, stagetst, besttime,
           visualmd. */
        char path[64];

        if (argc < 3) {
            return -1;
        }
        snprintf(path, sizeof(path), "bin/lib%s.so", argv[2]);
        if (!Title_LoadDLL(path)) {
            return -1;
        }
        state = FRONTEND_STATE_TITLE;
    } else {
        if (!Title_LoadDLL("bin/libopening.so")) {
            return -1;
        }
    }

    int running = 1;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = 0;
            }
        }

        Uint8 pressed = 0;
        const bool *keys = SDL_GetKeyboardState(NULL);
        if (keys[SDL_SCANCODE_W]) { pressed |= 0x01; }
        if (keys[SDL_SCANCODE_S]) { pressed |= 0x02; }
        if (keys[SDL_SCANCODE_A]) { pressed |= 0x04; }
        if (keys[SDL_SCANCODE_D]) { pressed |= 0x08; }
        if (keys[SDL_SCANCODE_J]) { pressed |= 0x10; }
        if (keys[SDL_SCANCODE_K]) { pressed |= 0x20; }
        if (keys[SDL_SCANCODE_L]) { pressed |= 0x40; }
        if (keys[SDL_SCANCODE_RETURN]) { pressed |= 0x80; }
        /* The live pad edges are tracked separately from joy1, because a
           running stage demo replaces joy1 with its recording. */
        static Uint8 lastRealPressed;
        Uint8 newlyPressed = (Uint8)(~lastRealPressed & pressed);
        lastRealPressed = pressed;
        Uint32 lastPadOn = padStatus.On;
        joy1.b.h = pressed;
        joy1.b.l = newlyPressed;
        padStatus.On = 0;
        if (keys[SDL_SCANCODE_J]) { padStatus.On |= 0x02; }
        if (keys[SDL_SCANCODE_K]) { padStatus.On |= 0x04; }
        padStatus.Press = (~lastPadOn) & padStatus.On;
        padStatus.Release = lastPadOn & ~padStatus.On;
        /* A running stage demo feeds its recorded pad instead of the live one. */
        Attract_OverridePad(&joy1, &joy2);
        /* A running movie is advanced here rather than from the state that
           started it, so the player keeps going no matter which DLL is on the
           screen polling Movie_Stat(). */
        Movie_Update(renderer);
        SWdataSet(joy1, joy2);

        switch (state) {
        case FRONTEND_STATE_TITLE:
            state = Title_Run();
            break;
        case FRONTEND_STATE_ROUND:
            state = Round_Run(renderer);
            break;
        case FRONTEND_STATE_WARP:
            state = Warp_Run(renderer);
            break;
        case FRONTEND_STATE_SPECIAL:
            state = Special_Run(renderer);
            break;
        case FRONTEND_STATE_ATTRACT:
            state = Attract_Run(renderer, pressed);
            break;
        case FRONTEND_STATE_ENDING:
            state = Ending_Run(newlyPressed);
            break;
        case FRONTEND_STATE_PLANET:
            state = Planet_Run(renderer);
            break;
        case FRONTEND_STATE_THANKS:
            state = Thanks_Run(renderer);
            break;
        case FRONTEND_STATE_MOVIE:
            state = MoviePlay_Run(pressed);
            break;
        case FRONTEND_STATE_TIME_ATTACK:
            state = TimeAttack_Run(renderer);
            break;
        case FRONTEND_STATE_EXIT:
            running = 0;
            break;
        default:
            running = 0;
            break;
        }
        if (state == FRONTEND_STATE_EXIT) {
            running = 0;
        }
    }

    if (state == FRONTEND_STATE_ROUND) {
        Round_Unload();
    } else if (state == FRONTEND_STATE_WARP) {
        Warp_Unload();
    } else if (state == FRONTEND_STATE_SPECIAL) {
        Special_Unload();
    } else if (state == FRONTEND_STATE_TIME_ATTACK) {
        TimeAttack_Unload();
    } else if (state == FRONTEND_STATE_TITLE) {
        Title_Unload();
    } else if (state == FRONTEND_STATE_ATTRACT) {
        Movie_Shutdown();
        Title_Unload();
    } else if (state == FRONTEND_STATE_ENDING) {
        Movie_Shutdown();
        Title_Unload();
    } else if (state == FRONTEND_STATE_PLANET) {
        Planet_Unload();
    } else if (state == FRONTEND_STATE_THANKS) {
        Thanks_Unload();
    } else if (state == FRONTEND_STATE_MOVIE) {
        Movie_Shutdown();
        Title_Unload();
    }
    Movie_Shutdown();
    Graphics_Shutdown();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
	return 0;
}
