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
Uint8 bossstart;
Uint8 generate_flag;
Sint16 scralim_left;
Sint16 scralim_right;
Sint16 scralim_n_left;
Sint16 scralim_n_right;
Uint16 scr_dir_tbl[5];

static int actionsub_count;
static sprite_status *actionsub_actor;
static int patchg_count;
static sprite_status *patchg_actor;
static Uint8 **patchg_table;
static int actwkchk_count;
static int next_alloc_index;
static int actwkchk_fail_after;
static int sub_sync_count;
static Sint16 sub_sync_requests[8];
static int genecolor_count;
static int frameout_count;
static sprite_status *frameout_actors[16];
static int soundset_count;
static Sint16 soundset_requests[16];
static int sinset_count;
static Uint8 sinset_angles[16];
static Sint16 sinset_sin;
static Sint16 sinset_cos;
static int scoreup_count;
static Uint32 scoreup_values[4];
static Sint32 random_values[8];
static int random_index;

void actionsub(sprite_status *pActwk);
void patchg(sprite_status *patchgwk, Uint8 **pat_dat);
Sint32 actwkchk(sprite_status **ppActwk);
void sub_sync(Sint16 ReqNo);
void genecolor(void);
void frameout(sprite_status *pActwk);
void soundset(Sint16 ReqNo);
void sinset(Uint8 kakudo, Sint16 *sin, Sint16 *cos);
void scoreup(Uint32 updata);
Sint32 random(void);

#include "src/r4/boss_4_2.c"

void actionsub(sprite_status *pActwk) {
    ++actionsub_count;
    actionsub_actor = pActwk;
}

