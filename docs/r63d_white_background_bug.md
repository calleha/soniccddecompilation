# Bug: R63C/R63D's backdrop was white after the boss, then stopped flashing

Two reports about the same index, `colorwk[32]` - the round backdrop:

1. after the R6 boss dies (screen flash + fade-in), the background was white
   instead of black, until the stage ended and the next one reloaded the palette;
2. with that worked around, the boss's white flash stopped covering the
   background at all.

The module code matches the reference exactly; both bugs were in how the port
picked the colour it clears the framebuffer with.

## The reference's backdrop

`BackgroundBMPCreate()` (`WINEXE/GRAPHICS.C:810`) builds the 320x224 bitmap that
is drawn behind everything (`gpBackground`, draw order 7000) and fills it with a
plain *surface* value:

```c
if ((gbSpecialStageLoaded && gKeepWork.SPEMode != 0) || gbWarpLoaded)
  *local_8 = 0x10101010;   /* 16: warp and special stages */
else
  *local_8 = 0x30303030;   /* 48: rounds, planet, thanks */
```

A surface value is not a game palette index. `makeFullScreenPalette()`
(`WINEXE/GRAPHICS.C:2175-2199`, the mode the game runs in) puts
`gpColorwk[0..63]` at surface entries **16-79**, and `FUN_0040653a()` loads
tiles and sprites the same way - `param_5 + pixel` with `param_5` 16/32/48/64
for the four palette lines, which is why sprite line 0 lands on `colorwk[0..15]`
at surface 16-31. So the round backdrop is:

```
surface 0x30 = 48  ->  colorwk[48 - 16] = colorwk[32]
```

For R63D that entry is `zone8colora[16]` (`mapinittbl.colorno2 = 4` selects
`colortbl[4]`, `zone8colora` at offset 16), which is `{0,0,0}` - black, as
reported. For the warp and special stages the backdrop is surface 16, i.e.
`colorwk[0]`.

## Why the port got it wrong

The port's own palette maps `colorwk[k]` to screen entry `k` (there is no 16
offset to keep), but its clear colour was transcribed from the reference's
surface value:

```c
SDL_FillSurfaceRect(framebuffer, NULL, 48);   /* colorwk[48], not colorwk[32] */
```

Tile and sprite surfaces use index 0 as their colour key, so every transparent
tile pixel of a round shows this colour - and that is exactly the backdrop, since
the reference's grid code drops empty cells (`SetGrid()`, `index == 0` ->
`_grdClearData()`) and lets the backdrop show through.

That explains both reports, because `colorwk[48]` and `colorwk[32]` behave very
differently around the boss (`R6/BOSS_6.C` `egg6_dead2()`, `R6/COL6D.C`):

| | `colorwk[32]` (reference backdrop) | `colorwk[48]` (port's) |
| --- | --- | --- |
| normal | black | black |
| `flashout()` whitens 0-63 | white | white |
| `flashin_boss6()` restores only 0-47 | restored | **left white** (report 1) |
| `fset_boss6(0)` at the end of `enkei_in()` | painted black | untouched |

`flashin_boss6()` aims the restore at entries 0-47 (`colorcnt = 47`) and
`flashin_new()` re-whitens entry 32 on every step on purpose, so nothing brings
`colorwk[48]` back until the next stage's `colorset()`/`mapinit()` - "it corrects
itself when the stage finishes".

The first fix here cleared with a reserved index the game never writes (`200`).
That kept the backdrop black, but the backdrop then no longer followed the
palette, so the flash left it black - report 2. The clear has to be a *game*
entry: the reference's flash covers the backdrop precisely because the backdrop
is a game colour.

## The fix

`src/frontend/graphics.cpp` clears the round framebuffer with the entry the
reference's backdrop resolves to:

```c
#define FRAMEBUFFER_CLEAR_INDEX 32   /* colorwk[32] = reference surface 0x30 - 16 */
...
SDL_FillSurfaceRect(framebuffer, NULL, FRAMEBUFFER_CLEAR_INDEX);
```

With that, the backdrop behaves exactly like the reference's: black normally,
white throughout the boss flash, restored by the fade-in and painted back to
black by the boss's own colour animation (`fset_boss6(0)` ends on
`boss6_col4[0] = {0,0,0}`). In R4's water region the R4 palette split moves it to
`colorwk3[32]`, which is the reference's `makeFullScreenPalette()` water range
(surface 80-143 = `gpColorwk3[0..63]`) as well.

## The other clear colours (same transcription, unreported)

The port fills the other screens with the same transcribed surface values, so
they are 16 entries too high. Per the derivation above the reference's values
resolve to:

| Screen | Port now | Reference surface | Equivalent `colorwk` entry |
| --- | --- | --- | --- |
| round (this bug) | 32 | 0x30 | 32 |
| warp | 16 | 0x10 | 0 |
| special | 16 | 0x10 | 0 |
| planet | 48 | 0x30 | 32 |
| thanks | 16 | 0x30 | 32 |

Only the round one is fixed here, because only that one was reported and
verified; the others are `colorwk[0]`/`colorwk[16]` in practice all black, so
they show no symptom until something repaints those entries.

## Verification

- `tests/backend/unit/r6/test_col6d.c`,
  `test_boss_flash_repaints_the_backdrop_entry()`: runs `flashout()` and
  `flashin_boss6()` through `FadeProc()` against real palettes and then
  `fset_boss6(0)`. It pins down that the flash whitens `colorwk[32]`, that the
  fade-in keeps it white (and that entry 48 is never restored), and that
  `fset_boss6(0)` paints the backdrop back to black.
- `frontend` target builds clean, no new warnings.
- Not verified in-game: the R6 boss needs a playthrough, so this is verified
  against the reference's backdrop/palette code and the palette flow, not by
  playing the stage.
