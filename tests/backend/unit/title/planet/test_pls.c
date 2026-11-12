/* <stdlib.h> declares POSIX random(); the decompiled title/planet headers
   declare the game's own Sint32 random(void).  Parse stdlib first, then rename
   the game symbol inside this harness so the two can coexist. */
#include <stdlib.h>

#define random game_random

#include <string.h>

#include "support/test_runner.h"
#include "types.h"

#include "src/title/planet/pls.c"

static pad_status pad_state;
static Uint32 last_pad_port;

static pad_status *stub_peri_pad_get(Uint32 port) {
    last_pad_port = port;
    return &pad_state;
}

pad_status *(*sPeriPadGet)(Uint32) = stub_peri_pad_get;
zoom_info_lpl zoomwk;
rotate_2 playwk;
rotate_1 k_wk;
Uint8 lp_cgflg;
Uint8 push_flg_s;
int_union rotspeed;
Uint16 rotflg;
Sint16 zoomflg;
Uint8 lpcg_chg;
Uint8 int1_flg;
Uint8 comflag_m;
Uint8 comflag_s;
ushort_union swdata1;

static void reset_state(void) {
    memset(&pad_state, 0, sizeof(pad_state));
    last_pad_port = 0;
    comflag_m = 0;
    comflag_s = 0;
    swdata1.w = 0;
    int1_flg = 0;
    lpcg_chg = 0;
    zoomflg = 0;
    rotflg = 0;
    rotspeed.l = 0;
    push_flg_s = 0;
    lp_cgflg = 0;
    memset(&k_wk, 0, sizeof(k_wk));
    memset(&playwk, 0, sizeof(playwk));
    memset(&zoomwk, 0, sizeof(zoomwk));
    memset(keyBuf, 0, sizeof(keyBuf));
}

static void test_player_dispatch_and_play0_initializes_camera_state(
    test_context *ctx) {
    reset_state();

    player();

    TEST_ASSERT_EQ_INT(ctx, 1, playwk.ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, -40, playwk.x_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 24, playwk.y_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 64, playwk.z_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, playwk.k_kaku);
    TEST_ASSERT_EQ_INT(ctx, 2048, zoomwk.xscale);
    TEST_ASSERT_EQ_INT(ctx, 1024, zoomwk.yscale);
    TEST_ASSERT_EQ_INT(ctx, 1, rotflg);
    TEST_ASSERT_EQ_INT(ctx, 1, zoomflg);
    TEST_ASSERT_EQ_INT(ctx, 2, rotspeed.w.h);
}

static void test_play1_start_button_sets_sub_command_and_resets(
    test_context *ctx) {
    reset_state();
    playwk.ACT_NO = 1;
    pad_state.On = 6;
    swdata1.b.h = 16;

    play1();

    TEST_ASSERT_EQ_INT(ctx, 1, last_pad_port);
    TEST_ASSERT_EQ_INT(ctx, 1, keyBuf[67]);
    TEST_ASSERT_EQ_INT(ctx, 1, keyBuf[88]);
    TEST_ASSERT_EQ_INT(ctx, 64, comflag_s);
    TEST_ASSERT_EQ_INT(ctx, 0, playwk.ACT_NO);
}

static void test_play1_movement_buttons_update_position_and_angle(
    test_context *ctx) {
    reset_state();
    play0();
    playwk.x_posi.w.h = 0;
    playwk.y_posi.w.h = 0;
    playwk.z_posi.w.h = 640;
    swdata1.b.h = 5;

    play1();

    TEST_ASSERT_EQ_INT(ctx, 8, playwk.x_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 8, playwk.y_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 2, playwk.k_kaku);
    TEST_ASSERT_EQ_INT(ctx, 1, playwk.ACT_NO);

    swdata1.b.h = 10;
    play1();
    TEST_ASSERT_EQ_INT(ctx, 0, playwk.x_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 0, playwk.y_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 4, playwk.k_kaku);
}

static void test_play1_clamps_positions_to_z_extent(test_context *ctx) {
    reset_state();
    playwk.ACT_NO = 1;
    playwk.z_posi.w.h = 64;
    rotspeed.w.h = 7;

    playwk.x_posi.w.h = 1000;
    playwk.y_posi.w.h = 1000;
    play1();
    TEST_ASSERT_EQ_INT(ctx, 400, playwk.x_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 400, playwk.y_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 7, playwk.k_kaku);

    playwk.x_posi.w.h = -1000;
    playwk.y_posi.w.h = -1000;
    play1();
    TEST_ASSERT_EQ_INT(ctx, -400, playwk.x_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, -400, playwk.y_posi.w.h);
    TEST_ASSERT_EQ_INT(ctx, 14, playwk.k_kaku);
}

