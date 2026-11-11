/* <stdlib.h> declares POSIX random(); the decompiled src/io.h declares the
   game's own Sint32 random(void). Parse stdlib first, then rename the game
   symbol inside this harness so the two can coexist. */
#include <stdlib.h>

#define random game_random

#include <string.h>

#include "support/test_runner.h"
#include "src/types.h"

sprite_status actwk[128];
Uint8 flagworkcnt;
Uint8 flagwork[766];
Uint8 flowercnt[3];
int_union flowerposi[192];
Uint8 plpower_b;
Uint8 plpower_m;
Uint8 plpower_a;
Uint8 plflag;
Uint8 plflag_s;
Sint16 plxposi_s;
Sint16 plyposi_s;
Sint32 pltime_s;
Uint8 time_flag;
Sint16 scralim_down_s;
Sint16 scra_h_posit_s;
Sint16 scra_v_posit_s;
Sint16 scrb_h_posit_s;
Sint16 scrb_v_posit_s;
Sint16 scrc_h_posit_s;
Sint16 scrc_v_posit_s;
Sint16 scrz_h_posit_s;
Sint16 scrz_v_posit_s;
Sint16 waterposi_m_s;
Uint8 water_flag_s;
Uint8 waterflag_s;
Uint8 plsubchg_flag;
Uint8 markerno;
Sint16 pl_air;
Uint8 bossstart;
Uint8 plpower_s;
Uint8 watercoliflag;
Sint16 waterposi;
short_union scroll_start;
Sint16 waterposi_m;
Uint8 water_flag;
Uint8 waterflag;
Sint16 scralim_down;
ushort_union plposiwkadr;
int_union scrz_v_posit;
int_union scrz_h_posit;
int_union scrc_v_posit;
int_union scrc_h_posit;
int_union scrb_v_posit;
int_union scrb_h_posit;
int_union scra_v_posit;
int_union scra_h_posit;
ushort_union swdata1;
Sint16 playposiwk[128];
short_union editmode;
int_union pltime;
short_union stageno;
short_union gametimer;

static sprite_pattern dummy_pattern = {1, {{0, 0, 0, 1}}};
sprite_pattern *markerpat[] = {&dummy_pattern};
sprite_pattern *bariapat[] = {&dummy_pattern};
sprite_pattern *bakupat[] = {&dummy_pattern};
sprite_pattern *exit2pat[] = {&dummy_pattern};

Uint8 markerchg0[] = {1, 0, 255};
Uint8 bariachg0[] = {1, 1, 255};
Uint8 bakuchg0[] = {1, 2, 255};
Uint8 exit2chg0[] = {1, 3, 255};
Uint8 *markerchg[] = {markerchg0, markerchg0, markerchg0, markerchg0};
Uint8 *bariachg[] = {bariachg0, bariachg0, bariachg0, bariachg0, bariachg0,
                      bariachg0};
Uint8 *bakuchg[] = {bakuchg0, bakuchg0};
Uint8 *exit2chg[] = {exit2chg0, exit2chg0, exit2chg0};
Uint8 awasintbl[256];

static int actionsub_count;
static sprite_status *actionsub_actor;
static int frameout_count;
static sprite_status *frameout_actor;
static int frameout_s_count;
static sprite_status *frameout_s_actor;
static int actwkchk_count;
static sprite_status *actwkchk_queue[16];
static int actwkchk_queue_count;
static int actwkchk_queue_index;
static int soundset_count;
static Sint16 soundset_requests[16];
static int sinset_count;
static Uint8 sinset_angle;
static Sint16 sinset_sin;
static Sint16 sinset_cos;
static int emycol_d_count;
static Sint16 emycol_d_result;
static int bgmset_count;
static Sint16 bgmset_requests[8];
static int speedset2_count;
static int jumpcolsub_count;
static Sint32 random_values[16];
static int random_index;

void actionsub(sprite_status *pActwk);
void frameout(sprite_status *pActwk);
Sint32 frameout_s(sprite_status *pActwk);
Sint32 actwkchk(sprite_status **ppActwk);
void soundset(Sint16 ReqNo);
void sinset(Uint8 kakudo, Sint16 *sin, Sint16 *cos);
Sint16 emycol_d(sprite_status *pActwk);
void bgmset(Sint16 ReqNo);
void speedset2(sprite_status *pActwk);
void jumpcolsub(void);
Sint32 random(void);

