#include <stddef.h>
#include <string.h>

#include "support/test_runner.h"
#include "src/types.h"
#include "src/spring.h"

sprite_status actwk[128];
Uint8 flagwork[766];
Uint8 plpower_a;
Uint8 plpower_m;
Uint8 time_flag;
Uint8 generate_flag;
int_union scra_h_posit;
short_union gametimer;

static int actionsub_count;
static sprite_status *actionsub_actor;
static int frameout_count;
static sprite_status *frameout_actor;
static int frameout_s_count;
static sprite_status *frameout_s_actor;
static int actwkchk_count;
static int actwkchk2_count;
static int next_alloc_index;
static int actwkchk_fail_after;
static int hitchk_count;
static Sint16 hitchk_result;
static sprite_status *hitchk_actor;
static sprite_status *hitchk_player;
static int playdamageset_count;
static sprite_status *playdamageset_player;
static sprite_status *playdamageset_actor;
static int ridechk_count;
static Sint16 ridechk_result;
static sprite_status *ridechk_actor;
static int ride_on_chk_count;
static Sint16 ride_on_chk_result;
static sprite_status *ride_on_chk_actor;
static int ride_on_clr_count;
static int patchg_count;
static sprite_status *patchg_actor;
static Uint8 **patchg_table;
static int soundset_count;
static Sint16 soundset_request;
static int sinset_count;
static Uint8 sinset_angle;
static Sint16 sinset_sin;
static Sint16 sinset_cos;

void actionsub(sprite_status *pActwk);
void frameout(sprite_status *pActwk);
Sint32 frameout_s(sprite_status *pActwk);
Sint32 actwkchk(sprite_status **ppActwk);
Sint32 actwkchk2(sprite_status *pActwk, sprite_status **ppNewActwk);
Sint16 hitchk(sprite_status *pActwk, sprite_status *pPlayerwk);
Sint16 playdamageset(sprite_status *pActwk, sprite_status *pColliAct);
Sint16 ridechk(sprite_status *pActwk, sprite_status *pPlayerwk);
Sint16 ride_on_chk(sprite_status *pActwk, sprite_status *pPlayerwk);
Sint16 ride_on_clr(sprite_status *pActwk, sprite_status *pPlayerwk);
void patchg(sprite_status *patchgwk, Uint8 **pat_dat);
void soundset(Sint16 ReqNo);
void sinset(Uint8 kakudo, Sint16 *sin, Sint16 *cos);

#include "src/r3/trap_r3.c"

void actionsub(sprite_status *pActwk) {
    ++actionsub_count;
    actionsub_actor = pActwk;
}

void frameout(sprite_status *pActwk) {
    ++frameout_count;
    frameout_actor = pActwk;
}

Sint32 frameout_s(sprite_status *pActwk) {
    ++frameout_s_count;
    frameout_s_actor = pActwk;
    return 0;
}

Sint32 actwkchk(sprite_status **ppActwk) {
    ++actwkchk_count;
    if (actwkchk_fail_after >= 0 && actwkchk_count > actwkchk_fail_after) {
        *ppActwk = 0;
        return 1;
    }
    *ppActwk = &actwk[next_alloc_index++];
    return 0;
}

Sint32 actwkchk2(sprite_status *pActwk, sprite_status **ppNewActwk) {
    (void)pActwk;
    ++actwkchk2_count;
    if (actwkchk_fail_after >= 0 && actwkchk2_count > actwkchk_fail_after) {
        *ppNewActwk = 0;
        return 1;
    }
    *ppNewActwk = &actwk[next_alloc_index++];
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
    return ridechk_result;
}

Sint16 ride_on_chk(sprite_status *pActwk, sprite_status *pPlayerwk) {
    (void)pPlayerwk;
    ++ride_on_chk_count;
    ride_on_chk_actor = pActwk;
    return ride_on_chk_result;
}

Sint16 ride_on_clr(sprite_status *pActwk, sprite_status *pPlayerwk) {
    (void)pActwk;
    (void)pPlayerwk;
    ++ride_on_clr_count;
    return 0;
}

void patchg(sprite_status *patchgwk, Uint8 **pat_dat) {
    ++patchg_count;
    patchg_actor = patchgwk;
    patchg_table = pat_dat;
}

void soundset(Sint16 ReqNo) {
    ++soundset_count;
    soundset_request = ReqNo;
}

void sinset(Uint8 kakudo, Sint16 *sin, Sint16 *cos) {
    ++sinset_count;
    sinset_angle = kakudo;
    *sin = sinset_sin;
    *cos = sinset_cos;
}

static void reset_state(void) {
    memset(actwk, 0, sizeof(actwk));
    memset(flagwork, 0, sizeof(flagwork));
    plpower_a = 0;
    plpower_m = 0;
    time_flag = 1;
    generate_flag = 0;
    scra_h_posit.l = 0;
    gametimer.w = 0;

    actionsub_count = 0;
    actionsub_actor = 0;
    frameout_count = 0;
    frameout_actor = 0;
    frameout_s_count = 0;
    frameout_s_actor = 0;
    actwkchk_count = 0;
    actwkchk2_count = 0;
    next_alloc_index = 10;
    actwkchk_fail_after = -1;
    hitchk_count = 0;
    hitchk_result = 0;
    hitchk_actor = 0;
    hitchk_player = 0;
    playdamageset_count = 0;
    playdamageset_player = 0;
    playdamageset_actor = 0;
    ridechk_count = 0;
    ridechk_result = 0;
    ridechk_actor = 0;
    ride_on_chk_count = 0;
    ride_on_chk_result = 0;
    ride_on_chk_actor = 0;
    ride_on_clr_count = 0;
    patchg_count = 0;
    patchg_actor = 0;
    patchg_table = 0;
    soundset_count = 0;
    soundset_request = 0;
    sinset_count = 0;
    sinset_angle = 0;
    sinset_sin = 128;
    sinset_cos = -64;
}

