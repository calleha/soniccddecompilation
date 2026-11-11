/* <stdlib.h> declares POSIX random(); the decompiled src/io.h declares the
   game's own Sint32 random(void). Parse stdlib first, then rename the game
   symbol inside this harness so the two can coexist. */
#include <stdlib.h>

#define random game_random

#include <string.h>

#include "support/test_runner.h"
#include "src/types.h"

Uint8 stagenm;
Uint8 ufoleft;
int_union spe_time;
Uint16 ringno;

static int patset_count;
static sprite_status_sp *patset_actor;
static int patinit_count;
static sprite_status_sp *patinit_actor;
static Uint8 patinit_pattern;
static int patinit1_count;
static sprite_status_sp *patinit1_actor;
static Uint8 patinit1_pattern;
static int key_set_count;
static Sint16 key_set_value;
static int scal_count;
static sprite_status_sp *scal_actor;
static int ufovspl_count;
static sprite_status_sp *ufovspl_actor;
static int ufovspl_sets_collision;
static int zbuf_set_count;
static sprite_status_sp *zbuf_set_actor;
static Sint32 zbuf_set_length;
static Sint32 dstns_value;
static int ufo_dec_count;
static int ring_add_count;
static Uint16 ring_add_value;
static Sint32 random_values[16];
static int random_count;
static int random_index;

void patset(sprite_status_sp *work);
void patinit(sprite_status_sp *work, Uint8 pattbl_number);
void patinit1(sprite_status_sp *work, Uint8 pattbl_number);
void zbuf_set(sprite_status_sp *work, Sint32 length);
void ufovspl(sprite_status_sp *pActwk);
void key_set(Sint16 ReqNo);
void scal(sprite_status_sp *pActwk);
void angleget(Sint16 iXtget, Sint16 iYtget, Sint16 iXbase, Sint16 iYbase,
              Uint8 *iAngle, Uint8 *iDirflg);
Sint32 dstnsget(Uint8 bAngle, Uint8 bDirflg, Sint16 iXposi, Sint16 iYposi,
                Sint16 iXbase, Sint16 iYbase);
Sint32 random(void);
void ufo_dec(void);
void ring_add(Uint16 iD0);

#include "src/special/sps_equ.c"
#include "src/special/ens.c"

sprite_patterns_sp *mpkage[10] = {
    &mpkageTBL0, &mpkageTBL1, &mpkageTBL2, &mpkageTBL3, &mpkageTBL4,
    &mpkageTBL5, &mpkageTBL6, &mpkageTBL7, &mpkageTBL8, &mpkageTBL9};
sprite_patterns_sp *mpufox[10] = {
    &mpufoxTBL0, &mpufoxTBL1, &mpufoxTBL2, &mpufoxTBL3, &mpufoxTBL4,
    &mpufoxTBL5, &mpufoxTBL6, &mpufoxTBL7, &mpufoxTBL8, &mpufoxTBL9};
sprite_patterns_sp *mpufoy[10] = {
    &mpufoyTBL0, &mpufoyTBL1, &mpufoyTBL2, &mpufoyTBL3, &mpufoyTBL4,
    &mpufoyTBL5, &mpufoyTBL6, &mpufoyTBL7, &mpufoyTBL8, &mpufoyTBL9};

void patset(sprite_status_sp *work) {
    ++patset_count;
    patset_actor = work;
}

void patinit(sprite_status_sp *work, Uint8 pattbl_number) {
    ++patinit_count;
    patinit_actor = work;
    patinit_pattern = pattbl_number;
    work->pattblno = pattbl_number;
    work->patno = 0;
}

void patinit1(sprite_status_sp *work, Uint8 pattbl_number) {
    ++patinit1_count;
    patinit1_actor = work;
    patinit1_pattern = pattbl_number;
    work->pattblno = pattbl_number;
}

void zbuf_set(sprite_status_sp *work, Sint32 length) {
    ++zbuf_set_count;
    zbuf_set_actor = work;
    zbuf_set_length = length;
}

void ufovspl(sprite_status_sp *pActwk) {
    ++ufovspl_count;
    ufovspl_actor = pActwk;
    if (ufovspl_sets_collision) {
        pActwk->colliflg = actwk[0].actno;
    }
}

void key_set(Sint16 ReqNo) {
    ++key_set_count;
    key_set_value = ReqNo;
}

void scal(sprite_status_sp *pActwk) {
    ++scal_count;
    scal_actor = pActwk;
}

void angleget(Sint16 iXtget, Sint16 iYtget, Sint16 iXbase, Sint16 iYbase,
              Uint8 *iAngle, Uint8 *iDirflg) {
    (void)iXtget;
    (void)iYtget;
    (void)iXbase;
    (void)iYbase;
    *iAngle = 3;
    *iDirflg = 4;
}