#include "src/r4/playsub4.c"

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
    if (actwkchk_queue_index >= actwkchk_queue_count) {
        *ppActwk = 0;
        return -1;
    }
    *ppActwk = actwkchk_queue[actwkchk_queue_index++];
    return 0;
}

void soundset(Sint16 ReqNo) {
    if (soundset_count < 16) {
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

Sint16 emycol_d(sprite_status *pActwk) {
    (void)pActwk;
    ++emycol_d_count;
    return emycol_d_result;
}

void bgmset(Sint16 ReqNo) {
    if (bgmset_count < 8) {
        bgmset_requests[bgmset_count] = ReqNo;
    }
    ++bgmset_count;
}

void speedset2(sprite_status *pActwk) {
    ++speedset2_count;
    pActwk->xposi.l += pActwk->xspeed.w;
    pActwk->yposi.l += pActwk->yspeed.w;
}

void jumpcolsub(void) { ++jumpcolsub_count; }

Sint32 random(void) {
    Sint32 value = random_values[random_index];
    if (random_index < 15) {
        ++random_index;
    }
    return value;
}

static void queue_actor(sprite_status *actor) {
    actwkchk_queue[actwkchk_queue_count++] = actor;
}

static void reset_playsub4_state(void) {
    memset(actwk, 0, sizeof(actwk));
    memset(flagwork, 0, sizeof(flagwork));
    memset(flowercnt, 0, sizeof(flowercnt));
    memset(flowerposi, 0, sizeof(flowerposi));
    memset(awasintbl, 0, sizeof(awasintbl));
    flagworkcnt = 0;
    plpower_b = 0;
    plpower_m = 0;
    plpower_a = 0;
    plflag = 0;
    plflag_s = 0;
    plxposi_s = 0;
    plyposi_s = 0;
    pltime_s = 0;
    time_flag = 0;
    scralim_down_s = 0;
    scra_h_posit_s = 0;
    scra_v_posit_s = 0;
    scrb_h_posit_s = 0;
    scrb_v_posit_s = 0;
    scrc_h_posit_s = 0;
    scrc_v_posit_s = 0;
    scrz_h_posit_s = 0;
    scrz_v_posit_s = 0;
    waterposi_m_s = 0;
    water_flag_s = 0;
    waterflag_s = 0;
    plsubchg_flag = 0;
    markerno = 0;
    pl_air = 0;
    bossstart = 0;
    plpower_s = 0;
    watercoliflag = 0;
    waterposi = 0;
    memset(&scroll_start, 0, sizeof(scroll_start));
    waterposi_m = 0;
    water_flag = 0;
    waterflag = 0;
    scralim_down = 0;
    memset(&plposiwkadr, 0, sizeof(plposiwkadr));
    memset(&scrz_v_posit, 0, sizeof(scrz_v_posit));
    memset(&scrz_h_posit, 0, sizeof(scrz_h_posit));
    memset(&scrc_v_posit, 0, sizeof(scrc_v_posit));
    memset(&scrc_h_posit, 0, sizeof(scrc_h_posit));
    memset(&scrb_v_posit, 0, sizeof(scrb_v_posit));
    memset(&scrb_h_posit, 0, sizeof(scrb_h_posit));
    memset(&scra_v_posit, 0, sizeof(scra_v_posit));
    memset(&scra_h_posit, 0, sizeof(scra_h_posit));
    memset(&swdata1, 0, sizeof(swdata1));
    memset(playposiwk, 0, sizeof(playposiwk));
    memset(&editmode, 0, sizeof(editmode));
    memset(&pltime, 0, sizeof(pltime));
    memset(&stageno, 0, sizeof(stageno));
    memset(&gametimer, 0, sizeof(gametimer));
    actionsub_count = 0;
    actionsub_actor = 0;
    frameout_count = 0;
    frameout_actor = 0;
    frameout_s_count = 0;
    frameout_s_actor = 0;
    actwkchk_count = 0;
    memset(actwkchk_queue, 0, sizeof(actwkchk_queue));
    actwkchk_queue_count = 0;
    actwkchk_queue_index = 0;
    soundset_count = 0;
    memset(soundset_requests, 0, sizeof(soundset_requests));
    sinset_count = 0;
    sinset_angle = 0;
    sinset_sin = 128;
    sinset_cos = 256;
    emycol_d_count = 0;
    emycol_d_result = 0;
    bgmset_count = 0;
    memset(bgmset_requests, 0, sizeof(bgmset_requests));
    speedset2_count = 0;
    jumpcolsub_count = 0;
    memset(random_values, 0, sizeof(random_values));
    random_index = 0;
}

static void test_tables_and_patchg(test_context *ctx) {
    Uint8 normal[] = {2, 33, 255};
    Uint8 reset[] = {1, 7, 255};
    Uint8 rewind[] = {1, 4, 5, 254, 2, 255};
    Uint8 set_mst[] = {1, 253, 3, 255};
    Uint8 next_rno[] = {1, 252, 255};
    Uint8 clear_rno1[] = {1, 251, 255};
    Uint8 next_rno1[] = {1, 250, 255};
    Uint8 *tables[] = {normal, reset, rewind, set_mst, next_rno, clear_rno1,
                       next_rno1};

    TEST_ASSERT_TRUE(ctx, flowchg[0] == flowchg0);
    TEST_ASSERT_TRUE(ctx, flow_pat[7] == &flo07);
    TEST_ASSERT_EQ_INT(ctx, 271, flo07.spra[0].index);
    TEST_ASSERT_TRUE(ctx, awapat[22] == &awasp22);
    TEST_ASSERT_TRUE(ctx, wavepat[7] == &wavepat3);
    TEST_ASSERT_TRUE(ctx, boupat[1] == &boupat1);

    reset_playsub4_state();
    actwk[1].cddat = 3;
    patchg(&actwk[1], tables);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[1].patno);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[1].actflg);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[1].patcnt);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[1].pattim);

    reset_playsub4_state();
    actwk[1].mstno.b.h = 1;
    actwk[1].mstno.b.l = 1;
    actwk[1].patcnt = 1;
    patchg(&actwk[1], tables);
    TEST_ASSERT_EQ_INT(ctx, 7, actwk[1].patno);

    reset_playsub4_state();
    actwk[1].mstno.b.h = 2;
    actwk[1].mstno.b.l = 2;
    actwk[1].patcnt = 2;
    patchg(&actwk[1], tables);
    TEST_ASSERT_EQ_INT(ctx, 4, actwk[1].patno);

    reset_playsub4_state();
    actwk[1].mstno.b.h = 3;
    actwk[1].mstno.b.l = 3;
    patchg(&actwk[1], tables);
    TEST_ASSERT_EQ_INT(ctx, 3, actwk[1].mstno.b.h);

    reset_playsub4_state();
    actwk[1].mstno.b.h = 4;
    actwk[1].mstno.b.l = 4;
    patchg(&actwk[1], tables);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[1].r_no0);

    reset_playsub4_state();
    actwk[1].mstno.b.h = 5;
    actwk[1].mstno.b.l = 5;
    actwk[1].r_no1 = 7;
    patchg(&actwk[1], tables);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[1].r_no1);

    reset_playsub4_state();
    actwk[1].mstno.b.h = 6;
    actwk[1].mstno.b.l = 6;
    patchg(&actwk[1], tables);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[1].r_no1);
}

