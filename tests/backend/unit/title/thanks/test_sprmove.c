/* <stdlib.h> declares POSIX random(); the decompiled thanks headers declare
   the game's own Sint32 random(void).  Parse stdlib first, then rename the game
   symbol inside this harness so the two can coexist. */
#include <stdlib.h>

#define random game_random

#include <string.h>

#include "support/test_runner.h"
#include "src/types.h"

static Sint32 random_values[32];
static Sint32 random_count;
static Sint32 sign_count;
static Sint32 get_actwk_return;
static Sint32 get_actwk_count;
static Sint32 soundset_count;
static Sint16 soundset_values[8];

#include "src/title/thanks/sprmove.c"

Uint8 animal_flg;
sprite_status_thanks actwk[66];
Sint16 actset_tm[8];
Sint16 heart_tm;
Uint8 poseno;

static sprite_pattern_mappings_title one_piece_mapping = {
    1,
    {{1, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}},
};

#define ONE_PATTERN(timer_value)                                               \
    { 1, { { (timer_value), &one_piece_mapping } } }
#define TWO_PATTERN(timer_a, timer_b)                                          \
    {                                                                          \
        2, { { (timer_a), &one_piece_mapping },                                \
             { (timer_b), &one_piece_mapping } }                               \
    }
#define SIX_PATTERN(timer_value)                                               \
    {                                                                          \
        6, { { (timer_value), &one_piece_mapping },                            \
             { (timer_value), &one_piece_mapping },                            \
             { (timer_value), &one_piece_mapping },                            \
             { (timer_value), &one_piece_mapping },                            \
             { (timer_value), &one_piece_mapping },                            \
             { (timer_value), &one_piece_mapping } }                           \
    }

sprite_patterns_title s_run_map = SIX_PATTERN(3);
sprite_patterns_title s_stop_map = SIX_PATTERN(5);
sprite_patterns_title s_stand1_map = ONE_PATTERN(1);
sprite_patterns_title s_stand2_map = ONE_PATTERN(1);
sprite_patterns_title s_wait_map = TWO_PATTERN(10, 10);
sprite_patterns_title s_ball_map = TWO_PATTERN(2, 2);
sprite_patterns_title erun_map = SIX_PATTERN(4);
sprite_patterns_title ekiss_map = TWO_PATTERN(10, 10);
sprite_patterns_title ejump_map = ONE_PATTERN(10);
sprite_patterns_title ecatch_map = TWO_PATTERN(10, 10);
sprite_patterns_title estand2_map = TWO_PATTERN(14, 14);
sprite_patterns_title estand_map = TWO_PATTERN(6, 6);
sprite_patterns_title m_fly1_map = TWO_PATTERN(2, 2);
sprite_patterns_title m_brake_map = TWO_PATTERN(2, 2);
sprite_patterns_title m_stop_map = TWO_PATTERN(20, 20);
sprite_patterns_title m_stand_map = ONE_PATTERN(2);
sprite_patterns_title m_furi_map = TWO_PATTERN(40, 40);
sprite_patterns_title m_kamae_map = TWO_PATTERN(2, 2);
sprite_patterns_title m_era_map = TWO_PATTERN(40, 20);
sprite_patterns_title m_era2_map = TWO_PATTERN(20, 20);
sprite_patterns_title eheart_map = TWO_PATTERN(40, 20);
sprite_patterns_title pocky_map = TWO_PATTERN(12, 12);
sprite_patterns_title pecky_map = TWO_PATTERN(12, 12);
sprite_patterns_title ricky_map = TWO_PATTERN(6, 6);
sprite_patterns_title sheep_map = TWO_PATTERN(6, 6);
sprite_patterns_title flick_map = TWO_PATTERN(4, 4);
sprite_patterns_title inco_map = TWO_PATTERN(4, 4);
sprite_patterns_title hato_map = TWO_PATTERN(4, 4);

Sint32 random(void) {
    Sint32 value = random_values[random_count % 32];
    ++random_count;
    return value;
}

Sint16 sign(Sint16 d3) {
    ++sign_count;
    if ((d3 & 511) == 128)
        return 256;
    if ((d3 & 511) == 384)
        return -256;
    return 0;
}

Sint16 cosign(Sint16 d3) { return sign((Sint16)(d3 + 128)); }

Sint16 get_actwk(sprite_status_thanks **pNewActwk) {
    ++get_actwk_count;
    *pNewActwk = &actwk[10 + get_actwk_count];
    return (Sint16)get_actwk_return;
}

void soundset(Sint16 ReqNo) {
    if (soundset_count < 8) {
        soundset_values[soundset_count] = ReqNo;
    }
    ++soundset_count;
}

static void reset_fixture(void) {
    memset(random_values, 0, sizeof(random_values));
    memset(actwk, 0, sizeof(actwk));
    memset(actset_tm, 0, sizeof(actset_tm));
    memset(soundset_values, 0, sizeof(soundset_values));
    animal_flg = 0;
    heart_tm = 0;
    poseno = 0;
    random_count = 0;
    sign_count = 0;
    get_actwk_return = 0;
    get_actwk_count = 0;
    soundset_count = 0;
}

static void test_position_and_visibility_helpers_capture_thresholds(
    test_context *ctx) {
    sprite_status_thanks actor;
    sprite_status_thanks other;
    Sint16 distance = 0;
    Sint16 side = -1;

    reset_fixture();
    memset(&actor, 0, sizeof(actor));
    memset(&other, 0, sizeof(other));
    actor.XPOSI.w.h = 100;
    other.XPOSI.w.h = 120;
    TEST_ASSERT_EQ_INT(ctx, 0,
                       chk_sonicposi(&actor, &other, &distance, &side));
    TEST_ASSERT_EQ_INT(ctx, 20, distance);
    TEST_ASSERT_EQ_INT(ctx, 0, side);

    other.XPOSI.w.h = 80;
    TEST_ASSERT_EQ_INT(ctx, -1,
                       chk_sonicposi(&actor, &other, &distance, &side));
    TEST_ASSERT_EQ_INT(ctx, 20, distance);
    TEST_ASSERT_EQ_INT(ctx, 1, side);

    actor.X_SPEED.w.h = 1;
    actor.XPOSI.w.h = 112;
    TEST_ASSERT_EQ_INT(ctx, 0, myposichk(&actor));
    actor.XPOSI.w.h = 280;
    TEST_ASSERT_EQ_INT(ctx, -1, myposichk(&actor));
    actor.XPOSI.w.h = 384;
    TEST_ASSERT_EQ_INT(ctx, -1, myposichk(&actor));
    actor.XPOSI.w.h = 520;
    TEST_ASSERT_EQ_INT(ctx, 0, myposichk(&actor));

    actor.X_SPEED.w.h = -1;
    actor.XPOSI.w.h = 20;
    TEST_ASSERT_EQ_INT(ctx, -1, myposichk(&actor));
    actor.XPOSI.w.h = 100;
    TEST_ASSERT_EQ_INT(ctx, 0, myposichk(&actor));
    actor.XPOSI.w.h = 320;
    TEST_ASSERT_EQ_INT(ctx, -1, myposichk(&actor));
    actor.XPOSI.w.h = 500;
    TEST_ASSERT_EQ_INT(ctx, -1, myposichk(&actor));

    actor.SPR_FLG = 0;
    actor.XPOSI.w.h = 116;
    TEST_ASSERT_EQ_INT(ctx, -1, centerchk(&actor));
    actor.XPOSI.w.h = 121;
    TEST_ASSERT_EQ_INT(ctx, 0, centerchk(&actor));
    actor.SPR_FLG = 128;
    actor.XPOSI.w.h = 172;
    TEST_ASSERT_EQ_INT(ctx, -1, centerchk(&actor));
    actor.XPOSI.w.h = 177;
    TEST_ASSERT_EQ_INT(ctx, 0, centerchk(&actor));

    actor.XPOSI.w.h = 304;
    TEST_ASSERT_EQ_INT(ctx, 0, dsplaychk(&actor));
    actor.XPOSI.w.h = 305;
    TEST_ASSERT_EQ_INT(ctx, -1, dsplaychk(&actor));
}

