#include <string.h>

#include "support/test_runner.h"
#include "src/types.h"

sprite_status actwk[128];
Uint8 bossflag;
Uint8 bossstart;

static int actionsub_count;
static sprite_status *actionsub_actor;
static int patchg_count;
static sprite_status *patchg_actor;
static Uint8 **patchg_table;
static int colorset2_count;
static Sint32 colorset2_value;
static int actwkchk_count;
static int next_alloc_index;
static int actwkchk_fail_after;
static int sinset_count;
static Uint8 sinset_angle;
static Sint16 sinset_sin;
static Sint16 sinset_cos;
static int soundset_count;
static Sint16 soundset_requests[8];
static int frameout_count;
static sprite_status *frameout_actors[8];

void actionsub(sprite_status *pActwk);
void patchg(sprite_status *patchgwk, Uint8 **pat_dat);
void colorset2(Sint32 ColorNo);
Sint32 actwkchk(sprite_status **ppActwk);
void sinset(Uint8 kakudo, Sint16 *sin, Sint16 *cos);
void soundset(Sint16 ReqNo);
void frameout(sprite_status *pActwk);

#include "src/r4/boss_4.c"

void actionsub(sprite_status *pActwk) {
    ++actionsub_count;
    actionsub_actor = pActwk;
}

void patchg(sprite_status *patchgwk, Uint8 **pat_dat) {
    ++patchg_count;
    patchg_actor = patchgwk;
    patchg_table = pat_dat;
}

void colorset2(Sint32 ColorNo) {
    ++colorset2_count;
    colorset2_value = ColorNo;
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

void sinset(Uint8 kakudo, Sint16 *sin, Sint16 *cos) {
    ++sinset_count;
    sinset_angle = kakudo;
    *sin = sinset_sin;
    *cos = sinset_cos;
}

void soundset(Sint16 ReqNo) {
    if (soundset_count < 8) {
        soundset_requests[soundset_count] = ReqNo;
    }
    ++soundset_count;
}

void frameout(sprite_status *pActwk) {
    if (frameout_count < 8) {
        frameout_actors[frameout_count] = pActwk;
    }
    ++frameout_count;
}

static void reset_boss4_state(void) {
    memset(actwk, 0, sizeof(actwk));
    bossflag = 0;
    bossstart = 0;
    actionsub_count = 0;
    actionsub_actor = 0;
    patchg_count = 0;
    patchg_actor = 0;
    patchg_table = 0;
    colorset2_count = 0;
    colorset2_value = 0;
    actwkchk_count = 0;
    next_alloc_index = 20;
    actwkchk_fail_after = -1;
    sinset_count = 0;
    sinset_angle = 0;
    sinset_sin = 512;
    sinset_cos = 256;
    soundset_count = 0;
    memset(soundset_requests, 0, sizeof(soundset_requests));
    frameout_count = 0;
    memset(frameout_actors, 0, sizeof(frameout_actors));
}

static void test_tables_capture_literal_data(test_context *ctx) {
    TEST_ASSERT_TRUE(ctx, egg4air_pchg[0] == air_pchg0);
    TEST_ASSERT_TRUE(ctx, egg4air_pchg[5] == air_pchg5);
    TEST_ASSERT_TRUE(ctx, egg4air_pat[0] == &air_pat00);
    TEST_ASSERT_TRUE(ctx, egg4air_pat[8] == &air_pat08);
    TEST_ASSERT_EQ_INT(ctx, 439, air_pat00.spra[0].index);
    TEST_ASSERT_EQ_INT(ctx, 447, air_pat08.spra[0].index);
    TEST_ASSERT_TRUE(ctx, egg4airhead_pchg[1] == head_pchg1);
    TEST_ASSERT_TRUE(ctx, egg4airhead_pat[3] == &head_pat3);
    TEST_ASSERT_EQ_INT(ctx, 1232, egg4_air_tbl1[0].E4A_X);
    TEST_ASSERT_EQ_INT(ctx, 736, egg4_air_tbl2[2].E4A_Y);
    TEST_ASSERT_EQ_INT(ctx, 4, egg4_air_tbl3[2].E4A_VEC);
}

static void test_egg4air_initializes_and_links_head(test_context *ctx) {
    sprite_status *actor = &actwk[4];

    reset_boss4_state();
    actor->xposi.w.h = 123;
    actor->yposi.w.h = 456;

    egg4air(actor);

    TEST_ASSERT_EQ_INT(ctx, 61, actor->colino);
    TEST_ASSERT_EQ_INT(ctx, 5, actor->colicnt);
    TEST_ASSERT_EQ_INT(ctx, 1, actor->cddat);
    TEST_ASSERT_EQ_INT(ctx, 8, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 4, actor->actflg);
    TEST_ASSERT_EQ_INT(ctx, 6, actor->sprpri);
    TEST_ASSERT_EQ_INT(ctx, 36, actor->sprhsize);
    TEST_ASSERT_EQ_INT(ctx, 56, actor->sprvsize);
    TEST_ASSERT_EQ_INT(ctx, 798, actor->sproffset);
    TEST_ASSERT_TRUE(ctx, actor->patbase == egg4air_pat);
    TEST_ASSERT_EQ_INT(ctx, 1, colorset2_count);
    TEST_ASSERT_EQ_INT(ctx, 5, colorset2_value);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 20, boss4_get_work(actor)->linked_actor_index);
    TEST_ASSERT_EQ_INT(ctx, 4, boss4_get_work(&actwk[20])->linked_actor_index);
    TEST_ASSERT_EQ_INT(ctx, 75, actwk[20].actno);
    TEST_ASSERT_EQ_INT(ctx, 123, actwk[20].xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 456, actwk[20].yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, patchg_count);
    TEST_ASSERT_TRUE(ctx, patchg_actor == actor);
    TEST_ASSERT_TRUE(ctx, patchg_table == egg4air_pchg);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_TRUE(ctx, actionsub_actor == actor);

    reset_boss4_state();
    actwkchk_fail_after = 0;
    actor = &actwk[4];
    egg4air(actor);
    TEST_ASSERT_EQ_INT(ctx, 0, boss4_get_work(actor)->linked_actor_index);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
}

