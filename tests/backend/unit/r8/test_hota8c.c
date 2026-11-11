#include <string.h>

#include "support/test_runner.h"
#include "src/types.h"
#include "src/r8/tobira_work.h"

sprite_status actwk[128];
short_union gametimer;
Uint8 shut_flag;
Uint8 bossflag;
Sint16 scra_vline;
static PALETTEENTRY color_buffer[128];
PALETTEENTRY *lpcolorwk = color_buffer;

static int actionsub_count;
static sprite_status *actionsub_actor;
static int frameout_count;
static sprite_status *frameout_actor;
static int patchg_count;
static sprite_status *patchg_actor;
static Uint8 **patchg_table;
static int actwkchk_count;
static sprite_status *actwkchk_queue[8];
static int actwkchk_queue_count;
static int actwkchk_queue_index;
static Sint16 emycol_d_result;

void actionsub(sprite_status *pActwk);
void frameout(sprite_status *pActwk);
void patchg(sprite_status *patchgwk, Uint8 **pat_dat);
Sint32 actwkchk(sprite_status **ppActwk);
Sint16 emycol_d(sprite_status *pActwk);

#include "src/r8/hota8c.c"

void actionsub(sprite_status *pActwk) {
    ++actionsub_count;
    actionsub_actor = pActwk;
}

void frameout(sprite_status *pActwk) {
    ++frameout_count;
    frameout_actor = pActwk;
    pActwk->actno = 0;
}

void patchg(sprite_status *patchgwk, Uint8 **pat_dat) {
    ++patchg_count;
    patchg_actor = patchgwk;
    patchg_table = pat_dat;
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

Sint16 emycol_d(sprite_status *pActwk) {
    (void)pActwk;
    return emycol_d_result;
}

static void queue_actor(sprite_status *actor) {
    actwkchk_queue[actwkchk_queue_count++] = actor;
}

static void set_hota8c_word(sprite_status *actor, int offset, Sint16 value) {
    if (offset == 10)
        hota8c_work_get(actor)->linked_actor_index = value;
}

static void reset_logs(void) {
    actionsub_count = 0;
    actionsub_actor = 0;
    frameout_count = 0;
    frameout_actor = 0;
    patchg_count = 0;
    patchg_actor = 0;
    patchg_table = 0;
    actwkchk_count = 0;
    memset(actwkchk_queue, 0, sizeof(actwkchk_queue));
    actwkchk_queue_count = 0;
    actwkchk_queue_index = 0;
    emycol_d_result = 0;
}

static void reset_hota8c_state(void) {
    memset(actwk, 0, sizeof(actwk));
    memset(&gametimer, 0, sizeof(gametimer));
    shut_flag = 0;
    bossflag = 0;
    scra_vline = 0;
    memset(color_buffer, 0, sizeof(color_buffer));
    lpcolorwk = color_buffer;
    reset_logs();
}

static void assert_actionsub_called_for(test_context *ctx,
                                        sprite_status *actor) {
    TEST_ASSERT_EQ_INT(ctx, 1, actionsub_count);
    TEST_ASSERT_TRUE(ctx, actionsub_actor == actor);
}

static void assert_patchg_called_for(test_context *ctx, sprite_status *actor) {
    TEST_ASSERT_EQ_INT(ctx, 1, patchg_count);
    TEST_ASSERT_TRUE(ctx, patchg_actor == actor);
    TEST_ASSERT_TRUE(ctx, patchg_table == hotaru8_pchg);
}

static void assert_palette_eq(test_context *ctx, int index,
                              PALETTEENTRY expected) {
    TEST_ASSERT_EQ_INT(ctx, expected.peRed, color_buffer[index].peRed);
    TEST_ASSERT_EQ_INT(ctx, expected.peGreen, color_buffer[index].peGreen);
    TEST_ASSERT_EQ_INT(ctx, expected.peBlue, color_buffer[index].peBlue);
    TEST_ASSERT_EQ_INT(ctx, expected.peFlags, color_buffer[index].peFlags);
}

static void test_hota8c_patterns_capture_literal_data(test_context *ctx) {
    TEST_ASSERT_TRUE(ctx, hotaru8pat[0] == &hotaru00);
    TEST_ASSERT_TRUE(ctx, hotaru8pat[1] == &hotaru01);
    TEST_ASSERT_TRUE(ctx, hotaru8pat[15] == &hotaru0f);
    TEST_ASSERT_TRUE(ctx, hotaru8pat[16] == &hotaru10);
    TEST_ASSERT_EQ_INT(ctx, 2, hotaru00.cnt);
    TEST_ASSERT_EQ_INT(ctx, -12, hotaru00.spra[0].xoff);
    TEST_ASSERT_EQ_INT(ctx, -26, hotaru00.spra[0].yoff);
    TEST_ASSERT_EQ_INT(ctx, 293, hotaru00.spra[0].index);
    TEST_ASSERT_EQ_INT(ctx, 294, hotaru01.spra[0].index);
    TEST_ASSERT_EQ_INT(ctx, 301, hotaru0f.spra[0].index);
    TEST_ASSERT_EQ_INT(ctx, 8, hotaru10.spra[0].etc);
}

static void test_hota8c_normal_actor_initializes_and_counts_down(
    test_context *ctx) {
    sprite_status *actor = &actwk[4];

    reset_hota8c_state();
    actor->xposi.w.h = 3040;
    actor->yposi.w.h = 360;
    actor->userflag.b.h = -128;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 2, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 4, actor->actflg);
    TEST_ASSERT_EQ_INT(ctx, 1, actor->sprpri);
    TEST_ASSERT_TRUE(ctx, actor->patbase == hotaru8pat);
    TEST_ASSERT_EQ_INT(ctx, 8, actor->sprhsize);
    TEST_ASSERT_EQ_INT(ctx, 8, actor->sprvsize);
    TEST_ASSERT_EQ_INT(ctx, 60, hota8c_work_get(actor)->timer);
    TEST_ASSERT_EQ_INT(ctx, 0, actor->userflag.b.h);
    assert_actionsub_called_for(ctx, actor);

    reset_logs();
    hota8c_work_get(actor)->timer = 1;
    actor->userflag.b.h = 5;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 4, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, actor->mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 5, hota8c_work_get(actor)->phase_flags);
    assert_patchg_called_for(ctx, actor);
    assert_actionsub_called_for(ctx, actor);
}