static void test_motion_helpers_move_and_kill_at_exact_bounds(
    test_context *ctx) {
    sprite_status_thanks actor;

    reset_fixture();
    memset(&actor, 0, sizeof(actor));
    actor.XPOSI.w.h = -31;
    actor.YPOSI.w.h = -4;
    TEST_ASSERT_EQ_INT(ctx, 0, killchk(&actor));
    actor.XPOSI.w.h = -32;
    TEST_ASSERT_EQ_INT(ctx, -1, killchk(&actor));
    actor.XPOSI.w.h = 336;
    TEST_ASSERT_EQ_INT(ctx, -1, killchk(&actor));
    actor.XPOSI.w.h = 0;
    actor.YPOSI.w.h = 224;
    TEST_ASSERT_EQ_INT(ctx, -1, killchk(&actor));

    memset(&actor, 0, sizeof(actor));
    actor.XPOSI.l = 1 << 16;
    actor.YPOSI.l = 2 << 16;
    actor.X_SPEED.l = 3 << 16;
    actor.Y_SPEED.l = -1 << 16;
    actor.X_ACCEL.l = 2 << 16;
    actor.Y_ACCEL.l = 4 << 16;
    actor.X_OFFSET = 5;
    actor.Y_OFFSET = 6;

    hbt_move(&actor);

    TEST_ASSERT_EQ_INT(ctx, 6, actor.XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 5, actor.YPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, (2 << 16) + 5, actor.X_ACCEL.l);
    TEST_ASSERT_EQ_INT(ctx, (4 << 16) + 6, actor.Y_ACCEL.l);

    memset(&actor, 0, sizeof(actor));
    actor.XPOSI.w.h = 340;
    actor.SPR_FLG = 4;
    actor.X_SPEED.l = 0x10000;
    waitmode(&actor);
    TEST_ASSERT_EQ_INT(ctx, 0, actor.X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 0, actor.EXE_NO);

    actor.SPR_FLG = 0;
    actor.EXE_NO = 9;
    actor.XPOSI.w.h = 10;
    waitmode(&actor);
    TEST_ASSERT_EQ_INT(ctx, 1, actor.EXE_NO);
}

static void test_escape_chk_forward_and_reverse_death_windows(
    test_context *ctx) {
    sprite_status_thanks actor;

    reset_fixture();
    memset(&actor, 0, sizeof(actor));
    actor.X_SPEED.l = 0x10000;
    actor.XPOSI.w.h = 320;
    escape_chk(&actor);
    TEST_ASSERT_TRUE(ctx, (actor.SPR_FLG & 16) != 0);

    memset(&actor, 0, sizeof(actor));
    actor.X_SPEED.l = -0x10000;
    actor.XPOSI.w.h = 460;
    escape_chk(&actor);
    TEST_ASSERT_TRUE(ctx, (actor.SPR_FLG & 16) != 0);

    memset(&actor, 0, sizeof(actor));
    actor.X_SPEED.l = 0x10000;
    actor.Y_SPEED.l = 0x20000;
    actor.XPOSI.w.h = 100;
    actor.YPOSI.w.h = 20;
    escape_chk(&actor);
    TEST_ASSERT_EQ_INT(ctx, 101, actor.XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 22, actor.YPOSI.w.h);
}

static void test_sonic_initial_and_setup_states(test_context *ctx) {
    reset_fixture();
    random_values[0] = 0;
    random_values[1] = 5;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 328, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 140, actwk[0].YPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, -196608, actwk[0].X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 25127, actwk[0].CGBASE);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].free[0]);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &s_run_map);
    TEST_ASSERT_EQ_INT(ctx, 133, actwk[0].TM_CNT);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[0].EXE_NO);

    reset_fixture();
    actwk[0].EXE_NO = 1;
    actwk[0].SPR_FLG = 128;
    actwk[0].XPOSI.l = 0x7fffffff;
    random_values[0] = 17;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, -196608, actwk[0].X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 81, actwk[0].TM_CNT);
    TEST_ASSERT_EQ_INT(ctx, 3, actwk[2].ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[0].EXE_NO);
}

static void test_sonic_turn_ball_and_exit_branches(test_context *ctx) {
    reset_fixture();
    actwk[0].EXE_NO = 3;
    actwk[0].X_SPEED.l = 196608;
    actwk[0].XPOSI.w.h = 100;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, -10240, actwk[0].X_OFFSET);
    TEST_ASSERT_TRUE(ctx, (actwk[0].SPR_FLG & 128) != 0);
    TEST_ASSERT_EQ_INT(ctx, 1, soundset_count);
    TEST_ASSERT_EQ_INT(ctx, 144, soundset_values[0]);
    TEST_ASSERT_EQ_INT(ctx, 4, actwk[0].EXE_NO);

    reset_fixture();
    actwk[0].EXE_NO = 9;
    actwk[0].SPR_FLG = 128;
    actwk[0].XPOSI.w.h = 100;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, -131072, actwk[0].X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, -393216, actwk[0].Y_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 1, soundset_count);
    TEST_ASSERT_EQ_INT(ctx, 146, soundset_values[0]);
    TEST_ASSERT_EQ_INT(ctx, 10, actwk[0].EXE_NO);

    reset_fixture();
    animal_flg = 1;
    actwk[0].EXE_NO = 11;
    actwk[0].SPR_FLG = 4;
    actwk[0].XPOSI.w.h = 340;
    actwk[1].EXE_NO = 0;
    actwk[2].EXE_NO = 0;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 13, actwk[0].EXE_NO);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &s_run_map);
}