Sint32 dstnsget(Uint8 bAngle, Uint8 bDirflg, Sint16 iXposi, Sint16 iYposi,
                Sint16 iXbase, Sint16 iYbase) {
    (void)bAngle;
    (void)bDirflg;
    (void)iXposi;
    (void)iYposi;
    (void)iXbase;
    (void)iYbase;
    return dstns_value;
}

Sint32 random(void) {
    Sint32 value = 0;

    if (random_index < random_count) {
        value = random_values[random_index++];
    }
    return value;
}

void ufo_dec(void) {
    ++ufo_dec_count;
    --ufoleft;
}

void ring_add(Uint16 iD0) {
    ++ring_add_count;
    ring_add_value = iD0;
    ringno += iD0;
}

static void queue_random(Sint32 value) {
    random_values[random_count++] = value;
}

static void reset_ens_state(void) {
    memset(actwk, 0, sizeof(actwk));
    patset_count = 0;
    patset_actor = 0;
    patinit_count = 0;
    patinit_actor = 0;
    patinit_pattern = 255;
    patinit1_count = 0;
    patinit1_actor = 0;
    patinit1_pattern = 255;
    key_set_count = 0;
    key_set_value = 0;
    scal_count = 0;
    scal_actor = 0;
    ufovspl_count = 0;
    ufovspl_actor = 0;
    ufovspl_sets_collision = 0;
    zbuf_set_count = 0;
    zbuf_set_actor = 0;
    zbuf_set_length = 0;
    dstns_value = 256;
    ufo_dec_count = 0;
    ring_add_count = 0;
    ring_add_value = 0;
    memset(random_values, 0, sizeof(random_values));
    random_count = 0;
    random_index = 0;
    stagenm = 0;
    ufoleft = 0;
    spe_time.l = 0;
    ringno = 0;
    pmflag = 0;
    time_stop = 0;
    game_start = 0;
    rufo_getnm = 0;
}

static void test_bara_ring_initializes_from_player_and_animates(
    test_context *ctx) {
    sprite_status_sp *ring = &actwk[12];

    reset_ens_state();
    actwk[0].sx_posi.w.h = 210;
    actwk[0].sy_posi.w.h = 180;
    queue_random(0x0003f000);

    bara_ring(ring);

    TEST_ASSERT_EQ_INT(ctx, 1, ring->exeno);
    TEST_ASSERT_EQ_INT(ctx, 59279, ring->sprcolor);
    TEST_ASSERT_TRUE(ctx, ring->pattbl == mpitem);
    TEST_ASSERT_EQ_INT(ctx, 4, special_enemy_get_work(ring)->pattern);
    TEST_ASSERT_EQ_INT(ctx, 4, patinit_pattern);
    TEST_ASSERT_TRUE(ctx, patinit_actor == ring);
    TEST_ASSERT_EQ_INT(ctx, 210, ring->sx_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 180, ring->sy_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 45, special_enemy_get_work(ring)->timer_low);
    TEST_ASSERT_EQ_INT(ctx, 1, pmflag);
    TEST_ASSERT_TRUE(ctx, ring->sx_speed.l > 0);
    TEST_ASSERT_EQ_INT(ctx, -10, ring->sy_speed.w.h);
    TEST_ASSERT_TRUE(ctx, patset_actor == ring);
}

static void test_bara_ring_alternates_x_speed_direction(test_context *ctx) {
    sprite_status_sp *ring = &actwk[12];

    reset_ens_state();
    pmflag = 1;
    queue_random(0x0003f000);

    b_ring00(ring);

    TEST_ASSERT_EQ_INT(ctx, 0, pmflag);
    TEST_ASSERT_TRUE(ctx, ring->sx_speed.l < 0);
}

static void test_bara_ring_wrapper_runs_move_state(test_context *ctx) {
    sprite_status_sp *ring = &actwk[12];

    reset_ens_state();
    ring->exeno = 1;
    special_enemy_get_work(ring)->timer_low = 2;
    ring->sy_posi.l = 300 << 16;

    bara_ring(ring);

    TEST_ASSERT_EQ_INT(ctx, 1, special_enemy_get_work(ring)->timer_low);
    TEST_ASSERT_TRUE(ctx, patset_actor == ring);
}

static void test_bara_ring_move_counts_down_and_bounces(test_context *ctx) {
    sprite_status_sp *ring = &actwk[12];

    reset_ens_state();
    special_enemy_get_work(ring)->timer_low = 1;
    ring->sx_speed.l = 0x00020000;
    ring->sy_speed.l = 0x00010000;
    ring->sx_posi.l = 100 << 16;
    ring->sy_posi.l = 346 << 16;

    b_ring01(ring);

    TEST_ASSERT_EQ_INT(ctx, 1, ring->actflg & 1);
    TEST_ASSERT_EQ_INT(ctx, 102, ring->sx_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 344, ring->sy_posi.w.h);
    TEST_ASSERT_TRUE(ctx, ring->sy_speed.l < 0);
}