static void test_harir3_initializes_damages_and_follows_parent(test_context *ctx) {
    sprite_status *hari = &actwk[5];
    sprite_status *player = &actwk[0];

    reset_state();
    hari->xposi.w.h = 300;
    hari->yposi.w.h = 120;
    hari->cddat = 8;
    player->yposi.l = 0x00100000;
    player->yspeed.w = 5;
    hitchk_result = 1;

    harir3(hari);

    TEST_ASSERT_EQ_INT(ctx, 2, hari->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 4, hari->actflg);
    TEST_ASSERT_TRUE(ctx, hari->patbase == harir3pat);
    TEST_ASSERT_EQ_INT(ctx, 300, trap3_get_work(hari)->origin_x);
    TEST_ASSERT_EQ_INT(ctx, 120, trap3_get_work(hari)->base_x);
    TEST_ASSERT_EQ_INT(ctx, 1, playdamageset_count);
    TEST_ASSERT_TRUE(ctx, playdamageset_player == player);
    TEST_ASSERT_TRUE(ctx, playdamageset_actor == hari);
    TEST_ASSERT_EQ_INT(ctx, 0x00100000 - (5 << 8), player->yposi.l);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_TRUE(ctx, actionsub_actor == hari);

    reset_state();
    hari = &actwk[5];
    player = &actwk[0];
    hari->xposi.w.h = 300;
    hari->yposi.w.h = 120;
    hari->userflag.b.h = 1;
    hari->cddat = 8;
    hitchk_result = 1;
    harir3_init(hari);
    TEST_ASSERT_EQ_INT(ctx, 1, hari->patno);
    TEST_ASSERT_EQ_INT(ctx, 0, playdamageset_count);
    TEST_ASSERT_EQ_INT(ctx, 1, hitchk_count);

    reset_state();
    hari = &actwk[5];
    hari->r_no0 = 2;
    trap3_get_work(hari)->wait_timer = 3;
    trap3_get_work(hari)->phase = 4;
    trap3_get_work(hari)->linked_actor_index = 9;
    trap3_get_work(hari)->origin_x = 320;
    actwk[9].xposi.w.h = 1000;
    actwk[9].yposi.w.h = 2000;
    harir3(hari);
    TEST_ASSERT_EQ_INT(ctx, 1003, hari->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 2004, hari->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, frameout_count);

    trap3_get_work(hari)->origin_x = 2048;
    harir3(hari);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actor == hari);
}

static void test_harir3_motion_and_guard_branches(test_context *ctx) {
    sprite_status *hari = &actwk[2];

    reset_state();
    trap3_get_work(hari)->base_x = 100;
    trap3_get_work(hari)->origin_x = 200;
    hari->userflag.b.h = 4;
    harir3_kind(hari);
    TEST_ASSERT_EQ_INT(ctx, 8, trap3_get_work(hari)->travel);
    TEST_ASSERT_EQ_INT(ctx, 108, hari->yposi.w.h);

    trap3_get_work(hari)->timer = 1;
    hari->actflg = 128;
    harir3_sub(hari);
    TEST_ASSERT_EQ_INT(ctx, 1, soundset_count);
    TEST_ASSERT_EQ_INT(ctx, 183, soundset_request);

    trap3_get_work(hari)->travel = 4;
    trap3_get_work(hari)->reverse_flag = 1;
    harir3_sub(hari);
    TEST_ASSERT_EQ_INT(ctx, 0, trap3_get_work(hari)->travel);
    TEST_ASSERT_EQ_INT(ctx, 0, trap3_get_work(hari)->reverse_flag);
    TEST_ASSERT_EQ_INT(ctx, 60, trap3_get_work(hari)->timer);

    reset_state();
    hari = &actwk[2];
    hari->cddat = 8;
    hitchk_result = 1;
    plpower_a = 1;
    harir3_move(hari);
    TEST_ASSERT_EQ_INT(ctx, 0, playdamageset_count);
    plpower_a = 0;
    plpower_m = 1;
    harir3_move(hari);
    TEST_ASSERT_EQ_INT(ctx, 0, playdamageset_count);
    plpower_m = 0;
    actwk[0].r_no0 = 4;
    harir3_move(hari);
    TEST_ASSERT_EQ_INT(ctx, 0, playdamageset_count);
    actwk[0].r_no0 = 0;
    player_work_get(&actwk[0])->damage_invulnerability_timer = 1;
    harir3_move(hari);
    TEST_ASSERT_EQ_INT(ctx, 0, playdamageset_count);
}

