/* <stdlib.h> declares POSIX random(); the decompiled thanks headers declare
   the game's own Sint32 random(void).  Parse stdlib first, then rename the game
   symbol inside this harness so the two can coexist. */
#include <stdlib.h>

#define random game_random

#include <string.h>

#include "support/test_runner.h"
#include "src/types.h"

static Sint32 work_ram_clear_count;
static Sint32 mainloop_count;
static Sint32 vdpinit_count;
static Sint32 random_value;
static Sint32 random_call_count;
static Sint32 grid_call_count;
static Sint32 grid_plane[2300];
static Sint32 grid_x[2300];
static Sint32 grid_y[2300];
static Sint32 grid_tile[2300];
static Sint32 grid_flag[2300];
static Sint32 easprset_call_count;
static Sint16 easprset_x[8];
static Sint16 easprset_y[8];
static Uint16 easprset_index[8];
static Uint16 easprset_link[8];
static Uint16 easprset_rev[8];
static Sint32 wave_request_count;
static Sint16 wave_request_values[8];
static Sint32 open_file_return;
static Sint32 open_file_count;
static char open_file_names[4][64];
static Sint32 read_file_count;
static Sint32 close_file_count;
static Sint32 debug_string_count;
static char debug_strings[4][64];
static Sint32 memset_count;
static Sint32 memcpy_count;
static Sint32 clr_sprite_debug_count;

#include "src/title/thanks/game.c"

bmp_info SprBmp[700];
Sint32 *lpFadeFlag;
int_union hscroll_storage[8];
int_union *lphscrollbuff;
PALETTEENTRY palette0[96];
PALETTEENTRY palette1[96];
PALETTEENTRY palette2[96];
PALETTEENTRY palette3[96];
PALETTEENTRY *lpcolorwk0;
PALETTEENTRY *lpcolorwk1;
PALETTEENTRY *lpcolorwk2;
PALETTEENTRY *lpcolorwk3;
short_union swdata2;
short_union swdata1;
Sint16 end_timer;
Sint16 heart_tm;
Sint16 posetimer;
Sint16 colorno2;
Sint16 coltimer2;
Sint16 colorno;
Sint16 coltimer;
Uint16 lpbs_timer[32];
Uint8 quit_flg;
PALETTEENTRY good0[16];
PALETTEENTRY good1[16];
PALETTEENTRY *lpbs_good[32];
PALETTEENTRY asc0[8];
PALETTEENTRY asc1[8];
PALETTEENTRY *asc_coltbl[2];
Sint16 animal_timer;
Uint8 animal_flg;
sprite_status_thanks actwk[66];
static Uint16 glyph6[6] = {1, 2, 3, 4, 5, 6};
static Uint16 glyph8[8] = {10, 11, 12, 13, 14, 15, 16, 17};
Uint16 *asc_maptbl[38];

void WorkRamClear(void) { ++work_ram_clear_count; }

void mainloop(void) { ++mainloop_count; }

void vdpinit(void) { ++vdpinit_count; }

Sint32 random(void) {
    ++random_call_count;
    return random_value;
}

static Sint32 fake_set_grid(Sint32 plane, Sint32 x, Sint32 y, Sint32 tile,
                            Sint32 flag) {
    if (grid_call_count < 2300) {
        grid_plane[grid_call_count] = plane;
        grid_x[grid_call_count] = x;
        grid_y[grid_call_count] = y;
        grid_tile[grid_call_count] = tile;
        grid_flag[grid_call_count] = flag;
    }
    ++grid_call_count;
    return 0;
}

static void fake_easprset(Sint16 x, Sint16 y, Uint16 index, Uint16 link,
                          Uint16 rev) {
    if (easprset_call_count < 8) {
        easprset_x[easprset_call_count] = x;
        easprset_y[easprset_call_count] = y;
        easprset_index[easprset_call_count] = index;
        easprset_link[easprset_call_count] = link;
        easprset_rev[easprset_call_count] = rev;
    }
    ++easprset_call_count;
}

static void fake_clr_sprite_debug(void) { ++clr_sprite_debug_count; }

static void fake_wave_request(Sint16 value) {
    if (wave_request_count < 8) {
        wave_request_values[wave_request_count] = value;
    }
    ++wave_request_count;
}

static void fake_cdplay(Sint16 value) { (void)value; }

static void fake_change_tile_bmp(Sint32 a, Sint32 b) {
    (void)a;
    (void)b;
}

