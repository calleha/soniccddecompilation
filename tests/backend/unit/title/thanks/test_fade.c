/* <stdlib.h> declares POSIX random(); the decompiled thanks headers declare
   the game's own Sint32 random(void).  Parse stdlib first, then rename the game
   symbol inside this harness so the two can coexist. */
#include <stdlib.h>

#define random game_random

#include <string.h>

#include "support/test_runner.h"
#include "src/types.h"

Uint8 colorcnt;
Uint8 startcolor;
Sint32 fade_flag;
Sint32 *lpFadeFlag;
Sint16 FadeFlag;
PALETTEENTRY palette0[80];
PALETTEENTRY palette1[80];
PALETTEENTRY palette3[80];
PALETTEENTRY *lpcolorwk0;
PALETTEENTRY *lpcolorwk1;
PALETTEENTRY *lpcolorwk3;
Uint8 int_flg;
static Sint32 v_int_call_count;

void v_int(void);

#include "src/title/thanks/fade.c"

void v_int(void) { ++v_int_call_count; }

static PALETTEENTRY entry(Uint8 red, Uint8 green, Uint8 blue, Uint8 flags) {
    PALETTEENTRY pe;

    pe.peRed = red;
    pe.peGreen = green;
    pe.peBlue = blue;
    pe.peFlags = flags;
    return pe;
}

static void reset_fixture(void) {
    memset(palette0, 0x44, sizeof(palette0));
    memset(palette1, 0, sizeof(palette1));
    memset(palette3, 0x22, sizeof(palette3));
    lpcolorwk0 = palette0;
    lpcolorwk1 = palette1;
    lpcolorwk3 = palette3;
    fade_flag = 0;
    lpFadeFlag = &fade_flag;
    FadeFlag = 0;
    FadeCount = 0;
    colorcnt = 0;
    startcolor = 0;
    int_flg = 0;
    v_int_call_count = 0;
}

static void assert_entry(test_context *ctx, PALETTEENTRY actual, Uint8 red,
                         Uint8 green, Uint8 blue, Uint8 flags) {
    TEST_ASSERT_EQ_INT(ctx, red, actual.peRed);
    TEST_ASSERT_EQ_INT(ctx, green, actual.peGreen);
    TEST_ASSERT_EQ_INT(ctx, blue, actual.peBlue);
    TEST_ASSERT_EQ_INT(ctx, flags, actual.peFlags);
}

static void test_fadein_initializes_full_palette_and_flags(test_context *ctx) {
    reset_fixture();

    fadein();

    TEST_ASSERT_EQ_INT(ctx, 0, startcolor);
    TEST_ASSERT_EQ_INT(ctx, 63, colorcnt);
    TEST_ASSERT_EQ_INT(ctx, 0, FadeCount);
    TEST_ASSERT_EQ_INT(ctx, 1, FadeFlag);
    TEST_ASSERT_EQ_INT(ctx, 1, fade_flag);
    assert_entry(ctx, palette0[0], 0, 0, 0, 1);
    assert_entry(ctx, palette0[63], 0, 0, 0, 1);
}

static void test_fadein0_respects_start_and_count(test_context *ctx) {
    reset_fixture();
    startcolor = 4;
    colorcnt = 2;

    fadein0();

    assert_entry(ctx, palette0[1], 0x44, 0x44, 0x44, 0x44);
    assert_entry(ctx, palette0[2], 0, 0, 0, 1);
    assert_entry(ctx, palette0[3], 0, 0, 0, 1);
    assert_entry(ctx, palette0[4], 0, 0, 0, 1);
    assert_entry(ctx, palette0[5], 0x44, 0x44, 0x44, 0x44);
}