static void test_frdr3_spawns_linked_actor_and_moves(test_context *ctx) {
    sprite_status *floor = &actwk[3];
    sprite_status *child;

    reset_state();
    floor->xposi.w.h = 400;
    floor->yposi.w.h = 96;
    floor->userflag.b.h = 2;
    frdr3_init(floor);
    child = &actwk[10];
    TEST_ASSERT_EQ_INT(ctx, 2, floor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 10, child->actno);
    TEST_ASSERT_EQ_INT(ctx, 400, child->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 96, child->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, -22, spring_work_get(child)->follow_y_offset);
    TEST_ASSERT_EQ_INT(ctx, 3, trap3_get_work(child)->linked_actor_index);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk2_count);

    reset_state();
    floor = &actwk[3];
    floor->userflag.b.h = 2;
    actwkchk_fail_after = 0;
    frdr3_init(floor);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actor == floor);

    reset_state();
    floor = &actwk[3];
    floor->userflag.b.h = 0;
    trap3_get_work(floor)->timer = 0;
    frdr3_move(floor);
    TEST_ASSERT_EQ_INT(ctx, 64, trap3_get_work(floor)->timer);
    TEST_ASSERT_EQ_INT(ctx, -8, trap3_get_work(floor)->delta);
    TEST_ASSERT_EQ_INT(ctx, 512, trap3_get_work(floor)->origin_x);

    floor->xposi.l = 0;
    trap3_get_work(floor)->timer = 2;
    trap3_get_work(floor)->travel = 0;
    trap3_get_work(floor)->origin_x = 512;
    trap3_get_work(floor)->delta = -8;
    frdr3_move(floor);
    TEST_ASSERT_EQ_INT(ctx, 504, floor->xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 504 << 8, floor->xposi.l);
    TEST_ASSERT_EQ_INT(ctx, 1, trap3_get_work(floor)->timer);

    time_flag = 0;
    floor->xposi.l = 0;
    frdr3_move(floor);
    TEST_ASSERT_EQ_INT(ctx, 0, floor->xposi.l);
}

static void test_trapdr3_children_and_vertical_response(test_context *ctx) {
    sprite_status *floor = &actwk[4];

    reset_state();
    floor->xposi.w.h = 160;
    floor->yposi.w.h = 80;
    floor->userflag.b.h = 1;
    trapdr3_init(floor);
    TEST_ASSERT_EQ_INT(ctx, 32, actwk[10].actno);
    TEST_ASSERT_EQ_INT(ctx, 32, trap3_get_work(&actwk[10])->wait_timer);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[10].userflag.b.h);
    TEST_ASSERT_EQ_INT(ctx, 4, trap3_get_work(&actwk[10])->linked_actor_index);

    reset_state();
    floor = &actwk[4];
    floor->xposi.w.h = 160;
    floor->yposi.w.h = 80;
    floor->userflag.b.h = 2;
    trapdr3_init(floor);
    TEST_ASSERT_EQ_INT(ctx, 10, actwk[10].actno);
    TEST_ASSERT_EQ_INT(ctx, 24, spring_work_get(&actwk[10])->follow_x_offset);
    TEST_ASSERT_EQ_INT(ctx, 4, actwk[10].userflag.b.h);

    reset_state();
    floor = &actwk[4];
    floor->xposi.w.h = 100;
    floor->sprhsize = 16;
    trap3_get_work(floor)->base_x = 70;
    actwk[0].xposi.w.h = 50;
    trapdr3_kind(floor);
    TEST_ASSERT_EQ_INT(ctx, 8, trap3_get_work(floor)->travel);
    TEST_ASSERT_EQ_INT(ctx, 78, floor->yposi.w.h);
    actwk[0].xposi.w.h = 200;
    trapdr3_kind(floor);
    TEST_ASSERT_EQ_INT(ctx, 0, trap3_get_work(floor)->travel);

    floor->userflag.b.h = 4;
    actwk[0].xposi.w.h = 50;
    trapdr3_kind(floor);
    TEST_ASSERT_EQ_INT(ctx, 62, floor->yposi.w.h);
}

static void test_for3_motion_modes_and_swing(test_context *ctx) {
    sprite_status *floor = &actwk[6];

    reset_state();
    floor->xposi.w.h = 300;
    floor->yposi.w.h = 120;
    for3_init(floor);
    TEST_ASSERT_EQ_INT(ctx, 2, floor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 32, floor->sprhsize);
    TEST_ASSERT_EQ_INT(ctx, 1, trap3_get_work(floor)->bob_enabled);
    TEST_ASSERT_EQ_INT(ctx, 1, ridechk_count);

    floor->cddat = 8;
    trap3_get_work(floor)->bob_angle = 0;
    sinset_sin = 128;
    dai3sub(floor);
    TEST_ASSERT_EQ_INT(ctx, 8, trap3_get_work(floor)->bob_angle);
    TEST_ASSERT_EQ_INT(ctx, 8, sinset_angle);
    TEST_ASSERT_EQ_INT(ctx, 122, floor->yposi.w.h);

    reset_state();
    floor = &actwk[6];
    floor->userflag.b.h = 1;
    for3_rmv(floor);
    TEST_ASSERT_EQ_INT(ctx, 28, trap3_get_work(floor)->wait_timer);
    TEST_ASSERT_EQ_INT(ctx, 768, trap3_get_work(floor)->velocity);
    floor->xposi.l = 0;
    trap3_get_work(floor)->wait_timer = 1;
    trap3_get_work(floor)->velocity = 768;
    trap3_get_work(floor)->acceleration = 0;
    for3_rmv(floor);
    TEST_ASSERT_EQ_INT(ctx, 768 << 8, floor->xposi.l);
    TEST_ASSERT_EQ_INT(ctx, 1, trap3_get_work(floor)->phase);

    reset_state();
    floor = &actwk[6];
    floor->userflag.b.h = 3;
    for3_dmv(floor);
    TEST_ASSERT_EQ_INT(ctx, 35, trap3_get_work(floor)->wait_timer);
    TEST_ASSERT_EQ_INT(ctx, -512, trap3_get_work(floor)->velocity);
    floor->yposi.l = 0;
    trap3_get_work(floor)->wait_timer = 1;
    trap3_get_work(floor)->velocity = -512;
    trap3_get_work(floor)->acceleration = 0;
    for3_dmv(floor);
    TEST_ASSERT_EQ_INT(ctx, -512 << 8, floor->yposi.l);
    TEST_ASSERT_EQ_INT(ctx, -512, floor->yspeed.w);

    reset_state();
    floor = &actwk[6];
    floor->userflag.b.h = 5;
    ridechk_result = 1;
    for3_rup1(floor);
    TEST_ASSERT_EQ_INT(ctx, 30, trap3_get_work(floor)->wait_timer);
    trap3_get_work(floor)->wait_timer = 1;
    for3_rup1(floor);
    TEST_ASSERT_EQ_INT(ctx, 2, trap3_get_work(floor)->timer);
    trap3_get_work(floor)->wait_timer = 0;
    for3_rup2(floor);
    TEST_ASSERT_EQ_INT(ctx, 16, trap3_get_work(floor)->wait_timer);
    TEST_ASSERT_EQ_INT(ctx, -16, trap3_get_work(floor)->origin_x);
}

