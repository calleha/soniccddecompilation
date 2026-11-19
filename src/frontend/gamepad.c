#include <stdio.h>
#include <string.h>

#include <SDL3/SDL.h>

#include "gamepad.h"

/* The game's pad bits, as `joy1` and the stage modules use them. */
#define PAD_UP 0x01
#define PAD_DOWN 0x02
#define PAD_LEFT 0x04
#define PAD_RIGHT 0x08
#define PAD_A 0x10
#define PAD_B 0x20
#define PAD_C 0x40
#define PAD_START 0x80

/* How far the left stick has to move before it steers.  A resting stick has to
   be silent: the stage module ends a stage after three minutes of a player who
   is not touching anything, and a drifting stick would keep it from ever
   standing still.  SDL's axis range is -32768..32767. */
#define STICK_DEADZONE 10000

/* How far a trigger has to be pulled before it counts as pressed.  A digital
   trigger reads the whole axis (32767) when down and at most half of it when up,
   so a threshold well above half separates the two without making an analog
   trigger feel stiff. */
#define TRIGGER_PRESSED 24000

/* The 8BitDo M30 is a Sega-style six-button pad, and SDL's game pad only has
   four face positions, so SDL's built-in mapping for it covers A, B, X and Y
   and leaves the pad's C and Z unmapped.  Those are the buttons read below: for
   that pad, any raw button its mapping does not use is another face button and
   joins the game's C. */
#define RAW_BUTTON_LIMIT 32

static SDL_Gamepad *gamepad;
static int is_m30;
static Uint32 mappedRaw;
static int padDebug = -1;
static Uint8 lastBits;
static Uint32 lastRaw;
static Sint16 lastLeftTrigger;
static Sint16 lastRightTrigger;

/* The raw button indices SDL's mapping for this pad already accounts for, read
   out of the mapping string ("GUID,name,a:b1,b:b0,...": only the part after the
   second comma is examined, so hexadecimal digits in the GUID cannot be mistaken
   for button numbers). */
static Uint32 Gamepad_MappedRawButtons(void) {
    const char *mapping = SDL_GetGamepadMapping(gamepad);
    const char *cursor;
    Uint32 mapped = 0;
    int commas = 0;

    if (!mapping) {
        return 0;
    }
    for (cursor = mapping; *cursor && commas < 2; ++cursor) {
        if (*cursor == ',') {
            ++commas;
        }
    }
    while (cursor && *cursor) {
        if (*cursor == 'b' && cursor[1] >= '0' && cursor[1] <= '9') {
            int index = SDL_atoi(cursor + 1);

            if (index >= 0 && index < RAW_BUTTON_LIMIT) {
                mapped |= 1u << index;
            }
        }
        ++cursor;
    }
    return mapped;
}

static void Gamepad_Open(void) {
    SDL_JoystickID *ids;
    int count = 0;
    int i;

    if (gamepad) {
        return;
    }
    ids = SDL_GetGamepads(&count);
    if (!ids) {
        return;
    }
    for (i = 0; i < count && !gamepad; ++i) {
        gamepad = SDL_OpenGamepad(ids[i]);
        if (gamepad) {
            const char *name = SDL_GetGamepadName(gamepad);

            is_m30 = (name && strstr(name, "M30") != NULL);
            mappedRaw = is_m30 ? Gamepad_MappedRawButtons() : 0;
            if (is_m30) {
                SDL_Log("gamepad: %s (its unmapped face buttons join C)",
                        name ? name : "(no name)");
            } else {
                SDL_Log("gamepad: %s", name ? name : "(no name)");
            }
        }
    }
    SDL_free(ids);
}

int Gamepad_Init(void) {
    Gamepad_Open();
    return gamepad != NULL;
}

void Gamepad_HandleEvent(const SDL_Event *event) {
    if (!event) {
        return;
    }
    if (event->type == SDL_EVENT_GAMEPAD_ADDED) {
        if (!gamepad) {
            Gamepad_Open();
        }
    } else if (event->type == SDL_EVENT_GAMEPAD_REMOVED) {
        if (gamepad && SDL_GetGamepadID(gamepad) == event->gdevice.which) {
            SDL_CloseGamepad(gamepad);
            gamepad = NULL;
            is_m30 = 0;
            mappedRaw = 0;
            lastBits = 0;
            lastRaw = 0;
            /* Another pad may already be plugged in. */
            Gamepad_Open();
        }
    }
}

