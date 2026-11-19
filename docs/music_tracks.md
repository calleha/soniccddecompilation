# Music tracks: how a `CDPlay()` number becomes a file

The modules never name a music file.  They call `CDPlay(track)` through the
function table, exactly as the reference does, and the number is a CD track
number:

```
CDPlay(track)  ->  musicTbl[track - 2]  ->  assets/Music/<file>
```

(`src/frontend/sound.c`; the table's index 0 is track 2, so the numbers line up
with the disc's track numbers and not with the array's.)  Which number a module
asks for is decided in three places:

* the title, game and menu modules call `CDPlay()` with a literal
  (`src/title/ta/ta.c`, `src/title/opening/opndo.c`, `src/title/planet/lplmain.c`,
  `src/special/game.c`, ...),
* an in-level music change goes through `sub_sync(ReqNo)` in `src/loader2.c`,
  which maps the reference's request numbers onto tracks
  (15..34 and 48..54 -> `ReqNo - 12`, 102..112 -> `ReqNo - 79`, 113 -> 2,
  114..115 -> `ReqNo - 80`, as `LOADER2.C` does),
* the sound test's D.A. list indexes the track number directly
  (`CDPlay(tblDA[nDANumberIndx])` in `src/title/soundtst/snddo.c`, and `tblDA[]`
  is 0..33).

Tracks 2..23 are the seven rounds' zone music (present / good future / bad
future), 24..33 the boss, special stage and menu music, 36..42 the past music:

| Track | File | Track | File |
| --- | --- | --- | --- |
| 2 | (none) | 28 | `ZoneComplete.ogg` |
| 3..23 | `R1A` .. `R8D` | 29 | `SpeedShoes.ogg` |
| 24 | `Boss.ogg` | 30 | `Invincibility.ogg` |
| 25 | `FinalBoss.ogg` | 31 | `GameOver.ogg` |
| 26 | `TitleScreen.ogg` | 32 | `SpecialStage.ogg` |
| 27 | `TimeAttack.ogg` | 33 | `DAGarden.ogg` |
| 36..42 | `R1B` .. `R8B` | 34, 35 | (none: the ending vocals, played from the movies' own audio) |

## The Time Attack menu

`TITLE/TA/TA.C` ends `TA_EACreate()`'s initialisation with `CDPlay(33)`, and the
port's `src/title/ta/ta.c` did the same.  CD96 shipped with the US soundtrack
only, and on that disc track 33 is the Time Attack music - which is not a tune
of its own but a *separate, identical copy* of the title screen music (track
26).

This port plays the Japanese soundtrack out of `assets/Music`, where the Time
Attack music is the pack's own `TimeAttack.ogg` at track 27 and track 33 is
`DAGarden.ogg`.  Asking for 33 therefore played the D.A. Garden music in the
Time Attack menu.

**Deviation:** `src/title/ta/ta.c` now asks for `CDPlay(27)`.  This is the only
change - the request number, nothing else - and the reason is the soundtrack
the port's asset pack carries rather than a difference in the menu's behaviour.
The alternative (moving `TimeAttack.ogg` to slot 33 in `musicTbl`) was rejected
because it would also change what track 33 plays everywhere else: the D.A.
Garden module (`src/title/planet/lplmain.c` plays track 33 once its
initialisation finishes) and the sound test's D.A. number 33 both want the
garden's music, and they keep it.

The sound test is consistent either way: D.A. number 27 now plays
`TimeAttack.ogg` and number 33 `DAGarden.ogg`, i.e. the same two tunes as
before, each under the number this pack keeps it at.

## Starting a track: the reference's CD-audio countdown

The reference's `CDPlay()` does not start anything.  It records the request,
stops whatever is playing and arms a 60-frame countdown; the main loop
decrements it and only then calls `changeMusic()`, which is what actually
switches the track (`WINEXE/WINMAIN.C`):

```c
void __stdcall CDPlay(short trackNumber) {
  gCurrentCdAudioTrack = trackNumber + -1;
  if (gUnknownCdAudioCountdown == 0) {
    cdAudio_stop();
    gUnknownCdAudioCountdown = 60;
  }
}
...
if (gUnknownCdAudioCountdown != 0 && --gUnknownCdAudioCountdown == 0 && gCurrentCdAudioTrack != -1) {
  changeMusic();
}
```

Besides the one second of silence the PC version's CD drive needed, that has a
second effect: requests arriving inside the countdown only update the pending
track, so a burst for the same track starts it **once**.  That matters here
because the D.A. Garden does not ask once.  The make-up sequence that swaps the
Little Planet's graphics calls the sub-CPU - and with it `cd_call()` - from
`lp_makeup()`'s modes 1, 2 and 4 as well as from `game()`'s case 0, so one
selection asked for the track three times.  The port started it in `CDPlay()`
back then, so the music restarted on each request.

**Deviation:** the port keeps the immediate start and drops the repeat instead.
`CDPlay()` starts the track at once unless it is the track that is *already
playing* (`track_number == currTrack && MIX_TrackPlaying()`), in which case the
request is ignored.  The countdown was implemented first and reverted: it put a
second of silence in front of every track, which pushed the title screen's music
out of step with the picture it is cued to.  Dropping the repeat fixes the
three-times restart without moving anything in time.

`CDPause(2)` - the reference's `CDPause()` - forgets the current track as it
stops it, so a request for the same track afterwards starts it again.  The
sound test and the menus do exactly that when a track is picked twice, and a
track that has played out (the one-shot jingles) is restarted the same way,
because `MIX_TrackPlaying()` is false once it has ended.

Looping is untouched by this: the `loop` column of `musicTbl` is still applied
when the track is started (`MIX_PROP_PLAY_LOOPS_NUMBER`), which is the same
choice `changeMusic()` makes from its hard-coded list of track numbers.

Verified headless from the install directory: `./frontend`, `./frontend
planet`, `./frontend ta` and `./frontend round 400` still log their single
`CDPlay(n)` and nothing complains about a missing file.  The repeat suppression
and the restart after `CDPause(2)` were checked while a temporary "starting"
log line existed (since removed, as `CDPlay()` already logs); the port's own
`CDPlay(n)` line cannot tell a dropped request from a started one.  The garden's
list-selection path itself could not be driven from a headless run (A is the
"leave the garden" button, and B and C do nothing until the list is open), so
the three-request burst is read off the module's code, which is the reference's
code unchanged.

Verified headless from the install directory:

```
SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout 4 ./frontend ta
```

logs `CDPlay(27)` and no `Couldn't read audio`, and entering the D.A. Garden
(`./frontend planet`) still logs `CDPlay(33)` without an error.
