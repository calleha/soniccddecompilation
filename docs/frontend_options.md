# Frontend launch options

The frontend takes the dev entry points listed in `docs/title_modules.md` and
`docs/attract_mode_and_fmv.md` (`round`, `special`, `attract`, `planet`,
`thanks`, `movie`, `menu`, ...).  On top of those it has a few switches, which
can be given anywhere on the command line:

| Option | Effect |
| --- | --- |
| `--scanlines` | Draw a soft scanline filter over the game's picture |
| `--timewarp-noskip` | Play the time warp transition (it is skipped by default) |

## Time warp (`--timewarp-noskip`)

Touching a Past/Future sign and keeping the speed up warps Sonic to the other
period.  The stage code picks the new period itself (`time_flag` with `0x80`,
the reference's `GAMEFLAG_NEXT_TIME`) and the frontend used to answer that by
loading the warp module, `bin/libwarp.so` (`src/warp/warp.c`): the seven-second
transition with Sonic spinning and the warp palette, which a button press cuts
short.  The fork in `soniccdrespinned` makes that optional and skips it by
default (its "Skip time warp" setting), and the port does the same.

`Round_Run()` in `src/frontend/round.c` therefore goes straight to
`Round_LoadStageByMenu(0)` - exactly the load `Warp_Run()` does when the
transition is over - instead of loading the warp module, so the stage in the
other period starts immediately.  The period, the music and the stage itself are
the same either way: the transition only animates.  `--timewarp-noskip` brings
the transition back for a run:

```
./frontend round 11A --timewarp-noskip   # warps play the full transition
```

The `warp` dev entry (`./frontend warp`) still loads the transition directly.

## Scanline filter (`--scanlines`)

```
./frontend --scanlines            # the game as usual, with scanlines
./frontend round 11A --scanlines  # the switches can follow a dev entry too
```

`ApplyScanlines()` in `src/frontend/graphics.cpp` is called from the present
step (`PresentFrame()`): every odd output row has its RGB scaled by three
quarters, alpha untouched, which reads as a soft darkening between lines rather
than the hard black gaps of a CRT shader.  The fork in `soniccdrespinned` applies
the same thing in its own `blit_screen()`, with `* 3 >> 2` as the single knob -
`* 4 >> 3` (half) gives a much heavier look, `* 7 >> 3` (seven eighths) a lighter
one.

Because it lives in the present step it covers every screen the game draws - the
opening and title screens, the menus, the stages, the special stages, Time
Attack, the planet and the thanks screen - and nothing else.

The FMV cutscenes are deliberately left unfiltered: they are presented by
`src/frontend/movie.c` through their own texture, and `PresentFrame()` returns
early while a movie is playing (`Movie_IsPlaying()`), so the scaler output
reaches the screen untouched, letterbox bars included.

Checked from headless runs by dumping frames with `FRAME_DUMP` and comparing row
brightness: with the switch on, every odd row is at 3/4 of its own value (a row
of 192,192,224 becomes 144,144,168) and every even row is untouched, while movie
frames show no difference between even and odd rows.
