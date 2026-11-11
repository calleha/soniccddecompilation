#include <string.h>

#include "support/test_runner.h"
#include "src/types.h"
#include "src/spring_work.h"

sprite_status actwk[128];
Uint8 plpower_m;
Uint8 plpower_a;

static int actionsub_count;
static sprite_status *actionsub_actor;
static int frameout_count;
static sprite_status *frameout_actor;
static int frameout_s00_count;
static sprite_status *frameout_s00_actor;
static Sint16 frameout_s00_xpos;
static int actwkchk_count;
static sprite_status *actwkchk_queue[8];
static int actwkchk_queue_count;
static int actwkchk_queue_index;
static int hitchk_count;
static sprite_status *hitchk_actor;
static sprite_status *hitchk_player;
static Sint16 hitchk_result;
static int playdamageset_count;
static sprite_status *playdamageset_player;
static sprite_status *playdamageset_actor;
static int ridechk_count;
static sprite_status *ridechk_actor;
static int ride_on_clr_count;
static sprite_status *ride_on_clr_actor;
static int ride_on_chk_count;
static sprite_status *ride_on_chk_actor;
static int soundset_count;
static Sint16 soundset_requests[8];

void actionsub(sprite_status *pActwk);
void frameout(sprite_status *pActwk);
Sint32 frameout_s00(sprite_status *pActwk, Sint16 xposi);
Sint32 actwkchk(sprite_status **ppActwk);
Sint16 hitchk(sprite_status *pActwk, sprite_status *pPlayerwk);
Sint16 playdamageset(sprite_status *pActwk, sprite_status *pColliAct);
Sint16 ridechk(sprite_status *pActwk, sprite_status *pPlayerwk);
Sint16 ride_on_clr(sprite_status *pActwk, sprite_status *pPlayerwk);
Sint16 ride_on_chk(sprite_status *pActwk, sprite_status *pPlayerwk);
void soundset(Sint16 ReqNo);

#include "src/r8/trap_r81.c"

void actionsub(sprite_status *pActwk) {
    ++actionsub_count;
    actionsub_actor = pActwk;
}

void frameout(sprite_status *pActwk) {
    ++frameout_count;
    frameout_actor = pActwk;
    pActwk->actno = 0;
}

