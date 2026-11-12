#include <SDL3/SDL.h>

#include "ending.h"
#include "globals.h"
#include "movie.h"
#include "sound.h"
#include "thanks.h"
#include "title.h"

#define ENDING_PART_COUNT 2

/* Movie ids requested by the reference's ending modules: the AVIBAD module
   plays 2 and then 3, the AVIGOOD module 4 and then 5 (src/title/avigood/
   avigoddo.c).  Each ending is a two part movie: either one combined file
   (END1.OGV / END2.OGV, credits included) in part 1's slot, or the pair of
   Mega-CD SFD halves.  Movie_Play() skips part 2 when part 1 was a combined
   file, so an ending never plays its second half twice.
   The ending messages then follow in the thanks module, with EndingMesType 1
   after the good ending and 2 after the bad one - the values the AVIGOOD and
   AVIBAD modules answer to the reference frontend. */
static const Sint32 kEndingMovies[2][ENDING_PART_COUNT] = {
    {2, 3}, /* bad ending */
    {4, 5}, /* good ending */
};

static int ending_active;
static int ending_good;
static int ending_part;

static int Ending_StartPart(void) {
    return Movie_Play(kEndingMovies[ending_good][ending_part]);
}

/* The reference's menu modules mark the message type before they load the
   thanks module; the low bits pick the message (1 = good, 2 = bad) and the
   high bit is the windowed flag. */
static frontend_state Ending_StartThanks(void) {
    gameInfo.EndingMesType = (Uint8)(ending_good ? 1 : 2);
    if (!Thanks_Load()) {
        if (!Title_LoadDLL("bin/libopening.so")) {
            return FRONTEND_STATE_EXIT;
        }
        return FRONTEND_STATE_TITLE;
    }
    return FRONTEND_STATE_THANKS;
}

void Ending_Play(int good) {
    ending_good = good ? 1 : 0;
    ending_active = 1;
    ending_part = 0;

    SDL_Log("ending: %s", ending_good ? "good" : "bad");
    CDPause(2); /* the movie brings its own soundtrack */

    /* A missing file is not fatal: try the second part, and if that is missing
       too the message screen follows straight away. */
    while (!Ending_StartPart()) {
        if (++ending_part >= ENDING_PART_COUNT) {
            ending_active = 0;
            return;
        }
    }
}

void Ending_Begin(void) {
    /* Same test as the reference: all seven Time Stones (clrspflg_save) or a
       good future reached in all seven zones (clrgood bits 0-6). */
    Ending_Play(gameInfo.clrspflg_save == 127 || scoreData.clrgood == 127);
}

frontend_state Ending_Run(Uint8 pressed) {
    (void)pressed; /* the reference ignores the pad while the movie plays */

    if (!ending_active) {
        return Ending_StartThanks();
    }
    if (Movie_IsPlaying()) {
        return FRONTEND_STATE_ENDING;
    }
    if (++ending_part >= ENDING_PART_COUNT) {
        ending_active = 0;
        return Ending_StartThanks();
    }
    while (!Ending_StartPart()) {
        if (++ending_part >= ENDING_PART_COUNT) {
            ending_active = 0;
            return Ending_StartThanks();
        }
    }
    return FRONTEND_STATE_ENDING;
}
