/* <stdlib.h> declares POSIX random(); the decompiled title/planet headers
   declare the game's own Sint32 random(void).  Parse stdlib first, then rename
   the game symbol inside this harness so the two can coexist. */
#include <stdlib.h>

#define random game_random

#include <string.h>

#include "support/test_runner.h"
#include "types.h"

#include "src/title/planet/lplmain.c"

typedef struct {
    Sint32 bmp;
    Sint32 x;
    Sint32 y;
    Sint32 tile;
    Sint32 flags;
} grid_call;

static grid_call grid_calls[2048];
static int grid_call_count;
static int close_calls;
static int cdplay_calls;
static Sint16 cdplay_args[16];
static int fadein_calls;
static int fadeout_calls;
static int flashin_calls;
static int flashout_calls;
static int action_calls;
static int sprascii_calls;
static int set_asciicg_calls;
static int kaiten0_calls;
static int kaiten_init_calls;
static int player_calls;
static int memset_calls;
static Sint32 memset_sizes[8];
static int memfree_calls;
static void *freed_ptrs[8];
static Uint8 allocation0[0x40000];
static Uint16 sm_adr_storage[32768 / sizeof(Uint16)];
static int open_file_result;
static int short_read_after;
static int read_call_count;
static Sint16 next_read_value;

static PALETTEENTRY color0[128];
static PALETTEENTRY color1[128];
static PALETTEENTRY color2[128];
static PALETTEENTRY color3[128];
static int_union hscrollbuf[64];
static Sint32 fade_flag_storage;
static game_info keep_work;
static char *buffer_table[8];
static char **buffer_table_ptrs[8];

static PALETTEENTRY planet_a[32][16];
static PALETTEENTRY planet_b[32][16];
static PALETTEENTRY planet_c[32][16];
static PALETTEENTRY planet_d[32][16];
static PALETTEENTRY *planet_group0[32];
static PALETTEENTRY *planet_group1[32];
static PALETTEENTRY *planet_group2[32];
static PALETTEENTRY *planet_group3[32];
static PALETTEENTRY *planet_group4[32];
static PALETTEENTRY *planet_group5[32];
static Sint16 funka_map0[4];
static Sint16 funka_map1[4];
static Sint16 funka_map2[4];

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

PALETTEENTRY init_col[64];
PALETTEENTRY ascii_col[16];
PALETTEENTRY sprite_col1[16];
PALETTEENTRY sprite_col2[16];
PALETTEENTRY **lp_coltbl[6];
Sint16 lpbs_timer[32];
PALETTEENTRY *sprite_color[2];
Sint16 *funka_map_tbl[12];
Sint16 funka_timetbl[12];

static Sint32 stub_set_grid(Sint32 bmp, Sint32 x, Sint32 y, Sint32 tile,
                            Sint32 flags) {
    if (grid_call_count < (int)(sizeof(grid_calls) / sizeof(grid_calls[0]))) {
        grid_calls[grid_call_count].bmp = bmp;
        grid_calls[grid_call_count].x = x;
        grid_calls[grid_call_count].y = y;
        grid_calls[grid_call_count].tile = tile;
        grid_calls[grid_call_count].flags = flags;
    }
    ++grid_call_count;
    return 0;
}

static void stub_easprset(Sint16 x, Sint16 y, Uint16 index, Uint16 link,
                          Uint16 flags) {
    (void)x;
    (void)y;
    (void)index;
    (void)link;
    (void)flags;
}

static void stub_clr_sprite_debug(void) {}
static void stub_wave_request(Sint16 wave) { (void)wave; }
static void stub_wave_all_stop(void) {}
static void stub_change_tile_bmp(Sint32 a, Sint32 b) {
    (void)a;
    (void)b;
}
static void stub_output_debug(char *text) { (void)text; }
static void stub_printf(char *dst, char *fmt) {
    (void)dst;
    (void)fmt;
}
static Sint32 stub_random(void) { return 0; }

