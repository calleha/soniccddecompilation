/* <stdlib.h> declares POSIX random(); the decompiled title/planet headers
   declare the game's own Sint32 random(void).  Parse stdlib first, then rename
   the game symbol inside this harness so the two can coexist. */
#include <stdlib.h>

#define random game_random

#include <string.h>

#include "support/test_runner.h"
#include "types.h"

#include "src/title/planet/chamov.c"

static Sint32 random_values[64];
static int random_count;
static int random_index;
static Sint16 sin_value;

Uint8 music_selflg;
Uint8 actset_flg;
sprite_status_lpl actwk[18];

static sprite_pattern_mappings_title dummy_map = {1, {{1, 0, 0, 0, 0, 0}}};
sprite_patterns_title flicky_tbl1 = {1, {{8, &dummy_map}}};
sprite_patterns_title flicky_tbl2 = {1, {{1, &dummy_map}}};
sprite_patterns_title flicky_tbl3 = {1, {{0, &dummy_map}}};
sprite_patterns_title star_tbl = {1, {{6, &dummy_map}}};
sprite_patterns_title ufo_tbl = {1, {{9, &dummy_map}}};
sprite_patterns_title ufo_tbl1 = {1, {{10, &dummy_map}}};
sprite_patterns_title ufo_tbl2 = {1, {{11, &dummy_map}}};
sprite_patterns_title eggm_tbl = {1, {{12, &dummy_map}}};
sprite_patterns_title eggm_tbl2 = {1, {{13, &dummy_map}}};
sprite_patterns_title msnc_tbl = {1, {{14, &dummy_map}}};
sprite_patterns_title msnc_tbl2 = {1, {{15, &dummy_map}}};
sprite_patterns_title tails_tbl = {1, {{16, &dummy_map}}};
sprite_patterns_title tails_up_tbl = {1, {{17, &dummy_map}}};
sprite_patterns_title tails_down_tbl = {1, {{18, &dummy_map}}};

Sint32 random(void) {
    if (random_index < random_count)
        return random_values[random_index++];
    return 0;
}

Sint16 sin_planet(Sint16 sDir) {
    (void)sDir;
    return sin_value;
}

static void set_randoms(const Sint32 *values, int count) {
    random_count = count;
    random_index = 0;
    for (int i = 0; i < count; ++i)
        random_values[i] = values[i];
}

static void reset_state(void) {
    memset(actwk, 0, sizeof(actwk));
    memset(random_values, 0, sizeof(random_values));
    random_count = 0;
    random_index = 0;
    sin_value = 64;
    music_selflg = 0;
    actset_flg = 0;
}

static void place_actor(sprite_status_lpl *actor, Uint16 actno, Sint16 x,
                        Sint16 y) {
    memset(actor, 0, sizeof(*actor));
    actor->ACT_NO = actno;
    actor->XPOSI.w.h = x;
    actor->YPOSI.w.h = y;
}

static void test_kill_helpers_and_actor_search(test_context *ctx) {
    sprite_status_lpl *found = 0;

    reset_state();
    actset_flg = 7;
    actwk[0].ACT_FLG = 3;

    spr_kill(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 4, actset_flg);
    TEST_ASSERT_EQ_INT(ctx, 16, actwk[0].SPR_FLG);

    reset_state();
    actset_flg = 7;
    actwk[0].ACT_NO = 4;
    actwk[0].ACT_FLG = 2;
    actwk[1].ACT_NO = 4;
    spr_kills(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, 16, actwk[0].SPR_FLG);
    TEST_ASSERT_EQ_INT(ctx, 7, actset_flg);

    actwk[1].ACT_NO = 0;
    actwk[2].ACT_NO = 4;
    actwk[2].ACT_FLG = 2;
    spr_kills(&actwk[2]);
    TEST_ASSERT_EQ_INT(ctx, 5, actset_flg);

    place_actor(&actwk[0], 1, -80, 0);
    TEST_ASSERT_EQ_INT(ctx, 1, killchk(&actwk[0]));
    actwk[0].XPOSI.w.h = 400;
    TEST_ASSERT_EQ_INT(ctx, 1, killchk(&actwk[0]));
    actwk[0].XPOSI.w.h = 0;
    actwk[0].YPOSI.w.h = -5;
    TEST_ASSERT_EQ_INT(ctx, 1, killchk(&actwk[0]));
    actwk[0].YPOSI.w.h = 224;
    TEST_ASSERT_EQ_INT(ctx, 1, killchk(&actwk[0]));
    actwk[0].YPOSI.w.h = 100;
    TEST_ASSERT_EQ_INT(ctx, 0, killchk(&actwk[0]));

    reset_state();
    actwk[0].ACT_NO = 9;
    actwk[3].ACT_NO = 9;
    TEST_ASSERT_EQ_INT(ctx, 1, chk_another(9, &actwk[0], &found));
    TEST_ASSERT_TRUE(ctx, found == &actwk[3]);
    actwk[3].ACT_NO = 0;
    TEST_ASSERT_EQ_INT(ctx, 0, chk_another(9, &actwk[0], &found));
}