static void test_bara_ring_move_applies_gravity_before_bounce_height(
    test_context *ctx) {
    sprite_status_sp *ring = &actwk[12];

    reset_ens_state();
    special_enemy_get_work(ring)->timer_low = 2;
    ring->sy_speed.l = 0x00010000;
    ring->sy_posi.l = 300 << 16;

    b_ring01(ring);

    TEST_ASSERT_EQ_INT(ctx, 1, special_enemy_get_work(ring)->timer_low);
    TEST_ASSERT_EQ_INT(ctx, 301, ring->sy_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 3, ring->sy_speed.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, ring->actflg & 1);
}

static void test_item_ring_initializes_sound_and_rises(test_context *ctx) {
    sprite_status_sp *item = &actwk[16];

    reset_ens_state();
    special_enemy_get_work(item)->subtype = 3;

    item_ring(item);

    TEST_ASSERT_EQ_INT(ctx, 1, item->exeno);
    TEST_ASSERT_EQ_INT(ctx, 34703, item->sprcolor);
    TEST_ASSERT_EQ_INT(ctx, 3, special_enemy_get_work(item)->pattern);
    TEST_ASSERT_EQ_INT(ctx, 16, special_enemy_get_work(item)->timer_low);
    TEST_ASSERT_EQ_INT(ctx, -16, item->sy_speed.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, key_set_count);
    TEST_ASSERT_EQ_INT(ctx, 149, key_set_value);
    TEST_ASSERT_TRUE(ctx, patset_actor == item);

    special_enemy_get_work(item)->timer_low = 1;
    i_ring01(item);

    TEST_ASSERT_EQ_INT(ctx, 1, item->actflg & 1);
    TEST_ASSERT_EQ_INT(ctx, -16, item->sy_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, -14, item->sy_speed.w.h);
}

static void test_item_ring_wrapper_runs_move_state(test_context *ctx) {
    sprite_status_sp *item = &actwk[16];

    reset_ens_state();
    item->exeno = 1;
    special_enemy_get_work(item)->timer_low = 2;
    item->sy_speed.w.h = -4;

    item_ring(item);

    TEST_ASSERT_EQ_INT(ctx, 1, special_enemy_get_work(item)->timer_low);
    TEST_ASSERT_EQ_INT(ctx, -4, item->sy_posi.w.h);
    TEST_ASSERT_TRUE(ctx, patset_actor == item);
}

static void test_press_blinks_by_timer_bit(test_context *ctx) {
    sprite_status_sp *press_actor = &actwk[9];

    reset_ens_state();

    press(press_actor);

    TEST_ASSERT_EQ_INT(ctx, 1, press_actor->exeno);
    TEST_ASSERT_EQ_INT(ctx, 34154, press_actor->sprcolor);
    TEST_ASSERT_TRUE(ctx, press_actor->pattbl == mppress);
    TEST_ASSERT_EQ_INT(ctx, 212, press_actor->sx_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 208, press_actor->sy_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, press_actor->actflg & 4);

    special_enemy_get_work(press_actor)->timer_low = 15;
    press01(press_actor);
    TEST_ASSERT_EQ_INT(ctx, 4, press_actor->actflg & 4);
}

static void test_press_wrapper_runs_existing_blink_state(test_context *ctx) {
    sprite_status_sp *press_actor = &actwk[9];

    reset_ens_state();
    press_actor->exeno = 1;
    special_enemy_get_work(press_actor)->timer_low = 1;

    press(press_actor);

    TEST_ASSERT_EQ_INT(ctx, 2, special_enemy_get_work(press_actor)->timer_low);
    TEST_ASSERT_EQ_INT(ctx, 0, press_actor->actflg & 4);
    TEST_ASSERT_TRUE(ctx, patset_actor == press_actor);
}

static void test_title_logo_runs_intro_wait_and_exit(test_context *ctx) {
    sprite_status_sp *logo = &actwk[10];
    int i;

    reset_ens_state();

    title_logo(logo);
    TEST_ASSERT_EQ_INT(ctx, 1, logo->exeno);
    TEST_ASSERT_EQ_INT(ctx, 456, logo->sx_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 240, logo->sy_posi.w.h);

    tlogo01(logo);
    TEST_ASSERT_EQ_INT(ctx, 1, logo->exeno);
    TEST_ASSERT_EQ_INT(ctx, 424, logo->sx_posi.w.h);

    for (i = 0; i < 3; ++i) {
        tlogo01(logo);
    }
    TEST_ASSERT_EQ_INT(ctx, 2, logo->exeno);
    TEST_ASSERT_EQ_INT(ctx, 332, logo->sx_posi.w.h);

    for (i = 0; i < 80; ++i) {
        tlogo02(logo);
    }
    TEST_ASSERT_EQ_INT(ctx, 3, logo->exeno);

    tlogo03(logo);
    TEST_ASSERT_EQ_INT(ctx, 0, logo->actflg & 1);
    for (i = 0; i < 4; ++i) {
        tlogo03(logo);
    }
    TEST_ASSERT_EQ_INT(ctx, 1, logo->actflg & 1);
}

