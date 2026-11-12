/* <stdlib.h> declares POSIX random(); the decompiled title/planet headers
   declare the game's own Sint32 random(void).  Parse stdlib first, then rename
   the game symbol inside this harness so the two can coexist. */
#include <stdlib.h>

#define random game_random

#include <string.h>

#include "support/test_runner.h"
#include "types.h"

#include "src/title/planet/flash.c"

static int v_int_calls;
static Sint32 fade_flag_storage;
static PALETTEENTRY color0[64];
static PALETTEENTRY color1[64];
static PALETTEENTRY color3[64];

Uint16 comdata_s7;
Uint16 comdata_s6;
Uint16 comdata_s5;
Uint16 comdata_s4;
Uint16 comdata_s3;
Uint16 comdata_s2;
Uint16 comdata_s1;
Uint16 comdata_s0;
ushort_union swdata2;
ushort_union swdata1;
Uint16 comdata_m7;
Uint16 comdata_m6;
Uint16 comdata_m5;
Uint16 comdata_m4;
Uint16 comdata_m3;
Uint16 comdata_m2;
Uint16 comdata_m1;
Uint16 comdata_m0;
Uint8 comflag_s;
Uint8 comflag_m;
Uint8 cgchg_end;
Uint8 cgchg_timer;
Uint8 flash_flg;
Uint8 colorcnt;
Uint8 startcolor;
Sint16 lplanet_no;
Sint16 flash_timer;
Sint32 overint;
Sint32 ranum;
Sint16 sr_save;
Sint16 gmtime1;
Sint16 gmtime0;
Sint16 intmode;
Sint16 linkdata;
Uint8 colorflg;
Uint8 int_flg;
Uint8 push_flg;
Uint8 actset_wk;
Uint8 actset_flg;
Sint16 actset_tm[6];
Sint16 spr_colno;
Sint16 spr_coltimer;
Sint16 main_lpcnt;
Sint16 funka_patno;
Sint16 funka_pctimer;
Sint16 colorno;
Uint16 coltimer;
sprite_status_lpl actwk[18];
Sint32 ProcMode;
Sint32 DLLRetMode;
Sint32 *lpFadeFlag;
Sint16 tv_adr[896];
wordram_info wordram;
int_union *lphscrollbuff;
PALETTEENTRY *lpcolorwk3;
PALETTEENTRY *lpcolorwk2;
PALETTEENTRY *lpcolorwk1;
PALETTEENTRY *lpcolorwk0;
short_union stageno;
Uint8 music_selflg;
Uint8 m2save[22528];
bmp_info SprBmp[700];

void WorkRamClear(void) {}

void v_int(void) { ++v_int_calls; }

static void reset_state(void) {
    memset(color0, 0, sizeof(color0));
    memset(color1, 0, sizeof(color1));
    memset(color3, 0, sizeof(color3));
    fade_flag_storage = 0;
    lpFadeFlag = &fade_flag_storage;
    lpcolorwk0 = color0;
    lpcolorwk1 = color1;
    lpcolorwk3 = color3;
    startcolor = 0;
    colorcnt = 0;
    flash_flg = 7;
    int_flg = 0;
    FadeCount = 0;
    v_int_calls = 0;
}

static void test_fadein_initializes_range_to_black_and_sets_flag(
    test_context *ctx) {
    reset_state();
    color0[1].peRed = 99;
    color0[2].peGreen = 88;
    startcolor = 2;
    colorcnt = 1;

    fadein0();

    TEST_ASSERT_EQ_INT(ctx, 0, color0[1].peRed);
    TEST_ASSERT_EQ_INT(ctx, 0, color0[1].peGreen);
    TEST_ASSERT_EQ_INT(ctx, 0, color0[1].peBlue);
    TEST_ASSERT_EQ_INT(ctx, 1, color0[1].peFlags);
    TEST_ASSERT_EQ_INT(ctx, 0, color0[2].peRed);
    TEST_ASSERT_EQ_INT(ctx, 0, color0[2].peGreen);
    TEST_ASSERT_EQ_INT(ctx, 0, color0[2].peBlue);
    TEST_ASSERT_EQ_INT(ctx, 1, color0[2].peFlags);
    TEST_ASSERT_EQ_INT(ctx, 1, fade_flag_storage);

    fadein();
    TEST_ASSERT_EQ_INT(ctx, 0, startcolor);
    TEST_ASSERT_EQ_INT(ctx, 63, colorcnt);
    TEST_ASSERT_EQ_INT(ctx, 1, fade_flag_storage);
}

