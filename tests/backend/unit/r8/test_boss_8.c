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
Sint16 scralim_left;
Sint16 scralim_n_left;
Sint16 scralim_right;
Sint16 scralim_n_right;

static int actionsub_count;
static sprite_status *actionsub_actor;
static int patchg_count;
static sprite_status *patchg_actor;
static Uint8 **patchg_table;
static int colchg_count;
static Uint8 *colchg_a3;
static Uint8 *colchg_a4;
static int fadein_count;
static int colorset_count;
static Sint32 colorset_values[8];
static int soundset_count;
static Sint16 soundset_values[8];
static int sub_sync_count;
static Sint16 sub_sync_values[8];
static int frameout_count;
static sprite_status *frameout_actor;
static int actwkchk_count;
static int actwkchk2_count;
static sprite_status *spawn_queue[16];
static int spawn_queue_count;
static int spawn_queue_index;
static Sint32 random_values[8];
static int random_count;
static int random_index;
static Sint16 sinset_sin;
static Sint16 sinset_cos;

void actionsub(sprite_status *pActwk);
void patchg(sprite_status *patchgwk, Uint8 **pat_dat);
void colchg_boss8(Uint8 *a3, Uint8 *a4);
void fadein_boss8(void);
void colorset2(Sint32 ColorNo);
void soundset(Sint16 ReqNo);
void sub_sync(Sint16 ReqNo);
void frameout(sprite_status *pActwk);
Sint32 actwkchk(sprite_status **ppActwk);
Sint32 actwkchk2(sprite_status *pActwk, sprite_status **ppNewActwk);
Sint32 random(void);
void sinset(unsigned char angle, short *sin, short *cos);
void scoreup(Uint32 lScore);

#include "src/r8/boss_8.c"

void actionsub(sprite_status *pActwk) {
    ++actionsub_count;
    actionsub_actor = pActwk;
}

void patchg(sprite_status *patchgwk, Uint8 **pat_dat) {
    ++patchg_count;
    patchg_actor = patchgwk;
    patchg_table = pat_dat;
}

void colchg_boss8(Uint8 *a3, Uint8 *a4) {
    ++colchg_count;
    colchg_a3 = a3;
    colchg_a4 = a4;
}

void fadein_boss8(void) { ++fadein_count; }

void colorset2(Sint32 ColorNo) {
    if (colorset_count < 8)
        colorset_values[colorset_count] = ColorNo;
    ++colorset_count;
}

void soundset(Sint16 ReqNo) {
    if (soundset_count < 8)
        soundset_values[soundset_count] = ReqNo;
    ++soundset_count;
}

void sub_sync(Sint16 ReqNo) {
    if (sub_sync_count < 8)
        sub_sync_values[sub_sync_count] = ReqNo;
    ++sub_sync_count;
}

void frameout(sprite_status *pActwk) {
    ++frameout_count;
    frameout_actor = pActwk;
    pActwk->actno = 0;
}

Sint32 actwkchk(sprite_status **ppActwk) {
    ++actwkchk_count;
    if (spawn_queue_index >= spawn_queue_count) {
        *ppActwk = 0;
        return 1;
    }
    *ppActwk = spawn_queue[spawn_queue_index++];
    return 0;
}

Sint32 actwkchk2(sprite_status *pActwk, sprite_status **ppNewActwk) {
    (void)pActwk;
    ++actwkchk2_count;
    if (spawn_queue_index >= spawn_queue_count) {
        *ppNewActwk = 0;
        return 1;
    }
    *ppNewActwk = spawn_queue[spawn_queue_index++];
    return 0;
}

Sint32 random(void) {
    if (random_index < random_count)
        return random_values[random_index++];
    return 0;
}

void sinset(unsigned char angle, short *sin, short *cos) {
    (void)angle;
    *sin = sinset_sin;
    *cos = sinset_cos;
}

void scoreup(Uint32 lScore) { (void)lScore; }

static void queue_spawn(sprite_status *actor) {
    spawn_queue[spawn_queue_count++] = actor;
}

static void reset_boss8_state(void) {
    memset(actwk, 0, sizeof(actwk));
    memset(spawn_queue, 0, sizeof(spawn_queue));
    memset(colorset_values, 0, sizeof(colorset_values));
    memset(soundset_values, 0, sizeof(soundset_values));
    memset(sub_sync_values, 0, sizeof(sub_sync_values));
    memset(random_values, 0, sizeof(random_values));
    bossflag = 0;
    bossstart = 0;
    scralim_left = 0;
    scralim_n_left = 0;
    scralim_right = 0;
    scralim_n_right = 0;
    actionsub_count = 0;
    actionsub_actor = 0;
    patchg_count = 0;
    patchg_actor = 0;
    patchg_table = 0;
    colchg_count = 0;
    colchg_a3 = 0;
    colchg_a4 = 0;
    fadein_count = 0;
    colorset_count = 0;
    soundset_count = 0;
    sub_sync_count = 0;
    frameout_count = 0;
    frameout_actor = 0;
    actwkchk_count = 0;
    actwkchk2_count = 0;
    spawn_queue_count = 0;
    spawn_queue_index = 0;
    random_count = 0;
    random_index = 0;
    sinset_sin = 0;
    sinset_cos = 0;
}

static void test_egg8_init_sets_boss_sprite_basics(test_context *ctx) {
    sprite_status *egg = &actwk[10];
    sprite_status *meca = &actwk[11];

    reset_boss8_state();
    egg->yposi.w.h = 512;

    egg8_ini(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 2, egg->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 4, egg->actflg);
    TEST_ASSERT_EQ_INT(ctx, 5, egg->sprpri);
    TEST_ASSERT_EQ_INT(ctx, 24, egg->sprhsize);
    TEST_ASSERT_EQ_INT(ctx, 16, egg->sprvsize);
    TEST_ASSERT_EQ_INT(ctx, 990, egg->sproffset);
    TEST_ASSERT_TRUE(ctx, egg->patbase == egg8_pat);
    TEST_ASSERT_EQ_INT(ctx, 4, boss8_get_work(egg)->hane_count);
    TEST_ASSERT_EQ_INT(ctx, 256, egg->yposi.w.h);
}

static void test_egg8_scrset_clamps_then_starts_boss(test_context *ctx) {
    sprite_status *egg = &actwk[10];
    sprite_status *meca = &actwk[11];

    reset_boss8_state();
    scralim_left = scralim_n_left = 0;
    actwk[0].xposi.w.h = 200;

    egg8_scrset(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 40, scralim_left);
    TEST_ASSERT_EQ_INT(ctx, 40, scralim_n_left);
    TEST_ASSERT_EQ_INT(ctx, 3680, scralim_right);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk2_count);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    egg->xposi.w.h = 3840;
    egg->yposi.w.h = 416;
    boss8_get_work(egg)->hane_count = 2;
    actwk[0].xposi.w.h = 3840;
    queue_spawn(meca);

    egg8_scrset(egg, 0);

    TEST_ASSERT_EQ_INT(ctx, 8, bossflag);
    TEST_ASSERT_EQ_INT(ctx, 8, bossstart);
    TEST_ASSERT_EQ_INT(ctx, 4, egg->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 11, boss8_get_work(egg)->parent_index);
    TEST_ASSERT_EQ_INT(ctx, 10, boss8_get_work(meca)->parent_index);
    TEST_ASSERT_EQ_INT(ctx, 65, meca->actno);
    TEST_ASSERT_EQ_INT(ctx, 106496, boss8_get_work(egg)->x_velocity);
    TEST_ASSERT_EQ_INT(ctx, 57344, sprite_status_get_xspeed_yspeed(egg));
    TEST_ASSERT_EQ_INT(ctx, 3, boss8_get_work(meca)->roll_speed);
}

static void test_egg8_open_sequence_cases(test_context *ctx) {
    sprite_status *egg = &actwk[10];
    sprite_status *meca = &actwk[11];

    reset_boss8_state();

    egg8_open1(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 5, egg->mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 3, colorset_count);
    TEST_ASSERT_EQ_INT(ctx, 7, colorset_values[0]);
    TEST_ASSERT_EQ_INT(ctx, 8, colorset_values[1]);
    TEST_ASSERT_EQ_INT(ctx, 9, colorset_values[2]);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    boss8_get_work(egg)->step = 1;
    egg->yposi.w.h = 160;
    boss8_get_work(egg)->timer = 239;

    egg8_open1(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 416, egg->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, sub_sync_count);
    TEST_ASSERT_EQ_INT(ctx, 104, sub_sync_values[0]);
    TEST_ASSERT_EQ_INT(ctx, 128, bossstart);
    TEST_ASSERT_EQ_INT(ctx, 2, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 2, egg->mstno.b.h);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    boss8_get_work(egg)->step = 2;

    egg8_open1(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 1, fadein_count);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->timer_low);

    boss8_get_work(egg)->timer_low = 62;
    egg8_open1(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 3, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->timer_low);
    TEST_ASSERT_EQ_INT(ctx, 0, egg->mstno.b.h);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    boss8_get_work(egg)->step = 3;
    boss8_get_work(egg)->timer_low = 39;

    egg8_open1(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->flags & 1);
    TEST_ASSERT_EQ_INT(ctx, 8, boss8_get_work(egg)->flags & 8);
    TEST_ASSERT_EQ_INT(ctx, 50, egg->colino);
    TEST_ASSERT_EQ_INT(ctx, 2, egg->colicnt);
    TEST_ASSERT_EQ_INT(ctx, 51, meca->colino);
    TEST_ASSERT_EQ_INT(ctx, 2, meca->colicnt);
}

static void test_egg8_movement_and_hane_control_helpers(test_context *ctx) {
    sprite_status *egg = &actwk[10];
    sprite_status *meca = &actwk[11];

    reset_boss8_state();
    egg->xposi.l = 100 << 16;
    boss8_get_work(egg)->x_velocity = 0x20000;
    boss8_get_work(egg)->target_pos = 102;

    egg8_move_r(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 102, egg->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->flags & 1);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    egg->xposi.l = 100 << 16;
    boss8_get_work(egg)->x_velocity = 0x20000;
    boss8_get_work(egg)->target_pos = 98;

    egg8_move_l(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 98, egg->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->flags & 1);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    boss8_get_work(egg)->hane_count = 2;

    egg8_hane_stop(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 16, boss8_get_work(meca)->flags & 16);
    TEST_ASSERT_EQ_INT(ctx, 8, boss8_get_work(meca)->hane_target);

    boss8_get_work(meca)->hane_count = 128;
    egg8_hane_stop(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->flags & 1);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];

    egg8_hane_chg2(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 1, soundset_count);
    TEST_ASSERT_EQ_INT(ctx, 201, soundset_values[0]);
    TEST_ASSERT_EQ_INT(ctx, 6, meca->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->flags & 1);
}

