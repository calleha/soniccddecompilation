#include <SDL3/SDL.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dll.h"
#include "file.h"
#include "frontend_state.h"
#include "globals.h"
#include "graphics.h"
#include "hmx.h"
#include "round.h"
#include "score.h"
#include "sound.h"
#include "special.h"
#include "ta.h"
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

static frontend_pad_status *FrontendPadGet(Uint32 padNo) {
    (void)padNo;
    return &padStatus;
}

int main(int argc, char **argv) {
    frontend_state state = FRONTEND_STATE_TITLE;

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
        Uint8 lastPressed = joy1.b.h;
        Uint32 lastPadOn = padStatus.On;
        joy1.b.h = pressed;
        joy1.b.l = (~lastPressed) & pressed;
        padStatus.On = 0;
        if (keys[SDL_SCANCODE_J]) { padStatus.On |= 0x02; }
        if (keys[SDL_SCANCODE_K]) { padStatus.On |= 0x04; }
        padStatus.Press = (~lastPadOn) & padStatus.On;
        padStatus.Release = lastPadOn & ~padStatus.On;
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
    }
    Graphics_Shutdown();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
	return 0;
}