static void test_egg4air_drop_and_escape_edges(test_context *ctx) {
    sprite_status *actor = &actwk[5];

    reset_boss4_state();
    actor->r_no0 = 4;
    actor->yposi.w.h = 1408;
    boss4_get_work(actor)->linked_actor_index = 20;
    actwk[20].r_no0 = 2;

    egg4air(actor);

    TEST_ASSERT_EQ_INT(ctx, 4, actwk[20].r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actors[0] == actor);
    TEST_ASSERT_EQ_INT(ctx, 0, patchg_count);
    TEST_ASSERT_EQ_INT(ctx, 0, actionsub_count);

    reset_boss4_state();
    actor = &actwk[5];
    actor->r_no0 = 8;
    actwk[0].xposi.w.h = 1144;

    egg4air(actor);

    TEST_ASSERT_EQ_INT(ctx, 2, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 4, bossstart);
    TEST_ASSERT_EQ_INT(ctx, 1, patchg_count);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);

    reset_boss4_state();
    actor = &actwk[5];
    actor->r_no0 = 4;
    actor->yposi.w.h = 1000;

    egg4air(actor);

    TEST_ASSERT_EQ_INT(ctx, 1, patchg_count);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);

    reset_boss4_state();
    actor = &actwk[5];
    actor->r_no0 = 8;
    boss4_get_work(actor)->damage_flash_timer = 2;
    actor->mstno.b.h = 1;

    egg4air(actor);

    TEST_ASSERT_EQ_INT(ctx, 1, boss4_get_work(actor)->damage_flash_timer);
    TEST_ASSERT_EQ_INT(ctx, 1, actor->mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 1, patchg_count);
}