Sint32 frameout_s00(sprite_status *pActwk, Sint16 xposi) {
    ++frameout_s00_count;
    frameout_s00_actor = pActwk;
    frameout_s00_xpos = xposi;
    return 0;
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

Sint16 hitchk(sprite_status *pActwk, sprite_status *pPlayerwk) {
    ++hitchk_count;
    hitchk_actor = pActwk;
    hitchk_player = pPlayerwk;
    return hitchk_result;
}

Sint16 playdamageset(sprite_status *pActwk, sprite_status *pColliAct) {
    ++playdamageset_count;
    playdamageset_player = pActwk;
    playdamageset_actor = pColliAct;
    return -1;
}

Sint16 ridechk(sprite_status *pActwk, sprite_status *pPlayerwk) {
    (void)pPlayerwk;
    ++ridechk_count;
    ridechk_actor = pActwk;
    return 0;
}

Sint16 ride_on_clr(sprite_status *pActwk, sprite_status *pPlayerwk) {
    (void)pPlayerwk;
    ++ride_on_clr_count;
    ride_on_clr_actor = pActwk;
    return 0;
}

Sint16 ride_on_chk(sprite_status *pActwk, sprite_status *pPlayerwk) {
    (void)pPlayerwk;
    ++ride_on_chk_count;
    ride_on_chk_actor = pActwk;
    return 0;
}

void soundset(Sint16 ReqNo) {
    if (soundset_count < (int)(sizeof(soundset_requests) / sizeof(soundset_requests[0]))) {
        soundset_requests[soundset_count] = ReqNo;
    }
    ++soundset_count;
}

static void queue_actwk(sprite_status *actor) {
    actwkchk_queue[actwkchk_queue_count++] = actor;
}

static void reset_trap_state(void) {
    memset(actwk, 0, sizeof(actwk));
    plpower_m = 0;
    plpower_a = 0;
    actionsub_count = 0;
    actionsub_actor = 0;
    frameout_count = 0;
    frameout_actor = 0;
    frameout_s00_count = 0;
    frameout_s00_actor = 0;
    frameout_s00_xpos = 0;
    actwkchk_count = 0;
    memset(actwkchk_queue, 0, sizeof(actwkchk_queue));
    actwkchk_queue_count = 0;
    actwkchk_queue_index = 0;
    hitchk_count = 0;
    hitchk_actor = 0;
    hitchk_player = 0;
    hitchk_result = 0;
    playdamageset_count = 0;
    playdamageset_player = 0;
    playdamageset_actor = 0;
    ridechk_count = 0;
    ridechk_actor = 0;
    ride_on_clr_count = 0;
    ride_on_clr_actor = 0;
    ride_on_chk_count = 0;
    ride_on_chk_actor = 0;
    soundset_count = 0;
    memset(soundset_requests, 0, sizeof(soundset_requests));
}

static void test_togedair8_initializes_platform_and_child(test_context *ctx) {
    sprite_status *platform = &actwk[2];
    sprite_status *child = &actwk[6];

    reset_trap_state();
    platform->actno = 54;
    platform->xposi.w.h = 320;
    platform->yposi.w.h = 96;
    queue_actwk(child);

    togedair8(platform);

    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_TRUE(ctx, actionsub_actor == platform);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_s00_count);
    TEST_ASSERT_TRUE(ctx, frameout_s00_actor == platform);
    TEST_ASSERT_EQ_INT(ctx, 320, frameout_s00_xpos);
    TEST_ASSERT_EQ_INT(ctx, 0, hitchk_count);
    /* The spawned child is a spring: it must be configured in spring_work. */
    TEST_ASSERT_EQ_INT(ctx, 10, child->actno);
    TEST_ASSERT_EQ_INT(ctx, -24, spring_work_get(child)->follow_y_offset);
    TEST_ASSERT_EQ_INT(ctx, 2, spring_work_get(child)->parent_index);
}

static void test_togedair8_userflag_one_spawns_harir8_child(test_context *ctx) {
    sprite_status *platform = &actwk[3];
    sprite_status *child = &actwk[7];

    reset_trap_state();
    platform->actno = 54;
    platform->userflag.b.h = 1;
    platform->xposi.w.h = 144;
    platform->yposi.w.h = 208;
    queue_actwk(child);

    togedair8(platform);

    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_s00_count);
    TEST_ASSERT_EQ_INT(ctx, 144, frameout_s00_xpos);
}

static void test_togedair8_delegates_negative_userflag_to_harir8(
    test_context *ctx) {
    sprite_status *spike = &actwk[5];

    reset_trap_state();
    spike->actno = 54;
    spike->userflag.b.h = -1;
    spike->xposi.w.h = 200;
    spike->yposi.w.h = 100;

    togedair8(spike);

    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_s00_count);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);
}

static void test_togedair8_frames_out_when_child_allocation_fails(
    test_context *ctx) {
    sprite_status *platform = &actwk[2];

    reset_trap_state();
    platform->actno = 54;
    platform->xposi.w.h = 320;
    platform->yposi.w.h = 96;

    togedair8(platform);

    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actor == platform);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_s00_count);
}

static void test_togedair8_active_platform_checks_near_player(
    test_context *ctx) {
    sprite_status *platform = &actwk[2];
    sprite_status *child = &actwk[6];
    sprite_status *player = &actwk[0];

    reset_trap_state();
    platform->actno = 54;
    platform->actflg = 128;
    platform->xposi.w.h = 320;
    platform->yposi.w.h = 96;
    player->yposi.w.h = 80;
    queue_actwk(child);

    togedair8(platform);

    TEST_ASSERT_EQ_INT(ctx, 1, hitchk_count);
    TEST_ASSERT_TRUE(ctx, hitchk_actor == platform);
    TEST_ASSERT_TRUE(ctx, hitchk_player == player);
}

