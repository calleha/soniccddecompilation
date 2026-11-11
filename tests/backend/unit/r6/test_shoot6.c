/* <stdlib.h> declares POSIX random(); the decompiled src/io.h declares the
   game's own Sint32 random(void). Parse stdlib first, then rename the game
   symbol inside this harness so the two can coexist. */
#include <stdlib.h>

#define random game_random

#include <string.h>

#include "support/test_runner.h"
#include "src/types.h"

sprite_status actwk[128];
ushort_union swdata;
ushort_union swdata1;
Uint8 time_flag;
char time_item;
Uint8 flagwork[766];
Uint8 sys_pattim4;
Uint8 plpower_m;
Uint8 plpower_a;
sprite_pattern *ringpat[9];
sprite_pattern *bariapat[13];

static int actionsub_count;
static sprite_status *actionsub_actor;
static int frameout_s_count;
static sprite_status *frameout_s_actor;
static int frameout_count;
static sprite_status *frameout_actor;
static int actwkchk_count;
static sprite_status *actwkchk_queue[16];
static int actwkchk_queue_count;
static int actwkchk_queue_index;
static int actwkchk2_count;
static sprite_status *actwkchk2_queue[8];
static int actwkchk2_queue_count;
static int actwkchk2_queue_index;
static int patchg_count;
static sprite_status *patchg_actor;
static Uint8 **patchg_table;
static int soundset_count;
static Sint16 soundset_requests[16];
static int sub_sync_count;
static Sint16 sub_sync_requests[8];
static int playdamageset_count;
static sprite_status *playdamageset_player;
static sprite_status *playdamageset_actor;
static int ridechk_count;
static sprite_status *ridechk_actor;
static sprite_status *ridechk_player;
static Sint16 ridechk_result;
static int hitchk_count;
static sprite_status *hitchk_actor;
static sprite_status *hitchk_player;
static Sint16 emycol_d_result;
static int emycol_d_count;
static sprite_status *emycol_d_actor;
static int random_result;

void actionsub(sprite_status *pActwk);
Sint32 frameout_s(sprite_status *pActwk);
void frameout(sprite_status *pActwk);
Sint32 actwkchk(sprite_status **ppActwk);
Sint32 actwkchk2(sprite_status *pActwk, sprite_status **ppNewActwk);
void patchg(sprite_status *patchgwk, Uint8 **pat_dat);
void soundset(Sint16 ReqNo);
void sub_sync(Sint16 ReqNo);
Sint16 playdamageset(sprite_status *pActwk, sprite_status *pActwk2);
Sint16 ridechk(sprite_status *pActwk, sprite_status *pPlayerwk);
Sint16 hitchk(sprite_status *pActwk, sprite_status *pPlayerwk);
Sint16 emycol_d(sprite_status *pActwk);
Sint32 random(void);

#include "src/r6/shoot6.c"

void actionsub(sprite_status *pActwk) {
    ++actionsub_count;
    actionsub_actor = pActwk;
}

Sint32 frameout_s(sprite_status *pActwk) {
    ++frameout_s_count;
    frameout_s_actor = pActwk;
    return 0;
}

