/* <stdlib.h> declares POSIX random(); the decompiled src/io.h declares the
   game's own Sint32 random(void). Parse stdlib first, then rename the game
   symbol inside this harness so the two can coexist. */
#include <stdlib.h>

#define random game_random

#include <stddef.h>
#include <string.h>

#include "support/test_runner.h"
#include "src/types.h"

sprite_status actwk[128];
Uint8 bossflag;
Uint8 bossstart;
Uint8 generate_flag;
Sint16 scralim_left;
Sint16 scralim_n_left;
Sint16 scralim_right;
Sint16 scralim_n_right;
Sint16 scralim_down;
Sint16 scralim_n_down;
Sint16 scr_dir_tbl[6] = {0, 0, 3904, 0, 0, 0};
Uint8 clchgtim[7];

static int actionsub_count;
static sprite_status *actionsub_actor;
static int patchg_count;
static sprite_status *patchg_actor;
static Uint8 **patchg_table;
static int frameout_count;
static sprite_status *frameout_actor;
static int actwkchk_count;
static int next_alloc_index;
static int actwkchk_fail_after;
static int colorset_count;
static Sint32 colorset_value;
static int colorset2_count;
static Sint32 colorset2_value;
static int soundset_count;
static Sint16 soundset_requests[16];
static int sub_sync_count;
static Sint16 sub_sync_requests[8];
static int scoreup_count;
static Uint32 scoreup_value;
static int genecolor_count;
static int flashout_count;
static int flashin_count;
static int fin_boss6_count;
static int fout_boss6_count;
static int ride_on_chk_count;
static sprite_status *ride_on_chk_actor;
static sprite_status *ride_on_chk_player;
static Sint16 ride_on_chk_result;
static int emycol_d_count;
static sprite_status *emycol_d_actor;
static Sint16 emycol_d_result;
static int pcole_count;
static sprite_status *pcole_player;
static sprite_status *pcole_actor;
static int sinset_count;
static Uint8 sinset_angle;
static Sint16 sinset_sin;
static Sint16 sinset_cos;
static Sint32 random_result;

void actionsub(sprite_status *pActwk);
void patchg(sprite_status *patchgwk, Uint8 **pat_dat);
void frameout(sprite_status *pActwk);
Sint32 actwkchk(sprite_status **ppActwk);
void colorset(Sint32 ColorNo);
void colorset2(Sint32 ColorNo);
void soundset(Sint16 ReqNo);
void sub_sync(Sint16 ReqNo);
void scoreup(Uint32 updata);
void genecolor(void);
void flashout(void);
void flashin_boss6(void);
void fin_boss6(Uint8 *pChgTime, Uint8 *pChgCnt);
void fout_boss6(Uint8 *pChgTime, Uint8 *pChgCnt);
Sint16 ride_on_chk(sprite_status *pActwk, sprite_status *pPlayerwk);
Sint16 emycol_d(sprite_status *pActwk);
Sint16 pcole(sprite_status *pActwk, sprite_status *pColliAct);
void sinset(Uint8 kakudo, Sint16 *sin, Sint16 *cos);
Sint32 random(void);

#include "src/r6/boss_6.c"

void actionsub(sprite_status *pActwk) {
    ++actionsub_count;
    actionsub_actor = pActwk;
}

void patchg(sprite_status *patchgwk, Uint8 **pat_dat) {
    ++patchg_count;
    patchg_actor = patchgwk;
    patchg_table = pat_dat;
}

void frameout(sprite_status *pActwk) {
    ++frameout_count;
    frameout_actor = pActwk;
}

Sint32 actwkchk(sprite_status **ppActwk) {
    ++actwkchk_count;
    if (actwkchk_fail_after >= 0 && actwkchk_count > actwkchk_fail_after) {
        *ppActwk = 0;
        return -1;
    }
    *ppActwk = &actwk[next_alloc_index++];
    return 0;
}

void colorset(Sint32 ColorNo) {
    ++colorset_count;
    colorset_value = ColorNo;
}

void colorset2(Sint32 ColorNo) {
    ++colorset2_count;
    colorset2_value = ColorNo;
}

void soundset(Sint16 ReqNo) {
    if (soundset_count < (int)(sizeof(soundset_requests) /
                               sizeof(soundset_requests[0]))) {
        soundset_requests[soundset_count] = ReqNo;
    }
    ++soundset_count;
}

void sub_sync(Sint16 ReqNo) {
    if (sub_sync_count < (int)(sizeof(sub_sync_requests) /
                               sizeof(sub_sync_requests[0]))) {
        sub_sync_requests[sub_sync_count] = ReqNo;
    }
    ++sub_sync_count;
}

void scoreup(Uint32 updata) {
    ++scoreup_count;
    scoreup_value = updata;
}

void genecolor(void) { ++genecolor_count; }

void flashout(void) { ++flashout_count; }

void flashin_boss6(void) { ++flashin_count; }

void fin_boss6(Uint8 *pChgTime, Uint8 *pChgCnt) {
    ++fin_boss6_count;
    ++*pChgTime;
    ++*pChgCnt;
}

void fout_boss6(Uint8 *pChgTime, Uint8 *pChgCnt) {
    ++fout_boss6_count;
    ++*pChgTime;
    ++*pChgCnt;
}

Sint16 ride_on_chk(sprite_status *pActwk, sprite_status *pPlayerwk) {
    ++ride_on_chk_count;
    ride_on_chk_actor = pActwk;
    ride_on_chk_player = pPlayerwk;
    return ride_on_chk_result;
}

Sint16 emycol_d(sprite_status *pActwk) {
    ++emycol_d_count;
    emycol_d_actor = pActwk;
    return emycol_d_result;
}

Sint16 pcole(sprite_status *pActwk, sprite_status *pColliAct) {
    ++pcole_count;
    pcole_player = pActwk;
    pcole_actor = pColliAct;
    return -1;
}

void sinset(Uint8 kakudo, Sint16 *sin, Sint16 *cos) {
    ++sinset_count;
    sinset_angle = kakudo;
    *sin = sinset_sin;
    *cos = sinset_cos;
}

Sint32 random(void) { return random_result; }

static void set_egg6_field(sprite_status *actor, int short_index,
                                  Sint16 value) {
    egg6_work *work = egg6_get_work(actor);

    if (actor == &actwk[0] && short_index == 26) {
        player_work_get(actor)->damage_invulnerability_timer = value;
        return;
    }

    switch (short_index) {
    case 23:
        work->timer_word = value;
        break;
    case 25:
        work->parent_index = value;
        break;
    case 26:
        work->x_acceleration = value;
        break;
    case 27:
        work->y_acceleration = value;
        break;
    case 28:
        work->table_offset = value;
        break;
    case 30:
        work->ride_y_offset = value;
        break;
    case 31:
        work->bob_offset = value;
        break;
    default:
        break;
    }
}

