/* <stdlib.h> declares POSIX random(); the decompiled thanks headers declare
   the game's own Sint32 random(void).  Parse stdlib first, then rename the game
   symbol inside this harness so the two can coexist. */
#include <stdlib.h>

#define random game_random

#include <string.h>

#include "support/test_runner.h"
#include "src/types.h"

static Sint32 mem_set_call_count;
static Sint32 mem_cmp_call_count;
static Sint32 mem_cpy_call_count;
static Sint32 strcpy_call_count;
static Sint32 put_ascii_count;
static Uint16 put_ascii_c[4];
static Uint16 put_ascii_x[4];
static Uint16 put_ascii_y[4];
static Sint32 sprinit_call_count;
static Sint32 action_call_count;

#include "src/title/thanks/bm_m.c"

Sint16 prg_mode;
Sint16 actset_tm[8];
score_data test_score_data;
score_data *lpScorData;
Uint8 quit_flg;
short_union swdata1;

void (*CDPause)(Sint16) = 0;
void (*CDPlay)(Sint16) = 0;
void (*ChangeTileBmp)(Sint32, Sint32) = 0;
void (*sCloseFile)(Sint32) = 0;
void (*ClrSpriteDebug)(void) = 0;
void (*EAsprset)(Sint16, Sint16, Uint16, Uint16, Uint16) = 0;
Sint32 (*sGetFileSize)(Sint32) = 0;
void *(*sMemAlloc)(Sint32) = 0;
Sint32 (*sReadFile)(Sint32, void *, Sint32) = 0;
Sint32 (*ReadIndx)(Uint32) = 0;
Uint32 (*ReadScore)(Sint32, char *, Uint32) = 0;
Sint32 (*sOpenFile)(char *) = 0;
Uint32 (*OpenScore)(Sint32) = 0;
void (*sOutputDebugString)(char *) = 0;
void (*sPrintf)(char *, const char *, ...) = 0;
Sint32 (*sRandom)(void) = 0;
Sint32 (*SetGrid)(Sint32, Sint32, Sint32, Sint32, Sint32) = 0;
void (*WaveAllStop)(void) = 0;
void (*WaveRequest)(Sint16) = 0;
Uint32 (*WriteIndx)(Sint32, Uint32) = 0;
Uint32 (*WriteScore)(Sint32, char *, Uint32) = 0;

static void stub_mem_set(void *dst, Uint8 value, Sint32 size) {
    ++mem_set_call_count;
    memset(dst, value, (size_t)size);
}

static Sint32 stub_mem_cmp(void *lhs, void *rhs, Sint32 size) {
    ++mem_cmp_call_count;
    return memcmp(lhs, rhs, (size_t)size);
}

static void stub_mem_cpy(void *dst, void *src, Sint32 size) {
    ++mem_cpy_call_count;
    memcpy(dst, src, (size_t)size);
}

static char *stub_strcpy(char *dst, char *src) {
    ++strcpy_call_count;
    return strcpy(dst, src);
}

Sint32 (*sMemCmp)(void *, void *, Sint32) = stub_mem_cmp;
void (*sMemCpy)(void *, void *, Sint32) = stub_mem_cpy;
void (*sMemSet)(void *, Uint8, Sint32) = stub_mem_set;
char *(*sStrcpy)(char *, char *) = stub_strcpy;

void PutAscii(Uint16 c, Uint16 XPos, Uint16 YPos) {
    if (put_ascii_count < 4) {
        put_ascii_c[put_ascii_count] = c;
        put_ascii_x[put_ascii_count] = XPos;
        put_ascii_y[put_ascii_count] = YPos;
    }
    ++put_ascii_count;
}

void sprinit(void) { ++sprinit_call_count; }

void action(void) { ++action_call_count; }

static void set_name(char dst[4], const char *src) {
    memcpy(dst, src, 4);
}