static void test_togedair8_active_platform_skips_far_player(test_context *ctx) {
    sprite_status *platform = &actwk[2];
    sprite_status *player = &actwk[0];

    reset_trap_state();
    platform->actno = 54;
    platform->r_no0 = 2;
    platform->actflg = 128;
    platform->xposi.w.h = 320;
    platform->yposi.w.h = 96;
    player->yposi.w.h = 200;

    togedair8(platform);

    TEST_ASSERT_EQ_INT(ctx, 0, hitchk_count);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_s00_count);
}

static void test_harir8_damages_player_when_collision_is_active(test_context *ctx) {
    sprite_status *spike = &actwk[5];
    sprite_status *player = &actwk[0];
    Sint32 original_y;

    reset_trap_state();
    spike->actno = 54;
    spike->userflag.b.h = -1;
    spike->xposi.w.h = 200;
    spike->yposi.w.h = 100;
    spike->cddat = 8;
    player->yposi.w.h = 112;
    player->yspeed.w = 3;
    original_y = player->yposi.l;
    hitchk_result = 1;

    harir8(spike);

    TEST_ASSERT_EQ_INT(ctx, 1, hitchk_count);
    TEST_ASSERT_TRUE(ctx, hitchk_actor == spike);
    TEST_ASSERT_TRUE(ctx, hitchk_player == player);
    TEST_ASSERT_EQ_INT(ctx, 1, playdamageset_count);
    TEST_ASSERT_TRUE(ctx, playdamageset_player == player);
    TEST_ASSERT_TRUE(ctx, playdamageset_actor == spike);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_s00_count);
    TEST_ASSERT_EQ_INT(ctx, 200, frameout_s00_xpos);
}

static void test_harir8_skips_damage_when_player_is_invincible(test_context *ctx) {
    sprite_status *spike = &actwk[5];
    sprite_status *player = &actwk[0];
    Sint32 original_y;

    reset_trap_state();
    spike->actno = 54;
    spike->userflag.b.h = -1;
    spike->xposi.w.h = 200;
    spike->yposi.w.h = 100;
    spike->cddat = 8;
    player->yposi.w.h = 112;
    player->yspeed.w = 3;
    original_y = player->yposi.l;
    hitchk_result = 1;
    plpower_a = 1;

    harir8(spike);

    TEST_ASSERT_EQ_INT(ctx, 1, hitchk_count);
    TEST_ASSERT_EQ_INT(ctx, 0, playdamageset_count);
}

static void test_harir8_initializes_solid_spike_variant(test_context *ctx) {
    sprite_status *spike = &actwk[5];
    sprite_status *player = &actwk[0];

    reset_trap_state();
    spike->actno = 54;
    spike->actflg = 128;
    spike->userflag.b.h = 0;
    spike->xposi.w.h = 200;
    spike->yposi.w.h = 100;
    player->yposi.w.h = 90;

    harir8(spike);

    TEST_ASSERT_EQ_INT(ctx, 1, hitchk_count);
    TEST_ASSERT_TRUE(ctx, hitchk_actor == spike);
}

static void test_harir8_solid_spike_ignores_when_inactive(test_context *ctx) {
    sprite_status *spike = &actwk[5];
    sprite_status *player = &actwk[0];

    reset_trap_state();
    spike->actno = 54;
    spike->r_no0 = 2;
    spike->patno = 1;
    spike->xposi.w.h = 200;
    spike->yposi.w.h = 100;
    player->yposi.w.h = 90;

    harir8(spike);

    TEST_ASSERT_EQ_INT(ctx, 0, hitchk_count);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_s00_count);
}

static void test_harir8_solid_spike_ignores_far_player(test_context *ctx) {
    sprite_status *spike = &actwk[5];
    sprite_status *player = &actwk[0];

    reset_trap_state();
    spike->actno = 54;
    spike->r_no0 = 2;
    spike->actflg = 128;
    spike->patno = 1;
    spike->xposi.w.h = 200;
    spike->yposi.w.h = 100;
    player->yposi.w.h = 200;

    harir8(spike);

    TEST_ASSERT_EQ_INT(ctx, 0, hitchk_count);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_s00_count);
}