void patchg(sprite_status *patchgwk, Uint8 **pat_dat) {
    ++patchg_count;
    patchg_actor = patchgwk;
    patchg_table = pat_dat;
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

void sub_sync(Sint16 ReqNo) {
    if (sub_sync_count < 8) {
        sub_sync_requests[sub_sync_count] = ReqNo;
    }
    ++sub_sync_count;
}

void genecolor(void) { ++genecolor_count; }

void frameout(sprite_status *pActwk) {
    if (frameout_count < 16) {
        frameout_actors[frameout_count] = pActwk;
    }
    ++frameout_count;
}

void soundset(Sint16 ReqNo) {
    if (soundset_count < 16) {
        soundset_requests[soundset_count] = ReqNo;
    }
    ++soundset_count;
}

void sinset(Uint8 kakudo, Sint16 *sin, Sint16 *cos) {
    if (sinset_count < 16) {
        sinset_angles[sinset_count] = kakudo;
    }
    ++sinset_count;
    *sin = sinset_sin;
    *cos = sinset_cos;
}

void scoreup(Uint32 updata) {
    if (scoreup_count < 4) {
        scoreup_values[scoreup_count] = updata;
    }
    ++scoreup_count;
}

Sint32 random(void) {
    Sint32 value = random_values[random_index];
    if (random_index < 7) {
        ++random_index;
    }
    return value;
}

static void reset_boss42_state(void) {
    memset(actwk, 0, sizeof(actwk));
    bossstart = 0;
    generate_flag = 0;
    scralim_left = 0;
    scralim_right = 0;
    scralim_n_left = 0;
    scralim_n_right = 0;
    memset(scr_dir_tbl, 0, sizeof(scr_dir_tbl));
    actionsub_count = 0;
    actionsub_actor = 0;
    patchg_count = 0;
    patchg_actor = 0;
    patchg_table = 0;
    actwkchk_count = 0;
    next_alloc_index = 20;
    actwkchk_fail_after = -1;
    sub_sync_count = 0;
    memset(sub_sync_requests, 0, sizeof(sub_sync_requests));
    genecolor_count = 0;
    frameout_count = 0;
    memset(frameout_actors, 0, sizeof(frameout_actors));
    soundset_count = 0;
    memset(soundset_requests, 0, sizeof(soundset_requests));
    sinset_count = 0;
    memset(sinset_angles, 0, sizeof(sinset_angles));
    sinset_sin = 256;
    sinset_cos = 256;
    scoreup_count = 0;
    memset(scoreup_values, 0, sizeof(scoreup_values));
    memset(random_values, 0, sizeof(random_values));
    random_index = 0;
}

static void test_tables_capture_literal_data(test_context *ctx) {
    TEST_ASSERT_TRUE(ctx, egg4_pat[0] == &egg4_pat0);
    TEST_ASSERT_TRUE(ctx, egg4_pat[6] == &egg4_pat6);
    TEST_ASSERT_EQ_INT(ctx, 452, egg4_pat0.spra[0].index);
    TEST_ASSERT_EQ_INT(ctx, 458, egg4_pat6.spra[0].index);
    TEST_ASSERT_TRUE(ctx, egg4meca_pat[1] == &egg4meca_pat1);
    TEST_ASSERT_EQ_INT(ctx, 460, egg4meca_pat1.spra[0].index);
    TEST_ASSERT_TRUE(ctx, egg4awa_pchg[4] == egg4awa_pchg4);
    TEST_ASSERT_TRUE(ctx, egg4awa_pat[8] == &egg4awa_pat8);
    TEST_ASSERT_EQ_INT(ctx, 471, egg4awa_pat8.spra[0].index);
    TEST_ASSERT_TRUE(ctx, egg4tama_pat[0] == &egg4tama_pat0);
    TEST_ASSERT_TRUE(ctx, egg4tama_pchg[0] == egg4tama_pchg0);
}

static void test_egg4_initializes_links_and_laughs(test_context *ctx) {
    sprite_status *boss = &actwk[3];

    reset_boss42_state();
    boss->xposi.w.h = 1234;
    boss->yposi.w.h = 1300;

    egg4(boss);

    TEST_ASSERT_EQ_INT(ctx, 2, boss->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 4, boss->actflg);
    TEST_ASSERT_EQ_INT(ctx, 4, boss->sprpri);
    TEST_ASSERT_EQ_INT(ctx, 32, boss->sprhsize);
    TEST_ASSERT_EQ_INT(ctx, 44, boss->sprvsize);
    TEST_ASSERT_EQ_INT(ctx, 798, boss->sproffset);
    TEST_ASSERT_TRUE(ctx, boss->patbase == egg4_pat);
    TEST_ASSERT_EQ_INT(ctx, 2, boss->colicnt);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 20, egg4_get_work(boss)->child_index);
    TEST_ASSERT_EQ_INT(ctx, 3, egg4_get_work(&actwk[20])->parent_index);
    TEST_ASSERT_EQ_INT(ctx, 77, actwk[20].actno);
    TEST_ASSERT_EQ_INT(ctx, 1234, actwk[20].xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1300, actwk[20].yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, patchg_count);
    TEST_ASSERT_TRUE(ctx, patchg_actor == boss);
    TEST_ASSERT_TRUE(ctx, patchg_table == egg4_pchg);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_TRUE(ctx, actionsub_actor == boss);

    reset_boss42_state();
    boss = &actwk[3];
    boss->r_no0 = 2;
    actwk[0].r_no0 = 6;
    egg4(boss);
    TEST_ASSERT_EQ_INT(ctx, 120, egg4_get_work(boss)->laugh_timer);
    TEST_ASSERT_EQ_INT(ctx, 1, boss->mstno.b.h);

    reset_boss42_state();
    boss = &actwk[3];
    boss->r_no0 = 2;
    egg4_get_work(boss)->laugh_timer = 1;
    boss->mstno.b.h = 1;
    boss->patno = 5;
    boss->patcnt = 6;
    boss->pattim = 7;
    boss->pattimm = 8;
    egg4(boss);
    TEST_ASSERT_EQ_INT(ctx, 0, boss->mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 0, boss->patno);
    TEST_ASSERT_EQ_INT(ctx, 0, boss->patcnt);
    TEST_ASSERT_EQ_INT(ctx, 0, boss->pattim);
    TEST_ASSERT_EQ_INT(ctx, 0, boss->pattimm);

    reset_boss42_state();
    boss = &actwk[3];
    actwkchk_fail_after = 0;
    egg4_ini(boss);
    TEST_ASSERT_EQ_INT(ctx, 0, egg4_get_work(boss)->child_index);
}

