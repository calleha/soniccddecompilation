#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <SDL3/SDL.h>
#include "../types.h"
#include "frontend_state.h"

/* The "THANKS" screen that follows an ending movie.
 *
 * TITLE/THANKS is a stage style module - game_init/game/DLL_meminit/
 * DLL_memfree/SWdataSet - with no entry point in the decompilation, so the
 * frontend drives it: load the DLL, hand out the memory and function tables,
 * run game() once per frame and draw the framebuffer.  The message it shows
 * depends on gameInfo.EndingMesType, which has to be set before Load(): the
 * reference uses 1 after the good ending's movies and 2 after the bad
 * ending's, adding 128 in windowed mode (WINEXE/WINMAIN.C cases 7 and 8).
 * game() returns -1 when the player leaves, and the title sequence starts
 * again. */

int Thanks_Load(void);
frontend_state Thanks_Run(SDL_Renderer *renderer);
void Thanks_Unload(void);

#ifdef __cplusplus
}
#endif