static void test_hota8c_setup_uses_timer_or_player_grid_targets(
    test_context *ctx) {
    sprite_status *actor = &actwk[4];

    reset_hota8c_state();
    actor->r_no0 = 4;
    actor->xposi.w.h = 3000;
    actor->yposi.w.h = 300;
    actwk[0].xposi.w.h = 100;
    gametimer.w = 10;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 6, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, hota8c_work_get(actor)->phase_flags);
    TEST_ASSERT_EQ_INT(ctx, 32, hota8c_work_get(actor)->sub_timer);
    TEST_ASSERT_EQ_INT(ctx, 0, actor->colino);
    TEST_ASSERT_EQ_INT(ctx, 528, actor->xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 576, actor->yspeed.w);
    assert_actionsub_called_for(ctx, actor);

    reset_hota8c_state();
    actor->r_no0 = 4;
    hota8c_work_get(actor)->phase_flags = 1;
    actor->xposi.w.h = 3000;
    actor->yposi.w.h = 300;
    actwk[0].xposi.w.h = 3104;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 6, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 2, hota8c_work_get(actor)->phase_flags);
    TEST_ASSERT_EQ_INT(ctx, 832, actor->xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 352, actor->yspeed.w);
}

static void test_hota8c_countdown_and_setup_boundary_edges(test_context *ctx) {
    sprite_status *actor = &actwk[4];

    reset_hota8c_state();
    actor->r_no0 = 2;
    hota8c_work_get(actor)->timer = 2;
    actor->userflag.b.h = 5;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 2, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, hota8c_work_get(actor)->timer);
    TEST_ASSERT_EQ_INT(ctx, 4, actor->mstno.b.h);
    assert_patchg_called_for(ctx, actor);
    assert_actionsub_called_for(ctx, actor);

    reset_hota8c_state();
    actor->r_no0 = 4;
    hota8c_work_get(actor)->phase_flags = 1;
    actor->xposi.w.h = 3000;
    actor->yposi.w.h = 300;
    gametimer.w = 1;
    actwk[0].xposi.w.h = 3000;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 6, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 456, actor->xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 608, actor->yspeed.w);

    reset_hota8c_state();
    actor->r_no0 = 4;
    hota8c_work_get(actor)->phase_flags = 1;
    actor->xposi.w.h = 3060;
    actor->yposi.w.h = 360;
    gametimer.w = 127;
    actwk[0].xposi.w.h = 3232;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 6, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 984, actor->xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, -64, actor->yspeed.w);
}