static Sint16 egg6_field(sprite_status *actor, int short_index) {
    egg6_work *work = egg6_get_work(actor);

    switch (short_index) {
    case 23:
        return work->timer_word;
    case 25:
        return work->parent_index;
    case 26:
        return work->x_acceleration;
    case 27:
        return work->y_acceleration;
    case 28:
        return work->table_offset;
    case 30:
        return work->ride_y_offset;
    case 31:
        return work->bob_offset;
    default:
        return 0;
    }
}

static void reset_boss6_state(void) {
    memset(actwk, 0, sizeof(actwk));
    bossflag = 0;
    bossstart = 0;
    generate_flag = 0;
    scralim_left = scralim_n_left = 0;
    scralim_right = scralim_n_right = 0;
    scralim_down = scralim_n_down = 0;
    memset(clchgtim, 0, sizeof(clchgtim));
    actionsub_count = 0;
    actionsub_actor = 0;
    patchg_count = 0;
    patchg_actor = 0;
    patchg_table = 0;
    frameout_count = 0;
    frameout_actor = 0;
    actwkchk_count = 0;
    next_alloc_index = 20;
    actwkchk_fail_after = -1;
    colorset_count = 0;
    colorset_value = 0;
    colorset2_count = 0;
    colorset2_value = 0;
    soundset_count = 0;
    memset(soundset_requests, 0, sizeof(soundset_requests));
    sub_sync_count = 0;
    memset(sub_sync_requests, 0, sizeof(sub_sync_requests));
    scoreup_count = 0;
    scoreup_value = 0;
    genecolor_count = 0;
    flashout_count = 0;
    flashin_count = 0;
    fin_boss6_count = 0;
    fout_boss6_count = 0;
    ride_on_chk_count = 0;
    ride_on_chk_actor = 0;
    ride_on_chk_player = 0;
    ride_on_chk_result = 0;
    emycol_d_count = 0;
    emycol_d_actor = 0;
    emycol_d_result = 0;
    pcole_count = 0;
    pcole_player = 0;
    pcole_actor = 0;
    sinset_count = 0;
    sinset_angle = 0;
    sinset_sin = 0;
    sinset_cos = 0;
    random_result = 0;
}

static void test_floor_beam_color_and_motion_helpers(test_context *ctx) {
    sprite_status *actor = &actwk[4];

    reset_boss6_state();
    actwk[0].yposi.w.h = 664;
    TEST_ASSERT_EQ_INT(ctx, 2, get_snc_floor());
    actwk[0].yposi.w.h = 665;
    TEST_ASSERT_EQ_INT(ctx, 1, get_snc_floor());
    actwk[0].yposi.w.h = 1177;
    TEST_ASSERT_EQ_INT(ctx, 0, get_snc_floor());

    actwk[0].yposi.w.h = 864;
    TEST_ASSERT_EQ_INT(ctx, 7, get_beam_pos());
    actwk[0].yposi.w.h = 865;
    TEST_ASSERT_EQ_INT(ctx, 6, get_beam_pos());
    actwk[0].yposi.w.h = 1377;
    TEST_ASSERT_EQ_INT(ctx, 5, get_beam_pos());

    colchg_set(9);
    TEST_ASSERT_EQ_INT(ctx, 9, clchgtim[0]);
    TEST_ASSERT_EQ_INT(ctx, 9, clchgtim[5]);
    colchg_stop();
    TEST_ASSERT_EQ_INT(ctx, 128, clchgtim[3]);
    colchg_start();
    TEST_ASSERT_EQ_INT(ctx, 0, clchgtim[3]);

    actor->xposi.l = 10 << 16;
    actor->yposi.l = 20 << 16;
    actor->xspeed.w = 256;
    actor->yspeed.w = -512;
    add_spd(actor);
    TEST_ASSERT_EQ_INT(ctx, 11, actor->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 18, actor->yposi.w.h);

    set_egg6_field(actor, 26, 16);
    set_egg6_field(actor, 27, -32);
    add_spd2(actor);
    TEST_ASSERT_EQ_INT(ctx, 272, actor->xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, -544, actor->yspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 12, actor->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 15, actor->yposi.w.h);

    actor->yposi.w.h = 1776;
    frameout_chk(actor);
    TEST_ASSERT_EQ_INT(ctx, 0, frameout_count);
    actor->yposi.w.h = 1777;
    frameout_chk(actor);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actor == actor);
}

static void test_start_initializes_boss_and_meca_parts(test_context *ctx) {
    sprite_status *boss = &actwk[4];

    reset_boss6_state();
    actwk[0].xposi.w.h = 2687;
    egg6_start(boss);
    TEST_ASSERT_EQ_INT(ctx, 0, boss->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);

    actwk[0].xposi.w.h = 2688;
    egg6_start(boss);
    TEST_ASSERT_EQ_INT(ctx, 2, boss->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, colorset_count);
    TEST_ASSERT_EQ_INT(ctx, 6, colorset_value);
    TEST_ASSERT_EQ_INT(ctx, 1, colorset2_count);
    TEST_ASSERT_EQ_INT(ctx, 6, colorset2_value);
    TEST_ASSERT_EQ_INT(ctx, 3, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 53, actwk[20].actno);
    TEST_ASSERT_EQ_INT(ctx, 54, actwk[21].actno);
    TEST_ASSERT_EQ_INT(ctx, 55, actwk[22].actno);
    TEST_ASSERT_EQ_INT(ctx, 20, egg6_field(boss, 25));
    TEST_ASSERT_EQ_INT(ctx, 4, egg6_field(&actwk[20], 25));
    TEST_ASSERT_EQ_INT(ctx, 60, boss->colino);
    TEST_ASSERT_EQ_INT(ctx, 61, actwk[20].colino);
    TEST_ASSERT_EQ_INT(ctx, 22, bossflag);
    TEST_ASSERT_EQ_INT(ctx, 6, bossstart);
    TEST_ASSERT_EQ_INT(ctx, 2528, scralim_left);
    TEST_ASSERT_EQ_INT(ctx, 2656, scralim_right);

    reset_boss6_state();
    bossflag = 1;
    set_egg6_field(boss, 23, 119);
    egg6_ini(boss);
    TEST_ASSERT_EQ_INT(ctx, 4, boss->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, boss->xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 768, boss->yspeed.w);
    TEST_ASSERT_EQ_INT(ctx, -1, egg6_field(boss, 27));
    TEST_ASSERT_EQ_INT(ctx, 1, sub_sync_count);
    TEST_ASSERT_EQ_INT(ctx, 103, sub_sync_requests[0]);
}

