#include <string.h>

#include "support/test_runner.h"
#include "src/types.h"
#include "src/spring_work.h"

sprite_status actwk[128];

static int frameout_s00_count;
static sprite_status *frameout_s00_actor;
static Sint16 frameout_s00_xpos;
static int frameout_count;
static sprite_status *frameout_actor;
static int actwkchk2_count;
static sprite_status *actwkchk2_actor;
static sprite_status *actwkchk2_queue[8];
static int actwkchk2_queue_count;
static int actwkchk2_queue_index;
static int emycol_d_count;
static sprite_status *emycol_d_actor;
static Sint16 emycol_d_results[8];
static int emycol_d_result_count;
static int emycol_r_count;
static sprite_status *emycol_r_actor;
static Uint8 emycol_r_sprhs;
static Sint16 emycol_r_result;
static int emycol_l_count;
static sprite_status *emycol_l_actor;
static Uint8 emycol_l_sprhs;
static Sint16 emycol_l_result;
static int speedset2_count;
static sprite_status *speedset2_actor;
static int patchg_count;
static sprite_status *patchg_actor;
static Uint8 **patchg_table;
static int actionsub_count;
static sprite_status *actionsub_actor;

Sint32 frameout_s00(sprite_status *pActwk, Sint16 xposi);
void frameout(sprite_status *pActwk);
Sint32 actwkchk2(sprite_status *pActwk, sprite_status **ppNewActwk);
Sint16 emycol_d(sprite_status *pActwk);
Sint16 emycol_r(sprite_status *pActwk, Uint8 sprhs);
Sint16 emycol_l(sprite_status *pActwk, Uint8 sprhs);
void speedset2(sprite_status *pActwk);
void patchg(sprite_status *patchgwk, Uint8 **pat_dat);
void actionsub(sprite_status *pActwk);

#include "src/r8/koma8.c"

Sint32 frameout_s00(sprite_status *pActwk, Sint16 xposi) {
    ++frameout_s00_count;
    frameout_s00_actor = pActwk;
    frameout_s00_xpos = xposi;
    return 0;
}

void frameout(sprite_status *pActwk) {
    ++frameout_count;
    frameout_actor = pActwk;
}

Sint32 actwkchk2(sprite_status *pActwk, sprite_status **ppNewActwk) {
    ++actwkchk2_count;
    actwkchk2_actor = pActwk;
    if (actwkchk2_queue_index >= actwkchk2_queue_count) {
        *ppNewActwk = 0;
        return -1;
    }
    *ppNewActwk = actwkchk2_queue[actwkchk2_queue_index++];
    return 0;
}

Sint16 emycol_d(sprite_status *pActwk) {
    ++emycol_d_count;
    emycol_d_actor = pActwk;
    if (emycol_d_count <= emycol_d_result_count) {
        return emycol_d_results[emycol_d_count - 1];
    }
    return 0;
}

Sint16 emycol_r(sprite_status *pActwk, Uint8 sprhs) {
    ++emycol_r_count;
    emycol_r_actor = pActwk;
    emycol_r_sprhs = sprhs;
    return emycol_r_result;
}

Sint16 emycol_l(sprite_status *pActwk, Uint8 sprhs) {
    ++emycol_l_count;
    emycol_l_actor = pActwk;
    emycol_l_sprhs = sprhs;
    return emycol_l_result;
}

void speedset2(sprite_status *pActwk) {
    ++speedset2_count;
    speedset2_actor = pActwk;
}

void patchg(sprite_status *patchgwk, Uint8 **pat_dat) {
    ++patchg_count;
    patchg_actor = patchgwk;
    patchg_table = pat_dat;
}

void actionsub(sprite_status *pActwk) {
    ++actionsub_count;
    actionsub_actor = pActwk;
}

