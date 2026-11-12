# Attract Mode and the FMV Cutscenes

This documents how the port reproduces the Windows frontend's "leave the title
screen alone and it plays the opening movie, then a demo, then another demo"
behaviour, and how the movies are decoded.

## Where the attract mode starts

Nothing happens at boot: the game opens on the title screen (the port loads
`bin/libopening.so`, the same `TITLE/OPENING` sequence the original frontend
did). After 900 title ticks without input, `src/title/opening/opndo.c` sets
`nMenuKind = 7` and `DLLMain()` returns `7` (see
`docs/title_opening_dllmain_returns.md`).

`src/frontend/title.c` maps that to `TITLE_RESULT_ATTRACT`, so it calls
`Attract_Begin()` and switches to `FRONTEND_STATE_ATTRACT`. The reference does
the same thing from `menuOrMovieLoop()` in `WINEXE/WINMAIN.C`: `changeMenuState()`
turns the opening's `7` into `5`, and `case 5` is the attract case.

## The rotation

The reference keeps a counter (`gDemoId`) that starts at 0 and is bumped after
every attract item, wrapping at 5. Entry 0 is the opening movie; entries 1-5 are
recorded stage demos. `src/frontend/attract.c` keeps the same order:

| Entry | Item | Stage id | Recording | `play_start` |
| --- | --- | --- | --- | --- |
| 0 | Opening movie | - | (movie file) | - |
| 1 | R11A demo | 400 | `KEY11A.BIN` | 0 |
| 2 | Special stage 1 | 470 | `SPECIAL1.BIN` | 0 |
| 3 | R43C demo | 428 | `KEY43C.BIN` | 2 |
| 4 | Special stage 6 | 475 | `SPECIAL6.BIN` | 0 |
| 5 | R82A demo | 464 | `KEY82A.BIN` | 2 |

The stage ids are the `ID_STAGE_*` values from `src/winexe/resource.h`. Each
item, when it finishes, returns to a freshly started title sequence, so the next
idle period plays the next entry; after entry 5 the rotation wraps back to the
movie.

Recordings are the original 4-bytes-per-frame pad data (held/pressed for each
pad, `0xFFFF` marks the end), fed to `SWdataSet()` instead of the live pad by
`Attract_OverridePad()`. A / B / Start on the real pad ends the demo, as in the
original.

## Movies

The movie files are decoded in-process with FFmpeg's libav* libraries
(`src/frontend/movie.c`). CMake looks for FFmpeg in this order:

1. `-DFFMPEG_ROOT=<prefix>`
2. `pkg-config libavformat libavcodec libavutil libswscale libswresample`
3. the `ffmpeg` binary on `PATH` (its prefix is used for headers and libraries)

`-DSONICCD_WITH_FFMPEG=OFF`, or a build where FFmpeg is not found, compiles the
player into no-ops: the movie entries are then skipped and only the demos play.

The reference's movie DLLs (`TITLE/AVIOPEN`, `TITLE/AVIGOOD`, `TITLE/AVIPEN`)
ask the frontend to play a movie by number and poll it for completion. The
frontend keeps that ABI (the three hooks sit in `functionTbl` right after
`hmx_ddagrid_set_tile`) but the player is non-blocking:

| Movie id | Meaning | Files tried in order |
| --- | --- | --- |
| 0 | Opening | `TITLE/AVI/OPN.OGV`, `TITLE/AVI/OPN.AVI`, `OPENING.SFD` |
| 1 | Pencil test | `TITLE/AVI/PENCIL.OGV`, `TITLE/AVI/PENCIL.AVI`, `SENGA.SFD` |
| 2 | Bad ending part 1 | `TITLE/AVI/END1.OGV`, `ENDING1_1.SFD` |
| 3 | Bad ending part 2 | `ENDING1_2.SFD` |
| 4 | Good ending part 1 | `TITLE/AVI/END2.OGV`, `ENDING2_1.SFD` |
| 5 | Good ending part 2 | `ENDING2_2.SFD` |

`Movie_Stat()` returns `3` ("nothing playing") once the file ends or was never
started, because that is what makes the DLLs advance to the next part.

The endings exist in two shapes: one combined file holding the whole ending
(the OGV files, credits included), or the Mega-CD era pair of SFD halves. Part 1
prefers the combined file, and part 2 only plays when part 1 came from the
split files, so an ending never runs its second half twice.


Frames are scaled to fit the game's 320x224 screen with their aspect ratio kept
and black bars around them, then presented from the main loop; while a movie
runs, `PresentFrame()` leaves the screen to the movie and the waiting game code
keeps running underneath.

Audio is decoded with libavcodec and resampled to 48 kHz stereo for an SDL
audio stream. The OGV files carry two complete soundtracks for the same picture
(the JP and the US music) with identical metadata, so the track is chosen by
position: `MOVIE_AUDIO_STREAM_INDEX` in `src/frontend/movie.c` (0 is the first).

## How the stage demos stay in sync

The recordings are pad data only, so a demo reproduces the original only if the
frontend feeds the recording at the rate the module consumes frames *and* the
module's own state matches. Two things matter here:

- **Recording rate.** One recorded frame per module frame. Rounds advance once
  per vblank (60 Hz) and special stages once per three (20 Hz, see the
  `specialStageLoop()` note in `src/frontend/graphics.cpp`), and the original
  recorded the pad on exactly the same cadence, so `SPECIAL1.BIN`'s 510 frames
  are 25.6 s of special-stage time, not 8.5 s.
- **The modules' own `random()`.** The special stage (`src/special/etc_s.c`) and
  the thanks screen have functions whose names also exist in libc. A shared
  object loaded after libc gets its internal calls bound to libc's version, so
  the special stage's `random()` (and its `ranum` seed) were never used and the
  attract demos - which were recorded against the original generator - drifted
  apart after the first few UFOs. The variant libraries are therefore linked
  with `-Wl,-Bsymbolic-functions`; keep that when adding module targets.

## The endings

Clearing the last act of R8 ends the game. The reference frontend
(`WINEXE/WINMAIN.C`, the "the stage game function returned 2" branch) then
decides which ending to show and plays it as a two-part movie:

- good ending when every Time Stone was collected (`clrspflg_save == 127`) or
  every zone reached act 3 in a good future (`clrgood == 127`) - movies 4 and 5,
  `TITLE/AVI/END2.OGV`
- bad ending otherwise - movies 2 and 3, `TITLE/AVI/END1.OGV`

`clrgood` is credited the way the reference does it: when a zone's act 1
starts, `DAT_0042570c` in the original - here a flag set from the "?3C" stage
id at load time - says whether the act 3 just played ran in a good future.

`src/frontend/ending.c` runs that sequence. After the movies the thanks module
takes over (see `docs/title_modules.md`): it shows the "PRESENTED BY SEGA" line
and, for the good ending only, the Greatest Player screen, then the title
sequence starts again. One difference from the reference: the "TRY AGAIN" /
"YOU'RE TOO COOL!" line is not drawn again there, because the ending cutscenes
this port plays already end with it.

## Trying it out

The frontend accepts the attract entries and the endings directly:

```
./frontend attract        # start the rotation at entry 0 (the opening movie)
./frontend attract 3      # start it at entry 3 (the R43C demo)
./frontend ending good    # play the good ending
./frontend ending bad     # play the bad ending
```

Each attract item still ends the way it does in the rotation, so this also walks
the rest of the sequence without waiting on the title screen.
