/* <stdlib.h> declares POSIX random(); the decompiled src/io.h declares the
   game's own Sint32 random(void). Parse stdlib first, then rename the game
   symbol inside this harness so the two can coexist. */
#include <stdlib.h>

#define random game_random

#include <string.h>

#include "support/test_runner.h"
#include "src/types.h"

sprite_status actwk[128];
Sint16 waterposi;
int_union scra_h_posit;
Uint8 awasintbl[128];

static sprite_pattern awa_pat0 = {1, {{0, 0, 0, 100}}};
sprite_pattern *awapat[1] = {&awa_pat0};

static int patchg_count;
static sprite_status *patchg_actor;
static Uint8 **patchg_table;
static int actionsub_count;
static sprite_status *actionsub_actor;
static int frameout_count;
static sprite_status *frameout_actor;
static int speedset2_count;
static sprite_status *speedset2_actor;
static int plairset_count;
static int soundset_count;
static Sint16 soundset_last;
static Sint32 random_values[32];
static int random_count;
static int random_index;
static int actwkchk_count;
static sprite_status *actwkchk_queue[16];
static int actwkchk_queue_count;
static int actwkchk_queue_index;

void patchg(sprite_status *patchgwk, Uint8 **pat_dat);
void actionsub(sprite_status *pActwk);
void frameout(sprite_status *pActwk);
void speedset2(sprite_status *pActwk);
void plairset(void);
void soundset(Sint16 ReqNo);
Sint32 random(void);
Sint32 actwkchk(sprite_status **ppActwk);

#include "src/r4/awa.c"

void patchg(sprite_status *patchgwk, Uint8 **pat_dat) {
    ++patchg_count;
    patchg_actor = patchgwk;
    patchg_table = pat_dat;
}

void actionsub(sprite_status *pActwk) {
    ++actionsub_count;
    actionsub_actor = pActwk;
}

void frameout(sprite_status *pActwk) {
    ++frameout_count;
    frameout_actor = pActwk;
}

void speedset2(sprite_status *pActwk) {
    ++speedset2_count;
    speedset2_actor = pActwk;
}

void plairset(void) { ++plairset_count; }

void soundset(Sint16 ReqNo) {
    ++soundset_count;
    soundset_last = ReqNo;
}