static void fake_memset(void *dst, Uint8 value, Sint32 size) {
    ++memset_count;
    memset(dst, value, (size_t)size);
}

static void fake_memcpy(void *dst, void *src, Sint32 size) {
    ++memcpy_count;
    memcpy(dst, src, (size_t)size);
}

static Sint32 fake_memcmp(void *lhs, void *rhs, Sint32 size) {
    return memcmp(lhs, rhs, (size_t)size);
}

static char *fake_strcpy(char *dst, char *src) { return strcpy(dst, src); }

static Sint32 fake_open_file(char *name) {
    if (open_file_count < 4) {
        strncpy(open_file_names[open_file_count], name,
                sizeof(open_file_names[0]) - 1);
    }
    ++open_file_count;
    return open_file_return;
}

static Sint32 fake_read_file(Sint32 handle, void *dst, Sint32 size) {
    Sint16 map[2048];

    ++read_file_count;
    memset(map, 0, sizeof(map));
    if (handle == 7) {
        map[0] = 1;
        map[1] = 286;
        map[2] = 288;
        map[64] = 1024;
    } else {
        map[0] = 155;
        map[1] = 482;
        map[2] = 154;
        map[3] = 483;
        map[64] = 1024;
    }
    memcpy(dst, map, (size_t)size);
    return size;
}

static void fake_close_file(Sint32 handle) {
    (void)handle;
    ++close_file_count;
}

static void fake_debug_string(char *message) {
    if (debug_string_count < 4) {
        strncpy(debug_strings[debug_string_count], message,
                sizeof(debug_strings[0]) - 1);
    }
    ++debug_string_count;
}

static Sint32 fake_random_func(void) { return 0; }
static void fake_printf(char *dst, char *src) {
    (void)dst;
    (void)src;
}

static void reset_palette(PALETTEENTRY *palette, Sint32 count, Uint8 seed) {
    for (Sint32 i = 0; i < count; ++i) {
        palette[i].peRed = (Uint8)(seed + i);
        palette[i].peGreen = (Uint8)(seed + i + 1);
        palette[i].peBlue = (Uint8)(seed + i + 2);
        palette[i].peFlags = 1;
    }
}

static void reset_fixture(void) {
    memset(SprBmp, 0, sizeof(SprBmp));
    memset(hscroll_storage, 0, sizeof(hscroll_storage));
    memset(palette0, 0, sizeof(palette0));
    memset(palette1, 0, sizeof(palette1));
    memset(palette2, 0, sizeof(palette2));
    memset(palette3, 0, sizeof(palette3));
    memset(lpbs_timer, 0, sizeof(lpbs_timer));
    memset(good0, 0, sizeof(good0));
    memset(good1, 0, sizeof(good1));
    memset(lpbs_good, 0, sizeof(lpbs_good));
    memset(asc0, 0, sizeof(asc0));
    memset(asc1, 0, sizeof(asc1));
    memset(asc_coltbl, 0, sizeof(asc_coltbl));
    memset(actwk, 0, sizeof(actwk));
    memset(asc_maptbl, 0, sizeof(asc_maptbl));
    memset(grid_plane, 0, sizeof(grid_plane));
    memset(grid_x, 0, sizeof(grid_x));
    memset(grid_y, 0, sizeof(grid_y));
    memset(grid_tile, 0, sizeof(grid_tile));
    memset(grid_flag, 0, sizeof(grid_flag));
    memset(easprset_x, 0, sizeof(easprset_x));
    memset(easprset_y, 0, sizeof(easprset_y));
    memset(easprset_index, 0, sizeof(easprset_index));
    memset(easprset_link, 0, sizeof(easprset_link));
    memset(easprset_rev, 0, sizeof(easprset_rev));
    memset(wave_request_values, 0, sizeof(wave_request_values));
    memset(open_file_names, 0, sizeof(open_file_names));
    memset(debug_strings, 0, sizeof(debug_strings));

    lpcolorwk0 = palette0;
    lpcolorwk1 = palette1;
    lpcolorwk2 = palette2;
    lpcolorwk3 = palette3;
    lphscrollbuff = hscroll_storage;
    for (Sint32 i = 0; i < 32; ++i) {
        lpbs_timer[i] = (Uint16)(10 + i);
        lpbs_good[i] = good0;
    }
    lpbs_good[1] = good1;
    asc_coltbl[0] = asc0;
    asc_coltbl[1] = asc1;
    asc_maptbl[1] = glyph6;
    asc_maptbl[27] = glyph8;
    reset_palette(good0, 16, 30);
    reset_palette(good1, 16, 60);
    reset_palette(asc0, 8, 90);
    reset_palette(asc1, 8, 120);

    SetGrid = fake_set_grid;
    EAsprset = fake_easprset;
    ClrSpriteDebug = fake_clr_sprite_debug;
    WaveRequest = fake_wave_request;
    CDPlay = fake_cdplay;
    ChangeTileBmp = fake_change_tile_bmp;
    sMemSet = fake_memset;
    sMemCpy = fake_memcpy;
    sMemCmp = fake_memcmp;
    sRandom = fake_random_func;
    sStrcpy = fake_strcpy;
    sPrintf = fake_printf;
    sOutputDebugString = fake_debug_string;
    sOpenFile = fake_open_file;
    sReadFile = fake_read_file;
    sCloseFile = fake_close_file;

    swdata1.w = 0;
    swdata2.w = 0;
    end_timer = 0;
    heart_tm = 0;
    posetimer = 0;
    colorno2 = 0;
    coltimer2 = 0;
    colorno = 0;
    coltimer = 0;
    quit_flg = 0;
    animal_timer = 0;
    animal_flg = 0;
    random_value = 0;
    random_call_count = 0;
    grid_call_count = 0;
    easprset_call_count = 0;
    wave_request_count = 0;
    open_file_return = -1;
    open_file_count = 0;
    read_file_count = 0;
    close_file_count = 0;
    debug_string_count = 0;
    memset_count = 0;
    memcpy_count = 0;
    clr_sprite_debug_count = 0;
    work_ram_clear_count = 0;
    mainloop_count = 0;
    vdpinit_count = 0;
    gEndingMesTimer = 0;
}

