# Movie sound through the game's mixer

Status: **implemented**.  The cutscene sound now runs through the game's one
audio device and stays with the picture; the old SDL audio stream is still
reachable with `SONICCD_MOVIE_STREAM_AUDIO=1` for comparing the two by ear.

## The problem it fixes

In every cutscene (opening, pencil test, endings) the sound ran about **one
second behind** the picture, and the first seconds of sound dropped out a couple
of times.

Two separate causes, both measured with `MOVIE_DEBUG=1`:

1. **Two audio devices.**  `movie.c` opened an SDL audio stream of its own beside
   the game's SDL3_mixer device, so the film had its own queue and its own
   latency, and it started life empty (the dropouts).
2. **The file's interleave.**  The films put their sound well behind their
   picture *in the file*: in `TITLE/AVI/OPN.OGV` the first ~0.6 s of picture
   comes before any audio packet at all.  Reading the file in step with the
   picture therefore cannot have the film's first sound in hand when the picture
   starts, so the soundtrack began that much later than the picture - and that
   delay *is* the ~1 s the sound trailed by.  Timing the picture from the audio
   device instead (tried in an earlier session) does not help with this and made
   the picture stutter while the device span up.

## What was built

**One device, one track** (`src/frontend/sound.c`, `sound.h`):

- `Sound_BeginMovieAudio(io, spec)` gives a dedicated movie track
  (`MIX_CreateTrack`) the film's raw PCM as an `SDL_IOStream`
  (`MIX_SetTrackRawIOStream`), after stopping whatever it was playing;
- `Sound_PlayMovieAudio()` starts it, called when the first frame is on screen so
  the sound and the picture start together;
- `Sound_GetMovieAudioPosition()` reads the track's played sample frames, which
  is what the debug line reports;
- `Sound_EndMovieAudio()` stops the track and detaches the stream before the
  movie frees it.  Nothing of the last cutscene can be heard over the next one,
  and the music track is untouched (`CDPause(2)` still owns that).

**A ring buffer, presented as a stream** (`src/frontend/movie.c`):

- decoded 44.1 kHz / S16 / stereo goes into a 1 MB ring (about 5.9 s), guarded by
  a mutex because the mixer reads it from its own thread.  It has separate read
  and write positions with the amount in flight between them, and only whole
  sample frames are ever written;
- `Movie_AudioRead()` answers every read in full, padding with silence when the
  decoder has not caught up (a dry patch must not look like the end of the
  stream, or the track stops).  Only when the picture is over *and* the ring has
  drained does it report end of stream, which is what ends the track;
- `Movie_AudioSeek()` accepts only "seek to the start", which is what the mixer
  does before playing; `Movie_AudioSize()` reports an unknown length, because the
  film decides when it ends.

> The first cut of this ring advanced a *single* index for both ends, so the
> decoder overwrote sound the mixer had not read yet and the fill bookkeeping
> drifted.  On playtesting that was constant stuttering, artifacts, and a
> soundtrack that sped up and slowed down.  The two positions are separate now -
> if that sound ever comes back, look there first.

**Reading ahead** (`Movie_ReadPacket`, `Movie_NextVideoPacket`, `Movie_ReadAhead`):

- packets are read ahead of the picture: audio goes straight into the ring, video
  packets go into a small queue (`MOVIE_VIDEO_QUEUE_MAX` = 240 packets, eight
  seconds at 30 fps) so the film still shows every frame, in order;
- before the first frame is shown the ring is filled to
  `MOVIE_AUDIO_PREFILL_MS` (2 s), which is larger than the interleave lead, so
  the film's first sound is in hand when the picture starts;
- every frame afterwards it is topped up to `MOVIE_AUDIO_AHEAD_MS` (1 s) so the
  margin never reaches zero;
- the old stream path is not read ahead: it starts its own device as soon as
  sound is pushed, so reading ahead would make *it* start early.

**Draining** — when the picture ends, the movie keeps the release back until the
track has played everything the decoder fed it (`Movie_MixerDrained()` compares
the played frame position with what was fed, silence padding included), or three
seconds pass.  The last note of a cutscene is not cut off.

**The safety switch** — `SONICCD_MOVIE_STREAM_AUDIO=1` keeps the old
`SDL_OpenAudioDeviceStream` path, so the two can be judged by ear and one
environment variable reverts to the previous behaviour.  Remove the switch and
the `movie.sound` branches once the mixer path has been confirmed on real
hardware.

## Debug support

`MOVIE_DEBUG=1` still logs once a second, now with the path in the line:

```
movie: t=4.84 picture=4.84 sound=4.87 (offset -30ms, queued 984ms, mixer)
movie: t=4.84 picture=4.84 sound=3.99 (offset 878ms, queued 878ms, device 23ms, stream)
```

`sound` is the film position of the sound (the mixer's played position minus the
silence it had to pad, or the stream's queued data in the old path), `queued` is
how much sound is still waiting, `offset` is `t - sound`, and the last word says
which path is running.  A silent film prints `sound=- (silent film)`.

## Verification

Headless runs from the game directory (`SDL_VIDEODRIVER=dummy`), opening movie:

| path | offset (t - sound) |
| --- | --- |
| old, `SONICCD_MOVIE_STREAM_AUDIO=1` | ~885 ms behind, constant (unchanged from the report) |
| new, mixer | within ±40 ms of the picture for the whole 91 s film |

Capturing the mixed output with SDL's disk audio driver and comparing it with the
film's own soundtrack showed the old path starting the film's sound 0.9 s after
the picture; with the read-ahead the mixer has the film's first sound in hand
before the first frame, which is what the ±40 ms above measures.  The residual
few tens of milliseconds are the mixer's own buffering and cannot be measured
without a speaker.

The same captures check that the sound itself is intact: over a 40 s run the only
silence in the output is the film's own (508 ms before its music starts), there
are no sample-to-sample discontinuities beyond what the soundtrack itself has
(largest step 13843 in the capture against 12098 in the film), and the output's
loudness envelope lines up with the film's within one 0.1 s window.

`movie 1` (pencil test, no audio) still plays through, `ending good` still plays
its movies with the mixer path, the full 91 s opening ends cleanly and hands back
to the title, and `Movie_Stop()` (a button press cutting a cutscene short)
releases everything without a stray sound.

Not implemented, and worth knowing: the picture still runs on the wall clock, as
it did before, so the hardware audio clock and the system clock drift apart
slowly (0.1-0.3 % measurements here, ~0.25 s over the opening on this machine's
dummy device).  Slaving the picture to the mixer's position - what mpv does -
would remove that drift, but the earlier attempt at an audio-driven picture
stuttered while the device started, so it is left as a separate step.

## How to try it

```
MOVIE_DEBUG=1 ./frontend movie 0                       # mixer path
MOVIE_DEBUG=1 SONICCD_MOVIE_STREAM_AUDIO=1 ./frontend movie 0   # old path
./frontend ending good                                 # ending + thanks screen
```

Judge the opening's first line of dialogue and the endings' music entries, which
are the easy places to hear a lag.
