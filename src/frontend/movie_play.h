#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <SDL3/SDL.h>
#include "../types.h"
#include "frontend_state.h"

/* A single movie that is not part of an ending.
 *
 * The reference reaches the opening movie and the pencil test through the
 * AVIOPEN / AVIPEN movie modules: TITLE/OPENING's visual mode entry answers 8
 * and 12, changeMenuState() turns those into loadAviOpen()/loadAvipen(), and
 * when the movie is over the module answers 9, which loads the opening again -
 * or the visual mode menu when the player came from there.  The port plays the
 * movie itself and goes back to the same place. */

/* Movie ids: 0 opening, 1 pencil test (see movie.h). */
void MoviePlay_Begin(Sint32 id, int backToVisualMode);

frontend_state MoviePlay_Run(Uint8 pressed);

#ifdef __cplusplus
}
#endif