void frameout(sprite_status *pActwk) {
    ++frameout_count;
    frameout_actor = pActwk;
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

Sint32 actwkchk2(sprite_status *pActwk, sprite_status **ppNewActwk) {
    (void)pActwk;
    ++actwkchk2_count;
    if (actwkchk2_queue_index >= actwkchk2_queue_count) {
        *ppNewActwk = 0;
        return -1;
    }
    *ppNewActwk = actwkchk2_queue[actwkchk2_queue_index++];
    return 0;
}

void patchg(sprite_status *patchgwk, Uint8 **pat_dat) {
    ++patchg_count;
    patchg_actor = patchgwk;
    patchg_table = pat_dat;
}

void soundset(Sint16 ReqNo) {
    if (soundset_count < 16)
        soundset_requests[soundset_count] = ReqNo;
    ++soundset_count;
}

void sub_sync(Sint16 ReqNo) {
    if (sub_sync_count < 8)
        sub_sync_requests[sub_sync_count] = ReqNo;
    ++sub_sync_count;
}

Sint16 playdamageset(sprite_status *pActwk, sprite_status *pActwk2) {
    ++playdamageset_count;
    playdamageset_player = pActwk;
    playdamageset_actor = pActwk2;
    return 0;
}

Sint16 ridechk(sprite_status *pActwk, sprite_status *pPlayerwk) {
    ++ridechk_count;
    ridechk_actor = pActwk;
    ridechk_player = pPlayerwk;
    return ridechk_result;
}

Sint16 hitchk(sprite_status *pActwk, sprite_status *pPlayerwk) {
    ++hitchk_count;
    hitchk_actor = pActwk;
    hitchk_player = pPlayerwk;
    return 0;
}

Sint16 emycol_d(sprite_status *pActwk) {
    ++emycol_d_count;
    emycol_d_actor = pActwk;
    return emycol_d_result;
}

Sint32 random(void) { return random_result; }

static void queue_actwkchk(sprite_status *actor) {
    actwkchk_queue[actwkchk_queue_count++] = actor;
}

static void queue_actwkchk2(sprite_status *actor) {
    actwkchk2_queue[actwkchk2_queue_count++] = actor;
}

static void reset_logs(void) {
    actionsub_count = 0;
    actionsub_actor = 0;
    frameout_s_count = 0;
    frameout_s_actor = 0;
    frameout_count = 0;
    frameout_actor = 0;
    actwkchk_count = 0;
    memset(actwkchk_queue, 0, sizeof(actwkchk_queue));
    actwkchk_queue_count = 0;
    actwkchk_queue_index = 0;
    actwkchk2_count = 0;
    memset(actwkchk2_queue, 0, sizeof(actwkchk2_queue));
    actwkchk2_queue_count = 0;
    actwkchk2_queue_index = 0;
    patchg_count = 0;
    patchg_actor = 0;
    patchg_table = 0;
    soundset_count = 0;
    memset(soundset_requests, 0, sizeof(soundset_requests));
    sub_sync_count = 0;
    memset(sub_sync_requests, 0, sizeof(sub_sync_requests));
    playdamageset_count = 0;
    playdamageset_player = 0;
    playdamageset_actor = 0;
    ridechk_count = 0;
    ridechk_actor = 0;
    ridechk_player = 0;
    ridechk_result = 0;
    hitchk_count = 0;
    hitchk_actor = 0;
    hitchk_player = 0;
    emycol_d_result = 0;
    emycol_d_count = 0;
    emycol_d_actor = 0;
    random_result = 0;
}

static void reset_shoot6_state(void) {
    memset(actwk, 0, sizeof(actwk));
    memset(&swdata, 0, sizeof(swdata));
    memset(&swdata1, 0, sizeof(swdata1));
    time_flag = 0;
    time_item = 0;
    memset(flagwork, 0, sizeof(flagwork));
    sys_pattim4 = 0;
    plpower_m = 0;
    plpower_a = 0;
    memset(ringpat, 0, sizeof(ringpat));
    memset(bariapat, 0, sizeof(bariapat));
    stackpointer = 0;
    reset_logs();
}

static void test_megami_spawns_rings_and_finishes(test_context *ctx) {
    sprite_status *megamiwk = &actwk[4];
    sprite_status *ring = &actwk[9];

    reset_shoot6_state();
    megamiwk->xposi.w.h = 100;
    megamiwk->yposi.w.h = 200;
    actwk[0].xposi.w.h = 1000;
    actwk[0].yposi.w.h = 200;

    megami(megamiwk);

    TEST_ASSERT_EQ_INT(ctx, 2, megamiwk->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 4, megamiwk->actflg);
    TEST_ASSERT_EQ_INT(ctx, 50, megami_get_work(megamiwk)->ring_count);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_s_count);

    actwk[0].xposi.w.h = 100;
    actwk[0].yposi.w.h = 200;
    megami(megamiwk);
    TEST_ASSERT_EQ_INT(ctx, 4, megamiwk->r_no0);

    queue_actwkchk(ring);
    random_result = 5;
    megami_get_work(megamiwk)->ring_timer = 0;
    megami_get_work(megamiwk)->ring_count = 1;
    megami(megamiwk);

    TEST_ASSERT_EQ_INT(ctx, 17, ring->actno);
    TEST_ASSERT_EQ_INT(ctx, 2, ring->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 124, ring->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 184, ring->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 384, ring->xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, -512, ring->yspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 255, sys_pattim4);

    megami_get_work(megamiwk)->ring_timer = 0;
    megami_get_work(megamiwk)->ring_count = 0;
    megami(megamiwk);
    TEST_ASSERT_EQ_INT(ctx, 6, megamiwk->r_no0);
}