static void test_sinmove_and_fnormalmove(test_context *ctx) {
    Sint32 values[] = {20};

    reset_state();
    place_actor(&actwk[0], 1, 10, 20);
    actwk[0].Y_OFFSET = 3;
    actwk[0].Y_WIDE = 8;
    actwk[0].X_WIDE = 48;
    actwk[0].SIN_CNT = 500;
    actwk[0].X_SPEED.l = 0x10000;
    actwk[0].Y_SPEED.l = 0x20000;
    set_randoms(values, 1);

    sinmove(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 2, actwk[0].Y_OFFSET);
    TEST_ASSERT_EQ_INT(ctx, 9, actwk[0].SIN_CNT);
    TEST_ASSERT_EQ_INT(ctx, 11, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 21, actwk[0].YPOSI.w.h);

    reset_state();
    place_actor(&actwk[0], 1, 10, 20);
    actwk[0].ACT_FLG = 1;
    actwk[0].XPOSI.w.h = 401;
    actset_flg = 3;
    sinmove(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, 2, actset_flg);

    reset_state();
    place_actor(&actwk[0], 1, 10, 20);
    actwk[0].SPR_FLG = 2;
    actwk[0].Y_SPEED.l = 1;
    actwk[0].X_SPEED.l = 0x10000;
    music_selflg = 1;
    fnormalmove(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 18, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 20, actwk[0].YPOSI.w.h);
}

static void test_flicky_states_and_spacing_adjustments(test_context *ctx) {
    Sint32 values[] = {12};

    reset_state();
    actwk[0].CGBASE = 5;
    flickyinit(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 16389, actwk[0].CGBASE);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].EXE_NO);

    reset_state();
    actwk[0].SPR_FLG = 1;
    flickyinit(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[0].EXE_NO);

    reset_state();
    actwk[0].SPR_FLG = 2;
    flickyinit(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 3, actwk[0].EXE_NO);

    reset_state();
    place_actor(&actwk[0], 1, 50, 40);
    actwk[0].X_SPEED.l = 0x10000;
    actwk[0].Y_SPEED.l = 0;
    set_randoms(values, 1);
    flickymove(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 51, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 12, actwk[0].SIN_CNT);

    reset_state();
    place_actor(&actwk[0], 1, -81, 40);
    flickymove(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, 16, actwk[0].SPR_FLG);

    reset_state();
    place_actor(&actwk[0], 1, 100, 40);
    actwk[0].X_SPEED.l = -1;
    fslowmove(&actwk[0]);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &flicky_tbl2);
    TEST_ASSERT_EQ_INT(ctx, -196608, actwk[0].X_SPEED.l);

    reset_state();
    place_actor(&actwk[0], 1, 100, 40);
    place_actor(&actwk[1], 1, 80, 40);
    actwk[0].X_SPEED.l = 1;
    fslowmove(&actwk[0]);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &flicky_tbl1);
    TEST_ASSERT_EQ_INT(ctx, 40960, actwk[0].X_SPEED.l);

    reset_state();
    place_actor(&actwk[0], 1, 100, 100);
    fdownmove(&actwk[0]);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &flicky_tbl2);
    TEST_ASSERT_EQ_INT(ctx, -45056, actwk[0].Y_SPEED.l);

    reset_state();
    place_actor(&actwk[0], 1, 100, 100);
    place_actor(&actwk[1], 1, 100, 96);
    fdownmove(&actwk[0]);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &flicky_tbl3);
    TEST_ASSERT_EQ_INT(ctx, 57344, actwk[0].Y_SPEED.l);
}