static void stub_cdplay(Sint16 track) {
    if (cdplay_calls < (int)(sizeof(cdplay_args) / sizeof(cdplay_args[0])))
        cdplay_args[cdplay_calls] = track;
    ++cdplay_calls;
}

static void *stub_mem_alloc(Sint32 size) {
    if (size == 0x40000)
        return allocation0;
    if (size == 32768)
        return sm_adr_storage;
    return 0;
}

static void stub_mem_free(void *ptr) {
    if (memfree_calls < (int)(sizeof(freed_ptrs) / sizeof(freed_ptrs[0])))
        freed_ptrs[memfree_calls] = ptr;
    ++memfree_calls;
}

static void stub_mem_set(void *dst, Uint8 value, Sint32 size) {
    if (memset_calls < (int)(sizeof(memset_sizes) / sizeof(memset_sizes[0])))
        memset_sizes[memset_calls] = size;
    ++memset_calls;
    memset(dst, value, (size_t)size);
}

static void stub_mem_cpy(void *dst, void *src, Sint32 size) {
    memcpy(dst, src, (size_t)size);
}

static Sint32 stub_open_file(char *path) {
    (void)path;
    read_call_count = 0;
    return open_file_result;
}

static Sint32 stub_read_file(Sint32 handle, void *dst, Sint32 size) {
    (void)handle;
    ++read_call_count;
    if (short_read_after > 0 && read_call_count >= short_read_after)
        return size - 1;
    if (size == (Sint32)sizeof(Sint16)) {
        *(Sint16 *)dst = next_read_value++;
        return size;
    }
    memset(dst, 0x5A, (size_t)size);
    return size;
}

static void stub_close_file(Sint32 handle) {
    (void)handle;
    ++close_calls;
}

void fadein(void) { ++fadein_calls; }
void fadeout(void) { ++fadeout_calls; }
void flashin(void) { ++flashin_calls; }
void flashout(void) { ++flashout_calls; }
Sint32 FadeProc(void) { return 0; }
void WorkRamClear(void) {}
void action(void) { ++action_calls; }
void sprascii(void) { ++sprascii_calls; }
void set_asciicg(void) { ++set_asciicg_calls; }
void kaiten0(void) { ++kaiten0_calls; }
void kaiten_init(void) { ++kaiten_init_calls; }
void player(void) { ++player_calls; }

static void init_palette_source(PALETTEENTRY colors[32][16], int base) {
    for (int row = 0; row < 32; ++row) {
        for (int col = 0; col < 16; ++col) {
            colors[row][col].peRed = (Uint8)(base + row + col);
            colors[row][col].peGreen = (Uint8)(base + row + col + 1);
            colors[row][col].peBlue = (Uint8)(base + row + col + 2);
            colors[row][col].peFlags = 1;
        }
    }
}