static void test_gas_spawns_child_and_child_resets_parent(test_context *ctx) {
    sprite_status *gaswk = &actwk[4];
    sprite_status *cloud = &actwk[8];

    reset_shoot6_state();
    gaswk->xposi.w.h = 300;
    gaswk->yposi.w.h = 400;
    gaswk->sproffset = 123;

    gas(gaswk);

    TEST_ASSERT_EQ_INT(ctx, 2, gaswk->r_no0);
    TEST_ASSERT_TRUE(ctx, gaswk->patbase == gaspat);
    TEST_ASSERT_EQ_INT(ctx, 119, gas_get_work(gaswk)->timer);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_s_count);

    reset_logs();
    gas_get_work(gaswk)->timer = 1;
    queue_actwkchk(cloud);
    gas_move0(gaswk);

    TEST_ASSERT_EQ_INT(ctx, 5, cloud->actno);
    TEST_ASSERT_EQ_INT(ctx, 3, cloud->sprpri);
    TEST_ASSERT_EQ_INT(ctx, gaswk->sproffset, cloud->sproffset);
    TEST_ASSERT_TRUE(ctx, cloud->patbase == gaswk->patbase);
    TEST_ASSERT_EQ_INT(ctx, 300, cloud->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 436, cloud->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 4, cloud->r_no0);

    reset_logs();
    gas_move2(cloud);

    TEST_ASSERT_EQ_INT(ctx, 120, gas_get_work(gaswk)->timer);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actor == cloud);
}

static void test_gas_collision_freezes_player_and_rejects_guards(
    test_context *ctx) {
    sprite_status *gaswk = &actwk[4];
    sprite_status *ice = &actwk[9];

    reset_shoot6_state();
    gaswk->mstno.b.h = 1;
    gaswk->xposi.w.h = 100;
    gaswk->yposi.w.h = 200;
    actwk[0].xposi.w.h = 100;
    actwk[0].yposi.w.h = 200;
    actwk[0].sprhs = 8;
    actwk[0].sprvsize = 16;
    queue_actwkchk(ice);

    gas_move1(gaswk);

    TEST_ASSERT_EQ_INT(ctx, 1, patchg_count);
    TEST_ASSERT_TRUE(ctx, patchg_actor == gaswk);
    TEST_ASSERT_TRUE(ctx, patchg_table == gaschg);
    TEST_ASSERT_EQ_INT(ctx, 65, player_work_get(&actwk[0])->status_flags);
    TEST_ASSERT_EQ_INT(ctx, 5, ice->actno);
    TEST_ASSERT_EQ_INT(ctx, 8, ice->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 24, ice->sprhs);
    TEST_ASSERT_EQ_INT(ctx, 8, ice->patno);

    reset_shoot6_state();
    gaswk->mstno.b.h = 1;
    gaswk->xposi.w.h = 100;
    gaswk->yposi.w.h = 200;
    actwk[0].xposi.w.h = 300;
    actwk[0].yposi.w.h = 200;
    actwk[0].sprhs = 8;
    actwk[0].sprvsize = 16;
    gas_coli(gaswk);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);

    actwk[0].xposi.w.h = 100;
    plpower_m = 1;
    gas_coli(gaswk);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);

    plpower_m = 0;
    plpower_a = 1;
    gas_coli(gaswk);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);
}