static void test_sonic_run_stand_wait_and_ball_motion_states(test_context *ctx) {
    reset_fixture();
    actwk[0].EXE_NO = 2;
    actwk[0].SPR_FLG = 4;
    actwk[0].X_SPEED.l = 0x10000;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 11, actwk[0].EXE_NO);

    reset_fixture();
    actwk[0].EXE_NO = 2;
    actwk[0].XPOSI.w.h = 375;
    actwk[0].X_SPEED.l = 0x10000;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 12, actwk[0].EXE_NO);

    reset_fixture();
    actwk[0].EXE_NO = 4;
    actwk[0].PAT_NO = 6;
    actwk[2].EXE_NO = 2;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].EXE_NO);

    reset_fixture();
    actwk[0].EXE_NO = 5;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[1].ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, 6, actwk[0].EXE_NO);

    reset_fixture();
    actwk[0].EXE_NO = 6;
    actwk[0].TM_CNT = 0;
    actwk[0].free[3] = 1;
    actwk[1].SPR_FLG = 4;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 7, actwk[0].EXE_NO);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[1].SPR_FLG & 4);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &s_stand2_map);

    reset_fixture();
    actwk[0].EXE_NO = 7;
    random_values[0] = 10;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 137, actwk[0].TM_CNT);
    TEST_ASSERT_TRUE(ctx, actwk[0].PAT_ADR == &s_wait_map);
    TEST_ASSERT_EQ_INT(ctx, 8, actwk[0].EXE_NO);

    reset_fixture();
    actwk[0].EXE_NO = 8;
    actwk[0].TM_CNT = 1;
    actwk[0].XPOSI.w.h = 100;
    actwk[2].XPOSI.w.h = 200;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 9, actwk[0].EXE_NO);

    reset_fixture();
    actwk[0].EXE_NO = 10;
    actwk[0].YPOSI.w.h = 139;
    actwk[0].Y_SPEED.l = 0x20000;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].EXE_NO);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].Y_ACCEL.l);
}

static void test_sonic_collision_random_and_pit_paths(test_context *ctx) {
    reset_fixture();
    actwk[0].EXE_NO = 2;
    actwk[0].XPOSI.w.h = 100;
    actwk[0].X_SPEED.l = 0;
    random_values[1] = 0;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 9, actwk[0].EXE_NO);

    reset_fixture();
    actwk[0].EXE_NO = 2;
    actwk[0].XPOSI.w.h = 100;
    actwk[0].X_SPEED.l = 0x10000;
    actwk[2].EXE_NO = 2;
    actwk[2].XPOSI.w.h = 101;
    actwk[2].X_SPEED.l = -0x10000;
    random_values[1] = 1;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 3, actwk[0].EXE_NO);
    TEST_ASSERT_EQ_INT(ctx, -1, s_metalchk(&actwk[0]));

    reset_fixture();
    actwk[0].EXE_NO = 2;
    actwk[0].XPOSI.w.h = 115;
    actwk[0].X_SPEED.l = 0;
    actwk[0].TM_CNT = 1;
    random_values[1] = 1;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 3, actwk[0].EXE_NO);

    reset_fixture();
    actwk[0].EXE_NO = 12;
    actwk[0].X_SPEED.l = 1;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 400, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[0].EXE_NO);

    reset_fixture();
    actwk[0].EXE_NO = 13;
    actwk[0].free[1] = 0;
    actwk[0].TM_CNT = 1;
    actwk[1].SPR_FLG = 4;
    actwk[2].SPR_FLG = 4;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 448, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 2, animal_flg);
    TEST_ASSERT_EQ_INT(ctx, 1800, actset_tm[0]);
    TEST_ASSERT_EQ_INT(ctx, 1800, actset_tm[7]);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].free[1]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[1].SPR_FLG & 4);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[2].SPR_FLG & 4);
}

static void test_sonic_turn_exit_escape_and_helper_edges(test_context *ctx) {
    sprite_status_thanks actor;

    reset_fixture();
    actwk[0].EXE_NO = 4;
    actwk[0].PAT_NO = 4;
    actwk[0].X_SPEED.l = 0x10000;
    actwk[0].X_ACCEL.l = 0;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 4, actwk[0].EXE_NO);

    reset_fixture();
    actwk[0].EXE_NO = 4;
    actwk[0].PAT_NO = 5;
    actwk[0].XPOSI.w.h = 280;
    actwk[0].X_SPEED.w.h = 1;
    actwk[2].EXE_NO = 0;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 9, actwk[0].EXE_NO);

    reset_fixture();
    actwk[0].EXE_NO = 4;
    actwk[0].PAT_NO = 5;
    actwk[0].XPOSI.w.h = 100;
    actwk[2].EXE_NO = 0;
    actwk[2].XPOSI.w.h = 130;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 5, actwk[0].EXE_NO);

    reset_fixture();
    animal_flg = 0;
    actwk[0].EXE_NO = 11;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].EXE_NO);

    reset_fixture();
    animal_flg = 1;
    actwk[0].EXE_NO = 11;
    actwk[0].SPR_FLG = 4;
    actwk[0].X_SPEED.l = 0x10000;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 11, actwk[0].EXE_NO);

    reset_fixture();
    animal_flg = 1;
    actwk[0].EXE_NO = 11;
    actwk[0].SPR_FLG = 4;
    actwk[1].EXE_NO = 12;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 11, actwk[0].EXE_NO);

    reset_fixture();
    animal_flg = 1;
    actwk[0].EXE_NO = 11;
    actwk[0].SPR_FLG = 4;
    actwk[1].EXE_NO = 13;
    actwk[1].X_SPEED.l = 1;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 11, actwk[0].EXE_NO);

    reset_fixture();
    actwk[0].EXE_NO = 13;
    actwk[0].free[1] = 1;
    actwk[0].X_SPEED.l = 0x10000;
    actwk[0].XPOSI.w.h = 320;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_TRUE(ctx, (actwk[0].SPR_FLG & 16) != 0);

    reset_fixture();
    actwk[0].EXE_NO = 12;
    actwk[0].X_SPEED.l = -1;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 368, actwk[0].XPOSI.w.h);

    reset_fixture();
    actwk[0].EXE_NO = 12;
    actwk[0].free[4] = 3;

    sonic_ctrl(&actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 390, actwk[0].XPOSI.w.h);

    memset(&actor, 0, sizeof(actor));
    actor.X_SPEED.w.h = 1;
    actor.XPOSI.w.h = 450;
    TEST_ASSERT_EQ_INT(ctx, -1, myposichk(&actor));
    actor.X_SPEED.w.h = -1;
    actor.XPOSI.w.h = 400;
    TEST_ASSERT_EQ_INT(ctx, 0, myposichk(&actor));
    actor.SPR_FLG = 128;
    actor.XPOSI.w.h = 100;
    TEST_ASSERT_EQ_INT(ctx, 0, centerchk(&actor));
}

