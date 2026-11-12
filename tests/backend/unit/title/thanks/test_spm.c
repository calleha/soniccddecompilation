/* <stdlib.h> declares POSIX random(); the decompiled thanks headers declare
   the game's own Sint32 random(void).  Parse stdlib first, then rename the game
   symbol inside this harness so the two can coexist. */
#include <stdlib.h>

#define random game_random

#include <string.h>

#include "support/test_runner.h"
#include "src/types.h"

void (*sMemSet)(void *, Uint8, Sint32);

static void *memset_ptrs[8];
static Uint8 memset_values[8];
static Sint32 memset_sizes[8];
static Sint32 memset_call_count;
static Sint32 fade_flag_storage;

#include "src/title/thanks/spm.c"

static sprite_status_thanks *dummy_priptr_values[4];

static void fake_memset(void *dst, Uint8 value, Sint32 size) {
    memset_ptrs[memset_call_count] = dst;
    memset_values[memset_call_count] = value;
    memset_sizes[memset_call_count] = size;
    ++memset_call_count;
    memset(dst, value, (size_t)size);
}

static void reset_fixture(void) {
    memset(memset_ptrs, 0, sizeof(memset_ptrs));
    memset(memset_values, 0, sizeof(memset_values));
    memset(memset_sizes, 0, sizeof(memset_sizes));
    memset(dummy_priptr_values, 0, sizeof(dummy_priptr_values));
    memset_call_count = 0;
    fade_flag_storage = 77;
    lpFadeFlag = &fade_flag_storage;
    sMemSet = fake_memset;

    FadeFlag = 77;
    DLLRetMode = 77;
    ProcMode = 77;
    quit_flg = 77;
    push_flg = 77;
    int_flg = 77;
    colorflg = 77;
    colorno = 77;
    coltimer = 77;
    linkdata = 77;
    intmode = 77;
    gmtime0 = 77;
    gmtime1 = 77;
    sr_save = 77;
    ranum = 77;
    overint = 77;
    startcolor = 77;
    colorcnt = 77;
    prg_mode = 77;
    wkarea1 = 77;
    wkarea2 = 77;
    wkarea3 = 77;
    colorno2 = 77;
    coltimer2 = 77;
    posetimer = 77;
    poseno = 77;
    animal_flg = 77;
    animal_timer = 77;
    heart_tm = 77;
    end_timer = 77;
    comflag_m = 77;
    comflag_s = 77;
    comdata_m0 = 77;
    comdata_m1 = 77;
    comdata_m2 = 77;
    comdata_m3 = 77;
    comdata_m4 = 77;
    comdata_m5 = 77;
    comdata_m6 = 77;
    comdata_m7 = 77;
    swdata1.w = 77;
    swdata2.w = 77;
    comdata_s0 = 77;
    comdata_s1 = 77;
    comdata_s2 = 77;
    comdata_s3 = 77;
    comdata_s4 = 77;
    comdata_s5 = 77;
    comdata_s6 = 77;
    comdata_s7 = 77;

    for (Sint32 i = 0; i < 8; ++i) {
        actset_tm[i] = 77;
    }
    for (Sint32 i = 0; i < 4; ++i) {
        priptr[i] = &dummy_priptr_values[i];
    }
}

