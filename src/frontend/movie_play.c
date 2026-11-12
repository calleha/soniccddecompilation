#include <SDL3/SDL.h>

#include "globals.h"
#include "movie.h"
#include "movie_play.h"
#include "sound.h"
#include "title.h"

/* A, B and Start on this port's pad layout (J, K, Return). */
#define MOVIE_STOP_BUTTONS 0xB0

static int movieActive;
static int movieBackToVisualMode;

void MoviePlay_Begin(Sint32 id, int backToVisualMode) {
    movieBackToVisualMode = backToVisualMode;
    movieActive = 1;
    CDPause(2); /* the movie brings its own soundtrack */
    if (!Movie_Play(id)) {
        /* Nothing to play: behave as if it had already finished. */
        movieActive = 0;
    }
}

frontend_state MoviePlay_Run(Uint8 pressed) {
    if (!movieActive) {
        return movieBackToVisualMode ? Title_VisualMode() : Title_Restart();
    }
    if (pressed & MOVIE_STOP_BUTTONS) {
        /* The reference's movie modules let the player cut the movie short. */
        Movie_Stop();
        movieActive = 0;
        return movieBackToVisualMode ? Title_VisualMode() : Title_Restart();
    }
    if (!Movie_IsPlaying()) {
        movieActive = 0;
        return movieBackToVisualMode ? Title_VisualMode() : Title_Restart();
    }
    return FRONTEND_STATE_MOVIE;
}