static void test_fadein2_steps_blue_then_green_then_red(test_context *ctx) {
    PALETTEENTRY dest = {0, 0, 0, 1};
    PALETTEENTRY src = {64, 64, 64, 1};

    reset_state();
    fadein2(&dest, &src);
    TEST_ASSERT_EQ_INT(ctx, 32, dest.peBlue);
    TEST_ASSERT_EQ_INT(ctx, 0, dest.peGreen);
    TEST_ASSERT_EQ_INT(ctx, 0, dest.peRed);

    dest.peBlue = 64;
    fadein2(&dest, &src);
    TEST_ASSERT_EQ_INT(ctx, 32, dest.peGreen);
    TEST_ASSERT_EQ_INT(ctx, 0, dest.peRed);

    dest.peGreen = 64;
    fadein2(&dest, &src);
    TEST_ASSERT_EQ_INT(ctx, 32, dest.peRed);

    dest.peRed = 64;
    fadein2(&dest, &src);
    TEST_ASSERT_EQ_INT(ctx, 64, dest.peRed);
}

static void test_fadeout_steps_channels_and_dispatches(test_context *ctx) {
    PALETTEENTRY dest = {64, 64, 64, 1};

    reset_state();
    fadeout();
    TEST_ASSERT_EQ_INT(ctx, 2, fade_flag_storage);

    fadeout2(&dest);
    TEST_ASSERT_EQ_INT(ctx, 32, dest.peRed);
    TEST_ASSERT_EQ_INT(ctx, 64, dest.peGreen);
    dest.peRed = 0;
    fadeout2(&dest);
    TEST_ASSERT_EQ_INT(ctx, 32, dest.peGreen);
    dest.peGreen = 0;
    fadeout2(&dest);
    TEST_ASSERT_EQ_INT(ctx, 32, dest.peBlue);
    dest.peBlue = 0;
    fadeout2(&dest);
    TEST_ASSERT_EQ_INT(ctx, 0, dest.peBlue);

    color0[0].peRed = 64;
    color3[0].peGreen = 64;
    colorcnt = 0;
    fade_flag_storage = 2;
    TEST_ASSERT_EQ_INT(ctx, 0, FadeProc());
    TEST_ASSERT_EQ_INT(ctx, 32, color0[0].peRed);
    TEST_ASSERT_EQ_INT(ctx, 32, color3[0].peGreen);
    TEST_ASSERT_EQ_INT(ctx, 10, int_flg);
    TEST_ASSERT_EQ_INT(ctx, 1, v_int_calls);
}

static void test_flashin_initializes_white_and_steps_toward_source(
    test_context *ctx) {
    PALETTEENTRY dest = {224, 224, 224, 1};
    PALETTEENTRY src = {128, 128, 128, 1};

    reset_state();
    startcolor = 2;
    colorcnt = 1;
    flashin();
    TEST_ASSERT_EQ_INT(ctx, 224, color0[1].peRed);
    TEST_ASSERT_EQ_INT(ctx, 224, color0[1].peGreen);
    TEST_ASSERT_EQ_INT(ctx, 224, color0[1].peBlue);
    TEST_ASSERT_EQ_INT(ctx, 1, color0[1].peFlags);
    TEST_ASSERT_EQ_INT(ctx, 3, fade_flag_storage);

    flashin2(&dest, &src);
    TEST_ASSERT_EQ_INT(ctx, 192, dest.peBlue);
    TEST_ASSERT_EQ_INT(ctx, 224, dest.peGreen);
    TEST_ASSERT_EQ_INT(ctx, 224, dest.peRed);

    dest.peBlue = 128;
    flashin2(&dest, &src);
    TEST_ASSERT_EQ_INT(ctx, 192, dest.peGreen);
    TEST_ASSERT_EQ_INT(ctx, 224, dest.peRed);

    dest.peGreen = 128;
    flashin2(&dest, &src);
    TEST_ASSERT_EQ_INT(ctx, 192, dest.peRed);

    dest.peRed = 128;
    flashin2(&dest, &src);
    TEST_ASSERT_EQ_INT(ctx, 128, dest.peRed);
}

