#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <stdio.h>
#include <string.h>
#include "sound.h"

static MIX_Mixer *mixer;
static MIX_Track *musicTrack;
static MIX_Audio *musicAudio;
static MIX_Track *movieTrack;
static Sint16 currTrack = -1;

typedef struct {
	char *filename;
	bool loop;
} MusicInfo;

static MusicInfo musicTbl[] = {
	{ NULL, false },
	{ "R1A.ogg", true},
	{ "R1C.ogg", true },
	{ "R1D.ogg", true },
	{ "R3A.ogg", true },
	{ "R3C.ogg", true },
	{ "R3D.ogg", true },
	{ "R4A.ogg", true },
	{ "R4C.ogg", true },
	{ "R4D.ogg", true },
	{ "R5A.ogg", true },
	{ "R5C.ogg", true },
	{ "R5D.ogg", true },
	{ "R6A.ogg", true },
	{ "R6C.ogg", true },
	{ "R6D.ogg", true },
	{ "R7A.ogg", true },
	{ "R7C.ogg", true },
	{ "R7D.ogg", true },
	{ "R8A.ogg", true },
	{ "R8C.ogg", true },
	{ "R8D.ogg", true },
	{ "Boss.ogg", true },
	{ "FinalBoss.ogg", true },
	{ "TitleScreen.ogg", false },
	{ "TimeAttack.ogg", true },
	{ "ZoneComplete.ogg", false },
	{ "SpeedShoes.ogg", false },
	{ "Invincibility.ogg", false },
	{ "GameOver.ogg", false },
	{ "SpecialStage.ogg", true },
	{ "DAGarden.ogg", true },
	{ NULL, false },
	{ NULL, false },
	{ "R1B.ogg", true },
	{ "R3B.ogg", true },
	{ "R4B.ogg", true },
	{ "R5B.ogg", true },
	{ "R6B.ogg", true },
	{ "R7B.ogg", true },
	{ "R8B.ogg", true },
};

#define NUM_SOUND_EFFECTS 86
static MIX_Audio *soundEffects[NUM_SOUND_EFFECTS];

void WaveRequest(Sint16 req_no) {
	SDL_Log("WaveRequest(%d)", req_no);
	if (req_no < 0 || req_no >= NUM_SOUND_EFFECTS || !soundEffects[req_no]) {
		return;
	}
	MIX_PlayAudio(mixer, soundEffects[req_no]);
}

static void Music_Start(Sint16 track_number) {
	if (track_number < 2 || (track_number - 2 >= SDL_arraysize(musicTbl))) {
		return;
	}
	MusicInfo *mi = &musicTbl[track_number - 2];
	MIX_StopTrack(musicTrack, 0);
	if (mi->filename) {
		char filenameBuff[80];
		snprintf(filenameBuff, sizeof(filenameBuff), "assets/Music/%s", mi->filename);
		if (musicAudio) {
			MIX_DestroyAudio(musicAudio);
		}
		musicAudio = MIX_LoadAudio(mixer, filenameBuff, false);
		if (!musicAudio) {
			SDL_Log("Couldn't read audio: %s", SDL_GetError());
			return;
		}
		MIX_SetTrackAudio(musicTrack, musicAudio);
		SDL_PropertiesID options = SDL_CreateProperties();
		SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, mi->loop ? -1 : 0);
		MIX_PlayTrack(musicTrack, options);
		SDL_DestroyProperties(options);
		currTrack = track_number;
	}
}

void CDPlay(Sint16 track_number) {
	SDL_Log("CDPlay(%d)", track_number);
	/* A request for the track that is already playing is dropped, so the
	   D.A. Garden's make-up - which asks for the track three times while it
	   swaps the Little Planet's graphics - plays it once.  The reference gets
	   the same result by not starting anything until its 60-frame CD countdown
	   expires (`CDPlay()` and the countdown in WINEXE/WINMAIN.C), but the port
	   starts at once: the music has to stay in step with the picture, and the
	   title screen's cue lands on the frame it is asked for. */
	if (track_number == currTrack && musicTrack && MIX_TrackPlaying(musicTrack)) {
		return;
	}
	Music_Start(track_number);
}

void CDPause(Sint16 mode) {
	SDL_Log("CDPause(%d)", mode);
    if (mode == 2) {
            /* The track is stopping, so forget it: a request for the same one
               after this has to start it again (the sound test and the menus do
               exactly that when a track is picked twice). */
            currTrack = -1;
            MIX_StopTrack(musicTrack, MIX_TrackMSToFrames(musicTrack, 500));
	}
	else if (mode == 1) {
		MIX_ResumeTrack(musicTrack);
	}
	else {
		MIX_PauseTrack(musicTrack);
	}
}

void WaveAllStop(void) {
	SDL_Log("WaveAllStop()");
}

/* The movie's track: created on first use, given the film's raw PCM stream as
   its input, and left silent between movies so nothing of the last cutscene can
   still be heard over the next one. */
int Sound_BeginMovieAudio(SDL_IOStream *io, const SDL_AudioSpec *spec) {
	if (!mixer || !io || !spec) {
		return 0;
	}
	if (!movieTrack) {
		movieTrack = MIX_CreateTrack(mixer);
		if (!movieTrack) {
			SDL_Log("Couldn't create the movie track: %s", SDL_GetError());
			return 0;
		}
	}
	MIX_StopTrack(movieTrack, 0);
	if (!MIX_SetTrackRawIOStream(movieTrack, io, spec, false)) {
		SDL_Log("Couldn't set the movie track's stream: %s", SDL_GetError());
		return 0;
	}
	return 1;
}

void Sound_PlayMovieAudio(void) {
	if (movieTrack) {
		MIX_PlayTrack(movieTrack, 0);
	}
}

/* Sample frames the movie track has played, -1 when there is no track.  The
   movie uses it to report where its sound really is. */
Sint64 Sound_GetMovieAudioPosition(void) {
	if (!movieTrack) {
		return -1;
	}
	return MIX_GetTrackPlaybackPosition(movieTrack);
}

void Sound_EndMovieAudio(void) {
	if (!movieTrack) {
		return;
	}
	MIX_StopTrack(movieTrack, 0);
	/* Detach the stream before the movie frees it: the mixer must not be able
	   to read from it any more. */
	MIX_SetTrackRawIOStream(movieTrack, NULL, NULL, false);
}

int Sound_Init(void) {
	if (!MIX_Init()) {
		SDL_Log("Couldn't init SDL_Mixer: %s", SDL_GetError());
		return 0;
	}

	mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
	if (!mixer) {
		SDL_Log("Couldn't create mixer on default device: %s", SDL_GetError());
		return 0;
	}

	musicTrack = MIX_CreateTrack(mixer);
	if (!musicTrack) {
		SDL_Log("Couldn't create music track: %s", SDL_GetError());
		return 0;
	}

	char filenameBuff[80];
	for (int i = 0; i < NUM_SOUND_EFFECTS; i++) {
		snprintf(filenameBuff, sizeof(filenameBuff), "assets/SFX/%02d.wav", i);
		soundEffects[i] = MIX_LoadAudio(mixer, filenameBuff, true);
		if (!soundEffects[i]) {
			SDL_Log("couldn't init sound %d", i);
		}
	}

	return 1;
}
