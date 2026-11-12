#include <string.h>

#include "support/test_runner.h"
#include "src/types.h"

sprite_status actwk[128];
Uint8 flagwork[766];
Uint8 time_flag;
int_union scra_h_posit;

static int hitchk_count;
static sprite_status *hitchk_actor;
static sprite_status *hitchk_player;
static Sint16 hitchk_result;
static int actionsub_count;
static sprite_status *actionsub_actor;
static int frameout_count;
static sprite_status *frameout_actor;

Sint16 hitchk(sprite_status *pActwk, sprite_status *pPlayerwk);
void actionsub(sprite_status *pActwk);
void frameout(sprite_status *pActwk);

#include "src/r4/wall4.c"

Sint16 hitchk(sprite_status *pActwk, sprite_status *pPlayerwk) {
    ++hitchk_count;
    hitchk_actor = pActwk;
    hitchk_player = pPlayerwk;
    return hitchk_result;
}

void actionsub(sprite_status *pActwk) {
    ++actionsub_count;
    actionsub_actor = pActwk;
}

void frameout(sprite_status *pActwk) {
    ++frameout_count;
    frameout_actor = pActwk;
}

static void reset_wall4_state(void) {
    memset(actwk, 0, sizeof(actwk));
    memset(flagwork, 0, sizeof(flagwork));
    time_flag = 0;
    scra_h_posit.l = 0;
    hitchk_count = 0;
    hitchk_actor = 0;
    hitchk_player = 0;
    hitchk_result = 0;
    actionsub_count = 0;
    actionsub_actor = 0;
    frameout_count = 0;
    frameout_actor = 0;
}