static void test_title_logo_wrapper_dispatches_later_states(test_context *ctx) {
    sprite_status_sp *logo = &actwk[10];

    reset_ens_state();
    logo->exeno = 1;
    logo->sx_posi.w.h = 340;
    title_logo(logo);
    TEST_ASSERT_EQ_INT(ctx, 2, logo->exeno);

    logo->exeno = 2;
    title_logo(logo);
    TEST_ASSERT_EQ_INT(ctx, 2, logo->exeno);

    logo->exeno = 3;
    logo->sx_posi.w.h = 464;
    title_logo(logo);
    TEST_ASSERT_EQ_INT(ctx, 1, logo->actflg & 1);
}

static void test_title_obi_controls_player_and_game_start(test_context *ctx) {
    sprite_status_sp *obi = &actwk[11];
    int i;

    reset_ens_state();
    game_start = 1;

    title_obi(obi);
    TEST_ASSERT_EQ_INT(ctx, 1, obi->exeno);
    TEST_ASSERT_EQ_INT(ctx, 248, obi->sx_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 64, obi->sy_posi.w.h);

    for (i = 0; i < 6; ++i) {
        tobi01(obi);
    }
    TEST_ASSERT_EQ_INT(ctx, 2, obi->exeno);
    TEST_ASSERT_EQ_INT(ctx, 240, obi->sy_posi.w.h);

    for (i = 0; i < 30; ++i) {
        tobi02(obi);
    }
    TEST_ASSERT_EQ_INT(ctx, 21, actwk[0].exeno);

    for (i = 0; i < 50; ++i) {
        tobi02(obi);
    }
    TEST_ASSERT_EQ_INT(ctx, 3, obi->exeno);

    for (i = 0; i < 8; ++i) {
        tobi03(obi);
    }
    TEST_ASSERT_EQ_INT(ctx, 4, obi->exeno);
    TEST_ASSERT_EQ_INT(ctx, 4, obi->actflg & 4);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].exeno);

    for (i = 0; i < 3; ++i) {
        tobi04(obi);
    }
    TEST_ASSERT_EQ_INT(ctx, 1, obi->actflg & 1);
    TEST_ASSERT_EQ_INT(ctx, 0, game_start);
}

static void test_title_obi_wrapper_dispatches_later_states(test_context *ctx) {
    sprite_status_sp *obi = &actwk[11];

    reset_ens_state();
    obi->exeno = 2;
    title_obi(obi);
    TEST_ASSERT_EQ_INT(ctx, 2, obi->exeno);

    obi->exeno = 3;
    obi->sy_posi.w.h = 1;
    title_obi(obi);
    TEST_ASSERT_EQ_INT(ctx, 4, obi->exeno);

    obi->exeno = 4;
    special_enemy_get_work(obi)->timer_low = 1;
    special_enemy_get_work(obi)->subtype = 0;
    title_obi(obi);
    TEST_ASSERT_EQ_INT(ctx, 1, obi->actflg & 1);
}

static void test_exp_set_finds_first_free_explosion_slot(test_context *ctx) {
    sprite_status_sp *slot = 0;
    int i;

    reset_ens_state();

    TEST_ASSERT_EQ_INT(ctx, 0, exp_set(&slot));
    TEST_ASSERT_TRUE(ctx, slot == &actwk[24]);

    for (i = 24; i < 31; ++i) {
        actwk[i].actno = 12;
    }

    TEST_ASSERT_EQ_INT(ctx, 1, exp_set(&slot));
}

static void test_explosion_initializes_timer_sound_and_expires(test_context *ctx) {
    sprite_status_sp *expl = &actwk[24];
    int i;

    reset_ens_state();

    explosion(expl);

    TEST_ASSERT_EQ_INT(ctx, 1, expl->exeno);
    TEST_ASSERT_EQ_INT(ctx, 34734, expl->sprcolor);
    TEST_ASSERT_TRUE(ctx, expl->pattbl == mpeexp);
    TEST_ASSERT_EQ_INT(ctx, 0, patinit_pattern);
    TEST_ASSERT_EQ_INT(ctx, 163, key_set_value);

    for (i = 0; i < 12; ++i) {
        eexp01(expl);
    }
    TEST_ASSERT_EQ_INT(ctx, 1, expl->actflg & 1);
}

