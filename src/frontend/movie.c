#include <stdio.h>
#include <string.h>

#include <SDL3/SDL.h>

#include "constants.h"
#include "movie.h"

#if !defined(SONICCD_HAVE_FFMPEG)

/* Built without FFmpeg: every entry point is a no-op, so the movie DLLs just
   see "nothing to play" and the game carries on. */

int Movie_IsPlaying(void) { return 0; }
Sint32 Movie_Play(Sint32 id) {
    (void)id;
    return 0;
}
Sint32 Movie_Stop(void) { return MOVIE_STAT_FINISHED; }
Sint32 Movie_Stat(void) { return MOVIE_STAT_FINISHED; }
void Movie_Update(SDL_Renderer *renderer) { (void)renderer; }
void Movie_Shutdown(void) {}

#else

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>

#define MOVIE_AUDIO_RATE 44100
#define MOVIE_AUDIO_CHANNELS 2

typedef struct {
    const char *candidates[3];
} movie_entry;

/* The ids the decompiled movie DLLs request (TITLE/AVIOPEN: 0; TITLE/AVIGOOD:
   2..5).  Each ending is a two-part movie: the DLL plays part 1 of a pair,
   waits for Movie_Stat() == MOVIE_STAT_FINISHED, then plays part 2.
   The endings exist in two shapes - one combined file holding the whole
   ending (the OGV files, credits included), or the Mega-CD era pair of SFD
   halves - so part 1 prefers the combined file and part 2 only has anything
   to play when part 1 came from the split files (see Movie_Play). */
static const movie_entry kMovies[] = {
    {{"TITLE/AVI/OPN.OGV", "TITLE/AVI/OPN.AVI", "OPENING.SFD"}},
    {{"TITLE/AVI/PENCIL.OGV", "TITLE/AVI/PENCIL.AVI", "SENGA.SFD"}},
    {{"TITLE/AVI/END1.OGV", "ENDING1_1.SFD", 0}}, /* bad ending, part 1 */
    {{"ENDING1_2.SFD", 0, 0}},                    /* bad ending, part 2 */
    {{"TITLE/AVI/END2.OGV", "ENDING2_1.SFD", 0}}, /* good ending, part 1 */
    {{"ENDING2_2.SFD", 0, 0}},                    /* good ending, part 2 */
};

#define MOVIE_COUNT ((int)(sizeof(kMovies) / sizeof(kMovies[0])))
#define MOVIE_RGBA_BYTES (SCREEN_WIDTH * SCREEN_HEIGHT * 4)
/* sws_scale works in chunks and is allowed to write a few pixels past the
   right edge of the picture it produces, so the buffer has slack after it. */
#define MOVIE_RGBA_SLACK 64

static struct {
    AVFormatContext *format;
    AVCodecContext *video;
    AVCodecContext *audio;
    struct SwsContext *scaler;
    struct SwrContext *resampler;
    AVPacket *packet;
    AVFrame *frame;
    AVFrame *audio_frame;
    int video_stream;
    int audio_stream;
    int width;
    int height;
    int scale_w;
    int scale_h;
    int scale_x;
    int scale_y;
    double time_base;
    double frame_duration;
    SDL_Texture *texture;
    SDL_AudioStream *sound;
    Uint8 *pixels;
    Uint64 start_ticks;
    double next_frame_time;
    int playing;
} movie;

static void Movie_Release(void) {
    movie.playing = 0;

    if (movie.sound) {
        SDL_DestroyAudioStream(movie.sound);
        movie.sound = NULL;
    }
    if (movie.texture) {
        SDL_DestroyTexture(movie.texture);
        movie.texture = NULL;
    }
    if (movie.pixels) {
        av_free(movie.pixels);
        movie.pixels = NULL;
    }
    sws_freeContext(movie.scaler);
    movie.scaler = NULL;
    swr_free(&movie.resampler);
    if (movie.audio_frame) {
        av_frame_free(&movie.audio_frame);
    }
    av_frame_free(&movie.frame);
    av_packet_free(&movie.packet);
    avcodec_free_context(&movie.audio);
    avcodec_free_context(&movie.video);
    if (movie.format) {
        avformat_close_input(&movie.format);
    }
    movie.format = NULL;
    movie.video_stream = -1;
    movie.audio_stream = -1;
}

