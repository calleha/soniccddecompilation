# Restarting the stage with the three jump buttons

Pressing **A + B + C together** (all three jump buttons) restarts the stage, the
way the original does, and it works while the game is paused.  The stage module
has the whole feature and it was left exactly as the reference has it.  What it
needs is input that can deliver three button-downs in one frame: a **gamepad**
does, and the frontend now supports one (`docs/gamepad_input.md`); a keyboard
cannot, so the keyboard workaround is parked as a patch (below).

## What the game does with it

`src/game.c` `game()` (reference `GAME.C:360-374`) has the give-up branch:

```c
if ((swdata1.b.l & 112) == 112) {     /* all three jump buttons, pressed this frame */
  pauseflag.b.h = 0;                  /* unpause */
  markerno = 0;
  demoflag.w = 0;
  gameflag.w = 1;                     /* answer the frontend with "the player is out" */
  lpKeepWork->ta_time = -1;
  --pl_suu;                           /* it costs a life */
  if ((char)pl_suu < 0) pl_suu = 0;
  sub_sync(14);
  flagwkclr();
  plflag = 0;
}
```

The frontend's death handler (`Round_HandlePlayerDeath()` in
`src/frontend/round.c`, `WINEXE/WINMAIN.C:610-650` in the reference) then reloads
the stage through `game_init()` when a life is left, and drops to the opening
when there is not - so with the last life this combination ends the game instead
of restarting.  The check does not care whether the game is paused: the branch
clears the pause flag itself.

## Why it cannot be triggered here

The check is on the **edge** bits (`swdata1.b.l`, the buttons that went down this
frame), so all three have to be pressed inside the same 16.7 ms frame.

- A **pad** can do that: the three jump buttons sit under one thumb and are
  sampled together in one 60 Hz poll.  The reference frontend reads a joystick
  (`setupJoystick()`, `JOYINFOEX`, `gJoystickInput1..4`, merged into `swdata1` in
  `readController`), so on a controller the restart works with no special help.
  The reference also feeds exactly this pad state itself when its own restart flag
  is set (`gbRestartStage`, from Alt+F2 and the "Restart Stage" debug menu entry,
  `WINEXE/WINMAIN.C:2418-2426`: `swdata1 = 0x70`).
- This port's frontend reads **only the keyboard** (`SDL_GetKeyboardState()` in
  `src/frontend/main.c`; the joystick code that exists is in the decompiled
  legacy `src/winexe/`, which the port does not run).  Key-down edges for three
  keys practically never share a frame - whichever key lands first has its edge
  in an earlier one - so the branch never runs, and pressing the buttons appears
  to do nothing.

## With a pad, and with a keyboard

**A gamepad works as the original does**: the frontend merges SDL3 gamepad
buttons into the same pad bits as the keyboard (`docs/gamepad_input.md`), and a
pad delivers all three button-downs in one poll, so the module's own check fires.
On the 8BitDo M30, the pad's own A, B and C reach the game's A, B and C, so the
combination is the pad's A+B+C.

**A keyboard still cannot**, because three key-down edges almost never share a
16.7 ms frame.  The workaround for that - feeding one synthetic `A+B+C` frame as
soon as the three jump buttons are *held*, once per press - is kept as a patch,
not applied:

```
patches/restart_stage_keyboard.patch      apply with: patch -p1 < patches/restart_stage_keyboard.patch
```

## Verification

- The module's path was confirmed by injecting a same-frame A+B+C into the pad: a
  headless `round 400` logged one `round result 1` and reloaded `R1/11A` again -
  the give-up branch and the restart both work.
- With the patch applied and the three buttons *held* for 80 frames, exactly one
  restart happened (a latch keeps it from looping); unmodified runs never restart.
- The patch applies and reverses cleanly against this tree
  (`patch -p1 [--dry-run]` and `git apply --check`).