static void test_shadow_and_player_shadow_follow_sources(test_context *ctx) {
    sprite_status_sp *shadow = &actwk[8];
    sprite_status_sp *player_shadow = &actwk[6];

    reset_ens_state();
    actwk[0].x_posi.w.h = 120;
    actwk[0].y_posi.w.h = 130;
    actwk[0].z_posi.w.h = 900;

    plkage(player_shadow);
    TEST_ASSERT_EQ_INT(ctx, 1, player_shadow->exeno);
    TEST_ASSERT_TRUE(ctx, player_shadow->pattbl == mpkage);
    TEST_ASSERT_EQ_INT(ctx, 120, player_shadow->x_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 130, player_shadow->y_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 900, player_shadow->z_posi.w.h);
    TEST_ASSERT_TRUE(ctx, scal_actor == player_shadow);

    reset_ens_state();
    actwk[0].z_posi.w.h = 640;
    actwk[3].x_posi.w.h = 200;
    actwk[3].y_posi.w.h = 220;
    actwk[3].z_posi.w.h = 640;
    special_enemy_get_work(shadow)->linked_actor_index = 3;
    shadow->actflg = 4;

    kage(shadow);

    TEST_ASSERT_EQ_INT(ctx, 1, shadow->exeno);
    TEST_ASSERT_EQ_INT(ctx, 200, shadow->x_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 220, shadow->y_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 640, shadow->z_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 4, shadow->actflg & 4);
    TEST_ASSERT_EQ_INT(ctx, 4, actwk[3].actflg & 4);
}

static void test_shadow_existing_state_clears_source_hidden_flag(
    test_context *ctx) {
    sprite_status_sp *shadow = &actwk[8];

    reset_ens_state();
    actwk[0].z_posi.w.h = 500;
    actwk[3].x_posi.w.h = 210;
    actwk[3].y_posi.w.h = 230;
    actwk[3].actflg = 4;
    shadow->exeno = 1;
    special_enemy_get_work(shadow)->linked_actor_index = 3;

    kage01(shadow);

    TEST_ASSERT_EQ_INT(ctx, 210, shadow->x_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 230, shadow->y_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[3].actflg & 4);
}

static void test_ufo_frout_chk_sets_hidden_outside_screen(test_context *ctx) {
    sprite_status_sp *ufo = &actwk[32];

    reset_ens_state();
    ufo->actflg = 4;
    ufo->sx_posi.w.h = 100;
    ufo->sy_posi.w.h = 300;

    ufo_frout_chk(ufo);
    TEST_ASSERT_EQ_INT(ctx, 0, ufo->actflg & 4);

    ufo->sx_posi.w.h = 512;
    ufo_frout_chk(ufo);
    TEST_ASSERT_EQ_INT(ctx, 4, ufo->actflg & 4);

    ufo->sx_posi.w.h = 100;
    ufo->sy_posi.w.h = 255;
    ufo_frout_chk(ufo);
    TEST_ASSERT_EQ_INT(ctx, 4, ufo->actflg & 4);

    ufo->actflg = 0;
    ufo->sy_posi.w.h = 448;
    ufo_frout_chk(ufo);
    TEST_ASSERT_EQ_INT(ctx, 4, ufo->actflg & 4);
}

static void test_ufo_initial_creates_stage_ufo_and_shadow_pairs(
    test_context *ctx) {
    reset_ens_state();
    stagenm = 0;

    ufo_initial();

    TEST_ASSERT_EQ_INT(ctx, 6, ufoleft);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[32].actno);
    TEST_ASSERT_EQ_INT(ctx, 0, special_enemy_get_work(&actwk[32])->ufo_type);
    TEST_ASSERT_EQ_INT(ctx, 0, special_enemy_get_work(&actwk[32])->ufo_direction);
    TEST_ASSERT_EQ_INT(ctx, 5, actwk[40].actno);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[37].actno);
    TEST_ASSERT_EQ_INT(ctx, 5, actwk[45].actno);
}

static void test_tufo_initial_respects_time_gate_and_existing_actor(
    test_context *ctx) {
    reset_ens_state();
    spe_time.l = 21;

    tufo_initial();
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[39].actno);

    spe_time.l = 20;
    actwk[39].actno = 9;
    tufo_initial();
    TEST_ASSERT_EQ_INT(ctx, 9, actwk[39].actno);

    actwk[39].actno = 0;
    tufo_initial();
    TEST_ASSERT_EQ_INT(ctx, 3, actwk[39].actno);
    TEST_ASSERT_EQ_INT(ctx, 2, special_enemy_get_work(&actwk[39])->ufo_type);
    TEST_ASSERT_EQ_INT(ctx, 0, special_enemy_get_work(&actwk[39])->ufo_direction);
    TEST_ASSERT_EQ_INT(ctx, 5, actwk[47].actno);
}

