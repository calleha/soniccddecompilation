# Title Modules And The Menu Result Codes

The Windows frontend never calls a menu itself: it loads a menu DLL, calls its
`DLLMain()` once per frame, and acts on the value it returns
(`WINEXE/WINMAIN.C` `changeMenuState()` → `menuOrMovieLoop()`).  This documents
which of those modules the port has, how each one is driven, and the result
codes.

## Two kinds of modules

*Title style* modules export `DLLInit(engine_dll *, char ***, void **)`,
`DLLMain()`, `DLLEnd()` and `SWdataSet()`.  They draw themselves through the
HMX calls they get from the function table, so a frontend state is only needed
to swap the DLL: `src/frontend/title.c` keeps them in `FRONTEND_STATE_TITLE`.

| Module | Entry file | Port wiring |
| --- | --- | --- |
| `bin/libopening.so` | `src/title/opening/opnentry.c` | loaded at boot and after the attract mode / endings |
| `bin/libsavedata.so` | `src/title/savedata/svdentry.c` | result 4 |
| `bin/libvisualmd.so` | `src/title/visualmd/vmdentry.c` | result 6, and the movies it asks for return to it |
| `bin/libsoundtst.so` | `src/title/soundtst/sndentry.c` | result 13 |
| `bin/libstagetst.so` | `src/title/stagetst/stgentry.c` | result 14 |
| `bin/libbesttime.so` | `src/title/besttime/besentry.c` | result 20 |
| `bin/libaviopen.so` | `src/title/aviopen/aviopnen.c` | not built: the frontend plays the movies itself (see `docs/attract_mode_and_fmv.md`) |
| `bin/libavigood.so`, `bin/libavibad.so` (AVIPEN shares the code) | `src/title/avigood/avigoden.c` | not built, same reason; `src/frontend/ending.c` plays the ending movies |

*Stage style* modules export `game_init()`, `game()`, `DLL_meminit()`,
`DLL_memfree()` and `SWdataSet()` and are driven like a level: the frontend
hands out the memory and function tables, calls `game_init()` once and `game()`
once per frame, and draws the framebuffer itself.  Three of them have no entry
point at all — in the reference either (nothing in the decompilation provides
one) — so they need a small frontend state each:

| Module | Frontend state | Draw path |
| --- | --- | --- |
| `bin/libta.so` (time attack) | `src/frontend/ta.c` | rounds / special stages |
| `bin/libplanet.so` (D.A. Garden, Little Planet) | `src/frontend/planet.c` | `Graphics_LoadPlanet` / `Graphics_DrawPlanet` |
| `bin/libthanks.so` (ending message + Greatest Player) | `src/frontend/thanks.c` | `Graphics_LoadThanks` / `Graphics_DrawThanks` |

`game_info.EndingMesType` has to be set before `Thanks_Load()`: 1 for the good
ending's message, 2 for the bad ending's (the reference adds 128 in windowed
mode; only the low bits are read).

## Result codes

`changeMenuState()` maps a module's return value onto an action; the port maps
the same values in `Title_Run()` (`src/frontend/title.c`).

| Return | Reference action | Port |
| --- | --- | --- |
| 0 | keep running | keep running |
| 1 | new game | `StartNewGame()` |
| 2 | continue | `ContinueGame()` |
| 3 | `loadTimeAttack(TRUE)` | `TimeAttack_Load(1)` |
| 4 | `loadSavedata` | `bin/libsavedata.so` |
| 5 | `loadPlanet` | `Planet_Load()` |
| 6 | `loadVisualmd` | `bin/libvisualmd.so` (sets the visual mode flag) |
| 7 | attract (`gDemoId`) | `Attract_Begin()` |
| 8 | `loadAviOpen` (opening movie) | movie 0, then back to the visual mode if that is where it came from |
| 9 | `loadOpening` (or visual mode when `gbVisualmode`) | back to the opening / visual mode |
| 10 | `playBadEnding()` → the AVIGOOD module | good ending movies 4/5, then thanks (type 1) |
| 11 | `playGoodEnding()` → the AVIBAD module | bad ending movies 2/3, then thanks (type 2) |
| 12 | `loadAvipen` (pencil test) | movie 1, then back |
| 13 | `loadSoundtst` | `bin/libsoundtst.so` |
| 14 | `loadStagetst` | `bin/libstagetst.so` |
| 15 | stage chosen in the stage select | load it, and the next clear returns to the title |
| 16, 17 | `loadThanks` with `EndingMesType` 1 / 2 | handled inside `src/frontend/ending.c`, never a menu answer here |
| 18 | time attack special stage (`ta_flag = 1`, stage 477) | `Special_LoadStage(477)` |
| 19 | toggle the debug flag and reload the opening | same (`Title_DebugFlag()`) |
| 20 | `loadBesttime` | `bin/libbesttime.so` |
| 21 | exit | exit |

The naming of the endings is crossed in the reference and the port keeps the
*behaviour*, not the names: `playBadEnding()` loads the AVIGOOD module (movies 4
and 5, `END2.OGV`) while `playGoodEnding()` loads AVIBAD (movies 2 and 3,
`END1.OGV`).  The ending condition (`gEndingMovieFlag == 1` for a good clear)
lives in `src/frontend/round.c`.

Visual mode's own menu is ordered EXIT, OPENING, GOOD ENDING, BAD ENDING,
PENCIL TEST, and it answers 9, 8, 10, 11, 12 respectively — the same values, so
the table above covers it.

## Two corrections this wiring needed

* `src/title/planet/lplmain.c` declared the `LPBS.MAP` file name as `char[26]`,
  which is exactly the length of the reference's `"TITLE\PLANET\MAP\LPBS.MAP"`.
  The port needs a `"../"` prefix, so the name was silently truncated to
  `LPBS.M`; the array is now 32 bytes.  The same off by one existed in the
  `TITLE/AVIOPEN` file name arrays and is fixed there too.
* Function table slot 54 is the per pad status getter the planet module reads
  (`sPeriPadGet`).  The port had it empty (the movie hooks start at 55), which
  made the planet screen call a null pointer; slot 54 now hands out
  `FrontendPadGet()`.

## The thanks screen

`src/title/thanks` shows the ending message sprites (75 "TRY AGAIN", 76 "YOU'RE
TOO COOL!", 77 "PRESENTED BY SEGA") and then, for the good ending only
(`EndingMesType & 2` returns early otherwise), the Greatest Player screen: it
scores the names in the time attack and special stage records (3/2/1 points for
1st/2nd/3rd) and shows the winner.  The ending cutscenes this port plays already
end with the "TRY AGAIN" / "YOU'RE TOO COOL!" text, so `EndingMes()` in
`src/title/thanks/game.c` starts at the "PRESENTED BY SEGA" line instead of
drawing that text a second time.  Both screens end on a button press.

## Trying them out

The frontend takes dev entry points for the states that are otherwise only
reachable by playing:

```
./frontend planet            # D.A. Garden
./frontend thanks good       # ending message type 1
./frontend thanks bad        # ending message type 2
./frontend movie 1           # the pencil test
./frontend menu visualmd     # any title style module by name
```

Setting `FRAME_DUMP=<n>` saves the n-th presented frame to `/tmp/frame.bmp`,
which is how these screens were checked from headless runs.