static void test_emie_initial_and_heart_spawning_states(test_context *ctx) {
    reset_fixture();
    actwk[0].ACT_NO = 1;

    emie_ctrl(&actwk[1]);

    TEST_ASSERT_EQ_INT(ctx, 502, actwk[1].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 141, actwk[1].YPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 147456, actwk[1].X_SPEED.l);
    TEST_ASSERT_TRUE(ctx, actwk[1].PAT_ADR == &erun_map);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[1].EXE_NO);

    reset_fixture();
    actwk[0].XPOSI.w.h = 50;
    actwk[1].EXE_NO = 3;
    actwk[1].XPOSI.w.h = 100;

    emie_ctrl(&actwk[1]);

    TEST_ASSERT_EQ_INT(ctx, -196608, actwk[1].Y_SPEED.l);
    TEST_ASSERT_TRUE(ctx, (actwk[1].SPR_FLG & 128) != 0);
    TEST_ASSERT_EQ_INT(ctx, 12, actwk[1].free[4]);
    TEST_ASSERT_EQ_INT(ctx, 1, heart_tm);
    TEST_ASSERT_EQ_INT(ctx, 7, actwk[11].ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, 4, actwk[1].EXE_NO);

    reset_fixture();
    get_actwk_return = -1;
    actwk[0].XPOSI.w.h = 200;
    actwk[1].EXE_NO = 5;
    actwk[1].XPOSI.w.h = 100;

    emie_ctrl(&actwk[1]);

    TEST_ASSERT_EQ_INT(ctx, 20, actwk[1].TM_CNT);
    TEST_ASSERT_EQ_INT(ctx, 6, actwk[1].EXE_NO);
    TEST_ASSERT_EQ_INT(ctx, 0, heart_tm);
}

static void test_emie_move_turn_stand_and_exit_states(test_context *ctx) {
    reset_fixture();
    actwk[0].XPOSI.w.h = 40;
    actwk[1].EXE_NO = 1;
    actwk[1].XPOSI.w.h = 100;

    emie_ctrl(&actwk[1]);

    TEST_ASSERT_EQ_INT(ctx, -147456, actwk[1].X_SPEED.l);
    TEST_ASSERT_TRUE(ctx, (actwk[1].SPR_FLG & 128) != 0);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[1].EXE_NO);

    reset_fixture();
    actwk[0].ACT_NO = 1;
    actwk[1].EXE_NO = 2;
    actwk[1].SPR_FLG = 4;

    emie_ctrl(&actwk[1]);

    TEST_ASSERT_EQ_INT(ctx, 13, actwk[1].EXE_NO);

    reset_fixture();
    actwk[0].XPOSI.w.h = 300;
    actwk[1].EXE_NO = 9;
    actwk[1].XPOSI.w.h = 100;

    emie_ctrl(&actwk[1]);

    TEST_ASSERT_EQ_INT(ctx, 10, actwk[1].EXE_NO);

    reset_fixture();
    poseno = 2;
    actwk[1].EXE_NO = 10;
    actwk[1].X_SPEED.l = -actwk[1].X_ACCEL.l;

    emie_ctrl(&actwk[1]);

    TEST_ASSERT_EQ_INT(ctx, 80, actwk[1].TM_CNT);
    TEST_ASSERT_EQ_INT(ctx, 3, actwk[1].EXE_NO);

    reset_fixture();
    random_values[0] = 1;
    actwk[0].XPOSI.w.h = 10;
    actwk[1].EXE_NO = 11;
    actwk[1].XPOSI.w.h = 100;

    emie_ctrl(&actwk[1]);

    TEST_ASSERT_TRUE(ctx, actwk[1].PAT_ADR == &estand2_map);
    TEST_ASSERT_TRUE(ctx, (actwk[1].SPR_FLG & 128) != 0);
    TEST_ASSERT_EQ_INT(ctx, 16, actwk[1].free[4]);
    TEST_ASSERT_EQ_INT(ctx, 12, actwk[1].EXE_NO);

    reset_fixture();
    actwk[0].XPOSI.w.h = 200;
    actwk[1].EXE_NO = 12;
    actwk[1].XPOSI.w.h = 100;
    actwk[1].TM_CNT = 1;

    emie_ctrl(&actwk[1]);

    TEST_ASSERT_EQ_INT(ctx, 1, actwk[1].EXE_NO);

    reset_fixture();
    animal_flg = 2;
    actwk[1].EXE_NO = 13;
    actwk[1].SPR_FLG = 4;

    emie_ctrl(&actwk[1]);

    TEST_ASSERT_EQ_INT(ctx, 448, actwk[1].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 15, actwk[1].EXE_NO);
}

static void test_emmy_pre_dispatch_kiss_jump_and_catch_paths(
    test_context *ctx) {
    Sint16 d5 = 0;
    Sint16 d6 = 0;

    reset_fixture();
    actwk[1].EXE_NO = 2;

    emmy_exeset(&actwk[1], &actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 15, actwk[1].EXE_NO);

    reset_fixture();
    actwk[0].ACT_NO = 1;
    actwk[0].EXE_NO = 6;
    actwk[0].XPOSI.w.h = 100;
    actwk[1].EXE_NO = 2;
    actwk[1].XPOSI.w.h = 120;

    emmy_exeset(&actwk[1], &actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 5, actwk[1].EXE_NO);

    reset_fixture();
    actwk[0].ACT_NO = 1;
    actwk[0].XPOSI.w.h = 100;
    actwk[1].EXE_NO = 2;
    actwk[1].XPOSI.w.h = 384;
    random_values[0] = 0;

    emmy_exeset(&actwk[1], &actwk[0]);

    TEST_ASSERT_EQ_INT(ctx, 394, actwk[1].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 11, actwk[1].EXE_NO);
    TEST_ASSERT_EQ_INT(ctx, 64, actwk[1].TM_CNT);

    reset_fixture();
    actwk[0].XPOSI.w.h = 50;
    actwk[1].EXE_NO = 4;
    actwk[1].YPOSI.w.h = 140;
    actwk[1].TM_CNT = 1;
    actwk[1].free[4] = 0;
    poseno = 2;
    actwk[0].SPR_FLG = 4;
    actwk[2].SPR_FLG = 4;

    emmykiss(&actwk[1]);

    TEST_ASSERT_EQ_INT(ctx, 1, actwk[1].EXE_NO);
    TEST_ASSERT_EQ_INT(ctx, 139, actwk[1].YPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, poseno);
    TEST_ASSERT_EQ_INT(ctx, 7, actwk[11].ACT_NO);

    reset_fixture();
    actwk[1].EXE_NO = 6;
    actwk[1].YPOSI.w.h = 140;
    actwk[1].TM_CNT = 1;
    actwk[1].free[4] = 0;

    emmyjump(&actwk[1]);

    TEST_ASSERT_EQ_INT(ctx, 7, actwk[1].EXE_NO);
    TEST_ASSERT_EQ_INT(ctx, 7, actwk[11].ACT_NO);

    reset_fixture();
    actwk[0].XPOSI.w.h = 100;
    actwk[0].SPR_FLG = 128;
    actwk[1].SPR_FLG = 128;
    actwk[1].X_SPEED.l = 0x10000;
    actwk[1].EXE_NO = 7;

    e_catchset(&actwk[1]);

    TEST_ASSERT_EQ_INT(ctx, 116, actwk[1].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[1].X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 8, actwk[1].EXE_NO);

    reset_fixture();
    actwk[0].SPR_FLG = 0;
    actwk[0].XPOSI.w.h = 100;
    actwk[1].SPR_FLG = 0;
    actwk[1].XPOSI.w.h = 100;
    actwk[1].EXE_NO = 7;

    e_catchset(&actwk[1]);

    TEST_ASSERT_EQ_INT(ctx, 84, actwk[1].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[1].SPR_FLG & 128);

    reset_fixture();
    actwk[0].SPR_FLG = 128;
    actwk[0].EXE_NO = 6;
    actwk[0].XPOSI.w.h = 100;
    actwk[1].EXE_NO = 8;
    actwk[1].X_SPEED.l = 0x10000;
    actwk[1].free[4] = 2;

    emmy_catch(&actwk[1]);

    TEST_ASSERT_EQ_INT(ctx, 116, actwk[1].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, -0x10000, actwk[1].X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[1].free[4]);

    reset_fixture();
    actwk[0].SPR_FLG = 128;
    actwk[0].EXE_NO = 6;
    actwk[0].XPOSI.w.h = 100;
    actwk[1].EXE_NO = 8;
    actwk[1].free[4] = 0;

    emmy_catch(&actwk[1]);

    TEST_ASSERT_EQ_INT(ctx, 7, actwk[11].ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, 16, actwk[1].free[4]);

    reset_fixture();
    get_actwk_return = -1;
    actwk[0].EXE_NO = 6;
    actwk[1].EXE_NO = 8;
    actwk[1].free[4] = 0;

    emmy_catch(&actwk[1]);

    TEST_ASSERT_EQ_INT(ctx, 40, actwk[1].TM_CNT);
    TEST_ASSERT_EQ_INT(ctx, 3, actwk[1].EXE_NO);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[1].free[0]);

    actwk[0].EXE_NO = 4;
    TEST_ASSERT_EQ_INT(ctx, 0,
                       chk_sonicposi(&actwk[1], &actwk[0], &d5, &d6));
}