static void test_egg8_wait_vertical_escape_and_center_move(test_context *ctx) {
    sprite_status *egg = &actwk[10];
    sprite_status *meca = &actwk[11];

    reset_boss8_state();
    boss8_get_work(egg)->wait_time = 2;
    boss8_get_work(egg)->timer_low = 1;

    egg8_wait(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->timer_low);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->flags & 1);

    reset_boss8_state();
    egg = &actwk[10];
    egg->yposi.l = 100 << 16;
    sprite_status_set_xspeed_yspeed(egg, 0x30000);
    boss8_get_work(egg)->target_pos = 103;

    egg8_move_d(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 103, egg->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->flags & 1);

    reset_boss8_state();
    egg = &actwk[10];
    egg->yposi.l = 100 << 16;
    sprite_status_set_xspeed_yspeed(egg, 0x30000);
    boss8_get_work(egg)->target_pos = 97;

    egg8_move_u(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 97, egg->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->flags & 1);

    reset_boss8_state();
    egg = &actwk[10];
    egg->xposi.w.h = 3800;

    egg8_move_c(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->step & 1);
    TEST_ASSERT_EQ_INT(ctx, 3964, boss8_get_work(egg)->target_x);

    reset_boss8_state();
    egg = &actwk[10];
    egg->xposi.w.h = 3900;

    egg8_move_c(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->step & 1);
    TEST_ASSERT_EQ_INT(ctx, 3716, boss8_get_work(egg)->target_x);

    reset_boss8_state();
    egg = &actwk[10];
    egg->yposi.l = 420 << 16;
    boss8_get_work(egg)->target_pos = 418;

    egg8_move_escu(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 8, meca->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 418, egg->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->flags & 1);
}

static void test_egg8_hane_state_changes(test_context *ctx) {
    sprite_status *egg = &actwk[10];
    sprite_status *meca = &actwk[11];

    reset_boss8_state();
    boss8_get_work(meca)->hane_count = 0;

    egg8_hane_roll(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 16, boss8_get_work(meca)->flags & 16);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->flags & 1);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];

    egg8_hane_chg1(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 4, meca->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 160, boss8_get_work(meca)->hane_count);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->flags & 1);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];

    egg8_hane_chg3(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 8, meca->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->flags & 1);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];

    egg8_hane_reset(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 12, meca->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->flags & 1);
}

static void test_hane_roll_stop_and_futa_helpers(test_context *ctx) {
    sprite_status *meca = &actwk[11];

    reset_boss8_state();
    boss8_get_work(meca)->roll_speed = 1;
    boss8_get_work(meca)->angle = 9;

    hane_stop(meca);

    TEST_ASSERT_EQ_INT(ctx, 32, boss8_get_work(meca)->hane_count & 32);
    TEST_ASSERT_EQ_INT(ctx, 64, boss8_get_work(meca)->flags & 64);

    reset_boss8_state();
    meca = &actwk[11];
    boss8_get_work(meca)->roll_speed = 1;
    boss8_get_work(meca)->hane_count = 32;
    boss8_get_work(meca)->angle = 5;

    hane_roll(meca);

    TEST_ASSERT_EQ_INT(ctx, 2, meca->patno);
    TEST_ASSERT_EQ_INT(ctx, 64, boss8_get_work(meca)->flags & 64);

    boss8_get_work(meca)->angle = 11;
    futa_roll(meca);

    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(meca)->angle);
    TEST_ASSERT_EQ_INT(ctx, 1, meca->patno);

    reset_boss8_state();
    meca = &actwk[11];
    boss8_get_work(meca)->flags = 16;
    boss8_get_work(meca)->hane_count = 64;
    boss8_get_work(meca)->roll_speed = 1;

    hane_ctrl(meca);

    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(meca)->hane_count & 64);
    TEST_ASSERT_EQ_INT(ctx, 64, boss8_get_work(meca)->flags & 64);
}

static void test_egg8meca_init_spawns_hanes_and_transitions(test_context *ctx) {
    sprite_status *meca = &actwk[11];
    sprite_status *egg = &actwk[10];
    sprite_status *h0 = &actwk[20];
    sprite_status *h1 = &actwk[21];
    sprite_status *h2 = &actwk[22];
    sprite_status *h3 = &actwk[23];

    reset_boss8_state();
    queue_spawn(h0);
    queue_spawn(h1);
    queue_spawn(h2);
    queue_spawn(h3);

    egg8meca_ini(meca, egg);

    TEST_ASSERT_EQ_INT(ctx, 2, meca->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 4, meca->actflg);
    TEST_ASSERT_EQ_INT(ctx, 36, meca->sprhsize);
    TEST_ASSERT_EQ_INT(ctx, 36, meca->sprvsize);
    TEST_ASSERT_TRUE(ctx, meca->patbase == egg8meca_pat);
    TEST_ASSERT_EQ_INT(ctx, 4, actwkchk2_count);
    TEST_ASSERT_EQ_INT(ctx, 64, h0->actno);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(h0)->wait_time);
    TEST_ASSERT_EQ_INT(ctx, 0, h0->patno);
    TEST_ASSERT_EQ_INT(ctx, 64, h3->actno);
    TEST_ASSERT_EQ_INT(ctx, 3, boss8_get_work(h3)->wait_time);
    TEST_ASSERT_EQ_INT(ctx, 12, h3->patno);
    TEST_ASSERT_EQ_INT(ctx, 160, boss8_get_work(meca)->hane_count);

    reset_boss8_state();
    meca = &actwk[11];
    egg = &actwk[10];
    egg->xposi.w.h = 3840;
    egg->yposi.w.h = 416;
    boss8_get_work(meca)->step = 1;
    boss8_get_work(meca)->target_pos = 250;

    egg8meca_chg2(meca, egg);

    TEST_ASSERT_EQ_INT(ctx, 256, boss8_get_work(meca)->target_pos);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(meca)->step);
    TEST_ASSERT_EQ_INT(ctx, 2, meca->r_no0);
}

static void test_egg8meca_follow_spin_and_reset_variants(test_context *ctx) {
    sprite_status *meca = &actwk[11];
    sprite_status *egg = &actwk[10];

    reset_boss8_state();
    egg->xposi.w.h = 3840;
    egg->yposi.w.h = 416;
    boss8_get_work(meca)->roll_speed = 1;

    egg8meca_normal(meca, egg);

    TEST_ASSERT_EQ_INT(ctx, 3840, meca->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 416, meca->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, meca->patno);

    reset_boss8_state();
    meca = &actwk[11];
    egg = &actwk[10];
    egg->xposi.w.h = 3840;
    egg->yposi.w.h = 416;
    boss8_get_work(meca)->angle = 5;

    egg8meca_spin(meca, egg);

    TEST_ASSERT_EQ_INT(ctx, 2, meca->patno);

    reset_boss8_state();
    meca = &actwk[11];
    egg = &actwk[10];
    egg->xposi.w.h = 3840;
    egg->yposi.w.h = 416;
    boss8_get_work(meca)->target_pos = 2;

    egg8meca_chg3(meca, egg);

    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(meca)->target_pos);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(meca)->step);
    TEST_ASSERT_EQ_INT(ctx, 2, meca->r_no0);

    reset_boss8_state();
    meca = &actwk[11];
    egg = &actwk[10];
    egg->xposi.w.h = 3840;
    egg->yposi.w.h = 416;
    boss8_get_work(egg)->hane_count = 2;
    boss8_get_work(meca)->hit_timer = 2;
    actwk[0].xposi.w.h = 5;
    actwk[0].pattim = 2;

    egg8meca_fire(meca, egg);

    TEST_ASSERT_EQ_INT(ctx, 24, boss8_get_work(meca)->color_b);
    TEST_ASSERT_EQ_INT(ctx, 2, meca->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 16, boss8_get_work(meca)->flags & 16);

    reset_boss8_state();
    meca = &actwk[11];
    egg = &actwk[10];
    egg->xposi.w.h = 3840;
    egg->yposi.w.h = 416;
    boss8_get_work(meca)->step = 1;
    boss8_get_work(meca)->target_pos = -1;

    egg8meca_hane_reset(meca, egg);

    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(meca)->target_pos);
    TEST_ASSERT_EQ_INT(ctx, 2, meca->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(meca)->flags & 8);

    reset_boss8_state();
    meca = &actwk[11];
    egg = &actwk[10];
    egg->xposi.w.h = 3800;
    egg->yposi.w.h = 400;

    egg8meca_dead(meca, egg);
    TEST_ASSERT_EQ_INT(ctx, 3800, meca->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 400, meca->yposi.w.h);

    egg->yposi.w.h = 380;
    egg8meca_tobi(meca, egg);
    TEST_ASSERT_EQ_INT(ctx, 256, boss8_get_work(meca)->target_pos);
}

static void test_egg8hibana_tracks_meca_animates_and_expires(test_context *ctx) {
    sprite_status *hibana = &actwk[12];
    sprite_status *meca = &actwk[11];

    reset_boss8_state();
    meca->xposi.w.h = 100;
    meca->yposi.w.h = 200;
    boss8_get_work(hibana)->parent_index = 11;

    egg8hibana_ini(hibana);

    TEST_ASSERT_EQ_INT(ctx, 2, hibana->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 4, hibana->actflg);
    TEST_ASSERT_TRUE(ctx, hibana->patbase == egg8hibana_pat);
    TEST_ASSERT_EQ_INT(ctx, 84, hibana->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 200, hibana->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(hibana)->timer_low);

    boss8_get_work(hibana)->angle = 3;
    egg8hibana_1(hibana);

    TEST_ASSERT_EQ_INT(ctx, 4, boss8_get_work(hibana)->step);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(hibana)->angle);
    TEST_ASSERT_EQ_INT(ctx, 1, hibana->patno);

    boss8_get_work(hibana)->timer_low = 149;
    egg8hibana_1(hibana);

    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actor == hibana);
}

