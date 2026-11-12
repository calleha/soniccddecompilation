#include <string.h>

#include "support/test_runner.h"
#include "src/title/common/hmx_types.h"
#include "src/title/common/score_data_types.h"
#include "src/types.h"

Uint16 nSequenceNum;
Sint32 nTimerCunt;
draw_context context;
draw_context *s_ctx;
sprite_bmp infoSprtBmp[64];
grid_bmp infoGridBmp[3];
score_data crntScorData;
Uint32 selectIndx;
Uint16 swData1;
Uint32 bFirstTitle;
PALETTEENTRY tblPal1[16];
PALETTEENTRY tblPal1a[16];
PALETTEENTRY tblPal2[16];
PALETTEENTRY tblPal2a[16];
PALETTEENTRY tblPal3[16];
PALETTEENTRY tblPal4[16];
void (*hmx_sprite_set_position_module)(hmx_sprite *, Sint32, Sint32);
void (*hmx_sprite_set_bitmap_module)(hmx_sprite *, hmx_bitmap *);
void (*hmx_grid_set_horz_offsets_module)(hmx_grid *, Sint32 *);
void (*hmx_ddagrid_set_scan_module)(hmx_ddagrid *, Sint32, Sint32, Sint32,
                                    Sint32, Sint32);
void (*sMemCpy)(void *, void *, Sint32);
void (*CDPause)(Sint16);
void (*CDPlay)(Sint16);
Sint32 (*ReadIndx)(Uint32);
Uint32 (*ReadScore)(Sint32, char *, Uint32);
void (*WaveRequest)(Sint16);

static hmx_sprite *sprite_table[128];
static char sprite_storage[128];
static char grid_storage[3];
static Uint32 sprite_handles[64];
static Uint32 grid_handles[3];
static Sint32 draw_call_count;
static Sint32 spr_create_call_count;
static Uint16 spr_create_values[4096];
static Sint32 spr_create_index_call_count;
static Sint32 spr_create_index_values[4096];
static Sint32 spr_delete_call_count;
static Uint16 spr_delete_values[32];
static Sint32 all_spr_delete_call_count;
static Sint32 grid_create_call_count;
static Uint16 grid_create_values[16];
static Sint32 set_sequence_call_count;
static Uint16 set_sequence_values[64];
static Sint32 clrset_call_count;
static PALETTEENTRY *clrset_palettes[4096];
static Uint16 clrset_numbers[4096];
static Sint32 cd_play_call_count;
static Sint16 cd_play_values[16];
static Sint32 cd_pause_call_count;
static Sint16 cd_pause_values[16];
static Sint32 wave_call_count;
static Sint16 wave_values[16];
static Sint32 position_call_count;
static hmx_sprite *position_sprites[4096];
static Sint32 position_x[4096];
static Sint32 position_y[4096];
static Sint32 bitmap_call_count;
static hmx_sprite *bitmap_sprites[4096];
static hmx_bitmap *bitmap_values[4096];
static Sint32 duplicate_call_count;
static Sint32 duplicate_index[16];
static Sint32 duplicate_palette[16];
static Sint32 duplicate_unfaded[16];
static char fake_flash_storage[16];
static Sint32 horz_offsets_call_count;
static hmx_grid *horz_offsets_grid[8];
static Sint32 horz_offsets_first[8];
static Sint32 horz_offsets_last[8];
static Sint32 scan_call_count;
static Sint32 scan_index[20000];
static Sint32 scan_start_x[20000];
static Sint32 scan_delta_x[20000];
static Sint32 scan_start_y[20000];
static Sint32 scan_delta_y[20000];
static Uint32 read_score_returns[8];
static score_data read_score_values[8];
static Sint32 read_score_call_count;
static Sint32 read_score_slots[32];
static Sint32 read_index_return;

#include "src/title/opening/opndo.c"

static hmx_sprite *sprite_handle(Sint32 handle) {
    return (hmx_sprite *)&sprite_storage[handle];
}

static void fake_sprite_set_position(hmx_sprite *sprite, Sint32 x, Sint32 y) {
    position_sprites[position_call_count] = sprite;
    position_x[position_call_count] = x;
    position_y[position_call_count] = y;
    ++position_call_count;
}

static void fake_sprite_set_bitmap(hmx_sprite *sprite, hmx_bitmap *bitmap) {
    bitmap_sprites[bitmap_call_count] = sprite;
    bitmap_values[bitmap_call_count] = bitmap;
    ++bitmap_call_count;
}

hmx_bitmap *OESprBmpDuplicateUnfaded(Sint32 indx, Sint32 palet,
                                     Sint32 unfaded) {
    duplicate_index[duplicate_call_count] = indx;
    duplicate_palette[duplicate_call_count] = palet;
    duplicate_unfaded[duplicate_call_count] = unfaded;
    return (hmx_bitmap *)&fake_flash_storage[duplicate_call_count++];
}

static void fake_grid_set_horz_offsets(hmx_grid *grid, Sint32 *offsets) {
    horz_offsets_grid[horz_offsets_call_count] = grid;
    horz_offsets_first[horz_offsets_call_count] = offsets[0];
    horz_offsets_last[horz_offsets_call_count] = offsets[63];
    ++horz_offsets_call_count;
}

static void fake_ddagrid_set_scan(hmx_ddagrid *grid, Sint32 index,
                                  Sint32 start_x, Sint32 delta_x,
                                  Sint32 start_y, Sint32 delta_y) {
    (void)grid;
    scan_index[scan_call_count] = index;
    scan_start_x[scan_call_count] = start_x;
    scan_delta_x[scan_call_count] = delta_x;
    scan_start_y[scan_call_count] = start_y;
    scan_delta_y[scan_call_count] = delta_y;
    ++scan_call_count;
}

static void fake_mem_cpy(void *dst, void *src, Sint32 bytes) {
    memcpy(dst, src, (size_t)bytes);
}