static void test_hota8c_move_spawns_shadow_and_advances_exactly(
    test_context *ctx) {
    sprite_status *actor = &actwk[4];
    sprite_status *shadow = &actwk[20];

    reset_hota8c_state();
    actor->r_no0 = 6;
    actor->xposi.l = 100 << 16;
    actor->yposi.l = 200 << 16;
    actor->xspeed.w = 2;
    actor->yspeed.w = -3;
    hota8c_work_get(actor)->sub_timer = 7;
    queue_actor(shadow);

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, (100 << 16) + 512, actor->xposi.l);
    TEST_ASSERT_EQ_INT(ctx, (200 << 16) - 768, actor->yposi.l);
    TEST_ASSERT_EQ_INT(ctx, 6, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 6, hota8c_work_get(actor)->sub_timer);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 57, shadow->actno);
    TEST_ASSERT_EQ_INT(ctx, actor->xposi.w.h, shadow->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, actor->yposi.w.h, shadow->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, -3, shadow->userflag.b.h);
    assert_patchg_called_for(ctx, actor);
    assert_actionsub_called_for(ctx, actor);
}

static void test_hota8c_move_countdown_no_shadow_and_state_advance(
    test_context *ctx) {
    sprite_status *actor = &actwk[4];

    reset_hota8c_state();
    actor->r_no0 = 6;
    actor->xposi.l = 100 << 16;
    actor->yposi.l = 200 << 16;
    actor->xspeed.w = -2;
    actor->yspeed.w = 3;
    hota8c_work_get(actor)->sub_timer = 2;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, (100 << 16) - 512, actor->xposi.l);
    TEST_ASSERT_EQ_INT(ctx, (200 << 16) + 768, actor->yposi.l);
    TEST_ASSERT_EQ_INT(ctx, 6, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, hota8c_work_get(actor)->sub_timer);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);
    assert_patchg_called_for(ctx, actor);
    assert_actionsub_called_for(ctx, actor);

    reset_logs();

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 8, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, hota8c_work_get(actor)->sub_timer);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);
    assert_patchg_called_for(ctx, actor);
    assert_actionsub_called_for(ctx, actor);
}

static void test_hota8c_move_shadow_allocation_failure_is_ignored(
    test_context *ctx) {
    sprite_status *actor = &actwk[4];

    reset_hota8c_state();
    actor->r_no0 = 6;
    hota8c_work_get(actor)->sub_timer = 3;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 6, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 2, hota8c_work_get(actor)->sub_timer);
    assert_patchg_called_for(ctx, actor);
    assert_actionsub_called_for(ctx, actor);
}

