#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <SDL3/SDL.h>
#include "../types.h"
#include "frontend_state.h"

/* Attract sequence.
 *
 * The Windows frontend ran this from menuOrMovieLoop()'s demo case: the first
 * idle period on the title screen played the opening movie, and every idle
 * period after that played one of the recorded stage demos, wrapping around.
 * The order (opening movie, R11A, special stage 1, R43C, special stage 6,
 * R82A) is the one the reference cycles through. */

/* Points the rotation at a given entry, so the `attract <n>` command line mode
   can start any of them without waiting for the title screen to time out. */
void Attract_Select(int entry);

/* Picks the next item in the rotation and starts it. */
void Attract_Begin(void);

/* Runs one frame of the current item; returns FRONTEND_STATE_ATTRACT while it
   is still going. `pressed` is the pad byte of keys that went down this frame,
   used to cut the item short. */
frontend_state Attract_Run(SDL_Renderer *renderer, Uint8 pressed);

/* While a stage demo runs the recorded pad replaces the live one.  Returns 1
   once the demo is over (recording finished, or the player pressed a button
   on the real pad). */
int Attract_OverridePad(ushort_union *pad1, ushort_union *pad2);

int Attract_IsDemo(void);

#ifdef __cplusplus
}
#endif