static void test_public_wrappers_dispatch_and_draw(test_context *ctx) {
    sprite_status *egg = &actwk[10];
    sprite_status *meca = &actwk[11];
    sprite_status *hane = &actwk[12];
    sprite_status *hibana = &actwk[13];

    reset_boss8_state();
    boss8_get_work(egg)->parent_index = 11;
    egg->r_no0 = 6;
    boss8_get_work(egg)->wait_time = 1;

    egg8(egg);

    TEST_ASSERT_EQ_INT(ctx, 1, patchg_count);
    TEST_ASSERT_TRUE(ctx, patchg_actor == egg);
    TEST_ASSERT_TRUE(ctx, patchg_table == egg8_pchg);
    TEST_ASSERT_EQ_INT(ctx, 1, colchg_count);
    TEST_ASSERT_TRUE(ctx, colchg_a3 == &boss8_get_work(egg)->hit_timer);
    TEST_ASSERT_TRUE(ctx, colchg_a4 == &boss8_get_work(egg)->color_a);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_TRUE(ctx, actionsub_actor == egg);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    boss8_get_work(meca)->parent_index = 10;
    meca->r_no0 = 2;
    egg->xposi.w.h = 300;
    egg->yposi.w.h = 400;

    egg8meca(meca);

    TEST_ASSERT_EQ_INT(ctx, 300, meca->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 400, meca->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_TRUE(ctx, actionsub_actor == meca);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    hane = &actwk[12];
    boss8_get_work(hane)->parent_index = 11;
    boss8_get_work(meca)->parent_index = 10;
    boss8_get_work(meca)->action_index = 128;
    hane->r_no0 = 2;
    meca->xposi.w.h = 120;
    meca->yposi.w.h = 220;

    egg8hane(hane);

    TEST_ASSERT_EQ_INT(ctx, 4, hane->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 120, hane->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 180, hane->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_TRUE(ctx, actionsub_actor == hane);

    reset_boss8_state();
    meca = &actwk[11];
    hibana = &actwk[13];
    boss8_get_work(hibana)->parent_index = 11;
    hibana->r_no0 = 2;
    meca->xposi.w.h = 200;
    meca->yposi.w.h = 300;

    egg8hibana(hibana);

    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_TRUE(ctx, actionsub_actor == hibana);
    TEST_ASSERT_EQ_INT(ctx, 184, hibana->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 300, hibana->yposi.w.h);
}

static void test_egg8_fire_spin_and_target_helpers(test_context *ctx) {
    sprite_status *egg = &actwk[10];
    sprite_status *meca = &actwk[11];

    reset_boss8_state();
    egg8_hane_fire(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 10, meca->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(meca)->hit_timer);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(meca)->color_a);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    boss8_get_work(egg)->step = 1;
    egg->yposi.l = 320 << 16;
    sprite_status_set_xspeed_yspeed(egg, 0x40000);

    egg8_hane_fire(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 2, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 316, egg->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->flags & 1);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    boss8_get_work(egg)->step = 2;
    boss8_get_work(egg)->hane_count = 3;
    boss8_get_work(meca)->hit_timer = 6;

    egg8_hane_fire(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->flags & 1);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    boss8_get_work(egg)->hane_count = 4;
    egg->yposi.l = 410 << 16;
    sprite_status_set_xspeed_yspeed(egg, 0x60000);

    egg8_spin_d(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 416, egg->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 16, boss8_get_work(meca)->flags & 16);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(meca)->hane_count & 128);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    boss8_get_work(egg)->step = 5;
    egg->r_no0 = 34;
    meca->r_no0 = 14;

    egg8_spin_r(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 2, meca->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->flags & 1);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    boss8_get_work(egg)->hane_count = 4;
    egg->xposi.w.h = 4005;
    actwk[0].xposi.w.h = 3900;

    egg8_spin_l(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 14, meca->r_no0);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    boss8_get_work(egg)->step = 2;
    boss8_get_work(egg)->hane_count = 4;
    boss8_get_work(egg)->timer_low = 19;
    egg->xposi.w.h = 3800;
    egg->yposi.w.h = 320;

    egg8_spin_r(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 3826, egg->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 316, egg->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 32, boss8_get_work(meca)->flags & 32);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    egg->xposi.w.h = 3800;
    egg->yposi.w.h = 350;
    boss8_get_work(egg)->hane_count = 2;
    actwk[0].xposi.w.h = 3820;

    egg8target_ini(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->step & 1);
    TEST_ASSERT_EQ_INT(ctx, 6, boss8_get_work(meca)->hane_target);
    TEST_ASSERT_TRUE(ctx, boss8_get_work(egg)->x_velocity > 0);
    TEST_ASSERT_TRUE(ctx, sprite_status_get_xspeed_yspeed(egg) > 0);

    boss8_get_work(meca)->hane_count = 128;
    egg8target_roll(egg, meca);
    TEST_ASSERT_EQ_INT(ctx, 2, boss8_get_work(egg)->step & 2);

    boss8_get_work(egg)->x_velocity = 0x10000;
    sprite_status_set_xspeed_yspeed(egg, 0x20000);
    egg->xposi.l = 3800 << 16;
    egg->yposi.l = 378 << 16;
    boss8_get_work(meca)->hane_target = 0;
    egg8target_move(egg, meca);
    TEST_ASSERT_EQ_INT(ctx, 380, egg->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 4, boss8_get_work(egg)->step & 4);
    TEST_ASSERT_TRUE(ctx, boss8_get_work(egg)->x_velocity < 0);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    boss8_get_work(egg)->hane_count = 3;
    egg->xposi.l = 3800 << 16;
    egg->yposi.l = 318 << 16;
    boss8_get_work(egg)->x_velocity = -0x10000;
    sprite_status_set_xspeed_yspeed(egg, -0x30000);

    egg8_targetreset(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 316, egg->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->flags & 1);
    TEST_ASSERT_EQ_INT(ctx, 4, boss8_get_work(meca)->roll_speed);
}

static void test_egg8_tobi_bom_action_and_collision_helpers(test_context *ctx) {
    sprite_status *egg = &actwk[10];
    sprite_status *meca = &actwk[11];
    sprite_status *spawn0 = &actwk[20];
    sprite_status *spawn1 = &actwk[21];

    reset_boss8_state();
    egg->xposi.w.h = 100;
    egg->yposi.w.h = 200;
    boss8_get_work(egg)->angle = 7;
    boss8_get_work(egg)->step = 1;
    boss8_get_work(egg)->target_x = 2;
    boss8_get_work(egg)->target_y = 3;
    sinset_sin = 128;
    sinset_cos = 256;

    tobi_set(egg);

    TEST_ASSERT_EQ_INT(ctx, 139, egg->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 145, egg->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 41, boss8_get_work(egg)->target_x);
    TEST_ASSERT_EQ_INT(ctx, -52, boss8_get_work(egg)->target_y);
    TEST_ASSERT_EQ_INT(ctx, 2352, boss8_get_work(egg)->timer);

    reset_boss8_state();
    egg = &actwk[10];
    spawn0 = &actwk[20];
    queue_spawn(spawn0);
    boss8_get_work(egg)->timer_low = 2;
    random_values[0] = 0x00050007;
    random_count = 1;

    bom_set(egg);

    TEST_ASSERT_EQ_INT(ctx, 24, spawn0->actno);
    TEST_ASSERT_EQ_INT(ctx, 255, spawn0->r_no1);
    TEST_ASSERT_EQ_INT(ctx, -1, spawn0->userflag.b.l);
    TEST_ASSERT_EQ_INT(ctx, 3845, spawn0->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 391, spawn0->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 158, soundset_values[0]);

    reset_boss8_state();
    egg = &actwk[10];
    spawn0 = &actwk[20];
    spawn1 = &actwk[21];
    queue_spawn(spawn0);
    queue_spawn(spawn1);
    egg->xposi.w.h = 3840;
    egg->yposi.w.h = 416;

    make_hibana(egg);

    TEST_ASSERT_EQ_INT(ctx, 66, spawn0->actno);
    TEST_ASSERT_EQ_INT(ctx, 10, boss8_get_work(spawn0)->parent_index);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(spawn0)->hane_mode);
    TEST_ASSERT_EQ_INT(ctx, 66, spawn1->actno);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(spawn1)->hane_mode);

    reset_boss8_state();
    egg = &actwk[10];
    boss8_get_work(egg)->hane_count = 1;
    boss8_get_work(egg)->action_index = 12;

    next_action(egg);

    TEST_ASSERT_EQ_INT(ctx, 18, egg->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 316, boss8_get_work(egg)->target_pos);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->action_index);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    spawn0 = &actwk[20];
    spawn1 = &actwk[21];
    queue_spawn(spawn0);
    queue_spawn(spawn1);
    egg->xposi.w.h = 3840;
    egg->yposi.w.h = 416;
    boss8_get_work(egg)->flags = 8;
    boss8_get_work(egg)->hane_count = 4;
    actwk[0].xposi.w.h = 3800;
    boss8_get_work(meca)->hane_base_patno = 2;

    egg8_colichk(egg, &actwk[0], meca);

    TEST_ASSERT_EQ_INT(ctx, 3, boss8_get_work(egg)->hane_count);
    TEST_ASSERT_EQ_INT(ctx, 2, boss8_get_work(meca)->action_index);
    TEST_ASSERT_EQ_INT(ctx, 172, soundset_values[0]);
    TEST_ASSERT_EQ_INT(ctx, 66, spawn0->actno);
    TEST_ASSERT_EQ_INT(ctx, 150, boss8_get_work(egg)->hit_flash);
    TEST_ASSERT_EQ_INT(ctx, 120, boss8_get_work(meca)->hit_flash);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    boss8_get_work(egg)->flags = 8;
    egg->r_no0 = 30;

    egg8_colichk(egg, &actwk[0], meca);

    TEST_ASSERT_EQ_INT(ctx, 50, egg->colino);
    TEST_ASSERT_EQ_INT(ctx, 2, egg->colicnt);
    TEST_ASSERT_EQ_INT(ctx, 51, meca->colino);
    TEST_ASSERT_EQ_INT(ctx, 2, meca->colicnt);

    reset_boss8_state();
    egg = &actwk[10];
    player_work_get(&actwk[0])->damage_invulnerability_timer = 1;
    egg8_warai_chk(egg);
    TEST_ASSERT_EQ_INT(ctx, 120, boss8_get_work(egg)->hit_flash);
    TEST_ASSERT_EQ_INT(ctx, 4, egg->mstno.b.h);
}

