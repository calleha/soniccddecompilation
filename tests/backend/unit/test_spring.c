#include <string.h>

#include "support/test_runner.h"
#include "types.h"
#include "spring.h"

sprite_status actwk[128];
int_union scra_h_posit;
Sint16 time_stop;

static int actionsub_count;
static sprite_status *actionsub_actor;
static int frameout_count;
static sprite_status *frameout_actor;
static int frameout_s_count;
static sprite_status *frameout_s_actor;
static int patchg_count;
static sprite_status *patchg_actor;
static Uint8 **patchg_table;
static int actwkchk_count;
static sprite_status *actwkchk_result;
static int actwkchk_fail;
static int emycol_d_count;
static Sint16 emycol_d_result;
static int speedset2_count;
static sprite_status *speedset2_actor;
static int ride_on_chk_count;
static sprite_status *ride_on_chk_platform;
static sprite_status *ride_on_chk_player;
static Sint16 ride_on_chk_result;
static int hitchk_u_count;
static sprite_status *hitchk_u_platform;
static sprite_status *hitchk_u_player;
static Sint16 hitchk_u_result;
static int soundset_count;
static Sint16 soundset_requests[8];
static int sinset_count;
static Uint8 sinset_angle;
static Sint16 sinset_sin;
static Sint16 sinset_cos;

void actionsub(sprite_status *pActwk);
void frameout(sprite_status *pActwk);
Sint32 frameout_s(sprite_status *pActwk);
void patchg(sprite_status *patchgwk, Uint8 **pat_dat);
Sint32 actwkchk(sprite_status **ppActwk);
Sint16 emycol_d(sprite_status *pActwk);
void speedset2(sprite_status *pActwk);
Sint16 ride_on_chk(sprite_status *pActwk, sprite_status *pPlayerwk);
Sint16 hitchk_u(sprite_status *pActwk, sprite_status *pPlayerwk);
void soundset(Sint16 ReqNo);
void sinset(Uint8 kakudo, Sint16 *sin, Sint16 *cos);

#define R11A
#include "src/spring.c"

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

void patchg(sprite_status *patchgwk, Uint8 **pat_dat) {
    ++patchg_count;
    patchg_actor = patchgwk;
    patchg_table = pat_dat;
}

Sint32 actwkchk(sprite_status **ppActwk) {
    ++actwkchk_count;
    if (actwkchk_fail) {
        *ppActwk = 0;
        return -1;
    }
    *ppActwk = actwkchk_result;
    return 0;
}

Sint16 emycol_d(sprite_status *pActwk) {
    (void)pActwk;
    ++emycol_d_count;
    return emycol_d_result;
}

void speedset2(sprite_status *pActwk) {
    ++speedset2_count;
    speedset2_actor = pActwk;
}

Sint16 ride_on_chk(sprite_status *pActwk, sprite_status *pPlayerwk) {
    ++ride_on_chk_count;
    ride_on_chk_platform = pActwk;
    ride_on_chk_player = pPlayerwk;
    return ride_on_chk_result;
}

Sint16 hitchk_u(sprite_status *pActwk, sprite_status *pPlayerwk) {
    ++hitchk_u_count;
    hitchk_u_platform = pActwk;
    hitchk_u_player = pPlayerwk;
    return hitchk_u_result;
}

void soundset(Sint16 ReqNo) {
    if (soundset_count < 8) {
        soundset_requests[soundset_count] = ReqNo;
    }
    ++soundset_count;
}

void sinset(Uint8 kakudo, Sint16 *sin, Sint16 *cos) {
    ++sinset_count;
    sinset_angle = kakudo;
    *sin = sinset_sin;
    *cos = sinset_cos;
}

static void reset_spring_state(void) {
    memset(actwk, 0, sizeof(actwk));
    memset(&scra_h_posit, 0, sizeof(scra_h_posit));
    time_stop = 0;
    actionsub_count = 0;
    actionsub_actor = 0;
    frameout_count = 0;
    frameout_actor = 0;
    frameout_s_count = 0;
    frameout_s_actor = 0;
    patchg_count = 0;
    patchg_actor = 0;
    patchg_table = 0;
    actwkchk_count = 0;
    actwkchk_result = &actwk[40];
    actwkchk_fail = 0;
    emycol_d_count = 0;
    emycol_d_result = 0;
    speedset2_count = 0;
    speedset2_actor = 0;
    ride_on_chk_count = 0;
    ride_on_chk_platform = 0;
    ride_on_chk_player = 0;
    ride_on_chk_result = 0;
    hitchk_u_count = 0;
    hitchk_u_platform = 0;
    hitchk_u_player = 0;
    hitchk_u_result = 0;
    soundset_count = 0;
    memset(soundset_requests, 0, sizeof(soundset_requests));
    sinset_count = 0;
    sinset_angle = 0;
    sinset_sin = 64;
    sinset_cos = 128;
}

