/* <stdlib.h> declares POSIX random(); the decompiled title/planet headers
   declare the game's own Sint32 random(void).  Parse stdlib first, then rename
   the game symbol inside this harness so the two can coexist. */
#include <stdlib.h>

#define random game_random

#include <string.h>

#include "support/test_runner.h"
#include "types.h"

#include "src/title/planet/asciiset.c"

static sprite_status_lpl *next_free_actor;
static Sint32 get_actwk_result;
static Sint32 chk_actwk_result;

ushort_union swdata1;
Uint8 push_flg;
Uint8 music_selflg;
Uint8 actset_flg;
Uint8 comflag_m;
Uint16 comdata_m4;
Uint16 comdata_m5;
Sint16 lplanet_no;
sprite_status_lpl actwk[18];

static sprite_pattern_mappings_title dummy_map = {1, {{7, 0, 0, 0, 0, 0}}};
static sprite_patterns_title dummy_pattern = {1, {{1, &dummy_map}}};
Sint16 ascii_xposi_tbl[78];
sprite_patterns_title *asciispr_tbl[39];

Sint32 get_actwk(sprite_status_lpl **ppFreeAct) {
    if (get_actwk_result == 0)
        *ppFreeAct = next_free_actor;
    return get_actwk_result;
}

Sint32 chk_actwk(void) { return chk_actwk_result; }

static void reset_state(void) {
    memset(actwk, 0, sizeof(actwk));
    memset(ascii_xposi_tbl, 0, sizeof(ascii_xposi_tbl));
    for (int i = 0; i < 39; ++i)
        asciispr_tbl[i] = &dummy_pattern;
    swdata1.w = 0;
    push_flg = 0;
    music_selflg = 0;
    actset_flg = 0;
    comflag_m = 0;
    comdata_m4 = 0;
    comdata_m5 = 0;
    lplanet_no = -1;
    next_free_actor = &actwk[3];
    get_actwk_result = 0;
    chk_actwk_result = 0;
    bit_SPACE = 0;
    space_flg = 0;
}

static void test_release_sets_selected_music(test_context *ctx) {
    reset_state();
    actwk[17].EXE_NO = 5;
    ascii_xposi_tbl[10] = 2;
    ascii_xposi_tbl[11] = 88;
    push_flg = 32;
    music_selflg = 1;
    comflag_m = 16;

    sprascii();

    TEST_ASSERT_EQ_INT(ctx, 0, push_flg);
    TEST_ASSERT_EQ_INT(ctx, 5, comdata_m4);
    TEST_ASSERT_EQ_INT(ctx, 2, comdata_m5);
    TEST_ASSERT_EQ_INT(ctx, 2, lplanet_no);
    TEST_ASSERT_EQ_INT(ctx, 32, comflag_m);
    TEST_ASSERT_EQ_INT(ctx, 0, actset_flg);
}

static void test_planet_three_does_not_replace_lplanet_no(test_context *ctx) {
    reset_state();
    actwk[17].EXE_NO = 7;
    ascii_xposi_tbl[14] = 3;
    lplanet_no = 9;
    push_flg = 16;
    music_selflg = 1;

    sprascii();

    TEST_ASSERT_EQ_INT(ctx, 7, comdata_m4);
    TEST_ASSERT_EQ_INT(ctx, 3, comdata_m5);
    TEST_ASSERT_EQ_INT(ctx, 9, lplanet_no);
    TEST_ASSERT_EQ_INT(ctx, 32, comflag_m);
}

static void test_release_without_music_enabled_only_clears_latches(
    test_context *ctx) {
    reset_state();
    push_flg = 32 | 16;

    sprascii();

    TEST_ASSERT_EQ_INT(ctx, 0, push_flg);
    TEST_ASSERT_EQ_INT(ctx, 0, comflag_m);
    TEST_ASSERT_EQ_INT(ctx, 0, comdata_m4);
}

static void test_start_release_while_music_enabled_selects_music(
    test_context *ctx) {
    reset_state();
    actwk[17].EXE_NO = 4;
    ascii_xposi_tbl[8] = 1;
    push_flg = 64;
    music_selflg = 1;

    sprascii();

    TEST_ASSERT_EQ_INT(ctx, 0, push_flg);
    TEST_ASSERT_EQ_INT(ctx, 4, comdata_m4);
    TEST_ASSERT_EQ_INT(ctx, 1, comdata_m5);
    TEST_ASSERT_EQ_INT(ctx, 1, lplanet_no);
    TEST_ASSERT_EQ_INT(ctx, 32, comflag_m);
}

