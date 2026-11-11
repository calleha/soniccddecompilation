#include <string.h>

#include "support/test_runner.h"
#include "src/types.h"
#include "src/spring.h"

sprite_status actwk[128];
Sint16 time_stop;
int_union scra_h_posit;
int_union scra_v_posit;
Uint8 time_flag;
Uint8 flagwork[766];

static int actionsub_count;
static sprite_status *actionsub_actor;
static int actwkchk_count;
static sprite_status *actwkchk_queue[8];
static int actwkchk_queue_count;
static int actwkchk_queue_index;
static int ridechk_count;
static sprite_status *ridechk_actor;
static sprite_status *ridechk_player;
static Sint16 ridechk_results[8];
static int ridechk_result_count;
static int ridechk_result_index;
static int ride_on_clr_count;
static sprite_status *ride_on_clr_actor;
static sprite_status *ride_on_clr_player;
static int frameout_count;
static sprite_status *frameout_actor;
static int sinset_count;
static Uint8 sinset_angle;
static Sint16 sinset_sin;
static Sint16 sinset_cos;
static int speedset2_count;
static sprite_status *speedset2_actor;
static int emycol_u_count;
static Sint16 emycol_u_result;
static int frameout_s_count;
static sprite_status *frameout_s_actor;
static int ride_on_chk_count;
static sprite_status *ride_on_chk_actor;
static sprite_status *ride_on_chk_player;
static int soundset_count;
static Sint16 soundset_last;

void actionsub(sprite_status *pActwk);
Sint32 actwkchk(sprite_status **ppActwk);
Sint16 ridechk(sprite_status *pActwk, sprite_status *pPlayerwk);
Sint16 ride_on_clr(sprite_status *pActwk, sprite_status *pPlayerwk);
void frameout(sprite_status *pActwk);
void sinset(Uint8 kakudo, Sint16 *sin, Sint16 *cos);
void speedset2(sprite_status *pActwk);
Sint16 emycol_u(sprite_status *pActwk);
Sint32 frameout_s(sprite_status *pActwk);
Sint16 ride_on_chk(sprite_status *pActwk, sprite_status *pPlayerwk);
void soundset(Sint16 ReqNo);

#include "src/dai_rd1.c"

void actionsub(sprite_status *pActwk) {
    ++actionsub_count;
    actionsub_actor = pActwk;
}

Sint32 actwkchk(sprite_status **ppActwk) {
    ++actwkchk_count;
    if (actwkchk_queue_index >= actwkchk_queue_count) {
        *ppActwk = 0;
        return -1;
    }
    *ppActwk = actwkchk_queue[actwkchk_queue_index++];
    return 0;
}

Sint16 ridechk(sprite_status *pActwk, sprite_status *pPlayerwk) {
    ++ridechk_count;
    ridechk_actor = pActwk;
    ridechk_player = pPlayerwk;
    if (ridechk_result_index < ridechk_result_count)
        return ridechk_results[ridechk_result_index++];
    return 0;
}

Sint16 ride_on_clr(sprite_status *pActwk, sprite_status *pPlayerwk) {
    ++ride_on_clr_count;
    ride_on_clr_actor = pActwk;
    ride_on_clr_player = pPlayerwk;
    return 0;
}

void frameout(sprite_status *pActwk) {
    ++frameout_count;
    frameout_actor = pActwk;
}

void sinset(Uint8 kakudo, Sint16 *sin, Sint16 *cos) {
    ++sinset_count;
    sinset_angle = kakudo;
    *sin = sinset_sin;
    *cos = sinset_cos;
}

void speedset2(sprite_status *pActwk) {
    ++speedset2_count;
    speedset2_actor = pActwk;
}

Sint16 emycol_u(sprite_status *pActwk) {
    ++emycol_u_count;
    (void)pActwk;
    return emycol_u_result;
}

Sint32 frameout_s(sprite_status *pActwk) {
    ++frameout_s_count;
    frameout_s_actor = pActwk;
    return 0;
}

Sint16 ride_on_chk(sprite_status *pActwk, sprite_status *pPlayerwk) {
    ++ride_on_chk_count;
    ride_on_chk_actor = pActwk;
    ride_on_chk_player = pPlayerwk;
    return 0;
}