static void fill_scores(void) {
    Sint16 round;
    Sint16 zone;

    memset(&test_score_data, 0, sizeof(test_score_data));
    for (round = 0; round < 7; ++round) {
        for (zone = 0; zone < 3; ++zone) {
            set_name(test_score_data.timeattack[round][zone][0].name, "AAA");
            set_name(test_score_data.timeattack[round][zone][1].name, "BBB");
            set_name(test_score_data.timeattack[round][zone][2].name, "CCC");
        }
        set_name(test_score_data.special[round][0].name, "DDD");
        set_name(test_score_data.special[round][1].name, "EEE");
        set_name(test_score_data.special[round][2].name, "FFF");
    }
}

static void reset_fixture(void) {
    memset(score, 0, sizeof(score));
    memset(GreatestPlayer, 0, sizeof(GreatestPlayer));
    memset(actset_tm, 0, sizeof(actset_tm));
    memset(&test_score_data, 0, sizeof(test_score_data));
    memset(put_ascii_c, 0, sizeof(put_ascii_c));
    memset(put_ascii_x, 0, sizeof(put_ascii_x));
    memset(put_ascii_y, 0, sizeof(put_ascii_y));
    lpScorData = &test_score_data;
    prg_mode = 0;
    quit_flg = 0;
    swdata1.w = 0;
    mem_set_call_count = 0;
    mem_cmp_call_count = 0;
    mem_cpy_call_count = 0;
    strcpy_call_count = 0;
    put_ascii_count = 0;
    sprinit_call_count = 0;
    action_call_count = 0;
}

static void test_pointcnt_creates_and_updates_entries(test_context *ctx) {
    char abc[4] = "ABC";
    char xyz[4] = "XYZ";

    reset_fixture();

    pointcnt(abc, 3);
    pointcnt(xyz, 2);
    pointcnt(abc, 5);

    TEST_ASSERT_EQ_INT(ctx, 8, score[0].pts);
    TEST_ASSERT_EQ_INT(ctx, 2, score[1].pts);
    TEST_ASSERT_TRUE(ctx, memcmp(score[0].name, "ABC", 4) == 0);
    TEST_ASSERT_TRUE(ctx, memcmp(score[1].name, "XYZ", 4) == 0);
    TEST_ASSERT_EQ_INT(ctx, 2, mem_cpy_call_count);
    TEST_ASSERT_TRUE(ctx, mem_cmp_call_count >= 2);
}

static void test_get_usrname_counts_all_scores_and_selects_greatest(
    test_context *ctx) {
    reset_fixture();
    fill_scores();

    get_usrname();

    TEST_ASSERT_TRUE(ctx, memcmp(GreatestPlayer, "AAA", 4) == 0);
    TEST_ASSERT_EQ_INT(ctx, 63, score[0].pts);
    TEST_ASSERT_EQ_INT(ctx, 42, score[1].pts);
    TEST_ASSERT_EQ_INT(ctx, 21, score[2].pts);
    TEST_ASSERT_EQ_INT(ctx, 21, score[3].pts);
    TEST_ASSERT_EQ_INT(ctx, 6, mem_cpy_call_count);
    TEST_ASSERT_EQ_INT(ctx, 1, strcpy_call_count);
}

static void test_character_class_helpers_cover_boundaries(test_context *ctx) {
    TEST_ASSERT_EQ_INT(ctx, 0, isdigit('0' - 1));
    TEST_ASSERT_EQ_INT(ctx, 1, isdigit('0'));
    TEST_ASSERT_EQ_INT(ctx, 1, isdigit('9'));
    TEST_ASSERT_EQ_INT(ctx, 0, isdigit('9' + 1));
    TEST_ASSERT_EQ_INT(ctx, 0, isupper('A' - 1));
    TEST_ASSERT_EQ_INT(ctx, 1, isupper('A'));
    TEST_ASSERT_EQ_INT(ctx, 1, isupper('Z'));
    TEST_ASSERT_EQ_INT(ctx, 0, isupper('Z' + 1));
}