static void test_hota8c_palette_and_attack_cycle(test_context *ctx) {
    sprite_status *actor = &actwk[4];
    PALETTEENTRY src0 = {11, 12, 13, 14};
    PALETTEENTRY src1 = {21, 22, 23, 24};
    PALETTEENTRY src2 = {31, 32, 33, 34};
    PALETTEENTRY src3 = {41, 42, 43, 44};
    PALETTEENTRY src4 = {51, 52, 53, 54};
    PALETTEENTRY src5 = {61, 62, 63, 64};

    reset_hota8c_state();
    clrtblC1[0] = src0;
    clrtblC1[1] = src1;
    clrtblC1[2] = src2;
    clrtblC1[3] = src3;
    clrtblC1[4] = src4;
    clrtblC1[5] = src5;

    colchg2(0);

    assert_palette_eq(ctx, 49, src0);
    assert_palette_eq(ctx, 50, src1);
    assert_palette_eq(ctx, 60, src2);
    assert_palette_eq(ctx, 61, src3);
    assert_palette_eq(ctx, 62, src4);
    assert_palette_eq(ctx, 63, src5);

    reset_logs();
    actor->r_no0 = 10;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 19, hota8c_work_get(actor)->sequence_index);
    TEST_ASSERT_EQ_INT(ctx, 2, actor->mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 11, actor->colino);
    TEST_ASSERT_EQ_INT(ctx, 64, hota8c_work_get(actor)->beam_accumulator);
    assert_patchg_called_for(ctx, actor);
    assert_actionsub_called_for(ctx, actor);

    reset_logs();
    actor->r_no0 = 12;
    hota8c_work_get(actor)->sequence_index = 1;
    actor->mstno.b.h = 7;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 4, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, actor->mstno.b.h);
    assert_patchg_called_for(ctx, actor);
}

static void test_hota8c_charge_flash_attack_and_recovery_completion(
    test_context *ctx) {
    sprite_status *actor = &actwk[4];
    sprite_status *beam = &actwk[20];
    PALETTEENTRY marker = {77, 78, 79, 80};

    reset_hota8c_state();
    clrtblC1[42] = marker;
    actor->r_no0 = 8;
    hota8c_work_get(actor)->sequence_index = 30;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 8, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 31, hota8c_work_get(actor)->sequence_index);
    assert_palette_eq(ctx, 49, marker);
    assert_patchg_called_for(ctx, actor);
    assert_actionsub_called_for(ctx, actor);

    reset_logs();

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 10, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, hota8c_work_get(actor)->sequence_index);
    assert_patchg_called_for(ctx, actor);
    assert_actionsub_called_for(ctx, actor);

    reset_logs();
    actor->r_no0 = 10;
    hota8c_work_get(actor)->sequence_index = 1;
    hota8c_work_get(actor)->beam_accumulator = 192;
    actor->xposi.w.h = 1234;
    actor->yposi.w.h = 456;
    queue_actor(beam);

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 12, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, hota8c_work_get(actor)->sequence_index);
    TEST_ASSERT_EQ_INT(ctx, 0, hota8c_work_get(actor)->beam_accumulator);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 57, beam->actno);
    TEST_ASSERT_EQ_INT(ctx, -1, beam->userflag.b.h);
    TEST_ASSERT_EQ_INT(ctx, 1234, beam->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 456, beam->yposi.w.h);
    assert_patchg_called_for(ctx, actor);
    assert_actionsub_called_for(ctx, actor);

    reset_logs();
    actor->r_no0 = 12;
    hota8c_work_get(actor)->sequence_index = 0;
    actor->mstno.b.h = 7;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 12, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 15, hota8c_work_get(actor)->sequence_index);
    TEST_ASSERT_EQ_INT(ctx, 0, actor->mstno.b.h);
    assert_patchg_called_for(ctx, actor);
    assert_actionsub_called_for(ctx, actor);

    reset_logs();
    hota8c_work_get(actor)->sequence_index = 1;
    actor->mstno.b.h = 7;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 4, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, hota8c_work_get(actor)->sequence_index);
    TEST_ASSERT_EQ_INT(ctx, 1, actor->mstno.b.h);
    assert_patchg_called_for(ctx, actor);
    assert_actionsub_called_for(ctx, actor);
}

static void test_hota8c_attack_beam_allocation_failure_still_finishes(
    test_context *ctx) {
    sprite_status *actor = &actwk[4];

    reset_hota8c_state();
    actor->r_no0 = 10;
    hota8c_work_get(actor)->sequence_index = 1;
    hota8c_work_get(actor)->beam_accumulator = 192;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 12, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, hota8c_work_get(actor)->sequence_index);
    assert_patchg_called_for(ctx, actor);
    assert_actionsub_called_for(ctx, actor);
}

