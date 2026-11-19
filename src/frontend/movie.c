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

#include "sound.h"

#define MOVIE_AUDIO_RATE 44100
#define MOVIE_AUDIO_CHANNELS 2
/* Room for the decoded sound waiting for the mixer: the demuxer hands the movie
   up to a second of sound ahead of the picture, so there is plenty of slack in
   ~5.9 s of 44.1 kHz stereo 16-bit. */
#define MOVIE_AUDIO_RING_BYTES (1024 * 1024)
#define MOVIE_AUDIO_FRAME_BYTES (MOVIE_AUDIO_CHANNELS * (int)sizeof(Sint16))

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
/* How many video packets may be held back while the sound is read ahead: eight
   seconds of a 30 fps film, far more than any interleave lead needs, and a
   ceiling so a file with its sound at the end cannot eat memory. */
#define MOVIE_VIDEO_QUEUE_MAX 240
/* How much sound to have in hand before the picture starts, and how much to keep
   in hand while it runs.  The films put all of a second of picture ahead of any
   sound in the file (see Movie_ReadAhead), so the first figure has to be larger
   than that lead for the sound to start with the picture instead of a second
   after it; the second only has to leave a margin. */
#define MOVIE_AUDIO_PREFILL_MS 2000
#define MOVIE_AUDIO_AHEAD_MS 1000

static struct {
    AVFormatContext *format;
    AVCodecContext *video;
    AVCodecContext *audio;
    struct SwsContext *scaler;
    struct SwrContext *resampler;
    AVPacket *packet;
    AVFrame *frame;
    AVFrame *audio_frame;
    /* Video packets read while the sound is read ahead, kept in file order so
       the picture loses no frames (see Movie_Prefill). */
    AVPacket video_queue[MOVIE_VIDEO_QUEUE_MAX];
    int video_queue_head;
    int video_queue_count;
    int at_end;
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
    /* Decoded sound waiting to be handed to the audio path below. */
    Uint8 *pixels;
    /* The old audio path, kept behind SONICCD_MOVIE_STREAM_AUDIO so the two can
       be compared by ear: an SDL audio stream with a device of its own. */
    SDL_AudioStream *sound;
    double device_latency;
    int audio_started;
    /* The mixer path: the same decoded sound in a ring buffer, which the game's
       mixer pulls through an SDL_IOStream (Movie_AudioRead) on the movie's own
       track - one audio device and one latency for the whole port.  The decoder
       writes at audio_write, the mixer reads at audio_read, and audio_fill is
       how much is waiting between them. */
    SDL_IOStream *audio_io;
    SDL_Mutex *audio_lock;
    Uint8 *audio_ring;
    size_t audio_capacity;
    size_t audio_read;
    size_t audio_write;
    size_t audio_fill;
    Uint64 fed_audio_frames;
    Uint64 silence_bytes;
    /* Set when the first frame has been presented: the picture and the sound
       both start from there, so the time spent decoding the first frame cannot
       put the sound ahead of the picture. */
    int started;
    Uint64 start_ticks;
    double next_frame_time;
    /* Set when the picture is over but decoded sound may still be playing. */
    int video_done;
    Uint64 drain_deadline;
    int playing;
} movie;

