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

## Opening / title backdrop colour (CD93 parity)

The 1993 release keeps the screen transparent black through the Sega logo, the
dark and the Sonic appearance, and only puts the title screen's dark blue
backdrop behind the title itself; the 1996 version this decompilation follows
draws the blue from the start.

The port gets the CD93 behaviour by when it builds the backdrop: `TITLE/OPENING`
grid 0 is the flat background tile (`hBmpBack`), and `src/title/opening/opndo.c`
now creates it in `OETitleSta()` (sequence 6, the title screen) instead of in
`OEOpenSta()`.  Until then `srfDraw()` skips the missing grid and the black
clear colour (`hmxBackground.color`, palette index 0) shows through.  The blue
itself is the model's (0, 0, 96) - `OPN_tblPal4[1]`, the palette entry the fork
uses - and it fades in with the title palette because the fade-in starts
drawing as soon as it runs.

Note that the colour is *drawn* content, not the clear colour: forcing
`hmxBackground.color` to another palette entry while running changes nothing on
screen, which is why this needed a module-side change rather than the fork's
`clear_screen_colour()` approach.

## Title screen flash and fade-in (CD93 parity)

Once Sonic has finished turning around on the opening screen, the 1993 release
flashes the whole screen white and then fades the title screen in from that
white, letting the colour channels come back one at a time, so the picture
washes through yellow and then red before it settles on its own colours.  Sonic
himself is the only thing that is not part of it.  The 1996 version this
decompilation follows fades towards white on palette lines 0-2 instead, which on
screen is a white flash of the middle band of the title art with Sonic
brightened along with it.

The port has the 1993 behaviour, with the total effect kept at the reference's
21 frames (`src/title/opening/opndo.c`):

* `OEFlashPalette()` writes **all four** palette lines from `tblPal1..4`, which
  is what makes the flash cover the whole screen - the tiled backdrop, the
  title art and its sprites (Little Planet included, as it shares Sonic's
  palette line).  The first three frames flood the screen with `FLASH_BRIGHT`
  (224, the value the reference's fade tops out at), then 18 frames fade back.
* The fade releases the channels in turn (`OEFlashChannel()`): blue returns to
  its own value over the first six frames (white turns yellow), green over the
  next six (yellow turns red) and red over the last six, which is the opening's
  own palette again on the last frame, so the hand-over to the title screen
  cannot be seen.
* Sonic is drawn from copies of his sprites instead, whose colours live at
  palette entries 144-159 rather than in line 3 (64-79), so the flash never
  touches them: `OEFlashSpritesCreate()` (called from `DLLInit()`) copies the
  hand frames, eyes, nose and held pose through `OESprBmpDuplicateUnfaded()`
  (`src/title/common/hmx_oeeactl.c`), and `OEFadeIn()` points those sprites at
  the copies while the flash runs.  The frontend keeps the colours - a snapshot
  of the fourth palette line taken when the title module was initialised
  (`Graphics_TitleSnapshotUnfadedPalette()`) - because line 3 is what the flash
  overwrites (`src/frontend/graphics.cpp`, `UpdatePalette()`).
* The art leaves holes in Sonic's sprites: his pupils and nose are transparent
  and show the artwork behind his face, which is why a palette flash shows
  through him.  `OESprBmpDuplicateUnfaded()` therefore also closes the
  transparent pixels that a sprite does not open onto its own border, using the
  line's black.
* `OEFadeIn()` shows the eyes and nose sprites (timing 12) as well as the pose
  and the raised hand, so his face is complete while the flash runs.

The effect is 3 white frames plus 18 fade frames, i.e. the same 21 frames as
the reference's fade; the fork in `soniccdrespinned` spreads the same idea over
4 + 21.

## Title screen arm blink (CD93 parity)

The reference's `OEUpdateHand()` guards its first pose with `if (num != 0)`, so
Sonic's raised hand is drawn on every title frame except the very first one,
where it disappears - the arm blinks just before the finger wave starts.
`OEUpdateHand()` in `src/title/opening/opndo.c` now draws that pose like the
rest (the wave timing is untouched).

Checked the way the fork did, by counting the glove's white pixels in the hand
box (x 161-220, y 58-121) per frame of a headless capture of the transition: the
first title frame had 172 instead of 305 before the fix, and 305 on every frame
after it.

## No stray pixels along the sprites' right edge (CD93 parity)

`TITLE/BMP/OPENINGS.CM_` pads every sprite row to a multiple of eight pixels and
does not always leave that padding zero, so a loader that copies whole padded
rows draws it: the port showed single stray pixels along the right edge of some
sprites, of which the two most visible were a light-grey and a white one at
(221, 121) and (223, 121) - the bottom right of the hand frames - and a peach
pair at (184, 80) and (185, 80) next to Sonic's face.  Their colour and whether
they appeared depended on which sprite was on screen, so some of them only showed
on some frames of the finger wave (the fork lists the same artefact as "stray
white padding pixels" and fixed it by clipping the opening's sprites to their
real width, `read_bitmap_clipped()`).

`ld_bitmap_4to8_clipped()` in `src/title/common/hmx_oeeactl.c` is
`ld_bitmap_4to8_1()` with the padded row width as a separate stride, and
`ld_load_sprite2()` now builds each opening bitmap at the sprite's own width
(`infoSprtBmp[i].w`) instead of the padded one, so the padding is never drawn.  The
two bitmaps that come from BMP files (`1996SEGA.BMP`, `EXIT.BMP`) keep their
sizes.  Comparing frame dumps before and after the change over the whole title
sequence, only six pixels differ and all of them were padding data.