static void test_egg4_screen_and_demo_states(test_context *ctx) {
    sprite_status *boss = &actwk[4];

    reset_boss42_state();
    boss->r_no0 = 2;
    actwk[0].xposi.w.h = 2700;
    scralim_left = 100;

    TEST_ASSERT_EQ_INT(ctx, 1, egg4_scrset(boss));
    TEST_ASSERT_EQ_INT(ctx, 6, boss->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 2480, scralim_left);
    TEST_ASSERT_EQ_INT(ctx, 2480, scralim_right);
    TEST_ASSERT_EQ_INT(ctx, 1, sub_sync_count);
    TEST_ASSERT_EQ_INT(ctx, 103, sub_sync_requests[0]);

    reset_boss42_state();
    boss = &actwk[4];
    scralim_right = 100;
    scralim_n_right = 100;
    scr_dir_tbl[2] = 120;
    TEST_ASSERT_EQ_INT(ctx, 0, egg4_scrreset(boss));
    TEST_ASSERT_EQ_INT(ctx, 106, scralim_right);
    TEST_ASSERT_EQ_INT(ctx, 0, frameout_count);

    reset_boss42_state();
    boss = &actwk[4];
    bossstart = 4;
    scralim_right = 116;
    scralim_n_right = 116;
    scr_dir_tbl[2] = 120;
    generate_flag = 1;
    TEST_ASSERT_EQ_INT(ctx, 0, egg4_scrreset(boss));
    TEST_ASSERT_EQ_INT(ctx, 0, bossstart);
    TEST_ASSERT_EQ_INT(ctx, 120, scralim_right);
    TEST_ASSERT_EQ_INT(ctx, 22, sub_sync_requests[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, genecolor_count);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actors[0] == boss);

    reset_boss42_state();
    boss = &actwk[4];
    scralim_right = 116;
    scralim_n_right = 116;
    scr_dir_tbl[2] = 120;
    TEST_ASSERT_EQ_INT(ctx, 0, egg4_scrreset(boss));
    TEST_ASSERT_EQ_INT(ctx, 23, sub_sync_requests[0]);

    reset_boss42_state();
    boss = &actwk[4];
    boss->yposi.w.h = 1200;
    egg4_get_work(boss)->timer = 9;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4_awademo1(boss));
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 79, actwk[20].actno);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[20].r_no1);

    reset_boss42_state();
    boss = &actwk[4];
    boss->yposi.w.h = 1400;
    egg4_get_work(boss)->timer = 19;
    egg4_get_work(boss)->bubble_slot = 15;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4_awademo1(boss));
    TEST_ASSERT_EQ_INT(ctx, 2736, boss->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1312, boss->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 182, soundset_requests[0]);
    TEST_ASSERT_EQ_INT(ctx, 79, actwk[20].actno);
    TEST_ASSERT_EQ_INT(ctx, 15, egg4_get_work(&actwk[20])->bubble_slot);
    TEST_ASSERT_EQ_INT(ctx, 8, boss->r_no0);

    reset_boss42_state();
    boss = &actwk[4];
    egg4_get_work(boss)->timer = 149;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4_awademo2(boss));
    TEST_ASSERT_EQ_INT(ctx, 0, egg4_get_work(boss)->timer);
    TEST_ASSERT_EQ_INT(ctx, 10, boss->r_no0);

    reset_boss42_state();
    boss = &actwk[4];
    egg4_get_work(boss)->timer = 149;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4_awademo3(boss));
    TEST_ASSERT_EQ_INT(ctx, 12, boss->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 96, egg4_get_work(boss)->radius);
    TEST_ASSERT_EQ_INT(ctx, 64, boss->yspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 96, egg4_get_work(boss)->x_offset);
}

