/* <stdlib.h> declares POSIX random(); the decompiled title/planet headers
   declare the game's own Sint32 random(void).  Parse stdlib first, then rename
   the game symbol inside this harness so the two can coexist. */
#include <stdlib.h>

#define random game_random

#include <string.h>

#include "support/test_runner.h"
#include "types.h"

#include "src/title/planet/plm.c"

static Sint32 random_values[32];
static int random_count;
static int random_index;
static Sint32 get_actwk_result;
static int next_slot;

Uint8 music_selflg;
Uint8 actset_wk;
Uint8 actset_flg;
Sint16 actset_tm[6];
Sint16 colorno;
sprite_status_lpl actwk[18];

static sprite_pattern_mappings_title dummy_map = {1, {{1, 0, 0, 0, 0, 0}}};
sprite_patterns_title flicky_tbl = {1, {{3, &dummy_map}}};
sprite_patterns_title flicky_tbl1 = {1, {{8, &dummy_map}}};
sprite_patterns_title flicky_tbl3 = {1, {{0, &dummy_map}}};

Sint32 random(void) {
    if (random_index < random_count)
        return random_values[random_index++];
    return 0;
}

Sint32 get_actwk(sprite_status_lpl **ppFreeAct) {
    if (get_actwk_result != 0)
        return get_actwk_result;
    if (next_slot >= 16)
        return -1;
    *ppFreeAct = &actwk[next_slot++];
    return 0;
}

static void set_randoms(const Sint32 *values, int count) {
    random_count = count;
    random_index = 0;
    for (int i = 0; i < count; ++i)
        random_values[i] = values[i];
}

static void reset_state(void) {
    memset(actwk, 0, sizeof(actwk));
    memset(actset_tm, 0, sizeof(actset_tm));
    memset(random_values, 0, sizeof(random_values));
    random_count = 0;
    random_index = 0;
    get_actwk_result = 0;
    next_slot = 0;
    music_selflg = 0;
    actset_wk = 0;
    actset_flg = 0;
    colorno = 16;
}

static void test_sprinit_respects_music_and_timer_gate(test_context *ctx) {
    Sint32 values[] = {0, 10, 1};

    reset_state();
    music_selflg = 1;
    actset_tm[0] = -1;
    set_randoms(values, 3);

    sprinit();

    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, -1, actset_tm[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, random_index);

    reset_state();
    for (int i = 0; i < 6; ++i)
        actset_tm[i] = 2;

    sprinit();

    TEST_ASSERT_EQ_INT(ctx, 1, actset_tm[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].ACT_NO);

    reset_state();
    actset_tm[0] = -1;
    set_randoms(values, 3);

    sprinit();

    TEST_ASSERT_EQ_INT(ctx, 6, actwk[0].ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, 600, actset_tm[0]);
    TEST_ASSERT_EQ_INT(ctx, 3, random_index);
}

static void test_f_actset_selects_all_flicky_pattern_modes(test_context *ctx) {
    Sint16 offsets[] = {5, -7, 9, 11};
    ini_pats pats = {2, offsets};
    Sint32 mode0[] = {0};
    Sint32 mode1[] = {1};
    Sint32 mode2[] = {2};

    reset_state();
    actset_wk = 2;
    set_randoms(mode0, 1);
    f_actset(&actwk[0], &pats, 10, 20, 1, 1, 1);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[0].ACT_FLG);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &flicky_tbl);
    TEST_ASSERT_EQ_INT(ctx, 81920, actwk[0].X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].Y_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 8, actwk[0].SPR_FLG);
    TEST_ASSERT_EQ_INT(ctx, 19, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 31, actwk[0].YPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 3, actwk[0].SPR_TIMER);

    reset_state();
    set_randoms(mode1, 1);
    f_actset(&actwk[0], &pats, 10, 20, 1, 1, 1);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &flicky_tbl1);
    TEST_ASSERT_EQ_INT(ctx, 40960, actwk[0].X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 1 | 8, actwk[0].SPR_FLG);
    TEST_ASSERT_EQ_INT(ctx, 8, actwk[0].SPR_TIMER);

    reset_state();
    set_randoms(mode2, 1);
    f_actset(&actwk[0], &pats, 10, 20, -1, 1, 1);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &flicky_tbl3);
    TEST_ASSERT_EQ_INT(ctx, -81920, actwk[0].X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 57344, actwk[0].Y_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 2 | 128, actwk[0].SPR_FLG);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 31, actwk[0].YPOSI.w.h);

    reset_state();
    f_actset(&actwk[0], &pats, 100, 200, 1, 0, 1);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &flicky_tbl);
    TEST_ASSERT_EQ_INT(ctx, 105, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 193, actwk[0].YPOSI.w.h);
}

