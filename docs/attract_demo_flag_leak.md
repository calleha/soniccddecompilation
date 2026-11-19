# Bug: R82A started at the demo's position in a real playthrough

Reported after watching the R82A title demo: in the next playthrough, R82A
started at the spot the demo starts from (6942, 1484) instead of the act's start
(46, 1164). Not reproducible on demand, but the flag that causes it is easy to
follow.

## How R82A picks its start

`R8/SCR82A.C:275` (`playposiset()`, port `src/r8/scr82a.c`) is the only stage
code that takes its start position from the *keep work*:

```c
if (lpKeepWork->demoflag.w) {          /* any non-zero value */
  pPositbl = &playpositbl[1][0];        /* { 6942, 1484 } - the demo's start */
} else if (plflag) { ... }
else pPositbl = &playpositbl[0][0];     /* { 46, 1164 } - the act's start */
```

The frame that plays the R82A demo is started by the attract mode, which marks
the keep work (`WINEXE/WINMAIN.C:213`):

```c
gKeepWork.demoflag.w = -1;
gbRecordPlayFlag = TRUE;
gKeepWork.play_start = 0;
```

so during a demo `demoflag.w` is -1 and R82A's player begins where the recording
begins. `game_init()` only clears the *module's* `demoflag` (`EQU.C:9`), not this
keep work copy, which is what makes the stage code read the demo start at all.

The reference drops the marker when the demo ends, in two places next to each
other (`WINEXE/WINMAIN.C:331-334` and `:572-575`):

```c
if (gbRecordPlayFlag == FALSE && gKeepWork.demoflag.w == -1) {
  gKeepWork.demoflag.w = 0;
  restartGame();
  return TRUE;
}
```

and `restartGame()` (`WINEXE/WINMAIN.C:1180`) clears it again before it reloads
the opening.

## The leak

`src/frontend/attract.c` set `gameInfo.demoflag.w = -1` when a demo started, but
nothing ever set it back: `Attract_Run()`'s exit paths only call
`Title_Restart()`, which reloaded `bin/libopening.so` and left the keep work
alone. The -1 therefore survived the title screen, the new game and every stage
up to R82A, where `playposiset()` read it and used the demo start.

## The fix

`src/frontend/title.c`:

- `Title_Restart()` clears `gameInfo.demoflag.w` before reloading the opening -
  this is the port's equivalent of the reference's `restartGame()`, and it is
  where the attract mode always returns to the title.
- `StartNewGame()` and `ContinueGame()` clear it too, as a guard: a demo can also
  end in a game over (`Round_HandlePlayerDeath()` unloads the round and loads the
  opening directly, without going through `Title_Restart()`), and no game should
  ever begin as a demo.

`src/frontend/attract.c`'s comment explaining the marker was corrected at the
same time - it claimed the stage DLLs reset it in `game_init()`, which is only
true of the module's own copy.

## Verification

- `frontend` builds clean, no new warnings or diagnostics.
- The stage half of the contract is already covered by
  `tests/backend/unit/r8/test_scr82a.c`,
  `test_scr82a_scr_set_uses_keep_work_demo_position()`: with the keep work's
  `demoflag.w` non-zero, `scr_set()` puts the player at 6942/1484 and the scroll
  at 6782/1388.
- Not reproduced in-game (the report says it was not reproducible): the frontend
  has no test harness, so the fix was verified by following the flag through
  `attract.c` -> `Title_Restart()`/`StartNewGame()` and the `scr82a` unit test,
  not by watching a playthrough.