static void init_environment(void) {
    memset(grid_calls, 0, sizeof(grid_calls));
    memset(color0, 0, sizeof(color0));
    memset(color1, 0, sizeof(color1));
    memset(color2, 0, sizeof(color2));
    memset(color3, 0, sizeof(color3));
    memset(hscrollbuf, 0, sizeof(hscrollbuf));
    memset(&keep_work, 0, sizeof(keep_work));
    memset(sm_adr_storage, 0, sizeof(sm_adr_storage));
    memset(allocation0, 0, sizeof(allocation0));
    memset(freed_ptrs, 0, sizeof(freed_ptrs));
    memset(memset_sizes, 0, sizeof(memset_sizes));

    grid_call_count = 0;
    close_calls = 0;
    cdplay_calls = 0;
    fadein_calls = 0;
    fadeout_calls = 0;
    flashin_calls = 0;
    flashout_calls = 0;
    action_calls = 0;
    sprascii_calls = 0;
    set_asciicg_calls = 0;
    kaiten0_calls = 0;
    kaiten_init_calls = 0;
    player_calls = 0;
    memset_calls = 0;
    memfree_calls = 0;
    open_file_result = -1;
    short_read_after = 0;
    read_call_count = 0;
    next_read_value = 1;

    init_palette_source(planet_a, 10);
    init_palette_source(planet_b, 40);
    init_palette_source(planet_c, 70);
    init_palette_source(planet_d, 100);
    for (int i = 0; i < 32; ++i) {
        planet_group0[i] = planet_a[i];
        planet_group1[i] = planet_b[i];
        planet_group2[i] = planet_c[i];
        planet_group3[i] = planet_d[i];
        planet_group4[i] = planet_a[i];
        planet_group5[i] = planet_b[i];
        lpbs_timer[i] = (Sint16)(3 + i);
    }
    lp_coltbl[0] = planet_group0;
    lp_coltbl[1] = planet_group1;
    lp_coltbl[2] = planet_group2;
    lp_coltbl[3] = planet_group3;
    lp_coltbl[4] = planet_group4;
    lp_coltbl[5] = planet_group5;
    for (int i = 0; i < 16; ++i) {
        init_col[i].peRed = (Uint8)(150 + i);
        ascii_col[i].peRed = (Uint8)(170 + i);
        sprite_col1[i].peRed = (Uint8)(190 + i);
        sprite_col2[i].peRed = (Uint8)(210 + i);
    }
    sprite_color[0] = sprite_col1;
    sprite_color[1] = sprite_col2;
    funka_map0[0] = 1;
    funka_map0[1] = 0;
    funka_map0[2] = 2;
    funka_map0[3] = 3;
    funka_map1[0] = 4;
    funka_map1[1] = 5;
    funka_map1[2] = 0;
    funka_map1[3] = 6;
    funka_map2[0] = 7;
    funka_map2[1] = 8;
    funka_map2[2] = 9;
    funka_map2[3] = 0;
    for (int i = 0; i < 12; ++i) {
        funka_map_tbl[i] = (i % 3 == 0) ? funka_map0
                                        : ((i % 3 == 1) ? funka_map1
                                                        : funka_map2);
        funka_timetbl[i] = (Sint16)(5 + i);
    }

    buffer_table[0] = 0;
    buffer_table[1] = (char *)color0;
    buffer_table[2] = (char *)color1;
    buffer_table[3] = (char *)color2;
    buffer_table[4] = (char *)color3;
    buffer_table[5] = (char *)hscrollbuf;
    buffer_table[6] = (char *)&fade_flag_storage;
    buffer_table[7] = (char *)&keep_work;
    for (int i = 0; i < 8; ++i)
        buffer_table_ptrs[i] = &buffer_table[i];
    buffer_table_ptrs[6] = (char **)&fade_flag_storage;
    buffer_table_ptrs[7] = (char **)&keep_work;

    void *funcs[64] = {0};
    funcs[0] = stub_set_grid;
    funcs[1] = stub_easprset;
    funcs[2] = stub_clr_sprite_debug;
    funcs[3] = stub_wave_request;
    funcs[4] = stub_cdplay;
    funcs[6] = stub_change_tile_bmp;
    funcs[10] = stub_wave_all_stop;
    funcs[11] = stub_mem_alloc;
    funcs[12] = stub_mem_free;
    funcs[13] = stub_mem_set;
    funcs[14] = stub_mem_cpy;
    funcs[16] = stub_random;
    funcs[20] = stub_printf;
    funcs[21] = stub_output_debug;
    funcs[22] = stub_open_file;
    funcs[23] = stub_read_file;
    funcs[24] = stub_close_file;

    SetGrid = stub_set_grid;
    EAsprset = stub_easprset;
    ClrSpriteDebug = stub_clr_sprite_debug;
    WaveRequest = stub_wave_request;
    CDPlay = stub_cdplay;
    ChangeTileBmp = stub_change_tile_bmp;
    WaveAllStop = stub_wave_all_stop;
    sMemAlloc = stub_mem_alloc;
    sMemFree = stub_mem_free;
    sMemSet = stub_mem_set;
    sMemCpy = stub_mem_cpy;
    sRandom = stub_random;
    sPrintf = stub_printf;
    sOutputDebugString = stub_output_debug;
    sOpenFile = stub_open_file;
    sReadFile = stub_read_file;
    sCloseFile = stub_close_file;
    sPeriPadGet = 0;

    lpcolorwk0 = color0;
    lpcolorwk1 = color1;
    lpcolorwk2 = color2;
    lpcolorwk3 = color3;
    lphscrollbuff = hscrollbuf;
    lpFadeFlag = &fade_flag_storage;
    lpKeepWork = &keep_work;
    keep_work.sm_adr0 = sm_adr_storage;
    keep_work.ptv_adr = (Uint16 *)tv_adr;
    wordram.wordram = allocation0;

    MakeupMode = 0;
    InitMode = 0;
    bGameInit = 0;
    ProcMode = 0;
    main_lpcnt = 1;
    intmode = 0;
    lplanet_no = 0;
    comflag_s = 0;
    comflag_m = 0;
    comdata_m4 = 0;
    comdata_m5 = 0;
    flash_flg = 0;
    coltimer = 1;
    colorno = 0;
    spr_coltimer = 1;
    spr_colno = 0;
    funka_pctimer = 0;
    funka_patno = 0;
    gmtime0 = 2;
    gmtime1 = 0;
}