static void assert_palette_entry(test_context *ctx, PALETTEENTRY actual,
                                 PALETTEENTRY expected) {
    TEST_ASSERT_EQ_INT(ctx, expected.peRed, actual.peRed);
    TEST_ASSERT_EQ_INT(ctx, expected.peGreen, actual.peGreen);
    TEST_ASSERT_EQ_INT(ctx, expected.peBlue, actual.peBlue);
    TEST_ASSERT_EQ_INT(ctx, expected.peFlags, actual.peFlags);
}

static void test_dll_meminit_assigns_callbacks_and_buffers(test_context *ctx) {
    game_info keep_work;
    Uint32 surf = 0;
    Uint32 hwnd = 0;
    score_data scores;
    Sint32 fade_flag = 0;
    char *color0_ptr = (char *)palette0;
    char *color1_ptr = (char *)palette1;
    char *color2_ptr = (char *)palette2;
    char *color3_ptr = (char *)palette3;
    char *hscroll_ptr = (char *)hscroll_storage;
    char **bufs[12];
    void *funcs[28];

    reset_fixture();
    memset(&keep_work, 0, sizeof(keep_work));
    memset(funcs, 0, sizeof(funcs));
    bufs[0] = 0;
    bufs[1] = &color0_ptr;
    bufs[2] = &color1_ptr;
    bufs[3] = &color2_ptr;
    bufs[4] = &color3_ptr;
    bufs[5] = &hscroll_ptr;
    bufs[6] = (char **)&fade_flag;
    bufs[7] = (char **)&keep_work;
    bufs[8] = (char **)&surf;
    bufs[9] = (char **)&hwnd;
    bufs[10] = (char **)&scores;
    funcs[0] = fake_set_grid;
    funcs[1] = fake_easprset;
    funcs[2] = fake_clr_sprite_debug;
    funcs[3] = fake_wave_request;
    funcs[4] = fake_cdplay;
    funcs[6] = fake_change_tile_bmp;
    funcs[13] = fake_memset;
    funcs[14] = fake_memcpy;
    funcs[15] = fake_memcmp;
    funcs[16] = fake_random_func;
    funcs[17] = fake_strcpy;
    funcs[20] = fake_printf;
    funcs[21] = fake_debug_string;
    funcs[22] = fake_open_file;
    funcs[23] = fake_read_file;
    funcs[24] = fake_close_file;

    DLL_meminit(bufs, funcs);

    TEST_ASSERT_TRUE(ctx, SetGrid == fake_set_grid);
    TEST_ASSERT_TRUE(ctx, EAsprset == fake_easprset);
    TEST_ASSERT_TRUE(ctx, lpcolorwk0 == palette0);
    TEST_ASSERT_TRUE(ctx, lpcolorwk1 == palette1);
    TEST_ASSERT_TRUE(ctx, lpcolorwk2 == palette2);
    TEST_ASSERT_TRUE(ctx, lpcolorwk3 == palette3);
    TEST_ASSERT_TRUE(ctx, lphscrollbuff == hscroll_storage);
    TEST_ASSERT_TRUE(ctx, lpFadeFlag == &fade_flag);
    TEST_ASSERT_TRUE(ctx, lpKeepWork == &keep_work);
    TEST_ASSERT_TRUE(ctx, lphSurf == &surf);
    TEST_ASSERT_TRUE(ctx, lpghWnd == &hwnd);
    TEST_ASSERT_TRUE(ctx, lpScorData == &scores);
    TEST_ASSERT_TRUE(ctx, keep_work.pSprBmp == SprBmp);
}