static int Movie_OpenVideo(const char *path) {
    const AVCodec *codec;
    AVRational rate;

    if (avformat_open_input(&movie.format, path, NULL, NULL) < 0) {
        movie.format = NULL;
        return 0;
    }
    if (avformat_find_stream_info(movie.format, NULL) < 0) {
        return 0;
    }

    movie.video_stream = av_find_best_stream(movie.format, AVMEDIA_TYPE_VIDEO, -1,
                                             -1, &codec, 0);
    if (movie.video_stream < 0 || !codec) {
        return 0;
    }
    movie.video = avcodec_alloc_context3(codec);
    if (!movie.video ||
        avcodec_parameters_to_context(
            movie.video, movie.format->streams[movie.video_stream]->codecpar) <
            0 ||
        avcodec_open2(movie.video, codec, NULL) < 0) {
        return 0;
    }

    movie.width = movie.video->width;
    movie.height = movie.video->height;
    movie.time_base =
        av_q2d(movie.format->streams[movie.video_stream]->time_base);
    rate = av_guess_frame_rate(movie.format,
                               movie.format->streams[movie.video_stream], NULL);
    movie.frame_duration = (rate.num > 0 && rate.den > 0) ? av_q2d(rate) : 0.0;
    if (movie.frame_duration > 0.0) {
        movie.frame_duration = 1.0 / movie.frame_duration;
    } else {
        movie.frame_duration = 1.0 / 30.0;
    }

    /* Fit the movie into the game's 320x224 screen without changing its aspect
       ratio; whatever is left over stays black (letterbox / pillarbox). */
    {
        double scale = (double)SCREEN_WIDTH / movie.width;
        double scale_h = (double)SCREEN_HEIGHT / movie.height;

        if (scale_h < scale) {
            scale = scale_h;
        }
        movie.scale_w = (int)(movie.width * scale) & ~1;
        movie.scale_h = (int)(movie.height * scale) & ~1;
        if (movie.scale_w <= 0 || movie.scale_h <= 0 ||
            movie.scale_w > SCREEN_WIDTH || movie.scale_h > SCREEN_HEIGHT) {
            movie.scale_w = SCREEN_WIDTH;
            movie.scale_h = SCREEN_HEIGHT;
        }
        movie.scale_x = (SCREEN_WIDTH - movie.scale_w) / 2;
        movie.scale_y = (SCREEN_HEIGHT - movie.scale_h) / 2;
    }

    movie.scaler = sws_getContext(movie.width, movie.height, movie.video->pix_fmt,
                                  movie.scale_w, movie.scale_h,
                                  AV_PIX_FMT_BGRA, SWS_BILINEAR, NULL, NULL,
                                  NULL);
    if (!movie.scaler) {
        return 0;
    }

    movie.pixels = av_malloc(MOVIE_RGBA_BYTES + MOVIE_RGBA_SLACK);
    movie.frame = av_frame_alloc();
    movie.packet = av_packet_alloc();
    return movie.pixels && movie.frame && movie.packet;
}

/* The OGV files carry more than one complete soundtrack for the same picture
   (the JP and the US music of the movie): the streams have identical metadata,
   the same length and no disposition to tell them apart, so the track is
   picked by position.  0 is the first one. */
#define MOVIE_AUDIO_STREAM_INDEX 0

static int Movie_AudioStream(void) {
    AVCodecParameters *parameters;
    int seen = 0;
    unsigned int i;

    for (i = 0; i < movie.format->nb_streams; ++i) {
        parameters = movie.format->streams[i]->codecpar;
        if (parameters && parameters->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (seen == MOVIE_AUDIO_STREAM_INDEX) {
                return (int)i;
            }
            ++seen;
        }
    }
    return -1;
}