static void reset_logs(void) {
    actionsub_count = 0;
    actionsub_actor = 0;
    frameout_count = 0;
    frameout_actor = 0;
    frameout_s_count = 0;
    frameout_s_actor = 0;
    patchg_count = 0;
    patchg_actor = 0;
    patchg_table = 0;
    speedset2_count = 0;
    speedset2_actor = 0;
    ride_on_chk_count = 0;
    ride_on_chk_platform = 0;
    ride_on_chk_player = 0;
    hitchk_u_count = 0;
    hitchk_u_platform = 0;
    hitchk_u_player = 0;
    soundset_count = 0;
    memset(soundset_requests, 0, sizeof(soundset_requests));
    sinset_count = 0;
}

static void test_spring_tables_capture_literal_data(test_context *ctx) {
    TEST_ASSERT_TRUE(ctx, springpat[0] == &spat00);
    TEST_ASSERT_TRUE(ctx, spring90pat[0] == &spat03);
    TEST_ASSERT_TRUE(ctx, spring45pat[0] == &spat06);
    TEST_ASSERT_TRUE(ctx, springpat2[0] == &spat10);
    TEST_ASSERT_TRUE(ctx, spring90pat2[0] == &spat13);
    TEST_ASSERT_TRUE(ctx, spring45pat2[0] == &spat16);
    TEST_ASSERT_TRUE(ctx, komapat[0] == &kpat00);
    TEST_ASSERT_TRUE(ctx, iwapat[0] == &ipat00);
    TEST_ASSERT_EQ_INT(ctx, 467, spat00.spra[0].index);
    TEST_ASSERT_EQ_INT(ctx, 487, spat18.spra[0].index);
    TEST_ASSERT_EQ_INT(ctx, 478, ipat00.spra[0].index);
    TEST_ASSERT_EQ_INT(ctx, 0, springchg0[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, springchg0[2]);
    TEST_ASSERT_EQ_INT(ctx, -4, springchg0[11]);
    TEST_ASSERT_TRUE(ctx, springchg[0] == springchg0);
    TEST_ASSERT_EQ_INT(ctx, 8, komachg0[0]);
    TEST_ASSERT_EQ_INT(ctx, -1, komachg0[3]);
    TEST_ASSERT_TRUE(ctx, komachg[0] == komachg0);
    TEST_ASSERT_TRUE(ctx, sjump_move_tbl[0] == sjumpinit);
    TEST_ASSERT_TRUE(ctx, sjump_move_tbl[12] == sjump3move3);
}

static void test_iwa_init_move_and_wrapper(test_context *ctx) {
    reset_spring_state();
    iwa(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[1].r_no0);
    TEST_ASSERT_EQ_INT(ctx, 4, actwk[1].actflg);
    TEST_ASSERT_EQ_INT(ctx, 4, actwk[1].sprpri);
    TEST_ASSERT_TRUE(ctx, actwk[1].patbase == iwapat);
    TEST_ASSERT_EQ_INT(ctx, 16, actwk[1].sprhsize);
    TEST_ASSERT_EQ_INT(ctx, 16, actwk[1].sprvsize);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_s_count);
    TEST_ASSERT_EQ_INT(ctx, 0, ride_on_chk_count);

    reset_spring_state();
    actwk[1].r_no0 = 2;
    actwk[1].actflg = 128;
    iwa(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, ride_on_chk_count);
    TEST_ASSERT_TRUE(ctx, ride_on_chk_platform == &actwk[1]);
    TEST_ASSERT_TRUE(ctx, ride_on_chk_player == &actwk[0]);
}