static void test_hit_coliset_and_beam_control(test_context *ctx) {
    sprite_status *boss = &actwk[4];
    sprite_status *meca = &actwk[20];

    reset_boss6_state();
    set_egg6_field(boss, 25, 20);
    egg6_get_work(boss)->flags = 1;
    egg6_coliset(boss);
    TEST_ASSERT_EQ_INT(ctx, 60, boss->colino);
    TEST_ASSERT_EQ_INT(ctx, 2, boss->colicnt);
    TEST_ASSERT_EQ_INT(ctx, 61, meca->colino);
    TEST_ASSERT_EQ_INT(ctx, 2, meca->colicnt);

    egg6_hitchk(boss);
    TEST_ASSERT_EQ_INT(ctx, 0, soundset_count);

    meca->colino = 0;
    egg6_hitchk(boss);
    TEST_ASSERT_EQ_INT(ctx, 1, egg6_get_work(boss)->hit_count);
    TEST_ASSERT_EQ_INT(ctx, 0, boss->colino);
    TEST_ASSERT_EQ_INT(ctx, 0, meca->colino);
    TEST_ASSERT_EQ_INT(ctx, 1, boss->mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 30, egg6_get_work(boss)->hit_timer);
    TEST_ASSERT_EQ_INT(ctx, 1, soundset_count);
    TEST_ASSERT_EQ_INT(ctx, 172, soundset_requests[0]);

    egg6_get_work(boss)->hit_timer = 1;
    egg6_hitchk(boss);
    TEST_ASSERT_EQ_INT(ctx, 60, boss->colino);
    TEST_ASSERT_EQ_INT(ctx, 61, meca->colino);

    reset_boss6_state();
    actwk[0].yposi.w.h = 1500;
    bossflag = 0;
    bossstart = 0;
    egg6_beam_ctrl();
    TEST_ASSERT_EQ_INT(ctx, 64, bossflag);
    bossflag = 128;
    egg6_beam_ctrl();
    TEST_ASSERT_EQ_INT(ctx, 128, bossflag);
    bossstart = 32;
    egg6_beam_ctrl();
    TEST_ASSERT_EQ_INT(ctx, 0, bossflag & 128);
}

static void test_vertical_state_transitions_and_event_helpers(test_context *ctx) {
    sprite_status *boss = &actwk[4];

    reset_boss6_state();
    egg6_get_work(boss)->floor_index = 0;
    boss->yposi.w.h = 1500;
    boss->yspeed.w = 256;
    set_egg6_field(boss, 27, 0);
    egg6_down(boss);
    TEST_ASSERT_EQ_INT(ctx, 0, boss->r_no0);
    boss->yposi.w.h = 1567;
    egg6_down(boss);
    TEST_ASSERT_EQ_INT(ctx, 1568, boss->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 6, boss->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, egg6_get_work(boss)->state);
    TEST_ASSERT_EQ_INT(ctx, 177, soundset_requests[0]);

    reset_boss6_state();
    egg6_get_work(boss)->floor_index = 0;
    boss->yposi.w.h = 1064;
    boss->yspeed.w = -256;
    set_egg6_field(boss, 27, 0);
    egg6_up(boss);
    TEST_ASSERT_EQ_INT(ctx, 1064, boss->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 10, boss->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 240, egg6_get_work(boss)->quake_timer);
    TEST_ASSERT_EQ_INT(ctx, 1, egg6_get_work(boss)->floor_index);
    TEST_ASSERT_EQ_INT(ctx, 163, soundset_requests[0]);

    reset_boss6_state();
    egg6_get_work(boss)->floor_index = 0;
    sinset_sin = 512;
    egg6_get_work(boss)->bob_angle = 7;
    fuwafuwa(boss);
    TEST_ASSERT_EQ_INT(ctx, 1, sinset_count);
    TEST_ASSERT_EQ_INT(ctx, 7, sinset_angle);
    TEST_ASSERT_EQ_INT(ctx, 8, egg6_field(boss, 31));
    TEST_ASSERT_EQ_INT(ctx, 9, egg6_get_work(boss)->bob_angle);

    egg6_get_work(boss)->floor_index = 3;
    egg6_get_work(boss)->bob_angle = 3;
    boss->xposi.w.h = 100;
    fuwafuwa(boss);
    TEST_ASSERT_EQ_INT(ctx, 99, boss->xposi.w.h);

    reset_boss6_state();
    egg6_get_work(boss)->floor_index = 2;
    event_end(boss);
    TEST_ASSERT_EQ_INT(ctx, 4, boss->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 768, boss->yspeed.w);
    TEST_ASSERT_EQ_INT(ctx, -1, egg6_field(boss, 27));
    egg6_get_work(boss)->floor_index = 3;
    event_end(boss);
    TEST_ASSERT_EQ_INT(ctx, 10, boss->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 360, egg6_field(boss, 23));
}

static void test_spawn_helpers_and_projectiles(test_context *ctx) {
    sprite_status *boss = &actwk[4];

    reset_boss6_state();
    egg6_get_work(boss)->floor_index = 1;
    TEST_ASSERT_EQ_INT(ctx, 1420, get_gareki_y(boss));
    egg6_get_work(boss)->floor_index = 2;
    TEST_ASSERT_EQ_INT(ctx, 908, get_gareki_y(boss));
    egg6_get_work(boss)->floor_index = 3;
    TEST_ASSERT_EQ_INT(ctx, 396, get_gareki_y(boss));

    make_beam(1);
    TEST_ASSERT_EQ_INT(ctx, 2, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 64, actwk[20].actno);
    TEST_ASSERT_EQ_INT(ctx, 2584, actwk[20].xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1160, actwk[20].yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[21].userflag.b.h);

    reset_boss6_state();
    egg6_get_work(boss)->floor_index = 1;
    actwk[0].xposi.w.h = 2600;
    make_toge(boss, 1);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);
    make_toge(boss, 0);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 56, actwk[20].actno);
    TEST_ASSERT_EQ_INT(ctx, 2592, actwk[20].xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1420, actwk[20].yposi.w.h);

    make_yuka(boss, 0);
    TEST_ASSERT_EQ_INT(ctx, 2, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 61, actwk[21].actno);
    TEST_ASSERT_EQ_INT(ctx, 2712, actwk[21].xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1420, actwk[21].yposi.w.h);

    random_result = 0x12345678;
    make_gareki(boss, 0);
    TEST_ASSERT_EQ_INT(ctx, 3, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 57, actwk[22].actno);
    TEST_ASSERT_EQ_INT(ctx, 2808, actwk[22].xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 376, actwk[22].xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 16, egg6_field(&actwk[22], 27));

    reset_boss6_state();
    make_bakuha(boss);
    TEST_ASSERT_EQ_INT(ctx, 63, actwk[20].actno);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[20].mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 158, soundset_requests[0]);
}

