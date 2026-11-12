/* <stdlib.h> declares POSIX random(); the decompiled src/io.h declares the
   game's own Sint32 random(void). Parse stdlib first, then rename the game
   symbol inside this harness so the two can coexist. */
#include <stdlib.h>

#define random game_random

#include <string.h>

#include "support/test_runner.h"
#include "src/types.h"

Uint8 cgchgcnt[6];
Uint8 cgchgtim[6];
ushort_union pauseflag;
int_union scra_h_posit;
int_union scra_v_posit;

typedef struct tile_call {
    Sint32 tile_start;
    Sint32 bmp_no;
} tile_call;

static tile_call tile_calls[64];
static int tile_call_count;

static void record_change_tile_bmp(Sint32 tile_start, Sint32 bmp_no) {
    tile_calls[tile_call_count].tile_start = tile_start;
    tile_calls[tile_call_count].bmp_no = bmp_no;
    ++tile_call_count;
}

void (*ChangeTileBmp)(Sint32, Sint32) = record_change_tile_bmp;

#include "src/r4/dev41a.c"

static void reset_dev41a_state(void) {
    memset(cgchgcnt, 0, sizeof(cgchgcnt));
    memset(cgchgtim, 0, sizeof(cgchgtim));
    memset(&pauseflag, 0, sizeof(pauseflag));
    memset(&scra_h_posit, 0, sizeof(scra_h_posit));
    memset(&scra_v_posit, 0, sizeof(scra_v_posit));
    memset(tile_calls, 0, sizeof(tile_calls));
    tile_call_count = 0;
    ChangeTileBmp = record_change_tile_bmp;
}

static void assert_map_init_data(test_context *ctx) {
    TEST_ASSERT_EQ_INT(ctx, 0, mapinittbl.CGdata);
    TEST_ASSERT_EQ_INT(ctx, 3, mapinittbl.iniCGno1);
    TEST_ASSERT_EQ_INT(ctx, 2, mapinittbl.stCGno1);
    TEST_ASSERT_EQ_INT(ctx, 4, mapinittbl.colorno1);
    TEST_ASSERT_EQ_INT(ctx, 4, mapinittbl.colorno2);
    TEST_ASSERT_EQ_INT(ctx, 129, mapinittbl.soundNo);
}

static void test_tables_capture_fire_tile_contract(test_context *ctx) {
    TEST_ASSERT_EQ_INT(ctx, 24, tbl_fir0.chgcnt);
    TEST_ASSERT_EQ_INT(ctx, 558, tbl_fir0.TileStart);
    TEST_ASSERT_EQ_INT(ctx, 24, tbl_fir1.chgcnt);
    TEST_ASSERT_EQ_INT(ctx, 574, tbl_fir1.TileStart);
    TEST_ASSERT_EQ_INT(ctx, 24, tbl_fir2.chgcnt);
    TEST_ASSERT_EQ_INT(ctx, 590, tbl_fir2.TileStart);
    TEST_ASSERT_EQ_INT(ctx, 0, fire0cg[0]);
    TEST_ASSERT_EQ_INT(ctx, 15, fire0cg[15]);
    TEST_ASSERT_EQ_INT(ctx, 16, fire1cg[0]);
    TEST_ASSERT_EQ_INT(ctx, 31, fire1cg[15]);
    TEST_ASSERT_EQ_INT(ctx, 112, fire7cg[0]);
    TEST_ASSERT_EQ_INT(ctx, 127, fire7cg[15]);
}