static void assert_wall4_callbacks(test_context *ctx, sprite_status *wall) {
    TEST_ASSERT_EQ_INT(ctx, 1, hitchk_count);
    TEST_ASSERT_TRUE(ctx, hitchk_actor == wall);
    TEST_ASSERT_TRUE(ctx, hitchk_player == &actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_TRUE(ctx, actionsub_actor == wall);
}

static void test_wall4_pattern_captures_tile_and_offsets(test_context *ctx) {
    TEST_ASSERT_TRUE(ctx, pat_wall4[0] == &wall4_pat0);
    TEST_ASSERT_EQ_INT(ctx, 1, wall4_pat0.cnt);
    TEST_ASSERT_EQ_INT(ctx, -64, wall4_pat0.spra[0].xoff);
    TEST_ASSERT_EQ_INT(ctx, -64, wall4_pat0.spra[0].yoff);
    TEST_ASSERT_EQ_INT(ctx, SPRITE_WALL4_BASE, wall4_pat0.spra[0].index);
}

static void test_wall4_initial_clear_flag_path_runs_callbacks(
    test_context *ctx) {
    sprite_status *wall = &actwk[1];

    reset_wall4_state();
    time_flag = 4;
    wall->cdsts = 2;

    wall4(wall);

    assert_wall4_callbacks(ctx, wall);
    TEST_ASSERT_EQ_INT(ctx, 0, frameout_count);
}

static void test_wall4_initial_tall_clear_flag_path_runs_callbacks(
    test_context *ctx) {
    sprite_status *wall = &actwk[2];

    reset_wall4_state();
    wall->userflag.b.h = 1;

    wall4(wall);

    assert_wall4_callbacks(ctx, wall);
    TEST_ASSERT_EQ_INT(ctx, 0, frameout_count);
}

static void test_wall4_initial_done_flag_paths_run_callbacks(test_context *ctx) {
    sprite_status *wall = &actwk[3];

    reset_wall4_state();
    flagwork[0] = 1;

    wall4(wall);

    assert_wall4_callbacks(ctx, wall);
    TEST_ASSERT_EQ_INT(ctx, 0, frameout_count);

    reset_wall4_state();
    flagwork[0] = 1;
    wall = &actwk[4];
    wall->userflag.b.h = 1;

    wall4(wall);

    assert_wall4_callbacks(ctx, wall);
    TEST_ASSERT_EQ_INT(ctx, 0, frameout_count);
}

static void test_wall4_sense_ignores_vertical_and_horizontal_misses(
    test_context *ctx) {
    sprite_status *wall = &actwk[5];

    reset_wall4_state();
    wall->r_no0 = 2;
    wall->yposi.w.h = 200;
    actwk[0].yposi.w.h = 0;

    wall4(wall);

    assert_wall4_callbacks(ctx, wall);
    TEST_ASSERT_EQ_INT(ctx, 0, frameout_count);

    reset_wall4_state();
    wall = &actwk[6];
    wall->r_no0 = 2;
    wall->xposi.w.h = 100;
    actwk[0].xposi.w.h = 100;
    actwk[0].yposi.w.h = 0;

    wall4(wall);

    assert_wall4_callbacks(ctx, wall);
    TEST_ASSERT_EQ_INT(ctx, 0, frameout_count);
}

static void test_wall4_sense_hit_then_wait_sets_flagwork(test_context *ctx) {
    sprite_status *wall = &actwk[7];

    reset_wall4_state();
    time_flag = 12;
    wall->r_no0 = 2;
    wall->xposi.w.h = 100;
    actwk[0].xposi.w.h = 260;
    actwk[0].yposi.w.h = 0;
    wall4_init(wall);
    wall->r_no0 = 2;

    wall4(wall);

    assert_wall4_callbacks(ctx, wall);
    TEST_ASSERT_EQ_INT(ctx, 0, frameout_count);

    hitchk_count = 0;
    hitchk_actor = 0;
    hitchk_player = 0;
    actionsub_count = 0;
    actionsub_actor = 0;

    wall4(wall);

    assert_wall4_callbacks(ctx, wall);
    TEST_ASSERT_EQ_INT(ctx, 1, flagwork[12] & 1);
}

static void test_wall4_wait_without_trigger_leaves_flagwork_clear(
    test_context *ctx) {
    sprite_status *wall = &actwk[8];

    reset_wall4_state();
    time_flag = 18;
    wall4_init(wall);
    wall->r_no0 = 4;

    wall4(wall);

    assert_wall4_callbacks(ctx, wall);
    TEST_ASSERT_EQ_INT(ctx, 0, flagwork[18] & 1);
}

/* The R43 boss chase opens its gates by setting `triggered` on the wall4 actor
   (see boss_4.c make_gate/egg4air_01_next), which is the same byte wall4_wait
   consumes here. */
static void test_wall4_wait_with_trigger_drops_the_wall(test_context *ctx) {
    sprite_status *wall = &actwk[8];

    reset_wall4_state();
    time_flag = 18;
    wall4_init(wall);
    wall->r_no0 = 4;

    wall4_get_work(wall)->triggered = 1;
    wall4(wall);

    assert_wall4_callbacks(ctx, wall);
    TEST_ASSERT_EQ_INT(ctx, 1, flagwork[18] & 1);
    TEST_ASSERT_EQ_INT(ctx, 6, wall->r_no0);
}

static void test_wall4_down_covers_counting_and_completion_paths(
    test_context *ctx) {
    sprite_status *wall = &actwk[9];

    reset_wall4_state();
    wall4_init(wall);
    wall->r_no0 = 6;

    wall4(wall);

    assert_wall4_callbacks(ctx, wall);
    TEST_ASSERT_EQ_INT(ctx, 0, frameout_count);

    hitchk_count = 0;
    hitchk_actor = 0;
    hitchk_player = 0;
    actionsub_count = 0;
    actionsub_actor = 0;

    for (int i = 0; i < 14; ++i) {
        hitchk_count = 0;
        hitchk_actor = 0;
        hitchk_player = 0;
        actionsub_count = 0;
        actionsub_actor = 0;
        wall4(wall);
    }

    hitchk_count = 0;
    hitchk_actor = 0;
    hitchk_player = 0;
    actionsub_count = 0;
    actionsub_actor = 0;

    wall4(wall);

    assert_wall4_callbacks(ctx, wall);
    TEST_ASSERT_EQ_INT(ctx, 0, frameout_count);
}

static void test_frameout_s_skips_userflag_one_and_near_screen(
    test_context *ctx) {
    sprite_status *wall = &actwk[10];

    reset_wall4_state();
    wall->userflag.b.h = 1;
    wall->xposi.w.h = 2048;
    scra_h_posit.w.h = 0;

    frameout_s_(wall);

    TEST_ASSERT_EQ_INT(ctx, 0, frameout_count);

    reset_wall4_state();
    wall = &actwk[11];
    wall->xposi.w.h = 256;
    scra_h_posit.w.h = 0;

    frameout_s_(wall);

    TEST_ASSERT_EQ_INT(ctx, 0, frameout_count);
}

static void test_frameout_s_clears_flagwork_and_frames_out(test_context *ctx) {
    sprite_status *wall = &actwk[12];

    reset_wall4_state();
    time_flag = 20;
    wall4_init(wall);
    wall->xposi.w.h = 1024;
    scra_h_posit.w.h = 128;
    flagwork[20] = 129;

    frameout_s_(wall);

    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actor == wall);
    TEST_ASSERT_EQ_INT(ctx, 1, flagwork[20]);
}

TEST_MAIN_BEGIN;
    test_wall4_pattern_captures_tile_and_offsets(&ctx);
    test_wall4_initial_clear_flag_path_runs_callbacks(&ctx);
    test_wall4_initial_tall_clear_flag_path_runs_callbacks(&ctx);
    test_wall4_initial_done_flag_paths_run_callbacks(&ctx);
    test_wall4_sense_ignores_vertical_and_horizontal_misses(&ctx);
    test_wall4_sense_hit_then_wait_sets_flagwork(&ctx);
    test_wall4_wait_without_trigger_leaves_flagwork_clear(&ctx);
    test_wall4_wait_with_trigger_drops_the_wall(&ctx);
    test_wall4_down_covers_counting_and_completion_paths(&ctx);
    test_frameout_s_skips_userflag_one_and_near_screen(&ctx);
    test_frameout_s_clears_flagwork_and_frames_out(&ctx);
TEST_MAIN_END
