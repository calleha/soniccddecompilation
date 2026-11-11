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

#include "src/r8/hota8d.c"

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

static void set_hota8d_word(sprite_status *actor, int offset, Sint16 value) {
    if (offset == 10)
        hota8d_work_get(actor)->linked_actor_index = value;
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

static void reset_hota8d_state(void) {
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

static void test_hota8d_patterns_capture_literal_data(test_context *ctx) {
    TEST_ASSERT_TRUE(ctx, hotaru8pat[0] == &hotaru00);
    TEST_ASSERT_TRUE(ctx, hotaru8pat[9] == &hotaru09);
    TEST_ASSERT_TRUE(ctx, hotaru8pat[15] == &hotaru0f);
    TEST_ASSERT_TRUE(ctx, hotaru8pat[16] == &hotaru10);
    TEST_ASSERT_EQ_INT(ctx, 293, hotaru00.spra[0].index);
    TEST_ASSERT_EQ_INT(ctx, 301, hotaru09.spra[0].index);
    TEST_ASSERT_EQ_INT(ctx, 301, hotaru0f.spra[0].index);
    TEST_ASSERT_EQ_INT(ctx, 8, hotaru10.spra[0].etc);
}

static void test_hota8d_initial_setup_and_movement_paths(test_context *ctx) {
    sprite_status *actor = &actwk[4];
    sprite_status *shadow = &actwk[20];

    reset_hota8d_state();
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
    TEST_ASSERT_EQ_INT(ctx, 60, hota8d_work_get(actor)->timer);
    TEST_ASSERT_EQ_INT(ctx, 0, actor->userflag.b.h);
    assert_actionsub_called_for(ctx, actor);

    reset_logs();
    actor->r_no0 = 4;
    hota8d_work_get(actor)->phase_flags = 1;
    actor->xposi.w.h = 3000;
    actor->yposi.w.h = 300;
    actwk[0].xposi.w.h = 3104;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 6, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 2, hota8d_work_get(actor)->phase_flags);
    TEST_ASSERT_EQ_INT(ctx, 832, actor->xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 352, actor->yspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 32, hota8d_work_get(actor)->sub_timer);

    reset_logs();
    actor->xposi.l = 100 << 16;
    actor->yposi.l = 200 << 16;
    actor->xspeed.w = 2;
    actor->yspeed.w = -3;
    hota8d_work_get(actor)->sub_timer = 7;
    queue_actor(shadow);

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, (100 << 16) + 512, actor->xposi.l);
    TEST_ASSERT_EQ_INT(ctx, (200 << 16) - 768, actor->yposi.l);
    TEST_ASSERT_EQ_INT(ctx, 57, shadow->actno);
    TEST_ASSERT_EQ_INT(ctx, -3, shadow->userflag.b.h);
    assert_patchg_called_for(ctx, actor);
    assert_actionsub_called_for(ctx, actor);
}

static void test_hota8d_spin_setup_and_idle_edge_paths(test_context *ctx) {
    sprite_status *actor = &actwk[4];

    reset_hota8d_state();
    actor->r_no0 = 2;
    hota8d_work_get(actor)->timer = 1;
    actor->userflag.b.h = 5;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 4, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, actor->mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 5, hota8d_work_get(actor)->phase_flags);
    assert_patchg_called_for(ctx, actor);
    assert_actionsub_called_for(ctx, actor);

    reset_logs();
    actor->r_no0 = 4;
    hota8d_work_get(actor)->phase_flags = 0;
    actor->xposi.w.h = 3066;
    actor->yposi.w.h = 357;
    gametimer.w = 10;
    actwk[0].xposi.w.h = 1;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 6, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, hota8d_work_get(actor)->phase_flags);
    TEST_ASSERT_EQ_INT(ctx, 0, actor->xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 0, actor->yspeed.w);

    reset_logs();
    actor->r_no0 = 6;
    hota8d_work_get(actor)->sub_timer = 1;
    actor->xspeed.w = 1;
    actor->yspeed.w = 1;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 8, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);
    assert_patchg_called_for(ctx, actor);
}