static void test_egg4_move_wait_die_and_escape(test_context *ctx) {
    sprite_status *boss = &actwk[5];

    reset_boss42_state();
    boss->r_no0 = 12;
    boss->xposi.w.h = 2600;
    boss->yposi.w.h = 1300;
    boss->yspeed.w = 16;
    egg4_get_work(boss)->angle_high = 32;
    egg4_get_work(boss)->angle = 1792;
    egg4_get_work(boss)->x_offset = 3;
    egg4_get_work(boss)->y_offset = 4;
    egg4_get_work(boss)->radius = 8;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4_movel(boss));
    TEST_ASSERT_EQ_INT(ctx, 4, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 78, actwk[20].actno);
    TEST_ASSERT_EQ_INT(ctx, 5, egg4_get_work(&actwk[20])->parent_index);
    TEST_ASSERT_EQ_INT(ctx, 1, sinset_count);
    TEST_ASSERT_EQ_INT(ctx, 8, sinset_angles[0]);

    reset_boss42_state();
    boss = &actwk[5];
    boss->r_no0 = 12;
    egg4_get_work(boss)->angle = 32512;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4_movel(boss));
    TEST_ASSERT_EQ_INT(ctx, 16, boss->r_no0);
    TEST_ASSERT_TRUE(ctx, (egg4_get_work(boss)->flags & 16) != 0);

    reset_boss42_state();
    boss = &actwk[5];
    boss->r_no0 = 14;
    egg4_get_work(boss)->angle = 30976;
    boss->cddat = 1;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4_mover(boss));
    TEST_ASSERT_EQ_INT(ctx, 4, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 29, egg4_get_work(&actwk[20])->x_offset);
    TEST_ASSERT_TRUE(ctx, egg4_get_work(&actwk[20])->radial_speed > 0);

    reset_boss42_state();
    boss = &actwk[5];
    egg4_get_work(boss)->angle = 256;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4_mover(boss));
    TEST_ASSERT_EQ_INT(ctx, 16, boss->r_no0);
    TEST_ASSERT_TRUE(ctx, (egg4_get_work(boss)->flags & 16) != 0);

    reset_boss42_state();
    boss = &actwk[5];
    egg4_get_work(boss)->child_index = 20;
    egg4_get_work(boss)->timer = 47;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4_wait(boss));
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[20].actflg);
    TEST_ASSERT_EQ_INT(ctx, 1, boss->actflg);

    egg4_get_work(boss)->timer = 95;
    boss->cddat = 1;
    egg4_get_work(boss)->bubble_slot = 13;
    egg4_get_work(boss)->bubble_position_index = 0;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4_wait(boss));
    TEST_ASSERT_EQ_INT(ctx, 22, boss->r_no0);
    TEST_ASSERT_TRUE(ctx, (egg4_get_work(boss)->flags & 8) != 0);

    reset_boss42_state();
    boss = &actwk[5];
    egg4_get_work(boss)->timer = 95;
    boss->cddat = 0;
    egg4_get_work(boss)->flags = 8;
    egg4_get_work(boss)->bubble_slot = 20;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4_wait(boss));
    TEST_ASSERT_EQ_INT(ctx, 12, boss->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, egg4_get_work(boss)->flags & 8);

    reset_boss42_state();
    boss = &actwk[5];
    egg4_get_work(boss)->child_index = 20;
    boss->colino = 0;
    boss->colicnt = 1;
    egg4_hitchk(boss);
    TEST_ASSERT_EQ_INT(ctx, 18, boss->r_no0);
    TEST_ASSERT_TRUE(ctx, (egg4_get_work(boss)->flags & 128) != 0);
    TEST_ASSERT_EQ_INT(ctx, 60, boss->colino);

    reset_boss42_state();
    boss = &actwk[5];
    egg4_get_work(boss)->child_index = 20;
    egg4_get_work(boss)->timer = 3;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4_die(boss));
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 24, actwk[20].actno);
    TEST_ASSERT_EQ_INT(ctx, 158, soundset_requests[0]);

    egg4_get_work(boss)->timer = 59;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4_die(boss));
    TEST_ASSERT_EQ_INT(ctx, 3, boss->mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 255, egg4_get_work(boss)->laugh_timer);

    egg4_get_work(boss)->timer = 60;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4_die(boss));
    TEST_ASSERT_EQ_INT(ctx, 20, boss->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, scoreup_count);
    TEST_ASSERT_EQ_INT(ctx, 100, scoreup_values[0]);

    reset_boss42_state();
    boss = &actwk[5];
    egg4_get_work(boss)->child_index = 20;
    egg4_get_work(boss)->timer = 59;
    boss->xposi.w.h = 2863;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4_esc(boss));
    TEST_ASSERT_EQ_INT(ctx, 4, boss->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actors[0] == &actwk[20]);

    reset_boss42_state();
    boss = &actwk[5];
    egg4_get_work(boss)->timer = 58;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4_esc(boss));
    TEST_ASSERT_EQ_INT(ctx, 0, frameout_count);
}

