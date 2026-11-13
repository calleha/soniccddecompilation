# Frontend launch options

The frontend takes the dev entry points listed in `docs/title_modules.md` and
`docs/attract_mode_and_fmv.md` (`round`, `special`, `attract`, `planet`,
`thanks`, `movie`, `menu`, ...).  On top of those it has a few switches, which
can be given anywhere on the command line:

| Option | Effect |
| --- | --- |
| `--scanlines` | Draw a soft scanline filter over the game's picture |

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