static void test_hota8c_beam_and_shadow_special_actor_paths(test_context *ctx) {
    sprite_status *beam = &actwk[8];
    sprite_status *shadow = &actwk[9];

    reset_hota8c_state();
    beam->userflag.b.h = -1;

    hotaru8(beam);

    TEST_ASSERT_EQ_INT(ctx, 2, beam->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 3, beam->sprpri);
    TEST_ASSERT_EQ_INT(ctx, 16, beam->sprvsize);
    TEST_ASSERT_EQ_INT(ctx, 4, beam->sprhsize);
    TEST_ASSERT_EQ_INT(ctx, 152, beam->colino);
    assert_actionsub_called_for(ctx, beam);

    reset_logs();
    beam->r_no0 = 2;
    beam->yposi.w.h = 100;
    emycol_d_result = -1;

    hotaru8(beam);

    TEST_ASSERT_EQ_INT(ctx, 4, beam->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 108, beam->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, hota8c_work_get(beam)->timer);
    TEST_ASSERT_EQ_INT(ctx, 0, hota8c_work_get(beam)->sub_timer);
    assert_patchg_called_for(ctx, beam);

    reset_logs();
    beam->r_no0 = 4;
    hota8c_work_get(beam)->timer = 1;
    hota8c_work_get(beam)->sub_timer = 1;
    beam->actno = 57;

    hotaru8(beam);

    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actor == beam);
    TEST_ASSERT_EQ_INT(ctx, 0, beam->actno);
    assert_actionsub_called_for(ctx, beam);

    reset_logs();
    shadow->userflag.b.h = -3;

    hotaru8(shadow);

    TEST_ASSERT_EQ_INT(ctx, 2, shadow->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 4, shadow->sprpri);
    TEST_ASSERT_EQ_INT(ctx, 23, hota8c_work_get(shadow)->timer);
    assert_patchg_called_for(ctx, shadow);
    assert_actionsub_called_for(ctx, shadow);

    reset_logs();
    shadow->actno = 57;
    shadow->r_no0 = 2;
    hota8c_work_get(shadow)->timer = 1;

    hotaru8(shadow);

    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actor == shadow);
    TEST_ASSERT_EQ_INT(ctx, 0, actionsub_count);
}

static void test_hota8c_beam_and_shadow_edge_paths(test_context *ctx) {
    sprite_status *beam = &actwk[8];
    sprite_status *shadow = &actwk[9];

    reset_hota8c_state();
    beam->userflag.b.h = -1;
    beam->r_no0 = 2;
    beam->yposi.w.h = 100;
    emycol_d_result = 0;

    hotaru8(beam);

    TEST_ASSERT_EQ_INT(ctx, 2, beam->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 108, beam->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 8, beam->mstno.b.h);
    assert_patchg_called_for(ctx, beam);
    assert_actionsub_called_for(ctx, beam);

    reset_logs();
    beam->r_no0 = 4;
    hota8c_work_get(beam)->timer = 0;
    hota8c_work_get(beam)->sub_timer = 1;
    beam->mstno.b.h = 0;

    hotaru8(beam);

    TEST_ASSERT_EQ_INT(ctx, 4, beam->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 2, hota8c_work_get(beam)->timer);
    TEST_ASSERT_EQ_INT(ctx, 1, hota8c_work_get(beam)->sub_timer);
    TEST_ASSERT_EQ_INT(ctx, 9, beam->mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 0, patchg_count);
    assert_actionsub_called_for(ctx, beam);

    reset_logs();
    hota8c_work_get(beam)->timer = 2;
    hota8c_work_get(beam)->sub_timer = 0;

    hotaru8(beam);

    TEST_ASSERT_EQ_INT(ctx, 4, beam->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, hota8c_work_get(beam)->timer);
    TEST_ASSERT_EQ_INT(ctx, 0, frameout_count);
    assert_patchg_called_for(ctx, beam);
    assert_actionsub_called_for(ctx, beam);

    reset_logs();
    shadow->userflag.b.h = -3;
    shadow->r_no0 = 2;
    hota8c_work_get(shadow)->timer = 2;

    hotaru8(shadow);

    TEST_ASSERT_EQ_INT(ctx, 1, hota8c_work_get(shadow)->timer);
    assert_patchg_called_for(ctx, shadow);
    assert_actionsub_called_for(ctx, shadow);
    TEST_ASSERT_EQ_INT(ctx, 0, frameout_count);
}