static void test_egg4_movec_edges(test_context *ctx) {
    sprite_status *boss = &actwk[6];

    reset_boss42_state();
    boss->xposi.w.h = 2735;
    boss->yposi.w.h = 1200;
    egg4_get_work(boss)->timer = 19;
    egg4_get_work(boss)->flags = 8;
    egg4_get_work(boss)->xy_offset = 77;
    egg4_get_work(boss)->x_offset = 1;
    egg4_get_work(boss)->y_offset = 2;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4_movec(boss));
    TEST_ASSERT_EQ_INT(ctx, 16, boss->r_no0);
    TEST_ASSERT_EQ_INT(ctx, -131073, egg4_get_work(boss)->xy_offset);
    TEST_ASSERT_EQ_INT(ctx, 32768, (Uint16)egg4_get_work(boss)->angle);
    TEST_ASSERT_EQ_INT(ctx, 1, boss->mstno.b.h);
    TEST_ASSERT_TRUE(ctx, (egg4_get_work(boss)->flags & 16) != 0);

    reset_boss42_state();
    boss = &actwk[6];
    boss->xposi.w.h = 2500;
    egg4_get_work(boss)->timer = 20;
    egg4_get_work(boss)->flags = 32;
    egg4_get_work(boss)->xy_offset = 3;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4_movec(boss));
    TEST_ASSERT_EQ_INT(ctx, 16, boss->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, egg4_get_work(boss)->flags & 32);

    reset_boss42_state();
    boss = &actwk[6];
    boss->xposi.w.h = 2600;
    egg4_get_work(boss)->flags = 8;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4_movec(boss));
    TEST_ASSERT_EQ_INT(ctx, 0, boss->r_no0);

    reset_boss42_state();
    boss = &actwk[6];
    boss->xposi.w.h = 2600;
    egg4_get_work(boss)->timer = 44;
    actwkchk_fail_after = 2;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4_movec(boss));
    TEST_ASSERT_EQ_INT(ctx, 3, actwkchk_count);

    TEST_ASSERT_EQ_INT(ctx, 1, egg4_movec2(boss));
    TEST_ASSERT_EQ_INT(ctx, 1, egg4_movec3(boss));
}