static void test_koma_init_success_failure_and_movement(test_context *ctx) {
    reset_spring_state();
    actwk[1].xposi.w.h = 100;
    actwk[1].yposi.w.h = 200;
    actwk[1].userflag.w = 0x1234;
    koma_init(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[1].r_no0);
    TEST_ASSERT_TRUE(ctx, actwk[1].patbase == komapat);
    TEST_ASSERT_EQ_INT(ctx, 384, actwk[1].xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 1, patchg_count);
    TEST_ASSERT_TRUE(ctx, patchg_table == (Uint8 **)&komachg);
    TEST_ASSERT_EQ_INT(ctx, 10, actwk[40].actno);
    TEST_ASSERT_EQ_INT(ctx, 100, actwk[40].xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 184, actwk[40].yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0x1234, actwk[40].userflag.w);

    reset_spring_state();
    actwkchk_fail = 1;
    koma_init(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actor == &actwk[1]);

    reset_spring_state();
    emycol_d_result = 3;
    actwk[1].yposi.w.h = 20;
    koma_move(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 21, actwk[1].yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[1].r_no0);

    reset_spring_state();
    emycol_d_result = -5;
    actwk[1].yposi.w.h = 20;
    koma_move(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 15, actwk[1].yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[1].r_no0);
}

static void test_koma_move2_time_and_frameout_paths(test_context *ctx) {
    reset_spring_state();
    emycol_d_result = -1;
    actwk[1].xposi.w.h = 100;
    actwk[1].yposi.w.h = 100;
    koma_move(&actwk[1]);
    reset_logs();
    emycol_d_result = -12;
    actwk[1].xspeed.w = 384;
    koma_move2(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 87, actwk[1].yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, -384, actwk[1].xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 1, speedset2_count);
    TEST_ASSERT_TRUE(ctx, speedset2_actor == &actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, patchg_count);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);

    reset_spring_state();
    time_stop = 1;
    koma_move2(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 0, emycol_d_count);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);

    reset_spring_state();
    actwk[1].xposi.w.h = 1024;
    scra_h_posit.w.h = 0;
    koma(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);

    reset_spring_state();
    actwk[1].r_no0 = 4;
    time_stop = 1;
    koma(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
}

static void test_spring_d_follows_player_and_calls_actionsub(test_context *ctx) {
    reset_spring_state();
    actwk[0].xposi.l = 0x00100020;
    actwk[0].yposi.l = 0x00300040;
    actwk[1].userflag.b.h = 2;
    spring_d(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[1].r_no0);
    TEST_ASSERT_TRUE(ctx, actwk[1].patbase == springpat);
    TEST_ASSERT_EQ_INT(ctx, 34080, actwk[1].sproffset);
    TEST_ASSERT_EQ_INT(ctx, 0x00100020, actwk[1].xposi.l);
    TEST_ASSERT_EQ_INT(ctx, 0x00300040, actwk[1].yposi.l);
    TEST_ASSERT_EQ_INT(ctx, 2, actionsub_count);

    reset_spring_state();
    spring_d(&actwk[1]);
    TEST_ASSERT_TRUE(ctx, actwk[1].patbase == springpat2);
}

static void test_sjumpinit_configures_all_spring_orientations(test_context *ctx) {
    reset_spring_state();
    actwk[1].xposi.w.h = 100;
    sjumpinit(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[1].r_no0);
    TEST_ASSERT_TRUE(ctx, actwk[1].patbase == springpat2);
    TEST_ASSERT_EQ_INT(ctx, 1312, actwk[1].sproffset);
    TEST_ASSERT_EQ_INT(ctx, 16, actwk[1].sprhsize);
    TEST_ASSERT_EQ_INT(ctx, 8, actwk[1].sprvsize);

    reset_spring_state();
    actwk[1].userflag.b.h = 2;
    sjumpinit(&actwk[1]);
    TEST_ASSERT_TRUE(ctx, actwk[1].patbase == springpat);
    TEST_ASSERT_EQ_INT(ctx, 9504, actwk[1].sproffset);

    reset_spring_state();
    actwk[1].userflag.b.h = 4;
    sjumpinit(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 8, actwk[1].r_no0);
    TEST_ASSERT_TRUE(ctx, actwk[1].patbase == spring90pat2);
    TEST_ASSERT_EQ_INT(ctx, 8, actwk[1].sprhsize);
    TEST_ASSERT_EQ_INT(ctx, 16, actwk[1].sprvsize);

    reset_spring_state();
    actwk[1].userflag.b.h = 6;
    sjumpinit(&actwk[1]);
    TEST_ASSERT_TRUE(ctx, actwk[1].patbase == spring90pat);

    reset_spring_state();
    actwk[1].userflag.b.h = 8;
    sjumpinit(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 20, actwk[1].r_no0);
    TEST_ASSERT_TRUE(ctx, actwk[1].patbase == spring45pat2);
    TEST_ASSERT_EQ_INT(ctx, 24, actwk[1].sprhsize);
    TEST_ASSERT_EQ_INT(ctx, 12, actwk[1].sprvsize);

    reset_spring_state();
    actwk[1].userflag.b.h = 10;
    sjumpinit(&actwk[1]);
    TEST_ASSERT_TRUE(ctx, actwk[1].patbase == spring45pat);

    reset_spring_state();
    actwk[1].actflg = 2;
    sjumpinit(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 14, actwk[1].r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[1].cddat);
}