static void test_flick_set_spawns_groups_and_honors_full_flags(
    test_context *ctx) {
    Sint32 values[] = {0, 5, 0, 2};

    reset_state();
    set_randoms(values, 4);

    flick_set();

    TEST_ASSERT_EQ_INT(ctx, 4, next_slot);
    TEST_ASSERT_EQ_INT(ctx, 0, actset_wk);
    TEST_ASSERT_EQ_INT(ctx, 1, actset_flg);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].ACT_FLG);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, -6, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 21, actwk[0].YPOSI.w.h);

    reset_state();
    actset_flg = 1;
    set_randoms(values, 4);

    flick_set();

    TEST_ASSERT_EQ_INT(ctx, 3, actset_flg);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[0].ACT_FLG);

    reset_state();
    actset_flg = 3;
    flick_set();

    TEST_ASSERT_EQ_INT(ctx, 0, next_slot);
    TEST_ASSERT_EQ_INT(ctx, 3, actset_flg);

    reset_state();
    get_actwk_result = -1;
    set_randoms(values, 4);
    flick_set();
    TEST_ASSERT_EQ_INT(ctx, 0, next_slot);
    TEST_ASSERT_EQ_INT(ctx, 0, actset_wk);

    reset_state();
    values[0] = -1;
    set_randoms(values, 4);
    flick_set();
    TEST_ASSERT_EQ_INT(ctx, 328, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 128, actwk[0].SPR_FLG);
}

static void test_star_set_gates_by_color_and_spawns_stars(test_context *ctx) {
    Sint32 values[] = {0, 5, 0, 64, 12, 20, 1, 2};

    reset_state();
    colorno = 10;
    star_set();
    TEST_ASSERT_EQ_INT(ctx, 0, next_slot);

    reset_state();
    colorno = 24;
    star_set();
    TEST_ASSERT_EQ_INT(ctx, 0, next_slot);

    reset_state();
    colorno = 16;
    set_randoms(values, 8);
    star_set();

    TEST_ASSERT_EQ_INT(ctx, 3, next_slot);
    TEST_ASSERT_EQ_INT(ctx, 1, actset_flg);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[0].ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].ACT_FLG);
    TEST_ASSERT_EQ_INT(ctx, 5 + 64, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].YPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, -0x20000, actwk[0].X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 0x40000, actwk[0].Y_SPEED.l);

    reset_state();
    actset_flg = 1;
    set_randoms(values, 8);
    star_set();
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[0].ACT_FLG);

    reset_state();
    actset_flg = 3;
    star_set();
    TEST_ASSERT_EQ_INT(ctx, 0, next_slot);

    reset_state();
    get_actwk_result = -1;
    set_randoms(values, 8);
    star_set();
    TEST_ASSERT_EQ_INT(ctx, 0, next_slot);
}

static void test_s_actset_positive_direction(test_context *ctx) {
    Sint16 offsets[] = {10, -20, 30};
    ini_pats pats = {3, offsets};
    Sint32 values[] = {0, 15};

    reset_state();
    actset_wk = 7;
    set_randoms(values, 2);

    s_actset(&actwk[0], &pats, 0, 40, 50, 1);

    TEST_ASSERT_EQ_INT(ctx, 2, actwk[0].ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, 7, actwk[0].ACT_FLG);
    TEST_ASSERT_EQ_INT(ctx, 55, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 30, actwk[0].YPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0x20000, actwk[0].X_SPEED.l);
}

static void test_ufo_set_spawns_forward_and_reverse(test_context *ctx) {
    Sint32 forward[] = {0, 10};
    Sint32 reverse[] = {-2, 20};

    reset_state();
    set_randoms(forward, 2);
    ufo_set();

    TEST_ASSERT_EQ_INT(ctx, 1, next_slot);
    TEST_ASSERT_EQ_INT(ctx, 4, actwk[0].ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, -6, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 26, actwk[0].YPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0x20000, actwk[0].X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 8, actwk[0].SPR_FLG);

    reset_state();
    actset_flg = 1;
    set_randoms(reverse, 2);
    ufo_set();

    TEST_ASSERT_EQ_INT(ctx, 2, next_slot);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[0].ACT_FLG);
    TEST_ASSERT_EQ_INT(ctx, 328, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 36, actwk[0].YPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, -0x20000, actwk[0].X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 128, actwk[0].SPR_FLG);
    TEST_ASSERT_EQ_INT(ctx, 312, actwk[1].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 84, actwk[1].YPOSI.w.h);

    reset_state();
    actset_flg = 3;
    ufo_set();
    TEST_ASSERT_EQ_INT(ctx, 0, next_slot);

    reset_state();
    get_actwk_result = -1;
    set_randoms(forward, 2);
    ufo_set();
    TEST_ASSERT_EQ_INT(ctx, 0, next_slot);
}