static void test_egg4meca_follows_parent(test_context *ctx) {
    sprite_status *meca = &actwk[7];

    reset_boss42_state();
    egg4_get_work(meca)->parent_index = 20;
    actwk[20].xposi.w.h = 1000;
    actwk[20].yposi.w.h = 1200;

    egg4meca(meca);

    TEST_ASSERT_EQ_INT(ctx, 2, meca->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 4, meca->actflg);
    TEST_ASSERT_EQ_INT(ctx, 4, meca->sprpri);
    TEST_ASSERT_EQ_INT(ctx, 32, meca->sprhsize);
    TEST_ASSERT_EQ_INT(ctx, 28, meca->sprvsize);
    TEST_ASSERT_EQ_INT(ctx, 798, meca->sproffset);
    TEST_ASSERT_TRUE(ctx, meca->patbase == egg4meca_pat);
    TEST_ASSERT_EQ_INT(ctx, 1000, meca->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1200, meca->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
}

static void test_egg4awa_init_deru_tuku_and_hiro(test_context *ctx) {
    sprite_status *awa = &actwk[8];
    sprite_status *boss = &actwk[20];

    reset_boss42_state();
    random_values[0] = 0;
    random_values[1] = 5;
    awa->xposi.w.h = 1000;
    awa->yposi.w.h = 1400;
    egg4_get_work(awa)->parent_index = 20;
    boss->xposi.w.h = 990;
    boss->yposi.w.h = 1400;

    egg4awa_ini(awa);

    TEST_ASSERT_EQ_INT(ctx, 4, awa->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 4, awa->actflg);
    TEST_ASSERT_EQ_INT(ctx, 3, awa->sprpri);
    TEST_ASSERT_EQ_INT(ctx, 16, awa->sprhsize);
    TEST_ASSERT_EQ_INT(ctx, 16, awa->sprvsize);
    TEST_ASSERT_EQ_INT(ctx, 798, awa->sproffset);
    TEST_ASSERT_TRUE(ctx, awa->patbase == egg4awa_pat);
    TEST_ASSERT_EQ_INT(ctx, 128, egg4_get_work(awa)->angular_speed);
    TEST_ASSERT_EQ_INT(ctx, 10, egg4_get_work(awa)->radius);
    TEST_ASSERT_EQ_INT(ctx, 32768, (Uint16)egg4_get_work(awa)->angle);
    TEST_ASSERT_EQ_INT(ctx, 2, awa->mstno.b.h);

    reset_boss42_state();
    awa = &actwk[8];
    random_values[0] = -1;
    random_values[1] = 0;
    awa->r_no1 = 1;
    awa->yposi.w.h = 1300;
    egg4awa_ini(awa);
    TEST_ASSERT_EQ_INT(ctx, -129, egg4_get_work(awa)->angular_speed);

    reset_boss42_state();
    awa = &actwk[8];
    egg4_get_work(awa)->parent_index = 20;
    awa->r_no1 = 1;
    awa->yposi.w.h = 1136;
    TEST_ASSERT_EQ_INT(ctx, 0, egg4awa_deru(awa));
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);

    reset_boss42_state();
    awa = &actwk[8];
    awa->r_no1 = 1;
    awa->yposi.w.h = 1300;
    egg4_get_work(awa)->angular_speed = 8;
    egg4_get_work(awa)->x_offset = 2;
    egg4_get_work(awa)->radius = 16;
    awa->patcnt = 4;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4awa_deru(awa));
    TEST_ASSERT_EQ_INT(ctx, 1, awa->mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 0, awa->patcnt);
    TEST_ASSERT_EQ_INT(ctx, 1, patchg_count);

    reset_boss42_state();
    awa = &actwk[8];
    boss = &actwk[20];
    egg4_get_work(awa)->parent_index = 20;
    awa->xposi.w.h = 90;
    awa->yposi.w.h = 100;
    boss->xposi.w.h = 100;
    boss->yposi.w.h = 100;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4awa_deru(awa));
    TEST_ASSERT_EQ_INT(ctx, 0, egg4_get_work(awa)->angle);

    reset_boss42_state();
    awa = &actwk[8];
    boss = &actwk[20];
    egg4_get_work(awa)->parent_index = 20;
    awa->r_no0 = 4;
    awa->xposi.w.h = 100;
    awa->yposi.w.h = 100;
    egg4_get_work(awa)->angular_speed = 5;
    boss->xposi.w.h = 100;
    boss->yposi.w.h = 100;
    boss->r_no0 = 10;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4awa_tuku(awa));
    TEST_ASSERT_EQ_INT(ctx, 6, awa->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 16, egg4_get_work(awa)->radius);
    TEST_ASSERT_EQ_INT(ctx, 16, awa->yspeed.w);

    reset_boss42_state();
    awa = &actwk[8];
    boss = &actwk[20];
    egg4_get_work(awa)->parent_index = 20;
    awa->r_no0 = 6;
    awa->yspeed.w = 48;
    egg4_get_work(awa)->radius = 48;
    egg4_get_work(boss)->flags = 8;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4awa_hiro(awa));
    TEST_ASSERT_EQ_INT(ctx, 8, awa->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 255, awa->colino);
    TEST_ASSERT_EQ_INT(ctx, 2, awa->colicnt);
    TEST_ASSERT_TRUE(ctx, (egg4_get_work(awa)->flags & 2) != 0);

    reset_boss42_state();
    awa = &actwk[8];
    boss = &actwk[20];
    egg4_get_work(awa)->parent_index = 20;
    egg4_get_work(awa)->flags = 2;
    awa->colino = 1;
    awa->patcnt = 2;
    awa->yspeed.w = 16;
    egg4_get_work(awa)->radius = 16;
    egg4_get_work(awa)->angle = 128;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4awa_hiro(awa));
    TEST_ASSERT_EQ_INT(ctx, 3, awa->mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 0, awa->patcnt);
}