static void test_harir8_master_child_follows_live_parent(test_context *ctx) {
    sprite_status *platform = &actwk[3];
    sprite_status *child = &actwk[7];

    reset_trap_state();
    platform->actno = 54;
    platform->userflag.b.h = 1;
    platform->xposi.w.h = 144;
    platform->yposi.w.h = 208;
    queue_actwk(child);
    togedair8(platform);
    actionsub_count = 0;
    frameout_s00_count = 0;
    platform->xposi.w.h = 150;
    platform->yposi.w.h = 70;
    trap_r81_work_get(child)->follow_x_offset = (Uint8)-6;

    harir8(child);

    TEST_ASSERT_EQ_INT(ctx, 144, frameout_s00_xpos);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_s00_count);
    TEST_ASSERT_EQ_INT(ctx, 0, frameout_count);
}

static void test_harir8_master_child_frames_out_when_parent_is_gone(
    test_context *ctx) {
    sprite_status *platform = &actwk[3];
    sprite_status *child = &actwk[7];

    reset_trap_state();
    platform->actno = 54;
    platform->userflag.b.h = 1;
    platform->xposi.w.h = 144;
    platform->yposi.w.h = 208;
    queue_actwk(child);
    togedair8(platform);
    actionsub_count = 0;
    frameout_s00_count = 0;
    platform->actno = 0;

    harir8(child);

    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actor == child);
    TEST_ASSERT_EQ_INT(ctx, 0, frameout_s00_count);
}

static void test_harir8_skips_when_player_is_too_far(test_context *ctx) {
    sprite_status *spike = &actwk[5];
    sprite_status *player = &actwk[0];

    reset_trap_state();
    spike->actno = 54;
    spike->r_no0 = 2;
    spike->xposi.w.h = 200;
    spike->yposi.w.h = 100;
    player->yposi.w.h = 200;

    harir8(spike);

    TEST_ASSERT_EQ_INT(ctx, 0, hitchk_count);
    TEST_ASSERT_EQ_INT(ctx, 0, playdamageset_count);
}

static void test_harir8_skips_when_hitchk_misses(test_context *ctx) {
    sprite_status *spike = &actwk[5];
    sprite_status *player = &actwk[0];

    reset_trap_state();
    spike->actno = 54;
    spike->r_no0 = 2;
    spike->xposi.w.h = 200;
    spike->yposi.w.h = 100;
    player->yposi.w.h = 90;
    hitchk_result = 0;

    harir8(spike);

    TEST_ASSERT_EQ_INT(ctx, 1, hitchk_count);
    TEST_ASSERT_EQ_INT(ctx, 0, playdamageset_count);
}

static void test_harir8_skips_when_collision_flag_is_clear(test_context *ctx) {
    sprite_status *spike = &actwk[5];
    sprite_status *player = &actwk[0];

    reset_trap_state();
    spike->actno = 54;
    spike->r_no0 = 2;
    spike->xposi.w.h = 200;
    spike->yposi.w.h = 100;
    player->yposi.w.h = 90;
    hitchk_result = 1;

    harir8(spike);

    TEST_ASSERT_EQ_INT(ctx, 1, hitchk_count);
    TEST_ASSERT_EQ_INT(ctx, 0, playdamageset_count);
}

static void test_harir8_skips_when_player_action_blocks_damage(
    test_context *ctx) {
    sprite_status *spike = &actwk[5];
    sprite_status *player = &actwk[0];

    reset_trap_state();
    spike->actno = 54;
    spike->r_no0 = 2;
    spike->xposi.w.h = 200;
    spike->yposi.w.h = 100;
    spike->cddat = 8;
    player->yposi.w.h = 90;
    player->r_no0 = 4;
    hitchk_result = 1;

    harir8(spike);

    TEST_ASSERT_EQ_INT(ctx, 1, hitchk_count);
    TEST_ASSERT_EQ_INT(ctx, 0, playdamageset_count);
}

static void test_harir8_skips_when_player_hidden_flag_blocks_damage(
    test_context *ctx) {
    sprite_status *spike = &actwk[5];
    sprite_status *player = &actwk[0];

    reset_trap_state();
    spike->actno = 54;
    spike->r_no0 = 2;
    spike->xposi.w.h = 200;
    spike->yposi.w.h = 100;
    spike->cddat = 8;
    player->yposi.w.h = 90;
    player_work_get(player)->damage_invulnerability_timer = 1;
    hitchk_result = 1;

    harir8(spike);

    TEST_ASSERT_EQ_INT(ctx, 1, hitchk_count);
    TEST_ASSERT_EQ_INT(ctx, 0, playdamageset_count);
}