static void test_egg8_tobi_death_and_escape_sequences(test_context *ctx) {
    sprite_status *egg = &actwk[10];
    sprite_status *meca = &actwk[11];
    sprite_status *spawn0 = &actwk[20];

    reset_boss8_state();
    egg->xposi.l = 3800 << 16;
    egg->yposi.l = 300 << 16;
    boss8_get_work(egg)->x_velocity = 0x30000;
    sprite_status_set_xspeed_yspeed(egg, 0x30000);
    actwk[0].xposi.w.h = 3700;

    egg8_tobi_d(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 3964, boss8_get_work(egg)->target_pos);

    egg->xposi.w.h = 3964;
    egg8_tobi_d(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 3, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 16, boss8_get_work(meca)->flags & 16);

    boss8_get_work(meca)->hane_count = 128;
    egg8_tobi_d(egg, meca);
    TEST_ASSERT_EQ_INT(ctx, 4, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 312, boss8_get_work(egg)->target_pos);

    egg->yposi.l = 310 << 16;
    sprite_status_set_xspeed_yspeed(egg, 0x30000);
    egg8_tobi_d(egg, meca);
    TEST_ASSERT_EQ_INT(ctx, 5, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->target_y);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    egg->xposi.w.h = 3700;

    egg8_tobi(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 2, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 18, meca->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 2, boss8_get_work(meca)->hane_mode);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    egg->xposi.w.h = 4005;
    boss8_get_work(egg)->step = 2;
    boss8_get_work(egg)->timer = 32760;
    sinset_sin = 0;
    sinset_cos = 256;

    egg8_tobi(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 3, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 180, soundset_values[0]);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    boss8_get_work(egg)->step = 5;
    meca->r_no0 = 18;

    egg8_tobi(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->flags & 1);
    TEST_ASSERT_EQ_INT(ctx, 2, meca->r_no0);

    reset_boss8_state();
    egg = &actwk[10];
    boss8_get_work(egg)->step = 1;
    boss8_get_work(egg)->timer = 16000;

    egg8_tobi_u(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->flags & 1);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->target_y);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    spawn0 = &actwk[20];
    queue_spawn(spawn0);
    boss8_get_work(egg)->timer_low = 89;

    egg8_dead(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 52, egg->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 67, spawn0->actno);
    TEST_ASSERT_EQ_INT(ctx, 3656, spawn0->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 460, spawn0->yposi.w.h);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    egg->xposi.l = 4063 << 16;
    egg->yposi.w.h = 300;
    boss8_get_work(egg)->x_velocity = 0x10000;
    sinset_sin = 128;

    egg8_esc(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 54, egg->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, bossstart);
    TEST_ASSERT_EQ_INT(ctx, 11, colorset_values[0]);

    reset_boss8_state();
    egg = &actwk[10];
    spawn0 = &actwk[20];
    queue_spawn(spawn0);
    boss8_get_work(egg)->timer_low = 1;
    random_values[0] = 0x00010002;
    random_count = 1;

    egg8_esc2(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 2, boss8_get_work(egg)->timer_low);
    TEST_ASSERT_EQ_INT(ctx, 24, spawn0->actno);
}

static void test_egg8hane_state_machine_helpers(test_context *ctx) {
    sprite_status *hane = &actwk[12];
    sprite_status *meca = &actwk[11];
    sprite_status *egg = &actwk[10];

    reset_boss8_state();
    meca->xposi.w.h = 100;
    meca->yposi.w.h = 200;
    boss8_get_work(meca)->hane_mode = 0;

    TEST_ASSERT_EQ_INT(ctx, 1, egg8hane_ini(hane, meca, egg));
    TEST_ASSERT_EQ_INT(ctx, 2, hane->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 4, hane->actflg);
    TEST_ASSERT_EQ_INT(ctx, 32, hane->sprhsize);
    TEST_ASSERT_EQ_INT(ctx, 0, hane->colino);

    reset_boss8_state();
    hane = &actwk[12];
    meca = &actwk[11];
    egg = &actwk[10];
    boss8_get_work(hane)->flags = 8;

    TEST_ASSERT_EQ_INT(ctx, -1, egg8hane_normal(hane, meca, egg));
    TEST_ASSERT_EQ_INT(ctx, 10, hane->r_no0);

    reset_boss8_state();
    hane = &actwk[12];
    meca = &actwk[11];
    egg = &actwk[10];
    meca->r_no0 = 10;
    hane->patno = 0;

    TEST_ASSERT_EQ_INT(ctx, 1, egg8hane_normal(hane, meca, egg));
    TEST_ASSERT_EQ_INT(ctx, 6, hane->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(meca)->hit_timer);

    reset_boss8_state();
    hane = &actwk[12];
    meca = &actwk[11];
    egg = &actwk[10];
    boss8_get_work(meca)->hane_mode = 0;
    boss8_get_work(meca)->flags = 64;
    hane->patno = 0;

    TEST_ASSERT_EQ_INT(ctx, 1, egg8hane_normal(hane, meca, egg));
    TEST_ASSERT_EQ_INT(ctx, 15, hane->patno);
    TEST_ASSERT_EQ_INT(ctx, 1, soundset_count);

    reset_boss8_state();
    hane = &actwk[12];
    meca = &actwk[11];
    egg = &actwk[10];
    boss8_get_work(hane)->flags = 8;

    TEST_ASSERT_EQ_INT(ctx, 0, egg8hane_wait(hane, meca, egg));
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actor == hane);

    reset_boss8_state();
    hane = &actwk[12];
    meca = &actwk[11];
    egg = &actwk[10];
    boss8_get_work(meca)->flags = 8;

    TEST_ASSERT_EQ_INT(ctx, 0, egg8hane_wait(hane, meca, egg));
    TEST_ASSERT_EQ_INT(ctx, 4, hane->r_no0);

    reset_boss8_state();
    hane = &actwk[12];
    meca = &actwk[11];
    egg = &actwk[10];
    hane->patno = 9;
    hane->patbase = egg8hane1_pat;
    hane->yposi.l = 100 << 16;

    TEST_ASSERT_EQ_INT(ctx, 1, egg8hane_kill(hane, meca, egg));
    TEST_ASSERT_TRUE(ctx, boss8_get_work(hane)->x_velocity < 0);
    TEST_ASSERT_TRUE(ctx, hane->patbase == egg8hane2_pat1);

    hane->yposi.w.h = 484;
    TEST_ASSERT_EQ_INT(ctx, 0, egg8hane_kill(hane, meca, egg));
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);

    reset_boss8_state();
    hane = &actwk[12];
    meca = &actwk[11];
    egg = &actwk[10];
    boss8_get_work(meca)->action_index = 2;
    boss8_get_work(hane)->wait_time = 2;

    hane_no_reset(hane, meca, egg);

    TEST_ASSERT_EQ_INT(ctx, 8, boss8_get_work(hane)->flags & 8);

    reset_boss8_state();
    hane = &actwk[12];
    meca = &actwk[11];
    egg = &actwk[10];
    boss8_get_work(egg)->hane_count = 1;
    boss8_get_work(hane)->wait_time = 3;

    hane_no_reset(hane, meca, egg);

    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(hane)->wait_time);

    reset_boss8_state();
    hane = &actwk[12];
    meca = &actwk[11];
    egg = &actwk[10];
    boss8_get_work(egg)->hane_count = 3;
    boss8_get_work(meca)->action_index = 1;
    boss8_get_work(hane)->wait_time = 0;

    hane_no_reset(hane, meca, egg);

    TEST_ASSERT_EQ_INT(ctx, 2, boss8_get_work(hane)->wait_time);
}

static void test_egg8hane_fire1_cases(test_context *ctx) {
    sprite_status *hane = &actwk[12];
    sprite_status *meca = &actwk[11];
    sprite_status *egg = &actwk[10];

    reset_boss8_state();
    boss8_get_work(egg)->hane_count = 1;
    hane->yposi.l = 225 << 16;

    TEST_ASSERT_EQ_INT(ctx, 1, egg8hane_fire1(hane, meca, egg));
    TEST_ASSERT_EQ_INT(ctx, 2, boss8_get_work(hane)->step);
    TEST_ASSERT_EQ_INT(ctx, 224, hane->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(meca)->color_a);
    TEST_ASSERT_TRUE(ctx, hane->patbase == egg8hane0_pat2);

    reset_boss8_state();
    hane = &actwk[12];
    meca = &actwk[11];
    egg = &actwk[10];
    boss8_get_work(egg)->hane_count = 2;
    boss8_get_work(meca)->color_a = 2;
    boss8_get_work(hane)->step = 2;

    TEST_ASSERT_EQ_INT(ctx, 1, egg8hane_fire1(hane, meca, egg));
    TEST_ASSERT_EQ_INT(ctx, 3, boss8_get_work(hane)->step);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(hane)->timer_low);

    reset_boss8_state();
    hane = &actwk[12];
    meca = &actwk[11];
    egg = &actwk[10];
    boss8_get_work(egg)->hane_count = 3;
    boss8_get_work(meca)->color_b = 4;
    boss8_get_work(hane)->step = 3;
    boss8_get_work(hane)->timer_low = 29;

    TEST_ASSERT_EQ_INT(ctx, 1, egg8hane_fire1(hane, meca, egg));
    TEST_ASSERT_EQ_INT(ctx, 4, boss8_get_work(hane)->step);
    TEST_ASSERT_EQ_INT(ctx, 3, hane->sprpri);
    TEST_ASSERT_EQ_INT(ctx, 3968, hane->xposi.w.h);
    TEST_ASSERT_TRUE(ctx, hane->patbase == egg8hane2_pat2);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(hane)->x_velocity);
    TEST_ASSERT_EQ_INT(ctx, 229376, sprite_status_get_xspeed_yspeed(hane));

    reset_boss8_state();
    hane = &actwk[12];
    meca = &actwk[11];
    egg = &actwk[10];
    boss8_get_work(egg)->hane_count = 1;
    boss8_get_work(hane)->step = 3;
    boss8_get_work(hane)->timer_low = 29;
    actwk[0].xposi.w.h = 3800;

    TEST_ASSERT_EQ_INT(ctx, 1, egg8hane_fire1(hane, meca, egg));
    TEST_ASSERT_EQ_INT(ctx, 3, hane->patno);
    TEST_ASSERT_EQ_INT(ctx, 4040, hane->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, -211897, boss8_get_work(hane)->x_velocity);
    TEST_ASSERT_EQ_INT(ctx, 87759, sprite_status_get_xspeed_yspeed(hane));

    reset_boss8_state();
    hane = &actwk[12];
    meca = &actwk[11];
    egg = &actwk[10];
    boss8_get_work(egg)->hane_count = 2;
    boss8_get_work(meca)->color_b = 8;
    boss8_get_work(hane)->step = 3;
    boss8_get_work(hane)->timer_low = 59;
    boss8_get_work(hane)->wait_time = 1;
    actwk[0].xposi.w.h = 3888;

    TEST_ASSERT_EQ_INT(ctx, 1, egg8hane_fire1(hane, meca, egg));
    TEST_ASSERT_EQ_INT(ctx, 3888, hane->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(hane)->x_velocity);
    TEST_ASSERT_EQ_INT(ctx, 229376, sprite_status_get_xspeed_yspeed(hane));

    reset_boss8_state();
    hane = &actwk[12];
    meca = &actwk[11];
    egg = &actwk[10];
    boss8_get_work(hane)->step = 4;
    hane->yposi.l = 438 << 16;
    hane->patbase = egg8hane0_pat2;
    sprite_status_set_xspeed_yspeed(hane, 0x40000);

    TEST_ASSERT_EQ_INT(ctx, 1, egg8hane_fire1(hane, meca, egg));
    TEST_ASSERT_EQ_INT(ctx, 5, boss8_get_work(hane)->step);
    TEST_ASSERT_EQ_INT(ctx, 440, hane->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 180, soundset_values[0]);

    reset_boss8_state();
    hane = &actwk[12];
    meca = &actwk[11];
    egg = &actwk[10];
    boss8_get_work(hane)->step = 5;
    boss8_get_work(hane)->timer_low = 29;
    hane->colino = 99;
    hane->colicnt = 9;

    TEST_ASSERT_EQ_INT(ctx, 1, egg8hane_fire1(hane, meca, egg));
    TEST_ASSERT_EQ_INT(ctx, 6, boss8_get_work(hane)->step);
    TEST_ASSERT_EQ_INT(ctx, 5, hane->sprpri);
    TEST_ASSERT_EQ_INT(ctx, 0, hane->colino);
    TEST_ASSERT_EQ_INT(ctx, 0, hane->colicnt);

    reset_boss8_state();
    hane = &actwk[12];
    meca = &actwk[11];
    egg = &actwk[10];
    boss8_get_work(egg)->hane_count = 4;
    boss8_get_work(hane)->step = 6;
    boss8_get_work(hane)->timer_low = 59;
    boss8_get_work(hane)->wait_time = 0;
    hane->patno = 7;

    TEST_ASSERT_EQ_INT(ctx, 0, egg8hane_fire1(hane, meca, egg));
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(meca)->hit_timer);
    TEST_ASSERT_EQ_INT(ctx, 8, hane->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, hane->patno);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(meca)->hane_base_patno);

    reset_boss8_state();
    hane = &actwk[12];
    hane->patno = 15;

    hane_rol_r(hane);

    TEST_ASSERT_EQ_INT(ctx, 0, hane->patno);
    TEST_ASSERT_EQ_INT(ctx, 186, soundset_values[0]);
}