static void test_eggman_set_direction_and_gate(test_context *ctx) {
    Sint32 positive[] = {0, 9};
    Sint32 negative[] = {-1, 5};

    reset_state();
    set_randoms(positive, 2);
    eggman_set();

    TEST_ASSERT_EQ_INT(ctx, 3, actwk[0].ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, 328, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 137, actwk[0].YPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, -81920, actwk[0].X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, -32768, actwk[0].Y_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 3, actwk[0].ACT_FLG);
    TEST_ASSERT_EQ_INT(ctx, 0, actset_wk);

    reset_state();
    set_randoms(negative, 2);
    eggman_set();
    TEST_ASSERT_EQ_INT(ctx, -6, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 81920, actwk[0].X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 128, actwk[0].SPR_FLG);

    reset_state();
    actset_flg = 1;
    eggman_set();
    TEST_ASSERT_EQ_INT(ctx, 0, next_slot);

    reset_state();
    get_actwk_result = -1;
    set_randoms(positive, 2);
    eggman_set();
    TEST_ASSERT_EQ_INT(ctx, 0, next_slot);
}

static void test_metals_set_quadrants_and_gate(test_context *ctx) {
    Sint32 upper_left[] = {-1, 40};
    Sint32 lower_right[] = {0, 200};

    reset_state();
    set_randoms(upper_left, 2);
    metals_set();

    TEST_ASSERT_EQ_INT(ctx, 5, actwk[0].ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, 40, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].YPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0x40000, actwk[0].X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 327680, actwk[0].Y_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 8, actwk[0].SPR_FLG);

    reset_state();
    set_randoms(lower_right, 2);
    metals_set();

    TEST_ASSERT_EQ_INT(ctx, 200, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 192, actwk[0].YPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, -0x40000, actwk[0].X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, -327680, actwk[0].Y_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 128, actwk[0].SPR_FLG);

    reset_state();
    actset_flg = 2;
    metals_set();
    TEST_ASSERT_EQ_INT(ctx, 0, next_slot);

    reset_state();
    get_actwk_result = -1;
    set_randoms(upper_left, 2);
    metals_set();
    TEST_ASSERT_EQ_INT(ctx, 0, next_slot);
}

static void test_tails_set_direction_and_gate(test_context *ctx) {
    Sint32 positive[] = {0, 10};
    Sint32 negative[] = {-1, 12};

    reset_state();
    set_randoms(positive, 2);
    tails_set();

    TEST_ASSERT_EQ_INT(ctx, 6, actwk[0].ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, -6, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 42, actwk[0].YPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0x20000, actwk[0].X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].SPR_FLG);

    reset_state();
    set_randoms(negative, 2);
    tails_set();
    TEST_ASSERT_EQ_INT(ctx, 328, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 44, actwk[0].YPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, -0x20000, actwk[0].X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 128, actwk[0].SPR_FLG);

    reset_state();
    actset_flg = 3;
    tails_set();
    TEST_ASSERT_EQ_INT(ctx, 0, next_slot);

    reset_state();
    get_actwk_result = -1;
    set_randoms(positive, 2);
    tails_set();
    TEST_ASSERT_EQ_INT(ctx, 0, next_slot);
}

TEST_MAIN_BEGIN;
    test_sprinit_respects_music_and_timer_gate(&ctx);
    test_f_actset_selects_all_flicky_pattern_modes(&ctx);
    test_flick_set_spawns_groups_and_honors_full_flags(&ctx);
    test_star_set_gates_by_color_and_spawns_stars(&ctx);
    test_s_actset_positive_direction(&ctx);
    test_ufo_set_spawns_forward_and_reverse(&ctx);
    test_eggman_set_direction_and_gate(&ctx);
    test_metals_set_quadrants_and_gate(&ctx);
    test_tails_set_direction_and_gate(&ctx);
TEST_MAIN_END
