#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "support/test_runner.h"
#include "types.h"

Uint8 generate_flag;
short_union stageno;
Uint8 time_flag;
Uint16 blockwk[1024][4];
Uint16 *pmapwk;
Uint8 boss_sound;

static int memset_count;
static void *memset_ptrs[140];
static Uint8 memset_values[140];
static Sint32 memset_sizes[140];
static int open_count;
static char open_paths[8][96];
static Sint32 open_result;
static int read_count;
static Sint32 read_handles[140];
static void *read_ptrs[140];
static Sint32 read_sizes[140];
static Sint32 read_fail_after;
static int close_count;
static Sint32 close_handles[8];
static int debug_count;
static char debug_messages[8][128];
static int wave_count;
static Sint16 wave_requests[16];
static int cdplay_count;
static Sint16 cdplay_requests[16];
static Uint16 scroll_map_storage[128 * 256];

static void memset_callback(void *ptr, Uint8 value, Sint32 size);
static Sint32 open_callback(char *path);
static Sint32 read_callback(Sint32 handle, void *ptr, Sint32 size);
static void close_callback(Sint32 handle);
static void debug_callback(char *message);
static void printf_callback(char *buffer, const char *format, ...);
static void wave_callback(Sint16 request);
static void cdplay_callback(Sint16 request);

void (*sMemSet)(void *, Uint8, Sint32) = memset_callback;
Sint32 (*sOpenFile)(char *) = open_callback;
Sint32 (*sReadFile)(Sint32, void *, Sint32) = read_callback;
void (*sCloseFile)(Sint32) = close_callback;
void (*sOutputDebugString)(char *) = debug_callback;
void (*sPrintf)(char *, const char *, ...) = printf_callback;
void (*WaveRequest)(Sint16) = wave_callback;
void (*CDPlay)(Sint16) = cdplay_callback;

#include "src/loader2.c"

static void memset_callback(void *ptr, Uint8 value, Sint32 size) {
    if (memset_count < 140) {
        memset_ptrs[memset_count] = ptr;
        memset_values[memset_count] = value;
        memset_sizes[memset_count] = size;
    }
    ++memset_count;
    memset(ptr, value, (size_t)size);
}

static Sint32 open_callback(char *path) {
    if (open_count < 8) {
        strcpy(open_paths[open_count], path);
    }
    ++open_count;
    return open_result;
}

static Sint32 read_callback(Sint32 handle, void *ptr, Sint32 size) {
    if (read_count < 140) {
        read_handles[read_count] = handle;
        read_ptrs[read_count] = ptr;
        read_sizes[read_count] = size;
    }
    if (read_fail_after >= 0 && read_count >= read_fail_after) {
        ++read_count;
        return -1;
    }
    memset(ptr, 0x5A, (size_t)size);
    ++read_count;
    return size;
}

static void close_callback(Sint32 handle) {
    if (close_count < 8) {
        close_handles[close_count] = handle;
    }
    ++close_count;
}

static void debug_callback(char *message) {
    if (debug_count < 8) {
        strcpy(debug_messages[debug_count], message);
    }
    ++debug_count;
}

static void printf_callback(char *buffer, const char *format, ...) {
    va_list args;

    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
}

static void wave_callback(Sint16 request) {
    if (wave_count < 16) {
        wave_requests[wave_count] = request;
    }
    ++wave_count;
}

static void cdplay_callback(Sint16 request) {
    if (cdplay_count < 16) {
        cdplay_requests[cdplay_count] = request;
    }
    ++cdplay_count;
}

static void reset_loader2_state(void) {
    generate_flag = 0;
    memset(&stageno, 0, sizeof(stageno));
    time_flag = 0;
    memset(blockwk, 0, sizeof(blockwk));
    memset(scroll_map_storage, 0, sizeof(scroll_map_storage));
    pmapwk = scroll_map_storage;
    boss_sound = 0;
    memset_count = 0;
    memset(memset_ptrs, 0, sizeof(memset_ptrs));
    memset(memset_values, 0, sizeof(memset_values));
    memset(memset_sizes, 0, sizeof(memset_sizes));
    open_count = 0;
    memset(open_paths, 0, sizeof(open_paths));
    open_result = 17;
    read_count = 0;
    memset(read_handles, 0, sizeof(read_handles));
    memset(read_ptrs, 0, sizeof(read_ptrs));
    memset(read_sizes, 0, sizeof(read_sizes));
    read_fail_after = -1;
    close_count = 0;
    memset(close_handles, 0, sizeof(close_handles));
    debug_count = 0;
    memset(debug_messages, 0, sizeof(debug_messages));
    wave_count = 0;
    memset(wave_requests, 0, sizeof(wave_requests));
    cdplay_count = 0;
    memset(cdplay_requests, 0, sizeof(cdplay_requests));
}