static void test_ufo0_initializes_moves_and_hides_during_start(
    test_context *ctx) {
    sprite_status_sp *ufo = &actwk[32];

    reset_ens_state();
    stagenm = 0;
    actwk[0].z_posi.w.h = 1000;
    u_init(0, ufo);

    ufo0(ufo);

    TEST_ASSERT_EQ_INT(ctx, 1, ufo->exeno);
    TEST_ASSERT_EQ_INT(ctx, 58432, ufo->sprcolor);
    TEST_ASSERT_TRUE(ctx, ufo->pattbl == mpufox);
    TEST_ASSERT_EQ_INT(ctx, 680, ufo->z_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, special_enemy_get_work(ufo)->flash_timer);
    TEST_ASSERT_EQ_INT(ctx, 4, ufo->actflg & 4);
    TEST_ASSERT_TRUE(ctx, zbuf_set_actor == ufo);
    TEST_ASSERT_TRUE(ctx, scal_actor == ufo);
    TEST_ASSERT_TRUE(ctx, ufovspl_actor == ufo);
}

static void test_ufo0_wrapper_runs_explosion_state(test_context *ctx) {
    sprite_status_sp *ufo = &actwk[32];

    reset_ens_state();
    ufo->exeno = 2;
    special_enemy_get_work(ufo)->timer_low = 2;
    special_enemy_get_work(ufo)->subtype = 1;

    ufo0(ufo);

    TEST_ASSERT_EQ_INT(ctx, 1, special_enemy_get_work(ufo)->timer_low);
    TEST_ASSERT_EQ_INT(ctx, 0, ufo->actflg & 1);
}

static void test_ufo0_uses_y_pattern_table_for_vertical_path(test_context *ctx) {
    sprite_status_sp *ufo = &actwk[32];

    reset_ens_state();
    stagenm = 0;
    u_init(2, ufo);

    ufo00(ufo);

    TEST_ASSERT_TRUE(ctx, ufo->pattbl == mpufoy);
}

static void test_timeufo_initializes_moves_and_requests_sound(test_context *ctx) {
    sprite_status_sp *tufo = &actwk[39];

    reset_ens_state();
    actwk[0].z_posi.w.h = 900;
    tufo_initial();

    timeufo(tufo);

    TEST_ASSERT_EQ_INT(ctx, 1, tufo->exeno);
    TEST_ASSERT_EQ_INT(ctx, 33856, tufo->sprcolor);
    TEST_ASSERT_TRUE(ctx, tufo->pattbl == tpufox);
    TEST_ASSERT_EQ_INT(ctx, 580, tufo->z_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, special_enemy_get_work(tufo)->flash_timer);
    TEST_ASSERT_EQ_INT(ctx, 188, key_set_value);
    TEST_ASSERT_TRUE(ctx, zbuf_set_actor == tufo);
    TEST_ASSERT_TRUE(ctx, scal_actor == tufo);
}

static void test_ufo_and_timeufo_refresh_route_when_counter_expires(
    test_context *ctx) {
    sprite_status_sp *ufo = &actwk[32];
    sprite_status_sp *tufo = &actwk[39];

    reset_ens_state();
    stagenm = 0;
    u_init(0, ufo);
    ufo00(ufo);
    special_enemy_get_work(ufo)->movement_count = 1;

    ufo01(ufo);

    TEST_ASSERT_TRUE(ctx, ufo->x_speed.l != 0 || ufo->y_speed.l != 0);

    reset_ens_state();
    tufo_initial();
    tufo00(tufo);
    special_enemy_get_work(tufo)->movement_count = 1;

    tufo01(tufo);

    TEST_ASSERT_TRUE(ctx, tufo->x_speed.l != 0 || tufo->y_speed.l != 0);
}

static void test_timeufo_wrapper_runs_explosion_state_and_stop_guard(
    test_context *ctx) {
    sprite_status_sp *tufo = &actwk[39];

    reset_ens_state();
    tufo->exeno = 2;
    special_enemy_get_work(tufo)->timer_low = 1;
    special_enemy_get_work(tufo)->subtype = 0;

    timeufo(tufo);

    TEST_ASSERT_EQ_INT(ctx, 1, tufo->actflg & 1);

    reset_ens_state();
    actwk[0].actno = 1;
    tufo_initial();
    tufo00(tufo);
    ufovspl_sets_collision = 1;
    time_stop = 1;

    tufo01(tufo);

    TEST_ASSERT_EQ_INT(ctx, 1, tufo->exeno);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[16].actno);
}

