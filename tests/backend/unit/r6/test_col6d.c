#include <stdio.h>
#include <string.h>

#include "support/test_runner.h"
#include "src/types.h"

Uint8 startcolor;
Uint8 colorcnt;
Uint8 int_flg;
Uint8 bossflag;
Sint32 fade_flag_storage;
Sint32 *lpFadeFlag = &fade_flag_storage;
PALETTEENTRY colorwk[256];
PALETTEENTRY colorwk2[256];
PALETTEENTRY colorwk3[256];
PALETTEENTRY colorwk4[256];
PALETTEENTRY *lpcolorwk = colorwk;
PALETTEENTRY *lpcolorwk2 = colorwk2;
PALETTEENTRY *lpcolorwk3 = colorwk3;
PALETTEENTRY *lpcolorwk4 = colorwk4;
Uint8 clchgtim[7];
Uint8 clchgcnt[7];
short_union stageno;

#include "src/r6/col6d.c"

static void reset_col6d_state(void) {
    memset(colorwk, 0, sizeof(colorwk));
    memset(colorwk2, 0, sizeof(colorwk2));
    memset(colorwk3, 0, sizeof(colorwk3));
    memset(colorwk4, 0, sizeof(colorwk4));
    memset(clchgtim, 0, sizeof(clchgtim));
    memset(clchgcnt, 0, sizeof(clchgcnt));
    memset(&stageno, 0, sizeof(stageno));
    startcolor = 0;
    colorcnt = 0;
    int_flg = 0;
    bossflag = 0;
    fade_flag_storage = 0;
    FadeCount = 0;
}

static void assert_palette(test_context *ctx, PALETTEENTRY actual, int red,
                           int green, int blue, int flags,
                           const char *label) {
    if (actual.peRed != red || actual.peGreen != green ||
        actual.peBlue != blue || actual.peFlags != flags) {
        ++ctx->failures;
        fprintf(stderr,
                "%s: expected {%d,%d,%d,%d}, got {%u,%u,%u,%u}\n", label,
                red, green, blue, flags, actual.peRed, actual.peGreen,
                actual.peBlue, actual.peFlags);
    }
}

static void test_col6d_tables_capture_palette_contract(test_context *ctx) {
    TEST_ASSERT_EQ_INT(ctx, 64,
                       (int)(sizeof(advacolor) / sizeof(advacolor[0])));
    TEST_ASSERT_EQ_INT(ctx, 64,
                       (int)(sizeof(advacolor2) / sizeof(advacolor2[0])));
    TEST_ASSERT_EQ_INT(ctx, 56,
                       (int)(sizeof(zone8colora) / sizeof(zone8colora[0])));
    TEST_ASSERT_EQ_INT(ctx, 48,
                       (int)(sizeof(zone82colora) / sizeof(zone82colora[0])));
    TEST_ASSERT_EQ_INT(ctx, 16,
                       (int)(sizeof(boss6col) / sizeof(boss6col[0])));
    TEST_ASSERT_EQ_INT(ctx, 88,
                       (int)(sizeof(boss6_col4) / sizeof(boss6_col4[0])));
    TEST_ASSERT_EQ_INT(ctx, 7, (int)(sizeof(colortbl) / sizeof(colortbl[0])));

    assert_palette(ctx, advacolor[33], 160, 192, 160, 1, "advacolor[33]");
    assert_palette(ctx, advacolor[63], 224, 160, 32, 1, "advacolor[63]");
    assert_palette(ctx, zone8colora[47], 224, 160, 32, 1,
                   "zone8colora[47]");
    assert_palette(ctx, zone8colora[55], 224, 128, 96, 1,
                   "zone8colora[55]");
    assert_palette(ctx, zone82colora[47], 224, 160, 32, 1,
                   "zone82colora[47]");
    assert_palette(ctx, boss6col[0], 224, 224, 96, 1, "boss6col[0]");
    assert_palette(ctx, boss6_col4[87], 224, 224, 224, 1,
                   "boss6_col4[87]");

    TEST_ASSERT_TRUE(ctx, colortbl[4].PeSrc == zone8colora);
    TEST_ASSERT_TRUE(ctx, colortbl[5].PeSrc == zone82colora);
    TEST_ASSERT_TRUE(ctx, colortbl[6].PeSrc == boss6col);
    TEST_ASSERT_EQ_INT(ctx, 48, colortbl[4].cnt);
    TEST_ASSERT_EQ_INT(ctx, 16, colortbl[6].offset);
    assert_palette(ctx, col6a0col[1], 64, 96, 64, 1, "col6a0col[1]");
    assert_palette(ctx, col6a3col[1], 64, 0, 64, 1, "col6a3col[1]");
    assert_palette(ctx, col6a3col_stop[1], 0, 0, 0, 1,
                   "col6a3col_stop[1]");
}