static void test_egg4air_combat_spawns_bombs_and_shakes(test_context *ctx) {
    sprite_status *actor = &actwk[6];

    reset_boss4_state();
    actor->r_no0 = 6;
    actor->xposi.w.h = 300;
    actor->yposi.w.h = 300;
    boss4_get_work(actor)->linked_actor_index = 20;
    boss4_get_work(actor)->bob_y_offset = 12;
    actwk[0].xposi.w.h = 700;
    actwk[0].yposi.w.h = 300;

    egg4air(actor);

    TEST_ASSERT_EQ_INT(ctx, 30, boss4_get_work(&actwk[20])->bomb_timer);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 24, actwk[20].actno);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[20].r_no1);
    TEST_ASSERT_EQ_INT(ctx, 300, actwk[20].xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 300, actwk[20].yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 158, soundset_requests[0]);
    TEST_ASSERT_TRUE(ctx, (boss4_get_work(actor)->approach_flags & 32) != 0);
    TEST_ASSERT_EQ_INT(ctx, 1, sinset_count);
    TEST_ASSERT_EQ_INT(ctx, 2, boss4_get_work(actor)->bob_angle);
    TEST_ASSERT_EQ_INT(ctx, 1, patchg_count);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
}

static void test_egg4air_path_helpers_cover_table_edges(test_context *ctx) {
    sprite_status *actor = &actwk[6];
    E4A step;

    reset_boss4_state();
    step.E4A_ETC = 0;
    actor->r_no1 = 18;
    egg4air_01_next(actor, &step);
    TEST_ASSERT_EQ_INT(ctx, 0, actor->r_no1);

    reset_boss4_state();
    step.E4A_ETC = 1;
    boss4_get_work(actor)->linked_actor_index = 20;
    boss4_get_work(&actwk[20])->gate1_index = 21;
    egg4air_01_next(actor, &step);
    TEST_ASSERT_EQ_INT(ctx, 128, actor->r_no1);
    /* The gate is a wall4 actor, so the signal has to land in its own work
       area - that is what wall4_wait() reads to drop the gate. */
    TEST_ASSERT_EQ_INT(ctx, 1, wall4_get_work(&actwk[21])->triggered);

    reset_boss4_state();
    step.E4A_ETC = 2;
    boss4_get_work(actor)->linked_actor_index = 20;
    boss4_get_work(&actwk[20])->gate2_index = 22;
    egg4air_01_next(actor, &step);
    TEST_ASSERT_EQ_INT(ctx, 64, actor->r_no1);
    TEST_ASSERT_EQ_INT(ctx, 1, wall4_get_work(&actwk[22])->triggered);

    reset_boss4_state();
    step.E4A_ETC = 3;
    actor->mstno.b.h = 2;
    egg4air_01_next(actor, &step);
    TEST_ASSERT_EQ_INT(ctx, 4, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, actor->mstno.b.h);
}