static void test_ufo_collision_switches_to_reward_state(test_context *ctx) {
    sprite_status_sp *ufo = &actwk[32];

    reset_ens_state();
    stagenm = 0;
    ufoleft = 1;
    rufo_getnm = 6;
    actwk[0].actno = 1;
    u_init(0, ufo);
    ufo00(ufo);
    ufovspl_sets_collision = 1;
    queue_random(1);

    ufo01(ufo);

    TEST_ASSERT_EQ_INT(ctx, 1, time_stop);
    TEST_ASSERT_EQ_INT(ctx, 1, ufo_dec_count);
    TEST_ASSERT_EQ_INT(ctx, 2, ufo->exeno);
    TEST_ASSERT_EQ_INT(ctx, 12, rufo_getnm);
    TEST_ASSERT_EQ_INT(ctx, 1, ring_add_count);
    TEST_ASSERT_EQ_INT(ctx, 6, ring_add_value);
    TEST_ASSERT_EQ_INT(ctx, 4, actwk[16].actno);
    TEST_ASSERT_EQ_INT(ctx, 0, special_enemy_get_work(&actwk[16])->subtype);
}

static void test_ufo_collision_type_one_sets_player_timer(test_context *ctx) {
    sprite_status_sp *ufo = &actwk[32];

    reset_ens_state();
    stagenm = 0;
    ufoleft = 3;
    actwk[0].actno = 1;
    u_init(2, ufo);
    ufo00(ufo);
    ufovspl_sets_collision = 1;

    ufo01(ufo);

    TEST_ASSERT_EQ_INT(ctx, 0, time_stop);
    TEST_ASSERT_EQ_INT(ctx, 20, rufo_getnm);
    TEST_ASSERT_EQ_INT(ctx, 0, ring_add_count);
    TEST_ASSERT_EQ_INT(ctx, 1, special_enemy_get_work(&actwk[16])->subtype);
}

static void test_timeufo_collision_adds_time_and_spawns_item_ring(
    test_context *ctx) {
    sprite_status_sp *tufo = &actwk[39];

    reset_ens_state();
    actwk[0].actno = 1;
    tufo_initial();
    tufo00(tufo);
    ufovspl_sets_collision = 1;
    spe_time.l = 10;
    queue_random(1);

    tufo01(tufo);

    TEST_ASSERT_EQ_INT(ctx, 2, tufo->exeno);
    TEST_ASSERT_EQ_INT(ctx, 40, spe_time.l);
    TEST_ASSERT_EQ_INT(ctx, 4, actwk[16].actno);
    TEST_ASSERT_EQ_INT(ctx, 3, special_enemy_get_work(&actwk[16])->subtype);
}

static void test_ufo02_and_tufo02_spawn_explosions_while_falling(
    test_context *ctx) {
    sprite_status_sp *ufo = &actwk[32];
    sprite_status_sp *tufo = &actwk[39];

    reset_ens_state();
    ufo->sx_posi.w.h = 200;
    ufo->sy_posi.w.h = 220;
    special_enemy_get_work(ufo)->timer_low = 2;
    special_enemy_get_work(ufo)->subtype = 0;
    special_enemy_get_work(ufo)->direction = 0;
    queue_random(7);

    ufo02(ufo);

    TEST_ASSERT_EQ_INT(ctx, 204, ufo->sx_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 221, ufo->sy_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 12, actwk[24].actno);
    TEST_ASSERT_EQ_INT(ctx, 195, actwk[24].sx_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 214, actwk[24].sy_posi.w.h);

    reset_ens_state();
    tufo->sx_posi.w.h = 180;
    tufo->sy_posi.w.h = 210;
    special_enemy_get_work(tufo)->timer_low = 2;
    special_enemy_get_work(tufo)->subtype = 0;
    special_enemy_get_work(tufo)->direction = 1;
    queue_random(5);

    tufo02(tufo);

    TEST_ASSERT_EQ_INT(ctx, 176, tufo->sx_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 211, tufo->sy_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 12, actwk[24].actno);
    TEST_ASSERT_EQ_INT(ctx, 165, actwk[24].sx_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 206, actwk[24].sy_posi.w.h);
}