Sint32 random(void) {
    ++random_count;
    if (random_index >= 32)
        return 0;
    return random_values[random_index++];
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

static void reset_awa_state(void) {
    memset(actwk, 0, sizeof(actwk));
    memset(&scra_h_posit, 0, sizeof(scra_h_posit));
    memset(awasintbl, 0, sizeof(awasintbl));
    waterposi = 0;
    patchg_count = 0;
    patchg_actor = 0;
    patchg_table = 0;
    actionsub_count = 0;
    actionsub_actor = 0;
    frameout_count = 0;
    frameout_actor = 0;
    speedset2_count = 0;
    speedset2_actor = 0;
    plairset_count = 0;
    soundset_count = 0;
    soundset_last = 0;
    memset(random_values, 0, sizeof(random_values));
    random_count = 0;
    random_index = 0;
    actwkchk_count = 0;
    memset(actwkchk_queue, 0, sizeof(actwkchk_queue));
    actwkchk_queue_count = 0;
    actwkchk_queue_index = 0;
}

static void queue_actor(sprite_status *actor) {
    actwkchk_queue[actwkchk_queue_count++] = actor;
}

static void set_random_value(int index, Sint32 value) {
    random_values[index] = value;
}

static void test_awa_tables_capture_literal_data(test_context *ctx) {
    TEST_ASSERT_TRUE(ctx, awachg[0] == awachg0);
    TEST_ASSERT_TRUE(ctx, awachg[6] == awachg6);
    TEST_ASSERT_EQ_INT(ctx, 14, awachg0[0]);
    TEST_ASSERT_EQ_INT(ctx, 252, awachg0[4]);
    TEST_ASSERT_EQ_INT(ctx, 4, awachg4[0]);
    TEST_ASSERT_EQ_INT(ctx, 19, awachg6[1]);
    TEST_ASSERT_EQ_INT(ctx, 255, awachg6[4]);
}

static void test_awainit_normal_sets_motion_and_moves(test_context *ctx) {
    sprite_status *actor = &actwk[4];

    reset_awa_state();
    actor->xposi.w.h = 120;
    actor->yposi.w.h = 200;
    actor->userflag.b.h = 1;
    awasintbl[5] = 7;
    set_random_value(0, 5);

    awa(actor);

    TEST_ASSERT_EQ_INT(ctx, 2, actor->r_no0);
    TEST_ASSERT_TRUE(ctx, actor->patbase == awapat);
    TEST_ASSERT_EQ_INT(ctx, 33930, actor->sproffset);
    TEST_ASSERT_EQ_INT(ctx, 132, actor->actflg);
    TEST_ASSERT_EQ_INT(ctx, 16, actor->sprhsize);
    TEST_ASSERT_EQ_INT(ctx, 1, actor->sprpri);
    TEST_ASSERT_EQ_INT(ctx, 1, actor->mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 120, awa_get_work(actor)->origin_x);
    TEST_ASSERT_EQ_INT(ctx, -136, actor->yspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 6, actor->direc.b.h);
    TEST_ASSERT_EQ_INT(ctx, 127, actor->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, random_count);
    TEST_ASSERT_EQ_INT(ctx, 1, patchg_count);
    TEST_ASSERT_TRUE(ctx, patchg_actor == actor);
    TEST_ASSERT_TRUE(ctx, patchg_table == awachg);
    TEST_ASSERT_EQ_INT(ctx, 1, speedset2_count);
    TEST_ASSERT_TRUE(ctx, speedset2_actor == actor);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_TRUE(ctx, actionsub_actor == actor);
}

static void test_awainit_master_spawns_and_records_spawn_counters(
    test_context *ctx) {
    sprite_status *actor = &actwk[4];
    sprite_status *bubble = &actwk[5];

    reset_awa_state();
    actor->xposi.w.h = 300;
    actor->yposi.w.h = 120;
    actor->userflag.b.h = (Sint8)130;
    actor->actflg = 128;
    waterposi = 0;
    queue_actor(bubble);
    set_random_value(0, 2);
    set_random_value(1, 9);
    set_random_value(2, 14);

    awa(actor);

    TEST_ASSERT_EQ_INT(ctx, 10, actor->r_no0);
    TEST_ASSERT_TRUE(ctx, actor->patbase == awapat);
    TEST_ASSERT_EQ_INT(ctx, 33930, actor->sproffset);
    TEST_ASSERT_EQ_INT(ctx, 132, actor->actflg);
    TEST_ASSERT_EQ_INT(ctx, 1, awa_get_work(actor)->spawn_count);
    TEST_ASSERT_EQ_INT(ctx, 2, awa_get_work(actor)->spawn_reload);
    TEST_ASSERT_EQ_INT(ctx, 1, awa_get_work(actor)->state_flags);
    TEST_ASSERT_EQ_INT(ctx, 9, awa_get_work(actor)->timer);
    TEST_ASSERT_EQ_INT(ctx, 1, awa_get_work(actor)->spawn_index);
    TEST_ASSERT_EQ_INT(ctx, 0, awa_get_work(actor)->table_offset);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 32, bubble->actno);
    TEST_ASSERT_EQ_INT(ctx, 306, bubble->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 120, bubble->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, bubble->userflag.b.h);
    TEST_ASSERT_EQ_INT(ctx, 3, random_count);
    TEST_ASSERT_EQ_INT(ctx, 1, patchg_count);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
}

static void test_awamove_marks_large_bubble_and_collision_refills_air(
    test_context *ctx) {
    sprite_status *actor = &actwk[4];
    sprite_status *player = &actwk[0];

    reset_awa_state();
    actor->xposi.w.h = 100;
    actor->yposi.w.h = 100;
    actor->actflg = 128;
    actor->r_no0 = 2;
    actor->mstno.b.h = 1;
    actor->patno = 6;
    awa_get_work(actor)->collider_enabled = 0;
    awa_get_work(actor)->origin_x = 100;
    player->xposi.w.h = 100;
    player->yposi.w.h = 105;
    player->xspeed.w = 22;
    player->yspeed.w = -33;
    player->mspeed.w = 44;
    player->mstno.b.h = 3;
    player_work_get(player)->status_flags = 0;
    player_work_get(player)->jump_started = 77;
    player->cddat = 64;

    awamove(actor);

    TEST_ASSERT_EQ_INT(ctx, 6, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 4, actor->mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 1, awa_get_work(actor)->collider_enabled);
    TEST_ASSERT_EQ_INT(ctx, 1, plairset_count);
    TEST_ASSERT_EQ_INT(ctx, 1, soundset_count);
    TEST_ASSERT_EQ_INT(ctx, 173, soundset_last);
    TEST_ASSERT_EQ_INT(ctx, 0, player->xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 0, player->yspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 0, player->mspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 21, player->mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 0, player_work_get(player)->jump_started);
    TEST_ASSERT_EQ_INT(ctx, 0, player->cddat & 48);
    TEST_ASSERT_EQ_INT(ctx, 2, patchg_count);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
}