static void test_egg4awa_roll_ychg_del_and_chi(test_context *ctx) {
    sprite_status *awa = &actwk[9];
    sprite_status *boss = &actwk[20];

    reset_boss42_state();
    egg4_get_work(awa)->parent_index = 20;
    egg4_get_work(boss)->flags = 16;
    awa->colino = 1;
    egg4_get_work(awa)->radial_speed = 65536;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4awa_roll(awa));
    TEST_ASSERT_EQ_INT(ctx, 10, awa->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, awa->r_no1);

    reset_boss42_state();
    awa = &actwk[9];
    boss = &actwk[20];
    egg4_get_work(awa)->parent_index = 20;
    awa->colino = 1;
    egg4_get_work(boss)->flags = 8;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4awa_roll(awa));
    TEST_ASSERT_EQ_INT(ctx, 1, sinset_count);

    reset_boss42_state();
    awa = &actwk[9];
    boss = &actwk[20];
    egg4_get_work(awa)->parent_index = 20;
    awa->colino = 1;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4awa_roll(awa));
    TEST_ASSERT_EQ_INT(ctx, 1, sinset_count);

    reset_boss42_state();
    awa = &actwk[9];
    boss = &actwk[20];
    egg4_get_work(awa)->parent_index = 20;
    awa->r_no0 = 10;
    awa->r_no1 = 1;
    awa->colino = 1;
    egg4_get_work(awa)->radial_speed = -1048576;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4awa_ychg(awa));
    TEST_ASSERT_EQ_INT(ctx, 10, awa->r_no0);

    reset_boss42_state();
    awa = &actwk[9];
    boss = &actwk[20];
    egg4_get_work(awa)->parent_index = 20;
    awa->r_no0 = 10;
    awa->r_no1 = 1;
    awa->colino = 1;
    egg4_get_work(boss)->flags = 8;
    egg4_get_work(awa)->radial_speed = -3145728;
    egg4_get_work(awa)->angle = 20000;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4awa_ychg(awa));
    TEST_ASSERT_EQ_INT(ctx, 8, awa->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, awa->r_no1);
    TEST_ASSERT_EQ_INT(ctx, 3, awa->sprpri);

    reset_boss42_state();
    awa = &actwk[9];
    boss = &actwk[20];
    egg4_get_work(awa)->parent_index = 20;
    awa->r_no0 = 10;
    awa->colino = 1;
    egg4_get_work(awa)->radial_speed = 0;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4awa_ychg(awa));
    TEST_ASSERT_EQ_INT(ctx, 1, awa->r_no1);

    reset_boss42_state();
    awa = &actwk[9];
    egg4_get_work(awa)->angle = 1000;
    ychg_rad_endproc(awa);
    TEST_ASSERT_EQ_INT(ctx, 31768, egg4_get_work(awa)->angle);
    egg4_get_work(awa)->angle = 20000;
    ychg_rad_endproc(awa);
    TEST_ASSERT_EQ_INT(ctx, 12768, egg4_get_work(awa)->angle);

    reset_boss42_state();
    awa = &actwk[9];
    boss = &actwk[20];
    egg4_get_work(awa)->parent_index = 20;
    egg4_get_work(boss)->flags = 32;
    egg4_get_work(awa)->radial_speed = 0x110000;
    sprite_status_set_xspeed_yspeed(awa, 0x110000);
    TEST_ASSERT_EQ_INT(ctx, 1, egg4awa_roll(awa));
    TEST_ASSERT_EQ_INT(ctx, 6, awa->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, egg4_get_work(boss)->spread_count);

    reset_boss42_state();
    awa = &actwk[9];
    boss = &actwk[20];
    egg4_get_work(awa)->parent_index = 20;
    awa->colino = 1;
    egg4_get_work(boss)->flags = 32 | 8;
    egg4_get_work(awa)->radial_speed = 0x110000;
    sprite_status_set_xspeed_yspeed(awa, 0x110000);
    TEST_ASSERT_EQ_INT(ctx, 1, egg4awa_roll(awa));
    TEST_ASSERT_EQ_INT(ctx, 6, awa->r_no0);

    reset_boss42_state();
    awa = &actwk[9];
    boss = &actwk[20];
    egg4_get_work(awa)->parent_index = 20;
    egg4_get_work(boss)->bubble_slot = 1;
    awa->colino = 0;
    awa_hitchk(awa);
    TEST_ASSERT_EQ_INT(ctx, 12, awa->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, egg4_get_work(boss)->bubble_slot);
    TEST_ASSERT_EQ_INT(ctx, 1, patchg_count);

    reset_boss42_state();
    awa = &actwk[9];
    TEST_ASSERT_EQ_INT(ctx, 1, egg4awa_del(awa));
    TEST_ASSERT_EQ_INT(ctx, 1, patchg_count);
    TEST_ASSERT_EQ_INT(ctx, 0, egg4awa_out(awa));
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);

    reset_boss42_state();
    awa = &actwk[9];
    awa->r_no0 = 12;
    egg4awa(awa);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);

    reset_boss42_state();
    awa = &actwk[9];
    awa->r_no0 = 14;
    egg4awa(awa);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_EQ_INT(ctx, 0, actionsub_count);
}

