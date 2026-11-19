# The "I'm outta here" idle trigger

Standing still for three minutes makes Sonic look at the camera, wag his finger,
say "I'm outta here" and jump off the screen; the stage then ends in a game over.
Reported as missing ("nothing happens if the player stands still for 3 minutes").
The trigger turned out to be implemented and working, exactly as the reference
has it - this note records where it lives, what it needs, and the run that
verified it, so it can be checked quickly next time.

## Where it lives

| Piece | Port | Reference |
| --- | --- | --- |
| per-frame counter | `static void bye_cnt(void)` in `src/game.c` (also `src/r4/game4.c`, `src/r5/game5.c`), called from `game()` while not paused | `GAME.C:840`, `R4/GAME4.C:773`, `R5/GAME5.C:759`, called from `GAME.C:451` |
| idle check + trigger | `void bye_chk(void)` in `src/player.c` and `src/player6.c` (rounds 6 and 7), called first thing in `play00walk()` | `PLAYER.C:27`, `PLAYER6.C:24`, called from `PLAYER.C:511` / `PLAYER6.C:571` |
| the voice | `sub_sync(121)` -> `WaveRequest(78)` (`src/loader2.c`), i.e. `assets/SFX/78.wav` | `LOADER2.C:182-184` |

`bye_chk()` counts only while the player is in the engine's idle state,
`actwk[0].mstno.b.h == 5` - which `levermove()` sets when `mspeed == 0`. Any
direction held, a slide, a ride, water (`mizuflag`) or a pause resets the count
back to 0. At **10800 counted frames** (three minutes at 60 Hz) it does:

```c
byecnt0.w = 0;
actwk[0].mstno.b.h = 43;        /* the "goodbye" animation (plchg43: 17,18,19,17,20,21,22,23,164,165) */
actwk[0].sproffset |= 32768;
actwk[0].sprpri = 0;
pl_suu = 1;                     /* so the death that follows is the last life */
actwk[0].yspeed.w = -1280;      /* jump up ... */
actwk[0].xspeed.w = 256;        /* ... and away, mirrored by the facing bit */
actwk[0].mspeed.w = 0;
sub_sync(121);                  /* WaveRequest(78): "I'm outta here" */
```

`play00walk()` then waits for the animation to reach pattern 23, applies the
speeds, and normal physics carries Sonic off; the death costs the last life, so
the stage ends in the game over.

## Timing

The counter is **frames**, not wall clock: 10800 of them, which is three minutes
of *stage* time (the in-game TIME display counts the same 60 frames per second).
The port has no separate tick limiter - the loop runs at the display's refresh
rate (SDL3's default vsync) - so on a machine that renders below 60 Hz, three
minutes of wall clock is fewer than 10800 frames and the trigger comes later
proportionally. The headless run below, with software presentation at ~47 fps,
needed about 3'50" of wall clock for the stage TIME to reach 3'00".

So the thing to watch when checking is the stage's own TIME counter, not a
stopwatch:

- trigger fires at ~3'00" of stage time
- the voice plays, Sonic wags his finger and jumps
- the game over follows (lives drop to 0)

## Verification

Instrumented headless run of `./frontend round 449` (R63D) from the game
directory, `SDL_VIDEODRIVER=dummy`, no input, with a temporary
`bye_chk`/`play00` log and frame dumps:

```
bye6: t=60   mstno=5 byecnt=59   x=48 y=1580 cddat=0 mspd=0
bye6: t=120  mstno=5 byecnt=119  ...
...
bye6: t=10200 mstno=5 byecnt=10199 x=48 y=1580
bye6: t=10800 mstno=43 byecnt=0  x=48 y=1580     <- trigger, stage TIME 2'57"48
frame 11200: "GAME OVER", lives 0, stage TIME 3'01"33
```

The counter increments exactly once per frame with no resets, the animation and
jump run, `WaveRequest(78)` is issued, and the stage ends in the game over. The
same was checked in `round 400` (R11A, `player.c` instead of `player6.c`). The
installed `bin/libr11a.so` from before this session was disassembled as well: its
`bye_chk` and the `call bye_chk` in `play00walk` are identical, so no module
rebuild was needed for this feature.

Two things can make it look absent, both of which match the reference:

1. the player is not in the engine's idle state (a direction still held, a
   slope/ride/water, or the game paused) - the counter resets every such frame;
2. the three minutes are measured by the wall clock on a machine whose frame
   rate is below 60 Hz, so the stage TIME has not reached 3'00" yet.