static void test_awamove_collision_restores_crouched_player_shape(
    test_context *ctx) {
    sprite_status *actor = &actwk[4];
    sprite_status *player = &actwk[0];

    reset_awa_state();
    actor->xposi.w.h = 100;
    actor->yposi.w.h = 100;
    actor->actflg = 128;
    actor->r_no0 = 2;
    actor->mstno.b.h = 1;
    awa_get_work(actor)->collider_enabled = 1;
    awa_get_work(actor)->origin_x = 100;
    player->xposi.w.h = 100;
    player->yposi.w.h = 105;
    player->sprvsize = 14;
    player->sprhs = 7;
    player->cddat = 4;

    awamove(actor);

    TEST_ASSERT_EQ_INT(ctx, 6, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, player->cddat & 4);
    TEST_ASSERT_EQ_INT(ctx, 19, player->sprvsize);
    TEST_ASSERT_EQ_INT(ctx, 9, player->sprhs);
    TEST_ASSERT_EQ_INT(ctx, 100, player->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, plairset_count);
    TEST_ASSERT_EQ_INT(ctx, 173, soundset_last);
}

static void test_awamove2_rises_hits_water_and_frameout_paths(
    test_context *ctx) {
    sprite_status *actor = &actwk[4];

    reset_awa_state();
    actor->r_no0 = 2;
    actor->mstno.b.h = 2;
    actor->yposi.w.h = 50;
    waterposi = 50;

    awamove2(actor);

    TEST_ASSERT_EQ_INT(ctx, 6, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 5, actor->mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 1, patchg_count);

    reset_awa_state();
    actor = &actwk[4];
    actor->r_no0 = 2;
    actor->mstno.b.h = 1;
    actor->xposi.w.h = 80;
    actor->yposi.w.h = 100;
    actor->actflg = 0;
    awa_get_work(actor)->origin_x = 80;
    awasintbl[3] = (Uint8)-4;
    actor->direc.b.h = 3;
    waterposi = 10;

    awamove2(actor);

    TEST_ASSERT_EQ_INT(ctx, 76, actor->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, speedset2_count);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actor == actor);
}

static void test_awamaster_countdown_and_fade_paths(test_context *ctx) {
    sprite_status *actor = &actwk[4];
    sprite_status *bubble = &actwk[5];

    reset_awa_state();
    actor->xposi.w.h = 200;
    actor->yposi.w.h = 100;
    actor->actflg = 128;
    awa_get_work(actor)->spawn_index = 0;
    awa_get_work(actor)->state_flags = 128;
    awa_get_work(actor)->timer = 0;
    queue_actor(bubble);
    set_random_value(0, 3);
    set_random_value(1, 4);
    waterposi = 0;

    awamaster(actor);

    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 32, bubble->actno);
    TEST_ASSERT_EQ_INT(ctx, 196, bubble->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 2, bubble->userflag.b.h);
    TEST_ASSERT_EQ_INT(ctx, 255, awa_get_work(actor)->spawn_index);
    TEST_ASSERT_EQ_INT(ctx, 1, patchg_count);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);

    reset_awa_state();
    actor = &actwk[4];
    actor->xposi.w.h = 1000;
    actor->yposi.w.h = 100;
    actor->actflg = 0;
    scra_h_posit.w.h = 128;
    waterposi = 0;

    awafoutchk(actor);

    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actor == actor);

    reset_awa_state();
    actor = &actwk[4];
    actor->actflg = 128;
    actor->yposi.w.h = 50;
    waterposi = 50;

    awafoutchk(actor);

    TEST_ASSERT_EQ_INT(ctx, 0, actor->actflg & 128);
}

static void test_awamaster_early_exit_and_countdown_paths(test_context *ctx) {
    sprite_status *actor = &actwk[4];

    reset_awa_state();
    actor->yposi.w.h = 100;
    actor->actflg = 128;
    waterposi = 100;

    awamaster(actor);

    TEST_ASSERT_EQ_INT(ctx, 0, patchg_count);
    TEST_ASSERT_EQ_INT(ctx, 0, actionsub_count);

    reset_awa_state();
    actor = &actwk[4];
    actor->yposi.w.h = 100;
    actor->actflg = 0;
    scra_h_posit.w.h = 128;
    waterposi = 0;

    awamaster(actor);

    TEST_ASSERT_EQ_INT(ctx, 0, patchg_count);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);

    reset_awa_state();
    actor = &actwk[4];
    actor->yposi.w.h = 100;
    actor->actflg = 128;
    awa_get_work(actor)->timer = 2;
    waterposi = 0;

    awamaster(actor);

    TEST_ASSERT_EQ_INT(ctx, 1, awa_get_work(actor)->timer);
    TEST_ASSERT_EQ_INT(ctx, 0, random_count);
    TEST_ASSERT_EQ_INT(ctx, 1, patchg_count);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);

    reset_awa_state();
    actor = &actwk[4];
    actor->yposi.w.h = 100;
    actor->actflg = 128;
    awa_get_work(actor)->state_flags = 1;
    awa_get_work(actor)->timer = 2;
    waterposi = 0;

    awamaster(actor);

    TEST_ASSERT_EQ_INT(ctx, 1, awa_get_work(actor)->timer);
    TEST_ASSERT_EQ_INT(ctx, 0, random_count);
    TEST_ASSERT_EQ_INT(ctx, 1, patchg_count);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
}