static void Movie_Release(void) {
    movie.playing = 0;

    /* Let go of the video packets that were read ahead. */
    while (movie.video_queue_count > 0) {
        av_packet_unref(&movie.video_queue[movie.video_queue_head]);
        movie.video_queue_head++;
        movie.video_queue_count--;
    }
    movie.video_queue_head = 0;
    movie.at_end = 0;

    /* Sound first: the mixer may be reading the ring right now, so its track is
       stopped and detached before anything it touches goes away. */
    if (movie.sound) {
        SDL_DestroyAudioStream(movie.sound);
        movie.sound = NULL;
    }
    Sound_EndMovieAudio();
    if (movie.audio_io) {
        SDL_CloseIO(movie.audio_io);
        movie.audio_io = NULL;
    }
    if (movie.audio_lock) {
        SDL_DestroyMutex(movie.audio_lock);
        movie.audio_lock = NULL;
    }
    SDL_free(movie.audio_ring);
    movie.audio_ring = NULL;
    movie.audio_capacity = 0;
    movie.audio_read = 0;
    movie.audio_write = 0;
    movie.audio_fill = 0;
    movie.fed_audio_frames = 0;
    movie.silence_bytes = 0;
    movie.video_done = 0;
    movie.drain_deadline = 0;
    movie.device_latency = 0.0;
    movie.audio_started = 0;
    movie.started = 0;
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
            0) {
        return 0;
    }
    /* Decode on every core: these are 640x480 Theora, and a machine that cannot
       decode a frame within one game frame would otherwise let the picture fall
       behind its sound. */
    movie.video->thread_count = 0;
    if (avcodec_open2(movie.video, codec, NULL) < 0) {
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

/* SONICCD_MOVIE_STREAM_AUDIO=1 keeps the older path (an SDL audio stream with a
   device of its own) instead of the game mixer, so the two can be compared by
   ear; drop the switch once the mixer path is confirmed - see
   docs/movie_audio_plan.md. */
static int Movie_UseStreamAudio(void) {
    static int enabled = -1;

    if (enabled < 0) {
        const char *option = SDL_getenv("SONICCD_MOVIE_STREAM_AUDIO");
        enabled = (option && *option && *option != '0') ? 1 : 0;
    }
    return enabled;
}

static size_t SDLCALL Movie_AudioRead(void *userdata, void *ptr, size_t size,
                                      SDL_IOStatus *status);
static Sint64 SDLCALL Movie_AudioSize(void *userdata);
static Sint64 SDLCALL Movie_AudioSeek(void *userdata, Sint64 offset,
                                      SDL_IOWhence whence);

/* The ring as the mixer sees it: a stream of raw PCM that only goes forward and
   whose length is not known up front. */
static const SDL_IOStreamInterface kMovieAudioStream = {
    sizeof(SDL_IOStreamInterface), /* version */
    Movie_AudioSize,
    Movie_AudioSeek,
    Movie_AudioRead,
    NULL, /* write */
    NULL, /* flush */
    NULL, /* close: the movie owns the ring */
};

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

    if (Movie_UseStreamAudio()) {
        /* The old path, for comparing the two by ear: an audio stream with a
           device of its own, which carries the demuxer's whole interleave lead
           as its own queue (that lead is the ~1 s the sound used to trail by). */
        movie.sound =
            SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec,
                                     NULL, NULL);
        /* Started by the first sound that is queued for it (see
           Movie_QueueAudio), so it does not run on an empty queue - that was
           audible as a click or two at the start of a movie. */
        if (movie.sound) {
            SDL_AudioDeviceID device = SDL_GetAudioStreamDevice(movie.sound);
            SDL_AudioSpec device_spec;
            int frames = 0;

            if (device != 0 &&
                SDL_GetAudioDeviceFormat(device, &device_spec, &frames) &&
                frames > 0 && device_spec.freq > 0) {
                movie.device_latency =
                    (double)frames / (double)device_spec.freq;
            }
        }
        return 1;
    }

    /* The mixer path: the decoded sound goes into a ring the mixer pulls
       through Movie_AudioRead, on the movie's own track.  A failure here only
       costs the sound - the picture still plays. */
    movie.audio_lock = SDL_CreateMutex();
    movie.audio_ring = SDL_malloc(MOVIE_AUDIO_RING_BYTES);
    movie.audio_capacity = MOVIE_AUDIO_RING_BYTES;
    movie.audio_read = 0;
    movie.audio_write = 0;
    movie.audio_fill = 0;
    movie.fed_audio_frames = 0;
    movie.silence_bytes = 0;
    if (movie.audio_lock && movie.audio_ring) {
        movie.audio_io = SDL_OpenIO(&kMovieAudioStream, NULL);
    }
    if (!movie.audio_io || !Sound_BeginMovieAudio(movie.audio_io, &spec)) {
        SDL_Log("movie: no sound (%s)", SDL_GetError());
        if (movie.audio_io) {
            SDL_CloseIO(movie.audio_io);
            movie.audio_io = NULL;
        }
    }
    return 1;
}