static void test_metal_initial_stop_and_erase_states(test_context *ctx) {
    reset_fixture();
    random_values[0] = 9;

    metals_ctrl(&actwk[2]);

    TEST_ASSERT_EQ_INT(ctx, 336, actwk[2].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 132, actwk[2].YPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, -237568, actwk[2].X_SPEED.l);
    TEST_ASSERT_TRUE(ctx, actwk[2].PAT_ADR == &m_fly1_map);
    TEST_ASSERT_EQ_INT(ctx, 137, actwk[2].TM_CNT);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[2].EXE_NO);

    reset_fixture();
    actwk[2].EXE_NO = 5;
    actwk[2].XPOSI.w.h = 384;

    metals_ctrl(&actwk[2]);

    TEST_ASSERT_EQ_INT(ctx, 394, actwk[2].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 32, actwk[2].TM_CNT);
    TEST_ASSERT_EQ_INT(ctx, 6, actwk[2].EXE_NO);

    reset_fixture();
    actwk[0].EXE_NO = 12;
    actwk[2].EXE_NO = 6;
    actwk[2].TM_CNT = 0;

    metals_ctrl(&actwk[2]);

    TEST_ASSERT_EQ_INT(ctx, 11, actwk[2].EXE_NO);

    reset_fixture();
    actwk[2].EXE_NO = 11;

    metals_ctrl(&actwk[2]);

    TEST_ASSERT_TRUE(ctx, actwk[2].PAT_ADR == &m_era_map);
    TEST_ASSERT_EQ_INT(ctx, 32, actwk[2].TM_CNT);
    TEST_ASSERT_EQ_INT(ctx, 12, actwk[2].EXE_NO);
}

static void test_metal_flight_stand_start_and_exit_states(test_context *ctx) {
    reset_fixture();
    actwk[2].EXE_NO = 1;
    actwk[2].SPR_FLG = 128;
    random_values[0] = 3;

    metals_ctrl(&actwk[2]);

    TEST_ASSERT_EQ_INT(ctx, -237568, actwk[2].X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 131, actwk[2].TM_CNT);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[2].EXE_NO);

    reset_fixture();
    actwk[2].EXE_NO = 2;
    actwk[2].SPR_FLG = 4;

    metals_ctrl(&actwk[2]);

    TEST_ASSERT_EQ_INT(ctx, 13, actwk[2].EXE_NO);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[2].free[2]);

    reset_fixture();
    actwk[0].ACT_NO = 0;
    actwk[2].EXE_NO = 2;

    metals_ctrl(&actwk[2]);

    TEST_ASSERT_EQ_INT(ctx, 15, actwk[2].EXE_NO);

    reset_fixture();
    actwk[2].EXE_NO = 3;
    actwk[2].SPR_FLG = 128;

    metals_ctrl(&actwk[2]);

    TEST_ASSERT_EQ_INT(ctx, 14336, actwk[2].X_OFFSET);
    TEST_ASSERT_TRUE(ctx, actwk[2].PAT_ADR == &m_brake_map);
    TEST_ASSERT_EQ_INT(ctx, 4, actwk[2].EXE_NO);

    reset_fixture();
    actwk[2].EXE_NO = 8;
    actwk[2].TM_CNT = 0;
    actwk[0].XPOSI.w.h = 20;
    actwk[2].XPOSI.w.h = 100;

    metals_ctrl(&actwk[2]);

    TEST_ASSERT_EQ_INT(ctx, 9, actwk[2].EXE_NO);
    TEST_ASSERT_TRUE(ctx, (actwk[2].SPR_FLG & 128) != 0);

    reset_fixture();
    actwk[0].XPOSI.w.h = 20;
    actwk[2].EXE_NO = 9;
    actwk[2].XPOSI.w.h = 100;

    metals_ctrl(&actwk[2]);

    TEST_ASSERT_TRUE(ctx, actwk[2].PAT_ADR == &m_furi_map);
    TEST_ASSERT_EQ_INT(ctx, 16, actwk[2].TM_CNT);
    TEST_ASSERT_EQ_INT(ctx, 10, actwk[2].EXE_NO);

    reset_fixture();
    actwk[2].EXE_NO = 10;
    actwk[2].TM_CNT = 0;

    metals_ctrl(&actwk[2]);

    TEST_ASSERT_TRUE(ctx, actwk[2].PAT_ADR == &m_kamae_map);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[2].free[3]);

    reset_fixture();
    animal_flg = 2;
    actwk[2].EXE_NO = 13;
    actwk[2].SPR_FLG = 4;

    metals_ctrl(&actwk[2]);

    TEST_ASSERT_EQ_INT(ctx, 448, actwk[2].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 15, actwk[2].EXE_NO);
}

