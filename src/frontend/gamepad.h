#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <SDL3/SDL.h>
#include "../types.h"

/* Gamepad input.
 *
 * SDL3 gamepads are polled alongside the keyboard and produce the same eight
 * game-pad bits the modules read (0x01 up, 0x02 down, 0x04 left, 0x08 right,
 * 0x10 A, 0x20 B, 0x40 C, 0x80 Start), so everything that works with keys -
 * steering, jumping, pausing, menus - works with a pad, and the two can be used
 * at the same time.  A pad delivers real same-frame button presses, which is
 * what the stage module's restart combination (A+B+C, see
 * docs/restart_stage.md) needs. */

int Gamepad_Init(void);
void Gamepad_HandleEvent(const SDL_Event *event);
Uint8 Gamepad_Poll(void);
void Gamepad_Shutdown(void);

#ifdef __cplusplus
}
#endif
