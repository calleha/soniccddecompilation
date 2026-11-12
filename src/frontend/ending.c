#include <SDL3/SDL.h>

#include "ending.h"
#include "globals.h"
#include "movie.h"
#include "sound.h"
#include "title.h"

#define ENDING_PART_COUNT 2

/* Movie ids requested by the reference's ending modules: the AVIBAD module
   plays 2 and then 3, the AVIGOOD module 4 and then 5 (src/title/avigood/
   avigoddo.c).  Each ending is a two part movie: either one combined file
   (END1.OGV / END2.OGV, credits included) in part 1's slot, or the pair of
   Mega-CD SFD halves.  Movie_Play() skips part 2 when part 1 was a combined
   file, so an ending never plays its second half twice. */
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

void Ending_Begin(void) {
    /* Same test as the reference: all seven Time Stones (clrspflg_save) or a
       good future reached in all seven zones (clrgood bits 0-6). */
    ending_good = (gameInfo.clrspflg_save == 127 || scoreData.clrgood == 127);
    ending_active = 1;
    ending_part = 0;

    SDL_Log("ending: %s", ending_good ? "good" : "bad");
    CDPause(2); /* the movie brings its own soundtrack */

    /* A missing file is not fatal: try the second part, and if that is missing
       too the sequence simply ends. */
    while (!Ending_StartPart()) {
        if (++ending_part >= ENDING_PART_COUNT) {
            ending_active = 0;
            return;
        }
    }
}

frontend_state Ending_Run(Uint8 pressed) {
    (void)pressed; /* the reference ignores the pad while the movie plays */

    if (!ending_active) {
        return Title_Restart();
    }
    if (Movie_IsPlaying()) {
        return FRONTEND_STATE_ENDING;
    }
    if (++ending_part >= ENDING_PART_COUNT) {
        ending_active = 0;
        return Title_Restart();
    }
    while (!Ending_StartPart()) {
        if (++ending_part >= ENDING_PART_COUNT) {
            ending_active = 0;
            return Title_Restart();
        }
    }
    return FRONTEND_STATE_ENDING;
}