static void test_child_actor_initializers_and_small_states(test_context *ctx) {
    sprite_status *parent = &actwk[4];
    sprite_status *child = &actwk[20];

    reset_boss6_state();
    parent->xposi.w.h = 300;
    parent->yposi.w.h = 500;
    set_egg6_field(child, 25, 4);
    egg6meca0(child);
    TEST_ASSERT_EQ_INT(ctx, 2, child->r_no0);
    TEST_ASSERT_TRUE(ctx, child->patbase == egg6meca0_pat);
    egg6meca0(child);
    TEST_ASSERT_EQ_INT(ctx, 300, child->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 484, child->yposi.w.h);
    child->r_no0 = 4;
    egg6meca0(child);
    TEST_ASSERT_EQ_INT(ctx, 1, child->patno);
    TEST_ASSERT_EQ_INT(ctx, 1, ride_on_chk_count);

    reset_boss6_state();
    set_egg6_field(child, 25, 4);
    egg6_get_work(parent)->state_timer = 1;
    egg6meca1(child);
    egg6meca1(child);
    TEST_ASSERT_EQ_INT(ctx, 2, egg6_get_work(parent)->state_timer);
    TEST_ASSERT_EQ_INT(ctx, 20, child->pattimm);
    TEST_ASSERT_EQ_INT(ctx, 2, child->pattim);
    egg6_get_work(parent)->state_timer = 255;
    egg6meca1(child);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);

    reset_boss6_state();
    set_egg6_field(child, 25, 4);
    egg6_get_work(parent)->state = 255;
    egg6meca2(child);
    egg6meca2(child);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);

    reset_boss6_state();
    child->mstno.b.h = 0;
    egg6bakuha(child);
    TEST_ASSERT_EQ_INT(ctx, 4, child->r_no0);
    set_egg6_field(child, 23, 23);
    egg6bakuha(child);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);

    reset_boss6_state();
    emycol_d_result = 2;
    egg6toge(child);
    TEST_ASSERT_EQ_INT(ctx, 4, child->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, child->colino);
    egg6_get_work(child)->anim_timer = 3;
    egg6toge(child);
    TEST_ASSERT_EQ_INT(ctx, 2, actionsub_count);
    egg6_get_work(child)->anim_timer = 9;
    egg6_get_work(child)->timer = 6;
    egg6toge(child);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
}

static void test_yuka_door_damage_and_death_helpers(test_context *ctx) {
    sprite_status *boss = &actwk[4];
    sprite_status *platform = &actwk[20];
    sprite_status *door = &actwk[30];

    reset_boss6_state();
    actwk[0].actno = 1;
    actwk[0].xposi.w.h = 100;
    actwk[0].yposi.w.h = 260;
    actwk[0].sprvsize = 14;
    platform->xposi.w.h = 100;
    platform->yposi.w.h = 300;
    platform->yspeed.w = 128;
    TEST_ASSERT_EQ_INT(ctx, -1, egg6_yuka_hasami_chk(platform));
    TEST_ASSERT_EQ_INT(ctx, 1, pcole_count);
    TEST_ASSERT_EQ_INT(ctx, 512, actwk[0].yspeed.w);

    pcole_count = 0;
    actwk[0].cddat = 8;
    actwk[0].yposi.l = 0;
    platform->yposi.l = 400 << 16;
    platform->yspeed.w = -256;
    egg6_yuka_ride_on(platform);
    TEST_ASSERT_EQ_INT(ctx, 370, actwk[0].yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, -256, actwk[0].yspeed.w);

    reset_boss6_state();
    door->userflag.b.h = 0;
    egg6door_ini(door);
    TEST_ASSERT_EQ_INT(ctx, 2, door->r_no0);
    TEST_ASSERT_TRUE(ctx, door->patbase == egg6door_pat);
    TEST_ASSERT_EQ_INT(ctx, 512, door->yspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 16384, egg6_field(door, 28));
    actwk[0].xposi.w.h = 2700;
    egg6door_closewait(door);
    TEST_ASSERT_EQ_INT(ctx, 8, door->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 96, bossflag);

    door->r_no0 = 8;
    actwk[0].actno = 1;
    actwk[0].xposi.w.h = 2500;
    egg6door_closed(door);
    TEST_ASSERT_EQ_INT(ctx, 2560, actwk[0].xposi.w.h);

    reset_boss6_state();
    actwk[0].actno = 1;
    actwk[0].xposi.w.h = 3100;
    egg6door_openwait(door);
    TEST_ASSERT_EQ_INT(ctx, 2944, actwk[0].xposi.w.h);

    reset_boss6_state();
    boss->xposi.w.h = 3900;
    actwk[0].xposi.w.h = 3072;
    egg6_dead3(boss);
    TEST_ASSERT_EQ_INT(ctx, 18, boss->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, bossstart);
    TEST_ASSERT_EQ_INT(ctx, 1, genecolor_count);

    reset_boss6_state();
    set_egg6_field(boss, 23, 468);
    dead2_end(boss);
    TEST_ASSERT_EQ_INT(ctx, 16, boss->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 3904, scralim_right);
    TEST_ASSERT_EQ_INT(ctx, 1, sub_sync_count);
    TEST_ASSERT_EQ_INT(ctx, 29, sub_sync_requests[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, scoreup_count);
    TEST_ASSERT_EQ_INT(ctx, 100, scoreup_value);
}

static void test_beam_yuka_and_door_state_machines(test_context *ctx) {
    sprite_status *beam = &actwk[10];
    sprite_status *platform = &actwk[20];
    sprite_status *parent = &actwk[4];
    sprite_status *door = &actwk[30];

    reset_boss6_state();
    beam->userflag.b.h = 1;
    egg6beam(beam);
    TEST_ASSERT_EQ_INT(ctx, 2, beam->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 3, egg6_get_work(beam)->state_timer);
    TEST_ASSERT_EQ_INT(ctx, 0, egg6_get_work(beam)->state);
    TEST_ASSERT_TRUE(ctx, beam->patbase == egg6beam_pat);
    TEST_ASSERT_EQ_INT(ctx, 1, beam->actflg & 1);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);

    egg6_get_work(beam)->state_timer = 1;
    egg6_get_work(beam)->state = 2;
    egg6_get_work(beam)->anim_timer = 29;
    egg6beam_01(beam);
    TEST_ASSERT_EQ_INT(ctx, 4, egg6_get_work(beam)->state_timer);
    TEST_ASSERT_EQ_INT(ctx, 0, egg6_get_work(beam)->state);
    TEST_ASSERT_EQ_INT(ctx, 0, beam->patno);

    egg6_get_work(beam)->anim_timer = 44;
    egg6beam_01(beam);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actor == beam);

    reset_boss6_state();
    platform->userflag.b.l = 1;
    platform->yposi.w.h = 1500;
    egg6yuka(platform);
    TEST_ASSERT_EQ_INT(ctx, 2, platform->r_no0);
    TEST_ASSERT_TRUE(ctx, platform->patbase == egg6yuka_pat);
    TEST_ASSERT_EQ_INT(ctx, 8, egg6_field(platform, 28));
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);

    egg6yuka_01(platform);
    TEST_ASSERT_EQ_INT(ctx, 4, platform->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1168, platform->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 180, soundset_requests[0]);

    set_egg6_field(platform, 25, 4);
    platform->xposi.w.h = 100;
    actwk[0].xposi.w.h = 110;
    egg6_get_work(parent)->release_flag = 0;
    egg6yuka_02(platform);
    TEST_ASSERT_EQ_INT(ctx, 4, platform->r_no0);

    egg6_get_work(parent)->release_flag = 1;
    egg6yuka_02(platform);
    TEST_ASSERT_EQ_INT(ctx, 2, platform->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 4, egg6_field(platform, 30));
    TEST_ASSERT_EQ_INT(ctx, -1536, platform->yspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 14, egg6_field(platform, 27));

    reset_boss6_state();
    door->userflag.b.h = 1;
    egg6door_ini(door);
    TEST_ASSERT_EQ_INT(ctx, 4, door->r_no0);
    TEST_ASSERT_EQ_INT(ctx, -512, door->yspeed.w);
    TEST_ASSERT_EQ_INT(ctx, -24576, egg6_field(door, 28));
    TEST_ASSERT_EQ_INT(ctx, 80, door->sprvsize);

    egg6door_move(door);
    TEST_ASSERT_EQ_INT(ctx, 4, door->r_no0);
    set_egg6_field(door, 23, -24064);
    egg6door_move(door);
    TEST_ASSERT_EQ_INT(ctx, 10, door->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 9, door->sprhsize);
    TEST_ASSERT_EQ_INT(ctx, 16, door->sprvsize);

    reset_boss6_state();
    door->userflag.b.h = -1;
    egg6door_ini(door);
    TEST_ASSERT_EQ_INT(ctx, 10, door->r_no0);
    TEST_ASSERT_TRUE(ctx, door->patbase == egg6beamer_pat);
}

