#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <SDL3/SDL.h>
#include "../types.h"

void WaveRequest(Sint16 req_no);
void CDPlay(Sint16 track_number);
void CDPause(Sint16 mode);
void WaveAllStop(void);

int Sound_Init(void);

/* Movie sound.
 *
 * A cutscene's PCM is handed to the mixer as a raw SDL_IOStream (the movie owns
 * the samples and pads it with silence between decoded packets), and played on
 * this dedicated track, so the film shares the game's one audio device instead
 * of opening an SDL audio stream of its own - see docs/movie_audio_plan.md. */
int Sound_BeginMovieAudio(SDL_IOStream *io, const SDL_AudioSpec *spec);
void Sound_PlayMovieAudio(void);
Sint64 Sound_GetMovieAudioPosition(void);
void Sound_EndMovieAudio(void);

#ifdef __cplusplus
}
#endif