static void test_dll_meminit_wires_callbacks_and_loads_map(test_context *ctx) {
    void *funcs[64] = {0};

    init_environment();
    funcs[0] = stub_set_grid;
    funcs[1] = stub_easprset;
    funcs[2] = stub_clr_sprite_debug;
    funcs[3] = stub_wave_request;
    funcs[4] = stub_cdplay;
    funcs[6] = stub_change_tile_bmp;
    funcs[10] = stub_wave_all_stop;
    funcs[11] = stub_mem_alloc;
    funcs[12] = stub_mem_free;
    funcs[13] = stub_mem_set;
    funcs[14] = stub_mem_cpy;
    funcs[16] = stub_random;
    funcs[20] = stub_printf;
    funcs[21] = stub_output_debug;
    funcs[22] = stub_open_file;
    funcs[23] = stub_read_file;
    funcs[24] = stub_close_file;
    open_file_result = 17;

    DLL_meminit(buffer_table_ptrs, funcs);

    TEST_ASSERT_TRUE(ctx, SetGrid == stub_set_grid);
    TEST_ASSERT_TRUE(ctx, CDPlay == stub_cdplay);
    TEST_ASSERT_TRUE(ctx, sMemAlloc == stub_mem_alloc);
    TEST_ASSERT_TRUE(ctx, wordram.wordram == allocation0);
    TEST_ASSERT_TRUE(ctx, keep_work.pSprBmp == SprBmp);
    TEST_ASSERT_TRUE(ctx, keep_work.ptv_adr == (Uint16 *)tv_adr);
    TEST_ASSERT_TRUE(ctx, keep_work.sm_adr0 == sm_adr_storage);
    TEST_ASSERT_EQ_INT(ctx, 56, read_call_count);
    TEST_ASSERT_EQ_INT(ctx, 1, close_calls);
    TEST_ASSERT_EQ_INT(ctx, 1, sm_adr_storage[0]);
    TEST_ASSERT_EQ_INT(ctx, 8, sm_adr_storage[7]);
    TEST_ASSERT_EQ_INT(ctx, 0, sm_adr_storage[8]);
    TEST_ASSERT_EQ_INT(ctx, 0, MakeupMode);
    TEST_ASSERT_EQ_INT(ctx, 0, InitMode);

    DLL_memfree();
    TEST_ASSERT_EQ_INT(ctx, 2, memfree_calls);
    TEST_ASSERT_TRUE(ctx, freed_ptrs[0] == allocation0);
    TEST_ASSERT_TRUE(ctx, freed_ptrs[1] == sm_adr_storage);
}