static void test_play1_enters_play2_when_main_command_flag_is_set(
    test_context *ctx) {
    reset_state();
    playwk.ACT_NO = 1;
    comflag_m = 16;
    rotspeed.w.h = 1;

    play1();

    TEST_ASSERT_EQ_INT(ctx, 2, playwk.ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, 1, playwk.k_kaku);
}

static void test_check_b_button_toggle_repeat_and_release_paths(
    test_context *ctx) {
    reset_state();
    rotflg = 1;
    rotspeed.l = 0x00020000;
    keyBuf[88] = 1;

    check_Bbtn();
    TEST_ASSERT_EQ_INT(ctx, 0, rotspeed.l);
    TEST_ASSERT_EQ_INT(ctx, 65535, rotflg);
    TEST_ASSERT_TRUE(ctx, (push_flg_s & 16) != 0);

    check_Bbtn();
    TEST_ASSERT_EQ_INT(ctx, 0, rotspeed.l);

    keyBuf[88] = 0;
    check_Bbtn();
    TEST_ASSERT_TRUE(ctx, (push_flg_s & 16) == 0);

    keyBuf[88] = 1;
    check_Bbtn();
    TEST_ASSERT_EQ_INT(ctx, -32768, rotspeed.l);

    rotspeed.l = 0x00010000;
    check_Bbtn();
    TEST_ASSERT_EQ_INT(ctx, 32768, rotspeed.l);
}

static void test_check_b_button_accelerates_positive_rotation(
    test_context *ctx) {
    reset_state();
    rotflg = 1;
    keyBuf[88] = 1;

    check_Bbtn();

    TEST_ASSERT_EQ_INT(ctx, 32768, rotspeed.l);
    TEST_ASSERT_TRUE(ctx, (push_flg_s & 16) != 0);
}

static void test_check_c_button_zoom_branches_and_key_release(
    test_context *ctx) {
    reset_state();
    playwk.z_posi.w.h = 0;
    zoomflg = 1;
    keyBuf[67] = 1;

    check_Cbtn();
    TEST_ASSERT_EQ_INT(ctx, -1, zoomflg);
    TEST_ASSERT_EQ_INT(ctx, -8, playwk.z_posi.w.h);
    TEST_ASSERT_TRUE(ctx, (push_flg_s & 32) != 0);

    check_Cbtn();
    TEST_ASSERT_EQ_INT(ctx, -1, zoomflg);
    TEST_ASSERT_EQ_INT(ctx, -16, playwk.z_posi.w.h);

    zoomflg = -1;
    push_flg_s &= (Uint8)~32;
    check_Cbtn();
    TEST_ASSERT_EQ_INT(ctx, 1, zoomflg);
    TEST_ASSERT_EQ_INT(ctx, -8, playwk.z_posi.w.h);

    playwk.z_posi.w.h = 1793;
    zoomflg = -1;
    push_flg_s &= (Uint8)~32;
    check_Cbtn();
    TEST_ASSERT_EQ_INT(ctx, 1793, playwk.z_posi.w.h);

    keyBuf[67] = 0;
    check_Cbtn();
    TEST_ASSERT_TRUE(ctx, (push_flg_s & 32) == 0);

    keyBuf[67] = 1;
    zoomflg = 1;
    playwk.z_posi.w.h = -113;
    check_Cbtn();
    TEST_ASSERT_EQ_INT(ctx, -113, playwk.z_posi.w.h);
}

static void test_play2_switches_back_or_resets_on_start(test_context *ctx) {
    reset_state();
    playwk.ACT_NO = 2;
    rotspeed.w.h = 3;

    play2();
    TEST_ASSERT_EQ_INT(ctx, 1, playwk.ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, 3, playwk.k_kaku);

    playwk.ACT_NO = 2;
    comflag_m = 16;
    play2();
    TEST_ASSERT_EQ_INT(ctx, 2, playwk.ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, 6, playwk.k_kaku);

    swdata1.b.h = 16;
    play2();
    TEST_ASSERT_EQ_INT(ctx, 0, playwk.ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, 64, comflag_s);
    TEST_ASSERT_EQ_INT(ctx, 9, playwk.k_kaku);
}

TEST_MAIN_BEGIN;
test_player_dispatch_and_play0_initializes_camera_state(&ctx);
test_play1_start_button_sets_sub_command_and_resets(&ctx);
test_play1_movement_buttons_update_position_and_angle(&ctx);
test_play1_clamps_positions_to_z_extent(&ctx);
test_play1_enters_play2_when_main_command_flag_is_set(&ctx);
test_check_b_button_toggle_repeat_and_release_paths(&ctx);
test_check_b_button_accelerates_positive_rotation(&ctx);
test_check_c_button_zoom_branches_and_key_release(&ctx);
test_play2_switches_back_or_resets_on_start(&ctx);
TEST_MAIN_END
