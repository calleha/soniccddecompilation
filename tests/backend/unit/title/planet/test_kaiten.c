/* <stdlib.h> declares POSIX random(); the decompiled title/planet headers
   declare the game's own Sint32 random(void).  Parse stdlib first, then rename
   the game symbol inside this harness so the two can coexist. */
#include <stdlib.h>

#define random game_random

#include <string.h>

#include "support/test_runner.h"
#include "types.h"

#include "src/title/planet/kaiten.c"

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
Sint16 tv_adr[896];

Sint16 sin_planet(Sint16 sDir) {
    sDir &= 511;
    if (sDir == 0)
        return 0;
    if (sDir == 128)
        return 256;
    if (sDir == 256)
        return 0;
    if (sDir == 384)
        return -256;
    return (Sint16)(sDir / 2);
}

Sint16 cos_planet(Sint16 sDir) { return sin_planet(sDir + 128); }

static void reset_state(void) {
    memset(&zoomwk, 0, sizeof(zoomwk));
    memset(&playwk, 0, sizeof(playwk));
    memset(&k_wk, 0, sizeof(k_wk));
    memset(tv_adr, 0, sizeof(tv_adr));
    lp_cgflg = 0;
    push_flg_s = 0;
    rotspeed.l = 0;
    rotflg = 0;
    zoomflg = 0;
    lpcg_chg = 0;
    int1_flg = 0;
}

static void test_kaiten_init_is_existing_noop(test_context *ctx) {
    reset_state();
    zoomwk.x = 12;
    k_wk.k_s = 34;

    kaiten_init();

    TEST_ASSERT_EQ_INT(ctx, 12, zoomwk.x);
    TEST_ASSERT_EQ_INT(ctx, 34, k_wk.k_s);
}

static void test_calc_helpers_with_positive_centers(test_context *ctx) {
    reset_state();
    k_wk.k_cos = 256;
    k_wk.k_sin = 0;
    k_wk.k_s = 256;
    k_wk.k_rx = 128;
    k_wk.k_ry = 112;
    k_wk.k_x_rx = 32;
    k_wk.k_y_ry = -16;
    k_wk.k_msx = 64;
    k_wk.k_msy = 48;

    TEST_ASSERT_EQ_INT(ctx, 1280, calc_xt_r());
    TEST_ASSERT_EQ_INT(ctx, 768, calc_yt_r());
    TEST_ASSERT_EQ_INT(ctx, 1024, calc_xt_s(1024));
    TEST_ASSERT_EQ_INT(ctx, 768, calc_yt_s(768));
}

static void test_calc_helpers_with_negative_centers(test_context *ctx) {
    reset_state();
    k_wk.k_cos = -128;
    k_wk.k_sin = 64;
    k_wk.k_s = -256;
    k_wk.k_rx = -32;
    k_wk.k_ry = -48;
    k_wk.k_x_rx = -20;
    k_wk.k_y_ry = 12;
    k_wk.k_msx = -40;
    k_wk.k_msy = -24;

    TEST_ASSERT_EQ_INT(ctx, -200, calc_xt_r());
    TEST_ASSERT_EQ_INT(ctx, -472, calc_yt_r());
    TEST_ASSERT_EQ_INT(ctx, -1152, calc_xt_s(512));
    TEST_ASSERT_EQ_INT(ctx, -128, calc_yt_s(-256));
}