static void test_hota8d_zanzou_allocation_failure_and_attack_cycle(
    test_context *ctx) {
    sprite_status *actor = &actwk[4];
    sprite_status *beam = &actwk[20];

    reset_hota8d_state();
    actor->r_no0 = 6;
    hota8d_work_get(actor)->sub_timer = 3;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[20].actno);
    assert_patchg_called_for(ctx, actor);

    reset_logs();
    actor->r_no0 = 8;
    hota8d_work_get(actor)->sequence_index = 31;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 10, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, hota8d_work_get(actor)->sequence_index);

    reset_logs();
    actor->r_no0 = 10;
    hota8d_work_get(actor)->sequence_index = 1;
    hota8d_work_get(actor)->beam_accumulator = 255;
    actor->xposi.w.h = 111;
    actor->yposi.w.h = 222;
    queue_actor(beam);

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 12, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 57, beam->actno);
    TEST_ASSERT_EQ_INT(ctx, -1, beam->userflag.b.h);
    TEST_ASSERT_EQ_INT(ctx, 111, beam->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 222, beam->yposi.w.h);

    reset_logs();
    actor->r_no0 = 12;
    hota8d_work_get(actor)->sequence_index = 1;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 4, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, actor->mstno.b.h);
    assert_patchg_called_for(ctx, actor);
}

static void test_hota8d_palette_cycle_uses_d_table_and_slots_52_to_63(
    test_context *ctx) {
    PALETTEENTRY samples[12] = {
        {1, 2, 3, 4},     {5, 6, 7, 8},     {9, 10, 11, 12},
        {13, 14, 15, 16}, {17, 18, 19, 20}, {21, 22, 23, 24},
        {25, 26, 27, 28}, {29, 30, 31, 32}, {33, 34, 35, 36},
        {37, 38, 39, 40}, {41, 42, 43, 44}, {45, 46, 47, 48}};

    reset_hota8d_state();
    for (int i = 0; i < 12; ++i) {
        clrtblD[i] = samples[i];
    }

    colchg2(0);

    for (int i = 0; i < 12; ++i) {
        assert_palette_eq(ctx, 52 + i, samples[i]);
    }
}

static void test_hota8d_attack_beam_and_shadow_paths(test_context *ctx) {
    sprite_status *actor = &actwk[4];
    sprite_status *beam = &actwk[8];
    sprite_status *shadow = &actwk[9];

    reset_hota8d_state();
    actor->r_no0 = 10;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 19, hota8d_work_get(actor)->sequence_index);
    TEST_ASSERT_EQ_INT(ctx, 2, actor->mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 11, actor->colino);
    TEST_ASSERT_EQ_INT(ctx, 64, hota8d_work_get(actor)->beam_accumulator);
    assert_patchg_called_for(ctx, actor);
    assert_actionsub_called_for(ctx, actor);

    reset_logs();
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
    TEST_ASSERT_EQ_INT(ctx, 0, hota8d_work_get(beam)->timer);
    TEST_ASSERT_EQ_INT(ctx, 0, hota8d_work_get(beam)->sub_timer);
    assert_patchg_called_for(ctx, beam);

    reset_logs();
    shadow->userflag.b.h = -3;

    hotaru8(shadow);

    TEST_ASSERT_EQ_INT(ctx, 2, shadow->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 4, shadow->sprpri);
    TEST_ASSERT_EQ_INT(ctx, 23, hota8d_work_get(shadow)->timer);
    assert_patchg_called_for(ctx, shadow);
    assert_actionsub_called_for(ctx, shadow);
}

static void test_hota8d_beam_and_shadow_completion_paths(test_context *ctx) {
    sprite_status *beam = &actwk[8];
    sprite_status *shadow = &actwk[9];

    reset_hota8d_state();
    beam->userflag.b.h = -1;
    beam->r_no0 = 4;
    hota8d_work_get(beam)->timer = 0;
    hota8d_work_get(beam)->sub_timer = 1;

    hotaru8(beam);

    TEST_ASSERT_EQ_INT(ctx, 2, hota8d_work_get(beam)->timer);
    TEST_ASSERT_EQ_INT(ctx, 9, beam->mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 0, patchg_count);
    assert_actionsub_called_for(ctx, beam);

    reset_logs();
    hota8d_work_get(beam)->timer = 1;
    hota8d_work_get(beam)->sub_timer = 1;

    hotaru8(beam);

    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actor == beam);
    TEST_ASSERT_EQ_INT(ctx, 0, beam->actno);

    reset_logs();
    shadow->userflag.b.h = -3;
    shadow->r_no0 = 2;
    hota8d_work_get(shadow)->timer = 1;

    hotaru8(shadow);

    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actor == shadow);
    TEST_ASSERT_EQ_INT(ctx, 0, actionsub_count);
}