static void test_hota8c_room_controller_visible_paths(test_context *ctx) {
    sprite_status *room = &actwk[6];
    sprite_status *door = &actwk[30];
    sprite_status *hotaru = &actwk[31];
    sprite_status *hotaru_left = &actwk[32];
    sprite_status *hotaru_right = &actwk[33];
    sprite_status *eggman = &actwk[34];

    reset_hota8c_state();
    room->userflag.b.h = -2;
    actwk[0].xposi.w.h = 3023;

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 0, room->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, shut_flag);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);

    reset_hota8c_state();
    room->userflag.b.h = -2;
    actwk[0].xposi.w.h = 3024;
    queue_actor(door);

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 1, shut_flag);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 41, door->actno);
    TEST_ASSERT_EQ_INT(ctx, 3240, door->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 464, door->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 2, room->r_no0);

    reset_logs();
    scra_vline = 204;
    actwk[0].xposi.w.h = 3024;

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 8, bossflag);
    TEST_ASSERT_EQ_INT(ctx, 210, scra_vline);
    TEST_ASSERT_EQ_INT(ctx, 4, room->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, hota8c_work_get(room)->timer);
    TEST_ASSERT_EQ_INT(ctx, 0, hota8c_work_get(room)->sub_timer);

    reset_logs();
    room->r_no0 = 4;
    hota8c_work_get(room)->timer = 16;
    queue_actor(hotaru);

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 57, hotaru->actno);
    TEST_ASSERT_EQ_INT(ctx, 3040, hotaru->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 400, hotaru->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 60, hota8c_work_get(room)->timer);
    TEST_ASSERT_EQ_INT(ctx, 6, room->r_no0);

    reset_logs();

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 6, room->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 60, hota8c_work_get(room)->timer);

    reset_logs();
    hotaru->actno = 0;
    hota8c_work_get(room)->timer = 1;
    queue_actor(hotaru_left);
    queue_actor(hotaru_right);

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 2, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 57, hotaru_left->actno);
    TEST_ASSERT_EQ_INT(ctx, 3040, hotaru_left->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 392, hotaru_left->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, hotaru_left->userflag.b.h);
    TEST_ASSERT_EQ_INT(ctx, 57, hotaru_right->actno);
    TEST_ASSERT_EQ_INT(ctx, 3200, hotaru_right->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 392, hotaru_right->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, hotaru_right->userflag.b.h);
    TEST_ASSERT_EQ_INT(ctx, 10, room->r_no0);

    reset_logs();

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 10, room->r_no0);

    hotaru_left->actno = 0;
    hotaru_right->actno = 0;

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 12, room->r_no0);

    reset_logs();
    hota8c_work_get(room)->timer = 16;
    queue_actor(eggman);

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 255, tobira8_get_work(door)->trigger_open);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 63, eggman->actno);
    TEST_ASSERT_EQ_INT(ctx, 3840, eggman->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 316, eggman->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actor == room);
    TEST_ASSERT_EQ_INT(ctx, 0, room->actno);
}