static void test_metal_remaining_timed_and_sound_paths(test_context *ctx) {
    reset_fixture();
    actwk[2].EXE_NO = 4;
    actwk[2].X_SPEED.l = -actwk[2].X_ACCEL.l;

    metals_ctrl(&actwk[2]);

    TEST_ASSERT_EQ_INT(ctx, 5, actwk[2].EXE_NO);

    reset_fixture();
    actwk[0].EXE_NO = 1;
    actwk[2].EXE_NO = 6;
    actwk[2].TM_CNT = 0;

    metals_ctrl(&actwk[2]);

    TEST_ASSERT_EQ_INT(ctx, 7, actwk[2].EXE_NO);

    reset_fixture();
    actwk[0].XPOSI.w.h = 20;
    actwk[2].EXE_NO = 10;
    actwk[2].TM_CNT = 0;
    actwk[2].free[3] = 1;

    metals_ctrl(&actwk[2]);

    TEST_ASSERT_EQ_INT(ctx, 1, actwk[2].EXE_NO);

    reset_fixture();
    actwk[2].EXE_NO = 12;
    actwk[2].TM_CNT = 0;

    metals_ctrl(&actwk[2]);

    TEST_ASSERT_TRUE(ctx, actwk[2].PAT_ADR == &m_era2_map);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[2].free[3]);

    actwk[2].TM_CNT = 0;
    metals_ctrl(&actwk[2]);
    TEST_ASSERT_EQ_INT(ctx, 9, actwk[2].EXE_NO);

    reset_fixture();
    actwk[2].X_SPEED.l = 1;
    actwk[2].XPOSI.w.h = 5;
    m_sndchk(&actwk[2]);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[2].free[2]);

    reset_fixture();
    actwk[2].X_SPEED.l = -1;
    actwk[2].XPOSI.w.h = 308;
    m_sndchk(&actwk[2]);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[2].free[2]);
}

static void test_metalsfly_and_standset_remaining_paths(test_context *ctx) {
    reset_fixture();
    actwk[0].ACT_NO = 1;
    actwk[2].EXE_NO = 2;
    actwk[2].XPOSI.w.h = 376;

    metals_ctrl(&actwk[2]);

    TEST_ASSERT_EQ_INT(ctx, 14, actwk[2].EXE_NO);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[2].free[2]);

    reset_fixture();
    actwk[0].ACT_NO = 1;
    actwk[0].XPOSI.w.h = 10;
    actwk[1].XPOSI.w.h = 20;
    actwk[2].EXE_NO = 2;
    actwk[2].XPOSI.w.h = 200;
    actwk[2].X_SPEED.w.h = 1;
    actwk[2].TM_CNT = 0;

    metals_ctrl(&actwk[2]);

    TEST_ASSERT_EQ_INT(ctx, 3, actwk[2].EXE_NO);

    reset_fixture();
    actwk[2].EXE_NO = 7;

    metals_ctrl(&actwk[2]);

    TEST_ASSERT_TRUE(ctx, actwk[2].PAT_ADR == &m_stand_map);
    TEST_ASSERT_EQ_INT(ctx, 64, actwk[2].TM_CNT);
    TEST_ASSERT_EQ_INT(ctx, 8, actwk[2].EXE_NO);

    reset_fixture();
    actwk[2].EXE_NO = 14;
    actwk[2].X_SPEED.l = -1;

    metals_ctrl(&actwk[2]);

    TEST_ASSERT_EQ_INT(ctx, 368, actwk[2].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[2].EXE_NO);

    reset_fixture();
    actwk[2].EXE_NO = 15;
    actwk[2].X_SPEED.l = 0x10000;
    actwk[2].XPOSI.w.h = 320;

    metals_ctrl(&actwk[2]);

    TEST_ASSERT_TRUE(ctx, (actwk[2].SPR_FLG & 16) != 0);
}