void soundset(Sint16 ReqNo) {
    ++soundset_count;
    soundset_last = ReqNo;
}

static void queue_actor(sprite_status *actor) {
    actwkchk_queue[actwkchk_queue_count++] = actor;
}

static void queue_ridechk_result(Sint16 value) {
    ridechk_results[ridechk_result_count++] = value;
}

static void reset_dai_rd1_state(void) {
    memset(actwk, 0, sizeof(actwk));
    memset(&scra_h_posit, 0, sizeof(scra_h_posit));
    memset(&scra_v_posit, 0, sizeof(scra_v_posit));
    memset(flagwork, 255, sizeof(flagwork));
    time_stop = 0;
    time_flag = 0;
    actionsub_count = 0;
    actionsub_actor = 0;
    actwkchk_count = 0;
    memset(actwkchk_queue, 0, sizeof(actwkchk_queue));
    actwkchk_queue_count = 0;
    actwkchk_queue_index = 0;
    ridechk_count = 0;
    ridechk_actor = 0;
    ridechk_player = 0;
    memset(ridechk_results, 0, sizeof(ridechk_results));
    ridechk_result_count = 0;
    ridechk_result_index = 0;
    ride_on_clr_count = 0;
    ride_on_clr_actor = 0;
    ride_on_clr_player = 0;
    frameout_count = 0;
    frameout_actor = 0;
    sinset_count = 0;
    sinset_angle = 0;
    sinset_sin = 0;
    sinset_cos = 0;
    speedset2_count = 0;
    speedset2_actor = 0;
    emycol_u_count = 0;
    emycol_u_result = 0;
    frameout_s_count = 0;
    frameout_s_actor = 0;
    ride_on_chk_count = 0;
    ride_on_chk_actor = 0;
    ride_on_chk_player = 0;
    soundset_count = 0;
    soundset_last = 0;
}

static void test_dodai_tables_capture_literal_contract(test_context *ctx) {
    TEST_ASSERT_TRUE(ctx, dai11a_pattbl[0] == &dai11a_pat1);
    TEST_ASSERT_TRUE(ctx, dai11a_pattbl[2] == &dai11a_pat3);
    TEST_ASSERT_EQ_INT(ctx, 1, dai11a_pat1.cnt);
    TEST_ASSERT_EQ_INT(ctx, -16, dai11a_pat1.spra[0].xoff);
    TEST_ASSERT_EQ_INT(ctx, SPRITE_DAI11A_BASE,
                       dai11a_pat1.spra[0].index);
    TEST_ASSERT_EQ_INT(ctx, SPRITE_DAI11A_BASE + 2,
                       dai11a_pat3.spra[0].index);
    TEST_ASSERT_EQ_INT(ctx, 16, dai_hstbl[0]);
    TEST_ASSERT_EQ_INT(ctx, 48, dai_hstbl[2]);
    TEST_ASSERT_EQ_INT(ctx, 2, dai_mcnttbl[0]);
    TEST_ASSERT_EQ_INT(ctx, 6, dai_mcnttbl[3]);
    TEST_ASSERT_TRUE(ctx, vfutapat[0] == &vfuta_pat0);
    TEST_ASSERT_EQ_INT(ctx, SPRITE_VFUTA_BASE, vfuta_pat0.spra[0].index);
    TEST_ASSERT_EQ_INT(ctx, SPRITE_VFUTA_BASE + 1, vfuta_pat1.spra[0].index);
}

