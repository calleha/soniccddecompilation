# Plan: movie sound through the game's mixer

Status: **not implemented**.  Written at the end of the session that reworked the
FMV player; the tree is at a safe point (wall-clock picture, working sound, see
the note at the bottom).

## The problem

In every cutscene (opening, pencil test, endings) the sound runs about **one
second behind the picture**, and the first seconds of sound drop out a couple of
times.  Measured facts (`MOVIE_DEBUG=1`):

- picture is on time: `t=4.80 picture=4.80`;
- the SDL audio stream reports **700-1000 ms queued** for the whole film - the
  file's own interleave lead - which is the size of the delay heard;
- `SDL_GetAudioDeviceFormat()` reports a 17 ms device buffer, so the latency is
  not the device's;
- the dropouts are at the start, where the stream is started before any sound has
  arrived and the demuxer's first packets come in dribs and drabs.

The fork in `soniccdrespinned` is not a reference here: its own cutscene timing is
not accurate either.  The target is "what mpv does" - one audio device, sound
driven by it, picture slaved to the sound - but the *implementation* has to be
ours, because we drive the mixer, not a decoder clock.

## Why the current shape cannot fix it

`movie.c` uses a **second** SDL audio stream (`SDL_OpenAudioDeviceStream`) beside
the game's SDL3_mixer device.  That stream carries its own queue (the demuxer's
interleave lead, ~1 s) and starts life empty, which is exactly what the two
dropouts are.  Timing the picture from that stream's reported position was tried
and failed: while the device spins up the position crawls (visible stutter) and a
slow device stretches the whole film.

## Plan

Move the movie's sound onto the **game's mixer device**, so there is one audio
path with one latency, and drop the separate stream.

1. **`src/frontend/sound.c`**
   - export the mixer: `MIX_Mixer *Sound_GetMixer(void);`
   - keep a dedicated movie track, created lazily:
     `static MIX_Track *movieTrack;` → `MIX_CreateTrack(Sound_GetMixer())` on first
     use, and a setter/getter pair for `movie.c`
     (`Sound_BeginMovieAudio(...)`, `Sound_EndMovieAudio()`).
   - `CDPause(2)` already stops the music track; make sure the movie track is
     stopped/freed on `Sound_EndMovieAudio()` so the two cannot overlap.

2. **`src/frontend/movie.c`**
   - replace the SDL audio stream with a small ring buffer of decoded
     44.1 kHz / S16 / stereo samples (`pending_audio` + read/write indexes, a few
     hundred KB is plenty).  `Movie_QueueAudio()` writes into it instead of
     calling `SDL_PutAudioStreamData`.
   - expose it as an `SDL_IOStream` whose `read` callback:
     * always returns the size asked for, padding with **silence** when the ring
       is empty (a dry moment must not look like end of stream, or the track
       stops), and
     * returns 0 only when the movie is over (`movie.video_done`) and the ring has
       drained.
   - wrap it with `MIX_LoadRawAudio_IO(Sound_GetMixer(), io, &spec, false)` and
     give the resulting `MIX_Audio` to the movie track; `MIX_PlayTrack()` when the
     first frame is presented (the film clock already starts there).
   - keep everything else as it is today: wall-clock picture, dropping late
     frames, `thread_count = 0`, drain on end.

3. **Debug support** (`MOVIE_DEBUG=1`)
   - next to `queued`, log the track's playback position
     (`MIX_GetTrackPlaybackPosition()` / `MIX_GetTrackRemaining()` if available,
     otherwise the ring fill and the samples consumed) so one log line says
     whether the sound really is at film position `t`.

4. **Safety switch**
   - `SONICCD_MOVIE_STREAM_AUDIO=1` (or similar) keeps the current
     `SDL_OpenAudioDeviceStream` path for one round, so the new path can be judged
     by ear and one variable reverts to working sound if it misbehaves.  Remove the
     switch once the mixer path is confirmed.

## How to verify

- `MOVIE_DEBUG=1 ./frontend movie 0` and compare with mpv playing
  `TITLE/AVI/OPN.OGV` (stream 0, the JP track - the US one is unused by this port,
  `MOVIE_AUDIO_STREAM_INDEX`); the opening's first line of dialogue and the
  endings' music entries are the easy places to judge.
- The start-up dropouts should be gone (there is no longer an empty stream being
  started).
- `movie 1` (pencil test) has no audio and must still play through; `ending good`
  / `ending bad` must still reach the thanks screen, and Start must still cut a
  cutscene short.
- `ctest -R backend.title` and `ctest -R backend.r8` stay green (146 tests).

## State of the tree when this plan was written

`frontend` = md5 `2de9587b…`: wall-clock picture starting at the first presented
frame, late frames dropped, video decoder threaded, remaining sound drained
(<= 3 s) when the picture ends, sound on the simple SDL stream path (audible, ~1 s
behind).  `MOVIE_DEBUG=1` traces `t / picture / sound / offset / device / queued`.
`docs/attract_mode_and_fmv.md` has the measurements under "Known issue".