static void reset_logs(void) {
    frameout_s00_count = 0;
    frameout_s00_actor = 0;
    frameout_s00_xpos = 0;
    frameout_count = 0;
    frameout_actor = 0;
    actwkchk2_count = 0;
    actwkchk2_actor = 0;
    memset(actwkchk2_queue, 0, sizeof(actwkchk2_queue));
    actwkchk2_queue_count = 0;
    actwkchk2_queue_index = 0;
    emycol_d_count = 0;
    emycol_d_actor = 0;
    memset(emycol_d_results, 0, sizeof(emycol_d_results));
    emycol_d_result_count = 0;
    emycol_r_count = 0;
    emycol_r_actor = 0;
    emycol_r_sprhs = 0;
    emycol_r_result = 7;
    emycol_l_count = 0;
    emycol_l_actor = 0;
    emycol_l_sprhs = 0;
    emycol_l_result = 7;
    speedset2_count = 0;
    speedset2_actor = 0;
    patchg_count = 0;
    patchg_actor = 0;
    patchg_table = 0;
    actionsub_count = 0;
    actionsub_actor = 0;
}

static void reset_koma8_state(void) {
    memset(actwk, 0, sizeof(actwk));
    reset_logs();
}

static void queue_actwkchk2(sprite_status *actor) {
    actwkchk2_queue[actwkchk2_queue_count++] = actor;
}

static void set_koma8_word(sprite_status *actor, int offset, Sint16 value) {
    koma8_work *work = koma8_get_work(actor);

    switch (offset) {
    case 12:
        work->origin_x = value;
        break;
    case 16:
        work->travel_range = value;
        break;
    }
}

static void set_emycol_d_results(Sint16 first, Sint16 second) {
    emycol_d_results[0] = first;
    emycol_d_results[1] = second;
    emycol_d_result_count = 2;
}

static void assert_frameout_s00_called(test_context *ctx, sprite_status *actor,
                                       Sint16 expected_xpos) {
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_s00_count);
    TEST_ASSERT_TRUE(ctx, frameout_s00_actor == actor);
    TEST_ASSERT_EQ_INT(ctx, expected_xpos, frameout_s00_xpos);
}

static void assert_move2_callbacks(test_context *ctx, sprite_status *actor) {
    TEST_ASSERT_EQ_INT(ctx, 1, speedset2_count);
    TEST_ASSERT_TRUE(ctx, speedset2_actor == actor);
    TEST_ASSERT_EQ_INT(ctx, 1, patchg_count);
    TEST_ASSERT_TRUE(ctx, patchg_actor == actor);
    TEST_ASSERT_TRUE(ctx, patchg_table == komachg);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_TRUE(ctx, actionsub_actor == actor);
    assert_frameout_s00_called(ctx, actor, 100);
}