static void test_fadein_steps_blue_then_green_then_red(test_context *ctx) {
    reset_fixture();
    colorcnt = 0;
    palette0[0] = entry(0, 0, 0, 0x44);
    palette1[0] = entry(64, 64, 64, 1);

    TEST_ASSERT_EQ_INT(ctx, 0, fadein0_new());
    assert_entry(ctx, palette0[0], 0, 0, 32, 0x44);
    TEST_ASSERT_EQ_INT(ctx, 10, int_flg);

    TEST_ASSERT_EQ_INT(ctx, 0, fadein0_new());
    assert_entry(ctx, palette0[0], 0, 0, 64, 0x44);

    TEST_ASSERT_EQ_INT(ctx, 0, fadein0_new());
    assert_entry(ctx, palette0[0], 0, 32, 64, 0x44);

    TEST_ASSERT_EQ_INT(ctx, 0, fadein0_new());
    assert_entry(ctx, palette0[0], 0, 64, 64, 0x44);

    TEST_ASSERT_EQ_INT(ctx, 0, fadein0_new());
    assert_entry(ctx, palette0[0], 32, 64, 64, 0x44);

    palette0[0] = palette1[0];
    FadeCount = 21;
    TEST_ASSERT_EQ_INT(ctx, 1, fadein0_new());
    TEST_ASSERT_EQ_INT(ctx, 0, FadeCount);
}

static void test_fadeout_initializes_and_steps_channels(test_context *ctx) {
    reset_fixture();

    fadeout();

    TEST_ASSERT_EQ_INT(ctx, 0, startcolor);
    TEST_ASSERT_EQ_INT(ctx, 63, colorcnt);
    TEST_ASSERT_EQ_INT(ctx, 0, FadeCount);
    TEST_ASSERT_EQ_INT(ctx, 2, FadeFlag);
    TEST_ASSERT_EQ_INT(ctx, 2, fade_flag);

    palette0[0] = entry(64, 64, 64, 1);
    fadeout2(&palette0[0]);
    assert_entry(ctx, palette0[0], 32, 64, 64, 1);
    palette0[0].peRed = 0;
    fadeout2(&palette0[0]);
    assert_entry(ctx, palette0[0], 0, 32, 64, 1);
    palette0[0].peGreen = 0;
    fadeout2(&palette0[0]);
    assert_entry(ctx, palette0[0], 0, 0, 32, 1);
    palette0[0].peBlue = 0;
    fadeout2(&palette0[0]);
    assert_entry(ctx, palette0[0], 0, 0, 0, 1);
}

static void test_fadeproc_dispatches_and_calls_vint(test_context *ctx) {
    reset_fixture();
    colorcnt = 0;
    palette0[0] = entry(0, 0, 0, 0x44);
    palette1[0] = entry(32, 0, 0, 1);
    fade_flag = 1;

    TEST_ASSERT_EQ_INT(ctx, 0, FadeProc());
    TEST_ASSERT_EQ_INT(ctx, 1, v_int_call_count);
    assert_entry(ctx, palette0[0], 32, 0, 0, 0x44);

    reset_fixture();
    colorcnt = 0;
    palette0[0] = entry(32, 0, 0, 1);
    palette3[0] = entry(0, 32, 0, 1);
    FadeCount = 21;
    fade_flag = 2;

    TEST_ASSERT_EQ_INT(ctx, 1, FadeProc());
    TEST_ASSERT_EQ_INT(ctx, 1, v_int_call_count);
    TEST_ASSERT_EQ_INT(ctx, 10, int_flg);
    TEST_ASSERT_EQ_INT(ctx, 0, FadeCount);
    assert_entry(ctx, palette0[0], 0, 0, 0, 1);
    assert_entry(ctx, palette3[0], 0, 0, 0, 1);

    reset_fixture();
    colorcnt = 0;
    palette0[0] = entry(32, 0, 0, 1);
    palette3[0] = entry(0, 0, 32, 1);
    fade_flag = 2;

    TEST_ASSERT_EQ_INT(ctx, 0, FadeProc());
    TEST_ASSERT_EQ_INT(ctx, 1, v_int_call_count);
    assert_entry(ctx, palette0[0], 0, 0, 0, 1);
    assert_entry(ctx, palette3[0], 0, 0, 0, 1);

    reset_fixture();
    fade_flag = 0;
    TEST_ASSERT_EQ_INT(ctx, 1, FadeProc());
    TEST_ASSERT_EQ_INT(ctx, 1, v_int_call_count);
}

TEST_MAIN_BEGIN;
test_fadein_initializes_full_palette_and_flags(&ctx);
test_fadein0_respects_start_and_count(&ctx);
test_fadein_steps_blue_then_green_then_red(&ctx);
test_fadeout_initializes_and_steps_channels(&ctx);
test_fadeproc_dispatches_and_calls_vint(&ctx);
TEST_MAIN_END