static void test_hota8c_room_controller_allocation_and_wait_edges(
    test_context *ctx) {
    sprite_status *room = &actwk[6];
    sprite_status *door = &actwk[30];
    sprite_status *hotaru = &actwk[31];
    PALETTEENTRY src = {91, 92, 93, 94};

    reset_hota8c_state();
    room->userflag.b.h = -2;
    actwk[0].xposi.w.h = 3024;

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 1, shut_flag);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 0, room->r_no0);

    reset_hota8c_state();
    room->userflag.b.h = -2;
    room->r_no0 = 2;
    actwk[0].xposi.w.h = 3023;
    scra_vline = 100;

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 2, room->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 100, scra_vline);
    TEST_ASSERT_EQ_INT(ctx, 0, bossflag);

    actwk[0].xposi.w.h = 3024;
    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 2, room->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 106, scra_vline);
    TEST_ASSERT_EQ_INT(ctx, 0, bossflag);

    reset_hota8c_state();
    room->userflag.b.h = -2;
    room->r_no0 = 4;
    hota8c_work_get(room)->timer = 0;
    hota8c_work_get(room)->sub_timer = 1;
    clrtblC1[42] = src;

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 4, room->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, hota8c_work_get(room)->timer);
    TEST_ASSERT_EQ_INT(ctx, 0, hota8c_work_get(room)->sub_timer);

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 2, hota8c_work_get(room)->timer);
    TEST_ASSERT_EQ_INT(ctx, 64, hota8c_work_get(room)->sub_timer);
    assert_palette_eq(ctx, 49, src);

    reset_hota8c_state();
    room->userflag.b.h = -2;
    room->r_no0 = 4;
    hota8c_work_get(room)->timer = 16;

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 4, room->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, hota8c_work_get(room)->timer);
    TEST_ASSERT_EQ_INT(ctx, 0, hota8c_work_get(room)->sub_timer);

    reset_hota8c_state();
    room->userflag.b.h = -2;
    room->r_no0 = 6;
    hota8c_work_get(room)->timer = 1;
    hotaru->actno = 0;

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 0, hota8c_work_get(room)->timer);
    TEST_ASSERT_EQ_INT(ctx, 6, room->r_no0);

    reset_hota8c_state();
    room->userflag.b.h = -2;
    room->r_no0 = 8;

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 8, room->r_no0);

    reset_hota8c_state();
    room->userflag.b.h = -2;
    room->r_no0 = 12;
    hota8c_work_get(room)->timer = 16;
    tobira8_get_work(&actwk[0])->trigger_open = 0;

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 255, tobira8_get_work(&actwk[0])->trigger_open);
    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 0, frameout_count);
    TEST_ASSERT_EQ_INT(ctx, 0, room->actno);
}

static void test_hota8c_remaining_room_controller_wait_edges(
    test_context *ctx) {
    sprite_status *room = &actwk[6];

    reset_hota8c_state();
    room->userflag.b.h = -2;
    room->r_no0 = 6;
    hota8c_work_get(room)->timer = 2;
    set_hota8c_word(room, 10, 31);
    actwk[31].actno = 0;

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 6, room->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, hota8c_work_get(room)->timer);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);

    reset_hota8c_state();
    room->userflag.b.h = -2;
    room->r_no0 = 10;
    set_hota8c_word(room, 10, 31);
    actwk[31].actno = 0;
    actwk[0].actno = 57;

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 10, room->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);

    reset_hota8c_state();
    room->userflag.b.h = -2;
    room->r_no0 = 12;
    hota8c_work_get(room)->timer = 0;
    hota8c_work_get(room)->sub_timer = 0;

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 12, room->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 2, hota8c_work_get(room)->timer);
    TEST_ASSERT_EQ_INT(ctx, 64, hota8c_work_get(room)->sub_timer);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);
}

TEST_MAIN_BEGIN;
test_hota8c_patterns_capture_literal_data(&ctx);
test_hota8c_normal_actor_initializes_and_counts_down(&ctx);
test_hota8c_setup_uses_timer_or_player_grid_targets(&ctx);
test_hota8c_countdown_and_setup_boundary_edges(&ctx);
test_hota8c_move_spawns_shadow_and_advances_exactly(&ctx);
test_hota8c_move_countdown_no_shadow_and_state_advance(&ctx);
test_hota8c_move_shadow_allocation_failure_is_ignored(&ctx);
test_hota8c_palette_and_attack_cycle(&ctx);
test_hota8c_charge_flash_attack_and_recovery_completion(&ctx);
test_hota8c_attack_beam_allocation_failure_still_finishes(&ctx);
test_hota8c_beam_and_shadow_special_actor_paths(&ctx);
test_hota8c_beam_and_shadow_edge_paths(&ctx);
test_hota8c_room_controller_visible_paths(&ctx);
test_hota8c_room_controller_allocation_and_wait_edges(&ctx);
test_hota8c_remaining_room_controller_wait_edges(&ctx);
TEST_MAIN_END