static void test_gas_floor_launch_damage_and_ice_fragments(test_context *ctx) {
    sprite_status *gaswk = &actwk[4];
    sprite_status *cloud = &actwk[8];
    sprite_status *piece0 = &actwk[10];
    sprite_status *piece1 = &actwk[11];
    char tbl[16] = {-1, 2, 3, 9, 1, -2, 4, 5, 6, -7, 8, 10, 0, 1, -1, 2};

    reset_shoot6_state();
    gaswk->yposi.w.h = 200;
    gas_init(gaswk);
    queue_actwkchk(cloud);
    gas_get_work(gaswk)->timer = 1;
    gas_move0(gaswk);
    reset_logs();
    emycol_d_result = -5;

    gas_move3(cloud);

    TEST_ASSERT_EQ_INT(ctx, 1, emycol_d_count);
    TEST_ASSERT_EQ_INT(ctx, 1, sub_sync_count);
    TEST_ASSERT_EQ_INT(ctx, 146, sub_sync_requests[0]);
    TEST_ASSERT_EQ_INT(ctx, 15, gas_get_work(cloud)->ice_timer);
    TEST_ASSERT_EQ_INT(ctx, 6, cloud->r_no0);
    TEST_ASSERT_EQ_INT(ctx, cloud->yposi.l, gaswk->yposi.l);

    gas_get_work(cloud)->ice_timer = 1;
    player_work_get(&actwk[0])->status_flags = 255;
    actwk[0].cddat = 32;
    swdata.b.l = 16;
    queue_actwkchk(piece0);
    queue_actwkchk(piece1);
    reset_logs();
    swdata.b.l = 16;
    gas_move4(cloud);

    TEST_ASSERT_EQ_INT(ctx, 0, player_work_get(gaswk)->status_flags & 65);
    TEST_ASSERT_EQ_INT(ctx, -1664, gaswk->yspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 2, gaswk->mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 1, soundset_count);
    TEST_ASSERT_EQ_INT(ctx, 8, cloud->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 10, cloud->patno);

    reset_shoot6_state();
    gas_init(gaswk);
    queue_actwkchk(cloud);
    gas_get_work(gaswk)->timer = 1;
    gas_move0(gaswk);
    gas_get_work(cloud)->ice_timer = 0;
    reset_logs();
    gas_move4(cloud);

    TEST_ASSERT_EQ_INT(ctx, 1, playdamageset_count);
    TEST_ASSERT_TRUE(ctx, playdamageset_player == gaswk);

    reset_shoot6_state();
    gaswk->xposi.w.h = 100;
    gaswk->yposi.w.h = 200;
    queue_actwkchk(piece0);
    queue_actwkchk(piece1);
    ice_sub0(gaswk, tbl, 1);

    TEST_ASSERT_EQ_INT(ctx, 5, piece0->actno);
    TEST_ASSERT_EQ_INT(ctx, 12, piece0->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 99, piece0->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 202, piece0->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 3, gas_get_work(piece0)->ice_timer);
    TEST_ASSERT_EQ_INT(ctx, -2, piece0->xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 106, piece1->xposi.w.h);

    gas_get_work(piece0)->ice_timer = 2;
    piece0->xspeed.w = 3;
    piece0->yspeed.w = -4;
    gas_move5(piece0);
    TEST_ASSERT_EQ_INT(ctx, 1, gas_get_work(piece0)->ice_timer);
    TEST_ASSERT_EQ_INT(ctx, 102, piece0->xposi.w.h);

    gas_get_work(piece0)->ice_timer = 1;
    piece0->patno = 11;
    gas_move5(piece0);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
}

static void test_gas_and_ice_early_return_edges(test_context *ctx) {
    sprite_status *gaswk = &actwk[4];
    sprite_status *cloud = &actwk[8];
    sprite_status *piece0 = &actwk[10];
    sprite_status *piece1 = &actwk[11];

    reset_shoot6_state();
    gas_get_work(gaswk)->timer = 0;
    gas_move0(gaswk);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);

    gas_get_work(gaswk)->timer = 1;
    gas_move0(gaswk);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 0, cloud->actno);

    reset_logs();
    ice_set(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 0, player_work_get(&actwk[0])->status_flags);

    reset_shoot6_state();
    gaswk->mstno.b.h = 0;
    gas_coli(gaswk);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);

    gaswk->mstno.b.h = 1;
    actwk[0].mstno.b.h = 43;
    gas_coli(gaswk);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);

    actwk[0].mstno.b.h = 0;
    actwk[0].r_no0 = 4;
    gas_coli(gaswk);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);

    actwk[0].r_no0 = 0;
    player_work_get(&actwk[0])->status_flags = 1;
    gas_coli(gaswk);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);

    player_work_get(&actwk[0])->status_flags = 0;
    gaswk->xposi.w.h = 100;
    gaswk->yposi.w.h = 200;
    actwk[0].xposi.w.h = 100;
    actwk[0].yposi.w.h = 400;
    actwk[0].sprhs = 8;
    actwk[0].sprvsize = 16;
    gas_coli(gaswk);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);

    reset_shoot6_state();
    gas_init(gaswk);
    queue_actwkchk(cloud);
    gas_get_work(gaswk)->timer = 1;
    gas_move0(gaswk);
    gas_get_work(cloud)->ice_timer = 2;
    reset_logs();
    swdata.b.l = 0;
    gas_move4(cloud);
    TEST_ASSERT_EQ_INT(ctx, 1, gas_get_work(cloud)->ice_timer);
    TEST_ASSERT_EQ_INT(ctx, 4, cloud->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, soundset_count);

    reset_shoot6_state();
    gaswk->xposi.w.h = 100;
    gaswk->yposi.w.h = 200;
    queue_actwkchk(piece0);
    queue_actwkchk(piece1);
    ice_sub_set(gaswk);
    TEST_ASSERT_EQ_INT(ctx, 3, actwkchk_count);

    reset_logs();
    gas_get_work(piece0)->ice_timer = 1;
    piece0->patno = 10;
    gas_get_work(piece0)->ice_table = 0;
    gas_move5(piece0);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actor == piece0);
}