static void test_start_input_waits_for_empty_actor_pool(test_context *ctx) {
    reset_state();
    push_flg = 64;
    chk_actwk_result = -1;

    sprascii();

    TEST_ASSERT_EQ_INT(ctx, 5, music_selflg);
    TEST_ASSERT_EQ_INT(ctx, 0, push_flg);
    TEST_ASSERT_EQ_INT(ctx, 0, comflag_m);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[17].ACT_NO);

    chk_actwk_result = 0;
    sprascii();

    TEST_ASSERT_EQ_INT(ctx, 1, music_selflg);
    TEST_ASSERT_EQ_INT(ctx, 16, comflag_m);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[17].ACT_NO);
}

static void test_space_input_uses_same_start_path(test_context *ctx) {
    reset_state();
    space_flg = 1;
    chk_actwk_result = 0;

    sprascii();

    TEST_ASSERT_EQ_INT(ctx, 1, music_selflg);
    TEST_ASSERT_EQ_INT(ctx, 16, comflag_m);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[17].ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, 0, space_flg);
}

static void test_space_release_while_music_enabled_selects_music(
    test_context *ctx) {
    reset_state();
    actwk[17].EXE_NO = 8;
    ascii_xposi_tbl[16] = 2;
    space_flg = 1;
    music_selflg = 1;

    sprascii();

    TEST_ASSERT_EQ_INT(ctx, 0, space_flg);
    TEST_ASSERT_EQ_INT(ctx, 8, comdata_m4);
    TEST_ASSERT_EQ_INT(ctx, 2, comdata_m5);
    TEST_ASSERT_EQ_INT(ctx, 2, lplanet_no);
    TEST_ASSERT_EQ_INT(ctx, 32, comflag_m);
}

static void test_held_inputs_set_push_latches(test_context *ctx) {
    reset_state();
    swdata1.b.h = 32 | 16 | 64;
    bit_SPACE = 1;

    ascchk(&actwk[17]);

    TEST_ASSERT_EQ_INT(ctx, 32 | 16 | 64, push_flg);
    TEST_ASSERT_EQ_INT(ctx, 1, space_flg);
}

static void test_ascii_change_state_is_noop(test_context *ctx) {
    reset_state();
    actwk[17].ACT_NO = 2;
    actwk[17].EXE_NO = 11;
    actwk[17].XPOSI.w.h = 77;

    sprascii();

    TEST_ASSERT_EQ_INT(ctx, 2, actwk[17].ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, 11, actwk[17].EXE_NO);
    TEST_ASSERT_EQ_INT(ctx, 77, actwk[17].XPOSI.w.h);
}

static void test_ascii_set_allocates_string_actor_slot(test_context *ctx) {
    reset_state();
    actwk[17].ACT_NO = 1;
    next_free_actor = &actwk[6];

    sprascii();

    TEST_ASSERT_EQ_INT(ctx, 6, actwk[17].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 2, music_selflg);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[17].ACT_NO);

    reset_state();
    actwk[17].ACT_NO = 1;
    get_actwk_result = -1;

    sprascii();

    TEST_ASSERT_EQ_INT(ctx, 1, actwk[17].ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, 0, music_selflg);
}

static void test_set_asciicg_assigns_actor_flags(test_context *ctx) {
    reset_state();
    music_selflg = 2;
    actwk[17].XPOSI.w.h = 4;

    set_asciicg();

    TEST_ASSERT_EQ_INT(ctx, 7, actwk[4].ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, 0, music_selflg);
    TEST_ASSERT_EQ_INT(ctx, 1, actset_flg);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[4].ACT_FLG);

    reset_state();
    music_selflg = 2;
    actset_flg = 1;
    actwk[17].XPOSI.w.h = 5;

    set_asciicg();

    TEST_ASSERT_EQ_INT(ctx, 7, actwk[5].ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, 3, actset_flg);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[5].ACT_FLG);
}

