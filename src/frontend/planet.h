#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <SDL3/SDL.h>
#include "../types.h"
#include "frontend_state.h"

/* D.A. Garden (Little Planet).
 *
 * TITLE/PLANET is one of the modules that has no entry point in the
 * decompilation - it exports game_init/game/DLL_meminit/DLL_memfree/SWdataSet
 * (plus FadeProc) like a stage module, so the frontend drives it the way it
 * drives a round: load the DLL, hand out the memory and function tables, run
 * game() once per frame and draw the framebuffer.  game() returns -1 when the
 * player leaves. */

int Planet_Load(void);
frontend_state Planet_Run(SDL_Renderer *renderer);
void Planet_Unload(void);

#ifdef __cplusplus
}
#endif