static void test_dodai_init_sets_shape_and_spawns_child(test_context *ctx) {
    sprite_status *platform = &actwk[2];
    sprite_status *child = &actwk[9];

    reset_dai_rd1_state();
    platform->xposi.w.h = 100;
    platform->yposi.w.h = 200;
    platform->userflag.b.h = 9;
    platform->userflag.b.l = 10;
    queue_actor(child);

    dodai(platform);

    TEST_ASSERT_EQ_INT(ctx, 2, platform->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 4, platform->actflg);
    TEST_ASSERT_EQ_INT(ctx, 17598, platform->sproffset);
    TEST_ASSERT_EQ_INT(ctx, 2, platform->sprpri);
    TEST_ASSERT_TRUE(ctx, platform->patbase == dai11a_pattbl);
    TEST_ASSERT_EQ_INT(ctx, 1, platform->patno);
    TEST_ASSERT_EQ_INT(ctx, 32, platform->sprhsize);
    TEST_ASSERT_EQ_INT(ctx, 8, platform->sprvsize);
    TEST_ASSERT_EQ_INT(ctx, 4, dai_rd1_get_work(platform)->amplitude);
    TEST_ASSERT_EQ_INT(ctx, 10, child->actno);
    TEST_ASSERT_EQ_INT(ctx, 108, child->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 184, child->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 2, child->userflag.b.h);
    TEST_ASSERT_EQ_INT(ctx, 8, spring_work_get(child)->follow_x_offset);
    TEST_ASSERT_EQ_INT(ctx, -16, spring_work_get(child)->follow_y_offset);
    TEST_ASSERT_EQ_INT(ctx, 2, spring_work_get(child)->parent_index);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_TRUE(ctx, actionsub_actor == platform);
}

static void test_dodai_init_allocation_failure_clears_flag_and_frames_out(
    test_context *ctx) {
    sprite_status *platform = &actwk[2];

    reset_dai_rd1_state();
    platform->xposi.w.h = 100;
    platform->yposi.w.h = 200;
    platform->cdsts = 3;
    platform->userflag.b.l = 8;
    time_flag = 2;

    dodai(platform);

    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 127, flagwork[11]);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actor == platform);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
}

static void test_dodai_time_stop_and_offscreen_frameout(test_context *ctx) {
    sprite_status *platform = &actwk[2];

    reset_dai_rd1_state();
    platform->r_no0 = 2;
    time_stop = 1;

    dodai(platform);

    TEST_ASSERT_EQ_INT(ctx, 1, ridechk_count);
    TEST_ASSERT_TRUE(ctx, ridechk_actor == platform);
    TEST_ASSERT_TRUE(ctx, ridechk_player == &actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);

    reset_dai_rd1_state();
    platform->r_no0 = 2;
    platform->xposi.w.h = 1024;
    platform->cdsts = 1;
    dai_rd1_get_work(platform)->origin_x = 1024;
    scra_h_posit.w.h = 0;

    dodai(platform);

    TEST_ASSERT_EQ_INT(ctx, 1, ride_on_clr_count);
    TEST_ASSERT_TRUE(ctx, ride_on_clr_actor == platform);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_EQ_INT(ctx, 127, flagwork[3]);
}

static void test_dodai_motion_types_and_ride_counter(test_context *ctx) {
    sprite_status *platform = &actwk[2];

    reset_dai_rd1_state();
    platform->r_no0 = 2;
    platform->userflag.b.h = 0;
    dai_rd1_get_work(platform)->origin_y = 200;
    dai_rd1_get_work(platform)->amplitude = 8;
    sinset_sin = 32;

    dodai(platform);

    TEST_ASSERT_EQ_INT(ctx, 1, sinset_count);
    TEST_ASSERT_EQ_INT(ctx, 1, dai_rd1_get_work(platform)->phase);
    TEST_ASSERT_EQ_INT(ctx, 216, platform->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, ridechk_count);

    reset_dai_rd1_state();
    dai_rd1_get_work(platform)->ride_offset = 2;
    queue_ridechk_result(0);
    TEST_ASSERT_EQ_INT(ctx, 0, dodai_ride2(platform));
    TEST_ASSERT_EQ_INT(ctx, 1, dai_rd1_get_work(platform)->ride_offset);

    reset_dai_rd1_state();
    dai_rd1_get_work(platform)->ride_offset = 7;
    queue_ridechk_result(1);
    TEST_ASSERT_EQ_INT(ctx, 1, dodai_ride2(platform));
    TEST_ASSERT_EQ_INT(ctx, 8, dai_rd1_get_work(platform)->ride_offset);
}