/* Bytes waiting for the mixer right now. */
static size_t Movie_AudioQueued(void) {
    size_t queued;

    if (!movie.audio_lock) {
        return 0;
    }
    SDL_LockMutex(movie.audio_lock);
    queued = movie.audio_fill;
    SDL_UnlockMutex(movie.audio_lock);
    return queued;
}

/* Decoded sound waiting for the mixer.  The mixer drains in real time while the
   decoder only runs as fast as the picture needs, so the ring holds the
   demuxer's interleave lead at most; if it ever did fill up, the newest sound is
   dropped rather than stalling the picture. */
static void Movie_AudioQueue(const Uint8 *data, size_t size) {
    size_t first;

    if (!movie.audio_lock || !movie.audio_ring || size == 0) {
        return;
    }
    SDL_LockMutex(movie.audio_lock);
    if (size > movie.audio_capacity - movie.audio_fill) {
        size = movie.audio_capacity - movie.audio_fill;
    }
    /* Whole sample frames only: a half frame written here would be served as a
       half frame later and click. */
    size -= size % (size_t)MOVIE_AUDIO_FRAME_BYTES;
    first = movie.audio_capacity - movie.audio_write;
    if (first > size) {
        first = size;
    }
    memcpy(movie.audio_ring + movie.audio_write, data, first);
    if (size > first) {
        memcpy(movie.audio_ring, data + first, size - first);
    }
    movie.audio_write = (movie.audio_write + size) % movie.audio_capacity;
    movie.audio_fill += size;
    movie.fed_audio_frames += (Uint64)(size / (size_t)MOVIE_AUDIO_FRAME_BYTES);
    SDL_UnlockMutex(movie.audio_lock);
}

/* The mixer reads the film's sound through here, on its own thread.  Every read
   is answered in full: when the decoder has not caught up yet, the rest is
   silence, because a dry patch must not look like the end of the stream (that
   would stop the track).  Only once the picture is over and the ring has
   drained is a real end of stream reported. */
static size_t SDLCALL Movie_AudioRead(void *userdata, void *ptr, size_t size,
                                      SDL_IOStatus *status) {
    Uint8 *out = (Uint8 *)ptr;
    size_t first;
    size_t got;
    size_t missed;

    (void)userdata;
    if (!movie.audio_lock || size == 0) {
        if (status) {
            *status = SDL_IO_STATUS_EOF;
        }
        return 0;
    }
    SDL_LockMutex(movie.audio_lock);
    if (movie.audio_fill == 0 && movie.video_done) {
        SDL_UnlockMutex(movie.audio_lock);
        if (status) {
            *status = SDL_IO_STATUS_EOF;
        }
        return 0;
    }
    got = movie.audio_fill;
    if (got > size) {
        got = size;
    }
    first = movie.audio_capacity - movie.audio_read;
    if (first > got) {
        first = got;
    }
    memcpy(out, movie.audio_ring + movie.audio_read, first);
    if (got > first) {
        memcpy(out + first, movie.audio_ring, got - first);
    }
    movie.audio_read = (movie.audio_read + got) % movie.audio_capacity;
    movie.audio_fill -= got;
    missed = size - got;
    movie.silence_bytes += (Uint64)missed;
    SDL_UnlockMutex(movie.audio_lock);

    if (missed > 0) {
        memset(out + got, 0, missed);
    }
    return size;
}

static Sint64 SDLCALL Movie_AudioSize(void *userdata) {
    (void)userdata;
    return -1; /* length unknown: the film decides when it ends */
}

static Sint64 SDLCALL Movie_AudioSeek(void *userdata, Sint64 offset,
                                      SDL_IOWhence whence) {
    (void)userdata;
    /* The mixer seeks to the start of its input before it plays; the ring only
       goes forward, so that is all this stream can promise. */
    if (whence == SDL_IO_SEEK_SET && offset == 0) {
        return 0;
    }
    return SDL_SetError("the movie's sound stream only plays from the start");
}