static void test_egg8_remaining_motion_and_meca_branches(test_context *ctx) {
    sprite_status *egg = &actwk[10];
    sprite_status *meca = &actwk[11];

    reset_boss8_state();
    boss8_get_work(egg)->step = 2;
    egg->xposi.l = 3970 << 16;
    boss8_get_work(egg)->x_velocity = 0x60000;
    boss8_get_work(egg)->target_x = 3964;
    boss8_get_work(egg)->target_pos = 2;

    egg8_move_c(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 3964, egg->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->step & 1);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->target_pos);
    TEST_ASSERT_EQ_INT(ctx, 3964, boss8_get_work(egg)->target_x);

    egg8_move_c(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 3840, boss8_get_work(egg)->target_x);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->target_pos);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    boss8_get_work(egg)->step = 3;
    egg->xposi.l = 3835 << 16;
    boss8_get_work(egg)->x_velocity = 0x60000;
    boss8_get_work(egg)->target_x = 3840;
    boss8_get_work(egg)->target_pos = 0;

    egg8_move_c(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 3840, egg->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->flags & 1);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    boss8_get_work(egg)->step = 3;
    egg->yposi.l = 378 << 16;
    boss8_get_work(meca)->hane_count = 128;
    sprite_status_set_xspeed_yspeed(egg, 0x20000);

    egg8_target(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 7, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->flags & 1);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    boss8_get_work(egg)->hane_count = 1;
    egg->xposi.w.h = 3840;
    egg->yposi.w.h = 380;
    actwk[0].xposi.w.h = 3940;

    egg8target_ini(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 2, boss8_get_work(meca)->hane_target);
    TEST_ASSERT_TRUE(ctx, boss8_get_work(egg)->x_velocity > 0);

    reset_boss8_state();
    meca = &actwk[11];
    boss8_get_work(meca)->step = 1;
    boss8_get_work(meca)->target_pos = 171;

    egg8meca_chg1(meca, egg);

    TEST_ASSERT_EQ_INT(ctx, 2, boss8_get_work(meca)->step);
    TEST_ASSERT_EQ_INT(ctx, 3, boss8_get_work(meca)->hane_mode);
    TEST_ASSERT_EQ_INT(ctx, 179, boss8_get_work(meca)->target_pos);

    boss8_get_work(meca)->step = 5;
    boss8_get_work(meca)->target_pos = 200;
    egg8meca_chg1(meca, egg);

    TEST_ASSERT_EQ_INT(ctx, 6, boss8_get_work(meca)->step);
    TEST_ASSERT_EQ_INT(ctx, 4, boss8_get_work(meca)->hane_mode);

    boss8_get_work(meca)->target_pos = 250;
    egg8meca_chg1(meca, egg);

    TEST_ASSERT_EQ_INT(ctx, 256, boss8_get_work(meca)->target_pos);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(meca)->step);
    TEST_ASSERT_EQ_INT(ctx, 2, meca->r_no0);

    reset_boss8_state();
    meca = &actwk[11];
    egg = &actwk[10];
    egg->xposi.w.h = 50;
    egg->yposi.w.h = 60;
    boss8_get_work(meca)->hit_timer = 1;
    boss8_get_work(meca)->hane_count = 4;
    boss8_get_work(meca)->angle = 9;

    egg8meca_fire(meca, egg);

    TEST_ASSERT_EQ_INT(ctx, 1, meca->patno);
    TEST_ASSERT_EQ_INT(ctx, 32, boss8_get_work(meca)->hane_count & 32);

    reset_boss8_state();
    meca = &actwk[11];
    egg = &actwk[10];
    egg->xposi.w.h = 70;
    egg->yposi.w.h = 80;

    egg8meca_hane_reset(meca, egg);

    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(meca)->step);
    TEST_ASSERT_EQ_INT(ctx, 8, boss8_get_work(meca)->flags & 8);
    TEST_ASSERT_EQ_INT(ctx, -250, boss8_get_work(meca)->target_pos);

    reset_boss8_state();
    meca = &actwk[11];
    boss8_get_work(meca)->hane_count = 0;
    boss8_get_work(meca)->timer_low = 1;
    boss8_get_work(meca)->angle = 9;
    boss8_get_work(meca)->roll_speed = 2;

    hane_ctrl(meca);

    TEST_ASSERT_EQ_INT(ctx, 32, boss8_get_work(meca)->hane_count & 32);
    TEST_ASSERT_EQ_INT(ctx, 64, boss8_get_work(meca)->flags & 64);

    reset_boss8_state();
    meca = &actwk[11];
    boss8_get_work(meca)->flags = 16;
    boss8_get_work(meca)->hane_count = 64;
    boss8_get_work(meca)->hane_base_patno = 5;
    boss8_get_work(meca)->hane_target = 5;

    hane_ctrl(meca);

    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(meca)->hane_count & 64);
    TEST_ASSERT_EQ_INT(ctx, 1, meca->patno);

    reset_boss8_state();
    meca = &actwk[11];
    boss8_get_work(meca)->hane_count = 32;
    boss8_get_work(meca)->hane_base_patno = 1;
    boss8_get_work(meca)->hane_target = 2;
    boss8_get_work(meca)->roll_speed = 1;

    hane_stop(meca);

    TEST_ASSERT_EQ_INT(ctx, 64, boss8_get_work(meca)->flags & 64);

    reset_boss8_state();
    meca = &actwk[11];
    boss8_get_work(meca)->hane_count = 32;
    boss8_get_work(meca)->angle = 5;
    boss8_get_work(meca)->roll_speed = 1;

    hane_roll(meca);

    TEST_ASSERT_EQ_INT(ctx, 2, meca->patno);
    TEST_ASSERT_EQ_INT(ctx, 64, boss8_get_work(meca)->flags & 64);
}