static void test_hota8d_room_controller_visible_paths(test_context *ctx) {
    sprite_status *room = &actwk[6];
    sprite_status *door = &actwk[30];
    sprite_status *hotaru = &actwk[31];
    sprite_status *hotaru_left = &actwk[32];
    sprite_status *hotaru_right = &actwk[33];
    sprite_status *eggman = &actwk[34];

    reset_hota8d_state();
    room->userflag.b.h = -2;
    actwk[0].xposi.w.h = 3024;
    queue_actor(door);

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 1, shut_flag);
    TEST_ASSERT_EQ_INT(ctx, 41, door->actno);
    TEST_ASSERT_EQ_INT(ctx, 3240, door->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 464, door->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 2, room->r_no0);

    scra_vline = 204;

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 8, bossflag);
    TEST_ASSERT_EQ_INT(ctx, 210, scra_vline);
    TEST_ASSERT_EQ_INT(ctx, 4, room->r_no0);

    reset_logs();
    room->r_no0 = 4;
    hota8d_work_get(room)->timer = 16;
    queue_actor(hotaru);

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 57, hotaru->actno);
    TEST_ASSERT_EQ_INT(ctx, 3040, hotaru->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 400, hotaru->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 60, hota8d_work_get(room)->timer);
    TEST_ASSERT_EQ_INT(ctx, 6, room->r_no0);

    hotaru->actno = 0;
    hota8d_work_get(room)->timer = 1;
    reset_logs();
    queue_actor(hotaru_left);
    queue_actor(hotaru_right);

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 2, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 57, hotaru_left->actno);
    TEST_ASSERT_EQ_INT(ctx, 3040, hotaru_left->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, hotaru_left->userflag.b.h);
    TEST_ASSERT_EQ_INT(ctx, 57, hotaru_right->actno);
    TEST_ASSERT_EQ_INT(ctx, 3200, hotaru_right->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, hotaru_right->userflag.b.h);
    TEST_ASSERT_EQ_INT(ctx, 10, room->r_no0);

    hotaru_left->actno = 0;
    hotaru_right->actno = 0;

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 12, room->r_no0);

    reset_logs();
    queue_actor(eggman);

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 255, tobira8_get_work(door)->trigger_open);
    TEST_ASSERT_EQ_INT(ctx, 63, eggman->actno);
    TEST_ASSERT_EQ_INT(ctx, 3840, eggman->xposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 316, eggman->yposi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 1, frameout_count);
    TEST_ASSERT_TRUE(ctx, frameout_actor == room);
    TEST_ASSERT_EQ_INT(ctx, 0, room->actno);
}

static void test_hota8d_room_controller_wait_and_failure_paths(
    test_context *ctx) {
    sprite_status *room = &actwk[6];
    sprite_status *door = &actwk[30];
    sprite_status *hotaru = &actwk[31];
    sprite_status *hotaru_left = &actwk[32];
    sprite_status *hotaru_right = &actwk[33];

    reset_hota8d_state();
    room->userflag.b.h = -2;
    actwk[0].xposi.w.h = 3000;

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 0, shut_flag);
    TEST_ASSERT_EQ_INT(ctx, 0, room->r_no0);

    actwk[0].xposi.w.h = 3024;
    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 1, shut_flag);
    TEST_ASSERT_EQ_INT(ctx, 0, room->r_no0);

    reset_logs();
    queue_actor(door);
    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 2, room->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 41, door->actno);

    actwk[0].xposi.w.h = 3000;
    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 2, room->r_no0);

    actwk[0].xposi.w.h = 3024;
    scra_vline = 100;
    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 2, room->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 106, scra_vline);

    room->r_no0 = 4;
    hota8d_work_get(room)->timer = 0;
    hota8d_work_get(room)->sub_timer = 2;
    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 1, hota8d_work_get(room)->sub_timer);
    TEST_ASSERT_EQ_INT(ctx, 4, room->r_no0);

    hota8d_work_get(room)->timer = 16;
    hota8d_work_get(room)->sub_timer = 7;
    reset_logs();
    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 0, hota8d_work_get(room)->timer);
    TEST_ASSERT_EQ_INT(ctx, 0, hota8d_work_get(room)->sub_timer);
    TEST_ASSERT_EQ_INT(ctx, 4, room->r_no0);

    reset_logs();
    hota8d_work_get(room)->timer = 16;
    hota8d_work_get(room)->sub_timer = 0;
    queue_actor(hotaru);
    hotaru8(room);
    TEST_ASSERT_EQ_INT(ctx, 6, room->r_no0);

    hotaru8(room);
    TEST_ASSERT_EQ_INT(ctx, 6, room->r_no0);

    hotaru->actno = 0;
    hota8d_work_get(room)->timer = 1;
    reset_logs();
    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 6, room->r_no0);

    reset_logs();
    hota8d_work_get(room)->timer = 1;
    queue_actor(hotaru_left);
    queue_actor(hotaru_right);
    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 10, room->r_no0);
    hotaru_left->actno = 57;
    hotaru_right->actno = 0;
    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 10, room->r_no0);

    hotaru_left->actno = 0;
    hotaru_right->actno = 57;
    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 10, room->r_no0);

    hotaru_right->actno = 0;
    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 12, room->r_no0);

    reset_logs();
    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 0, frameout_count);
    TEST_ASSERT_EQ_INT(ctx, 255, tobira8_get_work(door)->trigger_open);
}