static void test_anar8_initializes_left_gate_and_partner(test_context *ctx) {
    sprite_status *gate = &actwk[4];
    sprite_status *partner = &actwk[8];

    reset_trap_state();
    gate->actno = 55;
    gate->actflg = 128;
    gate->xposi.w.h = 100;
    gate->yposi.w.h = 64;
    queue_actwk(partner);

    anar8(gate);

    TEST_ASSERT_EQ_INT(ctx, 1, soundset_count);
    TEST_ASSERT_EQ_INT(ctx, 164, soundset_requests[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, ridechk_count);
    TEST_ASSERT_TRUE(ctx, ridechk_actor == gate);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_s00_count);
    TEST_ASSERT_EQ_INT(ctx, 96, frameout_s00_xpos);
}

static void test_anar8_opening_completes_into_wait_state(test_context *ctx) {
    sprite_status *gate = &actwk[4];

    reset_trap_state();
    gate->actno = 55;
    gate->r_no0 = 2;
    gate->actflg = 128;
    gate->xposi.w.h = 100;
    trap_r81_work_get(gate)->animation_index = 7;

    anar8(gate);

    TEST_ASSERT_EQ_INT(ctx, 1, soundset_count);
    TEST_ASSERT_EQ_INT(ctx, 1, ridechk_count);
    TEST_ASSERT_EQ_INT(ctx, 0, ride_on_clr_count);
}

static void test_anar8_closing_completes_back_to_opening_state(test_context *ctx) {
    sprite_status *gate = &actwk[4];

    reset_trap_state();
    gate->actno = 55;
    gate->r_no0 = 4;
    gate->actflg = 128;
    gate->xposi.w.h = 100;
    trap_r81_work_get(gate)->animation_index = 7;

    anar8(gate);

    TEST_ASSERT_EQ_INT(ctx, 1, soundset_count);
    TEST_ASSERT_EQ_INT(ctx, 0, ridechk_count);
    TEST_ASSERT_EQ_INT(ctx, 1, ride_on_clr_count);
    TEST_ASSERT_TRUE(ctx, ride_on_clr_actor == gate);
}

static void test_anar8_right_gate_initializes_without_partner(test_context *ctx) {
    sprite_status *gate = &actwk[4];

    reset_trap_state();
    gate->actno = 55;
    gate->actflg = 128;
    gate->xposi.w.h = 224;
    gate->yposi.w.h = 64;
    trap_r81_work_get(gate)->paired_gate = 1;

    anar8(gate);

    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 1, ridechk_count);
    TEST_ASSERT_EQ_INT(ctx, 1, soundset_count);
}

static void test_anar8_frames_out_when_partner_allocation_fails(
    test_context *ctx) {
    sprite_status *gate = &actwk[4];

    reset_trap_state();
    gate->actno = 55;
    gate->xposi.w.h = 100;
    gate->yposi.w.h = 64;

    anar8(gate);

    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actor == gate);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_s00_count);
}

static void test_anar8_open_wait_timer_clears_ride_without_moving(
    test_context *ctx) {
    sprite_status *gate = &actwk[4];

    reset_trap_state();
    gate->actno = 55;
    gate->r_no0 = 2;
    gate->xposi.w.h = 100;
    trap_r81_work_get(gate)->timer = 2;
    trap_r81_work_get(gate)->animation_index = 3;

    anar8(gate);

    TEST_ASSERT_EQ_INT(ctx, 1, ride_on_clr_count);
    TEST_ASSERT_EQ_INT(ctx, 0, ridechk_count);
}