static void test_remaining_controller_branch_alternatives(test_context *ctx) {
    reset_fixture();
    actwk[0].X_SPEED.l = 1;
    actwk[0].XPOSI.w.h = 100;
    actwk[2].EXE_NO = 5;
    TEST_ASSERT_EQ_INT(ctx, 0, s_metalchk(&actwk[0]));
    actwk[2].EXE_NO = 2;
    actwk[2].X_SPEED.l = 1;
    TEST_ASSERT_EQ_INT(ctx, 0, s_metalchk(&actwk[0]));
    actwk[2].X_SPEED.l = -1;
    actwk[2].XPOSI.w.h = 120;
    TEST_ASSERT_EQ_INT(ctx, 0, s_metalchk(&actwk[0]));
    actwk[0].X_SPEED.l = -1;
    actwk[2].X_SPEED.l = -1;
    TEST_ASSERT_EQ_INT(ctx, 0, s_metalchk(&actwk[0]));
    actwk[0].X_SPEED.l = 1;
    actwk[2].X_SPEED.l = -1;
    actwk[2].XPOSI.w.h = 95;
    TEST_ASSERT_EQ_INT(ctx, -1, s_metalchk(&actwk[0]));

    reset_fixture();
    actwk[0].EXE_NO = 3;
    actwk[0].X_SPEED.l = -196608;
    actwk[0].XPOSI.w.h = 100;
    sonic_ctrl(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 10240, actwk[0].X_OFFSET);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].SPR_FLG & 128);

    reset_fixture();
    actwk[0].EXE_NO = 4;
    actwk[0].PAT_NO = 5;
    actwk[0].XPOSI.w.h = 100;
    actwk[2].EXE_NO = 0;
    actwk[2].XPOSI.w.h = 120;
    sonic_ctrl(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 9, actwk[0].EXE_NO);

    reset_fixture();
    actwk[0].EXE_NO = 6;
    actwk[0].TM_CNT = 1;
    sonic_ctrl(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 6, actwk[0].EXE_NO);

    reset_fixture();
    actwk[0].EXE_NO = 8;
    actwk[0].XPOSI.w.h = 100;
    actwk[2].XPOSI.w.h = 104;
    sonic_ctrl(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].EXE_NO);

    reset_fixture();
    actwk[0].EXE_NO = 8;
    actwk[0].XPOSI.w.h = 100;
    actwk[0].TM_CNT = 5;
    actwk[2].XPOSI.w.h = 50;
    sonic_ctrl(&actwk[0]);
    TEST_ASSERT_TRUE(ctx, (actwk[0].SPR_FLG & 128) != 0);

    reset_fixture();
    animal_flg = 1;
    actwk[0].EXE_NO = 11;
    actwk[0].SPR_FLG = 4;
    actwk[2].EXE_NO = 12;
    sonic_ctrl(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 11, actwk[0].EXE_NO);

    reset_fixture();
    animal_flg = 1;
    actwk[0].EXE_NO = 11;
    actwk[0].SPR_FLG = 4;
    actwk[2].EXE_NO = 13;
    actwk[2].X_SPEED.l = 1;
    sonic_ctrl(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 11, actwk[0].EXE_NO);

    reset_fixture();
    actwk[0].EXE_NO = 13;
    actwk[0].free[1] = 0;
    actwk[0].TM_CNT = 2;
    sonic_ctrl(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].TM_CNT);

    reset_fixture();
    actwk[0].XPOSI.w.h = 200;
    actwk[1].EXE_NO = 1;
    actwk[1].XPOSI.w.h = 100;
    e_runinit(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[1].SPR_FLG & 128);

    reset_fixture();
    actwk[1].XPOSI.w.h = 383;
    actwk[1].X_SPEED.l = 0x10000;
    emmymove(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 385, actwk[1].XPOSI.w.h);

    reset_fixture();
    actwk[0].ACT_NO = 1;
    actwk[0].XPOSI.w.h = 200;
    actwk[1].EXE_NO = 2;
    actwk[1].XPOSI.w.h = 100;
    actwk[1].X_SPEED.l = -1;
    random_values[0] = 1;
    emmy_exeset(&actwk[1], &actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 9, actwk[1].EXE_NO);

    reset_fixture();
    actwk[0].XPOSI.w.h = 200;
    actwk[1].XPOSI.w.h = 100;
    e_kissset(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[1].SPR_FLG & 128);

    reset_fixture();
    actwk[0].XPOSI.w.h = 50;
    actwk[1].XPOSI.w.h = 100;
    e_jumpset(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, -65536, actwk[1].X_SPEED.l);
    TEST_ASSERT_EQ_INT(ctx, 7, actwk[11].ACT_NO);

    reset_fixture();
    get_actwk_return = -1;
    actwk[0].XPOSI.w.h = 200;
    actwk[1].XPOSI.w.h = 100;
    e_jumpset(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 65536, actwk[1].X_SPEED.l);

    reset_fixture();
    poseno = 0;
    actwk[1].EXE_NO = 10;
    actwk[1].X_SPEED.l = -actwk[1].X_ACCEL.l;
    emmyturn(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 20, actwk[1].TM_CNT);

    reset_fixture();
    random_values[0] = 0;
    actwk[0].XPOSI.w.h = 200;
    actwk[1].XPOSI.w.h = 100;
    e_standset(&actwk[1]);
    TEST_ASSERT_TRUE(ctx, actwk[1].PAT_ADR == &estand_map);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[1].SPR_FLG & 128);

    reset_fixture();
    actwk[0].XPOSI.w.h = 10;
    actwk[1].XPOSI.w.h = 100;
    actwk[1].TM_CNT = 2;
    emmymoji(&actwk[1]);
    TEST_ASSERT_TRUE(ctx, (actwk[1].SPR_FLG & 128) != 0);

    reset_fixture();
    actwk[0].X_SPEED.l = 0;
    actwk[2].X_SPEED.l = 0;
    actwk[1].XPOSI.w.h = 116;
    emmypose(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 9, actwk[1].EXE_NO);

    reset_fixture();
    actwk[1].X_SPEED.l = 0x10000;
    actwk[1].XPOSI.w.h = 320;
    emmyesc(&actwk[1]);
    TEST_ASSERT_TRUE(ctx, (actwk[1].SPR_FLG & 16) != 0);

    reset_fixture();
    actwk[0].ACT_NO = 1;
    actwk[0].XPOSI.w.h = 120;
    actwk[1].XPOSI.w.h = 220;
    actwk[2].EXE_NO = 2;
    actwk[2].XPOSI.w.h = 100;
    actwk[2].TM_CNT = 0;
    metalsfly(&actwk[2]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[2].TM_CNT);

    reset_fixture();
    actwk[0].ACT_NO = 1;
    actwk[0].XPOSI.w.h = 10;
    actwk[1].XPOSI.w.h = 200;
    actwk[2].EXE_NO = 2;
    actwk[2].XPOSI.w.h = 280;
    actwk[2].X_SPEED.w.h = 1;
    actwk[2].TM_CNT = 0;
    metalsfly(&actwk[2]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[2].TM_CNT);

    reset_fixture();
    actwk[0].ACT_NO = 1;
    actwk[0].XPOSI.w.h = 10;
    actwk[1].XPOSI.w.h = 120;
    actwk[2].EXE_NO = 2;
    actwk[2].XPOSI.w.h = 100;
    actwk[2].TM_CNT = 0;
    metalsfly(&actwk[2]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[2].TM_CNT);

    reset_fixture();
    actwk[0].EXE_NO = 9;
    actwk[2].EXE_NO = 6;
    actwk[2].TM_CNT = 0;
    metals_ctrl(&actwk[2]);
    TEST_ASSERT_EQ_INT(ctx, 7, actwk[2].EXE_NO);

    reset_fixture();
    actwk[0].XPOSI.w.h = 200;
    actwk[2].EXE_NO = 8;
    metalstand(&actwk[2]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[2].SPR_FLG & 128);

    reset_fixture();
    actwk[0].XPOSI.w.h = 200;
    actwk[2].EXE_NO = 9;
    m_startset(&actwk[2]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[2].SPR_FLG & 128);

    reset_fixture();
    actwk[0].XPOSI.w.h = 10;
    actwk[2].XPOSI.w.h = 100;
    actwk[2].EXE_NO = 10;
    metalstartcttbl(&actwk[2]);
    TEST_ASSERT_TRUE(ctx, (actwk[2].SPR_FLG & 128) != 0);

    reset_fixture();
    actwk[7].XPOSI.w.h = 0;
    actwk[7].YPOSI.w.h = -5;
    TEST_ASSERT_EQ_INT(ctx, -1, killchk(&actwk[7]));

    reset_fixture();
    actwk[2].free[2] = 1;
    m_sndchk(&actwk[2]);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[2].free[2]);
    reset_fixture();
    actwk[2].X_SPEED.l = 1;
    actwk[2].XPOSI.w.h = 6;
    m_sndchk(&actwk[2]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[2].free[2]);
    reset_fixture();
    actwk[2].X_SPEED.l = -1;
    actwk[2].XPOSI.w.h = 300;
    m_sndchk(&actwk[2]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[2].free[2]);
    actwk[2].XPOSI.w.h = 313;
    m_sndchk(&actwk[2]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[2].free[2]);
}

static void test_s_metalchk_positive_speed_else_path(test_context *ctx) {
    reset_fixture();
    actwk[0].X_SPEED.l = 0x10000;
    actwk[0].XPOSI.w.h = 40;
    actwk[2].EXE_NO = 2;
    actwk[2].X_SPEED.l = -0x10000;
    actwk[2].XPOSI.w.h = 100;

    TEST_ASSERT_EQ_INT(ctx, 0, s_metalchk(&actwk[0]));
    TEST_ASSERT_EQ_INT(ctx, 40, actwk[0].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 100, actwk[2].XPOSI.w.h);
}