static void test_hota8d_remaining_countdown_and_setup_edges(test_context *ctx) {
    sprite_status *actor = &actwk[4];

    reset_hota8d_state();
    actor->r_no0 = 2;
    hota8d_work_get(actor)->timer = 2;
    actor->userflag.b.h = 5;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 2, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, hota8d_work_get(actor)->timer);
    TEST_ASSERT_EQ_INT(ctx, 4, actor->mstno.b.h);
    TEST_ASSERT_EQ_INT(ctx, 5, actor->userflag.b.h);
    assert_patchg_called_for(ctx, actor);
    assert_actionsub_called_for(ctx, actor);

    reset_hota8d_state();
    actor->r_no0 = 4;
    hota8d_work_get(actor)->phase_flags = 1;
    actor->xposi.w.h = 3000;
    actor->yposi.w.h = 300;
    gametimer.w = 1;
    actwk[0].xposi.w.h = 3000;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 6, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 2, hota8d_work_get(actor)->phase_flags);
    TEST_ASSERT_EQ_INT(ctx, 456, actor->xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 608, actor->yspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 32, hota8d_work_get(actor)->sub_timer);

    reset_hota8d_state();
    actor->r_no0 = 4;
    hota8d_work_get(actor)->phase_flags = 1;
    actor->xposi.w.h = 3060;
    actor->yposi.w.h = 360;
    gametimer.w = 127;
    actwk[0].xposi.w.h = 3232;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 6, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 2, hota8d_work_get(actor)->phase_flags);
    TEST_ASSERT_EQ_INT(ctx, 984, actor->xspeed.w);
    TEST_ASSERT_EQ_INT(ctx, -64, actor->yspeed.w);
    TEST_ASSERT_EQ_INT(ctx, 32, hota8d_work_get(actor)->sub_timer);
}