static void test_egg4air_01_table_vectors(test_context *ctx) {
    sprite_status *actor = &actwk[6];
    E4A saved_step;

    reset_boss4_state();
    actor->r_no1 = 128;
    actor->xposi.w.h = 2000;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4air_01(actor));
    TEST_ASSERT_EQ_INT(ctx, 129, actor->r_no1);

    reset_boss4_state();
    actor->r_no1 = 64;
    actor->yposi.w.h = 1000;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4air_01(actor));
    TEST_ASSERT_EQ_INT(ctx, 65, actor->r_no1);
    TEST_ASSERT_EQ_INT(ctx, 1088, actor->yposi.w.h);

    reset_boss4_state();
    actor->r_no1 = 0;
    actor->yposi.w.h = 1100;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4air_01(actor));
    TEST_ASSERT_EQ_INT(ctx, 1, actor->r_no1);
    TEST_ASSERT_EQ_INT(ctx, 1056, actor->yposi.w.h);

    reset_boss4_state();
    actor->r_no1 = 5;
    actor->xposi.w.h = 1300;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4air_01(actor));
    TEST_ASSERT_EQ_INT(ctx, 6, actor->r_no1);
    TEST_ASSERT_TRUE(ctx, (actor->cddat & 1) != 0);

    reset_boss4_state();
    actor->r_no1 = 17;
    actor->xposi.w.h = 1696;
    actor->yposi.w.h = 816;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4air_01(actor));
    TEST_ASSERT_EQ_INT(ctx, 18, actor->r_no1);

    reset_boss4_state();
    actor->r_no1 = 1;
    actor->xposi.w.h = 1248;
    actor->yposi.w.h = 1072;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4air_01(actor));
    TEST_ASSERT_EQ_INT(ctx, 2, actor->r_no1);

    reset_boss4_state();
    actor->r_no1 = 66;
    actor->xposi.w.h = 1956;
    actor->yposi.w.h = 988;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4air_01(actor));
    TEST_ASSERT_EQ_INT(ctx, 67, actor->r_no1);

    reset_boss4_state();
    actor->r_no1 = 11;
    actor->xposi.w.h = 1000;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4air_01(actor));
    TEST_ASSERT_EQ_INT(ctx, 1, sinset_count);

    reset_boss4_state();
    actor->r_no1 = 0;
    actor->yposi.w.h = 900;
    boss4_get_work(actor)->bob_y_offset = 7;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4air_01(actor));
    TEST_ASSERT_EQ_INT(ctx, 900, actor->yposi.w.h);

    reset_boss4_state();
    actor->r_no1 = 18;
    actor->xposi.w.h = 1400;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4air_01(actor));
    TEST_ASSERT_EQ_INT(ctx, 1, sinset_count);

    reset_boss4_state();
    actor->r_no1 = 8;
    actor->yposi.w.h = 2176;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4air_01(actor));
    TEST_ASSERT_EQ_INT(ctx, 128, actor->yposi.w.h);

    reset_boss4_state();
    actor->r_no1 = 17;
    actor->xposi.w.h = 1800;
    actor->yposi.w.h = 700;
    boss4_get_work(actor)->bob_y_offset = 9;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4air_01(actor));
    TEST_ASSERT_EQ_INT(ctx, 700, actor->yposi.w.h);

    reset_boss4_state();
    actor->r_no1 = 1;
    actor->xposi.w.h = 1200;
    actor->yposi.w.h = 900;
    boss4_get_work(actor)->bob_y_offset = 11;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4air_01(actor));
    TEST_ASSERT_EQ_INT(ctx, 900, actor->yposi.w.h);

    reset_boss4_state();
    actor->r_no1 = 64;
    actor->yposi.w.h = 1200;
    boss4_get_work(actor)->bob_y_offset = 13;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4air_01(actor));
    TEST_ASSERT_EQ_INT(ctx, 1200, actor->yposi.w.h);

    reset_boss4_state();
    actor->r_no1 = 66;
    actor->xposi.w.h = 1800;
    actor->yposi.w.h = 1200;
    boss4_get_work(actor)->bob_y_offset = 15;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4air_01(actor));
    TEST_ASSERT_EQ_INT(ctx, 1200, actor->yposi.w.h);

    saved_step = egg4_air_tbl3[0];
    egg4_air_tbl3[0].E4A_VEC = 3;
    egg4_air_tbl3[0].E4A_ETC = 0;
    egg4_air_tbl3[0].E4A_X = 1596;
    egg4_air_tbl3[0].E4A_Y = 1088;

    reset_boss4_state();
    actor->r_no1 = 64;
    actor->xposi.w.h = 1596;
    actor->yposi.w.h = 1088;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4air_01(actor));
    TEST_ASSERT_EQ_INT(ctx, 65, actor->r_no1);
    TEST_ASSERT_EQ_INT(ctx, 2, actor->mstno.b.h);
    TEST_ASSERT_TRUE(ctx, (actor->cddat & 1) != 0);

    reset_boss4_state();
    actor->r_no1 = 64;
    actor->xposi.w.h = 1700;
    actor->yposi.w.h = 1200;
    boss4_get_work(actor)->bob_y_offset = 5;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4air_01(actor));
    TEST_ASSERT_EQ_INT(ctx, 1200, actor->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 64, actor->r_no1);

    egg4_air_tbl3[0] = saved_step;
}