static void test_clchg_sub0_waits_updates_and_wraps(test_context *ctx) {
    Uint8 cnt_tbl[] = {4, 2, 7, 1, 9, 0};
    PALETTEENTRY colors[] = {{11, 22, 33, 1}, {44, 55, 66, 1}};
    Uint8 change_time;
    Uint8 change_count;

    reset_col6d_state();
    change_time = 3;
    change_count = 0;
    colorwk[4] = (PALETTEENTRY){1, 2, 3, 4};

    clchg_sub0(&change_time, &change_count, cnt_tbl, colors);

    TEST_ASSERT_EQ_INT(ctx, 2, change_time);
    TEST_ASSERT_EQ_INT(ctx, 0, change_count);
    assert_palette(ctx, colorwk[4], 1, 2, 3, 4, "waiting color");

    change_time = 0;
    change_count = 0;
    clchg_sub0(&change_time, &change_count, cnt_tbl, colors);

    TEST_ASSERT_EQ_INT(ctx, 9, change_time);
    TEST_ASSERT_EQ_INT(ctx, 1, change_count);
    assert_palette(ctx, colorwk[4], 11, 22, 33, 1, "advanced color");

    change_time = 0;
    change_count = 1;
    clchg_sub0(&change_time, &change_count, cnt_tbl, colors);

    TEST_ASSERT_EQ_INT(ctx, 7, change_time);
    TEST_ASSERT_EQ_INT(ctx, 0, change_count);
    assert_palette(ctx, colorwk[4], 44, 55, 66, 1, "wrapped color");
}