static void test_heart_animal_and_bird_controllers(test_context *ctx) {
    reset_fixture();
    actwk[1].XPOSI.w.h = 123;
    actwk[1].YPOSI.w.h = 77;

    heart_ctrl(&actwk[3]);

    TEST_ASSERT_EQ_INT(ctx, 123, actwk[3].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 77, actwk[3].YPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, -32768, actwk[3].Y_SPEED.l);
    TEST_ASSERT_TRUE(ctx, actwk[3].PAT_ADR == &eheart_map);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[3].EXE_NO);

    reset_fixture();
    random_values[0] = 10;
    random_values[1] = 20;

    pocky_ctrl(&actwk[4]);

    TEST_ASSERT_EQ_INT(ctx, -10, actwk[4].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 144, actwk[4].YPOSI.w.h);
    TEST_ASSERT_TRUE(ctx, actwk[4].PAT_ADR == &pocky_map);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[4].EXE_NO);

    reset_fixture();
    random_values[0] = 30;

    flick_ctrl(&actwk[5]);

    TEST_ASSERT_EQ_INT(ctx, -10, actwk[5].XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 30, actwk[5].YPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 65536, actwk[5].X_SPEED.l);
    TEST_ASSERT_TRUE(ctx, actwk[5].PAT_ADR == &flick_map);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[5].EXE_NO);

    reset_fixture();
    random_values[0] = 11;
    random_values[1] = 12;
    pecky_ctrl(&actwk[6]);
    TEST_ASSERT_TRUE(ctx, actwk[6].PAT_ADR == &pecky_map);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[6].EXE_NO);

    reset_fixture();
    random_values[0] = 13;
    random_values[1] = 14;
    ricky_ctrl(&actwk[7]);
    TEST_ASSERT_EQ_INT(ctx, 164, actwk[7].YPOSI.w.h);
    TEST_ASSERT_TRUE(ctx, actwk[7].PAT_ADR == &ricky_map);

    reset_fixture();
    random_values[0] = 15;
    random_values[1] = 16;
    sheep_ctrl(&actwk[8]);
    TEST_ASSERT_EQ_INT(ctx, 16384, actwk[8].Y_OFFSET);
    TEST_ASSERT_TRUE(ctx, actwk[8].PAT_ADR == &sheep_map);

    reset_fixture();
    random_values[0] = 31;
    random_values[1] = 32;
    inco_ctrl(&actwk[9]);
    TEST_ASSERT_TRUE(ctx, actwk[9].PAT_ADR == &inco_map);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[9].EXE_NO);

    reset_fixture();
    random_values[0] = 33;
    random_values[1] = 34;
    hato_ctrl(&actwk[10]);
    TEST_ASSERT_TRUE(ctx, actwk[10].PAT_ADR == &hato_map);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[10].EXE_NO);

    reset_fixture();
    actwk[3].EXE_NO = 1;
    actwk[3].PAT_NO = 3;
    actwk[3].X_WIDE = 32;
    actwk[3].Y_WIDE = 4;
    heart_ctrl(&actwk[3]);
    TEST_ASSERT_TRUE(ctx, (actwk[3].SPR_FLG & 16) != 0);
}

static void test_sine_motion_and_move_controllers(test_context *ctx) {
    reset_fixture();
    random_values[0] = 512;
    sprite_status_thanks actor;
    memset(&actor, 0, sizeof(actor));
    actor.XPOSI.w.h = 20;
    actor.YPOSI.w.h = 30;
    actor.X_SPEED.l = 0x10000;
    actor.Y_SPEED.l = 0x20000;
    actor.X_WIDE = 8;
    actor.Y_WIDE = 4;
    actor.SIN_CNT = 128;

    sinvmove(&actor);

    TEST_ASSERT_EQ_INT(ctx, 29, actor.XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 32, actor.YPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 8, actor.X_OFFSET);
    TEST_ASSERT_EQ_INT(ctx, 1, sign_count);

    reset_fixture();
    random_values[0] = 512;
    memset(&actor, 0, sizeof(actor));
    actor.XPOSI.w.h = 20;
    actor.YPOSI.w.h = 30;
    actor.X_SPEED.l = 0x10000;
    actor.Y_SPEED.l = 0x20000;
    actor.X_WIDE = 8;
    actor.Y_WIDE = 4;
    actor.SIN_CNT = 128;

    sinhmove(&actor);

    TEST_ASSERT_EQ_INT(ctx, 21, actor.XPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 36, actor.YPOSI.w.h);
    TEST_ASSERT_EQ_INT(ctx, 4, actor.Y_OFFSET);

    reset_fixture();
    actwk[6].EXE_NO = 1;
    actwk[6].XPOSI.w.h = 400;
    actwk[6].YPOSI.w.h = 100;
    animalmove(&actwk[6]);
    TEST_ASSERT_TRUE(ctx, (actwk[6].SPR_FLG & 16) != 0);

    reset_fixture();
    actwk[6].XPOSI.w.h = 10;
    actwk[6].YPOSI.w.h = 160;
    actwk[6].Y_SPEED.l = 0x10000;
    animalmove(&actwk[6]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[6].Y_ACCEL.l);

    reset_fixture();
    actwk[7].EXE_NO = 1;
    actwk[7].XPOSI.w.h = 10;
    actwk[7].YPOSI.w.h = 20;
    actwk[7].X_SPEED.l = 0x10000;
    actwk[7].X_WIDE = 8;
    actwk[7].Y_WIDE = 4;
    birdmove(&actwk[7]);
    TEST_ASSERT_EQ_INT(ctx, 11, actwk[7].XPOSI.w.h);

    reset_fixture();
    actwk[7].XPOSI.w.h = -40;
    birdmove(&actwk[7]);
    TEST_ASSERT_TRUE(ctx, (actwk[7].SPR_FLG & 16) != 0);

    reset_fixture();
    random_values[0] = 511;
    memset(&actor, 0, sizeof(actor));
    actor.XPOSI.w.h = 20;
    actor.YPOSI.w.h = 30;
    actor.X_SPEED.l = 0;
    actor.Y_SPEED.l = 0;
    actor.X_WIDE = 8;
    actor.Y_WIDE = 4;
    actor.SIN_CNT = 510;
    sinhmove(&actor);
    TEST_ASSERT_EQ_INT(ctx, 6, actor.SIN_CNT);

    reset_fixture();
    random_values[0] = 511;
    memset(&actor, 0, sizeof(actor));
    actor.X_WIDE = 8;
    actor.Y_WIDE = 4;
    actor.SIN_CNT = 510;
    sinvmove(&actor);
    TEST_ASSERT_EQ_INT(ctx, 2, actor.SIN_CNT);
}

TEST_MAIN_BEGIN;
test_position_and_visibility_helpers_capture_thresholds(&ctx);
test_motion_helpers_move_and_kill_at_exact_bounds(&ctx);
test_escape_chk_forward_and_reverse_death_windows(&ctx);
test_sonic_initial_and_setup_states(&ctx);
test_sonic_turn_ball_and_exit_branches(&ctx);
test_sonic_run_stand_wait_and_ball_motion_states(&ctx);
test_sonic_collision_random_and_pit_paths(&ctx);
test_sonic_turn_exit_escape_and_helper_edges(&ctx);
test_emie_initial_and_heart_spawning_states(&ctx);
test_emie_move_turn_stand_and_exit_states(&ctx);
test_emmy_pre_dispatch_kiss_jump_and_catch_paths(&ctx);
test_metal_initial_stop_and_erase_states(&ctx);
test_metal_flight_stand_start_and_exit_states(&ctx);
test_metal_remaining_timed_and_sound_paths(&ctx);
test_metalsfly_and_standset_remaining_paths(&ctx);
test_remaining_controller_branch_alternatives(&ctx);
test_s_metalchk_positive_speed_else_path(&ctx);
test_heart_animal_and_bird_controllers(&ctx);
test_sine_motion_and_move_controllers(&ctx);
TEST_MAIN_END