static void test_egg8hane_and_collision_branch_variants(test_context *ctx) {
    sprite_status *hane = &actwk[12];
    sprite_status *meca = &actwk[11];
    sprite_status *egg = &actwk[10];
    sprite_status *spawn0 = &actwk[20];
    sprite_status *spawn1 = &actwk[21];

    reset_boss8_state();
    hane->xposi.w.h = 5;
    hane->yposi.w.h = 6;
    boss8_get_work(hane)->flags = 8;

    TEST_ASSERT_EQ_INT(ctx, -1, egg8hane_ini(hane, meca, egg));
    TEST_ASSERT_EQ_INT(ctx, 10, hane->r_no0);

    reset_boss8_state();
    hane = &actwk[12];
    meca = &actwk[11];
    egg = &actwk[10];
    egg->r_no0 = 4;
    hane->colino = 99;
    hane->colicnt = 9;

    TEST_ASSERT_EQ_INT(ctx, 1, egg8hane_demo(hane, meca, egg));
    TEST_ASSERT_EQ_INT(ctx, 0, hane->colino);
    TEST_ASSERT_EQ_INT(ctx, 0, hane->colicnt);

    egg->r_no0 = 6;
    hane->r_no0 = 2;
    TEST_ASSERT_EQ_INT(ctx, 1, egg8hane_demo(hane, meca, egg));
    TEST_ASSERT_EQ_INT(ctx, 4, hane->r_no0);

    reset_boss8_state();
    hane = &actwk[12];
    meca = &actwk[11];
    egg = &actwk[10];
    boss8_get_work(egg)->hane_count = 1;
    boss8_get_work(hane)->step = 3;
    boss8_get_work(hane)->timer_low = 29;
    actwk[0].xposi.w.h = 3900;

    TEST_ASSERT_EQ_INT(ctx, 1, egg8hane_fire1(hane, meca, egg));
    TEST_ASSERT_EQ_INT(ctx, 13, hane->patno);
    TEST_ASSERT_EQ_INT(ctx, 3640, hane->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 211897, boss8_get_work(hane)->x_velocity);

    reset_boss8_state();
    hane = &actwk[12];
    meca = &actwk[11];
    egg = &actwk[10];
    boss8_get_work(egg)->hane_count = 2;
    boss8_get_work(meca)->color_b = 8;
    boss8_get_work(hane)->step = 3;
    boss8_get_work(hane)->timer_low = 29;
    boss8_get_work(hane)->wait_time = 0;

    TEST_ASSERT_EQ_INT(ctx, 1, egg8hane_fire1(hane, meca, egg));
    TEST_ASSERT_EQ_INT(ctx, 3712, hane->xposi.w.h);

    reset_boss8_state();
    hane = &actwk[12];
    boss8_get_work(hane)->step = 4;
    hane->yposi.l = 446 << 16;
    hane->patbase = egg8hane2_pat2;
    sprite_status_set_xspeed_yspeed(hane, 0x30000);

    TEST_ASSERT_EQ_INT(ctx, 1, egg8hane_fire1(hane, meca, egg));
    TEST_ASSERT_EQ_INT(ctx, 448, hane->yposi.w.h);

    reset_boss8_state();
    hane = &actwk[12];
    boss8_get_work(hane)->step = 4;
    hane->patno = 3;
    hane->yposi.l = 462 << 16;
    sprite_status_set_xspeed_yspeed(hane, 0x30000);

    TEST_ASSERT_EQ_INT(ctx, 1, egg8hane_fire1(hane, meca, egg));
    TEST_ASSERT_EQ_INT(ctx, 464, hane->yposi.w.h);

    reset_boss8_state();
    hane = &actwk[12];
    boss8_get_work(hane)->step = 6;
    boss8_get_work(hane)->timer_low = 3;

    TEST_ASSERT_EQ_INT(ctx, 1, egg8hane_fire1(hane, meca, egg));

    reset_boss8_state();
    hane = &actwk[12];
    boss8_get_work(hane)->step = 6;
    boss8_get_work(hane)->timer_low = 1;

    TEST_ASSERT_EQ_INT(ctx, 0, egg8hane_fire1(hane, meca, egg));

    reset_boss8_state();
    hane = &actwk[12];
    hane->patno = 15;
    hane->patbase = egg8hane0_pat1;
    hane->yposi.w.h = 470;
    boss8_get_work(hane)->timer_low = 3;

    TEST_ASSERT_EQ_INT(ctx, 1, egg8hane_kill(hane, meca, egg));
    TEST_ASSERT_EQ_INT(ctx, 15, hane->patno);
    TEST_ASSERT_TRUE(ctx, hane->patbase == egg8hane0_pat3);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    boss8_get_work(egg)->flags = 8;
    boss8_get_work(meca)->hit_flash = 2;

    egg8_colichk(egg, &actwk[0], meca);

    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(meca)->hit_flash);
    TEST_ASSERT_EQ_INT(ctx, 255, boss8_get_work(meca)->action_index);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    spawn0 = &actwk[20];
    spawn1 = &actwk[21];
    queue_spawn(spawn0);
    queue_spawn(spawn1);
    boss8_get_work(egg)->flags = 8;
    boss8_get_work(egg)->hane_count = 3;
    egg->xposi.w.h = 3840;
    actwk[0].xposi.w.h = 3900;
    boss8_get_work(meca)->hane_base_patno = 13;

    egg8_colichk(egg, &actwk[0], meca);

    TEST_ASSERT_EQ_INT(ctx, 2, boss8_get_work(egg)->hane_count);
    TEST_ASSERT_EQ_INT(ctx, 2, boss8_get_work(meca)->action_index);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    spawn0 = &actwk[20];
    queue_spawn(spawn0);
    boss8_get_work(egg)->flags = 8;
    boss8_get_work(egg)->hane_count = 2;
    egg->xposi.w.h = 3840;
    actwk[0].xposi.w.h = 3800;
    boss8_get_work(meca)->hane_base_patno = 8;

    egg8_colichk(egg, &actwk[0], meca);

    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->hane_count);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(meca)->action_index);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    spawn0 = &actwk[20];
    queue_spawn(spawn0);
    boss8_get_work(egg)->flags = 8;
    boss8_get_work(egg)->hane_count = 1;
    egg->xposi.w.h = 3840;
    actwk[0].xposi.w.h = 3900;

    egg8_colichk(egg, &actwk[0], meca);

    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->hane_count);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(meca)->action_index);
    TEST_ASSERT_EQ_INT(ctx, 50, egg->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->flags & 1);
}

static void run_collision_case(sprite_status *egg, sprite_status *meca,
                               sprite_status *spawn, Uint8 hane_count,
                               Sint16 player_x, Sint16 egg_x,
                               Uint8 hane_patno) {
    reset_boss8_state();
    queue_spawn(spawn);
    boss8_get_work(egg)->flags = 8;
    boss8_get_work(egg)->hane_count = hane_count;
    egg->xposi.w.h = egg_x;
    actwk[0].xposi.w.h = player_x;
    boss8_get_work(meca)->hane_base_patno = hane_patno;

    egg8_colichk(egg, &actwk[0], meca);

}

static void test_egg8_more_wrapper_spawn_and_collision_branches(test_context *ctx) {
    sprite_status *hane = &actwk[12];
    sprite_status *meca = &actwk[11];
    sprite_status *egg = &actwk[10];
    sprite_status *hibana = &actwk[13];
    sprite_status *spawn0 = &actwk[20];
    sprite_status *spawn1 = &actwk[21];

    reset_boss8_state();
    boss8_get_work(hane)->parent_index = 11;
    boss8_get_work(meca)->parent_index = 10;
    egg->r_no0 = 4;
    boss8_get_work(meca)->action_index = 128;
    hane->r_no0 = 4;
    hane->patno = 5;

    egg8hane(hane);

    TEST_ASSERT_EQ_INT(ctx, 2, hane->actflg & 2);
    TEST_ASSERT_EQ_INT(ctx, 0, hane->actflg & 1);

    reset_boss8_state();
    hane = &actwk[12];
    meca = &actwk[11];
    egg = &actwk[10];
    boss8_get_work(hane)->parent_index = 11;
    boss8_get_work(meca)->parent_index = 10;
    egg->r_no0 = 4;
    boss8_get_work(meca)->action_index = 128;
    hane->r_no0 = 4;
    hane->patno = 9;

    egg8hane(hane);

    TEST_ASSERT_EQ_INT(ctx, 3, hane->actflg & 3);

    reset_boss8_state();
    hane = &actwk[12];
    meca = &actwk[11];
    egg = &actwk[10];
    boss8_get_work(hane)->parent_index = 11;
    boss8_get_work(meca)->parent_index = 10;
    egg->r_no0 = 4;
    boss8_get_work(meca)->action_index = 128;
    hane->r_no0 = 4;
    hane->actflg = 2;
    hane->patno = 12;

    egg8hane(hane);

    TEST_ASSERT_EQ_INT(ctx, 1, hane->actflg & 1);
    TEST_ASSERT_EQ_INT(ctx, 0, hane->actflg & 2);

    reset_boss8_state();
    meca = &actwk[11];
    hibana = &actwk[13];
    boss8_get_work(hibana)->parent_index = 11;
    boss8_get_work(hibana)->angle = 3;
    boss8_get_work(hibana)->step = 8;
    hibana->patno = 1;

    TEST_ASSERT_EQ_INT(ctx, 0, egg8hibana_1(hibana));
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(hibana)->step);
    TEST_ASSERT_EQ_INT(ctx, 2, hibana->patno);

    reset_boss8_state();
    meca = &actwk[11];
    spawn0 = &actwk[20];
    spawn1 = &actwk[21];
    queue_spawn(spawn0);
    queue_spawn(spawn1);

    make_hane(meca);

    TEST_ASSERT_EQ_INT(ctx, 3, actwkchk2_count);
    TEST_ASSERT_EQ_INT(ctx, 64, spawn0->actno);
    TEST_ASSERT_EQ_INT(ctx, 64, spawn1->actno);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[22].actno);

    reset_boss8_state();
    egg = &actwk[10];
    spawn0 = &actwk[20];
    queue_spawn(spawn0);
    boss8_get_work(egg)->timer_low = 0;

    egg8_dead(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 3, egg->mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 16, meca->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, spawn0->actno);

    reset_boss8_state();
    egg = &actwk[10];
    spawn0 = &actwk[20];
    queue_spawn(spawn0);
    boss8_get_work(egg)->timer_low = 1;

    egg8_dead(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 24, spawn0->actno);

    reset_boss8_state();
    egg = &actwk[10];
    spawn0 = &actwk[20];
    queue_spawn(spawn0);
    boss8_get_work(egg)->timer_low = 3;

    bom_set(egg);

    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 0, spawn0->actno);

    reset_boss8_state();
    egg = &actwk[10];
    boss8_get_work(egg)->timer_low = 4;

    bom_set(egg);

    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 0, soundset_count);

    egg = &actwk[10];
    meca = &actwk[11];
    spawn0 = &actwk[20];

    run_collision_case(egg, meca, spawn0, 4, 3800, 3840, 6);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(meca)->action_index);

    egg = &actwk[10];
    meca = &actwk[11];
    spawn0 = &actwk[20];
    run_collision_case(egg, meca, spawn0, 4, 3800, 3840, 10);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(meca)->action_index);

    egg = &actwk[10];
    meca = &actwk[11];
    spawn0 = &actwk[20];
    run_collision_case(egg, meca, spawn0, 4, 3800, 3840, 13);
    TEST_ASSERT_EQ_INT(ctx, 3, boss8_get_work(meca)->action_index);

    egg = &actwk[10];
    meca = &actwk[11];
    spawn0 = &actwk[20];
    run_collision_case(egg, meca, spawn0, 4, 3900, 3840, 4);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(meca)->action_index);

    egg = &actwk[10];
    meca = &actwk[11];
    spawn0 = &actwk[20];
    run_collision_case(egg, meca, spawn0, 4, 3900, 3840, 8);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(meca)->action_index);

    egg = &actwk[10];
    meca = &actwk[11];
    spawn0 = &actwk[20];
    run_collision_case(egg, meca, spawn0, 4, 3900, 3840, 9);
    TEST_ASSERT_EQ_INT(ctx, 3, boss8_get_work(meca)->action_index);

    egg = &actwk[10];
    meca = &actwk[11];
    spawn0 = &actwk[20];
    run_collision_case(egg, meca, spawn0, 3, 3800, 3840, 4);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(meca)->action_index);

    egg = &actwk[10];
    meca = &actwk[11];
    spawn0 = &actwk[20];
    run_collision_case(egg, meca, spawn0, 3, 3900, 3840, 4);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(meca)->action_index);

    egg = &actwk[10];
    meca = &actwk[11];
    spawn0 = &actwk[20];
    run_collision_case(egg, meca, spawn0, 3, 3900, 3840, 7);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(meca)->action_index);
}