static void test_colchg6a_updates_running_and_stopped_boss_cycles(
    test_context *ctx) {
    reset_col6d_state();

    clchgctr();

    TEST_ASSERT_EQ_INT(ctx, 1, clchgcnt[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, clchgcnt[3]);
    assert_palette(ctx, colorwk[33], 64, 96, 64, 1, "cycle color 33");
    assert_palette(ctx, colorwk[34], 0, 0, 0, 1, "cycle color 34");
    assert_palette(ctx, colorwk[35], 160, 192, 160, 1, "cycle color 35");
    assert_palette(ctx, colorwk[49], 0, 0, 0, 1, "running cycle color 49");

    reset_col6d_state();
    bossflag = 32;
    colorwk[49] = (PALETTEENTRY){9, 8, 7, 6};

    colchg6a();

    TEST_ASSERT_EQ_INT(ctx, 1, clchgcnt[3]);
    assert_palette(ctx, colorwk[49], 0, 0, 0, 1, "stopped cycle color 49");
}

static void test_fadein_stage_gate_is_reversed_from_col6c(test_context *ctx) {
    reset_col6d_state();
    startcolor = 4;
    colorcnt = 2;
    colorwk[2] = (PALETTEENTRY){99, 88, 77, 0};
    colorwk[4] = (PALETTEENTRY){99, 88, 77, 0};

    fadein0();

    assert_palette(ctx, colorwk[2], 0, 0, 0, 1, "fadein0 first");
    assert_palette(ctx, colorwk[4], 0, 0, 0, 1, "fadein0 last");
    TEST_ASSERT_EQ_INT(ctx, 1, fade_flag_storage);

    colorwk2[2] = (PALETTEENTRY){64, 32, 96, 1};
    colorwk2[3] = (PALETTEENTRY){64, 0, 0, 1};
    colorwk4[2] = (PALETTEENTRY){0, 0, 64, 1};
    colorwk4[3] = (PALETTEENTRY){0, 64, 0, 1};

    TEST_ASSERT_EQ_INT(ctx, 0, fadein0_new());
    TEST_ASSERT_EQ_INT(ctx, 18, int_flg);
    assert_palette(ctx, colorwk[2], 0, 0, 32, 1, "fadein blue first");
    assert_palette(ctx, colorwk[3], 32, 0, 0, 1, "fadein red first");
    assert_palette(ctx, colorwk3[2], 0, 0, 0, 0, "fadein wk3 skipped");

    stageno.b.h = 1;
    TEST_ASSERT_EQ_INT(ctx, 0, fadein0_new());
    assert_palette(ctx, colorwk3[2], 0, 0, 32, 0, "fadein wk3 stage one");
    assert_palette(ctx, colorwk3[3], 0, 32, 0, 0, "fadein wk3 green");

    FadeCount = 21;
    TEST_ASSERT_EQ_INT(ctx, 1, fadein0_new());
    TEST_ASSERT_EQ_INT(ctx, 0, FadeCount);
}

static void test_fadeout_flashin_and_flashout_transitions(test_context *ctx) {
    reset_col6d_state();
    colorwk[0] = (PALETTEENTRY){96, 64, 32, 1};
    colorwk[1] = (PALETTEENTRY){0, 64, 32, 1};
    colorwk[2] = (PALETTEENTRY){0, 0, 32, 1};
    colorwk3[0] = (PALETTEENTRY){32, 0, 0, 1};

    fadeout();

    TEST_ASSERT_EQ_INT(ctx, 2, fade_flag_storage);
    TEST_ASSERT_EQ_INT(ctx, 0, FadeProc());
    assert_palette(ctx, colorwk[0], 64, 64, 32, 1, "fadeout red");
    assert_palette(ctx, colorwk[1], 0, 32, 32, 1, "fadeout green");
    assert_palette(ctx, colorwk[2], 0, 0, 0, 1, "fadeout blue");
    assert_palette(ctx, colorwk3[0], 0, 0, 0, 1, "fadeout wk3 red");

    FadeCount = 21;
    TEST_ASSERT_EQ_INT(ctx, 1, FadeProc());

    reset_col6d_state();
    colorwk2[0] = (PALETTEENTRY){128, 96, 64, 1};
    colorwk2[1] = (PALETTEENTRY){224, 128, 224, 1};
    colorwk4[0] = (PALETTEENTRY){224, 224, 192, 1};

    flashin();

    TEST_ASSERT_EQ_INT(ctx, 3, fade_flag_storage);
    colorwk3[0] = (PALETTEENTRY){224, 224, 224, 0};
    colorwk[32] = (PALETTEENTRY){1, 2, 3, 4};
    TEST_ASSERT_EQ_INT(ctx, 0, FadeProc());
    assert_palette(ctx, colorwk[0], 224, 224, 192, 1, "flashin blue down");
    assert_palette(ctx, colorwk[1], 224, 192, 224, 1, "flashin green down");
    assert_palette(ctx, colorwk[32], 224, 224, 224, 1,
                   "flashin forced boss color");
    assert_palette(ctx, colorwk3[0], 224, 224, 224, 0,
                   "flashin wk3 skipped");

    stageno.b.h = 1;
    TEST_ASSERT_EQ_INT(ctx, 0, FadeProc());
    assert_palette(ctx, colorwk3[0], 224, 224, 192, 0,
                   "flashin wk3 stage one");

    FadeCount = 21;
    TEST_ASSERT_EQ_INT(ctx, 1, FadeProc());

    reset_col6d_state();
    colorwk[0] = (PALETTEENTRY){0, 0, 0, 1};
    colorwk[1] = (PALETTEENTRY){224, 0, 0, 1};
    colorwk[2] = (PALETTEENTRY){224, 224, 0, 1};
    colorwk3[0] = (PALETTEENTRY){0, 0, 0, 1};

    flashout();

    TEST_ASSERT_EQ_INT(ctx, 4, fade_flag_storage);
    TEST_ASSERT_EQ_INT(ctx, 0, FadeProc());
    assert_palette(ctx, colorwk[0], 32, 0, 0, 1, "flashout red");
    assert_palette(ctx, colorwk[1], 224, 32, 0, 1, "flashout green");
    assert_palette(ctx, colorwk[2], 224, 224, 32, 1, "flashout blue");
    assert_palette(ctx, colorwk3[0], 32, 0, 0, 1, "flashout wk3 red");

    FadeCount = 21;
    TEST_ASSERT_EQ_INT(ctx, 1, FadeProc());
}

static void test_direct_fade_helpers_leave_matching_palette_entries_alone(
    test_context *ctx) {
    PALETTEENTRY entry = {32, 64, 96, 7};
    PALETTEENTRY same = {32, 64, 96, 1};
    PALETTEENTRY white = {224, 224, 224, 3};

    fadein2(&entry, &same);
    assert_palette(ctx, entry, 32, 64, 96, 7, "fadein2 same");

    flashin2(&entry, &same);
    assert_palette(ctx, entry, 32, 64, 96, 7, "flashin2 same");

    entry = (PALETTEENTRY){96, 64, 32, 7};
    same = (PALETTEENTRY){32, 64, 32, 1};
    flashin2(&entry, &same);
    assert_palette(ctx, entry, 64, 64, 32, 7, "flashin2 red");

    entry = (PALETTEENTRY){32, 64, 96, 7};
    fadeout2(&entry);
    fadeout2(&entry);
    fadeout2(&entry);
    assert_palette(ctx, entry, 0, 0, 96, 7, "fadeout2 priority");

    flashout2(&white);
    assert_palette(ctx, white, 224, 224, 224, 3, "flashout2 same");
}

static void test_colorset_routes_to_palette_work_buffers(test_context *ctx) {
    reset_col6d_state();

    colorset(3);
    assert_palette(ctx, colorwk2[0], 0, 0, 0, 1, "colorset work 1");
    assert_palette(ctx, colorwk2[15], 224, 224, 0, 1, "colorset work 1 end");

    colorset2(4);
    assert_palette(ctx, colorwk[16], 0, 0, 0, 1, "colorset2 work 0");
    assert_palette(ctx, colorwk[63], 224, 160, 32, 1,
                   "colorset2 copied 48 entries only");
    assert_palette(ctx, colorwk[64], 0, 0, 0, 0, "colorset2 beyond count");

    colorset3(5);
    assert_palette(ctx, colorwk[63], 224, 160, 32, 1,
                   "colorset3 work 0 end");

    colorset00(0, 2);
    assert_palette(ctx, colorwk3[0], 0, 0, 0, 1, "colorset00 work 2");
    assert_palette(ctx, colorwk3[63], 224, 160, 32, 1,
                   "colorset00 work 2 end");

    colorset00(0, 3);
    assert_palette(ctx, colorwk4[0], 0, 0, 0, 1, "colorset00 default");
    assert_palette(ctx, colorwk4[63], 224, 160, 32, 1,
                   "colorset00 default end");

    colorset(6);
    assert_palette(ctx, colorwk2[16], 224, 224, 96, 1, "colorset boss");
    assert_palette(ctx, colorwk2[31], 224, 0, 0, 1, "colorset boss end");
}

static void test_boss_fade_helpers_step_and_sparse_slots(test_context *ctx) {
    Uint8 chg_time;
    Uint8 chg_cnt;

    reset_col6d_state();
    chg_time = 10;
    chg_cnt = 20;

    fin_boss6(&chg_time, &chg_cnt);

    TEST_ASSERT_EQ_INT(ctx, 11, chg_time);
    TEST_ASSERT_EQ_INT(ctx, 20, chg_cnt);

    chg_time = 11;
    chg_cnt = 5;

    fin_boss6(&chg_time, &chg_cnt);

    TEST_ASSERT_EQ_INT(ctx, 0, chg_time);
    TEST_ASSERT_EQ_INT(ctx, 0, chg_cnt);
    assert_palette(ctx, colorwk[32], 0, 0, 0, 1, "fin boss first");
    assert_palette(ctx, colorwk[50], 64, 64, 224, 1, "fin boss 50");
    assert_palette(ctx, colorwk[52], 0, 0, 0, 0, "fin boss skipped 52");
    assert_palette(ctx, colorwk[55], 0, 0, 0, 0, "fin boss skipped 55");
    assert_palette(ctx, colorwk[63], 224, 160, 32, 1, "fin boss 63");

    chg_time = 10;
    chg_cnt = 33;

    fout_boss6(&chg_time, &chg_cnt);

    TEST_ASSERT_EQ_INT(ctx, 11, chg_time);
    TEST_ASSERT_EQ_INT(ctx, 33, chg_cnt);

    chg_time = 11;
    chg_cnt = 22;

    fout_boss6(&chg_time, &chg_cnt);

    TEST_ASSERT_EQ_INT(ctx, 0, chg_time);
    TEST_ASSERT_EQ_INT(ctx, 33, chg_cnt);
    assert_palette(ctx, colorwk[32], 96, 96, 96, 1, "fout boss first");
    assert_palette(ctx, colorwk[63], 224, 224, 128, 1, "fout boss 63");

    chg_time = 11;
    chg_cnt = 77;

    fout_boss6(&chg_time, &chg_cnt);

    TEST_ASSERT_EQ_INT(ctx, 0, chg_time);
    TEST_ASSERT_EQ_INT(ctx, 77, chg_cnt);
    assert_palette(ctx, colorwk[32], 224, 224, 224, 1,
                   "fout boss clamped first");

    fset_boss6(44, &chg_cnt);
    TEST_ASSERT_EQ_INT(ctx, 44, chg_cnt);
    assert_palette(ctx, colorwk[32], 128, 128, 128, 1, "fset boss first");
    assert_palette(ctx, colorwk[50], 192, 192, 224, 1, "fset boss 50");
    assert_palette(ctx, colorwk[63], 224, 224, 160, 1, "fset boss 63");
}

static void test_flashin_boss6_and_fadeproc_defaults(test_context *ctx) {
    reset_col6d_state();
    colorwk[48] = (PALETTEENTRY){1, 2, 3, 4};

    flashin_boss6();

    TEST_ASSERT_EQ_INT(ctx, 0, startcolor);
    TEST_ASSERT_EQ_INT(ctx, 47, colorcnt);
    TEST_ASSERT_EQ_INT(ctx, 3, fade_flag_storage);
    assert_palette(ctx, colorwk[0], 224, 224, 224, 1, "boss flash first");
    assert_palette(ctx, colorwk[47], 224, 224, 224, 1, "boss flash last");
    assert_palette(ctx, colorwk[48], 1, 2, 3, 4, "boss flash beyond range");

    reset_col6d_state();
    fade_flag_storage = 1;
    TEST_ASSERT_EQ_INT(ctx, 0, FadeProc());

    reset_col6d_state();
    fade_flag_storage = 99;
    TEST_ASSERT_EQ_INT(ctx, 1, FadeProc());
}

/* The boss flash restores only the entries its fade-in covers: flashout()
   whitens 0-63, flashin_boss6() then aims the restore at 0-47 (colorcnt = 47)
   and flashin_new() re-whitens entry 32 on every step on purpose.  Entry 48 and
   the rest of the last palette line keep the flash's white; the only pieces that
   touch them again are the stopped background cycle, which blackens entry 49,
   and the boss's own colour animation.

   Entry 32 is the round's backdrop (FRAMEBUFFER_CLEAR_INDEX in
   src/frontend/graphics.cpp fills the framebuffer with it, which is what the
   reference's BackgroundBMPCreate() backdrop is), so it has to be inside both
   the flash range and the restore range - and the animation's last step,
   fset_boss6(0), paints it back to black.  See docs/r63d_white_background_bug.md. */
static void test_boss_flash_repaints_the_backdrop_entry(test_context *ctx) {
    int frame;
    Uint8 chg_cnt = 0;

    reset_col6d_state();
    colorset2(0); /* working palette: advacolor */
    colorset(0);  /* restore copy: advacolor */
    bossflag = 32; /* the boss is dead: the cycle runs with col6a3col_stop */

    flashout();
    TEST_ASSERT_EQ_INT(ctx, 4, fade_flag_storage);
    for (frame = 0; frame < 64 && FadeProc() == 0; ++frame) {
    }
    fade_flag_storage = 0;
    assert_palette(ctx, colorwk[32], 224, 224, 224, 1, "flash whitens 32");
    assert_palette(ctx, colorwk[48], 224, 224, 224, 1, "flash whitens 48");

    flashin_boss6();
    TEST_ASSERT_EQ_INT(ctx, 3, fade_flag_storage);
    for (frame = 0; frame < 64 && FadeProc() == 0; ++frame) {
    }
    fade_flag_storage = 0;

    /* 0-47 come back from the restore copy ... */
    assert_palette(ctx, colorwk[0], 0, 0, 0, 1, "restored entry 0");
    assert_palette(ctx, colorwk[33], 160, 192, 160, 1, "restored entry 33");
    /* ... except 32, which the fade-in keeps white on purpose ... */
    assert_palette(ctx, colorwk[32], 224, 224, 224, 1, "flashin keeps 32");
    /* ... and 48 is never restored at all. */
    assert_palette(ctx, colorwk[48], 224, 224, 224, 1, "flashin skips 48");

    /* The boss's own colour animation ends with the backdrop back to black. */
    fset_boss6(0, &chg_cnt);
    assert_palette(ctx, colorwk[32], 0, 0, 0, 1, "fset_boss6(0) backdrop");
    assert_palette(ctx, colorwk[48], 224, 224, 224, 1, "48 stays white");

    /* colchg_stop() parks the cycles for 128 frames, then the stopped cycle
       paints entry 49 black again. */
    memset(clchgtim, 128, sizeof(clchgtim));
    memset(clchgcnt, 0, sizeof(clchgcnt));
    for (frame = 0; frame < 200; ++frame) {
        clchgctr();
    }
    assert_palette(ctx, colorwk[49], 0, 0, 0, 1, "stopped cycle blackens 49");
}

TEST_MAIN_BEGIN;
test_col6d_tables_capture_palette_contract(&ctx);
test_clchg_sub0_waits_updates_and_wraps(&ctx);
test_colchg6a_updates_running_and_stopped_boss_cycles(&ctx);
test_fadein_stage_gate_is_reversed_from_col6c(&ctx);
test_fadeout_flashin_and_flashout_transitions(&ctx);
test_direct_fade_helpers_leave_matching_palette_entries_alone(&ctx);
test_colorset_routes_to_palette_work_buffers(&ctx);
test_boss_fade_helpers_step_and_sparse_slots(&ctx);
test_flashin_boss6_and_fadeproc_defaults(&ctx);
test_boss_flash_repaints_the_backdrop_entry(&ctx);
TEST_MAIN_END