static void Movie_QueueAudio(const AVPacket *packet) {
    if (!movie.audio || !movie.resampler ||
        (!movie.sound && !movie.audio_io)) {
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
            int bytes = converted * MOVIE_AUDIO_FRAME_BYTES;

            if (movie.sound) {
                if (SDL_PutAudioStreamData(movie.sound, buffer, bytes) &&
                    !movie.audio_started) {
                    movie.audio_started = 1;
                    SDL_ResumeAudioStreamDevice(movie.sound);
                }
            } else {
                Movie_AudioQueue(buffer, (size_t)bytes);
            }
        }
        av_free(buffer);
        av_frame_unref(movie.audio_frame);
    }
}

/* Reads one packet from the file: sound goes straight into the audio path, the
   picture's packets are queued so the sound can be read ahead without losing a
   frame.  Returns 0 at the end of the file. */
static int Movie_ReadPacket(void) {
    if (movie.at_end) {
        return 0;
    }
    if (av_read_frame(movie.format, movie.packet) < 0) {
        movie.at_end = 1;
        return 0;
    }
    if (movie.packet->stream_index == movie.audio_stream) {
        Movie_QueueAudio(movie.packet);
        av_packet_unref(movie.packet);
        return 1;
    }
    if (movie.packet->stream_index == movie.video_stream) {
        if (movie.video_queue_count >= MOVIE_VIDEO_QUEUE_MAX) {
            /* Only the read-ahead can fill the queue, and it stops at this
               ceiling; should it ever be hit, lose the packet rather than hold
               the whole film for it. */
            SDL_Log("movie: video queue full, dropping a packet");
            av_packet_unref(movie.packet);
            return 1;
        }
        {
            int index =
                (movie.video_queue_head + movie.video_queue_count) %
                MOVIE_VIDEO_QUEUE_MAX;

            av_packet_move_ref(&movie.video_queue[index], movie.packet);
            movie.video_queue_count++;
        }
        return 1;
    }
    av_packet_unref(movie.packet);
    return 1;
}

/* Hands the video decoder the next picture packet, from the queue when the
   sound was read ahead of the picture, else from the file.  Returns 0 at the end
   of the stream. */
static int Movie_NextVideoPacket(void) {
    for (;;) {
        if (movie.video_queue_count > 0) {
            av_packet_move_ref(movie.packet,
                               &movie.video_queue[movie.video_queue_head]);
            movie.video_queue_head =
                (movie.video_queue_head + 1) % MOVIE_VIDEO_QUEUE_MAX;
            movie.video_queue_count--;
            return 1;
        }
        if (movie.at_end) {
            return 0;
        }
        if (!Movie_ReadPacket()) {
            return 0;
        }
    }
}

/* Pulls packets until one video frame comes out, feeding audio as it goes.
   Returns 0 at the end of the stream. */
static int Movie_DecodeFrame(void) {
    for (;;) {
        int result;

        if (!Movie_NextVideoPacket()) {
            if (avcodec_send_packet(movie.video, NULL) >= 0 &&
                avcodec_receive_frame(movie.video, movie.frame) == 0) {
                return 1;
            }
            return 0;
        }
        result = avcodec_send_packet(movie.video, movie.packet);
        av_packet_unref(movie.packet);
        if (result >= 0 && avcodec_receive_frame(movie.video, movie.frame) == 0) {
            return 1;
        }
    }
}

/* The films interleave their sound well behind the picture - OPN.OGV puts the
   whole first second of picture before any of its sound in the file - so reading
   the file in step with the picture would start the film silent and leave the
   whole soundtrack that far behind, which is exactly how the sound used to run a
   second late.  This reads packets ahead of the picture until the sound in hand
   reaches `target_ms`, queueing the picture packets met on the way so the film
   still shows every frame, in order.  It runs once before the first frame (so
   the sound that belongs at the start is there when the track starts) and then
   every frame, keeping a margin between the sound and the picture above zero.
   The old SDL stream path is left to start its own device and is not read
   ahead. */