static void test_dll_meminit_returns_on_short_map_read(test_context *ctx) {
    void *funcs[64] = {0};

    init_environment();
    funcs[0] = stub_set_grid;
    funcs[1] = stub_easprset;
    funcs[2] = stub_clr_sprite_debug;
    funcs[3] = stub_wave_request;
    funcs[4] = stub_cdplay;
    funcs[6] = stub_change_tile_bmp;
    funcs[10] = stub_wave_all_stop;
    funcs[11] = stub_mem_alloc;
    funcs[12] = stub_mem_free;
    funcs[13] = stub_mem_set;
    funcs[14] = stub_mem_cpy;
    funcs[16] = stub_random;
    funcs[20] = stub_printf;
    funcs[21] = stub_output_debug;
    funcs[22] = stub_open_file;
    funcs[23] = stub_read_file;
    funcs[24] = stub_close_file;
    open_file_result = 17;
    short_read_after = 1;

    DLL_meminit(buffer_table_ptrs, funcs);

    TEST_ASSERT_EQ_INT(ctx, 1, read_call_count);
    TEST_ASSERT_EQ_INT(ctx, 1, close_calls);
}

static void test_swdata_and_palette_vint(test_context *ctx) {
    ushort_union sw1;
    ushort_union sw2;

    init_environment();
    sw1.w = 0x1234;
    sw2.w = 0xABCD;
    SWdataSet(sw1, sw2);
    TEST_ASSERT_EQ_INT(ctx, 0x1234, swdata1.w);
    TEST_ASSERT_EQ_INT(ctx, 0xABCD, swdata2.w);

    v_int();

    TEST_ASSERT_EQ_INT(ctx, planet_b[0][0].peRed, color0[0].peRed);
    TEST_ASSERT_EQ_INT(ctx, planet_a[0][0].peRed, color0[16].peRed);
    TEST_ASSERT_EQ_INT(ctx, sprite_col1[0].peRed, color0[32].peRed);
    TEST_ASSERT_EQ_INT(ctx, 2, spr_colno);
    TEST_ASSERT_EQ_INT(ctx, 5, spr_coltimer);
    TEST_ASSERT_EQ_INT(ctx, 3, coltimer);
    TEST_ASSERT_EQ_INT(ctx, 1, colorno);
    TEST_ASSERT_EQ_INT(ctx, 0, funka_pctimer);
    TEST_ASSERT_EQ_INT(ctx, 1, gmtime0);
    TEST_ASSERT_EQ_INT(ctx, 1, gmtime1);

    flash_flg = 1;
    color0[0].peRed = 0;
    v_int();
    TEST_ASSERT_EQ_INT(ctx, 0, color0[0].peRed);
}

static void test_prg_init_patchg_and_cgchange_timer(test_context *ctx) {
    init_environment();
    lplanet_no = 2;

    prg_init();

    TEST_ASSERT_EQ_INT(ctx, planet_b[0][0].peRed, color1[0].peRed);
    TEST_ASSERT_EQ_INT(ctx, planet_a[0][0].peRed, color1[16].peRed);
    TEST_ASSERT_EQ_INT(ctx, sprite_col1[0].peRed, color1[32].peRed);
    TEST_ASSERT_EQ_INT(ctx, ascii_col[0].peRed, color1[48].peRed);
    TEST_ASSERT_EQ_INT(ctx, 0, colorno);
    TEST_ASSERT_EQ_INT(ctx, 3, coltimer);
    TEST_ASSERT_EQ_INT(ctx, 5, spr_coltimer);
    TEST_ASSERT_EQ_INT(ctx, 0, spr_colno);
    TEST_ASSERT_EQ_INT(ctx, 4, grid_call_count);
    TEST_ASSERT_EQ_INT(ctx, 721, grid_calls[0].tile);
    TEST_ASSERT_EQ_INT(ctx, 720, grid_calls[1].tile);

    grid_call_count = 0;
    funka_pctimer = 0;
    funka_patno = 12;
    lplanet_no = 0;
    cgchange_chk();

    TEST_ASSERT_EQ_INT(ctx, 5, funka_pctimer);
    TEST_ASSERT_EQ_INT(ctx, 1, funka_patno);
    TEST_ASSERT_EQ_INT(ctx, 621, grid_calls[0].tile);
    TEST_ASSERT_EQ_INT(ctx, 0, grid_calls[1].tile);
    TEST_ASSERT_EQ_INT(ctx, 622, grid_calls[2].tile);
    TEST_ASSERT_EQ_INT(ctx, 623, grid_calls[3].tile);
}