static void test_getdair3_extends_retracts_and_frames_out(test_context *ctx) {
    sprite_status *floor = &actwk[7];

    reset_state();
    floor->xposi.w.h = 500;
    floor->yposi.w.h = 150;
    floor->userflag.b.h = 1;
    actwk[0].yposi.w.h = 200;
    getdair3_init(floor);
    TEST_ASSERT_EQ_INT(ctx, 2, floor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 30, trap3_get_work(floor)->timer);
    TEST_ASSERT_EQ_INT(ctx, 16, trap3_get_work(floor)->travel);
    TEST_ASSERT_EQ_INT(ctx, 484, floor->xposi.w.h);

    reset_state();
    floor = &actwk[7];
    floor->xposi.w.h = 500;
    floor->yposi.w.h = 150;
    trap3_get_work(floor)->origin_x = 500;
    floor->userflag.b.h = -1;
    actwk[0].yposi.w.h = 200;
    getdair3_move(floor);
    TEST_ASSERT_EQ_INT(ctx, 516, floor->xposi.w.h);

    floor->userflag.b.h = 0;
    trap3_get_work(floor)->travel = 8;
    trap3_get_work(floor)->reverse_flag = 0;
    actwk[0].yposi.w.h = 100;
    getdair3_move_sub(floor);
    TEST_ASSERT_EQ_INT(ctx, 0, trap3_get_work(floor)->travel);
    TEST_ASSERT_EQ_INT(ctx, 60, trap3_get_work(floor)->timer);
    TEST_ASSERT_EQ_INT(ctx, 255, trap3_get_work(floor)->reverse_flag);

    trap3_get_work(floor)->travel = 32;
    actwk[0].yposi.w.h = 100;
    getdair3_move_sub(floor);
    TEST_ASSERT_EQ_INT(ctx, 16, trap3_get_work(floor)->travel);
    floor->userflag.b.h = 1;
    trap3_get_work(floor)->travel = 8;
    getdair3_move_sub(floor);
    TEST_ASSERT_EQ_INT(ctx, 0, trap3_get_work(floor)->travel);
    floor->userflag.b.h = 0;
    trap3_get_work(floor)->travel = 8;
    trap3_get_work(floor)->reverse_flag = 7;
    getdair3_move_sub(floor);
    TEST_ASSERT_EQ_INT(ctx, 7, trap3_get_work(floor)->reverse_flag);
    floor->userflag.b.h = 0;
    trap3_get_work(floor)->travel = 60;
    actwk[0].yposi.w.h = 200;
    getdair3_move_sub(floor);
    TEST_ASSERT_EQ_INT(ctx, 64, trap3_get_work(floor)->travel);
    TEST_ASSERT_EQ_INT(ctx, 8, trap3_get_work(floor)->timer);

    trap3_get_work(floor)->origin_x = 2048;
    floor->r_no0 = 2;
    getdair3(floor);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
}