static void test_playsave_marker_and_bakuha(test_context *ctx) {
    reset_playsub4_state();
    plflag = 2;
    actwk[0].xposi.w.h = 123;
    actwk[0].yposi.w.h = 456;
    water_flag = 3;
    scralim_down = 111;
    scra_h_posit.w.h = 10;
    scra_v_posit.w.h = 20;
    scrb_h_posit.w.h = 30;
    scrb_v_posit.w.h = 40;
    scrc_h_posit.w.h = 50;
    scrc_v_posit.w.h = 60;
    scrz_h_posit.w.h = 70;
    scrz_v_posit.w.h = 80;
    waterposi_m = 90;
    waterflag = 4;
    pltime.l = 327681;
    playsave0(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 2, plflag_s);
    TEST_ASSERT_EQ_INT(ctx, 123, plxposi_s);
    TEST_ASSERT_EQ_INT(ctx, 456, plyposi_s);
    TEST_ASSERT_EQ_INT(ctx, 327680, pltime_s);
    pltime.l = 1234;
    playsave0(&actwk[0]);
    TEST_ASSERT_EQ_INT(ctx, 1234, pltime_s);

    reset_playsub4_state();
    markerno = 2;
    actwk[1].userflag.b.h = 1;
    actwk[1].xposi.w.h = 100;
    actwk[1].yposi.w.h = 200;
    queue_actor(&actwk[40]);
    marker(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[1].r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, marker_get_work(&actwk[1])->activated);
    TEST_ASSERT_EQ_INT(ctx, 19, actwk[40].actno);
    TEST_ASSERT_EQ_INT(ctx, 6, actwk[40].r_no0);
    TEST_ASSERT_EQ_INT(ctx, 100, actwk[40].xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 168, actwk[40].yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_s_count);

    reset_playsub4_state();
    actwk[1].userflag.b.h = 3;
    marker_init(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);

    reset_playsub4_state();
    actwk[1].userflag.b.h = 4;
    marker_get_work(&actwk[1])->activated = 1;
    marker_move0(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 0, soundset_count);
    marker_get_work(&actwk[1])->activated = 0;
    marker_move0(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 0, soundset_count);
    actwk[1].colicnt = 1;
    marker_move0(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, marker_get_work(&actwk[1])->activated);
    TEST_ASSERT_EQ_INT(ctx, 4, markerno);
    TEST_ASSERT_EQ_INT(ctx, 1, plflag);
    TEST_ASSERT_EQ_INT(ctx, 174, soundset_requests[0]);

    reset_playsub4_state();
    actwk[1].xposi.w.h = 50;
    actwk[1].yposi.w.h = 100;
    queue_actor(&actwk[40]);
    marker_init(&actwk[1]);
    marker_move1(&actwk[40]);
    TEST_ASSERT_EQ_INT(ctx, 1, marker_get_work(&actwk[40])->activated);
    marker_get_work(&actwk[1])->activated = 1;
    sinset_sin = 128;
    sinset_cos = 0;
    marker_move1(&actwk[40]);
    TEST_ASSERT_EQ_INT(ctx, 1, marker_get_work(&actwk[40])->activated);
    TEST_ASSERT_EQ_INT(ctx, 54, actwk[40].xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 76, actwk[40].yposi.w.h);
    marker_get_work(&actwk[40])->angle = 248;
    marker_move1(&actwk[40]);
    TEST_ASSERT_EQ_INT(ctx, 8, actwk[40].r_no0);

    reset_playsub4_state();
    markerno = 0;
    actwk[1].userflag.b.h = 1;
    queue_actor(&actwk[40]);
    marker_init(&actwk[1]);
    marker_move1(&actwk[40]);
    TEST_ASSERT_EQ_INT(ctx, 0, marker_get_work(&actwk[40])->activated);

    reset_playsub4_state();
    actwk[1].r_no0 = 2;
    actwk[1].colicnt = 1;
    marker(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, marker_get_work(&actwk[1])->activated);
    actwk[1].r_no0 = 4;
    marker_get_work(&actwk[1])->activated = 1;
    marker(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 8, marker_get_work(&actwk[1])->angle);
    actwk[1].r_no0 = 6;
    marker(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 3, actionsub_count);

    reset_playsub4_state();
    actwk[1].xposi.w.h = 300;
    actwk[1].yposi.w.h = 400;
    actwk[1].r_no1 = 1;
    tensuu_set(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);
    queue_actor(&actwk[40]);
    tensuu0(&actwk[1], 3);
    TEST_ASSERT_EQ_INT(ctx, 28, actwk[40].actno);
    TEST_ASSERT_EQ_INT(ctx, -125, actwk[40].userflag.b.h);
    reset_playsub4_state();
    queue_actor(&actwk[41]);
    playsub4_score_get_work(&actwk[1])->score_index = 6;
    test_act(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 28, actwk[41].actno);
    TEST_ASSERT_EQ_INT(ctx, -125, actwk[41].userflag.b.h);

    reset_playsub4_state();
    actwk[1].userflag.b.h = 1;
    bakuha(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[1].r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1664, actwk[1].sproffset);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[1].mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    actwk[1].r_no0 = 2;
    bakuha(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 2, actionsub_count);
    actwk[1].r_no0 = 4;
    actwk[1].r_no1 = 0;
    bakuha(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 31, actwk[1].actno);
    actwk[1].actno = 0;
    actwk[1].r_no0 = 4;
    actwk[1].r_no1 = 1;
    baku_die(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
}

static void test_flower_exit_and_barrier(test_context *ctx) {
    reset_playsub4_state();
    time_flag = 2;
    actwk[1].cdsts = 4;
    TEST_ASSERT_TRUE(ctx, flow_sub(&actwk[1]) == &flagwork[14]);
    TEST_ASSERT_EQ_INT(ctx, 129, flow_sub1(65));

    reset_playsub4_state();
    time_flag = 1;
    actwk[1].cdsts = 2;
    flagwork[7] = 64;
    flow_init(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 4, actwk[1].r_no0);
    TEST_ASSERT_EQ_INT(ctx, 3, actwk[1].mstno.b.h);

    reset_playsub4_state();
    actwk[1].userflag.b.h = 1;
    flow_init(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[1].r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1751, actwk[1].sproffset);

    reset_playsub4_state();
    emycol_d_result = -5;
    flow_move0(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 4, actwk[1].r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[1].mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, -5, actwk[1].yposi.w.h);

    reset_playsub4_state();
    flower(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[1].r_no0);
    actwk[1].r_no0 = 2;
    flower(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 2, actionsub_count);
    actwk[1].r_no0 = 6;
    flower(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[1].mstno.b.h);
    actwk[1].r_no0 = 4;
    flower(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 4, actionsub_count);
    actwk[1].r_no0 = 8;
    flower(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 3, actwk[1].mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 5, actionsub_count);

    reset_playsub4_state();
    actwk[1].userflag.b.h = 2;
    exit2_init(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[1].r_no0);
    TEST_ASSERT_EQ_INT(ctx, 161, soundset_requests[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    exit2_erase(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);

    reset_playsub4_state();
    actwk[1].userflag.b.h = 1;
    mizukiri(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 162, soundset_requests[0]);
    actwk[1].r_no0 = 2;
    waterposi = 333;
    exit2(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 337, actwk[1].yposi.w.h);
    actwk[1].r_no0 = 4;
    futa(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    exit2_set(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 2, frameout_count);

    reset_playsub4_state();
    actwk[1].mstno.b.h = 0;
    bariainit(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[1].r_no0);
    TEST_ASSERT_TRUE(ctx, actwk[1].patbase == bariapat);
    actwk[1].r_no0 = 2;
    plpower_b = 0;
    bariamove(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);

    reset_playsub4_state();
    actwk[1].mstno.b.h = 5;
    bariainit(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 6, actwk[1].r_no0);
    reset_playsub4_state();
    actwk[1].mstno.b.h = 1;
    bariainit(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 4, actwk[1].r_no0);

    reset_playsub4_state();
    actwk[0].xposi.w.h = 500;
    actwk[0].yposi.w.h = 600;
    actwk[0].cddat = 3;
    actwk[1].r_no0 = 2;
    plpower_b = 1;
    bariamove(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 500, actwk[1].xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 600, actwk[1].yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 130, plsubchg_flag);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    plpower_a = 1;
    bariamove(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);

    reset_playsub4_state();
    actwk[1].r_no0 = 4;
    plpower_a = 1;
    mutekimove(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 0, frameout_count);
    plpower_a = 0;
    plpower_a = 0;
    plpower_m = 0;
    mutekimove(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    plpower_m = 1;
    mutekimove(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);

    reset_playsub4_state();
    actwk[1].r_no0 = 6;
    plpower_a = 1;
    backtomove(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    plpower_a = 0;
    backtomove(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);

    reset_playsub4_state();
    plpower_m = 1;
    plposiwkadr.w = 48;
    playposiwk[12] = 321;
    playposiwk[13] = 654;
    actwk[1].mstno.b.h = 1;
    muteki_get_work(&actwk[1])->history_offset = 20;
    muteki_sub(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 321, actwk[1].xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 654, actwk[1].yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, muteki_get_work(&actwk[1])->history_offset);

    reset_playsub4_state();
    plsubchg_flag = 8;
    actwk[1].r_no0 = 2;
    actionsub0(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 0, actionsub_count);

    reset_playsub4_state();
    actwk[1].r_no0 = 0;
    baria(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[1].r_no0);
    actwk[1].r_no0 = 2;
    plpower_b = 1;
    baria(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    actwk[1].r_no0 = 4;
    plpower_a = 1;
    baria(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    actwk[1].r_no0 = 6;
    plpower_a = 0;
    baria(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
}

static void test_air_bubble_wave_and_bou(test_context *ctx) {
    reset_playsub4_state();
    pl_air = 10;
    plpower_m = 0;
    bossstart = 0;
    plairset();
    TEST_ASSERT_EQ_INT(ctx, 130, bgmset_requests[0]);
    TEST_ASSERT_EQ_INT(ctx, 30, pl_air);
    stageno.w = 259;
    pl_air = 10;
    plairset();
    TEST_ASSERT_EQ_INT(ctx, 134, bgmset_requests[1]);
    plpower_m = 1;
    pl_air = 10;
    plairset();
    TEST_ASSERT_EQ_INT(ctx, 135, bgmset_requests[2]);
    bossstart = 1;
    pl_air = 10;
    plairset();
    TEST_ASSERT_EQ_INT(ctx, 140, bgmset_requests[3]);

    reset_playsub4_state();
    actwk[0].xposi.w.h = 1000;
    actwk[0].yposi.w.h = 1200;
    queue_actor(&actwk[40]);
    random_values[0] = 7;
    random_values[1] = 99;
    plawa_get_work(&actwk[1])->spawn_state = 1;
    plawamaster_jump2(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 33, actwk[40].actno);
    TEST_ASSERT_EQ_INT(ctx, 1006, actwk[40].xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1200, actwk[40].yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 6, actwk[40].userflag.b.h);

    reset_playsub4_state();
    actwk[0].xposi.w.h = 1000;
    actwk[0].yposi.w.h = 1200;
    actwk[0].cddat = 1;
    queue_actor(&actwk[40]);
    random_values[0] = 3;
    random_values[1] = 55;
    plawa_get_work(&actwk[1])->drowning_timer = 120;
    plawa_get_work(&actwk[1])->spawn_state = 1;
    plawamaster_jump2(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 994, actwk[40].xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1188, actwk[40].yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 55, actwk[40].direc.b.h);

    reset_playsub4_state();
    actwk[0].r_no0 = 0;
    actwk[0].cddat = 64;
    pl_air = 1;
    plawa_get_work(&actwk[1])->warning_timer = 1;
    plawa_get_work(&actwk[1])->bubble_timer = 0;
    queue_actor(&actwk[40]);
    plawamaster(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 0, pl_air);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);

    reset_playsub4_state();
    actwk[0].r_no0 = 6;
    plawamaster(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);
    actwk[0].r_no0 = 0;
    actwk[0].cddat = 0;
    plawamaster(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);
    actwk[0].cddat = 64;
    plawa_get_work(&actwk[1])->bubble_timer = 2;
    plawamaster(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, plawa_get_work(&actwk[1])->bubble_timer);

    reset_playsub4_state();
    actwk[0].r_no0 = 0;
    actwk[0].cddat = 64;
    pl_air = 0;
    plawa_get_work(&actwk[1])->bubble_timer = 0;
    plawamaster(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 129, player_work_get(&actwk[0])->status_flags);
    TEST_ASSERT_EQ_INT(ctx, 1, scroll_start.b.h);
    TEST_ASSERT_EQ_INT(ctx, 1, jumpcolsub_count);
    TEST_ASSERT_EQ_INT(ctx, 182, soundset_requests[1]);

    reset_playsub4_state();
    plawa_get_work(&actwk[1])->drowning_timer = 2;
    actwk[0].yspeed.w = 16;
    plawamaster(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, plawa_get_work(&actwk[1])->drowning_timer);
    TEST_ASSERT_EQ_INT(ctx, 1, speedset2_count);
    plawa_get_work(&actwk[1])->drowning_timer = 1;
    plawamaster(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 6, actwk[0].r_no0);

    reset_playsub4_state();
    plawa_get_work(&actwk[1])->spawn_state = 0;
    plawamaster_jump(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);
    plawa_get_work(&actwk[1])->spawn_state = 1;
    plawa_get_work(&actwk[1])->jump_timer = 1;
    plawamaster_jump(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);
    plawa_get_work(&actwk[1])->jump_timer = 0;
    queue_actor(&actwk[40]);
    plawamaster_jump(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    queue_actor(&actwk[41]);
    plawamaster_jump0(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 2, actwkchk_count);

    reset_playsub4_state();
    actwk[0].xposi.w.h = 1000;
    actwk[0].yposi.w.h = 1200;
    pl_air = 8;
    queue_actor(&actwk[40]);
    plawa_get_work(&actwk[1])->spawn_flags = 128;
    plawa_get_work(&actwk[1])->spawn_counter = 1;
    random_values[0] = 0;
    plawamaster_jump2(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 8192, actwk[40].sproffset);
    TEST_ASSERT_EQ_INT(ctx, 4, actwk[40].userflag.b.h);
    TEST_ASSERT_EQ_INT(ctx, 28, plawa_get_work(&actwk[40])->bubble_timer);

    reset_playsub4_state();
    actwk[0].xposi.w.h = 1000;
    actwk[0].yposi.w.h = 1200;
    pl_air = 8;
    queue_actor(&actwk[40]);
    plawa_get_work(&actwk[1])->spawn_flags = 128 | 64;
    plawa_get_work(&actwk[1])->spawn_counter = 1;
    random_values[0] = 0;
    plawamaster_jump2(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 6, actwk[40].userflag.b.h);

    reset_playsub4_state();
    actwk[0].xposi.w.h = 1000;
    actwk[0].yposi.w.h = 1200;
    queue_actor(&actwk[40]);
    plawa_get_work(&actwk[1])->spawn_flags = 128 | 64;
    plawa_get_work(&actwk[1])->spawn_counter = 0;
    random_values[0] = 0;
    random_values[1] = 1;
    plawamaster_jump2(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 6, actwk[40].userflag.b.h);

    reset_playsub4_state();
    actwk[0].xposi.w.h = 1000;
    actwk[0].yposi.w.h = 1200;
    pl_air = 10;
    queue_actor(&actwk[40]);
    plawa_get_work(&actwk[1])->spawn_flags = 128;
    plawa_get_work(&actwk[1])->spawn_counter = 0;
    random_values[0] = 0;
    random_values[1] = 1;
    plawamaster_jump2(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 5, actwk[40].userflag.b.h);

    reset_playsub4_state();
    actwk[1].userflag.b.h = 130;
    actwk[0].cddat = 0;
    plawa(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 10, actwk[1].r_no0);

    reset_playsub4_state();
    actwk[1].userflag.b.h = 2;
    actwk[1].xposi.w.h = 100;
    actwk[1].yposi.w.h = 200;
    waterposi = 250;
    plawainit(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 6, actwk[1].r_no0);
    TEST_ASSERT_EQ_INT(ctx, 9, actwk[1].mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);

    reset_playsub4_state();
    actwk[1].mstno.b.h = 20;
    actwk[1].yposi.w.h = 200;
    waterposi = 250;
    plawamove2(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 15, actwk[1].mstno.b.h);

    reset_playsub4_state();
    actwk[1].userflag.b.h = 1;
    actwk[1].xposi.w.h = 100;
    actwk[1].yposi.w.h = 200;
    waterposi = 100;
    awasintbl[2] = 7;
    plawainit(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[1].r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, speedset2_count);

    reset_playsub4_state();
    actwk[1].r_no0 = 2;
    waterposi = 100;
    actwk[1].yposi.w.h = 200;
    watercoliflag = 1;
    actwk[1].actflg = 0;
    plawa_get_work(&actwk[1])->origin_x = 50;
    awasintbl[2] = 4;
    plawamove2(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 54, plawa_get_work(&actwk[1])->origin_x);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);

    reset_playsub4_state();
    actwk[1].r_no0 = 12;
    plawa(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    plawamove4(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 2, frameout_count);

    reset_playsub4_state();
    pl_air = 13;
    plawamove5(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    reset_playsub4_state();
    pl_air = 12;
    plawa_get_work(&actwk[1])->bubble_timer = 1;
    actwk[1].mstno.b.h = 9;
    plawamove5(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 16, actwk[1].r_no0);
    TEST_ASSERT_EQ_INT(ctx, 15, actwk[1].mstno.b.h);
    reset_playsub4_state();
    pl_air = 12;
    plawa_get_work(&actwk[1])->bubble_timer = 2;
    actwk[1].actflg = 128;
    plawamove5(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);

    reset_playsub4_state();
    plawa_get_work(&actwk[1])->bubble_timer = 1;
    actwk[1].mstno.b.h = 1;
    scra_h_posit.w.h = 10;
    scra_v_posit.w.h = 20;
    plawasub(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 12, actwk[1].r_no0);
    TEST_ASSERT_EQ_INT(ctx, 15, plawa_get_work(&actwk[1])->bubble_timer);

    reset_playsub4_state();
    scra_h_posit.w.h = 100;
    waterposi = 200;
    wave(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[1].r_no0);
    TEST_ASSERT_EQ_INT(ctx, 164, actwk[1].xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 202, actwk[1].yposi.w.h);
    wave_move(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 356, actwk[1].xposi.w.h);
    wave_get_work(&actwk[1])->frame_counter = 3;
    wave_move(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 0, wave_get_work(&actwk[1])->frame_counter);

    reset_playsub4_state();
    actwk[0].xposi.w.h = 100;
    actwk[0].yposi.w.h = 100;
    actwk[0].xspeed.w = -1;
    actwk[1].xposi.w.h = 100;
    actwk[1].yposi.w.h = 100;
    bou_init(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 4, actwk[1].r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, player_work_get(&actwk[0])->status_flags & 1);
    TEST_ASSERT_EQ_INT(ctx, 76, actwk[0].xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 17, actwk[0].mstno.b.h);
    bou_get_work(&actwk[1])->hold_timer = 1;
    bou_move1(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 6, actwk[1].r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, player_work_get(&actwk[0])->status_flags & 1);

    reset_playsub4_state();
    actwk[0].xposi.w.h = 100;
    actwk[0].yposi.w.h = 100;
    actwk[1].xposi.w.h = 100;
    actwk[1].yposi.w.h = 100;
    actwk[1].userflag.b.h = 1;
    bou_move0(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 60, bou_get_work(&actwk[1])->hold_timer);
    bou_get_work(&actwk[1])->hold_timer = 2;
    swdata1.b.h = 1;
    actwk[0].yposi.w.h = 70;
    bou_move1(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 76, actwk[0].yposi.w.h);
    bou_get_work(&actwk[1])->hold_timer = 2;
    swdata1.b.h = 2;
    actwk[0].yposi.w.h = 130;
    bou_move1(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 124, actwk[0].yposi.w.h);

    reset_playsub4_state();
    actwk[1].r_no0 = 0;
    bou(&actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, 4, actwk[1].r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_s_count);
    bou_move2(&actwk[1]);

    reset_playsub4_state();
    editmode.b.h = 1;
    TEST_ASSERT_EQ_INT(ctx, 0, bou_coli(&actwk[1]));
    editmode.b.h = 0;
    actwk[0].xposi.w.h = 200;
    actwk[0].yposi.w.h = 100;
    actwk[1].xposi.w.h = 100;
    actwk[1].yposi.w.h = 100;
    TEST_ASSERT_EQ_INT(ctx, 0, bou_coli(&actwk[1]));
    actwk[0].xposi.w.h = 80;
    TEST_ASSERT_EQ_INT(ctx, 0, bou_coli(&actwk[1]));
    actwk[0].xposi.w.h = 100;
    actwk[0].yposi.w.h = 200;
    TEST_ASSERT_EQ_INT(ctx, 0, bou_coli(&actwk[1]));
    actwk[0].yposi.w.h = 20;
    TEST_ASSERT_EQ_INT(ctx, 0, bou_coli(&actwk[1]));
}

TEST_MAIN_BEGIN;
test_tables_and_patchg(&ctx);
test_playsave_marker_and_bakuha(&ctx);
test_flower_exit_and_barrier(&ctx);
test_air_bubble_wave_and_bou(&ctx);
TEST_MAIN_END