static void test_dead_state_helpers_and_timed_dead2_events(test_context *ctx) {
    sprite_status *boss = &actwk[4];
    sprite_status *meca = &actwk[20];

    reset_boss6_state();
    actwk[0].cddat = 128;
    actwk[0].xposi.w.h = 1000;
    dead_snc_pos();
    TEST_ASSERT_EQ_INT(ctx, 1000, actwk[0].xposi.w.h);

    actwk[0].cddat = 0;
    actwk[0].yposi.w.h = 700;
    actwk[0].xposi.w.h = 2700;
    dead_snc_pos();
    TEST_ASSERT_EQ_INT(ctx, 652, actwk[0].yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 2672, actwk[0].xposi.w.h);
    actwk[0].xposi.w.h = 2800;
    dead_snc_pos();
    TEST_ASSERT_EQ_INT(ctx, 2832, actwk[0].xposi.w.h);

    dead_snc_pos2();
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 2752, actwk[0].xposi.w.h);

    reset_boss6_state();
    boss->yposi.w.h = 650;
    boss->xspeed.w = 100;
    boss->yspeed.w = 256;
    egg6_dead1(boss);
    TEST_ASSERT_EQ_INT(ctx, -100, boss->xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 0, boss->r_no0);

    boss->yposi.w.h = 672;
    egg6_dead1(boss);
    TEST_ASSERT_EQ_INT(ctx, 14, boss->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 3, boss->mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 672, boss->yposi.w.h);

    reset_boss6_state();
    bossstart = 255;
    set_egg6_field(boss, 23, 120);
    egg6_dead2(boss);
    TEST_ASSERT_EQ_INT(ctx, 31, bossstart);

    reset_boss6_state();
    set_egg6_field(boss, 23, 239);
    egg6_dead2(boss);
    TEST_ASSERT_EQ_INT(ctx, 128, clchgtim[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, fout_boss6_count);

    reset_boss6_state();
    set_egg6_field(boss, 23, 324);
    egg6_dead2(boss);
    TEST_ASSERT_EQ_INT(ctx, 1, flashout_count);

    reset_boss6_state();
    set_egg6_field(boss, 25, 20);
    meca->yposi.w.h = 200;
    set_egg6_field(boss, 23, 325);
    egg6_dead2(boss);
    TEST_ASSERT_EQ_INT(ctx, 4, boss->mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 2864, boss->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 216, meca->yposi.w.h);

    reset_boss6_state();
    set_egg6_field(boss, 23, 382);
    egg6_dead2(boss);
    TEST_ASSERT_EQ_INT(ctx, 1, flashin_count);

    reset_boss6_state();
    set_egg6_field(boss, 23, 383);
    egg6_dead2(boss);
    TEST_ASSERT_EQ_INT(ctx, 1, fin_boss6_count);

    reset_boss6_state();
    set_egg6_field(boss, 23, 468);
    egg6_dead2(boss);
    TEST_ASSERT_EQ_INT(ctx, 16, boss->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, scoreup_count);
}