static void test_star_ufo_and_eggman_states(test_context *ctx) {
    Sint32 values[] = {9, 17, 20, 11, 0, 7};

    reset_state();
    starinit(&actwk[0]);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &star_tbl);
    TEST_ASSERT_EQ_INT(ctx, 6, actwk[0].SPR_TIMER);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].EXE_NO);

    place_actor(&actwk[0], 2, 10, 20);
    actwk[0].SIN_CNT = 500;
    actwk[0].X_SPEED.l = 0x10000;
    actwk[0].Y_SPEED.l = 0x10000;
    set_randoms(values, 1);
    starmove(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 509, actwk[0].SIN_CNT);
    TEST_ASSERT_EQ_INT(ctx, 11, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 21, actwk[0].YPOSI.w.h);

    reset_state();
    set_randoms(values, 1);
    ufoinit(&actwk[0]);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &ufo_tbl);
    TEST_ASSERT_EQ_INT(ctx, 5, actwk[0].timer1);
    TEST_ASSERT_EQ_INT(ctx, 9, actwk[0].timer2);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[0].EXE_NO);

    reset_state();
    place_actor(&actwk[0], 4, 100, 80);
    actwk[0].timer1 = 3;
    ufomode(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[0].timer1);
    TEST_ASSERT_EQ_INT(ctx, 24, actwk[0].timer2);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &ufo_tbl);

    reset_state();
    place_actor(&actwk[0], 4, 100, 80);
    actwk[0].timer1 = 4;
    set_randoms(&values[1], 3);
    ufomode(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 3, actwk[0].EXE_NO);
    TEST_ASSERT_EQ_INT(ctx, 3, actwk[0].timer1);
    TEST_ASSERT_EQ_INT(ctx, 181, actwk[0].END_XPOSI);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &ufo_tbl1);

    reset_state();
    place_actor(&actwk[0], 4, 100, 80);
    actwk[0].SPR_FLG = 8;
    actwk[0].Y_SPEED.l = 1;
    set_randoms(&values[2], 2);
    umset2(&actwk[0]);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &ufo_tbl2);
    TEST_ASSERT_EQ_INT(ctx, 43, actwk[0].END_YPOSI);
    TEST_ASSERT_TRUE(ctx, actwk[0].Y_SPEED.l < 0);

    reset_state();
    set_randoms(&values[4], 2);
    eggminit(&actwk[0]);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &eggm_tbl);
    TEST_ASSERT_EQ_INT(ctx, 3, actwk[0].timer1);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].EXE_NO);

    place_actor(&actwk[0], 3, 10, 20);
    actwk[0].timer1 = 1;
    actwk[0].X_WIDE = 48;
    actwk[0].Y_WIDE = 4;
    set_randoms(&values[4], 2);
    eggmmove(&actwk[0]);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &eggm_tbl2);
    TEST_ASSERT_EQ_INT(ctx, 32, actwk[0].timer1);
}

static void test_ufo_movement_transitions(test_context *ctx) {
    reset_state();
    place_actor(&actwk[0], 4, 100, 100);
    actwk[0].EXE_NO = 3;
    actwk[0].timer1 = 1;
    actwk[0].END_XPOSI = 101;
    actwk[0].END_YPOSI = 102;
    actwk[0].X_SPEED.l = 0x10000;
    actwk[0].Y_SPEED.l = 0x20000;
    ufomove2(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].EXE_NO);

    reset_state();
    place_actor(&actwk[0], 4, 100, 100);
    actwk[0].EXE_NO = 3;
    actwk[0].timer1 = 1;
    actwk[0].END_XPOSI = 99;
    actwk[0].END_YPOSI = 98;
    actwk[0].X_SPEED.l = -0x10000;
    actwk[0].Y_SPEED.l = -0x20000;
    ufomove2(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].EXE_NO);

    reset_state();
    place_actor(&actwk[0], 4, 10, 20);
    actwk[0].X_WIDE = 48;
    actwk[0].Y_WIDE = 4;
    actwk[0].X_SPEED.l = 0;
    actwk[0].timer2 = 1;
    ufomove1(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 72, actwk[0].X_WIDE);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].EXE_NO);
}

