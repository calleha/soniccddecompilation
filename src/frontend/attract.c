#include <stdio.h>
#include <string.h>

#include <SDL3/SDL.h>

#include "attract.h"
#include "globals.h"
#include "movie.h"
#include "round.h"
#include "sound.h"
#include "special.h"
#include "title.h"

#define ATTRACT_RECORDING_MAX 0x3000
#define ATTRACT_END_MARKER 0xFFFF
/* A, B and Start on this port's pad layout (J, K, Return). */
#define ATTRACT_STOP_BUTTONS 0xB0

typedef enum { ATTRACT_MOVIE, ATTRACT_ROUND, ATTRACT_SPECIAL } attract_kind;

typedef struct {
    attract_kind kind;
    Uint32 stage;
    Uint8 play_start;
    const char *recording;
} attract_entry;

/* The rotation the reference cycles through: opening movie, R11A, special
   stage 1, R43C, special stage 6, R82A, then back to the movie. */
static const attract_entry kAttract[] = {
    {ATTRACT_MOVIE, 0, 0, NULL},
    {ATTRACT_ROUND, 400, 0, "KEY11A.BIN"},                      /* R11A */
    {ATTRACT_SPECIAL, 470, 0, "SPECIAL1.BIN"},                   /* stagenm 0 */
    {ATTRACT_ROUND, 428, 2, "KEY43C.BIN"},                       /* R43C */
    {ATTRACT_SPECIAL, 475, 0, "SPECIAL6.BIN"},                   /* stagenm 5 */
    {ATTRACT_ROUND, 464, 2, "KEY82A.BIN"},                       /* R82A */
};
#define ATTRACT_COUNT ((int)(sizeof(kAttract) / sizeof(kAttract[0])))

static int attract_next;
static int attract_active;
static int attract_demo;
static int attract_demo_done;
static attract_kind attract_kind_cur;
static Uint8 attract_recording[ATTRACT_RECORDING_MAX];
static int attract_frames;
static int attract_frame;

static int Attract_LoadRecording(const char *name) {
    char path[128];
    FILE *file;
    int length;

    snprintf(path, sizeof(path), "%s", name);
    file = fopen(path, "rb");
    if (!file) {
        snprintf(path, sizeof(path), "assets/../%s", name);
        file = fopen(path, "rb");
    }
    if (!file) {
        return 0;
    }
    length = (int)fread(attract_recording, 1, sizeof(attract_recording), file);
    fclose(file);
    if (length <= 0) {
        return 0;
    }
    /* Four bytes per frame: (held, newly pressed) for each pad. */
    return length / 4;
}

void Attract_Select(int entry) {
    if (entry < 0) {
        entry = 0;
    }
    attract_next = entry % ATTRACT_COUNT;
}

void Attract_Begin(void) {
    const attract_entry *entry = &kAttract[attract_next];

    attract_next = (attract_next + 1) % ATTRACT_COUNT;
    attract_active = 1;
    attract_demo = 0;
    attract_demo_done = 0;
    attract_kind_cur = entry->kind;

    if (entry->kind == ATTRACT_MOVIE) {
        SDL_Log("attract: opening movie");
        CDPause(2); /* the movie brings its own soundtrack */
        Movie_Play(0);
        return;
    }

    attract_frames = Attract_LoadRecording(entry->recording);
    if (attract_frames == 0) {
        SDL_Log("attract: recording %s missing, skipping", entry->recording);
        attract_active = 0;
        return;
    }
    SDL_Log("attract: demo %s", entry->recording);

    attract_frame = 0;
    attract_demo = 1;

    /* The reference marks an attract demo in the keep work and asks the stage
       code for the demo's alternate start (play_start bit 1).  Note that the
       game DLLs reset their own demoflag in game_init(), exactly as the
       original does, so the demos run from the act's normal start position. */
    gameInfo.play_start = entry->play_start;
    gameInfo.demoflag.w = -1;
    gameInfo.plflag = 0;
    gameInfo.clrspflg_save = 0;
    gameInfo.stagenm = 0;

    if (entry->kind == ATTRACT_ROUND) {
        if (!Round_LoadStageByMenu(entry->stage)) {
            attract_active = 0;
        }
    } else if (!Special_LoadStage(entry->stage)) {
        attract_active = 0;
    }
}

frontend_state Attract_Run(SDL_Renderer *renderer, Uint8 pressed) {
    frontend_state next;

    if (!attract_active) {
        return Title_Restart();
    }

    if (!attract_demo) {
        /* The movie itself is advanced by the main loop; here we only watch for
           the end of it. */
        if (pressed & ATTRACT_STOP_BUTTONS) {
            Movie_Stop();
            attract_active = 0;
            return Title_Restart();
        }
        if (!Movie_IsPlaying()) {
            attract_active = 0;
            return Title_Restart();
        }
        return FRONTEND_STATE_ATTRACT;
    }

    if (attract_demo_done) {
        attract_active = 0;
        return Title_Restart();
    }

    /* The demo stage runs like a normal stage, only fed from the recording; it
       leaving on its own (goal, time over, ...) also ends the demo. */
    next = (attract_kind_cur == ATTRACT_ROUND) ? Round_Run(renderer)
                                              : Special_Run(renderer);
    if (next == FRONTEND_STATE_ROUND || next == FRONTEND_STATE_SPECIAL) {
        return FRONTEND_STATE_ATTRACT;
    }
    attract_active = 0;
    return Title_Restart();
}

int Attract_OverridePad(ushort_union *pad1, ushort_union *pad2) {
    const Uint8 *frame;
    int end0;
    int end1;

    if (!attract_active || !attract_demo) {
        return 0;
    }
    /* The player pressing a button ends the demo, as in the original. */
    if (pad1->b.l & ATTRACT_STOP_BUTTONS) {
        attract_demo_done = 1;
        return 1;
    }
    if (attract_frame >= attract_frames) {
        attract_demo_done = 1;
        return 1;
    }

    frame = attract_recording + attract_frame * 4;
    end0 = (int)(Uint16)(frame[0] | (frame[1] << 8));
    end1 = (int)(Uint16)(frame[2] | (frame[3] << 8));
    if (end0 == ATTRACT_END_MARKER || end1 == ATTRACT_END_MARKER) {
        attract_demo_done = 1;
        return 1;
    }

    pad1->b.h = frame[0];
    pad1->b.l = frame[1];
    pad2->b.h = frame[2];
    pad2->b.l = frame[3];
    ++attract_frame;
    return 0;
}

int Attract_IsDemo(void) { return attract_active && attract_demo; }