static void test_swdata_set_copies_both_inputs(test_context *ctx) {
    ushort_union sw1;
    ushort_union sw2;

    reset_fixture();
    sw1.w = 0x1234;
    sw2.w = 0xabcd;

    SWdataSet(sw1, sw2);

    TEST_ASSERT_EQ_INT(ctx, 0x1234, swdata1.w);
    TEST_ASSERT_EQ_INT(ctx, -21555, swdata2.w);
}

static void test_dll_memfree_is_callable_noop(test_context *ctx) {
    reset_fixture();

    DLL_memfree();

    TEST_ASSERT_EQ_INT(ctx, 0, work_ram_clear_count);
    TEST_ASSERT_EQ_INT(ctx, 0, grid_call_count);
}

static void test_game_init_sets_timers_palette_and_maps(test_context *ctx) {
    reset_fixture();
    open_file_return = -1;

    game_init();

    TEST_ASSERT_EQ_INT(ctx, 1, work_ram_clear_count);
    TEST_ASSERT_EQ_INT(ctx, -4096, swdata1.w);
    TEST_ASSERT_EQ_INT(ctx, 1120 * 2, grid_call_count);
    TEST_ASSERT_EQ_INT(ctx, 2, open_file_count);
    TEST_ASSERT_EQ_INT(ctx, 2, debug_string_count);
    TEST_ASSERT_EQ_INT(ctx, lpbs_timer[0], coltimer);
    TEST_ASSERT_EQ_INT(ctx, 0, colorno);
    TEST_ASSERT_EQ_INT(ctx, 20, coltimer2);
    TEST_ASSERT_EQ_INT(ctx, 1, colorno2);
    TEST_ASSERT_EQ_INT(ctx, 1536, posetimer);
    TEST_ASSERT_EQ_INT(ctx, 1, heart_tm);
    TEST_ASSERT_EQ_INT(ctx, 10800, end_timer);
    TEST_ASSERT_EQ_INT(ctx, 224, palette0[3].peBlue);
}

static void test_cgmwrt_a_reads_file_and_converts_tile_ranges(
    test_context *ctx) {
    reset_fixture();
    open_file_return = 7;

    cgmwrt_a();

    TEST_ASSERT_EQ_INT(ctx, 1120, grid_call_count);
    TEST_ASSERT_EQ_INT(ctx, 1, open_file_count);
    TEST_ASSERT_EQ_INT(ctx, 1, read_file_count);
    TEST_ASSERT_EQ_INT(ctx, 1, close_file_count);
    TEST_ASSERT_EQ_INT(ctx, 175, grid_tile[0]);
    TEST_ASSERT_EQ_INT(ctx, 460, grid_tile[1]);
    TEST_ASSERT_EQ_INT(ctx, 0, grid_tile[2]);
    TEST_ASSERT_EQ_INT(ctx, 1024, grid_tile[40]);
}

static void test_cgmwrt_b_reads_file_and_converts_tile_ranges(
    test_context *ctx) {
    reset_fixture();
    open_file_return = 8;

    cgmwrt_b();

    TEST_ASSERT_EQ_INT(ctx, 1120, grid_call_count);
    TEST_ASSERT_EQ_INT(ctx, 1, open_file_count);
    TEST_ASSERT_EQ_INT(ctx, 1, read_file_count);
    TEST_ASSERT_EQ_INT(ctx, 1, close_file_count);
    TEST_ASSERT_EQ_INT(ctx, 1, grid_tile[0]);
    TEST_ASSERT_EQ_INT(ctx, 328, grid_tile[1]);
    TEST_ASSERT_EQ_INT(ctx, 0, grid_tile[2]);
    TEST_ASSERT_EQ_INT(ctx, 0, grid_tile[3]);
    TEST_ASSERT_EQ_INT(ctx, 1024, grid_tile[40]);
}