static void test_ufo_explosion_states_cover_timer_and_allocator_failures(
    test_context *ctx) {
    sprite_status_sp *ufo = &actwk[32];
    sprite_status_sp *tufo = &actwk[39];
    int i;

    reset_ens_state();
    special_enemy_get_work(ufo)->timer_low = 1;
    special_enemy_get_work(ufo)->subtype = 0;
    ufo02(ufo);
    TEST_ASSERT_EQ_INT(ctx, 1, ufo->actflg & 1);

    reset_ens_state();
    for (i = 24; i < 31; ++i) {
        actwk[i].actno = 12;
    }
    special_enemy_get_work(ufo)->timer_low = 2;
    special_enemy_get_work(ufo)->subtype = 0;
    ufo02(ufo);
    TEST_ASSERT_EQ_INT(ctx, 12, actwk[24].actno);

    reset_ens_state();
    special_enemy_get_work(tufo)->timer_low = 1;
    special_enemy_get_work(tufo)->subtype = 0;
    tufo02(tufo);
    TEST_ASSERT_EQ_INT(ctx, 1, tufo->actflg & 1);

    reset_ens_state();
    for (i = 24; i < 31; ++i) {
        actwk[i].actno = 12;
    }
    special_enemy_get_work(tufo)->timer_low = 2;
    special_enemy_get_work(tufo)->subtype = 0;
    tufo02(tufo);
    TEST_ASSERT_EQ_INT(ctx, 12, actwk[24].actno);
}

static void test_mvtblset_wraps_to_saved_route_at_sentinel(test_context *ctx) {
    sprite_status_sp *ufo = &actwk[32];
    int i;

    reset_ens_state();
    stagenm = 0;
    u_init(0, ufo);

    for (i = 0; i < 3; ++i) {
        mvtblset(ufo);
    }

    TEST_ASSERT_TRUE(ctx, ufo->x_speed.l != 0 || ufo->y_speed.l != 0);
}

static void test_ptset_ufo_updates_pattern_and_can_return_when_unchanged(
    test_context *ctx) {
    sprite_status_sp *ufo = &actwk[32];

    reset_ens_state();
    actwk[0].x_posi.w.h = 100;
    actwk[0].y_posi.w.h = 100;
    ufo->x_posi.w.h = 120;
    ufo->y_posi.w.h = 120;
    dstns_value = 256;

    ptset_ufo(ufo);

    TEST_ASSERT_EQ_INT(ctx, 5, special_enemy_get_work(ufo)->pattern);
    TEST_ASSERT_EQ_INT(ctx, 1, patinit1_count);
    TEST_ASSERT_EQ_INT(ctx, 5, patinit1_pattern);

    ptset_ufo(ufo);
    TEST_ASSERT_EQ_INT(ctx, 1, patinit1_count);

    dstns_value = 2000;
    ptset_ufo(ufo);
    TEST_ASSERT_EQ_INT(ctx, 9, special_enemy_get_work(ufo)->pattern);
}

TEST_MAIN_BEGIN;
test_bara_ring_initializes_from_player_and_animates(&ctx);
test_bara_ring_alternates_x_speed_direction(&ctx);
test_bara_ring_wrapper_runs_move_state(&ctx);
test_bara_ring_move_counts_down_and_bounces(&ctx);
test_bara_ring_move_applies_gravity_before_bounce_height(&ctx);
test_item_ring_initializes_sound_and_rises(&ctx);
test_item_ring_wrapper_runs_move_state(&ctx);
test_press_blinks_by_timer_bit(&ctx);
test_press_wrapper_runs_existing_blink_state(&ctx);
test_title_logo_runs_intro_wait_and_exit(&ctx);
test_title_logo_wrapper_dispatches_later_states(&ctx);
test_title_obi_controls_player_and_game_start(&ctx);
test_title_obi_wrapper_dispatches_later_states(&ctx);
test_exp_set_finds_first_free_explosion_slot(&ctx);
test_explosion_initializes_timer_sound_and_expires(&ctx);
test_shadow_and_player_shadow_follow_sources(&ctx);
test_shadow_existing_state_clears_source_hidden_flag(&ctx);
test_ufo_frout_chk_sets_hidden_outside_screen(&ctx);
test_ufo_initial_creates_stage_ufo_and_shadow_pairs(&ctx);
test_tufo_initial_respects_time_gate_and_existing_actor(&ctx);
test_ufo0_initializes_moves_and_hides_during_start(&ctx);
test_ufo0_uses_y_pattern_table_for_vertical_path(&ctx);
test_ufo0_wrapper_runs_explosion_state(&ctx);
test_timeufo_initializes_moves_and_requests_sound(&ctx);
test_ufo_and_timeufo_refresh_route_when_counter_expires(&ctx);
test_timeufo_wrapper_runs_explosion_state_and_stop_guard(&ctx);
test_ufo_collision_switches_to_reward_state(&ctx);
test_ufo_collision_type_one_sets_player_timer(&ctx);
test_timeufo_collision_adds_time_and_spawns_item_ring(&ctx);
test_ufo02_and_tufo02_spawn_explosions_while_falling(&ctx);
test_ufo_explosion_states_cover_timer_and_allocator_failures(&ctx);
test_mvtblset_wraps_to_saved_route_at_sentinel(&ctx);
test_ptset_ufo_updates_pattern_and_can_return_when_unchanged(&ctx);
TEST_MAIN_END