static void test_event_wrappers_and_search_paths(test_context *ctx) {
    sprite_status *boss = &actwk[4];
    Uint8 offset;
    sprite_status *found;

    reset_boss6_state();
    actwk[2].actno = 10;
    actwk[3].actno = 62;
    offset = 2;
    TEST_ASSERT_EQ_INT(ctx, 0, act_search(62, &offset, &found));
    TEST_ASSERT_TRUE(ctx, found == &actwk[3]);
    TEST_ASSERT_EQ_INT(ctx, 4, offset);
    TEST_ASSERT_EQ_INT(ctx, -1, act_search(99, &offset, &found));

    actwk[2].actno = 62;
    actwk[2].r_no0 = 2;
    actwk[3].actno = 62;
    actwk[3].r_no0 = 4;
    door_open(boss);
    TEST_ASSERT_EQ_INT(ctx, 8, actwk[3].r_no0);

    reset_boss6_state();
    egg6_get_work(boss)->floor_index = 2;
    actwk[0].yposi.w.h = 600;
    beamdemo_start(boss);
    TEST_ASSERT_EQ_INT(ctx, 178, soundset_requests[0]);
    TEST_ASSERT_EQ_INT(ctx, 2, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 64, actwk[20].actno);
    TEST_ASSERT_EQ_INT(ctx, 648, actwk[20].yposi.w.h);

    reset_boss6_state();
    bossstart = 3;
    actwk[0].yposi.w.h = 1500;
    beam_start(boss);
    TEST_ASSERT_EQ_INT(ctx, 35, bossstart);
    actwk[0].yposi.w.h = 1000;
    beam_start(boss);
    TEST_ASSERT_EQ_INT(ctx, 67, bossstart);
    actwk[0].yposi.w.h = 600;
    beam_start(boss);
    TEST_ASSERT_EQ_INT(ctx, 131, bossstart);

    reset_boss6_state();
    egg6_get_work(boss)->floor_index = 3;
    boss->colino = 60;
    actwk[0].xposi.w.h = 2800;
    set_egg6_field(boss, 23, 1);
    rakkabutu(boss);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 56, actwk[20].actno);

    reset_boss6_state();
    egg6_get_work(boss)->floor_index = 3;
    boss->colino = 0;
    rakkabutu(boss);
    TEST_ASSERT_EQ_INT(ctx, 12, boss->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 199, soundset_requests[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 63, actwk[20].actno);

    reset_boss6_state();
    set_egg6_field(boss, 23, 497);
    egg6_event(boss);
    TEST_ASSERT_EQ_INT(ctx, 178, soundset_requests[0]);
    set_egg6_field(boss, 23, 539);
    egg6_event(boss);
    TEST_ASSERT_TRUE(ctx, bossstart != 0);
    set_egg6_field(boss, 23, 839);
    egg6_get_work(boss)->floor_index = 1;
    egg6_event(boss);
    TEST_ASSERT_EQ_INT(ctx, 4, boss->r_no0);
}

static void test_additional_boss_hover_and_event_edges(test_context *ctx) {
    sprite_status *boss = &actwk[4];

    reset_boss6_state();
    egg6_get_work(boss)->flags = 0;
    egg6_hitchk(boss);
    TEST_ASSERT_EQ_INT(ctx, 0, soundset_count);

    reset_boss6_state();
    bossflag = 1;
    set_egg6_field(boss, 23, 0);
    egg6_ini(boss);
    TEST_ASSERT_EQ_INT(ctx, 0, boss->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, egg6_field(boss, 23));

    boss->actno = 42;
    egg6_none(boss);
    TEST_ASSERT_EQ_INT(ctx, 42, boss->actno);

    reset_boss6_state();
    egg6_get_work(boss)->floor_index = 0;
    boss->yposi.w.h = 1200;
    boss->yspeed.w = -256;
    egg6_up(boss);
    TEST_ASSERT_EQ_INT(ctx, 1199, boss->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, boss->r_no0);

    reset_boss6_state();
    actwk[0].yposi.w.h = 1500;
    egg6_get_work(boss)->floor_index = 1;
    egg6_hover(boss);
    TEST_ASSERT_EQ_INT(ctx, 10, boss->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 360, egg6_field(boss, 23));

    reset_boss6_state();
    actwk[0].yposi.w.h = 1500;
    egg6_get_work(boss)->floor_index = 0;
    set_egg6_field(boss, 23, 298);
    egg6_hover(boss);
    TEST_ASSERT_EQ_INT(ctx, 0, boss->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 299, egg6_field(boss, 23));

    egg6_hover(boss);
    TEST_ASSERT_EQ_INT(ctx, 8, boss->r_no0);
    TEST_ASSERT_EQ_INT(ctx, -256, boss->yspeed.w);
    TEST_ASSERT_EQ_INT(ctx, -16, egg6_field(boss, 27));

    reset_boss6_state();
    egg6_get_work(boss)->floor_index = 3;
    egg6_get_work(boss)->bob_angle = 4;
    boss->xposi.w.h = 100;
    stbRad = 1;
    fuwafuwa(boss);
    TEST_ASSERT_EQ_INT(ctx, 101, boss->xposi.w.h);

    reset_boss6_state();
    egg6_get_work(boss)->quake_timer = 2;
    egg6_jisin(boss);
    TEST_ASSERT_EQ_INT(ctx, -6, scralim_down);
    egg6_jisin(boss);
    TEST_ASSERT_EQ_INT(ctx, -8, scralim_down);

    reset_boss6_state();
    actwk[0].xposi.w.h = 2600;
    boss->r_no0 = 0;
    egg6boss(boss);
    TEST_ASSERT_EQ_INT(ctx, 0, patchg_count);
    TEST_ASSERT_EQ_INT(ctx, 0, actionsub_count);

    reset_boss6_state();
    boss->r_no0 = 20;
    boss->actno = 77;
    egg6boss(boss);
    TEST_ASSERT_EQ_INT(ctx, 1, patchg_count);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);

    reset_boss6_state();
    egg6_get_work(boss)->floor_index = 3;
    boss->colino = 60;
    set_egg6_field(boss, 23, 181);
    rakkabutu(boss);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 0, boss->r_no0);

    reset_boss6_state();
    set_egg6_field(boss, 23, 569);
    bossflag = 255;
    egg6_event(boss);
    TEST_ASSERT_EQ_INT(ctx, 223, bossflag);

    reset_boss6_state();
    set_egg6_field(boss, 23, 599);
    egg6_event(boss);
    TEST_ASSERT_EQ_INT(ctx, 1, egg6_get_work(boss)->release_flag);

    set_egg6_field(boss, 23, 629);
    egg6_event(boss);
    TEST_ASSERT_EQ_INT(ctx, 0, egg6_get_work(boss)->release_flag);
    TEST_ASSERT_EQ_INT(ctx, 32, bossflag & 32);
}