static int Movie_OpenAudio(void) {
    const AVCodec *codec;
    SDL_AudioSpec spec;
    AVChannelLayout out_layout = AV_CHANNEL_LAYOUT_STEREO;
    int stream;

    stream = Movie_AudioStream();
    if (stream < 0) {
        return 1; /* silent movie: not a failure */
    }
    codec = avcodec_find_decoder(
        movie.format->streams[stream]->codecpar->codec_id);
    if (!codec) {
        return 1;
    }
    movie.audio = avcodec_alloc_context3(codec);
    if (!movie.audio ||
        avcodec_parameters_to_context(
            movie.audio, movie.format->streams[stream]->codecpar) < 0 ||
        avcodec_open2(movie.audio, codec, NULL) < 0) {
        return 1;
    }
    if (swr_alloc_set_opts2(&movie.resampler, &out_layout, AV_SAMPLE_FMT_S16,
                            MOVIE_AUDIO_RATE, &movie.audio->ch_layout,
                            movie.audio->sample_fmt, movie.audio->sample_rate, 0,
                            NULL) < 0 ||
        swr_init(movie.resampler) < 0) {
        return 1;
    }
    movie.audio_frame = av_frame_alloc();
    if (!movie.audio_frame) {
        return 1;
    }
    movie.audio_stream = stream;

    SDL_zero(spec);
    spec.format = SDL_AUDIO_S16;
    spec.channels = MOVIE_AUDIO_CHANNELS;
    spec.freq = MOVIE_AUDIO_RATE;
    movie.sound = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                                            &spec, NULL, NULL);
    if (movie.sound) {
        SDL_ResumeAudioStreamDevice(movie.sound);
    }
    return 1;
}

static void Movie_QueueAudio(const AVPacket *packet) {
    if (!movie.audio || !movie.resampler || !movie.sound) {
        return;
    }
    if (avcodec_send_packet(movie.audio, packet) < 0) {
        return;
    }
    while (avcodec_receive_frame(movie.audio, movie.audio_frame) == 0) {
        int samples = (int)av_rescale_rnd(
            swr_get_delay(movie.resampler, movie.audio->sample_rate) +
                movie.audio_frame->nb_samples,
            MOVIE_AUDIO_RATE, movie.audio->sample_rate, AV_ROUND_UP);
        Uint8 *buffer = av_malloc((size_t)samples * MOVIE_AUDIO_CHANNELS *
                                  sizeof(Sint16));
        int converted;

        if (!buffer) {
            return;
        }
        converted = swr_convert(movie.resampler, &buffer, samples,
                                (const Uint8 **)movie.audio_frame->extended_data,
                                movie.audio_frame->nb_samples);
        if (converted > 0) {
            SDL_PutAudioStreamData(movie.sound, buffer,
                                   converted * MOVIE_AUDIO_CHANNELS *
                                       (int)sizeof(Sint16));
        }
        av_free(buffer);
        av_frame_unref(movie.audio_frame);
    }
}

/* Pulls packets until one video frame comes out, feeding audio as it goes.
   Returns 0 at the end of the stream. */
static int Movie_DecodeFrame(void) {
    for (;;) {
        int result = av_read_frame(movie.format, movie.packet);

        if (result < 0) {
            if (avcodec_send_packet(movie.video, NULL) >= 0 &&
                avcodec_receive_frame(movie.video, movie.frame) == 0) {
                return 1;
            }
            return 0;
        }
        if (movie.packet->stream_index == movie.video_stream) {
            result = avcodec_send_packet(movie.video, movie.packet);
            av_packet_unref(movie.packet);
            if (result >= 0 &&
                avcodec_receive_frame(movie.video, movie.frame) == 0) {
                return 1;
            }
        } else if (movie.packet->stream_index == movie.audio_stream) {
            Movie_QueueAudio(movie.packet);
            av_packet_unref(movie.packet);
        } else {
            av_packet_unref(movie.packet);
        }
    }
}

