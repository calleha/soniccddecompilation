#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <SDL3/SDL.h>
#include "score.h"
#include "../types.h"

extern game_info gameInfo;
extern score_data scoreData;
extern char **memoryTbl[];
extern void *functionTbl[];

/* Pad status for the modules that ask for it (the planeta module's
   sPeriPadGet).  Layout is the pad_status struct the modules declare. */
void *FrontendPadGet(Uint32 padNo);

void Globals_Init(SDL_Window *w, SDL_Renderer *r);

#ifdef __cplusplus
}
#endif