static void test_dodai_diagonal_and_fixed_motion_types(test_context *ctx) {
    sprite_status *platform = &actwk[2];

    reset_dai_rd1_state();
    dai_rd1_get_work(platform)->origin_x = 100;
    dai_rd1_get_work(platform)->origin_y = 200;
    dai_rd1_get_work(platform)->amplitude = 8;
    sinset_sin = 32;
    dodai_lr(platform);
    TEST_ASSERT_EQ_INT(ctx, 116, platform->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 200, platform->yposi.w.h);

    reset_dai_rd1_state();
    dai_rd1_get_work(platform)->origin_x = 100;
    dai_rd1_get_work(platform)->origin_y = 200;
    dai_rd1_get_work(platform)->amplitude = 8;
    sinset_sin = 32;
    dodai_nA(platform);
    TEST_ASSERT_EQ_INT(ctx, 116, platform->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 216, platform->yposi.w.h);

    reset_dai_rd1_state();
    dai_rd1_get_work(platform)->origin_x = 100;
    dai_rd1_get_work(platform)->origin_y = 200;
    dai_rd1_get_work(platform)->amplitude = 8;
    sinset_sin = 32;
    dodai_nB(platform);
    TEST_ASSERT_EQ_INT(ctx, 84, platform->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 216, platform->yposi.w.h);

    reset_dai_rd1_state();
    dai_rd1_get_work(platform)->origin_y = 200;
    dai_rd1_get_work(platform)->ride_offset = 6;
    dodai_fix(platform);
    TEST_ASSERT_EQ_INT(ctx, 203, platform->yposi.w.h);
}

static void test_dodai_fall_and_rise_state_machines(test_context *ctx) {
    sprite_status *platform = &actwk[2];

    reset_dai_rd1_state();
    queue_ridechk_result(1);
    dodai_fal(platform);
    TEST_ASSERT_EQ_INT(ctx, 2, dai_rd1_get_work(platform)->state);
    TEST_ASSERT_EQ_INT(ctx, 29, dai_rd1_get_work(platform)->wait_timer);

    reset_dai_rd1_state();
    dai_rd1_get_work(platform)->state = 2;
    dai_rd1_get_work(platform)->wait_timer = 1;
    dodai_fal(platform);
    TEST_ASSERT_EQ_INT(ctx, 0, dai_rd1_get_work(platform)->wait_timer);

    reset_dai_rd1_state();
    dai_rd1_get_work(platform)->state = 2;
    platform->yspeed.w = 128;
    platform->yposi.l = 100 << 16;
    scra_v_posit.w.h = 0;
    dodai_fal(platform);
    TEST_ASSERT_EQ_INT(ctx, 192, platform->yspeed.w);
    TEST_ASSERT_EQ_INT(ctx, (100 << 16) + (128 << 8), platform->yposi.l);

    reset_dai_rd1_state();
    dai_rd1_get_work(platform)->state = 2;
    platform->yspeed.w = 1024;
    platform->yposi.w.h = 400;
    scra_v_posit.w.h = 100;
    dodai_fal(platform);
    TEST_ASSERT_EQ_INT(ctx, 1, ride_on_clr_count);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);

    reset_dai_rd1_state();
    queue_ridechk_result(1);
    dodai_up(platform);
    TEST_ASSERT_EQ_INT(ctx, 2, dai_rd1_get_work(platform)->state);

    reset_dai_rd1_state();
    dai_rd1_get_work(platform)->state = 2;
    dai_rd1_get_work(platform)->phase = 64;
    platform->yposi.w.h = 123;
    dodai_up(platform);
    TEST_ASSERT_EQ_INT(ctx, 4, dai_rd1_get_work(platform)->state);
    TEST_ASSERT_EQ_INT(ctx, 123, dai_rd1_get_work(platform)->origin_y);
}

