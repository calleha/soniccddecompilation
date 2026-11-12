#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <SDL3/SDL.h>
#include "../types.h"

/* Full motion video.
 *
 * The game never opens a movie file itself: the movie DLLs ask the frontend to
 * play a movie by index and poll it for completion, exactly like the Windows
 * frontend did, so the decompiled TITLE/AVIOPEN and TITLE/AVIGOOD code runs
 * unmodified:
 *
 *   0  opening             TITLE/AVI/OPN.OGV     (OPENING.SFD)
 *   1  pencil test         TITLE/AVI/PENCIL.OGV  (SENGA.SFD)
 *   2  bad ending part 1   TITLE/AVI/END1.OGV    (ENDING1_1.SFD)
 *   3  bad ending part 2   TITLE/AVI/END1B.OGV   (ENDING1_2.SFD)
 *   4  good ending part 1  TITLE/AVI/END2.OGV    (ENDING2_1.SFD)
 *   5  good ending part 2  TITLE/AVI/END2B.OGV   (ENDING2_2.SFD)
 *
 * The endings are two-part movies: the DLL plays part 1, waits for
 * Movie_Stat() == MOVIE_STAT_FINISHED, then plays part 2. A single file holding
 * the whole ending therefore goes in part 1's slot; part 2 picks up an END*B
 * file when one exists (or the second SFD) and is otherwise skipped, so the
 * ending plays once instead of twice.
 *
 * The first existing candidate wins, so OGV files are preferred and the
 * Mega-CD era SFD files work too (FFmpeg decodes both).
 *
 * Playback is non-blocking: Movie_Play() starts it, the main loop calls
 * Movie_Update() once per frame, and Movie_Stat() reports MOVIE_STAT_PLAYING
 * until the movie ends or is stopped. Without FFmpeg (or without the file)
 * every entry point is a no-op and the game simply carries on. */

/* The movie DLLs poll Movie_Stat() and advance when it returns 3. */
#define MOVIE_STAT_PLAYING 0
#define MOVIE_STAT_FINISHED 3

Sint32 Movie_Play(Sint32 id);
Sint32 Movie_Stop(void);
Sint32 Movie_Stat(void);
int Movie_IsPlaying(void);
void Movie_Update(SDL_Renderer *renderer);
void Movie_Shutdown(void);

#ifdef __cplusplus
}
#endif