static void test_egg4air_hitchk_variants(test_context *ctx) {
    sprite_status *actor = &actwk[6];

    reset_boss4_state();
    boss4_get_work(actor)->speed_boost_timer = 1;
    egg4air_hitchk(actor);
    TEST_ASSERT_EQ_INT(ctx, 0, boss4_get_work(actor)->speed_boost_timer);

    reset_boss4_state();
    actor->colicnt = 2;
    boss4_get_work(actor)->hit_invulnerability_timer = 1;
    actor->colino = 0;
    egg4air_hitchk(actor);
    TEST_ASSERT_EQ_INT(ctx, 61, actor->colino);

    reset_boss4_state();
    actor->colicnt = 2;
    actor->colino = 0;
    boss4_get_work(actor)->linked_actor_index = 20;
    egg4air_hitchk(actor);
    TEST_ASSERT_EQ_INT(ctx, 172, soundset_requests[0]);
    TEST_ASSERT_EQ_INT(ctx, 40, boss4_get_work(&actwk[20])->bomb_timer);
    TEST_ASSERT_EQ_INT(ctx, 70, boss4_get_work(actor)->speed_boost_timer);
    TEST_ASSERT_EQ_INT(ctx, 16, boss4_get_work(actor)->damage_flash_timer);
    TEST_ASSERT_EQ_INT(ctx, 30,
                       boss4_get_work(actor)->hit_invulnerability_timer);

    reset_boss4_state();
    actor->colicnt = 1;
    actor->colino = 0;
    boss4_get_work(actor)->linked_actor_index = 20;
    egg4air_hitchk(actor);
    TEST_ASSERT_EQ_INT(ctx, 6, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, actor->colicnt);
}

static void test_egg4air_combat_distance_bands(test_context *ctx) {
    sprite_status *actor = &actwk[6];

    reset_boss4_state();
    actor->xposi.w.h = 300;
    actor->yposi.w.h = 300;
    boss4_get_work(actor)->bomb_timer = 1;
    actor->xspeed.w = 2049;
    actwk[0].xposi.w.h = 300;
    actwk[0].yposi.w.h = 300;
    e4a_03_most_near(actor);
    TEST_ASSERT_TRUE(ctx, actor->mstno.b.h <= 2);

    reset_boss4_state();
    actor->xposi.w.h = 300;
    actor->yposi.w.h = 300;
    boss4_get_work(actor)->bomb_timer = 1;
    actor->xspeed.w = 2048;
    actwk[0].xposi.w.h = 400;
    actwk[0].yposi.w.h = 300;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4air_03(actor));

    reset_boss4_state();
    actor->xposi.w.h = 300;
    actor->yposi.w.h = 300;
    boss4_get_work(actor)->bomb_timer = 1;
    actor->xspeed.w = 2048;
    actwk[0].xposi.w.h = 500;
    actwk[0].yposi.w.h = 300;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4air_03(actor));

    reset_boss4_state();
    actor->xposi.w.h = 300;
    actor->yposi.w.h = 300;
    boss4_get_work(actor)->bomb_timer = 1;
    boss4_get_work(actor)->approach_flags = 32;
    actwk[0].xposi.w.h = 500;
    actwk[0].yposi.w.h = 300;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4air_03(actor));
    TEST_ASSERT_TRUE(ctx, (boss4_get_work(actor)->approach_flags & 32) != 0);

    reset_boss4_state();
    actor->xposi.w.h = 300;
    actor->yposi.w.h = 300;
    boss4_get_work(actor)->bomb_timer = 1;
    boss4_get_work(actor)->approach_flags = 32;
    actwk[0].xposi.w.h = 320;
    actwk[0].yposi.w.h = 300;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4air_03(actor));
    TEST_ASSERT_EQ_INT(ctx, 0, boss4_get_work(actor)->approach_flags & 32);

    reset_boss4_state();
    actor->xposi.w.h = 300;
    actor->yposi.w.h = 300;
    boss4_get_work(actor)->bomb_timer = 1;
    actwk[0].xposi.w.h = 250;
    actwk[0].yposi.w.h = 250;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4air_03(actor));
}