static void test_additional_spawn_failure_and_fragment_paths(test_context *ctx) {
    sprite_status *boss = &actwk[4];
    sprite_status *piece = &actwk[10];

    reset_boss6_state();
    egg6_get_work(boss)->floor_index = 1;
    actwkchk_fail_after = 0;
    make_toge(boss, 0);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[20].actno);

    reset_boss6_state();
    egg6_get_work(boss)->floor_index = 1;
    actwkchk_fail_after = 0;
    make_yuka(boss, 0);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[20].actno);

    reset_boss6_state();
    egg6_get_work(boss)->floor_index = 1;
    actwkchk_fail_after = 0;
    make_gareki(boss, 0);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[20].actno);

    reset_boss6_state();
    actwkchk_fail_after = 1;
    make_beam(0);
    TEST_ASSERT_EQ_INT(ctx, 2, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 64, actwk[20].actno);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[21].actno);

    reset_boss6_state();
    actwkchk_fail_after = 0;
    make_bakuha(boss);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 0, soundset_count);

    reset_boss6_state();
    actwkchk_fail_after = 0;
    make_meca(boss);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 0, egg6_field(boss, 25));

    reset_boss6_state();
    actwkchk_fail_after = 1;
    make_meca(boss);
    TEST_ASSERT_EQ_INT(ctx, 2, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 53, actwk[20].actno);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[21].actno);

    reset_boss6_state();
    actwkchk_fail_after = 2;
    make_meca(boss);
    TEST_ASSERT_EQ_INT(ctx, 3, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 53, actwk[20].actno);
    TEST_ASSERT_EQ_INT(ctx, 54, actwk[21].actno);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[22].actno);

    reset_boss6_state();
    piece->actno = 57;
    piece->r_no0 = 4;
    piece->actflg = 4;
    piece->sprpri = 3;
    piece->sprhsize = 4;
    piece->sprvsize = 4;
    piece->sproffset = 17472;
    piece->xposi.w.h = 1000;
    piece->yposi.w.h = 1200;
    next_alloc_index = 20;
    make_gareki4(piece);
    TEST_ASSERT_EQ_INT(ctx, 3, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 3, piece->patno);
    TEST_ASSERT_EQ_INT(ctx, 57, actwk[20].actno);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[20].patno);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[21].patno);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[22].patno);
    TEST_ASSERT_EQ_INT(ctx, 512, piece->xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, -448, piece->yspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 48, egg6_field(piece, 27));

    reset_boss6_state();
    actwkchk_fail_after = 1;
    piece->actno = 57;
    piece->r_no0 = 4;
    make_gareki4(piece);
    TEST_ASSERT_EQ_INT(ctx, 2, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 57, actwk[20].actno);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[21].actno);

    reset_boss6_state();
    piece->xposi.w.h = 3000;
    piece->yposi.w.h = 600;
    random_result = 0x01020304;
    make_bakuha2(piece);
    TEST_ASSERT_EQ_INT(ctx, 63, actwk[20].actno);
    TEST_ASSERT_EQ_INT(ctx, 3000, actwk[20].xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 592, actwk[20].yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, soundset_count);

    reset_boss6_state();
    piece->xposi.w.h = 3000;
    piece->yposi.w.h = 600;
    random_result = 0x0000FC01;
    make_bakuha2(piece);
    TEST_ASSERT_EQ_INT(ctx, -1023, actwk[20].xspeed.w);
}

static void test_additional_child_and_door_edges(test_context *ctx) {
    sprite_status *parent = &actwk[4];
    sprite_status *child = &actwk[20];
    sprite_status *door = &actwk[30];
    sprite_status *platform = &actwk[40];

    reset_boss6_state();
    set_egg6_field(child, 25, 4);
    parent->xposi.w.h = 700;
    parent->yposi.w.h = 900;
    egg6_get_work(parent)->state_timer = 2;
    child->r_no0 = 2;
    child->pattimm = 5;
    child->pattim = 1;
    child->patno = 0;
    egg6meca1(child);
    TEST_ASSERT_EQ_INT(ctx, 1, child->patno);
    TEST_ASSERT_EQ_INT(ctx, 2, child->pattimm);
    TEST_ASSERT_EQ_INT(ctx, 2, child->pattim);

    child->pattimm = 6;
    child->pattim = 1;
    child->patno = 3;
    egg6meca1(child);
    TEST_ASSERT_EQ_INT(ctx, 0, child->patno);
    TEST_ASSERT_EQ_INT(ctx, 3, child->pattimm);

    reset_boss6_state();
    set_egg6_field(child, 25, 4);
    parent->xposi.w.h = 900;
    parent->yposi.w.h = 500;
    child->mstno.b.h = 1;
    child->patno = 3;
    set_egg6_field(child, 23, 419);
    egg6bakuha(child);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);

    reset_boss6_state();
    set_egg6_field(child, 25, 4);
    parent->xposi.w.h = 900;
    parent->yposi.w.h = 500;
    child->mstno.b.h = 1;
    child->patno = 0;
    random_result = 0x00010002;
    egg6bakuha(child);
    TEST_ASSERT_EQ_INT(ctx, 1, patchg_count);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 63, actwk[20].actno);

    reset_boss6_state();
    egg6_get_work(child)->anim_timer = 5;
    egg6toge_02(child);
    TEST_ASSERT_EQ_INT(ctx, 0, actionsub_count);
    TEST_ASSERT_EQ_INT(ctx, 6, egg6_get_work(child)->anim_timer);

    egg6_get_work(child)->anim_timer = 9;
    egg6_get_work(child)->timer = 5;
    egg6toge_02(child);
    TEST_ASSERT_EQ_INT(ctx, 0, egg6_get_work(child)->anim_timer);
    TEST_ASSERT_EQ_INT(ctx, 0, frameout_count);

    reset_boss6_state();
    child->actno = 57;
    child->xspeed.w = 128;
    child->yspeed.w = 256;
    set_egg6_field(child, 26, 16);
    set_egg6_field(child, 27, 32);
    egg6gareki(child);
    TEST_ASSERT_EQ_INT(ctx, 2, child->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_EQ_INT(ctx, 0, frameout_count);
    emycol_d_result = 1;
    next_alloc_index = 30;
    egg6gareki(child);
    TEST_ASSERT_EQ_INT(ctx, 4, child->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 3, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 57, actwk[30].actno);
    egg6gareki(child);
    TEST_ASSERT_EQ_INT(ctx, 3, actionsub_count);

    reset_boss6_state();
    actwk[0].actno = 1;
    actwk[0].xposi.w.h = 100;
    actwk[0].yposi.w.h = 300;
    actwk[0].sprvsize = 14;
    platform->xposi.w.h = 100;
    platform->yposi.w.h = 280;
    platform->yspeed.w = 128;
    TEST_ASSERT_EQ_INT(ctx, -1, egg6_yuka_hasami_chk(platform));
    TEST_ASSERT_EQ_INT(ctx, 1, pcole_count);

    pcole_count = 0;
    actwk[0].cddat = 128;
    TEST_ASSERT_EQ_INT(ctx, -1, mydamage(platform));
    TEST_ASSERT_EQ_INT(ctx, 0, pcole_count);

    reset_boss6_state();
    actwk[0].cddat = 8;
    actwk[0].sprvsize = 14;
    platform->yposi.l = 400 << 16;
    platform->yspeed.w = 128;
    egg6_yuka_ride_on(platform);
    TEST_ASSERT_EQ_INT(ctx, 370, actwk[0].yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].yspeed.w);

    reset_boss6_state();
    actwk[0].actno = 1;
    actwk[0].cddat = 8;
    actwk[0].xposi.w.h = 100;
    actwk[0].yposi.w.h = 200;
    actwk[0].yspeed.w = -100;
    platform->xposi.w.h = 200;
    platform->yposi.w.h = 300;
    platform->yspeed.w = 128;
    ride_on_chk_result = 0;
    egg6_yuka_ridechk(platform);
    TEST_ASSERT_EQ_INT(ctx, -100, actwk[0].yspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 1, ride_on_chk_count);

    ride_on_chk_result = 1;
    egg6_yuka_ridechk(platform);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].yspeed.w);

    reset_boss6_state();
    door->userflag.b.h = 0;
    door->xposi.w.h = 2600;
    door->yposi.w.h = 100;
    egg6door(door);
    TEST_ASSERT_EQ_INT(ctx, 1, ride_on_chk_count);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);

    reset_boss6_state();
    door->userflag.b.h = 0;
    door->userflag.b.l = 0;
    door->yposi.w.h = 100;
    actwk[0].actno = 1;
    actwk[0].yposi.w.h = 100;
    actwk[0].r_no0 = 4;
    bossstart = 1;
    egg6door_beamer(door);
    TEST_ASSERT_EQ_INT(ctx, 1, pcole_count);

    pcole_count = 0;
    set_egg6_field(&actwk[0], 26, 1);
    egg6door_beamer(door);
    TEST_ASSERT_EQ_INT(ctx, 0, pcole_count);
}