static void Movie_ReadAhead(int target_ms) {
    size_t target = (size_t)MOVIE_AUDIO_RATE * (size_t)target_ms / 1000 *
                    (size_t)MOVIE_AUDIO_FRAME_BYTES;

    if (movie.sound || !movie.audio_io) {
        return;
    }
    while (!movie.at_end && Movie_AudioQueued() < target &&
           movie.video_queue_count < MOVIE_VIDEO_QUEUE_MAX) {
        if (!Movie_ReadPacket()) {
            break;
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
    movie.started = 0;
    movie.start_ticks = 0;
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

/* True once the mixer's track has played every sample the decoder handed it,
   silence padding included, so releasing the movie cannot cut the last note. */
static int Movie_MixerDrained(void) {
    Sint64 played = Sound_GetMovieAudioPosition();
    Uint64 silence_frames;
    Uint64 fed_frames;
    int drained;

    if (played < 0) {
        return 1;
    }
    if (!movie.audio_lock) {
        return 1;
    }
    SDL_LockMutex(movie.audio_lock);
    silence_frames = movie.silence_bytes / (Uint64)MOVIE_AUDIO_FRAME_BYTES;
    fed_frames = movie.fed_audio_frames;
    SDL_UnlockMutex(movie.audio_lock);

    drained = (Uint64)played >= fed_frames + silence_frames;
    return drained;
}

/* The picture is over.  Decoded sound may still be waiting for the device, and
   the last second of a cutscene should not be cut off, so keep feeding it for a
   moment before the movie reports itself finished. */
static void Movie_Finish(void) {
    if (SDL_GetTicks() < movie.drain_deadline) {
        if (movie.sound) {
            if (SDL_GetAudioStreamQueued(movie.sound) > 0) {
                return;
            }
        } else if (!Movie_MixerDrained()) {
            return;
        }
    }
    Movie_Release();
}

/* Where the film's sound is, in seconds of the film, from what the mixer has
   played on the movie's track.  The samples that had to be padded with silence
   while the decoder was behind are subtracted, since they are not part of the
   film; the figure still leads the speaker by the device's own buffer, which is
   the small print of any mixer path. */
static double Movie_AudioFilmPosition(void) {
    Sint64 played = Sound_GetMovieAudioPosition();
    Uint64 silence_frames = movie.silence_bytes / (Uint64)MOVIE_AUDIO_FRAME_BYTES;

    if (played < 0 || (Uint64)played < silence_frames) {
        return 0.0;
    }
    return (double)((Uint64)played - silence_frames) / (double)MOVIE_AUDIO_RATE;
}

/* MOVIE_DEBUG=1 in the environment logs, once a second, where the picture and
   the sound are, so a report of "the sound is late" can be checked without a
   debugger.  `sound` is the film position of the sound that has been played
   (mixer path) or of what is queued for the stream (old path), `queued` is how
   much sound is still waiting, and the last word says which path is running. */
static void Movie_DebugTrace(double elapsed, double clock, double video_pos) {
    static int enabled = -1;
    static Uint64 last_log;
    Uint64 now = SDL_GetTicks();
    double sound;
    double queued;

    if (enabled < 0) {
        const char *option = SDL_getenv("MOVIE_DEBUG");
        enabled = (option && *option && *option != '0') ? 1 : 0;
    }
    if (!enabled || now - last_log < 1000) {
        return;
    }
    last_log = now;
    if (!movie.audio) {
        SDL_Log("movie: t=%.2f picture=%.2f sound=- (silent film)", elapsed,
                video_pos);
        return;
    }
    if (movie.sound) {
        queued = (double)SDL_GetAudioStreamQueued(movie.sound);
        sound = clock - queued / (MOVIE_AUDIO_FRAME_BYTES * MOVIE_AUDIO_RATE);
        SDL_Log("movie: t=%.2f picture=%.2f sound=%.2f (offset %.0fms, queued "
                "%.0fms, device %.0fms, stream)",
                elapsed, video_pos, sound, (elapsed - sound) * 1000.0,
                queued * 1000.0 / (MOVIE_AUDIO_FRAME_BYTES * MOVIE_AUDIO_RATE),
                movie.device_latency * 1000.0);
        return;
    }
    queued = (double)Movie_AudioQueued();
    sound = Movie_AudioFilmPosition();
    SDL_Log("movie: t=%.2f picture=%.2f sound=%.2f (offset %.0fms, queued %.0fms, "
            "mixer)",
            elapsed, video_pos, sound, (elapsed - sound) * 1000.0,
            queued * 1000.0 / (MOVIE_AUDIO_FRAME_BYTES * MOVIE_AUDIO_RATE));
}

void Movie_Update(SDL_Renderer *renderer) {
    double elapsed;
    double clock;
    double stamp;
    int shown = 0;
    int dropped = 0;

    if (!movie.playing) {
        return;
    }
    elapsed = movie.started
                  ? (double)(SDL_GetTicks() - movie.start_ticks) / 1000.0
                  : 0.0;

    Movie_ReadAhead(movie.started ? MOVIE_AUDIO_AHEAD_MS
                                  : MOVIE_AUDIO_PREFILL_MS);

    if (movie.video_done) {
        Movie_Finish();
        return;
    }
    /* The picture runs on the wall clock, one frame per frame time, like the
       game itself: it cannot stutter waiting for the audio device and cannot be
       stretched by one.  The sound is kept level with it by the read-ahead above
       and by the mixer path (docs/movie_audio_plan.md); only a hardware audio
       clock that runs at a different rate than the system clock can still make
       the two drift apart, by a fraction of a percent. */
    clock = elapsed;

    /* Behind the clock (a slow machine, or the start of a file): throw frames
       away until the picture is back where it belongs, instead of showing them
       late and letting the picture drift away from the sound. */
    while (clock - movie.next_frame_time > movie.frame_duration &&
           dropped < 8) {

        if (!Movie_DecodeFrame()) {
            movie.video_done = 1;
            movie.drain_deadline = SDL_GetTicks() + 3000;
            Movie_Finish();
            return;
        }
        stamp = (movie.frame->best_effort_timestamp == AV_NOPTS_VALUE)
                    ? movie.next_frame_time
                    : (double)movie.frame->best_effort_timestamp *
                          movie.time_base;
        movie.next_frame_time = stamp + movie.frame_duration;
        av_frame_unref(movie.frame);
        ++dropped;
    }

    /* Show every frame whose time has come; two per tick at most, so a slow
       frame cannot make the movie run away. */
    while (shown < 2 && clock >= movie.next_frame_time) {
        double stamp;
        double shown_pos;

        if (!Movie_DecodeFrame()) {
            movie.video_done = 1;
            movie.drain_deadline = SDL_GetTicks() + 3000;
            Movie_Finish();
            return;
        }
        Movie_Present(renderer);
        ++shown;
        if (!movie.started) {
            movie.started = 1;
            movie.start_ticks = SDL_GetTicks();
            /* The picture and the sound start together, from the first frame
               that is on the screen: the film's sound track is started here, so
               the mixer cannot run ahead of the picture while the first frame is
               still being decoded. */
            if (!movie.sound) {
                Sound_PlayMovieAudio();
            }
        }

        stamp = (movie.frame->best_effort_timestamp == AV_NOPTS_VALUE)
                    ? movie.next_frame_time
                    : (double)movie.frame->best_effort_timestamp *
                          movie.time_base;
        shown_pos = stamp;
        Movie_DebugTrace(elapsed, clock, shown_pos);
        movie.next_frame_time = stamp + movie.frame_duration;
        av_frame_unref(movie.frame);
    }
}

void Movie_Shutdown(void) { Movie_Release(); }

#endif /* SONICCD_HAVE_FFMPEG */