static void test_v_int_updates_animal_timers_palettes_and_quit(
    test_context *ctx) {
    reset_fixture();
    animal_flg = 2;
    animal_timer = 1;
    posetimer = 1;
    actwk[0].ACT_NO = 1;
    random_value = 7;
    coltimer = 1;
    colorno = 32;
    coltimer2 = 1;
    colorno2 = 2;
    end_timer = 1;

    v_int();

    TEST_ASSERT_EQ_INT(ctx, 1, animal_flg);
    TEST_ASSERT_EQ_INT(ctx, 900, animal_timer);
    TEST_ASSERT_EQ_INT(ctx, 1, random_call_count);
    TEST_ASSERT_EQ_INT(ctx, 8199, posetimer);
    TEST_ASSERT_EQ_INT(ctx, 10, coltimer);
    TEST_ASSERT_EQ_INT(ctx, 1, colorno);
    assert_palette_entry(ctx, palette0[16], good0[0]);
    TEST_ASSERT_EQ_INT(ctx, 20, coltimer2);
    TEST_ASSERT_EQ_INT(ctx, 1, colorno2);
    assert_palette_entry(ctx, palette0[40], asc0[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, quit_flg);
}

static void test_put_ascii_draws_three_or_four_cell_glyphs(test_context *ctx) {
    reset_fixture();

    PutAscii(1, 5, 6);

    TEST_ASSERT_EQ_INT(ctx, 6, grid_call_count);
    TEST_ASSERT_EQ_INT(ctx, 5, grid_x[0]);
    TEST_ASSERT_EQ_INT(ctx, 6, grid_y[0]);
    TEST_ASSERT_EQ_INT(ctx, 33098, grid_tile[0]);
    TEST_ASSERT_EQ_INT(ctx, 6, grid_x[1]);
    TEST_ASSERT_EQ_INT(ctx, 8, grid_y[5]);
    TEST_ASSERT_EQ_INT(ctx, 33103, grid_tile[5]);

    reset_fixture();
    PutAscii(27, 9, 10);

    TEST_ASSERT_EQ_INT(ctx, 8, grid_call_count);
    TEST_ASSERT_EQ_INT(ctx, 33107, grid_tile[0]);
    TEST_ASSERT_EQ_INT(ctx, 33114, grid_tile[7]);
}

static void test_soundset_skips_171_and_maps_other_requests(
    test_context *ctx) {
    reset_fixture();

    soundset(171);
    soundset(144);
    soundset(156);

    TEST_ASSERT_EQ_INT(ctx, 2, wave_request_count);
    TEST_ASSERT_EQ_INT(ctx, 0, wave_request_values[0]);
    TEST_ASSERT_EQ_INT(ctx, 11, wave_request_values[1]);
}

static void test_ending_mes_color_copies_literal_palette(test_context *ctx) {
    reset_fixture();
    memset(palette0, 0xaa, sizeof(palette0));

    EndingMesColor();

    TEST_ASSERT_EQ_INT(ctx, 0, palette0[0].peRed);
    TEST_ASSERT_EQ_INT(ctx, 224, palette0[3].peBlue);
    TEST_ASSERT_EQ_INT(ctx, 224, palette0[6].peRed);
    TEST_ASSERT_EQ_INT(ctx, 224, palette0[15].peRed);
    TEST_ASSERT_EQ_INT(ctx, 0, palette0[63].peBlue);
}

static void test_ending_mes_type_one_goes_straight_to_presented_by_sega(
    test_context *ctx) {
    game_info keep_work;

    reset_fixture();
    memset(&keep_work, 0, sizeof(keep_work));
    lpKeepWork = &keep_work;
    keep_work.EndingMesType = 1;

    /* The port starts on "PRESENTED BY SEGA" (sprite 77): the "YOU'RE TOO
       COOL!" line (sprite 76) is already in the ending cutscene, so the ticks
       that would draw it are skipped. */
    TEST_ASSERT_EQ_INT(ctx, 1, EndingMes());
    TEST_ASSERT_EQ_INT(ctx, 1, easprset_call_count);
    TEST_ASSERT_EQ_INT(ctx, 77, easprset_index[0]);
    TEST_ASSERT_EQ_INT(ctx, 180, gEndingMesTimer);

    gEndingMesTimer = 359;
    swdata1.b.l = 0;
    EndingMes();
    TEST_ASSERT_EQ_INT(ctx, 359, gEndingMesTimer);
    TEST_ASSERT_EQ_INT(ctx, 0, vdpinit_count);

    swdata1.b.l = 0x10;
    EndingMes();
    TEST_ASSERT_EQ_INT(ctx, 360, gEndingMesTimer);
    TEST_ASSERT_EQ_INT(ctx, 1, vdpinit_count);

    TEST_ASSERT_EQ_INT(ctx, 0, EndingMes());
}

static void test_ending_mes_type_two_blacks_palette_on_input(
    test_context *ctx) {
    game_info keep_work;

    reset_fixture();
    reset_palette(palette0, 64, 40);
    memset(&keep_work, 0, sizeof(keep_work));
    lpKeepWork = &keep_work;
    keep_work.EndingMesType = 2;
    gEndingMesTimer = 359;
    swdata1.b.l = 0x80;

    EndingMes();

    TEST_ASSERT_EQ_INT(ctx, 0, palette0[0].peRed);
    TEST_ASSERT_EQ_INT(ctx, 0, palette0[63].peGreen);
    TEST_ASSERT_EQ_INT(ctx, 0, palette0[63].peBlue);
}

static void test_ending_mes_type_two_sprites_and_wait_without_input(
    test_context *ctx) {
    game_info keep_work;

    reset_fixture();
    memset(&keep_work, 0, sizeof(keep_work));
    lpKeepWork = &keep_work;
    keep_work.EndingMesType = 2;

    /* Type 2 starts on "PRESENTED BY SEGA" as well - "TRY AGAIN" (sprite 75)
       is part of the cutscene the port plays. */
    TEST_ASSERT_EQ_INT(ctx, 1, EndingMes());
    TEST_ASSERT_EQ_INT(ctx, 1, easprset_call_count);
    TEST_ASSERT_EQ_INT(ctx, 77, easprset_index[0]);

    gEndingMesTimer = 359;
    swdata1.b.l = 0;
    EndingMes();
    TEST_ASSERT_EQ_INT(ctx, 359, gEndingMesTimer);
}

static void test_game_return_paths(test_context *ctx) {
    game_info keep_work;

    reset_fixture();
    memset(&keep_work, 0, sizeof(keep_work));
    lpKeepWork = &keep_work;
    gEndingMesTimer = 360;
    end_timer = 2;
    posetimer = 2;
    coltimer = 2;
    coltimer2 = 2;

    TEST_ASSERT_EQ_INT(ctx, 0, game());
    TEST_ASSERT_EQ_INT(ctx, 1, mainloop_count);

    reset_fixture();
    lpKeepWork = &keep_work;
    keep_work.EndingMesType = 2;
    gEndingMesTimer = 360;
    TEST_ASSERT_EQ_INT(ctx, -1, game());

    reset_fixture();
    lpKeepWork = &keep_work;
    keep_work.EndingMesType = 0;
    gEndingMesTimer = 360;
    quit_flg = 1;
    TEST_ASSERT_EQ_INT(ctx, -1, game());

    reset_fixture();
    lpKeepWork = &keep_work;
    keep_work.EndingMesType = 0;
    gEndingMesTimer = 0;
    TEST_ASSERT_EQ_INT(ctx, 0, game());
    TEST_ASSERT_EQ_INT(ctx, 0, mainloop_count);
}

TEST_MAIN_BEGIN;
test_dll_meminit_assigns_callbacks_and_buffers(&ctx);
test_swdata_set_copies_both_inputs(&ctx);
test_dll_memfree_is_callable_noop(&ctx);
test_game_init_sets_timers_palette_and_maps(&ctx);
test_cgmwrt_a_reads_file_and_converts_tile_ranges(&ctx);
test_cgmwrt_b_reads_file_and_converts_tile_ranges(&ctx);
test_v_int_updates_animal_timers_palettes_and_quit(&ctx);
test_put_ascii_draws_three_or_four_cell_glyphs(&ctx);
test_soundset_skips_171_and_maps_other_requests(&ctx);
test_ending_mes_color_copies_literal_palette(&ctx);
test_ending_mes_type_one_goes_straight_to_presented_by_sega(&ctx);
test_ending_mes_type_two_blacks_palette_on_input(&ctx);
test_ending_mes_type_two_sprites_and_wait_without_input(&ctx);
test_game_return_paths(&ctx);
TEST_MAIN_END