static void test_catapult_cycle_and_child_follow(test_context *ctx) {
    sprite_status *cata = &actwk[4];
    sprite_status *child = &actwk[9];

    reset_shoot6_state();
    cata->xposi.w.h = 100;
    cata->yposi.w.h = 200;
    queue_actwkchk2(child);
    ridechk_result = 1;

    catapalt(cata);

    TEST_ASSERT_EQ_INT(ctx, 4, cata->r_no0);
    TEST_ASSERT_TRUE(ctx, cata->patbase == cata_pat);
    TEST_ASSERT_EQ_INT(ctx, 4, child->actno);
    TEST_ASSERT_EQ_INT(ctx, 8, child->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, player_work_get(&actwk[0])->status_flags & 1);
    TEST_ASSERT_EQ_INT(ctx, 3072, cata->xspeed.w);

    reset_logs();
    swdata1.b.h = 16;
    player_work_get(&actwk[0])->status_flags = 1;
    cata_move0(cata);

    TEST_ASSERT_EQ_INT(ctx, 0, player_work_get(&actwk[0])->status_flags & 1);
    TEST_ASSERT_EQ_INT(ctx, -1664, actwk[0].yspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 1, soundset_count);

    cata->xposi.w.h = 1013;
    cata->cddat = 8;
    player_work_get(&actwk[0])->status_flags = 1;
    cata_move0(cata);
    TEST_ASSERT_EQ_INT(ctx, 1012, cata->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 6, cata->r_no0);

    cata_move1(cata);
    TEST_ASSERT_EQ_INT(ctx, 1008, cata->xposi.w.h);
    cata->xposi.w.h = 100;
    cata_move1(cata);
    TEST_ASSERT_EQ_INT(ctx, 2, cata->r_no0);

    reset_logs();
    child->xposi.w.h = 333;
    child->yposi.w.h = 444;
    child->r_no0 = 2;
    cata_move2(child);

    TEST_ASSERT_EQ_INT(ctx, 76, child->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 184, child->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, hitchk_count);
}

static void test_catapult_early_return_edges(test_context *ctx) {
    sprite_status *cata = &actwk[4];
    sprite_status *child = &actwk[9];

    reset_shoot6_state();
    cata->xposi.w.h = 100;
    cata->yposi.w.h = 200;
    cata_init(cata);
    TEST_ASSERT_EQ_INT(ctx, 2, cata->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk2_count);

    reset_logs();
    cata_wait(cata);
    TEST_ASSERT_EQ_INT(ctx, 2, cata->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, ridechk_count);

    reset_shoot6_state();
    cata->xposi.w.h = 100;
    cata->yposi.w.h = 200;
    queue_actwkchk2(child);
    cata_init(cata);
    reset_logs();
    cata->xposi.w.h = 1013;
    cata->cddat = 0;
    cata->r_no0 = 4;
    cata_move0(cata);
    TEST_ASSERT_EQ_INT(ctx, 1012, cata->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 6, cata->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, player_work_get(&actwk[0])->status_flags);

    reset_logs();
    child->r_no0 = 8;
    child->xposi.w.h = 333;
    child->yposi.w.h = 444;
    cata->r_no0 = 4;
    cata_move2(child);
    TEST_ASSERT_EQ_INT(ctx, 333, child->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 444, child->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, hitchk_count);
}