static void test_anar8_right_gate_closing_progresses_without_finishing(
    test_context *ctx) {
    sprite_status *gate = &actwk[4];

    reset_trap_state();
    gate->actno = 55;
    gate->r_no0 = 4;
    gate->actflg = 128;
    gate->xposi.w.h = 220;
    trap_r81_work_get(gate)->paired_gate = 1;
    trap_r81_work_get(gate)->animation_index = 2;

    anar8(gate);

    TEST_ASSERT_EQ_INT(ctx, 1, ridechk_count);
    TEST_ASSERT_EQ_INT(ctx, 0, ride_on_clr_count);
    TEST_ASSERT_EQ_INT(ctx, 1, soundset_count);
}

static void test_anar8_close_wait_timer_checks_ride_without_moving(
    test_context *ctx) {
    sprite_status *gate = &actwk[4];

    reset_trap_state();
    gate->actno = 55;
    gate->r_no0 = 4;
    gate->xposi.w.h = 100;
    trap_r81_work_get(gate)->timer = 2;
    trap_r81_work_get(gate)->animation_index = 3;

    anar8(gate);

    TEST_ASSERT_EQ_INT(ctx, 1, ridechk_count);
    TEST_ASSERT_EQ_INT(ctx, 0, ride_on_clr_count);
}

static void test_futagor8_initializes_first_platform_shape(test_context *ctx) {
    sprite_status *platform = &actwk[9];

    reset_trap_state();
    platform->actno = 56;
    platform->xposi.w.h = 512;
    platform->yposi.w.h = 128;

    futagor8(platform);

    TEST_ASSERT_EQ_INT(ctx, 1, ride_on_chk_count);
    TEST_ASSERT_TRUE(ctx, ride_on_chk_actor == platform);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_s00_count);
    TEST_ASSERT_EQ_INT(ctx, 512, frameout_s00_xpos);
}

static void test_futagor8_wraps_animation_index_after_last_shape(
    test_context *ctx) {
    sprite_status *platform = &actwk[9];

    reset_trap_state();
    platform->actno = 56;
    platform->r_no0 = 2;
    platform->patno = 6;
    trap_r81_work_get(platform)->timer = 1;
    trap_r81_work_get(platform)->pattern_index = 11;

    futagor8(platform);

    TEST_ASSERT_EQ_INT(ctx, 1, ride_on_chk_count);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
}

TEST_MAIN_BEGIN;
    test_togedair8_initializes_platform_and_child(&ctx);
    test_togedair8_userflag_one_spawns_harir8_child(&ctx);
    test_togedair8_delegates_negative_userflag_to_harir8(&ctx);
    test_togedair8_frames_out_when_child_allocation_fails(&ctx);
    test_togedair8_active_platform_checks_near_player(&ctx);
    test_togedair8_active_platform_skips_far_player(&ctx);
    test_harir8_damages_player_when_collision_is_active(&ctx);
    test_harir8_skips_damage_when_player_is_invincible(&ctx);
    test_harir8_initializes_solid_spike_variant(&ctx);
    test_harir8_solid_spike_ignores_when_inactive(&ctx);
    test_harir8_solid_spike_ignores_far_player(&ctx);
    test_harir8_master_child_follows_live_parent(&ctx);
    test_harir8_master_child_frames_out_when_parent_is_gone(&ctx);
    test_harir8_skips_when_player_is_too_far(&ctx);
    test_harir8_skips_when_hitchk_misses(&ctx);
    test_harir8_skips_when_collision_flag_is_clear(&ctx);
    test_harir8_skips_when_player_action_blocks_damage(&ctx);
    test_harir8_skips_when_player_hidden_flag_blocks_damage(&ctx);
    test_anar8_initializes_left_gate_and_partner(&ctx);
    test_anar8_opening_completes_into_wait_state(&ctx);
    test_anar8_closing_completes_back_to_opening_state(&ctx);
    test_anar8_right_gate_initializes_without_partner(&ctx);
    test_anar8_frames_out_when_partner_allocation_fails(&ctx);
    test_anar8_open_wait_timer_clears_ride_without_moving(&ctx);
    test_anar8_right_gate_closing_progresses_without_finishing(&ctx);
    test_anar8_close_wait_timer_checks_ride_without_moving(&ctx);
    test_futagor8_initializes_first_platform_shape(&ctx);
    test_futagor8_wraps_animation_index_after_last_shape(&ctx);
TEST_MAIN_END