static void test_sjump_vertical_activation_and_reset(test_context *ctx) {
    reset_spring_state();
    sjumpinit(&actwk[1]);
    sjumpmove(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 0, ride_on_chk_count);

    actwk[1].actflg |= 128;
    ride_on_chk_result = 0;
    sjumpmove(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, ride_on_chk_count);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[1].r_no0);

    ride_on_chk_result = 1;
    actwk[0].yposi.w.h = 50;
    actwk[0].cddat = 8;
    actwk[1].cddat = 8;
    sjumpmove(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 4, actwk[1].r_no0);
    TEST_ASSERT_EQ_INT(ctx, 58, actwk[0].yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, -4096, actwk[0].yspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[0].cddat);
    TEST_ASSERT_EQ_INT(ctx, 16, actwk[0].mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[1].cddat);
    TEST_ASSERT_EQ_INT(ctx, 1, soundset_count);
    TEST_ASSERT_EQ_INT(ctx, 152, soundset_requests[0]);

    sjumpmove2(&actwk[1]);
    TEST_ASSERT_TRUE(ctx, patchg_table == (Uint8 **)&springchg);
    actwk[1].cddat = 8;
    actwk[1].patno = 2;
    sjumpmove3(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[1].cddat);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[1].mstno.b.l);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[1].r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[1].patno);
}

static void test_sdush_horizontal_activation_branches(test_context *ctx) {
    reset_spring_state();
    sjumpinit(&actwk[1]);
    actwk[1].r_no0 = 8;
    sdushmove(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 0, ride_on_chk_count);

    actwk[1].actflg |= 128;
    sdushmove(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, ride_on_chk_count);
    TEST_ASSERT_EQ_INT(ctx, 8, actwk[1].r_no0);

    actwk[1].cddat = 32;
    actwk[0].xposi.w.h = 100;
    sdushmove(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 10, actwk[1].r_no0);
    TEST_ASSERT_EQ_INT(ctx, 92, actwk[0].xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 4096, actwk[0].xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 4096, actwk[0].mspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].direc.b.h);
    TEST_ASSERT_EQ_INT(ctx, 1, soundset_count);

    reset_spring_state();
    actwk[1].actflg = 128;
    actwk[1].cddat = 33;
    actwk[0].cddat = 4;
    actwk[0].xposi.w.h = 100;
    sjumpinit(&actwk[1]);
    actwk[1].r_no0 = 8;
    actwk[1].cddat = 33;
    sdushmove(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 108, actwk[0].xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, -4096, actwk[0].xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 5, actwk[0].cddat);

    sdushmove2(&actwk[1]);
    TEST_ASSERT_TRUE(ctx, patchg_table == (Uint8 **)&springchg);
    sdushmove3(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 6, actwk[1].r_no0);
}

static void test_sjump2_upward_activation_and_reset(test_context *ctx) {
    reset_spring_state();
    sjumpinit(&actwk[1]);
    actwk[1].r_no0 = 14;
    sjump2move(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 0, hitchk_u_count);

    actwk[1].actflg |= 128;
    hitchk_u_result = 0;
    sjump2move(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, hitchk_u_count);
    TEST_ASSERT_EQ_INT(ctx, 14, actwk[1].r_no0);

    hitchk_u_result = 1;
    actwk[0].yposi.w.h = 50;
    actwk[0].cddat = 8;
    actwk[1].cddat = 8;
    sjump2move(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 16, actwk[1].r_no0);
    TEST_ASSERT_EQ_INT(ctx, 42, actwk[0].yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 4096, actwk[0].yspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[0].cddat);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[1].cddat);
    TEST_ASSERT_EQ_INT(ctx, 1, soundset_count);

    sjump2move2(&actwk[1]);
    TEST_ASSERT_TRUE(ctx, patchg_table == (Uint8 **)&springchg);
    sjump2move3(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 12, actwk[1].r_no0);
}