static void assert_scalar_state_cleared(test_context *ctx) {
    TEST_ASSERT_EQ_INT(ctx, 0, FadeFlag);
    TEST_ASSERT_EQ_INT(ctx, 0, fade_flag_storage);
    TEST_ASSERT_EQ_INT(ctx, 0, DLLRetMode);
    TEST_ASSERT_EQ_INT(ctx, 0, ProcMode);
    TEST_ASSERT_EQ_INT(ctx, 0, quit_flg);
    TEST_ASSERT_EQ_INT(ctx, 0, push_flg);
    TEST_ASSERT_EQ_INT(ctx, 0, int_flg);
    TEST_ASSERT_EQ_INT(ctx, 0, colorflg);
    TEST_ASSERT_EQ_INT(ctx, 0, colorno);
    TEST_ASSERT_EQ_INT(ctx, 0, coltimer);
    TEST_ASSERT_EQ_INT(ctx, 0, linkdata);
    TEST_ASSERT_EQ_INT(ctx, 0, intmode);
    TEST_ASSERT_EQ_INT(ctx, 0, gmtime0);
    TEST_ASSERT_EQ_INT(ctx, 0, gmtime1);
    TEST_ASSERT_EQ_INT(ctx, 0, sr_save);
    TEST_ASSERT_EQ_INT(ctx, 0, ranum);
    TEST_ASSERT_EQ_INT(ctx, 0, overint);
    TEST_ASSERT_EQ_INT(ctx, 0, startcolor);
    TEST_ASSERT_EQ_INT(ctx, 0, colorcnt);
    TEST_ASSERT_EQ_INT(ctx, 0, prg_mode);
    TEST_ASSERT_EQ_INT(ctx, 0, wkarea1);
    TEST_ASSERT_EQ_INT(ctx, 0, wkarea2);
    TEST_ASSERT_EQ_INT(ctx, 0, wkarea3);
    TEST_ASSERT_EQ_INT(ctx, 0, colorno2);
    TEST_ASSERT_EQ_INT(ctx, 0, coltimer2);
    TEST_ASSERT_EQ_INT(ctx, 0, posetimer);
    TEST_ASSERT_EQ_INT(ctx, 0, poseno);
    TEST_ASSERT_EQ_INT(ctx, 0, animal_flg);
    TEST_ASSERT_EQ_INT(ctx, 0, animal_timer);
    TEST_ASSERT_EQ_INT(ctx, 0, heart_tm);
    TEST_ASSERT_EQ_INT(ctx, 0, end_timer);
    TEST_ASSERT_EQ_INT(ctx, 0, comflag_m);
    TEST_ASSERT_EQ_INT(ctx, 0, comflag_s);
    TEST_ASSERT_EQ_INT(ctx, 0, comdata_m0);
    TEST_ASSERT_EQ_INT(ctx, 0, comdata_m1);
    TEST_ASSERT_EQ_INT(ctx, 0, comdata_m2);
    TEST_ASSERT_EQ_INT(ctx, 0, comdata_m3);
    TEST_ASSERT_EQ_INT(ctx, 0, comdata_m4);
    TEST_ASSERT_EQ_INT(ctx, 0, comdata_m5);
    TEST_ASSERT_EQ_INT(ctx, 0, comdata_m6);
    TEST_ASSERT_EQ_INT(ctx, 0, comdata_m7);
    TEST_ASSERT_EQ_INT(ctx, 0, swdata1.w);
    TEST_ASSERT_EQ_INT(ctx, 0, swdata2.w);
    TEST_ASSERT_EQ_INT(ctx, 0, comdata_s0);
    TEST_ASSERT_EQ_INT(ctx, 0, comdata_s1);
    TEST_ASSERT_EQ_INT(ctx, 0, comdata_s2);
    TEST_ASSERT_EQ_INT(ctx, 0, comdata_s3);
    TEST_ASSERT_EQ_INT(ctx, 0, comdata_s4);
    TEST_ASSERT_EQ_INT(ctx, 0, comdata_s5);
    TEST_ASSERT_EQ_INT(ctx, 0, comdata_s6);
    TEST_ASSERT_EQ_INT(ctx, 0, comdata_s7);
}

static void test_work_ram_clear_resets_state_and_buffers(test_context *ctx) {
    reset_fixture();

    WorkRamClear();

    assert_scalar_state_cleared(ctx);
    TEST_ASSERT_TRUE(ctx, priptr[0] == 0);
    TEST_ASSERT_TRUE(ctx, priptr[1] == 0);
    TEST_ASSERT_TRUE(ctx, priptr[2] == 0);
    TEST_ASSERT_TRUE(ctx, priptr[3] == 0);
    for (Sint32 i = 0; i < 8; ++i) {
        TEST_ASSERT_EQ_INT(ctx, 0, actset_tm[i]);
    }

    TEST_ASSERT_EQ_INT(ctx, 6, memset_call_count);
    TEST_ASSERT_TRUE(ctx, memset_ptrs[0] == actwk);
    TEST_ASSERT_EQ_INT(ctx, 0, memset_values[0]);
    TEST_ASSERT_EQ_INT(ctx, sizeof(actwk), memset_sizes[0]);
    TEST_ASSERT_TRUE(ctx, memset_ptrs[1] == pribuf0);
    TEST_ASSERT_EQ_INT(ctx, sizeof(pribuf0), memset_sizes[1]);
    TEST_ASSERT_TRUE(ctx, memset_ptrs[2] == pribuf1);
    TEST_ASSERT_EQ_INT(ctx, sizeof(pribuf1), memset_sizes[2]);
    TEST_ASSERT_TRUE(ctx, memset_ptrs[3] == pribuf2);
    TEST_ASSERT_EQ_INT(ctx, sizeof(pribuf2), memset_sizes[3]);
    TEST_ASSERT_TRUE(ctx, memset_ptrs[4] == pribuf3);
    TEST_ASSERT_EQ_INT(ctx, sizeof(pribuf3), memset_sizes[4]);
    TEST_ASSERT_TRUE(ctx, memset_ptrs[5] == actset_tm);
    TEST_ASSERT_EQ_INT(ctx, sizeof(actset_tm), memset_sizes[5]);
}

TEST_MAIN_BEGIN;
test_work_ram_clear_resets_state_and_buffers(&ctx);
TEST_MAIN_END