static void test_shooter_route_and_speed_paths(test_context *ctx) {
    sprite_status *shoot = &actwk[3];

    reset_shoot6_state();
    TEST_ASSERT_TRUE(ctx, shooterpositbl[0] == shooterposi_0);
    TEST_ASSERT_TRUE(ctx, shooterpositbl[18] == shooterposi_12);
    TEST_ASSERT_EQ_INT(ctx, 78, shooterposi_0[0]);

    time_flag = 128 | 4;
    time_item = 2;
    shoot->cdsts = 3;
    flagwork[11] = 255;
    actwk[0].mstno.b.h = 43;
    shooter(shoot);
    TEST_ASSERT_EQ_INT(ctx, 127, flagwork[11]);
    TEST_ASSERT_EQ_INT(ctx, 0, shoot->r_no0);

    reset_shoot6_state();
    shoot->xposi.w.h = 100;
    shoot->yposi.w.h = 200;
    actwk[0].xposi.w.h = 100;
    actwk[0].yposi.w.h = 200;
    actwk[0].r_no0 = 4;
    shooterinit(shoot);
    TEST_ASSERT_EQ_INT(ctx, 4, shoot->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[0].r_no0);
    TEST_ASSERT_EQ_INT(ctx, 129, player_work_get(&actwk[0])->status_flags);
    TEST_ASSERT_EQ_INT(ctx, 1, soundset_count);

    reset_shoot6_state();
    shoot->r_no0 = 2;
    shoot->xposi.w.h = 100;
    shoot->yposi.w.h = 200;
    actwk[0].xposi.w.h = 200;
    actwk[0].yposi.w.h = 200;
    shootermove(shoot);
    TEST_ASSERT_EQ_INT(ctx, 2, shoot->r_no0);

    actwk[0].xposi.w.h = 100;
    actwk[0].yposi.w.h = 200;
    shoot->userflag.b.h = -1;
    shoot->userflag.b.l = 1;
    shootermove(shoot);
    TEST_ASSERT_EQ_INT(ctx, 4, shoot->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 129 | 64,
                       player_work_get(&actwk[0])->status_flags);
    TEST_ASSERT_EQ_INT(ctx, 64, actwk[0].actflg & 64);

    reset_shoot6_state();
    shoot->xposi.w.h = 100;
    shoot->yposi.w.h = 200;
    actwk[0].xposi.w.h = 100;
    actwk[0].yposi.w.h = 200;
    shooterinit(shoot);
    reset_logs();
    shootermove2(shoot);
    TEST_ASSERT_EQ_INT(ctx, 6, shoot->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, soundset_count);

    actwk[0].xspeed.w = 1;
    shooter_get_work(shoot)->move_timer_high = 1;
    shootermove3(shoot);
    TEST_ASSERT_TRUE(ctx, actwk[0].xposi.l != 100 << 16);

    shooter_get_work(shoot)->move_timer_high = 0;
    shooter_get_work(shoot)->move_index = 10;
    shooter_get_work(shoot)->move_limit = 12;
    player_work_get(&actwk[0])->status_flags = 129;
    shootermove3(shoot);
    TEST_ASSERT_EQ_INT(ctx, 0, shoot->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, player_work_get(&actwk[0])->status_flags);

    reset_shoot6_state();
    shoot->xposi.w.h = 100;
    shoot->yposi.w.h = 200;
    actwk[0].xposi.w.h = 100;
    actwk[0].yposi.w.h = 200;
    actwk[0].mspeed.w = 2048;
    shooter_get_work(shoot)->target_x = 0;
    shooter_get_work(shoot)->target_y = 64;
    shooterspdset(shoot);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 2048, actwk[0].yspeed.w);

    shooter_get_work(shoot)->target_x = -64;
    shooter_get_work(shoot)->target_y = 0;
    shooterspdset(shoot);
    TEST_ASSERT_EQ_INT(ctx, -2048, actwk[0].xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].yspeed.w);
}

