# Gamepad input

The frontend reads SDL3 gamepads alongside the keyboard.  Both produce the same
eight game-pad bits the modules already understand, so steering, jumping,
pausing, menus and the attract demos' "press a button to skip" all work with a
pad, and a pad and the keyboard can be used at once.

Nothing about the game modules changed: they still receive `joy1`/`joy2` through
`SWdataSet()`, with the held bits in `b.h` and the buttons that went down this
frame in `b.l`.

## The bits

| Bit | Button | Keyboard (this port) | Gamepad |
| --- | --- | --- | --- |
| `0x01` | Up | `W` | d-pad up, left stick up |
| `0x02` | Down | `S` | d-pad down, left stick down |
| `0x04` | Left | `A` | d-pad left, left stick left |
| `0x08` | Right | `D` | d-pad right, left stick right |
| `0x10` | A | `J` | south face button (the pad's A) |
| `0x20` | B | `K` | east face button (the pad's B) |
| `0x40` | C | `L` | right trigger (the pad's C), or the M30's hidden C when SDL cannot see it |
| `0x80` | Start | `Return` | Start |

**Only the pad's A, B and C jump** - nothing else does.  The naming matters
because the game's three-button restart combination is A+B+C
(`docs/restart_stage.md`), and because a pad's own A should be the game's A.
Everything else on the pad - the north and west face buttons, the left trigger,
both shoulders, guide - is deliberately silent, so the three jump buttons are
exactly the three the game names.

The left stick has a dead zone (`STICK_DEADZONE`, about 30 %): a resting stick
must be silent, or the stage module's three-minute idle timer would never see a
player standing still.

## The 8BitDo M30

The M30 is a Sega-style six-button pad, which is the case that needs care:

- SDL's built-in mapping for it (`a:b1, b:b0, x:b4, y:b3, ...`) puts the pad's
  own **A on SDL's SOUTH** and its **B on SDL's EAST**, so those two come out as
  the game's A and B with no special handling.
- SDL's game pad has only four face positions, so the M30's **C and Z are not in
  the mapping at all**.  They are read from the joystick directly: for that pad,
  any raw button its own mapping does not mention is another face button, and
  they join the game's C.  That is why the pad's C reaches the game's C in the
  Bluetooth mode too, without hard-coding button numbers.
- The M30's mapping has **no d-pad entries**, and it has no stick - it is a
  d-pad-only pad - so the d-pad is read from the raw hat as well.
- Pads with more buttons than SDL has positions often put the extras on **ZL/ZR**,
  which SDL reports as the two trigger axes rather than as buttons.  That is
  where the M30's C comes from: the right trigger is the game's C.  The left
  trigger is left silent.
- 8BitDo pads report themselves differently depending on the mode switch: in
  XInput mode SDL sees an "Xbox 360 Controller", where the M30's C arrives as the
  **right trigger** (`pad 40 … triggers 0,32767`) and the rest as face buttons
  and shoulders.  In Bluetooth mode SDL uses its own M30 mapping, and C is one of
  the two buttons that mapping has no room for - that case is covered by the raw
  button rule above.

The result, checked against a virtual pad carrying SDL's real M30 mapping:

```
M30 A (raw 1)                     -> 0x10  (game A)   ok
M30 B (raw 0)                     -> 0x20  (game B)   ok
M30 C (raw 5, not in SDL mapping) -> 0x40  (game C)   ok
M30 Z (raw 8, not in SDL mapping) -> 0x40  (game C)   ok
X / Y (raw 4 / raw 3)             -> 0x00  (silent)   ok
guide (raw 2)                     -> 0x00             ok
start (raw 11)                    -> 0x80  (Start)    ok
left / right shoulder             -> 0x00  (silent)   ok
left trigger                      -> 0x00  (silent)   ok
right trigger                     -> 0x40  (game C)   ok
A+B+C together                    -> 0x70             ok
d-pad up / stick left             -> 0x01 / 0x04      ok
```

Because a pad delivers real same-frame presses, holding the M30's A+B+C is the
restart combination, exactly as the stage module has always expected it - no
frontend help needed (unlike the keyboard, which is why that workaround is parked
in `patches/restart_stage_keyboard.patch`).

## Pads come and go

`SDL_INIT_GAMEPAD` is part of the frontend's `SDL_Init()`.  A pad that is already
plugged in is opened at startup; `SDL_EVENT_GAMEPAD_ADDED`/`REMOVED` are handled
in the main loop, so a pad can be connected, unplugged and swapped while the game
runs.  With no pad, everything behaves exactly as before.

`PAD_DEBUG=1` logs every change, including raw buttons, the raw buttons the SDL
mapping does not place, and both triggers:

```
gamepad: 8BitDo M30 Gamepad (its unmapped face buttons join C)
gamepad: pad 70 raw 00000023 (unmapped 00000020) triggers 0,32767
gamepad: pad 10 raw 00000000 triggers 32767,0        (a pad SDL does not call an M30)
```

That is how to check a pad's buttons against the game's without a debugger,
including buttons and triggers the mapping does not reach.

## Where it lives

- `src/frontend/gamepad.c`, `gamepad.h`: opening/polling the pad, the mapping
  above, hot-plug, the debug line.
- `src/frontend/main.c`: merges `Gamepad_Poll()` into the same `pressed` bits the
  keyboard fills (before the edges are worked out, so pad and keys produce edges
  the same way), feeds the pad's A/B to the planet scene's `padStatus`, and
  forwards events to `Gamepad_HandleEvent()`.