static void fake_cd_play(Sint16 track) {
    cd_play_values[cd_play_call_count] = track;
    ++cd_play_call_count;
}

static void fake_cd_pause(Sint16 mode) {
    cd_pause_values[cd_pause_call_count] = mode;
    ++cd_pause_call_count;
}

static void fake_wave_request(Sint16 wave) {
    wave_values[wave_call_count] = wave;
    ++wave_call_count;
}

static Sint32 fake_read_index(Uint32 slot) {
    (void)slot;
    return read_index_return;
}

static Uint32 fake_read_score(Sint32 slot, char *data, Uint32 mode) {
    (void)mode;
    read_score_slots[read_score_call_count] = slot;
    ++read_score_call_count;
    if (slot >= 0 && slot < 8) {
        memcpy(data, &read_score_values[slot], sizeof(score_data));
        return read_score_returns[slot];
    }
    return 0;
}

void srfDraw(void) { ++draw_call_count; }

Sint32 OESprCreate(Uint16 timing) {
    spr_create_values[spr_create_call_count] = timing;
    ++spr_create_call_count;
    return 0;
}

Sint32 OESprCreateIndx(Sint32 indx) {
    spr_create_index_values[spr_create_index_call_count] = (Uint16)indx;
    ++spr_create_index_call_count;
    return 0;
}

void OESprDelete(Uint16 timing) {
    spr_delete_values[spr_delete_call_count] = timing;
    ++spr_delete_call_count;
}

void OEAllSprDelete(void) { ++all_spr_delete_call_count; }

Sint32 OEGridCreate(Uint16 indx) {
    grid_create_values[grid_create_call_count] = indx;
    ++grid_create_call_count;
    return 0;
}

void OESetSequenceNum(Uint16 num) {
    set_sequence_values[set_sequence_call_count] = num;
    ++set_sequence_call_count;
    nSequenceNum = num;
    nTimerCunt = -1;
}

void OEClrset(PALETTEENTRY *palette, Uint16 number) {
    clrset_palettes[clrset_call_count] = palette;
    clrset_numbers[clrset_call_count] = number;
    ++clrset_call_count;
}

static void fill_palette(PALETTEENTRY *palette, Uint8 base) {
    Sint32 i;

    for (i = 0; i < 16; ++i) {
        palette[i].peRed = (Uint8)(base + i);
        palette[i].peGreen = (Uint8)(base + i + 20);
        palette[i].peBlue = (Uint8)(base + i + 40);
        palette[i].peFlags = (Uint8)(base + i + 60);
    }
}

static void reset_fixture(void) {
    Sint32 i;

    memset(&context, 0, sizeof(context));
    memset(infoSprtBmp, 0, sizeof(infoSprtBmp));
    memset(infoGridBmp, 0, sizeof(infoGridBmp));
    memset(sprite_handles, 0, sizeof(sprite_handles));
    memset(grid_handles, 0, sizeof(grid_handles));
    memset(spr_create_values, 0, sizeof(spr_create_values));
    memset(spr_create_index_values, 0, sizeof(spr_create_index_values));
    memset(spr_delete_values, 0, sizeof(spr_delete_values));
    memset(set_sequence_values, 0, sizeof(set_sequence_values));
    memset(clrset_palettes, 0, sizeof(clrset_palettes));
    memset(clrset_numbers, 0, sizeof(clrset_numbers));
    memset(cd_play_values, 0, sizeof(cd_play_values));
    memset(cd_pause_values, 0, sizeof(cd_pause_values));
    memset(wave_values, 0, sizeof(wave_values));
    memset(position_sprites, 0, sizeof(position_sprites));
    memset(position_x, 0, sizeof(position_x));
    memset(position_y, 0, sizeof(position_y));
    memset(bitmap_sprites, 0, sizeof(bitmap_sprites));
    memset(bitmap_values, 0, sizeof(bitmap_values));
    memset(duplicate_index, 0, sizeof(duplicate_index));
    memset(duplicate_palette, 0, sizeof(duplicate_palette));
    memset(duplicate_unfaded, 0, sizeof(duplicate_unfaded));
    memset(horz_offsets_grid, 0, sizeof(horz_offsets_grid));
    memset(horz_offsets_first, 0, sizeof(horz_offsets_first));
    memset(horz_offsets_last, 0, sizeof(horz_offsets_last));
    memset(scan_index, 0, sizeof(scan_index));
    memset(scan_start_x, 0, sizeof(scan_start_x));
    memset(scan_delta_x, 0, sizeof(scan_delta_x));
    memset(scan_start_y, 0, sizeof(scan_start_y));
    memset(scan_delta_y, 0, sizeof(scan_delta_y));
    memset(read_score_returns, 0, sizeof(read_score_returns));
    memset(read_score_values, 0, sizeof(read_score_values));
    memset(read_score_slots, 0, sizeof(read_score_slots));
    memset(&crntScorData, 0, sizeof(crntScorData));
    memset(workPalet, 0, sizeof(workPalet));

    for (i = 0; i < 128; ++i) {
        sprite_table[i] = sprite_handle(i);
    }
    for (i = 0; i < 64; ++i) {
        infoSprtBmp[i].lphSpr = &sprite_handles[i];
        infoSprtBmp[i].point.x = i * 3;
        infoSprtBmp[i].point.y = i * 5;
        infoSprtBmp[i].w = 10 + i;
    }
    for (i = 0; i < 3; ++i) {
        context.grids[i] = (hmx_grid *)&grid_storage[i];
        infoGridBmp[i].lphGrid = &grid_handles[i];
    }
    context.sprites = sprite_table;
    s_ctx = &context;

    fill_palette(tblPal1, 10);
    fill_palette(tblPal1a, 30);
    fill_palette(tblPal2, 50);
    fill_palette(tblPal2a, 70);
    fill_palette(tblPal3, 90);
    fill_palette(tblPal4, 110);

    nSequenceNum = 0;
    nTimerCunt = 0;
    selectIndx = 0;
    swData1 = 0;
    bFirstTitle = 0;
    hFx0 = 0;
    hFx1 = 0;
    nFadoTime = 0;
    nFadoValu = 0;
    memset(bFadePaletDisable, 0, sizeof(bFadePaletDisable));
    nFadoSeqNum = 0;
    nMenuKind = 0;
    bDrawDisable = 0;
    testBmp = 0;
    testSpr = 0;
    memset(flashSprtBmp, 0, sizeof(flashSprtBmp));
    KeyState[0] = 0;
    hmx_sprite_set_position_module = fake_sprite_set_position;
    hmx_sprite_set_bitmap_module = fake_sprite_set_bitmap;
    hmx_grid_set_horz_offsets_module = fake_grid_set_horz_offsets;
    hmx_ddagrid_set_scan_module = fake_ddagrid_set_scan;
    sMemCpy = fake_mem_cpy;
    CDPlay = fake_cd_play;
    CDPause = fake_cd_pause;
    WaveRequest = fake_wave_request;
    ReadScore = fake_read_score;
    ReadIndx = fake_read_index;
    read_index_return = 4;

    draw_call_count = 0;
    spr_create_call_count = 0;
    spr_create_index_call_count = 0;
    spr_delete_call_count = 0;
    all_spr_delete_call_count = 0;
    grid_create_call_count = 0;
    set_sequence_call_count = 0;
    clrset_call_count = 0;
    cd_play_call_count = 0;
    cd_pause_call_count = 0;
    wave_call_count = 0;
    position_call_count = 0;
    bitmap_call_count = 0;
    duplicate_call_count = 0;
    horz_offsets_call_count = 0;
    scan_call_count = 0;
    read_score_call_count = 0;
}