static void test_remaining_boss6_edge_branches(test_context *ctx) {
    sprite_status *boss = &actwk[4];
    sprite_status *parent = &actwk[4];
    sprite_status *child = &actwk[20];
    sprite_status *platform = &actwk[30];
    sprite_status *door = &actwk[40];

    reset_boss6_state();
    set_egg6_field(boss, 23, 99);
    egg6_event(boss);
    TEST_ASSERT_EQ_INT(ctx, 100, egg6_field(boss, 23));

    reset_boss6_state();
    actwk[2].actno = 62;
    actwk[2].r_no0 = 4;
    set_egg6_field(boss, 23, 179);
    egg6_dead2(boss);
    TEST_ASSERT_EQ_INT(ctx, 180, egg6_field(boss, 23));
    TEST_ASSERT_EQ_INT(ctx, 8, actwk[2].r_no0);

    reset_boss6_state();
    generate_flag = 1;
    dead2_end(boss);
    TEST_ASSERT_EQ_INT(ctx, 1, sub_sync_count);
    TEST_ASSERT_EQ_INT(ctx, 28, sub_sync_requests[0]);

    reset_boss6_state();
    set_egg6_field(child, 25, 4);
    parent->xposi.w.h = 1000;
    parent->yposi.w.h = 700;
    egg6_get_work(parent)->state_timer = 0;
    child->r_no0 = 2;
    egg6meca1(child);
    TEST_ASSERT_EQ_INT(ctx, 636, child->yposi.w.h);

    egg6_get_work(parent)->state_timer = 2;
    child->pattim = 2;
    child->pattimm = 9;
    egg6meca1(child);
    TEST_ASSERT_EQ_INT(ctx, 1, child->pattim);

    child->pattim = 1;
    child->pattimm = 4;
    child->patno = 0;
    egg6meca1(child);
    TEST_ASSERT_EQ_INT(ctx, 2, child->pattimm);
    TEST_ASSERT_EQ_INT(ctx, 2, child->pattim);

    reset_boss6_state();
    set_egg6_field(platform, 28, 0);
    set_egg6_field(platform, 30, 0);
    set_egg6_field(platform, 27, 0);
    platform->yposi.w.h = 1000;
    egg6yuka_01(platform);
    TEST_ASSERT_EQ_INT(ctx, 1024, platform->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, platform->r_no0);

    reset_boss6_state();
    actwk[0].actno = 1;
    actwk[0].xposi.w.h = 120;
    actwk[0].yposi.w.h = 270;
    actwk[0].sprvsize = 14;
    platform->xposi.w.h = 100;
    platform->yposi.w.h = 300;
    platform->yspeed.w = 128;
    TEST_ASSERT_EQ_INT(ctx, 0, egg6_yuka_hasami_chk(platform));

    actwk[0].xposi.w.h = 130;
    actwk[0].yposi.w.h = 300;
    TEST_ASSERT_EQ_INT(ctx, -1, egg6_yuka_hasami_chk(platform));

    actwk[0].cddat = 0;
    actwk[0].mstno.b.h = 24;
    mydamage(platform);
    TEST_ASSERT_EQ_INT(ctx, 128, actwk[0].cddat & 128);

    reset_boss6_state();
    door->userflag.b.h = 0;
    door->yspeed.w = 512;
    set_egg6_field(door, 28, 16384);
    set_egg6_field(door, 23, 0);
    egg6door_move(door);
    TEST_ASSERT_EQ_INT(ctx, 0, door->r_no0);

    set_egg6_field(door, 23, 15872);
    egg6door_move(door);
    TEST_ASSERT_EQ_INT(ctx, 6, door->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 9, door->sprhsize);
    TEST_ASSERT_EQ_INT(ctx, 32, door->sprvsize);

    reset_boss6_state();
    door->userflag.b.l = 0;
    door->yposi.w.h = 100;
    actwk[0].actno = 1;
    actwk[0].yposi.w.h = 110;
    bossstart = 1;
    egg6door_beamer(door);
    TEST_ASSERT_EQ_INT(ctx, 0, pcole_count);

    actwk[0].yposi.w.h = 104;
    egg6door_beamer(door);
    TEST_ASSERT_EQ_INT(ctx, 1, pcole_count);

    reset_boss6_state();
    actwkchk_fail_after = 0;
    make_bakuha2(boss);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 0, soundset_count);
}

TEST_MAIN_BEGIN;
    test_floor_beam_color_and_motion_helpers(&ctx);
    test_start_initializes_boss_and_meca_parts(&ctx);
    test_hit_coliset_and_beam_control(&ctx);
    test_vertical_state_transitions_and_event_helpers(&ctx);
    test_spawn_helpers_and_projectiles(&ctx);
    test_child_actor_initializers_and_small_states(&ctx);
    test_yuka_door_damage_and_death_helpers(&ctx);
    test_beam_yuka_and_door_state_machines(&ctx);
    test_dead_state_helpers_and_timed_dead2_events(&ctx);
    test_event_wrappers_and_search_paths(&ctx);
    test_additional_boss_hover_and_event_edges(&ctx);
    test_additional_spawn_failure_and_fragment_paths(&ctx);
    test_additional_child_and_door_edges(&ctx);
    test_remaining_boss6_edge_branches(&ctx);
TEST_MAIN_END