static void test_metal_sonic_paths(test_context *ctx) {
    Sint32 values[] = {1};

    reset_state();
    msncinit(&actwk[0]);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &msnc_tbl);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].EXE_NO);

    reset_state();
    place_actor(&actwk[0], 5, 120, 96);
    actwk[0].X_SPEED.l = 0x90000;
    actwk[0].Y_SPEED.l = 0x40000;
    msncstart(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 128, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 100, actwk[0].YPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 64, actwk[0].timer1);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[0].EXE_NO);

    reset_state();
    place_actor(&actwk[0], 5, 90, 90);
    actwk[0].SPR_FLG = 128;
    music_selflg = 1;
    msncstart(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 4, actwk[0].EXE_NO);
    TEST_ASSERT_EQ_INT(ctx, -1572864, actwk[0].X_SPEED.l);

    reset_state();
    place_actor(&actwk[0], 5, 100, 100);
    actwk[0].timer1 = 2;
    actwk[0].timer2 = 0;
    actwk[0].X_WIDE = 48;
    actwk[0].Y_WIDE = 4;
    set_randoms(values, 1);
    msncfloat(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].timer1);
    TEST_ASSERT_EQ_INT(ctx, 23, actwk[0].timer2);
    TEST_ASSERT_EQ_INT(ctx, 128, actwk[0].SPR_FLG);

    reset_state();
    place_actor(&actwk[0], 5, 100, 100);
    actwk[0].timer1 = 0;
    msncfloat(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 3, actwk[0].EXE_NO);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &msnc_tbl2);

    reset_state();
    place_actor(&actwk[0], 5, 100, 100);
    actwk[0].timer1 = 2;
    actwk[0].X_WIDE = 48;
    actwk[0].Y_WIDE = 4;
    msncdush(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].timer1);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].EXE_NO);

    music_selflg = 1;
    msncdush(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 4, actwk[0].EXE_NO);
    TEST_ASSERT_EQ_INT(ctx, 1572864, actwk[0].X_SPEED.l);

    reset_state();
    place_actor(&actwk[0], 5, 10, 20);
    actwk[0].X_SPEED.l = 0x10000;
    actwk[0].Y_SPEED.l = 0x10000;
    music_selflg = 1;
    msncexit(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 12, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 22, actwk[0].YPOSI.w.h);
}

static void test_tails_paths(test_context *ctx) {
    Sint32 values[] = {9, 17, 20, 11};

    reset_state();
    set_randoms(values, 1);
    tailsinit(&actwk[0]);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &tails_tbl);
    TEST_ASSERT_EQ_INT(ctx, 5, actwk[0].timer1);
    TEST_ASSERT_EQ_INT(ctx, 9, actwk[0].timer2);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[0].EXE_NO);

    reset_state();
    place_actor(&actwk[0], 6, 100, 100);
    actwk[0].SPR_FLG = 128;
    tsset1(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, -0x20000, actwk[0].X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 24, actwk[0].timer2);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[0].EXE_NO);

    reset_state();
    place_actor(&actwk[0], 6, 100, 120);
    actwk[0].Y_SPEED.l = 0;
    set_randoms(&values[1], 2);
    tsset2(&actwk[0]);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &tails_down_tbl);
    TEST_ASSERT_TRUE(ctx, actwk[0].Y_SPEED.l < 0);

    reset_state();
    place_actor(&actwk[0], 6, 100, 80);
    actwk[0].Y_SPEED.l = -1;
    set_randoms(&values[1], 2);
    tsset2(&actwk[0]);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &tails_up_tbl);
    TEST_ASSERT_EQ_INT(ctx, 180, actwk[0].END_YPOSI);

    reset_state();
    place_actor(&actwk[0], 6, 10, 20);
    actwk[0].X_WIDE = 48;
    actwk[0].Y_WIDE = 4;
    actwk[0].timer2 = 1;
    tailsmove1(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].EXE_NO);

    reset_state();
    place_actor(&actwk[0], 6, 100, 100);
    actwk[0].timer1 = 3;
    tailsmove(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[0].timer1);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[0].EXE_NO);
}