static void test_dodai_upx_and_horizontal_state_machines(test_context *ctx) {
    sprite_status *platform = &actwk[2];

    reset_dai_rd1_state();
    dai_rd1_get_work(platform)->state = 2;
    dai_rd1_get_work(platform)->wait_timer = 1;
    dodai_upx(platform);
    TEST_ASSERT_EQ_INT(ctx, 0, dai_rd1_get_work(platform)->wait_timer);

    reset_dai_rd1_state();
    dai_rd1_get_work(platform)->state = 2;
    emycol_u_result = -5;
    platform->yspeed.w = 80;
    platform->yposi.w.h = 200;
    dodai_upx(platform);
    TEST_ASSERT_EQ_INT(ctx, 1, speedset2_count);
    TEST_ASSERT_EQ_INT(ctx, 1, emycol_u_count);
    TEST_ASSERT_EQ_INT(ctx, 205, platform->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 4, dai_rd1_get_work(platform)->state);

    reset_dai_rd1_state();
    dai_rd1_get_work(platform)->origin_x = 100;
    dai_rd1_get_work(platform)->origin_y = 200;
    dai_rd1_get_work(platform)->state = 2;
    dai_rd1_get_work(platform)->phase = 2;
    dai_rd1_get_work(platform)->amplitude = 8;
    sinset_sin = 32;
    dodai_rm(platform);
    TEST_ASSERT_EQ_INT(ctx, 116, platform->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 3, dai_rd1_get_work(platform)->phase);

    reset_dai_rd1_state();
    dai_rd1_get_work(platform)->origin_x = 100;
    dai_rd1_get_work(platform)->origin_y = 200;
    dai_rd1_get_work(platform)->state = 2;
    dai_rd1_get_work(platform)->phase = 2;
    dai_rd1_get_work(platform)->amplitude = 8;
    sinset_sin = 32;
    dodai_lm(platform);
    TEST_ASSERT_EQ_INT(ctx, 84, platform->xposi.w.h);

    reset_dai_rd1_state();
    dai_rd1_get_work(platform)->state = 2;
    dai_rd1_get_work(platform)->phase = 64;
    platform->xposi.w.h = 222;
    dodai_rm(platform);
    TEST_ASSERT_EQ_INT(ctx, 4, dai_rd1_get_work(platform)->state);
    TEST_ASSERT_EQ_INT(ctx, 222, dai_rd1_get_work(platform)->origin_x);
}

static void test_dodai_state_machine_break_and_timer_edges(test_context *ctx) {
    sprite_status *platform = &actwk[2];

    reset_dai_rd1_state();
    queue_ridechk_result(0);
    dodai_fal(platform);
    TEST_ASSERT_EQ_INT(ctx, 0, dai_rd1_get_work(platform)->state);
    TEST_ASSERT_EQ_INT(ctx, 0, dai_rd1_get_work(platform)->wait_timer);

    reset_dai_rd1_state();
    queue_ridechk_result(0);
    dodai_up(platform);
    TEST_ASSERT_EQ_INT(ctx, 0, dai_rd1_get_work(platform)->state);

    reset_dai_rd1_state();
    queue_ridechk_result(0);
    dodai_upx(platform);
    TEST_ASSERT_EQ_INT(ctx, 0, dai_rd1_get_work(platform)->state);

    reset_dai_rd1_state();
    queue_ridechk_result(1);
    dodai_upx(platform);
    TEST_ASSERT_EQ_INT(ctx, 2, dai_rd1_get_work(platform)->state);
    TEST_ASSERT_EQ_INT(ctx, 59, dai_rd1_get_work(platform)->wait_timer);

    reset_dai_rd1_state();
    dai_rd1_get_work(platform)->state = 2;
    emycol_u_result = 3;
    dodai_upx(platform);
    TEST_ASSERT_EQ_INT(ctx, 1, speedset2_count);
    TEST_ASSERT_EQ_INT(ctx, 1, emycol_u_count);
    TEST_ASSERT_EQ_INT(ctx, 1, ridechk_count);
    TEST_ASSERT_EQ_INT(ctx, 2, dai_rd1_get_work(platform)->state);

    reset_dai_rd1_state();
    queue_ridechk_result(0);
    dodai_rm(platform);
    TEST_ASSERT_EQ_INT(ctx, 0, dai_rd1_get_work(platform)->state);

    reset_dai_rd1_state();
    queue_ridechk_result(1);
    dodai_rm(platform);
    TEST_ASSERT_EQ_INT(ctx, 2, dai_rd1_get_work(platform)->state);
    TEST_ASSERT_EQ_INT(ctx, 59, dai_rd1_get_work(platform)->wait_timer);

    reset_dai_rd1_state();
    dai_rd1_get_work(platform)->state = 2;
    dai_rd1_get_work(platform)->wait_timer = 1;
    dodai_rm(platform);
    TEST_ASSERT_EQ_INT(ctx, 0, dai_rd1_get_work(platform)->wait_timer);

    reset_dai_rd1_state();
    queue_ridechk_result(0);
    dodai_lm(platform);
    TEST_ASSERT_EQ_INT(ctx, 0, dai_rd1_get_work(platform)->state);

    reset_dai_rd1_state();
    queue_ridechk_result(1);
    dodai_lm(platform);
    TEST_ASSERT_EQ_INT(ctx, 2, dai_rd1_get_work(platform)->state);
    TEST_ASSERT_EQ_INT(ctx, 59, dai_rd1_get_work(platform)->wait_timer);

    reset_dai_rd1_state();
    dai_rd1_get_work(platform)->state = 2;
    dai_rd1_get_work(platform)->wait_timer = 1;
    dodai_lm(platform);
    TEST_ASSERT_EQ_INT(ctx, 0, dai_rd1_get_work(platform)->wait_timer);

    reset_dai_rd1_state();
    dai_rd1_get_work(platform)->state = 2;
    dai_rd1_get_work(platform)->phase = 64;
    platform->xposi.w.h = 111;
    dodai_lm(platform);
    TEST_ASSERT_EQ_INT(ctx, 4, dai_rd1_get_work(platform)->state);
    TEST_ASSERT_EQ_INT(ctx, 111, dai_rd1_get_work(platform)->origin_x);
}