static void test_koma8_patterns_capture_default_literal_data(test_context *ctx) {
    TEST_ASSERT_TRUE(ctx, komachg[0] == komachg0);
    TEST_ASSERT_EQ_INT(ctx, 8, komachg0[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, komachg0[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, komachg0[2]);
    TEST_ASSERT_EQ_INT(ctx, 255, komachg0[3]);
    TEST_ASSERT_TRUE(ctx, koma8pat[0] == &kom00);
    TEST_ASSERT_TRUE(ctx, koma8pat[1] == &kom01);
    TEST_ASSERT_EQ_INT(ctx, 1, kom00.cnt);
    TEST_ASSERT_EQ_INT(ctx, -8, kom00.spra[0].xoff);
    TEST_ASSERT_EQ_INT(ctx, -8, kom00.spra[0].yoff);
    TEST_ASSERT_EQ_INT(ctx, SPRITE_KOMA8_BASE, kom00.spra[0].index);
    TEST_ASSERT_EQ_INT(ctx, SPRITE_KOMA8_BASE + 1, kom01.spra[0].index);
}

static void test_koma8_initializes_main_actor_and_spawned_spark(
    test_context *ctx) {
    sprite_status *actor = &actwk[3];
    sprite_status *spark = &actwk[20];

    reset_koma8_state();
    actor->xposi.w.h = 100;
    actor->yposi.w.h = 200;
    actor->userflag.b.h = 18;
    queue_actwkchk2(spark);
    emycol_d_results[0] = 0;
    emycol_d_result_count = 1;

    koma8(actor);

    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk2_count);
    TEST_ASSERT_TRUE(ctx, actwkchk2_actor == actor);
    /* The spawned child is a spring: it must be configured in spring_work. */
    TEST_ASSERT_EQ_INT(ctx, 10, spark->actno);
    TEST_ASSERT_EQ_INT(ctx, -16, spring_work_get(spark)->follow_y_offset);
    TEST_ASSERT_EQ_INT(ctx, 3, spring_work_get(spark)->parent_index);
    assert_frameout_s00_called(ctx, actor, 100);
}

static void test_koma8_initial_negative_userflag_uses_short_range_and_lands(
    test_context *ctx) {
    sprite_status *actor = &actwk[3];
    sprite_status *spark = &actwk[20];

    reset_koma8_state();
    actor->xposi.w.h = 100;
    actor->yposi.w.h = 200;
    actor->userflag.b.h = -1;
    queue_actwkchk2(spark);
    emycol_d_results[0] = -5;
    emycol_d_result_count = 1;

    koma8(actor);

    assert_frameout_s00_called(ctx, actor, 100);
}

static void test_koma8_initialization_frames_out_when_allocator_fails(
    test_context *ctx) {
    sprite_status *actor = &actwk[3];

    reset_koma8_state();
    actor->xposi.w.h = 100;

    koma8(actor);

    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk2_count);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actor == actor);
    TEST_ASSERT_EQ_INT(ctx, 0, emycol_d_count);
    assert_frameout_s00_called(ctx, actor, 100);
}

static void test_koma8_fall_continues_until_ground_collision(test_context *ctx) {
    sprite_status *actor = &actwk[3];

    reset_koma8_state();
    actor->r_no0 = 2;
    actor->xposi.w.h = 100;
    actor->yposi.w.h = 200;
    set_koma8_word(actor, 12, 100);
    emycol_d_results[0] = 3;
    emycol_d_result_count = 1;

    koma8(actor);

    TEST_ASSERT_EQ_INT(ctx, 1, emycol_d_count);
    TEST_ASSERT_EQ_INT(ctx, 0, actionsub_count);
    assert_frameout_s00_called(ctx, actor, 100);

    reset_logs();
    actor->yposi.w.h = 201;
    actor->r_no0 = 2;
    set_koma8_word(actor, 12, 100);
    emycol_d_results[0] = -2;
    emycol_d_result_count = 1;

    koma8(actor);

    assert_frameout_s00_called(ctx, actor, 100);
}

static void test_koma8_move2_reverses_when_forward_wall_is_close(
    test_context *ctx) {
    sprite_status *actor = &actwk[3];

    reset_koma8_state();
    actor->r_no0 = 4;
    actor->xspeed.w = 256;
    actor->xposi.w.h = 110;
    actor->yposi.w.h = 200;
    set_koma8_word(actor, 12, 100);
    set_koma8_word(actor, 16, 80);
    emycol_r_result = 6;

    koma8(actor);

    TEST_ASSERT_EQ_INT(ctx, 1, emycol_r_count);
    TEST_ASSERT_TRUE(ctx, emycol_r_actor == actor);
    TEST_ASSERT_EQ_INT(ctx, 32, emycol_r_sprhs);
    TEST_ASSERT_EQ_INT(ctx, 0, emycol_d_count);
    assert_move2_callbacks(ctx, actor);
}