static void test_dsp_usrname_draws_digits_and_uppercase_only(
    test_context *ctx) {
    reset_fixture();
    memcpy(GreatestPlayer, "A9!", 4);

    dsp_usrname();

    TEST_ASSERT_EQ_INT(ctx, 2, put_ascii_count);
    TEST_ASSERT_EQ_INT(ctx, 11, put_ascii_c[0]);
    TEST_ASSERT_EQ_INT(ctx, 17, put_ascii_x[0]);
    TEST_ASSERT_EQ_INT(ctx, 12, put_ascii_y[0]);
    TEST_ASSERT_EQ_INT(ctx, 10, put_ascii_c[1]);
    TEST_ASSERT_EQ_INT(ctx, 19, put_ascii_x[1]);
    TEST_ASSERT_EQ_INT(ctx, 12, put_ascii_y[1]);
}

static void test_get_tmdata_sets_timers_mode_and_draws_name(test_context *ctx) {
    reset_fixture();
    fill_scores();
    for (Sint32 i = 0; i < 8; ++i) {
        actset_tm[i] = 123;
    }

    get_tmdata();

    TEST_ASSERT_EQ_INT(ctx, 1, prg_mode);
    TEST_ASSERT_EQ_INT(ctx, 600, actset_tm[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, actset_tm[1]);
    TEST_ASSERT_EQ_INT(ctx, 0, actset_tm[2]);
    TEST_ASSERT_EQ_INT(ctx, 0, actset_tm[3]);
    TEST_ASSERT_EQ_INT(ctx, 0, actset_tm[4]);
    TEST_ASSERT_EQ_INT(ctx, 0, actset_tm[5]);
    TEST_ASSERT_EQ_INT(ctx, 0, actset_tm[6]);
    TEST_ASSERT_EQ_INT(ctx, 600, actset_tm[7]);
    TEST_ASSERT_EQ_INT(ctx, 3, put_ascii_count);
}

static void test_get_keydata_runs_player_sprite_init_and_action(
    test_context *ctx) {
    reset_fixture();
    swdata1.b.l = 0;

    get_keydata();

    TEST_ASSERT_EQ_INT(ctx, 0, quit_flg);
    TEST_ASSERT_EQ_INT(ctx, 1, sprinit_call_count);
    TEST_ASSERT_EQ_INT(ctx, 1, action_call_count);
}

static void test_player_sets_quit_for_high_nibble_input(test_context *ctx) {
    reset_fixture();
    swdata1.b.l = 0x10;

    player();

    TEST_ASSERT_EQ_INT(ctx, 1, quit_flg);

    swdata1.b.l = 0x0f;
    player();

    TEST_ASSERT_EQ_INT(ctx, 0, quit_flg);
}

static void test_mainloop_dispatches_by_program_mode(test_context *ctx) {
    reset_fixture();
    fill_scores();
    prg_mode = 0;

    mainloop();

    TEST_ASSERT_EQ_INT(ctx, 1, prg_mode);
    TEST_ASSERT_EQ_INT(ctx, 0, sprinit_call_count);
    TEST_ASSERT_EQ_INT(ctx, 0, action_call_count);

    mainloop();

    TEST_ASSERT_EQ_INT(ctx, 1, sprinit_call_count);
    TEST_ASSERT_EQ_INT(ctx, 1, action_call_count);
}

TEST_MAIN_BEGIN;
test_pointcnt_creates_and_updates_entries(&ctx);
test_get_usrname_counts_all_scores_and_selects_greatest(&ctx);
test_character_class_helpers_cover_boundaries(&ctx);
test_dsp_usrname_draws_digits_and_uppercase_only(&ctx);
test_get_tmdata_sets_timers_mode_and_draws_name(&ctx);
test_get_keydata_runs_player_sprite_init_and_action(&ctx);
test_player_sets_quit_for_high_nibble_input(&ctx);
test_mainloop_dispatches_by_program_mode(&ctx);
TEST_MAIN_END