static void test_vfuta_initializes_triggers_and_resets(test_context *ctx) {
    sprite_status *door = &actwk[4];

    reset_dai_rd1_state();
    door->xposi.w.h = 120;
    door->yposi.w.h = 80;
    actwk[0].xposi.w.h = 100;
    actwk[0].yposi.w.h = 60;
    door->actflg = 128;

    vfuta(door);

    TEST_ASSERT_EQ_INT(ctx, 4, door->r_no0);
    TEST_ASSERT_TRUE(ctx, door->patbase == vfutapat);
    TEST_ASSERT_EQ_INT(ctx, 1, door->sprpri);
    TEST_ASSERT_EQ_INT(ctx, 132, door->actflg);
    TEST_ASSERT_EQ_INT(ctx, 4, door->sprhsize);
    TEST_ASSERT_EQ_INT(ctx, 24, door->sprvsize);
    TEST_ASSERT_EQ_INT(ctx, 1, door->patno);
    TEST_ASSERT_EQ_INT(ctx, 1, soundset_count);
    TEST_ASSERT_EQ_INT(ctx, 164, soundset_last);
    TEST_ASSERT_EQ_INT(ctx, 1, ride_on_chk_count);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_s_count);

    reset_dai_rd1_state();
    door->r_no0 = 2;
    door->xposi.w.h = 120;
    door->yposi.w.h = 40;
    actwk[0].xposi.w.h = 100;
    actwk[0].yposi.w.h = 60;
    vfuta(door);
    TEST_ASSERT_EQ_INT(ctx, 4, door->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, ride_on_chk_count);

    reset_dai_rd1_state();
    door->r_no0 = 4;
    dai_rd1_get_work(door)->vfuta_phase = 248;
    door->patno = 1;
    vfuta(door);
    TEST_ASSERT_EQ_INT(ctx, 2, door->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, door->patno);
}

TEST_MAIN_BEGIN;
    test_dodai_tables_capture_literal_contract(&ctx);
    test_dodai_init_sets_shape_and_spawns_child(&ctx);
    test_dodai_init_allocation_failure_clears_flag_and_frames_out(&ctx);
    test_dodai_time_stop_and_offscreen_frameout(&ctx);
    test_dodai_motion_types_and_ride_counter(&ctx);
    test_dodai_diagonal_and_fixed_motion_types(&ctx);
    test_dodai_fall_and_rise_state_machines(&ctx);
    test_dodai_upx_and_horizontal_state_machines(&ctx);
    test_dodai_state_machine_break_and_timer_edges(&ctx);
    test_vfuta_initializes_triggers_and_resets(&ctx);
TEST_MAIN_END