static void test_egg4tama_states_and_frameout_edges(test_context *ctx) {
    sprite_status *tama = &actwk[10];
    sprite_status *boss = &actwk[20];

    reset_boss42_state();
    egg4_get_work(tama)->parent_index = 20;
    egg4_get_work(tama)->x_offset = 7;
    egg4_get_work(tama)->y_offset = 9;
    boss->xposi.w.h = 1000;
    boss->yposi.w.h = 1100;

    egg4tama(tama);

    TEST_ASSERT_EQ_INT(ctx, 2, tama->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 4, tama->actflg);
    TEST_ASSERT_EQ_INT(ctx, 6, tama->sprpri);
    TEST_ASSERT_EQ_INT(ctx, 4, tama->sprhsize);
    TEST_ASSERT_EQ_INT(ctx, 4, tama->sprvsize);
    TEST_ASSERT_TRUE(ctx, tama->patbase == egg4tama_pat);
    TEST_ASSERT_EQ_INT(ctx, 1007, tama->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1109, tama->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, patchg_count);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);

    reset_boss42_state();
    tama = &actwk[10];
    boss = &actwk[20];
    tama->r_no0 = 2;
    egg4_get_work(tama)->timer = 44;
    egg4_get_work(tama)->parent_index = 20;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4tama_01(tama));
    TEST_ASSERT_EQ_INT(ctx, 254, tama->colino);
    TEST_ASSERT_EQ_INT(ctx, 2, tama->colicnt);
    TEST_ASSERT_EQ_INT(ctx, 4, tama->r_no0);

    reset_boss42_state();
    tama = &actwk[10];
    boss = &actwk[20];
    tama->r_no0 = 4;
    egg4_get_work(tama)->parent_index = 20;
    egg4_get_work(tama)->radial_speed = 0;
    sprite_status_set_xspeed_yspeed(tama, 0);
    tama->xposi.w.h = 2816;
    tama->yposi.w.h = 1488;
    TEST_ASSERT_EQ_INT(ctx, 1, egg4tama_02(tama));
    TEST_ASSERT_EQ_INT(ctx, 0, frameout_count);
    tama->xposi.w.h = 2817;
    TEST_ASSERT_EQ_INT(ctx, 0, egg4tama_02(tama));
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);

    reset_boss42_state();
    tama = &actwk[10];
    boss = &actwk[20];
    egg4_get_work(boss)->flags = 128;
    egg4_get_work(tama)->parent_index = 20;
    TEST_ASSERT_EQ_INT(ctx, 0, egg4tama_01(tama));
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);

    reset_boss42_state();
    tama = &actwk[10];
    boss = &actwk[20];
    tama->r_no0 = 4;
    egg4_get_work(boss)->flags = 128;
    egg4_get_work(tama)->parent_index = 20;
    TEST_ASSERT_EQ_INT(ctx, 0, egg4tama_02(tama));
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
}

TEST_MAIN_BEGIN;
test_tables_capture_literal_data(&ctx);
test_egg4_initializes_links_and_laughs(&ctx);
test_egg4_screen_and_demo_states(&ctx);
test_egg4_move_wait_die_and_escape(&ctx);
test_egg4_movec_edges(&ctx);
test_egg4meca_follows_parent(&ctx);
test_egg4awa_init_deru_tuku_and_hiro(&ctx);
test_egg4awa_roll_ychg_del_and_chi(&ctx);
test_egg4tama_states_and_frameout_edges(&ctx);
TEST_MAIN_END