static void test_k3d_populates_projection_state_and_table(test_context *ctx) {
    reset_state();
    zoomwk.x = 144;
    zoomwk.y = 96;
    zoomwk.z = 0;
    zoomwk.az = 0;

    k_3d();

    TEST_ASSERT_EQ_INT(ctx, 128, k_wk.k_rx);
    TEST_ASSERT_EQ_INT(ctx, 112, k_wk.k_ry);
    TEST_ASSERT_EQ_INT(ctx, 256, k_wk.k_s);
    TEST_ASSERT_EQ_INT(ctx, 128, k_wk.k_s2);
    TEST_ASSERT_EQ_INT(ctx, 256, k_wk.k_cos);
    TEST_ASSERT_EQ_INT(ctx, 0, k_wk.k_sin);
    TEST_ASSERT_EQ_INT(ctx, 2048, k_wk.k_dx);
    TEST_ASSERT_EQ_INT(ctx, 0, k_wk.k_dy);
    TEST_ASSERT_EQ_INT(ctx, 272, k_wk.k_msx);
    TEST_ASSERT_EQ_INT(ctx, 184, k_wk.k_msy);
    TEST_ASSERT_EQ_INT(ctx, 160, k_wk.k_y_ry);
    TEST_ASSERT_EQ_INT(ctx, 1152, tv_adr[0]);
    TEST_ASSERT_EQ_INT(ctx, 768, tv_adr[1]);
    TEST_ASSERT_EQ_INT(ctx, 2048, tv_adr[2]);
    TEST_ASSERT_EQ_INT(ctx, 0, tv_adr[3]);
    TEST_ASSERT_EQ_INT(ctx, 1152, tv_adr[700]);
    TEST_ASSERT_EQ_INT(ctx, 2168, tv_adr[701]);
}

static void test_k3d_uses_signed_zoom_depth_path(test_context *ctx) {
    reset_state();
    zoomwk.x = 120;
    zoomwk.y = 140;
    zoomwk.z = -256;
    zoomwk.az = 128;

    k_3d();

    TEST_ASSERT_EQ_INT(ctx, -256, k_wk.k_s);
    TEST_ASSERT_EQ_INT(ctx, -128, k_wk.k_s2);
    TEST_ASSERT_EQ_INT(ctx, 0, k_wk.k_cos);
    TEST_ASSERT_EQ_INT(ctx, 256, k_wk.k_sin);
    TEST_ASSERT_EQ_INT(ctx, 0, k_wk.k_dx);
    TEST_ASSERT_EQ_INT(ctx, -2048, k_wk.k_dy);
    TEST_ASSERT_EQ_INT(ctx, 204, k_wk.k_y_ry);
    TEST_ASSERT_EQ_INT(ctx, -608, tv_adr[0]);
    TEST_ASSERT_EQ_INT(ctx, 2880, tv_adr[1]);
    TEST_ASSERT_EQ_INT(ctx, 0, tv_adr[2]);
    TEST_ASSERT_EQ_INT(ctx, -2048, tv_adr[3]);
}

static void test_kaiten0_copies_player_position_and_angle(test_context *ctx) {
    reset_state();
    playwk.k_kaku = 0;
    playwk.x_posi.w.h = 160;
    playwk.y_posi.w.h = 104;
    playwk.z_posi.w.h = 0;

    kaiten0();

    TEST_ASSERT_EQ_INT(ctx, 0, zoomwk.az);
    TEST_ASSERT_EQ_INT(ctx, 160, zoomwk.x);
    TEST_ASSERT_EQ_INT(ctx, 104, zoomwk.y);
    TEST_ASSERT_EQ_INT(ctx, 0, zoomwk.z);
    TEST_ASSERT_EQ_INT(ctx, 256, k_wk.k_cos);
    TEST_ASSERT_EQ_INT(ctx, 0, k_wk.k_sin);
    TEST_ASSERT_EQ_INT(ctx, 1280, tv_adr[0]);
}

TEST_MAIN_BEGIN;
test_kaiten_init_is_existing_noop(&ctx);
test_calc_helpers_with_positive_centers(&ctx);
test_calc_helpers_with_negative_centers(&ctx);
test_k3d_populates_projection_state_and_table(&ctx);
test_k3d_uses_signed_zoom_depth_path(&ctx);
test_kaiten0_copies_player_position_and_angle(&ctx);
TEST_MAIN_END