static void test_egg8_spin_target_and_tobi_remaining_branches(test_context *ctx) {
    sprite_status *egg = &actwk[10];
    sprite_status *meca = &actwk[11];

    reset_boss8_state();
    boss8_get_work(egg)->hane_count = 2;
    egg->xposi.l = 3958 << 16;
    egg->yposi.l = 410 << 16;
    boss8_get_work(egg)->x_velocity = 0x60000;
    sprite_status_set_xspeed_yspeed(egg, 0x60000);
    actwk[0].xposi.w.h = 3900;

    egg8_spin_d(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 3964, egg->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 416, egg->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 8, boss8_get_work(meca)->hane_target);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    boss8_get_work(egg)->hane_count = 3;
    egg->xposi.l = 3720 << 16;
    egg->yposi.l = 410 << 16;
    boss8_get_work(egg)->x_velocity = 0x60000;
    sprite_status_set_xspeed_yspeed(egg, 0x60000);
    actwk[0].xposi.w.h = 3900;

    egg8_spin_d(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 3716, egg->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(meca)->hane_target);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    boss8_get_work(egg)->step = 128;
    boss8_get_work(egg)->timer_low = 59;

    egg8_spin_r(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->timer_low);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->step & 128);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    boss8_get_work(egg)->step = 2;
    boss8_get_work(egg)->hane_count = 4;
    boss8_get_work(egg)->timer_low = 19;
    boss8_get_work(egg)->spin_frame = 3;
    egg->xposi.w.h = 4005;
    egg->yposi.w.h = 320;

    egg8_spin_r(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 131, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->angle);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    boss8_get_work(egg)->step = 3;
    boss8_get_work(egg)->hane_count = 2;
    boss8_get_work(egg)->timer_low = 19;
    boss8_get_work(egg)->spin_frame = 3;
    boss8_get_work(egg)->angle = 5;
    egg->xposi.w.h = 3800;
    egg->yposi.w.h = 320;

    egg8_spin_r(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 133, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 4, boss8_get_work(egg)->angle);
    TEST_ASSERT_EQ_INT(ctx, 64, boss8_get_work(meca)->flags & 64);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    boss8_get_work(egg)->hane_count = 1;
    egg->xposi.w.h = 3840;
    egg->yposi.w.h = 380;
    actwk[0].xposi.w.h = 3800;

    egg8target_ini(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 4, boss8_get_work(meca)->hane_target);
    TEST_ASSERT_TRUE(ctx, boss8_get_work(egg)->x_velocity < 0);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    boss8_get_work(egg)->step = 1;
    egg->yposi.l = 392 << 16;
    sprite_status_set_xspeed_yspeed(egg, 0x30000);
    boss8_get_work(meca)->hane_target = 2;

    egg8target_move(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 394, egg->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 4, boss8_get_work(egg)->step & 4);

    reset_boss8_state();
    egg = &actwk[10];
    egg->xposi.w.h = 3800;
    egg->yposi.w.h = 300;
    sinset_sin = 128;
    sinset_cos = 256;

    egg8_tobi_u(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->target_x);
    TEST_ASSERT_EQ_INT(ctx, -52, boss8_get_work(egg)->target_y);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    egg->xposi.w.h = 3675;
    boss8_get_work(egg)->step = 1;
    boss8_get_work(egg)->timer = 32760;
    sinset_sin = 0;
    sinset_cos = 256;

    egg8_tobi(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 4, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 180, soundset_values[0]);

    reset_boss8_state();
    egg = &actwk[10];
    meca = &actwk[11];
    egg->xposi.w.h = 3900;
    boss8_get_work(egg)->step = 1;
    boss8_get_work(egg)->timer = 32760;
    sinset_sin = 0;
    sinset_cos = 256;

    egg8_tobi(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 41, boss8_get_work(egg)->target_x);
}

static void test_egg8_late_coverage_edges(test_context *ctx) {
    sprite_status *egg = &actwk[10];
    sprite_status *meca = &actwk[11];
    sprite_status *spawn0 = &actwk[20];
    sprite_status *spawn1 = &actwk[21];

    reset_boss8_state();
    boss8_get_work(egg)->parent_index = 11;
    egg->r_no0 = 6;
    boss8_get_work(egg)->hit_flash = 1;
    egg->mstno.b.h = 4;
    boss8_get_work(meca)->flags = 255;
    boss8_get_work(meca)->hit_flash = 2;

    egg8(egg);

    TEST_ASSERT_EQ_INT(ctx, 159, boss8_get_work(meca)->flags);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->hit_flash);
    TEST_ASSERT_EQ_INT(ctx, 0, egg->mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 0, actionsub_count);

    reset_boss8_state();
    egg->xposi.w.h = 3900;
    boss8_get_work(egg)->hane_count = 4;
    actwk[0].xposi.w.h = 3890;

    egg8_spin_r(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 7, boss8_get_work(egg)->angle);
    TEST_ASSERT_EQ_INT(ctx, 14, meca->r_no0);

    reset_boss8_state();
    boss8_get_work(egg)->step = 1;
    boss8_get_work(egg)->hane_count = 2;
    boss8_get_work(egg)->timer_low = 19;
    boss8_get_work(egg)->spin_frame = 3;
    boss8_get_work(egg)->angle = 0;
    egg->xposi.w.h = 3800;
    egg->yposi.w.h = 320;

    egg8_spin_r(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 132, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 64, boss8_get_work(meca)->flags & 64);

    reset_boss8_state();
    boss8_get_work(egg)->step = 1;
    boss8_get_work(egg)->hane_count = 2;
    boss8_get_work(egg)->timer_low = 19;
    boss8_get_work(egg)->angle = 5;
    egg->xposi.w.h = 3800;
    egg->yposi.w.h = 320;

    egg8_spin_r(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 4, boss8_get_work(egg)->angle);
    TEST_ASSERT_EQ_INT(ctx, 64, boss8_get_work(meca)->flags & 64);

    reset_boss8_state();
    boss8_get_work(egg)->hane_count = 2;
    egg->xposi.w.h = 3900;
    egg->yposi.w.h = 360;
    actwk[0].xposi.w.h = 3800;

    egg8target_ini(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 6, boss8_get_work(meca)->hane_target);
    TEST_ASSERT_TRUE(ctx, boss8_get_work(egg)->x_velocity < 0);

    reset_boss8_state();
    egg->xposi.w.h = 3700;
    actwk[0].xposi.w.h = 3900;

    egg8_tobi_d(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 3, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 3716, boss8_get_work(egg)->target_pos);

    reset_boss8_state();
    egg->xposi.w.h = 3800;
    sinset_sin = 0;
    sinset_cos = 256;

    egg8_tobi(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 2, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, -41, boss8_get_work(egg)->target_x);
    TEST_ASSERT_EQ_INT(ctx, 18, meca->r_no0);

    reset_boss8_state();
    egg->xposi.w.h = 4005;
    boss8_get_work(egg)->step = 2;
    boss8_get_work(egg)->timer = 32760;
    sinset_sin = 0;
    sinset_cos = 256;

    egg8_tobi(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 3, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 41, boss8_get_work(egg)->target_x);

    reset_boss8_state();
    boss8_get_work(egg)->timer_low = 89;

    egg8_dead(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 90, boss8_get_work(egg)->timer_low);
    TEST_ASSERT_EQ_INT(ctx, 0, egg->r_no0);

    reset_boss8_state();
    boss8_get_work(egg)->timer_low = 59;
    queue_spawn(spawn0);
    queue_spawn(spawn1);

    egg8_esc(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 3, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 24, spawn0->actno);
    TEST_ASSERT_EQ_INT(ctx, 66, spawn1->actno);

    reset_boss8_state();
    boss8_get_work(egg)->flags = 8;
    boss8_get_work(egg)->hane_count = 4;
    egg->xposi.w.h = 3840;
    egg->r_no0 = 30;
    meca->colino = 0;
    boss8_get_work(meca)->hit_flash = 0;
    boss8_get_work(meca)->hane_base_patno = 6;
    actwk[0].xposi.w.h = 3900;

    egg8_colichk(egg, &actwk[0], meca);

    TEST_ASSERT_EQ_INT(ctx, 50, egg->colino);
    TEST_ASSERT_EQ_INT(ctx, 51, meca->colino);

    reset_boss8_state();
    boss8_get_work(meca)->step = 1;
    boss8_get_work(meca)->target_pos = 178;

    egg8meca_chg1(meca, egg);

    TEST_ASSERT_EQ_INT(ctx, 2, boss8_get_work(meca)->step);
    TEST_ASSERT_EQ_INT(ctx, 3, boss8_get_work(meca)->hane_mode);

    reset_boss8_state();
    boss8_get_work(meca)->flags = 16;
    boss8_get_work(meca)->hane_base_patno = 3;
    boss8_get_work(meca)->hane_target = 3;

    hane_ctrl(meca);

    TEST_ASSERT_EQ_INT(ctx, 64, boss8_get_work(meca)->hane_count & 64);

    reset_boss8_state();
    boss8_get_work(meca)->flags = 16;
    boss8_get_work(meca)->hane_count = 64 | 32 | 128;

    hane_ctrl(meca);

    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(meca)->hane_count & 64);

    reset_boss8_state();
    boss8_get_work(egg)->hane_count = 4;
    boss8_get_work(meca)->action_index = 128;
    boss8_get_work(meca)->hane_mode = 1;
    boss8_get_work(&actwk[12])->parent_index = 11;
    boss8_get_work(meca)->parent_index = 10;
    actwk[12].r_no0 = 4;
    actwk[12].patno = 13;

    egg8hane(&actwk[12]);

    TEST_ASSERT_EQ_INT(ctx, 1, actwk[12].actflg & 1);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[12].actflg & 2);

    reset_boss8_state();
    sprite_status *hibana = &actwk[12];
    boss8_get_work(hibana)->parent_index = 11;
    boss8_get_work(meca)->parent_index = 10;
    hibana->actno = 76;
    hibana->r_no0 = 2;
    boss8_get_work(hibana)->timer_low = 149;
    hibana->xposi.w.h = 3840;
    hibana->yposi.w.h = 479;
    sprite_status_set_xspeed_yspeed(hibana, 0x30000);

    egg8hibana(hibana);

    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_EQ_INT(ctx, 0, hibana->actno);
}