static void test_koma8_move2_reverses_when_backward_wall_is_close(
    test_context *ctx) {
    sprite_status *actor = &actwk[3];

    reset_koma8_state();
    actor->r_no0 = 4;
    actor->xspeed.w = -256;
    actor->xposi.w.h = 90;
    set_koma8_word(actor, 12, 100);
    set_koma8_word(actor, 16, 80);
    emycol_l_result = 6;

    koma8(actor);

    TEST_ASSERT_EQ_INT(ctx, 1, emycol_l_count);
    TEST_ASSERT_TRUE(ctx, emycol_l_actor == actor);
    TEST_ASSERT_EQ_INT(ctx, 232, emycol_l_sprhs);
    assert_move2_callbacks(ctx, actor);
}

static void test_koma8_move2_reverses_at_horizontal_range_limit(
    test_context *ctx) {
    sprite_status *actor = &actwk[3];

    reset_koma8_state();
    actor->r_no0 = 4;
    actor->xspeed.w = 256;
    actor->xposi.w.h = 180;
    set_koma8_word(actor, 12, 100);
    set_koma8_word(actor, 16, 80);
    emycol_r_result = 7;

    koma8(actor);

    TEST_ASSERT_EQ_INT(ctx, 0, emycol_d_count);
    assert_move2_callbacks(ctx, actor);

    reset_logs();
    actor->xspeed.w = -256;
    actor->xposi.w.h = 20;
    set_koma8_word(actor, 12, 100);
    set_koma8_word(actor, 16, 80);
    emycol_l_result = 7;

    koma8(actor);

    TEST_ASSERT_EQ_INT(ctx, 0, emycol_d_count);
    assert_move2_callbacks(ctx, actor);
}

static void test_koma8_move2_reverses_on_steep_floor_offsets(
    test_context *ctx) {
    sprite_status *actor = &actwk[3];

    reset_koma8_state();
    actor->r_no0 = 4;
    actor->xspeed.w = 256;
    actor->xposi.w.h = 110;
    set_koma8_word(actor, 12, 100);
    set_koma8_word(actor, 16, 80);
    emycol_r_result = 7;
    emycol_d_results[0] = -8;
    emycol_d_result_count = 1;

    koma8(actor);

    assert_move2_callbacks(ctx, actor);

    reset_logs();
    actor->xspeed.w = 256;
    actor->xposi.w.h = 110;
    set_koma8_word(actor, 12, 100);
    set_koma8_word(actor, 16, 80);
    emycol_r_result = 7;
    emycol_d_results[0] = 8;
    emycol_d_result_count = 1;

    koma8(actor);

    assert_move2_callbacks(ctx, actor);
}

static void test_koma8_move2_follows_gentle_floor_without_reversing(
    test_context *ctx) {
    sprite_status *actor = &actwk[3];

    reset_koma8_state();
    actor->r_no0 = 4;
    actor->xspeed.w = 256;
    actor->xposi.w.h = 110;
    actor->yposi.w.h = 200;
    set_koma8_word(actor, 12, 100);
    set_koma8_word(actor, 16, 80);
    emycol_r_result = 7;
    emycol_d_results[0] = 7;
    emycol_d_result_count = 1;

    koma8(actor);

    TEST_ASSERT_EQ_INT(ctx, 1, emycol_d_count);
    assert_move2_callbacks(ctx, actor);
}

TEST_MAIN_BEGIN;
    test_koma8_patterns_capture_default_literal_data(&ctx);
    test_koma8_initializes_main_actor_and_spawned_spark(&ctx);
    test_koma8_initial_negative_userflag_uses_short_range_and_lands(&ctx);
    test_koma8_initialization_frames_out_when_allocator_fails(&ctx);
    test_koma8_fall_continues_until_ground_collision(&ctx);
    test_koma8_move2_reverses_when_forward_wall_is_close(&ctx);
    test_koma8_move2_reverses_when_backward_wall_is_close(&ctx);
    test_koma8_move2_reverses_at_horizontal_range_limit(&ctx);
    test_koma8_move2_reverses_on_steep_floor_offsets(&ctx);
    test_koma8_move2_follows_gentle_floor_without_reversing(&ctx);
TEST_MAIN_END