Uint8 Gamepad_Poll(void) {
    SDL_Joystick *joystick;
    Sint16 x;
    Sint16 y;
    Sint16 left_trigger;
    Sint16 right_trigger;
    Uint8 bits = 0;
    Uint32 raw = 0;
    Uint32 extra = 0;

    if (!gamepad) {
        return 0;
    }
    if (padDebug < 0) {
        const char *option = SDL_getenv("PAD_DEBUG");

        padDebug = (option && *option && *option != '0') ? 1 : 0;
    }
    SDL_UpdateGamepads();

    if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_UP)) {
        bits |= PAD_UP;
    }
    if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN)) {
        bits |= PAD_DOWN;
    }
    if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_LEFT)) {
        bits |= PAD_LEFT;
    }
    if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT)) {
        bits |= PAD_RIGHT;
    }
    x = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX);
    y = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY);
    if (x <= -STICK_DEADZONE) {
        bits |= PAD_LEFT;
    }
    if (x >= STICK_DEADZONE) {
        bits |= PAD_RIGHT;
    }
    if (y <= -STICK_DEADZONE) {
        bits |= PAD_UP;
    }
    if (y >= STICK_DEADZONE) {
        bits |= PAD_DOWN;
    }

    /* Only the pad's A, B and C jump - nothing else.  SDL maps a Sega-style pad
       by position: the 8BitDo M30 reports its own A as SOUTH and its B as EAST
       (SDL's built-in mapping for it is `a:b1, b:b0`), and its C arrives as the
       right trigger, so SOUTH is the game's A, EAST the game's B and the right
       trigger the game's C.  The other face buttons, the left trigger and the
       shoulders are deliberately silent. */
    if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_SOUTH)) {
        bits |= PAD_A;
    }
    if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_EAST)) {
        bits |= PAD_B;
    }
    left_trigger = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER);
    right_trigger = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);
    if (right_trigger > TRIGGER_PRESSED) {
        bits |= PAD_C;
    }

    if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_START)) {
        bits |= PAD_START;
    }

    joystick = SDL_GetGamepadJoystick(gamepad);
    if (joystick) {
        int count = SDL_GetNumJoystickButtons(joystick);
        int hats = SDL_GetNumJoystickHats(joystick);
        int i;

        for (i = 0; i < count && i < RAW_BUTTON_LIMIT; ++i) {
            if (SDL_GetJoystickButton(joystick, i)) {
                raw |= 1u << i;
            }
        }
        /* A d-pad reported as a hat is read here as well: SDL's mapping for the
           M30 has no d-pad entries at all (it is a d-pad-only pad, so that would
           leave it with no way to steer), and reading the hat directly costs
           nothing on pads whose d-pad the mapping does cover. */
        if (hats > 0) {
            Uint8 hat = SDL_GetJoystickHat(joystick, 0);

            if (hat & SDL_HAT_UP) {
                bits |= PAD_UP;
            }
            if (hat & SDL_HAT_DOWN) {
                bits |= PAD_DOWN;
            }
            if (hat & SDL_HAT_LEFT) {
                bits |= PAD_LEFT;
            }
            if (hat & SDL_HAT_RIGHT) {
                bits |= PAD_RIGHT;
            }
        }
    }
    if (is_m30 && joystick && SDL_GetNumJoystickHats(joystick) > 0) {
        /* The M30's C and Z: whichever raw buttons its SDL mapping does not
           place are the face buttons SDL has no position for, and they jump.
           Only done when the pad's d-pad is a hat, so a d-pad reported as
           buttons can never be mistaken for face buttons. */
        extra = raw & ~mappedRaw;
        if (extra) {
            bits |= PAD_C;
        }
    }

    /* PAD_DEBUG=1 logs every change, raw buttons, unmapped raw buttons and both
       triggers included, so a pad's buttons can be checked against the game's by
       playing - including buttons the mapping does not reach. */
    if (padDebug && (bits != lastBits || raw != lastRaw ||
                     left_trigger != lastLeftTrigger ||
                     right_trigger != lastRightTrigger)) {
        if (is_m30) {
            SDL_Log("gamepad: pad %02x raw %08x (unmapped %08x) triggers %d,%d",
                    bits, (unsigned)raw, (unsigned)(raw & ~mappedRaw),
                    (int)left_trigger, (int)right_trigger);
        } else {
            SDL_Log("gamepad: pad %02x raw %08x triggers %d,%d", bits,
                    (unsigned)raw, (int)left_trigger, (int)right_trigger);
        }
    }
    lastBits = bits;
    lastRaw = raw;
    lastLeftTrigger = left_trigger;
    lastRightTrigger = right_trigger;
    return bits;
}

void Gamepad_Shutdown(void) {
    if (gamepad) {
        SDL_CloseGamepad(gamepad);
        gamepad = NULL;
    }
    is_m30 = 0;
    mappedRaw = 0;
    lastBits = 0;
    lastRaw = 0;
}