static void test_cg_chg1_waits_advances_and_wraps(test_context *ctx) {
    Uint8 *tim;
    Uint8 *cnt;
    Sint32 bmp_no[16] = {-1};
    Sint32 tile_start = -1;
    int i;

    reset_dev41a_state();
    cgchgtim[0] = 2;
    cgchgcnt[0] = 5;
    tim = cgchgtim;
    cnt = cgchgcnt;

    TEST_ASSERT_EQ_INT(ctx, 0,
                       cg_chg1(&tbl_fir0, 15, &tim, &cnt, bmp_no,
                               &tile_start));
    TEST_ASSERT_EQ_INT(ctx, 1, cgchgtim[0]);
    TEST_ASSERT_EQ_INT(ctx, 5, cgchgcnt[0]);
    TEST_ASSERT_TRUE(ctx, tim == &cgchgtim[1]);
    TEST_ASSERT_TRUE(ctx, cnt == &cgchgcnt[1]);
    TEST_ASSERT_EQ_INT(ctx, -1, bmp_no[0]);
    TEST_ASSERT_EQ_INT(ctx, -1, tile_start);

    reset_dev41a_state();
    cgchgtim[0] = 0;
    cgchgcnt[0] = 0;
    tim = cgchgtim;
    cnt = cgchgcnt;

    TEST_ASSERT_EQ_INT(ctx, 1,
                       cg_chg1(&tbl_fir0, 15, &tim, &cnt, bmp_no,
                               &tile_start));
    TEST_ASSERT_EQ_INT(ctx, 8, cgchgtim[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, cgchgcnt[0]);
    TEST_ASSERT_EQ_INT(ctx, 558, tile_start);
    for (i = 0; i < 16; ++i) {
        TEST_ASSERT_EQ_INT(ctx, 16 + i, bmp_no[i]);
    }

    reset_dev41a_state();
    cgchgtim[0] = 0;
    cgchgcnt[0] = 23;
    tim = cgchgtim;
    cnt = cgchgcnt;

    TEST_ASSERT_EQ_INT(ctx, 1,
                       cg_chg1(&tbl_fir2, 15, &tim, &cnt, bmp_no,
                               &tile_start));
    TEST_ASSERT_EQ_INT(ctx, 60, cgchgtim[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, cgchgcnt[0]);
    TEST_ASSERT_EQ_INT(ctx, 590, tile_start);
    for (i = 0; i < 16; ++i) {
        TEST_ASSERT_EQ_INT(ctx, i, bmp_no[i]);
    }
}

static void test_cg_change_returns_when_paused_or_out_of_range(
    test_context *ctx) {
    reset_dev41a_state();
    pauseflag.b.h = 1;
    cgchgtim[0] = 7;

    cg_change();

    TEST_ASSERT_EQ_INT(ctx, 0, tile_call_count);
    TEST_ASSERT_EQ_INT(ctx, 7, cgchgtim[0]);

    reset_dev41a_state();
    scra_h_posit.w.h = 640;
    cgchgtim[0] = 8;

    cg_change();

    TEST_ASSERT_EQ_INT(ctx, 0, tile_call_count);
    TEST_ASSERT_EQ_INT(ctx, 8, cgchgtim[0]);

    reset_dev41a_state();
    scra_v_posit.w.h = 352;
    cgchgtim[0] = 9;

    cg_change();

    TEST_ASSERT_EQ_INT(ctx, 0, tile_call_count);
    TEST_ASSERT_EQ_INT(ctx, 9, cgchgtim[0]);
}

static void test_cg_change_wait_path_makes_no_tile_calls(test_context *ctx) {
    reset_dev41a_state();
    cgchgtim[0] = 2;
    cgchgcnt[0] = 1;
    cgchgtim[1] = 3;
    cgchgcnt[1] = 2;
    cgchgtim[2] = 4;
    cgchgcnt[2] = 3;

    cg_change();

    TEST_ASSERT_EQ_INT(ctx, 0, tile_call_count);
    TEST_ASSERT_EQ_INT(ctx, 1, cgchgtim[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, cgchgcnt[0]);
    TEST_ASSERT_EQ_INT(ctx, 2, cgchgtim[1]);
    TEST_ASSERT_EQ_INT(ctx, 2, cgchgcnt[1]);
    TEST_ASSERT_EQ_INT(ctx, 3, cgchgtim[2]);
    TEST_ASSERT_EQ_INT(ctx, 3, cgchgcnt[2]);
}

static void test_cg_change_emits_three_fire_sequences(test_context *ctx) {
    int i;

    reset_dev41a_state();
    cgchgtim[0] = 0;
    cgchgcnt[0] = 0;
    cgchgtim[1] = 0;
    cgchgcnt[1] = 0;
    cgchgtim[2] = 0;
    cgchgcnt[2] = 0;

    cg_change();

    TEST_ASSERT_EQ_INT(ctx, 48, tile_call_count);
    TEST_ASSERT_EQ_INT(ctx, 8, cgchgtim[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, cgchgcnt[0]);
    TEST_ASSERT_EQ_INT(ctx, 8, cgchgtim[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, cgchgcnt[1]);
    TEST_ASSERT_EQ_INT(ctx, 8, cgchgtim[2]);
    TEST_ASSERT_EQ_INT(ctx, 1, cgchgcnt[2]);
    for (i = 0; i < 16; ++i) {
        TEST_ASSERT_EQ_INT(ctx, 558 + i, tile_calls[i].tile_start);
        TEST_ASSERT_EQ_INT(ctx, 16 + i, tile_calls[i].bmp_no);
        TEST_ASSERT_EQ_INT(ctx, 574 + i, tile_calls[16 + i].tile_start);
        TEST_ASSERT_EQ_INT(ctx, 16 + i, tile_calls[16 + i].bmp_no);
        TEST_ASSERT_EQ_INT(ctx, 590 + i, tile_calls[32 + i].tile_start);
        TEST_ASSERT_EQ_INT(ctx, i, tile_calls[32 + i].bmp_no);
    }
}

TEST_MAIN_BEGIN;
assert_map_init_data(&ctx);
test_tables_capture_fire_tile_contract(&ctx);
test_cg_chg1_waits_advances_and_wraps(&ctx);
test_cg_change_returns_when_paused_or_out_of_range(&ctx);
test_cg_change_wait_path_makes_no_tile_calls(&ctx);
test_cg_change_emits_three_fire_sequences(&ctx);
TEST_MAIN_END