static void test_sjump3_diagonal_activation_branches(test_context *ctx) {
    reset_spring_state();
    sjumpinit(&actwk[1]);
    actwk[1].r_no0 = 20;
    sjump3move(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 0, ride_on_chk_count);

    actwk[1].actflg |= 128;
    ride_on_chk_result = 0;
    sjump3move(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, ride_on_chk_count);
    TEST_ASSERT_EQ_INT(ctx, 20, actwk[1].r_no0);

    actwk[1].cddat = 32;
    actwk[0].xposi.w.h = 100;
    actwk[0].yposi.w.h = 50;
    actwk[0].cddat = 0xff;
    sinset_sin = 64;
    sinset_cos = 128;
    sjump3move(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 22, actwk[1].r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, sinset_count);
    TEST_ASSERT_EQ_INT(ctx, 224, sinset_angle);
    TEST_ASSERT_EQ_INT(ctx, 92, actwk[0].xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 58, actwk[0].yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1024, actwk[0].yspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 2048, actwk[0].xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 214, actwk[0].cddat);
    TEST_ASSERT_EQ_INT(ctx, 1, patchg_count);
    TEST_ASSERT_EQ_INT(ctx, 1, soundset_count);

    reset_spring_state();
    sjumpinit(&actwk[1]);
    actwk[1].r_no0 = 20;
    actwk[1].actflg |= 130;
    actwk[1].cddat = 33;
    actwk[0].xposi.w.h = 100;
    actwk[0].yposi.w.h = 50;
    sinset_sin = 64;
    sinset_cos = 128;
    sjump3move(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 108, actwk[0].xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 42, actwk[0].yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, -1024, actwk[0].yspeed.w);
    TEST_ASSERT_EQ_INT(ctx, -2048, actwk[0].xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 3, actwk[0].cddat);

    sjump3move3(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 18, actwk[1].r_no0);
}

static void test_spring_wrapper_attached_and_frameout_paths(test_context *ctx) {
    reset_spring_state();
    actwk[1].xposi.w.h = 100;
    actwk[1].yposi.w.h = 200;
    actwkchk_result = &actwk[40];
    koma_init(&actwk[1]);
    reset_logs();
    actwk[1].xposi.w.h = 333;
    actwk[1].yposi.w.h = 444;
    spring(&actwk[40]);
    TEST_ASSERT_EQ_INT(ctx, 333, actwk[40].xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 428, actwk[40].yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);

    reset_spring_state();
    actwk[1].xposi.w.h = 1024;
    spring(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);

    reset_spring_state();
    actwk[1].r_no1 = 5;
    actwk[0].xposi.w.h = 10;
    actwk[0].yposi.w.h = 20;
    spring(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 10, actwk[1].xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 20, actwk[1].yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 3, actionsub_count);
}

static void test_spring_attached_offsets_position_child(test_context *ctx) {
    spring_work *work;

    /* Mirrors what trapdr3_init/frdr3_init set up for their spring child:
       parent_index plus follow offsets, which spring() applies to the parent. */
    reset_spring_state();
    actwk[5].xposi.w.h = 400;
    actwk[5].yposi.w.h = 100;
    work = spring_work_get(&actwk[6]);
    work->parent_index = 5;
    work->follow_x_offset = 24;
    work->follow_y_offset = -22;
    spring(&actwk[6]);
    TEST_ASSERT_EQ_INT(ctx, 424, actwk[6].xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 78, actwk[6].yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, frameout_count);
}

TEST_MAIN_BEGIN;
    test_spring_tables_capture_literal_data(&ctx);
    test_iwa_init_move_and_wrapper(&ctx);
    test_koma_init_success_failure_and_movement(&ctx);
    test_koma_move2_time_and_frameout_paths(&ctx);
    test_spring_d_follows_player_and_calls_actionsub(&ctx);
    test_sjumpinit_configures_all_spring_orientations(&ctx);
    test_sjump_vertical_activation_and_reset(&ctx);
    test_sdush_horizontal_activation_branches(&ctx);
    test_sjump2_upward_activation_and_reset(&ctx);
    test_sjump3_diagonal_activation_branches(&ctx);
    test_spring_wrapper_attached_and_frameout_paths(&ctx);
    test_spring_attached_offsets_position_child(&ctx);
TEST_MAIN_END