static void test_megami_early_return_edges(test_context *ctx) {
    sprite_status *megamiwk = &actwk[4];

    reset_shoot6_state();
    megamiwk->r_no0 = 2;
    megamiwk->xposi.w.h = 100;
    megamiwk->yposi.w.h = 200;
    actwk[0].xposi.w.h = 100;
    actwk[0].yposi.w.h = 300;
    m_move0(megamiwk);
    TEST_ASSERT_EQ_INT(ctx, 2, megamiwk->r_no0);

    megamiwk->r_no0 = 4;
    megami_get_work(megamiwk)->ring_timer = 2;
    m_move1(megamiwk);
    TEST_ASSERT_EQ_INT(ctx, 1, megami_get_work(megamiwk)->ring_timer);
    TEST_ASSERT_EQ_INT(ctx, 4, megamiwk->r_no0);

    megamiwk->actno = 77;
    m_move2(megamiwk);
    TEST_ASSERT_EQ_INT(ctx, 77, megamiwk->actno);

    reset_logs();
    ring_set(megamiwk);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
}

static void test_shooter_wrapper_and_route_edges(test_context *ctx) {
    sprite_status *shoot = &actwk[3];

    reset_shoot6_state();
    time_flag = 0;
    shoot->cdsts = 4;
    flagwork[12] = 255;
    actwk[0].mstno.b.h = 43;
    shooter(shoot);
    TEST_ASSERT_EQ_INT(ctx, 255, flagwork[12]);

    reset_shoot6_state();
    time_flag = 128;
    shoot->cdsts = 0;
    flagwork[0] = 255;
    actwk[0].mstno.b.h = 43;
    shooter(shoot);
    TEST_ASSERT_EQ_INT(ctx, 255, flagwork[0]);

    reset_shoot6_state();
    time_flag = 128 | 1;
    time_item = 3;
    shoot->cdsts = 2;
    flagwork[6] = 255;
    actwk[0].mstno.b.h = 43;
    shooter(shoot);
    TEST_ASSERT_EQ_INT(ctx, 127, flagwork[6]);

    reset_shoot6_state();
    time_flag = 128 | 9;
    time_item = 1;
    shoot->cdsts = 2;
    flagwork[8] = 255;
    actwk[0].mstno.b.h = 43;
    shooter(shoot);
    TEST_ASSERT_EQ_INT(ctx, 127, flagwork[8]);

    reset_shoot6_state();
    actwk[0].r_no0 = 6;
    shooter(shoot);
    TEST_ASSERT_EQ_INT(ctx, 0, shoot->r_no0);

    reset_shoot6_state();
    shoot->r_no0 = 2;
    shoot->xposi.w.h = 100;
    shoot->yposi.w.h = 200;
    actwk[0].xposi.w.h = 100;
    actwk[0].yposi.w.h = 200;
    shooter(shoot);
    TEST_ASSERT_EQ_INT(ctx, 4, shoot->r_no0);

    reset_shoot6_state();
    shoot->r_no0 = 4;
    shoot->xposi.w.h = 100;
    shoot->yposi.w.h = 200;
    actwk[0].xposi.w.h = 100;
    actwk[0].yposi.w.h = 200;
    actwk[0].mspeed.w = 2048;
    shooter_get_work(shoot)->target_x = 0;
    shooter_get_work(shoot)->target_y = 64;
    shooter(shoot);
    TEST_ASSERT_EQ_INT(ctx, 6, shoot->r_no0);

    reset_shoot6_state();
    shoot->xposi.w.h = 100;
    shoot->yposi.w.h = 200;
    actwk[0].xposi.w.h = 100;
    actwk[0].yposi.w.h = 200;
    shooter(shoot);
    TEST_ASSERT_EQ_INT(ctx, 4, shoot->r_no0);

    reset_shoot6_state();
    shoot->r_no0 = 2;
    actwk[0].r_no0 = 6;
    shootermove(shoot);
    TEST_ASSERT_EQ_INT(ctx, 2, shoot->r_no0);

    actwk[0].r_no0 = 0;
    actwk[0].xposi.w.h = 100;
    actwk[0].yposi.w.h = 400;
    shoot->xposi.w.h = 100;
    shoot->yposi.w.h = 200;
    shootermove(shoot);
    TEST_ASSERT_EQ_INT(ctx, 2, shoot->r_no0);

    actwk[0].yposi.w.h = 200;
    player_work_get(&actwk[0])->status_flags = 1;
    shootermove(shoot);
    TEST_ASSERT_EQ_INT(ctx, 2, shoot->r_no0);

    reset_shoot6_state();
    shoot->xposi.w.h = 100;
    shoot->yposi.w.h = 200;
    actwk[0].xposi.w.h = 100;
    actwk[0].yposi.w.h = 200;
    actwk[0].mspeed.w = 2048;
    shooterinit(shoot);
    reset_logs();
    shooter_get_work(shoot)->move_timer_high = 0;
    shooter_get_work(shoot)->move_index = 0;
    shooter_get_work(shoot)->move_limit = 12;
    shootermove3(shoot);
    TEST_ASSERT_EQ_INT(ctx, 6, shooter_get_work(shoot)->move_index);

    reset_shoot6_state();
    shoot->xposi.w.h = 100;
    shoot->yposi.w.h = 200;
    actwk[0].xposi.w.h = 100;
    actwk[0].yposi.w.h = 200;
    actwk[0].mspeed.w = 2048;
    shooterinit(shoot);
    shootermove2(shoot);
    reset_logs();
    shooter_get_work(shoot)->move_timer_high = 1;
    shooter_get_work(shoot)->move_index = 56;
    shooter_get_work(shoot)->move_limit = 78;
    swdata1.b.h = 0;
    shooter(shoot);
    TEST_ASSERT_EQ_INT(ctx, 56, shooter_get_work(shoot)->move_index);

    reset_logs();
    shooter_get_work(shoot)->move_timer_high = 1;
    shooter_get_work(shoot)->move_index = 56;
    shooter_get_work(shoot)->move_limit = 78;
    swdata1.b.h = 16;
    shootermove3(shoot);
    TEST_ASSERT_EQ_INT(ctx, 158, shooter_get_work(shoot)->move_index);

    reset_shoot6_state();
    shoot->xposi.w.h = 100;
    shoot->yposi.w.h = 200;
    actwk[0].xposi.w.h = 100;
    actwk[0].yposi.w.h = 200;
    actwk[0].mspeed.w = 2048;
    shooter_get_work(shoot)->target_x = 32;
    shooter_get_work(shoot)->target_y = 64;
    shooterspdset(shoot);
    TEST_ASSERT_EQ_INT(ctx, 1024, actwk[0].xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 2048, actwk[0].yspeed.w);

    shooter_get_work(shoot)->target_x = 32;
    shooter_get_work(shoot)->target_y = -64;
    shooterspdset(shoot);
    TEST_ASSERT_EQ_INT(ctx, 1024, actwk[0].xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, -2048, actwk[0].yspeed.w);

    shooter_get_work(shoot)->target_x = -64;
    shooter_get_work(shoot)->target_y = 32;
    shooterspdset(shoot);
    TEST_ASSERT_EQ_INT(ctx, 1024, actwk[0].yspeed.w);
    TEST_ASSERT_EQ_INT(ctx, -2048, actwk[0].xspeed.w);

    actwk[0].mspeed.w = -2048;
    shooter_get_work(shoot)->target_x = 0;
    shooter_get_work(shoot)->target_y = 64;
    shooterspdset(shoot);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, -2048, actwk[0].yspeed.w);

    shooter_get_work(shoot)->target_x = 64;
    shooter_get_work(shoot)->target_y = 32;
    shooterspdset(shoot);
    TEST_ASSERT_EQ_INT(ctx, -2048, actwk[0].xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, -1024, actwk[0].yspeed.w);
}

TEST_MAIN_BEGIN;
    test_megami_early_return_edges(&ctx);
    test_megami_spawns_rings_and_finishes(&ctx);
    test_gas_spawns_child_and_child_resets_parent(&ctx);
    test_gas_collision_freezes_player_and_rejects_guards(&ctx);
    test_gas_floor_launch_damage_and_ice_fragments(&ctx);
    test_gas_and_ice_early_return_edges(&ctx);
    test_catapult_cycle_and_child_follow(&ctx);
    test_catapult_early_return_edges(&ctx);
    test_shooter_route_and_speed_paths(&ctx);
    test_shooter_wrapper_and_route_edges(&ctx);
TEST_MAIN_END
