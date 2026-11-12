/* <stdlib.h> declares POSIX random(); the decompiled title/planet headers
   declare the game's own Sint32 random(void).  Parse stdlib first, then rename
   the game symbol inside this harness so the two can coexist. */
#include <stdlib.h>

#define random game_random

#include <string.h>

#include "support/test_runner.h"
#include "types.h"

static int memset_calls;
static int memset_sizes[4];

static void stub_memset(void *dst, Uint8 value, Sint32 size) {
    if (memset_calls < (int)(sizeof(memset_sizes) / sizeof(memset_sizes[0])))
        memset_sizes[memset_calls] = size;
    ++memset_calls;
    memset(dst, value, size);
}

void (*sMemSet)(void *, Uint8, Sint32) = stub_memset;

#include "src/title/planet/spm_equ.c"

static Sint32 fade_flag_storage;

static void dirty_state(void) {
    lpFadeFlag = &fade_flag_storage;
    fade_flag_storage = 99;
    DLLRetMode = 1;
    ProcMode = 2;
    memset(actwk, 0x7F, sizeof(actwk));
    coltimer = 3;
    colorno = 4;
    funka_pctimer = 5;
    funka_patno = 6;
    main_lpcnt = 7;
    spr_coltimer = 8;
    spr_colno = 9;
    memset(actset_tm, 0x7F, sizeof(actset_tm));
    actset_flg = 10;
    actset_wk = 11;
    push_flg = 12;
    int_flg = 13;
    colorflg = 14;
    linkdata = 15;
    intmode = 16;
    gmtime0 = 17;
    gmtime1 = 18;
    sr_save = 19;
    ranum = 20;
    overint = 21;
    flash_timer = 22;
    lplanet_no = 23;
    startcolor = 24;
    colorcnt = 25;
    flash_flg = 26;
    cgchg_timer = 27;
    cgchg_end = 28;
    comflag_m = 29;
    comflag_s = 30;
    comdata_m0 = 31;
    comdata_m1 = 32;
    comdata_m2 = 33;
    comdata_m3 = 34;
    comdata_m4 = 35;
    comdata_m5 = 36;
    comdata_m6 = 37;
    comdata_m7 = 38;
    swdata1.w = 39;
    swdata2.w = 40;
    comdata_s0 = 41;
    comdata_s1 = 42;
    comdata_s2 = 43;
    comdata_s3 = 44;
    comdata_s4 = 45;
    comdata_s5 = 46;
    comdata_s6 = 47;
    comdata_s7 = 48;
    memset_calls = 0;
    memset_sizes[0] = 0;
    memset_sizes[1] = 0;
}

static void test_work_ram_clear_resets_mode_actor_and_palette_state(
    test_context *ctx) {
    dirty_state();

    WorkRamClear();

    TEST_ASSERT_EQ_INT(ctx, 0, fade_flag_storage);
    TEST_ASSERT_EQ_INT(ctx, 0, DLLRetMode);
    TEST_ASSERT_EQ_INT(ctx, 0, ProcMode);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[17].timer2);
    TEST_ASSERT_EQ_INT(ctx, 0, coltimer);
    TEST_ASSERT_EQ_INT(ctx, 0, colorno);
    TEST_ASSERT_EQ_INT(ctx, 0, funka_pctimer);
    TEST_ASSERT_EQ_INT(ctx, 0, funka_patno);
    TEST_ASSERT_EQ_INT(ctx, 0, main_lpcnt);
    TEST_ASSERT_EQ_INT(ctx, 0, spr_coltimer);
    TEST_ASSERT_EQ_INT(ctx, 0, spr_colno);
    TEST_ASSERT_EQ_INT(ctx, 2, memset_calls);
    TEST_ASSERT_EQ_INT(ctx, sizeof(actwk), memset_sizes[0]);
    TEST_ASSERT_EQ_INT(ctx, 6, memset_sizes[1]);
}

static void test_work_ram_clear_resets_flags_timers_and_controller_state(
    test_context *ctx) {
    dirty_state();

    WorkRamClear();

    TEST_ASSERT_EQ_INT(ctx, 0, actset_tm[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, actset_tm[1]);
    TEST_ASSERT_EQ_INT(ctx, 0, actset_tm[2]);
    TEST_ASSERT_EQ_INT(ctx, 0, actset_flg);
    TEST_ASSERT_EQ_INT(ctx, 0, actset_wk);
    TEST_ASSERT_EQ_INT(ctx, 0, push_flg);
    TEST_ASSERT_EQ_INT(ctx, 0, int_flg);
    TEST_ASSERT_EQ_INT(ctx, 0, colorflg);
    TEST_ASSERT_EQ_INT(ctx, 0, linkdata);
    TEST_ASSERT_EQ_INT(ctx, 0, intmode);
    TEST_ASSERT_EQ_INT(ctx, 0, gmtime0);
    TEST_ASSERT_EQ_INT(ctx, 0, gmtime1);
    TEST_ASSERT_EQ_INT(ctx, 0, sr_save);
    TEST_ASSERT_EQ_INT(ctx, 0, ranum);
    TEST_ASSERT_EQ_INT(ctx, 0, overint);
    TEST_ASSERT_EQ_INT(ctx, 0, flash_timer);
    TEST_ASSERT_EQ_INT(ctx, 0, lplanet_no);
    TEST_ASSERT_EQ_INT(ctx, 0, startcolor);
    TEST_ASSERT_EQ_INT(ctx, 0, colorcnt);
    TEST_ASSERT_EQ_INT(ctx, 0, flash_flg);
    TEST_ASSERT_EQ_INT(ctx, 0, cgchg_timer);
    TEST_ASSERT_EQ_INT(ctx, 0, cgchg_end);
}

static void test_work_ram_clear_resets_command_words(test_context *ctx) {
    dirty_state();

    WorkRamClear();

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

TEST_MAIN_BEGIN;
test_work_ram_clear_resets_mode_actor_and_palette_state(&ctx);
test_work_ram_clear_resets_flags_timers_and_controller_state(&ctx);
test_work_ram_clear_resets_command_words(&ctx);
TEST_MAIN_END