static void test_cgdata_change_and_cd_call(test_context *ctx) {
    init_environment();
    open_file_result = 9;
    comdata_m5 = 1;
    lplanet_no = 1;

    cgdata_change();

    TEST_ASSERT_EQ_INT(ctx, 56, read_call_count);
    TEST_ASSERT_EQ_INT(ctx, 1, close_calls);
    TEST_ASSERT_EQ_INT(ctx, 201, sm_adr_storage[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, sm_adr_storage[8]);
    TEST_ASSERT_EQ_INT(ctx, 70, grid_call_count);
    TEST_ASSERT_EQ_INT(ctx, 201, grid_calls[0].tile);

    comdata_m4 = 5;
    cd_call();
    TEST_ASSERT_EQ_INT(ctx, 1, cdplay_calls);
    TEST_ASSERT_EQ_INT(ctx, 37, cdplay_args[0]);
}

static void test_game_modes_and_makeup_sequence(test_context *ctx) {
    init_environment();
    ProcMode = 0;
    main_lpcnt = -1;

    TEST_ASSERT_EQ_INT(ctx, 0, game());
    TEST_ASSERT_EQ_INT(ctx, 1, ProcMode);
    TEST_ASSERT_EQ_INT(ctx, 1, main_lpcnt);
    TEST_ASSERT_EQ_INT(ctx, 1, action_calls);
    TEST_ASSERT_EQ_INT(ctx, 1, sprascii_calls);

    TEST_ASSERT_EQ_INT(ctx, 0, game());
    TEST_ASSERT_EQ_INT(ctx, 2, ProcMode);
    TEST_ASSERT_EQ_INT(ctx, 2, action_calls);
    TEST_ASSERT_EQ_INT(ctx, 1, intmode);

    comflag_s = 0;
    TEST_ASSERT_EQ_INT(ctx, 0, game());
    TEST_ASSERT_EQ_INT(ctx, 4, ProcMode);
    TEST_ASSERT_EQ_INT(ctx, 1, set_asciicg_calls);

    ProcMode = 4;
    comflag_s = 0;
    main_lpcnt = 3;
    TEST_ASSERT_EQ_INT(ctx, 0, game());
    TEST_ASSERT_EQ_INT(ctx, 0, ProcMode);
    TEST_ASSERT_EQ_INT(ctx, 2, main_lpcnt);

    ProcMode = 5;
    comflag_s = 99;
    comflag_m = 88;
    TEST_ASSERT_EQ_INT(ctx, -1, game());
    TEST_ASSERT_EQ_INT(ctx, 0, comflag_s);
    TEST_ASSERT_EQ_INT(ctx, 0, comflag_m);

    init_environment();
    MakeupMode = 0;
    lp_makeup();
    TEST_ASSERT_EQ_INT(ctx, 1, flashout_calls);
    TEST_ASSERT_EQ_INT(ctx, 1, MakeupMode);

    lp_makeup();
    TEST_ASSERT_EQ_INT(ctx, 2, MakeupMode);
    TEST_ASSERT_EQ_INT(ctx, 1, memset_calls);
    TEST_ASSERT_EQ_INT(ctx, 960, memset_sizes[0]);

    MakeupMode = 6;
    music_selflg = 3;
    lp_makeup();
    TEST_ASSERT_EQ_INT(ctx, 1, flashin_calls);
    TEST_ASSERT_EQ_INT(ctx, 7, MakeupMode);
    TEST_ASSERT_EQ_INT(ctx, 2, music_selflg);
}

static void test_game_init_and_init_display_modes(test_context *ctx) {
    init_environment();
    InitMode = 5;
    open_file_result = 8;

    game_init();

    TEST_ASSERT_EQ_INT(ctx, 33, cdplay_args[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, fadein_calls);
    TEST_ASSERT_EQ_INT(ctx, 0, bGameInit);
    TEST_ASSERT_EQ_INT(ctx, init_col[0].peRed, color1[0].peRed);

    init_environment();
    InitMode = 0;
    init_dsp();
    TEST_ASSERT_EQ_INT(ctx, 1, InitMode);
    TEST_ASSERT_EQ_INT(ctx, 1, kaiten0_calls);

    InitMode = 1;
    comflag_s = 64;
    init_dsp();
    TEST_ASSERT_EQ_INT(ctx, 2, InitMode);
    TEST_ASSERT_EQ_INT(ctx, 1, fadeout_calls);
    TEST_ASSERT_EQ_INT(ctx, 1, flash_flg);

    InitMode = 2;
    intmode = 4;
    init_dsp();
    TEST_ASSERT_EQ_INT(ctx, 3, InitMode);
    TEST_ASSERT_EQ_INT(ctx, 5, intmode);

    InitMode = 4;
    comflag_s = 0;
    init_dsp();
    TEST_ASSERT_EQ_INT(ctx, 5, InitMode);

    InitMode = 4;
    comflag_s = 64;
    fadeout_calls = 0;
    init_dsp();
    TEST_ASSERT_EQ_INT(ctx, 1, fadeout_calls);

    init_environment();
    InitMode = 0;
    open_file_result = -1;
    game_init();
    TEST_ASSERT_EQ_INT(ctx, 33, cdplay_args[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, bGameInit);
    TEST_ASSERT_EQ_INT(ctx, 5, InitMode);
    TEST_ASSERT_TRUE(ctx, grid_call_count > 1000);
}

static void test_more_game_and_makeup_branches(test_context *ctx) {
    init_environment();
    ProcMode = 2;
    comflag_s = 32 | 64;
    game();
    TEST_ASSERT_EQ_INT(ctx, 1, MakeupMode);
    TEST_ASSERT_EQ_INT(ctx, 3, ProcMode);

    init_environment();
    ProcMode = 3;
    MakeupMode = -1;
    comflag_m = 0;
    comflag_s = 32;
    game();
    TEST_ASSERT_EQ_INT(ctx, 4, ProcMode);
    TEST_ASSERT_EQ_INT(ctx, 16, comflag_s);

    init_environment();
    ProcMode = 4;
    comflag_s = 64;
    game();
    TEST_ASSERT_EQ_INT(ctx, 5, ProcMode);
    TEST_ASSERT_EQ_INT(ctx, 1, fadeout_calls);

    init_environment();
    for (int mode = 2; mode <= 5; ++mode) {
        MakeupMode = mode;
        comflag_s = 0;
        lp_makeup();
        TEST_ASSERT_EQ_INT(ctx, mode + 1, MakeupMode);
    }

    for (int mode = 1; mode <= 5; ++mode) {
        MakeupMode = mode;
        comflag_s = 64;
        lp_makeup();
        TEST_ASSERT_EQ_INT(ctx, -1, MakeupMode);
    }

    MakeupMode = 2;
    comflag_s = 64;
    lp_makeup();
    TEST_ASSERT_EQ_INT(ctx, -1, MakeupMode);

    MakeupMode = 7;
    main_lpcnt = 9;
    intmode = 8;
    lp_makeup();
    TEST_ASSERT_EQ_INT(ctx, 0, main_lpcnt);
    TEST_ASSERT_EQ_INT(ctx, -1, intmode);
    TEST_ASSERT_EQ_INT(ctx, -1, MakeupMode);

    MakeupMode = 99;
    lp_makeup();
    TEST_ASSERT_EQ_INT(ctx, -1, MakeupMode);
}

static void test_cgdata_change_variants_and_grid_writers(test_context *ctx) {
    init_environment();
    open_file_result = 5;
    short_read_after = 1;
    comdata_m5 = 0;

    cgdata_change();

    TEST_ASSERT_EQ_INT(ctx, 1, close_calls);
    TEST_ASSERT_EQ_INT(ctx, 1, read_call_count);

    init_environment();
    comdata_m5 = 3;
    cgdata_change();
    TEST_ASSERT_EQ_INT(ctx, 0, read_call_count);
    TEST_ASSERT_EQ_INT(ctx, 0, grid_call_count);

    init_environment();
    open_file_result = -1;
    comdata_m5 = 2;
    lplanet_no = 3;
    cgdata_change();
    TEST_ASSERT_EQ_INT(ctx, 70, grid_call_count);
    TEST_ASSERT_EQ_INT(ctx, 1, sm_adr_storage[6]);

    init_environment();
    lplanet_no = 2;
    funka_pctimer = 0;
    funka_patno = 1;
    cgchange_chk();
    TEST_ASSERT_EQ_INT(ctx, 6, funka_pctimer);
    TEST_ASSERT_EQ_INT(ctx, 724, grid_calls[0].tile);

    init_environment();
    lplanet_no = 3;
    cgchange_chk();
    TEST_ASSERT_EQ_INT(ctx, 0, grid_call_count);

    init_environment();
    lplanet_no = 3;
    patchg_init();
    TEST_ASSERT_EQ_INT(ctx, 1, grid_calls[0].tile);

    init_environment();
    open_file_result = 5;
    comdata_m5 = 2;
    lplanet_no = 2;
    cgdata_change();
    TEST_ASSERT_EQ_INT(ctx, 417, sm_adr_storage[0]);
    TEST_ASSERT_EQ_INT(ctx, 721, sm_adr_storage[6]);

    init_environment();
    for (int i = 0; i < 704; ++i)
        ScrBMap[i] = (Sint16)(1000 + i);
    cgmwrt_a();
    TEST_ASSERT_EQ_INT(ctx, 240, grid_call_count);
    TEST_ASSERT_EQ_INT(ctx, 32818, grid_calls[0].tile);
    TEST_ASSERT_EQ_INT(ctx, 27, grid_calls[239].y);

    grid_call_count = 0;
    cgmwrt_b();
    TEST_ASSERT_EQ_INT(ctx, 1000, grid_calls[120].tile);
    TEST_ASSERT_EQ_INT(ctx, 1022, grid_calls[152].tile);
    TEST_ASSERT_EQ_INT(ctx, 49, grid_calls[0].tile);
}

static void test_subcpu_and_vint_alternate_edges(test_context *ctx) {
    init_environment();
    bGameInit = 0;
    comflag_s = 16;
    comflag_m = 32;
    comdata_m4 = 0;
    open_file_result = -1;

    SubCpuMain();

    TEST_ASSERT_EQ_INT(ctx, 1, player_calls);
    TEST_ASSERT_EQ_INT(ctx, 1, cdplay_calls);
    TEST_ASSERT_EQ_INT(ctx, 1, kaiten0_calls);
    TEST_ASSERT_EQ_INT(ctx, 32, comflag_s);

    init_environment();
    colorno = 31;
    coltimer = 1;
    spr_colno = 2;
    spr_coltimer = 1;
    v_int();
    TEST_ASSERT_EQ_INT(ctx, 1, colorno);
    TEST_ASSERT_EQ_INT(ctx, 0, spr_colno);
    TEST_ASSERT_EQ_INT(ctx, sprite_col2[0].peRed, color0[32].peRed);
}

TEST_MAIN_BEGIN;
    test_dll_meminit_wires_callbacks_and_loads_map(&ctx);
    test_dll_meminit_returns_on_short_map_read(&ctx);
    test_swdata_and_palette_vint(&ctx);
    test_prg_init_patchg_and_cgchange_timer(&ctx);
    test_cgdata_change_and_cd_call(&ctx);
    test_game_modes_and_makeup_sequence(&ctx);
    test_game_init_and_init_display_modes(&ctx);
    test_more_game_and_makeup_branches(&ctx);
    test_cgdata_change_variants_and_grid_writers(&ctx);
    test_subcpu_and_vint_alternate_edges(&ctx);
TEST_MAIN_END