static void test_egg4airhead_initializes_gates_and_follows_body(test_context *ctx) {
    sprite_status *head = &actwk[7];

    reset_boss4_state();
    egg4airhead(head);

    TEST_ASSERT_EQ_INT(ctx, 2, head->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 4, head->actflg);
    TEST_ASSERT_EQ_INT(ctx, 6, head->sprpri);
    TEST_ASSERT_EQ_INT(ctx, 36, head->sprhsize);
    TEST_ASSERT_EQ_INT(ctx, 56, head->sprvsize);
    TEST_ASSERT_EQ_INT(ctx, 798, head->sproffset);
    TEST_ASSERT_TRUE(ctx, head->patbase == egg4airhead_pat);
    TEST_ASSERT_EQ_INT(ctx, 20, boss4_get_work(head)->gate1_index);
    TEST_ASSERT_EQ_INT(ctx, 21, boss4_get_work(head)->gate2_index);
    TEST_ASSERT_EQ_INT(ctx, 71, actwk[20].actno);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[20].userflag.b.h);
    TEST_ASSERT_EQ_INT(ctx, 1888, actwk[20].xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 704, actwk[20].yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 71, actwk[21].actno);
    TEST_ASSERT_EQ_INT(ctx, 1728, actwk[21].xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1120, actwk[21].yposi.w.h);

    reset_boss4_state();
    head = &actwk[7];
    head->r_no0 = 2;
    boss4_get_work(head)->bomb_timer = 2;
    boss4_get_work(head)->linked_actor_index = 20;
    actwk[20].xposi.w.h = 111;
    actwk[20].yposi.w.h = 222;
    actwk[20].cddat = 3;

    egg4airhead(head);

    TEST_ASSERT_EQ_INT(ctx, 1, boss4_get_work(head)->bomb_timer);
    TEST_ASSERT_EQ_INT(ctx, 1, head->mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 111, head->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 222, head->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 3, head->cddat);
    TEST_ASSERT_EQ_INT(ctx, 1, patchg_count);
    TEST_ASSERT_TRUE(ctx, patchg_table == egg4airhead_pchg);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);

    reset_boss4_state();
    head = &actwk[7];
    head->r_no0 = 2;
    boss4_get_work(head)->bomb_timer = 1;
    head->mstno.b.h = 1;
    head->patno = 5;
    head->patcnt = 6;
    head->pattim = 7;
    head->pattimm = 8;

    egg4airhead(head);

    TEST_ASSERT_EQ_INT(ctx, 0, boss4_get_work(head)->bomb_timer);
    TEST_ASSERT_EQ_INT(ctx, 0, head->mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 0, head->patno);
    TEST_ASSERT_EQ_INT(ctx, 0, head->patcnt);
    TEST_ASSERT_EQ_INT(ctx, 0, head->pattim);
    TEST_ASSERT_EQ_INT(ctx, 0, head->pattimm);
}

static void test_egg4airhead_goal_frameout(test_context *ctx) {
    sprite_status *head = &actwk[7];

    reset_boss4_state();
    head->r_no0 = 4;
    boss4_get_work(head)->gate1_index = 20;
    boss4_get_work(head)->gate2_index = 21;
    actwk[20].actno = 71;
    actwk[21].actno = 71;
    actwk[0].xposi.w.h = 2112;
    actwk[0].yposi.w.h = 1360;

    egg4airhead(head);

    TEST_ASSERT_EQ_INT(ctx, 4, bossflag);
    TEST_ASSERT_EQ_INT(ctx, 3, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actors[0] == &actwk[20]);
    TEST_ASSERT_TRUE(ctx, frameout_actors[1] == &actwk[21]);
    TEST_ASSERT_TRUE(ctx, frameout_actors[2] == head);
}

TEST_MAIN_BEGIN;
test_tables_capture_literal_data(&ctx);
test_egg4air_initializes_and_links_head(&ctx);
test_egg4air_drop_and_escape_edges(&ctx);
test_egg4air_combat_spawns_bombs_and_shakes(&ctx);
test_egg4air_path_helpers_cover_table_edges(&ctx);
test_egg4air_01_table_vectors(&ctx);
test_egg4air_hitchk_variants(&ctx);
test_egg4air_combat_distance_bands(&ctx);
test_egg4airhead_initializes_gates_and_follows_body(&ctx);
test_egg4airhead_goal_frameout(&ctx);
TEST_MAIN_END