static void test_remaining_branch_edges(test_context *ctx) {
    Sint32 values[] = {0, 1, 4, 8, 12};
    sprite_status_lpl *found = 0;

    reset_state();
    place_actor(&actwk[0], 1, 10, 20);
    actwk[0].X_WIDE = 48;
    actwk[0].Y_WIDE = 4;
    actwk[0].X_SPEED.l = 0x10000;
    music_selflg = 1;
    sinmove(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 18, actwk[0].XPOSI.w.h);

    reset_state();
    place_actor(&actwk[0], 1, 10, 20);
    actwk[0].X_SPEED.l = 1;
    actwk[0].Y_SPEED.l = 0;
    actwk[0].SIN_CNT = 500;
    set_randoms(&values[4], 1);
    fnormalmove(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].SIN_CNT);

    reset_state();
    actwk[0].EXE_NO = 0;
    flicky_ctrl(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].EXE_NO);

    reset_state();
    place_actor(&actwk[0], 1, 10, 20);
    actwk[0].X_SPEED.l = 1;
    flickyslow(&actwk[0]);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &flicky_tbl2);

    reset_state();
    place_actor(&actwk[0], 1, -81, 20);
    flickyslow(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].ACT_NO);

    reset_state();
    place_actor(&actwk[0], 1, 10, 20);
    flickydown(&actwk[0]);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &flicky_tbl2);

    reset_state();
    place_actor(&actwk[0], 1, -81, 20);
    flickydown(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].ACT_NO);

    reset_state();
    place_actor(&actwk[0], 1, 100, 40);
    place_actor(&actwk[1], 1, 200, 40);
    actwk[0].X_SPEED.l = 1;
    fslowmove(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 196608, actwk[0].X_SPEED.l);

    actwk[0].X_SPEED.l = -1;
    actwk[1].XPOSI.w.h = 120;
    fslowmove(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, -40960, actwk[0].X_SPEED.l);

    reset_state();
    place_actor(&actwk[0], 1, 100, 90);
    place_actor(&actwk[1], 1, 100, 120);
    fdownmove(&actwk[0]);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &flicky_tbl3);

    reset_state();
    actwk[0].EXE_NO = 0;
    star_ctrl(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].EXE_NO);

    place_actor(&actwk[0], 2, 401, 20);
    starmove(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].ACT_NO);

    reset_state();
    place_actor(&actwk[0], 2, 10, 20);
    actwk[0].SIN_CNT = 500;
    set_randoms(&values[4], 1);
    starmove(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].SIN_CNT);

    reset_state();
    actwk[0].EXE_NO = 0;
    ufo_ctrl(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[0].EXE_NO);

    place_actor(&actwk[0], 4, 100, 50);
    actwk[0].SPR_FLG = 128;
    set_randoms(&values[2], 1);
    uendset(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 32, actwk[0].END_XPOSI);

    reset_state();
    place_actor(&actwk[0], 4, -81, 50);
    ufomove2(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].ACT_NO);

    reset_state();
    place_actor(&actwk[0], 4, 10, 50);
    actwk[0].X_SPEED.l = 0x10000;
    music_selflg = 1;
    ufomove2(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 18, actwk[0].XPOSI.w.h);

    reset_state();
    place_actor(&actwk[0], 4, 10, 50);
    actwk[0].X_SPEED.l = 0x10000;
    actwk[0].X_WIDE = 48;
    actwk[0].Y_WIDE = 4;
    actwk[0].timer2 = 2;
    ufomove1(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 40, actwk[0].X_WIDE);

    reset_state();
    actwk[0].EXE_NO = 0;
    eggm_ctrl(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].EXE_NO);

    place_actor(&actwk[0], 3, 10, 20);
    actwk[0].X_WIDE = 48;
    actwk[0].Y_WIDE = 4;
    actwk[0].timer1 = 2;
    eggmmove(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].timer1);

    reset_state();
    actwk[0].EXE_NO = 0;
    msnc_ctrl(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].EXE_NO);

    reset_state();
    place_actor(&actwk[0], 5, -81, 20);
    actwk[0].ACT_FLG = 1;
    actset_flg = 3;
    msncstart(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 2, actset_flg);

    reset_state();
    place_actor(&actwk[0], 5, 140, 96);
    actwk[0].SPR_FLG = 128;
    actwk[0].X_SPEED.l = -0xC0000;
    actwk[0].Y_SPEED.l = 0x40000;
    msncstart(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 128, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[0].EXE_NO);

    reset_state();
    place_actor(&actwk[0], 5, 120, 96);
    actwk[0].SPR_FLG = 8;
    actwk[0].X_SPEED.l = 0x80000;
    actwk[0].Y_SPEED.l = 0x40000;
    msncstart(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 100, actwk[0].YPOSI.w.h);

    reset_state();
    place_actor(&actwk[0], 5, 100, 96);
    actwk[0].X_SPEED.l = 0x10000;
    actwk[0].Y_SPEED.l = 0x40000;
    msncstart(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 101, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].EXE_NO);

    reset_state();
    place_actor(&actwk[0], 5, 120, 102);
    actwk[0].X_SPEED.l = 0x80000;
    actwk[0].Y_SPEED.l = 0;
    msncstart(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 128, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 102, actwk[0].YPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].EXE_NO);

    reset_state();
    place_actor(&actwk[0], 5, 100, 100);
    actwk[0].timer1 = 255;
    actwk[0].X_WIDE = 48;
    actwk[0].Y_WIDE = 4;
    msncfloat(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 255, actwk[0].timer1);

    reset_state();
    place_actor(&actwk[0], 5, 100, 100);
    actwk[0].timer1 = 2;
    actwk[0].timer2 = 0;
    actwk[0].X_WIDE = 48;
    actwk[0].Y_WIDE = 4;
    set_randoms(values, 1);
    msncfloat(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].SPR_FLG);

    reset_state();
    place_actor(&actwk[0], 5, 100, 100);
    actwk[0].SPR_FLG = 128;
    actwk[0].timer1 = 0;
    actwk[0].timer2 = 1;
    msncfloat(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 65536, actwk[0].X_SPEED.l);

    reset_state();
    place_actor(&actwk[0], 5, 100, 100);
    actwk[0].SPR_FLG = 128;
    actwk[0].timer1 = 0;
    msncdush(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, -1572864, actwk[0].X_SPEED.l);

    reset_state();
    place_actor(&actwk[0], 5, -81, 20);
    actwk[0].ACT_FLG = 1;
    actset_flg = 3;
    msncexit(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 2, actset_flg);

    reset_state();
    actwk[0].EXE_NO = 0;
    tails_ctrl(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[0].EXE_NO);

    reset_state();
    place_actor(&actwk[0], 6, 100, 100);
    actwk[0].timer1 = 2;
    set_randoms(values, 3);
    tailsmove(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 3, actwk[0].EXE_NO);

    reset_state();
    place_actor(&actwk[0], 6, 100, 80);
    actwk[0].SPR_FLG = 128;
    actwk[0].Y_SPEED.l = 0;
    set_randoms(values, 2);
    tsset2(&actwk[0]);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &tails_up_tbl);
    TEST_ASSERT_TRUE(ctx, actwk[0].X_SPEED.l < 0);

    TEST_ASSERT_EQ_INT(ctx, 0, chk_another(99, &actwk[0], &found));
}

TEST_MAIN_BEGIN;
    test_kill_helpers_and_actor_search(&ctx);
    test_sinmove_and_fnormalmove(&ctx);
    test_flicky_states_and_spacing_adjustments(&ctx);
    test_star_ufo_and_eggman_states(&ctx);
    test_ufo_movement_transitions(&ctx);
    test_metal_sonic_paths(&ctx);
    test_tails_paths(&ctx);
    test_remaining_branch_edges(&ctx);
TEST_MAIN_END