static void test_awamaster_underflow_and_special_spawn_flags(
    test_context *ctx) {
    sprite_status *actor = &actwk[4];
    sprite_status *bubble = &actwk[5];

    reset_awa_state();
    actor->xposi.w.h = 200;
    actor->yposi.w.h = 100;
    actor->actflg = 128;
    awa_get_work(actor)->spawn_count = 0;
    awa_get_work(actor)->spawn_reload = 5;
    awa_get_work(actor)->timer = 0;
    waterposi = 0;
    set_random_value(0, 1);

    awamaster(actor);

    TEST_ASSERT_EQ_INT(ctx, 5, awa_get_work(actor)->spawn_count);
    TEST_ASSERT_EQ_INT(ctx, 128, awa_get_work(actor)->state_flags & 128);

    reset_awa_state();
    actor = &actwk[4];
    bubble = &actwk[5];
    actor->xposi.w.h = 200;
    actor->yposi.w.h = 100;
    actor->actflg = 128;
    awa_get_work(actor)->spawn_index = 1;
    awa_get_work(actor)->state_flags = 192;
    awa_get_work(actor)->timer = 0;
    queue_actor(bubble);
    waterposi = 0;
    set_random_value(0, 3);
    set_random_value(1, 4);
    set_random_value(2, 0);

    awamaster(actor);

    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 32, bubble->actno);
    TEST_ASSERT_EQ_INT(ctx, 1, bubble->userflag.b.h);
    TEST_ASSERT_EQ_INT(ctx, 0, awa_get_work(actor)->spawn_index);

    reset_awa_state();
    actor = &actwk[4];
    bubble = &actwk[5];
    actor->xposi.w.h = 200;
    actor->yposi.w.h = 100;
    actor->actflg = 128;
    awa_get_work(actor)->spawn_index = 0;
    awa_get_work(actor)->state_flags = 128;
    awa_get_work(actor)->timer = 0;
    queue_actor(bubble);
    waterposi = 0;
    set_random_value(0, 3);
    set_random_value(1, 4);
    set_random_value(2, 1);

    awamaster(actor);

    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 32, bubble->actno);
    TEST_ASSERT_EQ_INT(ctx, 2, bubble->userflag.b.h);
    TEST_ASSERT_EQ_INT(ctx, 255, awa_get_work(actor)->spawn_index);
    TEST_ASSERT_EQ_INT(ctx, 0, awa_get_work(actor)->state_flags);
}

static void test_awacoli_bounds(test_context *ctx) {
    sprite_status *actor = &actwk[4];
    sprite_status *player = &actwk[0];

    reset_awa_state();
    actor->xposi.w.h = 100;
    actor->yposi.w.h = 100;
    player->xposi.w.h = 100;
    player->yposi.w.h = 105;

    TEST_ASSERT_EQ_INT(ctx, 1, awacoli(actor));

    player_work_get(player)->status_flags = 128;
    TEST_ASSERT_EQ_INT(ctx, 0, awacoli(actor));

    player_work_get(player)->status_flags = 0;
    player->xposi.w.h = 83;
    TEST_ASSERT_EQ_INT(ctx, 0, awacoli(actor));

    player->xposi.w.h = 100;
    player->yposi.w.h = 99;
    TEST_ASSERT_EQ_INT(ctx, 0, awacoli(actor));

    player->yposi.w.h = 117;
    TEST_ASSERT_EQ_INT(ctx, 0, awacoli(actor));
}

static void test_awamove4_erases_actor(test_context *ctx) {
    sprite_status *actor = &actwk[4];

    reset_awa_state();

    awamove4(actor);

    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actor == actor);
}

TEST_MAIN_BEGIN;
test_awa_tables_capture_literal_data(&ctx);
test_awainit_normal_sets_motion_and_moves(&ctx);
test_awainit_master_spawns_and_records_spawn_counters(&ctx);
test_awamove_marks_large_bubble_and_collision_refills_air(&ctx);
test_awamove_collision_restores_crouched_player_shape(&ctx);
test_awamove2_rises_hits_water_and_frameout_paths(&ctx);
test_awamaster_countdown_and_fade_paths(&ctx);
test_awamaster_early_exit_and_countdown_paths(&ctx);
test_awamaster_underflow_and_special_spawn_flags(&ctx);
test_awacoli_bounds(&ctx);
test_awamove4_erases_actor(&ctx);
TEST_MAIN_END