static void test_egg8_remaining_uncovered_edges(test_context *ctx) {
    sprite_status *egg = &actwk[10];
    sprite_status *meca = &actwk[11];
    sprite_status *hane = &actwk[12];
    sprite_status *spawn0 = &actwk[20];
    sprite_status *spawn1 = &actwk[21];

    reset_boss8_state();
    boss8_get_work(egg)->parent_index = 11;
    boss8_get_work(egg)->flags = 1;
    boss8_get_work(egg)->hane_count = 1;
    boss8_get_work(egg)->action_index = 0;
    egg->r_no0 = 4;

    egg8(egg);

    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->action_index);
    TEST_ASSERT_EQ_INT(ctx, 18, egg->r_no0);

    reset_boss8_state();
    boss8_get_work(egg)->step = 2;
    egg->xposi.l = 4000 << 16;
    boss8_get_work(egg)->x_velocity = 0x10000;
    boss8_get_work(egg)->target_x = 3900;

    egg8_move_c(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 3999, egg->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->flags & 1);

    reset_boss8_state();
    boss8_get_work(egg)->step = 1;
    egg->xposi.l = 3800 << 16;
    boss8_get_work(egg)->x_velocity = 0x10000;
    boss8_get_work(egg)->target_x = 3900;

    egg8_move_c(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 3801, egg->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->flags & 1);

    reset_boss8_state();
    boss8_get_work(egg)->step = 1;
    egg->xposi.w.h = 3964;
    boss8_get_work(egg)->target_x = 3964;
    boss8_get_work(egg)->target_pos = 2;

    egg8_move_c(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 3716, boss8_get_work(egg)->target_x);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->step & 1);

    reset_boss8_state();
    boss8_get_work(egg)->hane_count = 1;

    egg8_hane_stop(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 4, boss8_get_work(meca)->hane_target);

    reset_boss8_state();
    boss8_get_work(egg)->hane_count = 2;
    egg->xposi.w.h = 3800;
    actwk[0].xposi.w.h = 3900;

    egg8_spin_d(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 4, boss8_get_work(meca)->hane_target);
    TEST_ASSERT_EQ_INT(ctx, 2, boss8_get_work(egg)->step);

    reset_boss8_state();
    boss8_get_work(egg)->step = 3;
    egg->yposi.w.h = 300;
    boss8_get_work(egg)->target_pos = 416;

    egg8_spin_d(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 3, boss8_get_work(egg)->step);

    reset_boss8_state();
    egg->xposi.w.h = 3700;
    boss8_get_work(egg)->hane_count = 4;
    actwk[0].xposi.w.h = 3900;

    egg8_spin_r(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 2, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->angle);

    reset_boss8_state();
    boss8_get_work(egg)->step = 3;
    boss8_get_work(egg)->hane_count = 2;
    boss8_get_work(egg)->timer_low = 19;
    boss8_get_work(egg)->spin_frame = 3;
    boss8_get_work(egg)->angle = 1;

    egg8_spin_r(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 133, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->angle);

    reset_boss8_state();
    boss8_get_work(egg)->step = 3;
    egg->yposi.w.h = 312;
    boss8_get_work(meca)->hane_count = 128;
    boss8_get_work(egg)->timer = 32760;
    sinset_sin = 0;
    sinset_cos = 256;

    egg8_tobi_d(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 5, boss8_get_work(egg)->step);

    reset_boss8_state();
    boss8_get_work(egg)->step = 5;
    boss8_get_work(egg)->timer = 32760;
    sinset_sin = 0;
    sinset_cos = 256;

    egg8_tobi_d(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->flags & 1);
    TEST_ASSERT_EQ_INT(ctx, 180, soundset_values[0]);

    reset_boss8_state();
    egg->xposi.w.h = 3840;
    sinset_cos = 256;

    egg8_tobi(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 41, boss8_get_work(egg)->target_x);

    reset_boss8_state();
    egg->xposi.w.h = 3600;
    boss8_get_work(egg)->step = 1;
    boss8_get_work(egg)->timer = 32760;
    sinset_sin = 0;
    sinset_cos = 256;

    egg8_tobi(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(egg)->step);

    reset_boss8_state();
    egg->xposi.w.h = 4000;
    boss8_get_work(egg)->step = 2;
    boss8_get_work(egg)->timer = 32760;
    sinset_sin = 0;
    sinset_cos = 256;

    egg8_tobi(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 3, boss8_get_work(egg)->step);

    reset_boss8_state();
    queue_spawn(spawn0);
    queue_spawn(spawn1);
    boss8_get_work(egg)->flags = 8;
    boss8_get_work(egg)->hane_count = 4;
    egg->xposi.w.h = 3840;
    boss8_get_work(meca)->hane_base_patno = 0;
    actwk[0].xposi.w.h = 3900;

    egg8_colichk(egg, &actwk[0], meca);

    TEST_ASSERT_EQ_INT(ctx, 2, boss8_get_work(meca)->action_index);

    reset_boss8_state();
    queue_spawn(spawn0);
    queue_spawn(spawn1);
    boss8_get_work(egg)->flags = 8;
    boss8_get_work(egg)->hane_count = 3;
    egg->xposi.w.h = 3840;
    boss8_get_work(meca)->hane_base_patno = 13;
    actwk[0].xposi.w.h = 3800;

    egg8_colichk(egg, &actwk[0], meca);

    TEST_ASSERT_EQ_INT(ctx, 2, boss8_get_work(meca)->action_index);

    reset_boss8_state();
    queue_spawn(spawn0);
    queue_spawn(spawn1);
    boss8_get_work(egg)->flags = 8;
    boss8_get_work(egg)->hane_count = 3;
    egg->xposi.w.h = 3840;
    boss8_get_work(meca)->hane_base_patno = 8;
    actwk[0].xposi.w.h = 3800;

    egg8_colichk(egg, &actwk[0], meca);

    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(meca)->action_index);

    reset_boss8_state();
    queue_spawn(spawn0);
    queue_spawn(spawn1);
    boss8_get_work(egg)->flags = 8;
    boss8_get_work(egg)->hane_count = 2;
    egg->xposi.w.h = 3840;
    boss8_get_work(meca)->hane_base_patno = 7;
    actwk[0].xposi.w.h = 3900;

    egg8_colichk(egg, &actwk[0], meca);

    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(meca)->action_index);

    reset_boss8_state();
    queue_spawn(spawn0);
    queue_spawn(spawn1);
    boss8_get_work(egg)->flags = 8;
    boss8_get_work(egg)->hane_count = 99;

    egg8_colichk(egg, &actwk[0], meca);

    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->hane_count);

    reset_boss8_state();
    boss8_get_work(meca)->target_pos = 100;

    egg8meca_chg1(meca, egg);

    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(meca)->step);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(meca)->hane_mode);

    reset_boss8_state();

    egg8meca_chg2(meca, egg);

    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(meca)->step);
    TEST_ASSERT_EQ_INT(ctx, 1, boss8_get_work(meca)->hane_mode);

    reset_boss8_state();
    boss8_get_work(meca)->hane_count = 64 | 32;

    hane_ctrl(meca);

    TEST_ASSERT_EQ_INT(ctx, 64, boss8_get_work(meca)->flags & 64);

    reset_boss8_state();
    boss8_get_work(meca)->hane_count = 32;
    boss8_get_work(meca)->hane_base_patno = 5;
    boss8_get_work(meca)->hane_target = 5;

    hane_stop(meca);

    TEST_ASSERT_EQ_INT(ctx, 128, boss8_get_work(meca)->hane_count & 128);
    TEST_ASSERT_EQ_INT(ctx, 0, meca->patno);

    reset_boss8_state();
    boss8_get_work(hane)->parent_index = 11;
    boss8_get_work(meca)->parent_index = 10;
    boss8_get_work(meca)->action_index = 0;
    hane->r_no0 = 8;
    boss8_get_work(hane)->flags = 8;

    egg8hane(hane);

    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);

    reset_boss8_state();
    boss8_get_work(hane)->step = 1;
    boss8_get_work(hane)->timer_low = 3;
    boss8_get_work(hane)->angle = 1;
    hane->patno = 15;
    hane->yposi.w.h = 300;

    TEST_ASSERT_EQ_INT(ctx, 0, egg8hane_kill(hane, meca, egg));
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(hane)->timer_low);
    TEST_ASSERT_EQ_INT(ctx, 0, hane->patno);

    reset_boss8_state();
    hane->r_no0 = 2;
    boss8_get_work(hane)->angle = 3;
    hane->patno = 2;

    egg8hibana(hane);

    TEST_ASSERT_EQ_INT(ctx, 0, hane->patno);

    reset_boss8_state();
    egg->xposi.w.h = 3840;
    egg->yposi.w.h = 300;

    egg8meca_tobi(meca, egg);

    TEST_ASSERT_EQ_INT(ctx, 256, boss8_get_work(meca)->target_pos);

    reset_boss8_state();
    egg->xposi.w.h = 3840;
    egg->yposi.w.h = 380;
    actwk[0].xposi.w.h = 3800;

    egg8_target(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 5, boss8_get_work(egg)->step);
    TEST_ASSERT_EQ_INT(ctx, 0, boss8_get_work(egg)->flags & 1);

    reset_boss8_state();
    boss8_get_work(egg)->step = 1;
    egg->yposi.w.h = 300;
    boss8_get_work(meca)->hane_count = 128;

    egg8_target(egg, meca);

    TEST_ASSERT_EQ_INT(ctx, 3, boss8_get_work(egg)->step);

    reset_boss8_state();
    boss8_get_work(hane)->parent_index = 11;
    boss8_get_work(meca)->parent_index = 10;
    boss8_get_work(hane)->flags = 8;

    egg8hane(hane);

    TEST_ASSERT_EQ_INT(ctx, 10, hane->r_no0);

    reset_boss8_state();
    boss8_get_work(meca)->flags = 32;
    boss8_get_work(meca)->hane_mode = 1;
    hane->patno = 4;

    TEST_ASSERT_EQ_INT(ctx, 1, egg8hane_normal(hane, meca, egg));
    TEST_ASSERT_EQ_INT(ctx, 5, hane->patno);
}

TEST_MAIN_BEGIN;
test_egg8_init_sets_boss_sprite_basics(&ctx);
test_egg8_scrset_clamps_then_starts_boss(&ctx);
test_egg8_open_sequence_cases(&ctx);
test_egg8_movement_and_hane_control_helpers(&ctx);
test_egg8_wait_vertical_escape_and_center_move(&ctx);
test_egg8_hane_state_changes(&ctx);
test_hane_roll_stop_and_futa_helpers(&ctx);
test_egg8meca_init_spawns_hanes_and_transitions(&ctx);
test_egg8meca_follow_spin_and_reset_variants(&ctx);
test_egg8hibana_tracks_meca_animates_and_expires(&ctx);
test_public_wrappers_dispatch_and_draw(&ctx);
test_egg8_fire_spin_and_target_helpers(&ctx);
test_egg8_tobi_bom_action_and_collision_helpers(&ctx);
test_egg8_tobi_death_and_escape_sequences(&ctx);
test_egg8hane_state_machine_helpers(&ctx);
test_egg8hane_fire1_cases(&ctx);
test_egg8_remaining_motion_and_meca_branches(&ctx);
test_egg8hane_and_collision_branch_variants(&ctx);
test_egg8_more_wrapper_spawn_and_collision_branches(&ctx);
test_egg8_spin_target_and_tobi_remaining_branches(&ctx);
test_egg8_late_coverage_edges(&ctx);
test_egg8_remaining_uncovered_edges(&ctx);
TEST_MAIN_END