static void test_draw_respects_sequence_and_draw_disable(test_context *ctx) {
    reset_fixture();

    nSequenceNum = 1;
    OEDraw();
    TEST_ASSERT_EQ_INT(ctx, 0, draw_call_count);

    nSequenceNum = 2;
    OEDraw();
    TEST_ASSERT_EQ_INT(ctx, 1, draw_call_count);

    bDrawDisable = 1;
    OEDraw();
    TEST_ASSERT_EQ_INT(ctx, 1, draw_call_count);
}

static void test_initial_hand_update_reuses_first_pattern(test_context *ctx) {
    reset_fixture();
    nTimerCunt = 5;

    OEUpdateHand();

    TEST_ASSERT_EQ_INT(ctx, 1, spr_create_call_count);
    TEST_ASSERT_EQ_INT(ctx, 11, spr_create_values[0]);
}

static void test_score_slot_helpers_and_enabled_menu_table(test_context *ctx) {
    Uint32 menu[7];

    reset_fixture();
    TEST_ASSERT_EQ_INT(ctx, 1, ChkNewSlot());
    TEST_ASSERT_EQ_INT(ctx, 1, read_score_call_count);
    TEST_ASSERT_EQ_INT(ctx, 0, read_score_slots[0]);

    reset_fixture();
    read_score_returns[0] = 1;
    read_score_values[0].saved = 0;
    TEST_ASSERT_EQ_INT(ctx, 1, ChkNewSlot());
    TEST_ASSERT_EQ_INT(ctx, 1, read_score_call_count);

    reset_fixture();
    for (Sint32 i = 0; i < 6; ++i) {
        read_score_returns[i] = 1;
        read_score_values[i].saved = 1;
    }
    TEST_ASSERT_EQ_INT(ctx, 0, ChkNewSlot());
    TEST_ASSERT_EQ_INT(ctx, 6, read_score_call_count);

    reset_fixture();
    memset(menu, 99, sizeof(menu));
    crntScorData.saved = 1;
    crntScorData.roundNo = 7;
    crntScorData.total = 90000;
    GetEnabeMenu(menu);
    TEST_ASSERT_EQ_INT(ctx, 1, menu[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, menu[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, menu[2]);
    TEST_ASSERT_EQ_INT(ctx, 0, menu[3]);
    TEST_ASSERT_EQ_INT(ctx, 1, menu[4]);
    TEST_ASSERT_EQ_INT(ctx, 1, menu[5]);
    TEST_ASSERT_EQ_INT(ctx, 1, menu[6]);

    reset_fixture();
    memset(menu, 0, sizeof(menu));
    read_score_returns[0] = 1;
    read_score_values[0].saved = 1;
    read_score_values[4].saved = 1;
    read_score_values[4].roundNo = 7;
    read_score_values[4].total = 100000;
    for (Sint32 i = 1; i < 6; ++i) {
        read_score_returns[i] = 1;
        read_score_values[i].saved = 1;
    }
    GetEnabeMenu(menu);
    TEST_ASSERT_EQ_INT(ctx, 4, selectIndx);
    TEST_ASSERT_EQ_INT(ctx, 0, menu[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, menu[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, menu[2]);
    TEST_ASSERT_EQ_INT(ctx, 1, menu[3]);
    TEST_ASSERT_EQ_INT(ctx, 1, menu[4]);
    TEST_ASSERT_EQ_INT(ctx, 0, menu[5]);
    TEST_ASSERT_EQ_INT(ctx, 1, menu[6]);
}

static void test_get_next_menu_forward_backward_and_initial(test_context *ctx) {
    Uint32 menu[7] = {1, 0, 0, 1, 0, 0, 1};

    reset_fixture();

    TEST_ASSERT_EQ_INT(ctx, 0, GetNextMenu(menu, -1, 0));
    menu[1] = 1;
    TEST_ASSERT_EQ_INT(ctx, 1, GetNextMenu(menu, -1, 0));
    TEST_ASSERT_EQ_INT(ctx, 3, GetNextMenu(menu, 1, 1));
    TEST_ASSERT_EQ_INT(ctx, 0, GetNextMenu(menu, 6, 1));
    TEST_ASSERT_EQ_INT(ctx, 6, GetNextMenu(menu, 0, 0));
    TEST_ASSERT_EQ_INT(ctx, 3, GetNextMenu(menu, 6, 0));
}

static void test_start_open_logo_dark_and_sonic_sequences(test_context *ctx) {
    reset_fixture();
    OETitleSta();
    /* The title screen brings the blue backdrop (grid 0) in with the water
       (grid 1); it is not on screen for the earlier sequences. */
    TEST_ASSERT_EQ_INT(ctx, 2, grid_create_call_count);
    TEST_ASSERT_EQ_INT(ctx, 0, grid_create_values[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, grid_create_values[1]);
    TEST_ASSERT_EQ_INT(ctx, 2, spr_create_call_count);
    TEST_ASSERT_EQ_INT(ctx, 7, spr_create_values[0]);
    TEST_ASSERT_EQ_INT(ctx, 11, spr_create_values[1]);
    TEST_ASSERT_EQ_INT(ctx, 9, set_sequence_values[0]);
    /* The flash writes all four palette lines, so no line is exempt any more,
       and the effect keeps the reference's 21 frames (see the fade-in test). */
    TEST_ASSERT_EQ_INT(ctx, 21, nFadoTime);
    TEST_ASSERT_EQ_INT(ctx, 0, bFadePaletDisable[3]);

    reset_fixture();
    nTimerCunt = 1;
    OETitleSta();
    TEST_ASSERT_EQ_INT(ctx, 0, grid_create_call_count);
    TEST_ASSERT_EQ_INT(ctx, 2, spr_create_call_count);

    reset_fixture();
    bFirstTitle = 1;
    OEOpenSta();
    TEST_ASSERT_EQ_INT(ctx, 0, grid_create_values[0]);
    TEST_ASSERT_EQ_INT(ctx, 2, set_sequence_values[0]);
    reset_fixture();
    OEOpenSta();
    TEST_ASSERT_EQ_INT(ctx, 5, set_sequence_values[0]);

    reset_fixture();
    OESegaLogo();
    TEST_ASSERT_TRUE(ctx, clrset_palettes[0] == tblPal1a);
    TEST_ASSERT_EQ_INT(ctx, 0, clrset_numbers[0]);
    nTimerCunt = 1;
    OESegaLogo();
    TEST_ASSERT_EQ_INT(ctx, 1, spr_create_values[0]);
    nTimerCunt = 90;
    OESegaLogo();
    TEST_ASSERT_EQ_INT(ctx, 1, spr_delete_values[0]);
    TEST_ASSERT_EQ_INT(ctx, 3, set_sequence_values[0]);
    TEST_ASSERT_TRUE(ctx, clrset_palettes[1] == tblPal1);

    reset_fixture();
    nTimerCunt = 29;
    OEDark();
    TEST_ASSERT_EQ_INT(ctx, 0, set_sequence_call_count);
    nTimerCunt = 30;
    OEDark();
    TEST_ASSERT_EQ_INT(ctx, 5, set_sequence_values[0]);

    reset_fixture();
    OESonic();
    TEST_ASSERT_EQ_INT(ctx, 26, cd_play_values[0]);
    nTimerCunt = 1;
    OESonic();
    TEST_ASSERT_EQ_INT(ctx, 2, spr_create_values[0]);
    nTimerCunt = 30;
    OESonic();
    TEST_ASSERT_EQ_INT(ctx, 3, spr_create_values[1]);
    nTimerCunt = 33;
    OESonic();
    TEST_ASSERT_EQ_INT(ctx, 4, spr_create_values[2]);
    nTimerCunt = 36;
    OESonic();
    TEST_ASSERT_EQ_INT(ctx, 5, spr_create_values[3]);
    nTimerCunt = 38;
    OESonic();
    TEST_ASSERT_EQ_INT(ctx, 6, set_sequence_values[0]);
}

static void test_fade_out_copies_darkens_and_finishes(test_context *ctx) {
    reset_fixture();
    nFadoTime = 5;
    nFadoValu = 32;
    nFadoSeqNum = 77;

    OEFadeOut();
    TEST_ASSERT_EQ_INT(ctx, tblPal1[0].peRed, workPalet[0][0].peRed);
    TEST_ASSERT_EQ_INT(ctx, tblPal4[15].peBlue, workPalet[3][15].peBlue);

    nTimerCunt = 1;
    workPalet[0][0].peRed = 40;
    workPalet[0][0].peGreen = 20;
    workPalet[0][0].peBlue = 10;
    workPalet[0][1].peRed = 10;
    workPalet[0][1].peGreen = 40;
    workPalet[0][1].peBlue = 20;
    workPalet[0][2].peRed = 0;
    workPalet[0][2].peGreen = 10;
    workPalet[0][2].peBlue = 40;
    workPalet[0][3].peRed = 0;
    workPalet[0][3].peGreen = 0;
    workPalet[0][3].peBlue = 10;
    bFadePaletDisable[1] = 1;
    OEFadeOut();
    TEST_ASSERT_EQ_INT(ctx, 8, workPalet[0][0].peRed);
    TEST_ASSERT_EQ_INT(ctx, 20, workPalet[0][0].peGreen);
    TEST_ASSERT_EQ_INT(ctx, 0, workPalet[0][1].peRed);
    TEST_ASSERT_EQ_INT(ctx, 8, workPalet[0][1].peGreen);
    TEST_ASSERT_EQ_INT(ctx, 0, workPalet[0][2].peGreen);
    TEST_ASSERT_EQ_INT(ctx, 8, workPalet[0][2].peBlue);
    TEST_ASSERT_EQ_INT(ctx, 0, workPalet[0][3].peBlue);
    TEST_ASSERT_EQ_INT(ctx, 3, clrset_call_count);
    TEST_ASSERT_EQ_INT(ctx, 0, clrset_numbers[0]);
    TEST_ASSERT_EQ_INT(ctx, 2, clrset_numbers[1]);
    TEST_ASSERT_EQ_INT(ctx, 3, clrset_numbers[2]);

    nTimerCunt = 5;
    OEFadeOut();
    TEST_ASSERT_EQ_INT(ctx, 77, set_sequence_values[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, bFadePaletDisable[1]);
}

static void test_fade_in_intermediate_and_finish(test_context *ctx) {
    /* First frame of the fade: every channel is at FLASH_BRIGHT and the eyes
       and nose that complete Sonic's face are up with his pose and hand. */
    reset_fixture();
    nFadoSeqNum = 7;
    nFadoTime = FLASH_WHITE_FRAMES + FLASH_FADE_FRAMES;
    nFadoValu = 32;
    nTimerCunt = 0;
    tblPal1[0].peRed = 250;
    tblPal1[0].peGreen = 251;
    tblPal1[0].peBlue = 252;

    OEFadeIn();
    TEST_ASSERT_EQ_INT(ctx, 1, all_spr_delete_call_count);
    TEST_ASSERT_EQ_INT(ctx, 3, spr_create_call_count);
    TEST_ASSERT_EQ_INT(ctx, 7, spr_create_values[0]);
    TEST_ASSERT_EQ_INT(ctx, 12, spr_create_values[1]);
    TEST_ASSERT_EQ_INT(ctx, 11, spr_create_values[2]);
    TEST_ASSERT_EQ_INT(ctx, 0, bDrawDisable);
    TEST_ASSERT_EQ_INT(ctx, 4, clrset_call_count);
    TEST_ASSERT_EQ_INT(ctx, 0, clrset_numbers[0]);
    TEST_ASSERT_EQ_INT(ctx, 3, clrset_numbers[3]);
    TEST_ASSERT_EQ_INT(ctx, 250, workPalet[0][0].peRed);
    TEST_ASSERT_EQ_INT(ctx, 251, workPalet[0][0].peGreen);
    TEST_ASSERT_EQ_INT(ctx, 252, workPalet[0][0].peBlue);
    TEST_ASSERT_EQ_INT(ctx, tblPal4[15].peFlags, workPalet[3][15].peFlags);

    /* Last frame of the fade: the opening's own colours are back, so nothing
       moves as the title screen takes over. */
    reset_fixture();
    nFadoSeqNum = 7;
    nFadoTime = FLASH_WHITE_FRAMES + FLASH_FADE_FRAMES;
    nTimerCunt = FLASH_WHITE_FRAMES + FLASH_FADE_FRAMES - 1;
    OEFadeIn();
    for (Sint32 i = 0; i < 16; ++i) {
        TEST_ASSERT_EQ_INT(ctx, tblPal1[i].peRed, workPalet[0][i].peRed);
        TEST_ASSERT_EQ_INT(ctx, tblPal1[i].peGreen, workPalet[0][i].peGreen);
        TEST_ASSERT_EQ_INT(ctx, tblPal1[i].peBlue, workPalet[0][i].peBlue);
        TEST_ASSERT_EQ_INT(ctx, tblPal4[i].peRed, workPalet[3][i].peRed);
        TEST_ASSERT_EQ_INT(ctx, tblPal4[i].peGreen, workPalet[3][i].peGreen);
        TEST_ASSERT_EQ_INT(ctx, tblPal4[i].peBlue, workPalet[3][i].peBlue);
    }

    reset_fixture();
    nFadoSeqNum = 55;
    nFadoTime = 5;
    nFadoValu = 32;
    nTimerCunt = 5;
    OEFadeIn();
    TEST_ASSERT_EQ_INT(ctx, 4, clrset_call_count);
    TEST_ASSERT_TRUE(ctx, clrset_palettes[0] == tblPal1);
    TEST_ASSERT_TRUE(ctx, clrset_palettes[3] == tblPal4);
    TEST_ASSERT_EQ_INT(ctx, 55, set_sequence_values[0]);
}

static void test_fade_in_releases_the_channels_in_turn(test_context *ctx) {
    static const Sint32 step = FLASH_FADE_FRAMES / 3;

    /* White first: every channel is at FLASH_BRIGHT for the first frames. */
    for (Sint32 frame = 0; frame < FLASH_WHITE_FRAMES; ++frame) {
        reset_fixture();
        nFadoSeqNum = 7;
        nFadoTime = FLASH_WHITE_FRAMES + FLASH_FADE_FRAMES;
        nTimerCunt = frame;
        OEFadeIn();
        TEST_ASSERT_EQ_INT(ctx, FLASH_BRIGHT, workPalet[0][0].peRed);
        TEST_ASSERT_EQ_INT(ctx, FLASH_BRIGHT, workPalet[0][0].peGreen);
        TEST_ASSERT_EQ_INT(ctx, FLASH_BRIGHT, workPalet[0][0].peBlue);
    }

    /* Then blue returns on its own (white turns yellow)... */
    reset_fixture();
    nFadoSeqNum = 7;
    nFadoTime = FLASH_WHITE_FRAMES + FLASH_FADE_FRAMES;
    nTimerCunt = FLASH_WHITE_FRAMES + step - 1;
    OEFadeIn();
    TEST_ASSERT_EQ_INT(ctx, tblPal1[0].peBlue, workPalet[0][0].peBlue);
    TEST_ASSERT_EQ_INT(ctx, FLASH_BRIGHT, workPalet[0][0].peGreen);
    TEST_ASSERT_EQ_INT(ctx, FLASH_BRIGHT, workPalet[0][0].peRed);

    /* ...green next (yellow turns red)... */
    nTimerCunt = FLASH_WHITE_FRAMES + step * 2 - 1;
    OEFadeIn();
    TEST_ASSERT_EQ_INT(ctx, tblPal1[0].peBlue, workPalet[0][0].peBlue);
    TEST_ASSERT_EQ_INT(ctx, tblPal1[0].peGreen, workPalet[0][0].peGreen);
    TEST_ASSERT_EQ_INT(ctx, FLASH_BRIGHT, workPalet[0][0].peRed);

    /* ...and red last, which is the opening's own palette again. */
    nTimerCunt = FLASH_WHITE_FRAMES + step * 3 - 1;
    OEFadeIn();
    TEST_ASSERT_EQ_INT(ctx, tblPal1[0].peBlue, workPalet[0][0].peBlue);
    TEST_ASSERT_EQ_INT(ctx, tblPal1[0].peGreen, workPalet[0][0].peGreen);
    TEST_ASSERT_EQ_INT(ctx, tblPal1[0].peRed, workPalet[0][0].peRed);
}

static void test_flash_sprites_create_copies_outside_the_faded_lines(
    test_context *ctx) {
    reset_fixture();

    OEFlashSpritesCreate();

    TEST_ASSERT_EQ_INT(ctx, (Sint32)NUM_FLASH_SPRT, duplicate_call_count);
    TEST_ASSERT_EQ_INT(ctx, 8, duplicate_index[0]);
    TEST_ASSERT_EQ_INT(ctx, 11, duplicate_index[3]);
    TEST_ASSERT_EQ_INT(ctx, 12, duplicate_index[4]);
    TEST_ASSERT_EQ_INT(ctx, 13, duplicate_index[5]);
    TEST_ASSERT_EQ_INT(ctx, 23, duplicate_index[6]);
    for (Sint32 i = 0; i < duplicate_call_count; ++i) {
        /* Sonic's sprites are cut from the fourth palette line... */
        TEST_ASSERT_EQ_INT(ctx, 64, duplicate_palette[i]);
        /* ...and their copies take their colours from outside it. */
        TEST_ASSERT_EQ_INT(ctx, 144, duplicate_unfaded[i]);
        TEST_ASSERT_TRUE(ctx, flashSprtBmp[i] ==
                                  (hmx_bitmap *)&fake_flash_storage[i]);
    }
}

static void test_fade_in_draws_sonic_from_the_unfaded_copies(test_context *ctx) {
    reset_fixture();
    OEFlashSpritesCreate();
    for (Sint32 i = 0; i < (Sint32)NUM_FLASH_SPRT; ++i)
        sprite_handles[flashSprtIndx[i]] = (Uint32)(i + 1);
    sprite_handles[14] = 40; /* Little Planet flashes with the screen */
    nFadoSeqNum = 7;
    nFadoTime = FLASH_WHITE_FRAMES + FLASH_FADE_FRAMES;
    nTimerCunt = 4;

    OEFadeIn();

    TEST_ASSERT_EQ_INT(ctx, (Sint32)NUM_FLASH_SPRT, bitmap_call_count);
    for (Sint32 i = 0; i < bitmap_call_count; ++i) {
        TEST_ASSERT_TRUE(ctx, bitmap_sprites[i] == context.sprites[i + 1]);
        TEST_ASSERT_TRUE(ctx, bitmap_values[i] ==
                                  (hmx_bitmap *)&fake_flash_storage[i]);
    }

    /* The frame that hands over to the title screen uses the real bitmaps. */
    reset_fixture();
    nFadoSeqNum = 7;
    nFadoTime = 5;
    nTimerCunt = 5;
    OEFadeIn();
    TEST_ASSERT_EQ_INT(ctx, 0, bitmap_call_count);
}

static void test_animation_update_helpers(test_context *ctx) {
    reset_fixture();
    OEUpdateHand();
    TEST_ASSERT_EQ_INT(ctx, 0, spr_create_call_count);
    nTimerCunt = 1;
    OEUpdateHand();
    TEST_ASSERT_EQ_INT(ctx, 11, spr_create_values[0]);
    nTimerCunt = 18;
    OEUpdateHand();
    TEST_ASSERT_EQ_INT(ctx, 10, spr_create_values[1]);

    reset_fixture();
    OEUpdateMayu();
    TEST_ASSERT_EQ_INT(ctx, 12, spr_create_values[0]);
    nTimerCunt = 18;
    OEUpdateMayu();
    TEST_ASSERT_EQ_INT(ctx, 12, spr_create_values[1]);
    nTimerCunt = 19;
    OEUpdateMayu();
    TEST_ASSERT_EQ_INT(ctx, 2, spr_create_call_count);

    reset_fixture();
    sprite_handles[14] = 9;
    OEUpdatePlanet();
    TEST_ASSERT_TRUE(ctx, position_sprites[0] == sprite_handle(9));
    TEST_ASSERT_EQ_INT(ctx, 240, position_x[0]);
    TEST_ASSERT_EQ_INT(ctx, -33, position_y[0]);
    nTimerCunt = 16;
    OEUpdatePlanet();
    TEST_ASSERT_EQ_INT(ctx, -33, position_y[1]);
    nTimerCunt = 17;
    OEUpdatePlanet();
    TEST_ASSERT_EQ_INT(ctx, -34, position_y[2]);
    nTimerCunt = 32;
    OEUpdatePlanet();
    nTimerCunt = 48;
    OEUpdatePlanet();
    nTimerCunt = 84;
    OEUpdatePlanet();
    nTimerCunt = 100;
    OEUpdatePlanet();
    nTimerCunt = 116;
    OEUpdatePlanet();
    nTimerCunt = 128;
    OEUpdatePlanet();
    nTimerCunt = 129;
    OEUpdatePlanet();
    TEST_ASSERT_EQ_INT(ctx, 240, position_x[position_call_count - 1]);
    TEST_ASSERT_EQ_INT(ctx, -33, position_y[position_call_count - 1]);

    reset_fixture();
    grid_handles[1] = 1;
    OEUpdateMizu();
    TEST_ASSERT_EQ_INT(ctx, 2, horz_offsets_call_count);
    TEST_ASSERT_EQ_INT(ctx, 0, horz_offsets_first[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, horz_offsets_first[1]);
    TEST_ASSERT_EQ_INT(ctx, -6, horz_offsets_last[1]);
    nTimerCunt = 1;
    OEUpdateMizu();
    TEST_ASSERT_EQ_INT(ctx, 2, horz_offsets_call_count);
    nTimerCunt = 2;
    OEUpdateMizu();
    TEST_ASSERT_EQ_INT(ctx, 3, horz_offsets_call_count);
    for (Sint32 i = 4; i < 130; i += 2) {
        nTimerCunt = i;
        OEUpdateMizu();
    }
    TEST_ASSERT_TRUE(ctx, horz_offsets_last[horz_offsets_call_count - 1] > -320);

    reset_fixture();
    OEUpdateKumo();
    TEST_ASSERT_EQ_INT(ctx, 92, scan_call_count);
    TEST_ASSERT_EQ_INT(ctx, 0, scan_index[0]);
    TEST_ASSERT_EQ_INT(ctx, 91, scan_index[91]);
    TEST_ASSERT_EQ_INT(ctx, 0, scan_delta_y[91]);
    TEST_ASSERT_TRUE(ctx, scan_start_y[91] > scan_start_y[0]);
    for (Sint32 i = 0; i < 127; ++i) {
        nTimerCunt = i + 1;
        OEUpdateKumo();
    }
    TEST_ASSERT_EQ_INT(ctx, 128 * 92, scan_call_count);
}

static void drive_hidden_sequence(const Uint16 *inputs, Sint32 count) {
    nTimerCunt = 0;
    swData1 = 0;
    OEUpdateMsg();
    for (Sint32 i = 0; i < count; ++i) {
        swData1 = inputs[i];
        nTimerCunt += 1;
        OEUpdateMsg();
        if (i != count - 1) {
            swData1 = 0;
            nTimerCunt += 8;
            OEUpdateMsg();
        }
    }
}

static void assert_hidden_sequence_rejects(test_context *ctx,
                                           const Uint16 *inputs,
                                           Sint32 count) {
    reset_fixture();
    drive_hidden_sequence(inputs, count);
    TEST_ASSERT_EQ_INT(ctx, 0, set_sequence_call_count);
    TEST_ASSERT_EQ_INT(ctx, 0, nMenuKind);
}

static void open_menu_to_stable(Sint32 end_time) {
    nTimerCunt = 0;
    swData1 = 0;
    OEUpdateMsg();
    swData1 = 16384;
    nTimerCunt = 1;
    OEUpdateMsg();
    swData1 = 0;
    for (Sint32 i = 2; i <= end_time; ++i) {
        nTimerCunt = i;
        OEUpdateMsg();
    }
}

static void test_title_calls_all_update_helpers(test_context *ctx) {
    reset_fixture();
    sprite_handles[14] = 14;
    grid_handles[1] = 1;

    OETitle();

    TEST_ASSERT_EQ_INT(ctx, 1, all_spr_delete_call_count);
    TEST_ASSERT_EQ_INT(ctx, 7, spr_create_values[0]);
    TEST_ASSERT_EQ_INT(ctx, 2, grid_create_values[0]);
    TEST_ASSERT_TRUE(ctx, position_call_count > 0);
    TEST_ASSERT_TRUE(ctx, horz_offsets_call_count > 0);
    TEST_ASSERT_EQ_INT(ctx, 92, scan_call_count);
}

static void test_menu_update_timeout_cheats_and_selection(test_context *ctx) {
    reset_fixture();
    OEUpdateMsg();
    TEST_ASSERT_EQ_INT(ctx, 0, spr_create_index_values[0]);
    nTimerCunt = 20;
    swData1 = 0;
    OEUpdateMsg();
    TEST_ASSERT_EQ_INT(ctx, 24, spr_create_index_values[0]);

    reset_fixture();
    nTimerCunt = 900;
    OEUpdateMsg();
    TEST_ASSERT_EQ_INT(ctx, 8, set_sequence_values[0]);
    TEST_ASSERT_EQ_INT(ctx, 7, nMenuKind);

    reset_fixture();
    static const Uint16 debug1[] = {512, 512, 512, 1024, 2048, 16384};
    drive_hidden_sequence(debug1, 6);
    TEST_ASSERT_EQ_INT(ctx, 8, set_sequence_values[0]);
    TEST_ASSERT_EQ_INT(ctx, 8, nMenuKind);

    reset_fixture();
    static const Uint16 debug2[] = {256, 512, 512, 1024, 2048, 16384};
    drive_hidden_sequence(debug2, 6);
    TEST_ASSERT_EQ_INT(ctx, 9, nMenuKind);

    reset_fixture();
    static const Uint16 debug3[] = {2048, 2048, 256, 256, 512, 16384};
    drive_hidden_sequence(debug3, 6);
    TEST_ASSERT_EQ_INT(ctx, 10, nMenuKind);

    reset_fixture();
    static const Uint16 wrong_debug[] = {512, 256, 512, 1024, 2048, 16384};
    drive_hidden_sequence(wrong_debug, 6);
    TEST_ASSERT_EQ_INT(ctx, 0, set_sequence_call_count);

    reset_fixture();
    read_score_returns[0] = 1;
    read_score_values[0].saved = 1;
    read_score_values[4].saved = 1;
    crntScorData.saved = 1;
    crntScorData.roundNo = 1;
    nTimerCunt = 0;
    swData1 = 0;
    OEUpdateMsg();
    swData1 = 16384;
    nTimerCunt = 1;
    OEUpdateMsg();
    swData1 = 0;
    for (Sint32 i = 2; i < 40; ++i) {
        nTimerCunt = i;
        OEUpdateMsg();
    }
    swData1 = 16384;
    nTimerCunt = 40;
    OEUpdateMsg();
    TEST_ASSERT_EQ_INT(ctx, 8, set_sequence_values[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, nMenuKind);

    reset_fixture();
    crntScorData.saved = 1;
    crntScorData.roundNo = 7;
    crntScorData.total = 90000;
    sprite_handles[32] = 32;
    sprite_handles[33] = 33;
    open_menu_to_stable(40);
    for (Sint32 i = 41; i <= 60; ++i) {
        nTimerCunt = i;
        swData1 = 0;
        OEUpdateMsg();
    }
    TEST_ASSERT_TRUE(ctx, position_call_count > 0);
    swData1 = 2048;
    nTimerCunt = 61;
    OEUpdateMsg();
    swData1 = 0;
    for (Sint32 i = 62; i <= 90; ++i) {
        nTimerCunt = i;
        OEUpdateMsg();
    }
    TEST_ASSERT_TRUE(ctx, position_call_count > 0);

    swData1 = 1024;
    nTimerCunt = 91;
    OEUpdateMsg();
    swData1 = 0;
    for (Sint32 i = 92; i <= 120; ++i) {
        nTimerCunt = i;
        OEUpdateMsg();
    }
    TEST_ASSERT_TRUE(ctx, spr_create_index_call_count > 0);

    reset_fixture();
    open_menu_to_stable(40);
    for (Sint32 i = 0; i < 6; ++i) {
        read_score_returns[i] = 1;
        read_score_values[i].saved = 1;
    }
    swData1 = 16384;
    nTimerCunt = 41;
    OEUpdateMsg();
    TEST_ASSERT_EQ_INT(ctx, 28, wave_values[0]);
}

static void test_menu_update_rejects_each_hidden_code_wrong_turn(
    test_context *ctx) {
    static const Uint16 fail_case2[] = {512, 256};
    static const Uint16 fail_case3[] = {512, 512, 256};
    static const Uint16 fail_case4[] = {512, 512, 512, 512};
    static const Uint16 fail_case5[] = {512, 512, 512, 1024, 512};
    static const Uint16 fail_case6[] = {512, 512, 512, 1024, 2048, 512};
    static const Uint16 fail_case12[] = {256, 256};
    static const Uint16 fail_case13[] = {256, 512, 256};
    static const Uint16 fail_case14[] = {256, 512, 512, 512};
    static const Uint16 fail_case15[] = {256, 512, 512, 1024, 512};
    static const Uint16 fail_case16[] = {256, 512, 512, 1024, 2048, 512};
    static const Uint16 fail_case20[] = {2048, 512};
    static const Uint16 fail_case21[] = {2048, 2048, 512};
    static const Uint16 fail_case22[] = {2048, 2048, 256, 512};
    static const Uint16 fail_case23[] = {2048, 2048, 256, 256, 256};
    static const Uint16 fail_case24[] = {2048, 2048, 256, 256, 512, 512};

    assert_hidden_sequence_rejects(ctx, fail_case2, 2);
    assert_hidden_sequence_rejects(ctx, fail_case3, 3);
    assert_hidden_sequence_rejects(ctx, fail_case4, 4);
    assert_hidden_sequence_rejects(ctx, fail_case5, 5);
    assert_hidden_sequence_rejects(ctx, fail_case6, 6);
    assert_hidden_sequence_rejects(ctx, fail_case12, 2);
    assert_hidden_sequence_rejects(ctx, fail_case13, 3);
    assert_hidden_sequence_rejects(ctx, fail_case14, 4);
    assert_hidden_sequence_rejects(ctx, fail_case15, 5);
    assert_hidden_sequence_rejects(ctx, fail_case16, 6);
    assert_hidden_sequence_rejects(ctx, fail_case20, 2);
    assert_hidden_sequence_rejects(ctx, fail_case21, 3);
    assert_hidden_sequence_rejects(ctx, fail_case22, 4);
    assert_hidden_sequence_rejects(ctx, fail_case23, 5);
    assert_hidden_sequence_rejects(ctx, fail_case24, 6);
}

static void test_menu_update_selection_start_when_menu_text_visible(
    test_context *ctx) {
    reset_fixture();

    nTimerCunt = 0;
    swData1 = 0;
    OEUpdateMsg();
    nTimerCunt = 20;
    OEUpdateMsg();
    nTimerCunt = 21;
    swData1 = 16384;
    OEUpdateMsg();

    TEST_ASSERT_EQ_INT(ctx, 24, spr_create_index_values[1]);
    TEST_ASSERT_TRUE(ctx, clrset_palettes[0] == tblPal2a);
}

static void test_end_returns_menu_results_only_on_first_tick(test_context *ctx) {
    static const Sint32 returns[] = {1, 2, 3, 4, 5, 6, 21, 7, 13, 14, 20};

    reset_fixture();
    nTimerCunt = 1;
    nMenuKind = 0;
    TEST_ASSERT_EQ_INT(ctx, 0, OEEnd());
    TEST_ASSERT_EQ_INT(ctx, 0, cd_pause_call_count);

    for (Sint32 i = 0; i <= 10; ++i) {
        reset_fixture();
        nTimerCunt = 0;
        nMenuKind = (Uint16)i;
        TEST_ASSERT_EQ_INT(ctx, returns[i], OEEnd());
        TEST_ASSERT_EQ_INT(ctx, 2, cd_pause_values[0]);
    }
}

TEST_MAIN_BEGIN;
test_initial_hand_update_reuses_first_pattern(&ctx);
test_draw_respects_sequence_and_draw_disable(&ctx);
test_score_slot_helpers_and_enabled_menu_table(&ctx);
test_get_next_menu_forward_backward_and_initial(&ctx);
test_start_open_logo_dark_and_sonic_sequences(&ctx);
test_fade_out_copies_darkens_and_finishes(&ctx);
test_fade_in_intermediate_and_finish(&ctx);
test_fade_in_releases_the_channels_in_turn(&ctx);
test_flash_sprites_create_copies_outside_the_faded_lines(&ctx);
test_fade_in_draws_sonic_from_the_unfaded_copies(&ctx);
test_animation_update_helpers(&ctx);
test_title_calls_all_update_helpers(&ctx);
test_menu_update_timeout_cheats_and_selection(&ctx);
test_menu_update_rejects_each_hidden_code_wrong_turn(&ctx);
test_menu_update_selection_start_when_menu_text_visible(&ctx);
test_end_returns_menu_results_only_on_first_tick(&ctx);
TEST_MAIN_END
