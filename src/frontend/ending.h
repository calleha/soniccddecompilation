#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <SDL3/SDL.h>
#include "../types.h"
#include "frontend_state.h"

/* Ending sequence.
 *
 * Clearing the last act of R8 ends the game.  The reference (WINEXE/WINMAIN.C,
 * the "stage game function returned 2" branch) then works out which ending to
 * show - every Time Stone or every zone's good future means the good one - and
 * sets gEndingMovieFlag, which the main loop turns into a two part movie: the
 * AVIBAD module plays movies 2 and 3, the AVIGOOD module movies 4 and 5 (see
 * src/title/avigood/avigoddo.c).  The files are TITLE/AVI/END1.OGV and
 * TITLE/AVI/END2.OGV. */

/* Starts the ending that matches the current save: good when every Time Stone
   or every good future was collected, bad otherwise. */
void Ending_Begin(void);

/* Runs one frame; returns FRONTEND_STATE_ENDING until the movies are over. */
frontend_state Ending_Run(Uint8 pressed);

#ifdef __cplusplus
}
#endif