static void test_get_round_str_formats_time_zones(test_context *ctx) {
    char buffer[8];

    reset_loader2_state();
    GetRoundStr(0x0000, 0, buffer);
    TEST_ASSERT_TRUE(ctx, strcmp("11B", buffer) == 0);

    GetRoundStr(0x0102, 1, buffer);
    TEST_ASSERT_TRUE(ctx, strcmp("33A", buffer) == 0);

    generate_flag = 0;
    GetRoundStr(0x0203, 2, buffer);
    TEST_ASSERT_TRUE(ctx, strcmp("44D", buffer) == 0);

    generate_flag = 1;
    GetRoundStr(0x0203, 2, buffer);
    TEST_ASSERT_TRUE(ctx, strcmp("44C", buffer) == 0);

    buffer[0] = 'x';
    buffer[1] = 'x';
    buffer[2] = 'x';
    buffer[3] = 'x';
    GetRoundStr(0x0000, 3, buffer);
    TEST_ASSERT_TRUE(ctx, strcmp("11", buffer) == 0);
}

static void test_read_block_map_success_reads_and_reports_path(
    test_context *ctx) {
    reset_loader2_state();
    stageno.w = 0x0000;
    time_flag = 0;
    open_result = 42;

    ReadBlockMap();

    TEST_ASSERT_EQ_INT(ctx, 1, memset_count);
    TEST_ASSERT_TRUE(ctx, memset_ptrs[0] == blockwk);
    TEST_ASSERT_EQ_INT(ctx, 0, memset_values[0]);
    TEST_ASSERT_EQ_INT(ctx, sizeof(blockwk), memset_sizes[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, open_count);
    TEST_ASSERT_TRUE(ctx, strcmp("../R1/11B/T11B.MAP", open_paths[0]) == 0);
    TEST_ASSERT_EQ_INT(ctx, 1, read_count);
    TEST_ASSERT_EQ_INT(ctx, 42, read_handles[0]);
    TEST_ASSERT_TRUE(ctx, read_ptrs[0] == blockwk);
    TEST_ASSERT_EQ_INT(ctx, sizeof(blockwk), read_sizes[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, close_count);
    TEST_ASSERT_EQ_INT(ctx, 42, close_handles[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, debug_count);
    TEST_ASSERT_TRUE(ctx, strcmp("Block Map Read:../R1/11B/T11B.MAP\n",
                                 debug_messages[0]) == 0);
    TEST_ASSERT_EQ_INT(ctx, 0x5A5A, blockwk[0][0]);
    TEST_ASSERT_EQ_INT(ctx, 0x5A5A, blockwk[1023][3]);
}

static void test_read_block_map_reports_open_failure(test_context *ctx) {
    reset_loader2_state();
    stageno.w = 0x0102;
    time_flag = 1;
    open_result = -1;

    ReadBlockMap();

    TEST_ASSERT_EQ_INT(ctx, 1, memset_count);
    TEST_ASSERT_EQ_INT(ctx, 1, open_count);
    TEST_ASSERT_TRUE(ctx, strcmp("../R3/33A/T33A.MAP", open_paths[0]) == 0);
    TEST_ASSERT_EQ_INT(ctx, 0, read_count);
    TEST_ASSERT_EQ_INT(ctx, 0, close_count);
    TEST_ASSERT_EQ_INT(ctx, 1, debug_count);
    TEST_ASSERT_TRUE(ctx, strcmp("Block Map Read Error:../R3/33A/T33A.MAP\n",
                                 debug_messages[0]) == 0);
}

static void test_read_scroll_map_success_reads_until_failure(
    test_context *ctx) {
    reset_loader2_state();
    stageno.w = 0x0203;
    time_flag = 2;
    generate_flag = 1;
    open_result = 9;
    read_fail_after = 3;
    memset(scroll_map_storage, 0xAA, sizeof(scroll_map_storage));

    ReadScrolMap();

    TEST_ASSERT_EQ_INT(ctx, 128, memset_count);
    TEST_ASSERT_TRUE(ctx, memset_ptrs[0] == pmapwk);
    TEST_ASSERT_TRUE(ctx, memset_ptrs[127] == pmapwk);
    TEST_ASSERT_EQ_INT(ctx, 512, memset_sizes[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, open_count);
    TEST_ASSERT_TRUE(ctx, strcmp("../R4/44C/MAP44C.BIN", open_paths[0]) == 0);
    TEST_ASSERT_EQ_INT(ctx, 4, read_count);
    TEST_ASSERT_EQ_INT(ctx, 9, read_handles[0]);
    TEST_ASSERT_TRUE(ctx, read_ptrs[0] == &scroll_map_storage[0]);
    TEST_ASSERT_TRUE(ctx, read_ptrs[1] == &scroll_map_storage[256]);
    TEST_ASSERT_TRUE(ctx, read_ptrs[2] == &scroll_map_storage[512]);
    TEST_ASSERT_TRUE(ctx, read_ptrs[3] == &scroll_map_storage[768]);
    TEST_ASSERT_EQ_INT(ctx, 512, read_sizes[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, close_count);
    TEST_ASSERT_EQ_INT(ctx, 9, close_handles[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, debug_count);
    TEST_ASSERT_TRUE(ctx, strcmp("Scroll Map Read:../R4/44C/MAP44C.BIN\n",
                                 debug_messages[0]) == 0);
}

static void test_read_scroll_map_reports_open_failure(test_context *ctx) {
    reset_loader2_state();
    stageno.w = 0x0000;
    time_flag = 0;
    open_result = -1;
    memset(scroll_map_storage, 0xAA, sizeof(scroll_map_storage));

    ReadScrolMap();

    TEST_ASSERT_EQ_INT(ctx, 128, memset_count);
    TEST_ASSERT_TRUE(ctx, memset_ptrs[0] == pmapwk);
    TEST_ASSERT_TRUE(ctx, memset_ptrs[127] == pmapwk);
    TEST_ASSERT_EQ_INT(ctx, 512, memset_sizes[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, scroll_map_storage[0]);
    TEST_ASSERT_EQ_INT(ctx, 0xAAAA, scroll_map_storage[256]);
    TEST_ASSERT_EQ_INT(ctx, 1, open_count);
    TEST_ASSERT_TRUE(ctx, strcmp("../R1/11B/MAP11B.BIN", open_paths[0]) == 0);
    TEST_ASSERT_EQ_INT(ctx, 0, read_count);
    TEST_ASSERT_EQ_INT(ctx, 0, close_count);
    TEST_ASSERT_EQ_INT(ctx, 1, debug_count);
    TEST_ASSERT_TRUE(ctx, strcmp("Scroll Map Read Error:../R1/11B/MAP11B.BIN\n",
                                 debug_messages[0]) == 0);
}

static void test_soundset_maps_requests_and_ignores_171(test_context *ctx) {
    reset_loader2_state();

    soundset(144);
    soundset(155);
    soundset(171);

    TEST_ASSERT_EQ_INT(ctx, 2, wave_count);
    TEST_ASSERT_EQ_INT(ctx, 0, wave_requests[0]);
    TEST_ASSERT_EQ_INT(ctx, 11, wave_requests[1]);
}

static void test_sub_sync_routes_sound_requests(test_context *ctx) {
    reset_loader2_state();

    sub_sync(103);
    sub_sync(118);
    sub_sync(15);
    sub_sync(48);
    sub_sync(102);
    sub_sync(113);
    sub_sync(114);
    sub_sync(146);
    sub_sync(200);

    TEST_ASSERT_EQ_INT(ctx, 1, boss_sound);
    TEST_ASSERT_EQ_INT(ctx, 2, wave_count);
    TEST_ASSERT_EQ_INT(ctx, 75, wave_requests[0]);
    TEST_ASSERT_EQ_INT(ctx, 85, wave_requests[1]);
    TEST_ASSERT_EQ_INT(ctx, 6, cdplay_count);
    TEST_ASSERT_EQ_INT(ctx, 24, cdplay_requests[0]);
    TEST_ASSERT_EQ_INT(ctx, 3, cdplay_requests[1]);
    TEST_ASSERT_EQ_INT(ctx, 36, cdplay_requests[2]);
    TEST_ASSERT_EQ_INT(ctx, 23, cdplay_requests[3]);
    TEST_ASSERT_EQ_INT(ctx, 2, cdplay_requests[4]);
    TEST_ASSERT_EQ_INT(ctx, 34, cdplay_requests[5]);
}

TEST_MAIN_BEGIN;
    test_get_round_str_formats_time_zones(&ctx);
    test_read_block_map_success_reads_and_reports_path(&ctx);
    test_read_block_map_reports_open_failure(&ctx);
    test_read_scroll_map_success_reads_until_failure(&ctx);
    test_read_scroll_map_reports_open_failure(&ctx);
    test_soundset_maps_requests_and_ignores_171(&ctx);
    test_sub_sync_routes_sound_requests(&ctx);
TEST_MAIN_END;