static void test_flashout_steps_channels_and_dispatches(test_context *ctx) {
    PALETTEENTRY dest = {128, 128, 128, 1};

    reset_state();
    flashout();
    TEST_ASSERT_EQ_INT(ctx, 4, fade_flag_storage);

    flashout2(&dest);
    TEST_ASSERT_EQ_INT(ctx, 160, dest.peRed);
    TEST_ASSERT_EQ_INT(ctx, 128, dest.peGreen);
    dest.peRed = 224;
    flashout2(&dest);
    TEST_ASSERT_EQ_INT(ctx, 160, dest.peGreen);
    dest.peGreen = 224;
    flashout2(&dest);
    TEST_ASSERT_EQ_INT(ctx, 160, dest.peBlue);
    dest.peBlue = 224;
    flashout2(&dest);
    TEST_ASSERT_EQ_INT(ctx, 224, dest.peBlue);

    color0[0].peRed = 128;
    color3[0].peRed = 224;
    color3[0].peGreen = 224;
    color3[0].peBlue = 128;
    colorcnt = 0;
    fade_flag_storage = 4;
    TEST_ASSERT_EQ_INT(ctx, 0, FadeProc());
    TEST_ASSERT_EQ_INT(ctx, 160, color0[0].peRed);
    TEST_ASSERT_EQ_INT(ctx, 160, color3[0].peBlue);
    TEST_ASSERT_EQ_INT(ctx, 10, int_flg);
    TEST_ASSERT_EQ_INT(ctx, 1, v_int_calls);
}

static void test_fadeproc_completion_and_default_paths(test_context *ctx) {
    reset_state();
    fade_flag_storage = 1;
    flash_flg = 9;
    FadeCount = 0;
    TEST_ASSERT_EQ_INT(ctx, 0, FadeProc());
    TEST_ASSERT_EQ_INT(ctx, 9, flash_flg);

    reset_state();
    fade_flag_storage = 1;
    flash_flg = 9;
    FadeCount = 21;
    TEST_ASSERT_EQ_INT(ctx, 1, FadeProc());
    TEST_ASSERT_EQ_INT(ctx, 0, FadeCount);
    TEST_ASSERT_EQ_INT(ctx, 0, flash_flg);

    reset_state();
    fade_flag_storage = 2;
    flash_flg = 9;
    FadeCount = 21;
    TEST_ASSERT_EQ_INT(ctx, 1, FadeProc());
    TEST_ASSERT_EQ_INT(ctx, 0, FadeCount);
    TEST_ASSERT_EQ_INT(ctx, 0, flash_flg);

    reset_state();
    fade_flag_storage = 3;
    flash_flg = 9;
    FadeCount = 0;
    TEST_ASSERT_EQ_INT(ctx, 0, FadeProc());
    TEST_ASSERT_EQ_INT(ctx, 9, flash_flg);

    reset_state();
    fade_flag_storage = 3;
    flash_flg = 9;
    FadeCount = 21;
    TEST_ASSERT_EQ_INT(ctx, 1, FadeProc());
    TEST_ASSERT_EQ_INT(ctx, 0, flash_flg);

    reset_state();
    fade_flag_storage = 4;
    flash_flg = 9;
    FadeCount = 21;
    TEST_ASSERT_EQ_INT(ctx, 1, FadeProc());
    TEST_ASSERT_EQ_INT(ctx, 0, FadeCount);
    TEST_ASSERT_EQ_INT(ctx, 0, flash_flg);

    reset_state();
    fade_flag_storage = 99;
    flash_flg = 9;
    TEST_ASSERT_EQ_INT(ctx, 1, FadeProc());
    TEST_ASSERT_EQ_INT(ctx, 0, flash_flg);
    TEST_ASSERT_EQ_INT(ctx, 1, v_int_calls);
}

TEST_MAIN_BEGIN;
test_fadein_initializes_range_to_black_and_sets_flag(&ctx);
test_fadein2_steps_blue_then_green_then_red(&ctx);
test_fadeout_steps_channels_and_dispatches(&ctx);
test_flashin_initializes_white_and_steps_toward_source(&ctx);
test_flashout_steps_channels_and_dispatches(&ctx);
test_fadeproc_completion_and_default_paths(&ctx);
TEST_MAIN_END
