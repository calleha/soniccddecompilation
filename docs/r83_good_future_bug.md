# Bug: R83 came out as the good future (C) without the good ending earned

Reported from a full playthrough: good futures only in round 8 (Metallic
Madness), bad ending, but the final stage loaded as **R83C** (good future)
instead of R83D.

Status: **fixed** - as a deliberate deviation from the reference. It is the one
place where the reference does not give the result the game is supposed to
produce.

(Terminology: this game calls the eight levels *rounds* (R1-R8, `stageno.b.h`)
and their three parts *zones* (1-3, `stageno.b.l`).)

## What the reference does

The end-of-zone routine `clear_move2()` (`ZONE.C:687-740`, port `src/zone.c`)
keeps a two-bit mask in `gf_flag`, one bit per zone of the round that was just
played:

```
advance stageno (zone 3 of a round rolls over to zone 1 of the next round)
if (zone == 1) { gf_flag = 0; return; }          /* new round: mask reset */
if (!ta_flag && clrspflg_save != 127) {
  if (!generate_flag) return;                    /* no generator destroyed */
  generate_flag = 0;
  gf_flag |= 1 << stageno.b.l - 1;               /* bit 0 for zone 2, bit 1 for zone 3 */
  if (gf_flag != 3) return;                      /* both zones of the round needed */
}
generate_flag = 1;                               /* the round's zone 3 is the good future */
```

`gf_flag` is reset at the first zone of *every* round (and again by the
frontend's `setStage(0)` mirror, `Round_SetCurrentStage()`), so the two bits can
only ever come from the round being played. Round 8 therefore reaches its good
future when both of its robot generators are destroyed - which the reporter had
done - and the reference then loads `83C` even though the other seven rounds
were bad futures: the bad future of a round is not a requirement for anything.

Every other link of that chain was checked line by line and really is identical
to the reference, so none of it is the cause:

| port | reference | what it does |
| --- | --- | --- |
| `src/zone.c` `clear_move2()` | `ZONE.C:687-740` | the `gf_flag`/`generate_flag` logic above |
| `src/game.c` `game_init()` / `game()` | `GAME.C:262/400/475-505` | `gf_flag` restored from and written back to the keep work; `gf_flag = 0` only for a fresh game; `generate_flag = 1` for all Time Stones |
| `src/frontend/round.c` `Round_LoadNextStage()` | `WINEXE/WINMAIN.C:606-650` | credits `clrgood` when the next round's first zone starts, `stageno.w == 0x0700` -> ending |
| `src/frontend/round.c` `Round_ReadStageString()` | `WINEXE/WINMAIN.C:2487-2525` `readStageString()`, `LOADER2.C:21` `GetRoundStr()` | `?3C` iff `generate_flag != 0` |
| `src/r8/et8.c` | `R8/ET8.C` | the robot generator: sets `generate_flag = 1` only when it is destroyed in the past (`generate_flag == 0 && time_flag == 0`) |
| `src/r8/emie8.c` | `R8/EMIE8.C` | the final boss's exit: `gameflag.w = 2`, `stageno.w = 1792`, `time_flag \|= 128` |
| `src/r8/edtbl81/82/83.c` | `R8/EDTBL8x.C` | round 8's object tables, including zone 2's generator |
| `src/r8/*` `stageno` uses | `R8/*.C` | the same set of conditions; nothing refers to `0x0602`/1538 in either tree |

## Why it is a deviation

The final stage's time period is the game's way of telling the player which
ending they are about to get: `83C` means the good ending is earned, `83D` means
it is not. In the original game `83C` is unreachable unless one of the two
good-ending conditions already holds - all seven Time Stones (`clrspflg_save ==
127`), or a good future in every round (`clrgood == 127`). The reference has no
such test anywhere, so this port adds it, in the same place the game itself
would have to keep it: the frontend that picks the stage id.

Note the second condition is already satisfied whenever round 8's own good
future is earned *and* every earlier round earned one, so the test is the ending
test minus round 8's own bit, which is only credited after this zone is cleared:

```
good ending earned = clrspflg_save == 127
                  || (clrgood | 1 << 6) == 127      /* R8 is bit 6 */
```

When it does not hold, the good future of round 8's zone 3 is dropped even
though `gf_flag` reached 3. Everything else about `gf_flag`, `clrgood` and the
ending stays exactly as the reference has it.

## The fix

`src/frontend/round.c`:

- `Round_ClearLastZoneGoodFuture()` - when the stage about to be loaded is
  `stageno.b.h == 6 && stageno.b.l == 2` in the future (`time_flag == 2`) with
  `generate_flag` set, and neither good-ending condition holds, it clears
  `generate_flag`.
- It is called from `Round_LoadNextStage()`, before the next stage is loaded -
  only the "zone cleared, load the next one" path, never the time warp or a
  return from a special stage, which reload the stage the player is already in.
- The stage select (`setStage(id != 0)`, `Round_LoadStageByName()`) still loads
  any variant directly, as before.

Clearing `generate_flag` is enough on its own, because every user of it reads
the keep work after that point:

- `Round_ReadStageString()` builds the stage id from it, so the module loaded is
  `bin/libr83d.so` instead of `libr83c.so`;
- the module's `game_init()` copies the same value in (`clrspflg_save == 127` is
  excluded, so it cannot put it back), and its `GetRoundStr()` calls in
  `ReadBlockMap()`, `ReadScrolMap()` and the tile-change path then read
  `R8/83D/...`, and so does `Round_LoadGraphics()`;
- the frontend's `roundStageIsGoodFuture` (the reference's `DAT_0042570c`) sees
  "?3D", so round 8's `clrgood` bit is not credited and the ending stays bad.

With all seven Time Stones the reference already forces `generate_flag = 1` on
every clear (`clear_move2()` skips the mask block entirely), so condition 1 needs
no extra handling - `83C` and the good ending follow as before.

## Verification

- Builds clean (`frontend` target, no new warnings).
- Walked the reported run by hand through the flags: `81C`/`82C` credits
  `gf_flag` 1 then 3 and leaves `generate_flag = 1`; `Round_LoadNextStage()`
  then sees `clrspflg_save != 127` and `clrgood == 0`, clears `generate_flag`,
  and the load resolves to `83D`; the final boss's exit credits no `clrgood` bit
  and `Ending_Begin()` picks the bad ending.
- Walked the earned runs too: `clrgood == 0x3F` (all six earlier rounds good) or
  `clrspflg_save == 127` keeps `generate_flag`, loads `83C`, credits bit 6 and
  the ending is good.
- Not reproduced in-game: no automated way to drive a playthrough through zones
  1 and 2 of round 8, so the fix is verified against the flag flow, not by
  playing.