static void test_ascii_sprite_initializes_from_left_and_right(test_context *ctx) {
    reset_state();
    actwk[17].EXE_NO = 2;
    actwk[17].XPOSI.b.b3 = 1;
    ascii_xposi_tbl[5] = 96;
    actwk[0].CGBASE = 9000;

    ascii_sprite(&actwk[0]);

    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &dummy_pattern);
    TEST_ASSERT_EQ_INT(ctx, -128, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0x200000, actwk[0].X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 8 | 32, actwk[0].SPR_FLG);
    TEST_ASSERT_EQ_INT(ctx, 208, actwk[0].YPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 96, actwk[0].X_OFFSET);
    TEST_ASSERT_EQ_INT(ctx, 808, actwk[0].CGBASE);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].EXE_NO);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[17].ACT_NO);

    reset_state();
    actwk[17].EXE_NO = 3;
    ascii_xposi_tbl[7] = 77;
    actwk[0].SPR_FLG = 8;

    ascii_sprite(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 256, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, -0x200000, actwk[0].X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 32, actwk[0].SPR_FLG);
    TEST_ASSERT_EQ_INT(ctx, 77, actwk[0].X_OFFSET);
}

static void test_ascii_sprite_stops_at_target(test_context *ctx) {
    reset_state();
    actwk[0].EXE_NO = 1;
    actwk[0].XPOSI.w.h = 120;
    actwk[0].X_SPEED.l = -0x200000;
    actwk[0].X_OFFSET = 100;

    ascii_sprite(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 100, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[0].EXE_NO);

    reset_state();
    actwk[0].EXE_NO = 1;
    actwk[0].SPR_FLG = 8;
    actwk[0].XPOSI.w.h = 80;
    actwk[0].X_SPEED.l = 0x200000;
    actwk[0].X_OFFSET = 100;

    ascii_sprite(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 100, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[0].EXE_NO);
}

static void test_ascii_sprite_exit_direction_from_pad(test_context *ctx) {
    reset_state();
    actwk[0].EXE_NO = 2;
    swdata1.b.h = 8;

    ascii_sprite(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 3, actwk[0].EXE_NO);
    TEST_ASSERT_EQ_INT(ctx, 0x200000, actwk[0].X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 8, actwk[0].SPR_FLG);

    reset_state();
    actwk[0].EXE_NO = 2;
    actwk[0].SPR_FLG = 8;
    swdata1.b.h = 4;

    ascii_sprite(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 3, actwk[0].EXE_NO);
    TEST_ASSERT_EQ_INT(ctx, -0x200000, actwk[0].X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].SPR_FLG);
}

static void test_ascii_sprite_finishes_exit_and_wraps_selection(test_context *ctx) {
    reset_state();
    actwk[0].EXE_NO = 3;
    actwk[0].XPOSI.w.h = -120;
    actwk[0].X_SPEED.l = -0x100000;
    actwk[0].ACT_FLG = 2;
    actwk[17].EXE_NO = 31;
    actwk[17].XPOSI.b.b3 = 1;
    actset_flg = 3;

    ascii_sprite(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 4, music_selflg);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[17].XPOSI.b.b3);
    TEST_ASSERT_EQ_INT(ctx, 1, actset_flg);
    TEST_ASSERT_EQ_INT(ctx, 16, actwk[0].SPR_FLG);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[17].EXE_NO);

    reset_state();
    actwk[0].EXE_NO = 3;
    actwk[0].SPR_FLG = 8;
    actwk[0].XPOSI.w.h = 280;
    actwk[0].X_SPEED.l = 0x100000;
    actwk[0].ACT_FLG = 1;
    actwk[17].EXE_NO = 0;
    actset_flg = 1;

    ascii_sprite(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 4, music_selflg);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[17].XPOSI.b.b3);
    TEST_ASSERT_EQ_INT(ctx, 0, actset_flg);
    TEST_ASSERT_EQ_INT(ctx, 24, actwk[0].SPR_FLG);
    TEST_ASSERT_EQ_INT(ctx, 31, actwk[17].EXE_NO);
}

TEST_MAIN_BEGIN;
    test_release_sets_selected_music(&ctx);
    test_planet_three_does_not_replace_lplanet_no(&ctx);
    test_release_without_music_enabled_only_clears_latches(&ctx);
    test_start_release_while_music_enabled_selects_music(&ctx);
    test_start_input_waits_for_empty_actor_pool(&ctx);
    test_space_input_uses_same_start_path(&ctx);
    test_space_release_while_music_enabled_selects_music(&ctx);
    test_held_inputs_set_push_latches(&ctx);
    test_ascii_change_state_is_noop(&ctx);
    test_ascii_set_allocates_string_actor_slot(&ctx);
    test_set_asciicg_assigns_actor_flags(&ctx);
    test_ascii_sprite_initializes_from_left_and_right(&ctx);
    test_ascii_sprite_stops_at_target(&ctx);
    test_ascii_sprite_exit_direction_from_pad(&ctx);
    test_ascii_sprite_finishes_exit_and_wraps_selection(&ctx);
TEST_MAIN_END