static void test_gandair3_core_bullets_and_ride_effect(test_context *ctx) {
    sprite_status *floor = &actwk[8];
    sprite_status *core;
    sprite_status *bullet;

    reset_state();
    floor->xposi.w.h = 320;
    floor->yposi.w.h = 80;
    gandair3_daii(floor);
    core = &actwk[10];
    TEST_ASSERT_EQ_INT(ctx, 39, core->actno);
    TEST_ASSERT_EQ_INT(ctx, 1, core->userflag.b.h);
    TEST_ASSERT_EQ_INT(ctx, 88, core->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 2, floor->r_no0);

    reset_state();
    core = &actwk[9];
    core->userflag.b.h = 1;
    core->xposi.w.h = 300;
    core->yposi.w.h = 100;
    gandair3_core(core);
    TEST_ASSERT_EQ_INT(ctx, 2, core->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, core->patno);
    trap3_get_work(core)->base_x = 100;
    core->r_no0 = 2;
    trap3_get_work(core)->timer = 1;
    gandair3_core(core);
    TEST_ASSERT_EQ_INT(ctx, 4, core->r_no0);

    reset_state();
    core = &actwk[9];
    core->xposi.w.h = 300;
    core->yposi.w.h = 100;
    core->r_no0 = 4;
    trap3_get_work(core)->travel = 24;
    trap3_get_work(core)->base_x = 100;
    ride_on_chk_result = 0;
    gandair3_fire(core);
    TEST_ASSERT_EQ_INT(ctx, 32, trap3_get_work(core)->travel);
    TEST_ASSERT_EQ_INT(ctx, 68, core->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 2, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 39, actwk[10].actno);
    TEST_ASSERT_EQ_INT(ctx, 39, actwk[11].actno);
    TEST_ASSERT_EQ_INT(ctx, 6, core->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 8, trap3_get_work(core)->timer);

    reset_state();
    core = &actwk[9];
    core->xposi.w.h = 300;
    core->yposi.w.h = 100;
    trap3_get_work(core)->travel = 24;
    trap3_get_work(core)->base_x = 100;
    ride_on_chk_result = 1;
    gandair3_fire(core);
    TEST_ASSERT_EQ_INT(ctx, -1792, actwk[0].yspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[0].cddat & 2);
    TEST_ASSERT_EQ_INT(ctx, 16, actwk[0].mstno.b.h);

    reset_state();
    bullet = &actwk[12];
    bullet->userflag.b.h = 2;
    bullet->xposi.w.h = 100;
    gandair3_tama(bullet);
    TEST_ASSERT_EQ_INT(ctx, 2, bullet->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 152, bullet->colino);
    TEST_ASSERT_EQ_INT(ctx, -272, bullet->xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 2, patchg_count);
    TEST_ASSERT_TRUE(ctx, patchg_table == gandair3_pchg);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_s_count);

    trap3_get_work(bullet)->bullet_origin_x = 0;
    bullet->xposi.w.h = 100;
    trap3_get_work(bullet)->travel = 1;
    gandair3_tamm(bullet);
    TEST_ASSERT_EQ_INT(ctx, 0, bullet->colino);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
}

static void test_drumr3_and_tr3_fout(test_context *ctx) {
    sprite_status *drum = &actwk[13];

    reset_state();
    drum->xposi.w.h = 240;
    drum->yposi.w.h = 120;
    drumr3_init(drum);
    TEST_ASSERT_EQ_INT(ctx, 2, drum->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 24, drum->sprhsize);
    TEST_ASSERT_EQ_INT(ctx, 240, trap3_get_work(drum)->base_x);

    drumr3_cntset(drum);
    TEST_ASSERT_EQ_INT(ctx, 1, trap3_get_work(drum)->drum_speed_high);
    TEST_ASSERT_EQ_INT(ctx, 8, drum->sprvsize);
    TEST_ASSERT_EQ_INT(ctx, 0, drum->patno);
    TEST_ASSERT_EQ_INT(ctx, 4, trap3_get_work(drum)->timer);
    TEST_ASSERT_EQ_INT(ctx, 5, drum->sprpri);

    trap3_get_work(drum)->timer = 0;
    drumr3_move1(drum);
    TEST_ASSERT_EQ_INT(ctx, 1, ride_on_clr_count);
    TEST_ASSERT_EQ_INT(ctx, 4, trap3_get_work(drum)->timer);

    trap3_get_work(drum)->timer = 1;
    drum->yposi.l = 0;
    trap3_get_work(drum)->origin_x = 256;
    gametimer.w = 10;
    time_flag = 1;
    drumr3_move1(drum);
    TEST_ASSERT_EQ_INT(ctx, 256, drum->yspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 256 << 8, drum->yposi.l);

    drum->actflg = 128;
    drum->patno = 2;
    time_flag = 0;
    drumr3_ridechk(drum);
    TEST_ASSERT_EQ_INT(ctx, 8, drum->sprvsize);
    drum->patno = 4;
    drumr3_ridechk(drum);
    TEST_ASSERT_EQ_INT(ctx, 5, drum->sprvsize);

    reset_state();
    drum = &actwk[13];
    drum->cdsts = 2;
    time_flag = 1;
    flagwork[7] = 255;
    tr3_fout(drum);
    TEST_ASSERT_EQ_INT(ctx, 127, flagwork[7]);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actor == drum);
}