static void test_hota8d_remaining_attack_and_beam_edges(test_context *ctx) {
    PALETTEENTRY samples[12] = {
        {2, 3, 4, 5},     {6, 7, 8, 9},     {10, 11, 12, 13},
        {14, 15, 16, 17}, {18, 19, 20, 21}, {22, 23, 24, 25},
        {26, 27, 28, 29}, {30, 31, 32, 33}, {34, 35, 36, 37},
        {38, 39, 40, 41}, {42, 43, 44, 45}, {46, 47, 48, 49}};
    sprite_status *actor = &actwk[4];
    sprite_status *beam = &actwk[8];

    reset_hota8d_state();
    for (int i = 0; i < 12; ++i) {
        clrtblD[i] = samples[i];
    }
    actor->r_no0 = 8;
    hota8d_work_get(actor)->sequence_index = 0;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 8, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, hota8d_work_get(actor)->sequence_index);
    assert_patchg_called_for(ctx, actor);
    assert_actionsub_called_for(ctx, actor);
    for (int i = 0; i < 12; ++i) {
        assert_palette_eq(ctx, 52 + i, samples[i]);
    }

    reset_hota8d_state();
    actor->r_no0 = 10;
    hota8d_work_get(actor)->sequence_index = 1;
    hota8d_work_get(actor)->beam_accumulator = 192;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 12, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 0, hota8d_work_get(actor)->sequence_index);
    assert_patchg_called_for(ctx, actor);
    assert_actionsub_called_for(ctx, actor);

    reset_hota8d_state();
    actor->r_no0 = 12;
    hota8d_work_get(actor)->sequence_index = 0;
    actor->mstno.b.h = 7;

    hotaru8(actor);

    TEST_ASSERT_EQ_INT(ctx, 12, actor->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 15, hota8d_work_get(actor)->sequence_index);
    TEST_ASSERT_EQ_INT(ctx, 0, actor->mstno.b.h);
    assert_patchg_called_for(ctx, actor);
    assert_actionsub_called_for(ctx, actor);

    reset_hota8d_state();
    beam->userflag.b.h = -1;
    beam->r_no0 = 4;
    hota8d_work_get(beam)->timer = 2;
    hota8d_work_get(beam)->sub_timer = 0;

    hotaru8(beam);

    TEST_ASSERT_EQ_INT(ctx, 1, hota8d_work_get(beam)->timer);
    TEST_ASSERT_EQ_INT(ctx, 0, hota8d_work_get(beam)->sub_timer);
    TEST_ASSERT_EQ_INT(ctx, 0, frameout_count);
    assert_patchg_called_for(ctx, beam);
    assert_actionsub_called_for(ctx, beam);
}

static void test_hota8d_remaining_room_controller_edges(test_context *ctx) {
    PALETTEENTRY samples[12] = {
        {50, 51, 52, 53}, {54, 55, 56, 57}, {58, 59, 60, 61},
        {62, 63, 64, 65}, {66, 67, 68, 69}, {70, 71, 72, 73},
        {74, 75, 76, 77}, {78, 79, 80, 81}, {82, 83, 84, 85},
        {86, 87, 88, 89}, {90, 91, 92, 93}, {94, 95, 96, 97}};
    sprite_status *room = &actwk[6];

    reset_hota8d_state();
    for (int i = 0; i < 12; ++i) {
        clrtblD[84 + i] = samples[i];
    }
    room->userflag.b.h = -2;
    room->r_no0 = 4;
    hota8d_work_get(room)->timer = 0;
    hota8d_work_get(room)->sub_timer = 0;

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 4, room->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 2, hota8d_work_get(room)->timer);
    TEST_ASSERT_EQ_INT(ctx, 64, hota8d_work_get(room)->sub_timer);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);
    for (int i = 0; i < 12; ++i) {
        assert_palette_eq(ctx, 52 + i, samples[i]);
    }

    reset_hota8d_state();
    room->userflag.b.h = -2;
    room->r_no0 = 6;
    hota8d_work_get(room)->timer = 2;
    set_hota8d_word(room, 10, 31);
    actwk[31].actno = 0;

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 6, room->r_no0);
    TEST_ASSERT_EQ_INT(ctx, 1, hota8d_work_get(room)->timer);
    TEST_ASSERT_EQ_INT(ctx, 0, actwkchk_count);

    reset_hota8d_state();
    room->userflag.b.h = -2;
    room->r_no0 = 8;

    hotaru8(room);

    TEST_ASSERT_EQ_INT(ctx, 1, actwkchk_count);
    TEST_ASSERT_EQ_INT(ctx, 8, room->r_no0);
}

TEST_MAIN_BEGIN;
test_hota8d_patterns_capture_literal_data(&ctx);
test_hota8d_initial_setup_and_movement_paths(&ctx);
test_hota8d_spin_setup_and_idle_edge_paths(&ctx);
test_hota8d_zanzou_allocation_failure_and_attack_cycle(&ctx);
test_hota8d_palette_cycle_uses_d_table_and_slots_52_to_63(&ctx);
test_hota8d_attack_beam_and_shadow_paths(&ctx);
test_hota8d_beam_and_shadow_completion_paths(&ctx);
test_hota8d_room_controller_visible_paths(&ctx);
test_hota8d_room_controller_wait_and_failure_paths(&ctx);
test_hota8d_remaining_countdown_and_setup_edges(&ctx);
test_hota8d_remaining_attack_and_beam_edges(&ctx);
test_hota8d_remaining_room_controller_edges(&ctx);
TEST_MAIN_END