static void Movie_Present(SDL_Renderer *renderer) {
    Uint8 *dst[4];
    int dst_stride[4];
    int y;

    dst[0] = movie.pixels +
             ((size_t)movie.scale_y * SCREEN_WIDTH + movie.scale_x) * 4;
    dst[1] = dst[2] = dst[3] = NULL;
    dst_stride[0] = SCREEN_WIDTH * 4;
    dst_stride[1] = dst_stride[2] = dst_stride[3] = 0;

    sws_scale(movie.scaler, (const Uint8 *const *)movie.frame->data,
              movie.frame->linesize, 0, movie.height, dst, dst_stride);

    /* Letterbox bars: the scaler fills whole chunks, so it leaves stray pixels
       just past the picture's right edge; paint the bars after the scale. */
    memset(movie.pixels, 0, (size_t)movie.scale_y * SCREEN_WIDTH * 4);
    memset(movie.pixels + (size_t)(movie.scale_y + movie.scale_h) * SCREEN_WIDTH * 4,
           0,
           (size_t)(SCREEN_HEIGHT - movie.scale_y - movie.scale_h) *
               SCREEN_WIDTH * 4);
    for (y = movie.scale_y; y < movie.scale_y + movie.scale_h; ++y) {
        Uint8 *row = movie.pixels + (size_t)y * SCREEN_WIDTH * 4;

        memset(row, 0, (size_t)movie.scale_x * 4);
        memset(row + (size_t)(movie.scale_x + movie.scale_w) * 4, 0,
               (size_t)(SCREEN_WIDTH - movie.scale_x - movie.scale_w) * 4);
    }

    if (!movie.texture) {
        movie.texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                          SDL_TEXTUREACCESS_STREAMING,
                                          SCREEN_WIDTH, SCREEN_HEIGHT);
    }
    if (!movie.texture) {
        return;
    }
    SDL_UpdateTexture(movie.texture, NULL, movie.pixels, SCREEN_WIDTH * 4);
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, movie.texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

/* Movies 2/4 open an ending and 3/5 are its second half.  The second half only
   exists when the first one came from the split SFD files: the combined OGV
   already holds the whole ending. */
#define MOVIE_IS_ENDING_PART1(id) ((id) == 2 || (id) == 4)
#define MOVIE_IS_ENDING_PART2(id) ((id) == 3 || (id) == 5)

static int ending_part2_expected;

Sint32 Movie_Play(Sint32 id) {
    const movie_entry *entry;
    int i;

    Movie_Release();
    if (id < 0 || id >= MOVIE_COUNT) {
        return 0;
    }
    if (MOVIE_IS_ENDING_PART2(id) && !ending_part2_expected) {
        SDL_Log("movie %d: the ending played from a single file, no part 2",
                (int)id);
        return 0;
    }
    if (MOVIE_IS_ENDING_PART1(id)) {
        ending_part2_expected = 0;
    }
    entry = &kMovies[id];
    for (i = 0; i < (int)(sizeof(entry->candidates) /
                          sizeof(entry->candidates[0]));
         ++i) {
        if (!entry->candidates[i]) {
            break;
        }
        if (Movie_OpenVideo(entry->candidates[i])) {
            break;
        }
        Movie_Release();
    }
    if (!movie.format) {
        SDL_Log("movie %d: no playable file found", (int)id);
        return 0;
    }
    Movie_OpenAudio();

    if (MOVIE_IS_ENDING_PART1(id)) {
        /* Candidate 0 is the combined file; anything later is a split half. */
        ending_part2_expected = (i > 0);
    }

    movie.playing = 1;
    movie.start_ticks = SDL_GetTicks();
    movie.next_frame_time = 0.0;
    SDL_Log("movie %d: playing %s", (int)id, entry->candidates[i]);
    return 1;
}

Sint32 Movie_Stop(void) {
    Movie_Release();
    return MOVIE_STAT_FINISHED;
}

Sint32 Movie_Stat(void) {
    /* The movie DLLs treat 3 as "no movie is running" and poll it after
       Movie_Play(), so a finished (or never started) movie reports that. */
    return movie.playing ? MOVIE_STAT_PLAYING : MOVIE_STAT_FINISHED;
}

int Movie_IsPlaying(void) { return movie.playing; }

void Movie_Update(SDL_Renderer *renderer) {
    double elapsed;
    int shown = 0;

    if (!movie.playing) {
        return;
    }
    elapsed = (double)(SDL_GetTicks() - movie.start_ticks) / 1000.0;

    /* Show every frame whose time has come; two per tick at most, so a slow
       frame cannot make the movie run away. */
    while (shown < 2 && elapsed >= movie.next_frame_time) {
        double stamp;

        if (!Movie_DecodeFrame()) {
            Movie_Release();
            return;
        }
        Movie_Present(renderer);
        ++shown;

        stamp = (movie.frame->best_effort_timestamp == AV_NOPTS_VALUE)
                    ? movie.next_frame_time
                    : (double)movie.frame->best_effort_timestamp *
                          movie.time_base;
        movie.next_frame_time = stamp + movie.frame_duration;
        av_frame_unref(movie.frame);
    }
}

void Movie_Shutdown(void) { Movie_Release(); }

#endif /* SONICCD_HAVE_FFMPEG */