static void test_dispatchers_and_late_state_branches(test_context *ctx) {
    sprite_status *actor;

    reset_state();
    actor = &actwk[2];
    actor->cddat = 0;
    hitchk_result = 1;
    harir3_move(actor);
    TEST_ASSERT_EQ_INT(ctx, 0, playdamageset_count);

    reset_state();
    actor = &actwk[2];
    trap3_get_work(actor)->base_x = 100;
    trap3_get_work(actor)->origin_x = 200;
    harir3_ymv2(actor);
    TEST_ASSERT_EQ_INT(ctx, 92, actor->yposi.w.h);
    trap3_get_work(actor)->travel = 0;
    trap3_get_work(actor)->timer = 0;
    trap3_get_work(actor)->reverse_flag = 0;
    harir3_xmv1(actor);
    TEST_ASSERT_EQ_INT(ctx, 192, actor->xposi.w.h);
    trap3_get_work(actor)->travel = 0;
    trap3_get_work(actor)->timer = 0;
    trap3_get_work(actor)->reverse_flag = 0;
    harir3_xmv2(actor);
    TEST_ASSERT_EQ_INT(ctx, 208, actor->xposi.w.h);
    trap3_get_work(actor)->timer = 2;
    harir3_sub(actor);
    TEST_ASSERT_EQ_INT(ctx, 1, trap3_get_work(actor)->timer);
    trap3_get_work(actor)->timer = 0;
    trap3_get_work(actor)->reverse_flag = 1;
    trap3_get_work(actor)->travel = 16;
    harir3_sub(actor);
    TEST_ASSERT_EQ_INT(ctx, 8, trap3_get_work(actor)->travel);
    trap3_get_work(actor)->timer = 0;
    trap3_get_work(actor)->reverse_flag = 0;
    trap3_get_work(actor)->travel = 24;
    harir3_sub(actor);
    TEST_ASSERT_EQ_INT(ctx, 32, trap3_get_work(actor)->travel);
    TEST_ASSERT_EQ_INT(ctx, 1, trap3_get_work(actor)->reverse_flag);
    TEST_ASSERT_EQ_INT(ctx, 60, trap3_get_work(actor)->timer);

    reset_state();
    actor = &actwk[3];
    actor->userflag.b.h = 1;
    frdr3_init(actor);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk2_count);
    TEST_ASSERT_EQ_INT(ctx, 1, ride_on_chk_count);
    trap3_get_work(actor)->timer = 1;
    frdr3_move(actor);
    TEST_ASSERT_EQ_INT(ctx, 0, actor->xposi.l);
    actor->userflag.b.h = 0;
    trap3_get_work(actor)->timer = 1;
    trap3_get_work(actor)->travel = 0;
    trap3_get_work(actor)->origin_x = 512;
    trap3_get_work(actor)->delta = -8;
    frdr3_move(actor);
    TEST_ASSERT_EQ_INT(ctx, 1, trap3_get_work(actor)->travel);

    reset_state();
    actor = &actwk[3];
    actor->r_no0 = 2;
    trap3_get_work(actor)->base_x = 2048;
    frdr3(actor);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);

    reset_state();
    actor = &actwk[4];
    actor->userflag.b.h = 0;
    trapdr3_init(actor);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk2_count);
    TEST_ASSERT_EQ_INT(ctx, 1, hitchk_count);
    actor->userflag.b.h = 1;
    actwkchk_fail_after = 0;
    trapdr3_init(actor);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk2_count);
    TEST_ASSERT_EQ_INT(ctx, 2, hitchk_count);

    reset_state();
    actor = &actwk[4];
    actor->xposi.w.h = 100;
    actor->sprhsize = 16;
    trap3_get_work(actor)->travel = 4;
    actwk[0].xposi.w.h = 200;
    trapdr3_updown(actor);
    TEST_ASSERT_EQ_INT(ctx, 0, trap3_get_work(actor)->travel);
    trap3_get_work(actor)->travel = 31;
    actwk[0].xposi.w.h = 50;
    trapdr3_updown(actor);
    TEST_ASSERT_EQ_INT(ctx, 32, trap3_get_work(actor)->travel);

    reset_state();
    actor = &actwk[4];
    actor->r_no0 = 2;
    trap3_get_work(actor)->origin_x = 2048;
    trapdr3(actor);
    TEST_ASSERT_EQ_INT(ctx, 1, hitchk_count);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);

    reset_state();
    actor = &actwk[6];
    actor->r_no0 = 2;
    for3(actor);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_EQ_INT(ctx, 1, ridechk_count);
    trap3_get_work(actor)->base_x = 2048;
    for3_move(actor);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);

    reset_state();
    actor = &actwk[6];
    dai3sub(actor);
    TEST_ASSERT_EQ_INT(ctx, 0, sinset_count);
    trap3_get_work(actor)->bob_enabled = 1;
    trap3_get_work(actor)->bob_angle = 8;
    actor->cddat = 0;
    dai3sub(actor);
    TEST_ASSERT_EQ_INT(ctx, 0, trap3_get_work(actor)->bob_angle);
    trap3_get_work(actor)->bob_angle = 64;
    actor->cddat = 8;
    sinset_count = 0;
    dai3sub(actor);
    TEST_ASSERT_EQ_INT(ctx, 0, sinset_count);

    reset_state();
    actor = &actwk[6];
    actor->userflag.b.h = 2;
    for3_lmv(actor);
    TEST_ASSERT_EQ_INT(ctx, -768, trap3_get_work(actor)->velocity);
    trap3_get_work(actor)->wait_timer = 2;
    trap3_get_work(actor)->velocity = -768;
    trap3_get_work(actor)->acceleration = 0;
    for3_rmv(actor);
    TEST_ASSERT_EQ_INT(ctx, 1, trap3_get_work(actor)->wait_timer);
    trap3_get_work(actor)->wait_timer = 1;
    trap3_get_work(actor)->phase = 7;
    for3_rmv(actor);
    TEST_ASSERT_EQ_INT(ctx, 2, trap3_get_work(actor)->phase);

    actor = &actwk[7];
    actor->userflag.b.h = 4;
    for3_umv(actor);
    TEST_ASSERT_EQ_INT(ctx, 512, trap3_get_work(actor)->velocity);
    trap3_get_work(actor)->wait_timer = 2;
    trap3_get_work(actor)->velocity = 512;
    trap3_get_work(actor)->acceleration = 0;
    for3_dmv(actor);
    TEST_ASSERT_EQ_INT(ctx, 1, trap3_get_work(actor)->wait_timer);
    trap3_get_work(actor)->wait_timer = 1;
    trap3_get_work(actor)->phase = 7;
    for3_dmv(actor);
    TEST_ASSERT_EQ_INT(ctx, 2, trap3_get_work(actor)->phase);

    reset_state();
    actor = &actwk[6];
    actor->userflag.b.h = 5;
    ridechk_result = 1;
    for3_rup(actor);
    TEST_ASSERT_EQ_INT(ctx, 30, trap3_get_work(actor)->wait_timer);
    for3_rup3(actor);
    TEST_ASSERT_EQ_INT(ctx, 30, trap3_get_work(actor)->wait_timer);
    trap3_get_work(actor)->wait_timer = 2;
    for3_rup1(actor);
    TEST_ASSERT_EQ_INT(ctx, 1, trap3_get_work(actor)->wait_timer);

    reset_state();
    actor = &actwk[6];
    actor->userflag.b.h = 6;
    trap3_get_work(actor)->phase = 1;
    for3_rup2(actor);
    TEST_ASSERT_EQ_INT(ctx, -768, trap3_get_work(actor)->velocity);
    actor = &actwk[7];
    actor->userflag.b.h = 7;
    trap3_get_work(actor)->phase = 1;
    for3_rup2(actor);
    TEST_ASSERT_EQ_INT(ctx, -1024, trap3_get_work(actor)->velocity);
    trap3_get_work(actor)->wait_timer = 2;
    trap3_get_work(actor)->velocity = -1024;
    trap3_get_work(actor)->acceleration = 0;
    for3_rup2(actor);
    TEST_ASSERT_EQ_INT(ctx, 1, trap3_get_work(actor)->wait_timer);
    trap3_get_work(actor)->wait_timer = 1;
    trap3_get_work(actor)->phase = 2;
    trap3_get_work(actor)->velocity = -1024;
    trap3_get_work(actor)->acceleration = 0;
    for3_rup2(actor);
    TEST_ASSERT_EQ_INT(ctx, 2, trap3_get_work(actor)->timer);
    TEST_ASSERT_EQ_INT(ctx, 1, trap3_get_work(actor)->bob_enabled);

    reset_state();
    actor = &actwk[8];
    actwkchk_fail_after = 0;
    gandair3_daii(actor);
    TEST_ASSERT_EQ_INT(ctx, 0, actor->r_no0);

    reset_state();
    actor = &actwk[8];
    actor->userflag.b.h = 1;
    gandair3(actor);
    TEST_ASSERT_EQ_INT(ctx, 2, actor->r_no0);
    actor = &actwk[9];
    actor->userflag.b.h = 2;
    actor->xposi.w.h = 100;
    gandair3(actor);
    TEST_ASSERT_EQ_INT(ctx, 2, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_s_count);
    actor = &actwk[10];
    actor->r_no0 = 2;
    trap3_get_work(actor)->origin_x = 2048;
    gandair3(actor);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    frameout_count = 0;
    actor->userflag.b.h = 1;
    gandair3_core(actor);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);

    reset_state();
    actor = &actwk[8];
    actor->xposi.w.h = 300;
    actwk[0].xposi.w.h = 0;
    gandair3_wait(actor);
    TEST_ASSERT_EQ_INT(ctx, 1, ride_on_chk_count);
    TEST_ASSERT_EQ_INT(ctx, 0, trap3_get_work(actor)->timer);
    actor->xposi.w.h = 10;
    actwk[0].xposi.w.h = 50;
    gandair3_wait(actor);
    TEST_ASSERT_EQ_INT(ctx, 59, trap3_get_work(actor)->timer);
    trap3_get_work(actor)->timer = 0;
    actor->xposi.w.h = 100;
    actwk[0].xposi.w.h = 100;
    gandair3_wait(actor);
    TEST_ASSERT_EQ_INT(ctx, 59, trap3_get_work(actor)->timer);
    trap3_get_work(actor)->timer = 2;
    time_flag = 1;
    generate_flag = 0;
    patchg_count = 0;
    gandair3_wait(actor);
    TEST_ASSERT_EQ_INT(ctx, 1, patchg_count);
    TEST_ASSERT_EQ_INT(ctx, 1, trap3_get_work(actor)->timer);
    trap3_get_work(actor)->timer = 2;
    time_flag = 2;
    generate_flag = 1;
    patchg_count = 0;
    gandair3_wait(actor);
    TEST_ASSERT_EQ_INT(ctx, 0, patchg_count);

    reset_state();
    actor = &actwk[8];
    trap3_get_work(actor)->base_x = 100;
    trap3_get_work(actor)->travel = 8;
    gandair3_fire(actor);
    TEST_ASSERT_EQ_INT(ctx, 16, trap3_get_work(actor)->travel);
    TEST_ASSERT_EQ_INT(ctx, 1, ride_on_chk_count);

    reset_state();
    actor = &actwk[8];
    trap3_get_work(actor)->base_x = 100;
    trap3_get_work(actor)->timer = 1;
    gandair3_end(actor);
    TEST_ASSERT_EQ_INT(ctx, 0, trap3_get_work(actor)->timer);
    trap3_get_work(actor)->travel = 4;
    gandair3_end(actor);
    TEST_ASSERT_EQ_INT(ctx, 60, trap3_get_work(actor)->timer);
    TEST_ASSERT_EQ_INT(ctx, 2, actor->r_no0);
    trap3_get_work(actor)->timer = 0;
    actor->r_no0 = 0;
    trap3_get_work(actor)->travel = 2;
    gandair3_end(actor);
    TEST_ASSERT_EQ_INT(ctx, 0, actor->r_no0);

    actor->r_no0 = 8;
    trap3_get_work(actor)->timer = 0;
    gandair3_wait2(actor);
    TEST_ASSERT_EQ_INT(ctx, 2, actor->r_no0);
    trap3_get_work(actor)->timer = 2;
    gandair3_wait2(actor);
    TEST_ASSERT_EQ_INT(ctx, 1, trap3_get_work(actor)->timer);

    reset_state();
    actor = &actwk[8];
    time_flag = 0;
    gandair3_tamaset(actor);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);
    time_flag = 2;
    generate_flag = 1;
    gandair3_tamaset(actor);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);

    reset_state();
    actor = &actwk[12];
    actor->xposi.w.h = 100;
    trap3_get_work(actor)->timer = 1;
    gandair3_tami(actor);
    TEST_ASSERT_EQ_INT(ctx, 272, actor->xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 16, trap3_get_work(actor)->bullet_acceleration);
    TEST_ASSERT_EQ_INT(ctx, 2, actor->mstno.b.h);
    trap3_get_work(actor)->acceleration = 0;
    actor->xposi.w.h = 100;
    trap3_get_work(actor)->travel = 0;
    frameout_count = 0;
    gandair3_tamm(actor);
    TEST_ASSERT_EQ_INT(ctx, 29, trap3_get_work(actor)->travel);
    TEST_ASSERT_EQ_INT(ctx, 0, frameout_count);

    reset_state();
    actor = &actwk[13];
    actor->r_no0 = 2;
    trap3_get_work(actor)->timer = 1;
    time_flag = 0;
    drumr3_move1(actor);
    TEST_ASSERT_EQ_INT(ctx, 1, trap3_get_work(actor)->timer);
    time_flag = 1;
    gametimer.w = 11;
    drumr3_move1(actor);
    TEST_ASSERT_EQ_INT(ctx, 1, trap3_get_work(actor)->timer);
    gametimer.w = 10;
    trap3_get_work(actor)->origin_x = 256;
    drumr3_move1(actor);
    TEST_ASSERT_EQ_INT(ctx, 4, trap3_get_work(actor)->timer);
    trap3_get_work(actor)->timer = 2;
    trap3_get_work(actor)->travel = 0;
    drumr3_move1(actor);
    TEST_ASSERT_EQ_INT(ctx, 1, trap3_get_work(actor)->timer);
    trap3_get_work(actor)->timer = 1;
    trap3_get_work(actor)->travel = 1;
    drumr3_move1(actor);
    TEST_ASSERT_EQ_INT(ctx, 4, actor->r_no0);

    reset_state();
    actor = &actwk[13];
    actor->r_no0 = 2;
    trap3_get_work(actor)->base_x = 2048;
    drumr3(actor);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    actor->userflag.b.h = 2;
    actor->r_no0 = 0;
    drumr3_init(actor);
    TEST_ASSERT_EQ_INT(ctx, 4, actor->r_no0);
    actor->userflag.b.h = 6;
    actor->r_no0 = 0;
    drumr3_init(actor);
    TEST_ASSERT_EQ_INT(ctx, 6, actor->r_no0);

    actor->r_no0 = 4;
    trap3_get_work(actor)->timer = 0;
    drumr3_move2(actor);
    TEST_ASSERT_EQ_INT(ctx, 4, trap3_get_work(actor)->timer);
    trap3_get_work(actor)->timer = 1;
    time_flag = 0;
    drumr3_move2(actor);
    TEST_ASSERT_EQ_INT(ctx, 1, trap3_get_work(actor)->timer);
    time_flag = 1;
    gametimer.w = 11;
    drumr3_move2(actor);
    TEST_ASSERT_EQ_INT(ctx, 1, trap3_get_work(actor)->timer);
    trap3_get_work(actor)->timer = 1;
    trap3_get_work(actor)->travel = 5;
    trap3_get_work(actor)->origin_x = 256;
    gametimer.w = 10;
    time_flag = 1;
    drumr3_move2(actor);
    TEST_ASSERT_EQ_INT(ctx, 6, actor->r_no0);
    actor->r_no0 = 4;
    trap3_get_work(actor)->timer = 2;
    trap3_get_work(actor)->travel = 0;
    drumr3_move2(actor);
    TEST_ASSERT_EQ_INT(ctx, 1, trap3_get_work(actor)->timer);

    actor->r_no0 = 6;
    trap3_get_work(actor)->timer = 0;
    drumr3_move3(actor);
    TEST_ASSERT_EQ_INT(ctx, 4, trap3_get_work(actor)->timer);
    trap3_get_work(actor)->timer = 1;
    time_flag = 0;
    drumr3_move3(actor);
    TEST_ASSERT_EQ_INT(ctx, 1, trap3_get_work(actor)->timer);
    time_flag = 1;
    gametimer.w = 11;
    drumr3_move3(actor);
    TEST_ASSERT_EQ_INT(ctx, 1, trap3_get_work(actor)->timer);
    trap3_get_work(actor)->timer = 1;
    trap3_get_work(actor)->travel = 17;
    trap3_get_work(actor)->origin_x = 256;
    gametimer.w = 10;
    drumr3_move3(actor);
    TEST_ASSERT_EQ_INT(ctx, 2, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, trap3_get_work(actor)->travel);
    actor->r_no0 = 6;
    trap3_get_work(actor)->timer = 2;
    trap3_get_work(actor)->travel = 0;
    drumr3_move3(actor);
    TEST_ASSERT_EQ_INT(ctx, 1, trap3_get_work(actor)->timer);
}

TEST_MAIN_BEGIN;
    test_harir3_initializes_damages_and_follows_parent(&ctx);
    test_harir3_motion_and_guard_branches(&ctx);
    test_frdr3_spawns_linked_actor_and_moves(&ctx);
    test_trapdr3_children_and_vertical_response(&ctx);
    test_for3_motion_modes_and_swing(&ctx);
    test_getdair3_extends_retracts_and_frames_out(&ctx);
    test_gandair3_core_bullets_and_ride_effect(&ctx);
    test_drumr3_and_tr3_fout(&ctx);
    test_dispatchers_and_late_state_branches(&ctx);
TEST_MAIN_END
