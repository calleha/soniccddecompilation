#include <string.h>

#include "support/test_runner.h"
#include "src/title/common/engine_dll.h"
#include "src/title/common/hmx_types.h"
#include "src/types.h"

static draw_context draw_ctx;
static char env_storage;
static char loader_storage;
PALETTEENTRY tblPal1[16];
PALETTEENTRY tblPal2[16];
PALETTEENTRY tblPal3[16];
PALETTEENTRY tblPal4[16];

static Sint32 clrset_call_count;
static PALETTEENTRY *clrset_palettes[4];
static Uint16 clrset_numbers[4];
static Sint32 create_ea_call_count;
static Sint32 flash_sprites_create_call_count;
static Sint32 delete_ea_call_count;
static Sint32 open_sta_call_count;
static Sint32 sega_logo_call_count;
static Sint32 dark_call_count;
static Sint32 sonic_call_count;
static Sint32 title_sta_call_count;
static Sint32 title_call_count;
static Sint32 fade_out_call_count;
static Sint32 fade_in_call_count;
static Sint32 draw_call_count;
static Sint32 end_call_count;
static Sint32 end_return;
static Sint32 cd_play_call_count;
static Sint16 cd_play_values[4];
static Sint32 cd_pause_call_count;
static Sint32 wave_request_call_count;
static Sint16 wave_request_values[4];
static draw_context *draw_context_return;

#include "src/title/opening/opnentry.c"

void OEClrset(PALETTEENTRY *pPalet, Uint16 nNumber) {
    clrset_palettes[clrset_call_count] = pPalet;
    clrset_numbers[clrset_call_count] = nNumber;
    ++clrset_call_count;
}

Sint32 OECreateEA(void) {
    ++create_ea_call_count;
    return 0;
}

void OEFlashSpritesCreate(void) { ++flash_sprites_create_call_count; }

void OEDeleteEA(void) { ++delete_ea_call_count; }

void OEOpenSta(void) { ++open_sta_call_count; }

void OESegaLogo(void) { ++sega_logo_call_count; }

void OEDark(void) { ++dark_call_count; }

void OESonic(void) { ++sonic_call_count; }

void OETitleSta(void) { ++title_sta_call_count; }

void OETitle(void) { ++title_call_count; }

void OEFadeOut(void) { ++fade_out_call_count; }

void OEFadeIn(void) { ++fade_in_call_count; }

void OEDraw(void) { ++draw_call_count; }

Sint32 OEEnd(void) {
    ++end_call_count;
    return end_return;
}

static void fake_mem_cpy(void *dst, void *src, Sint32 bytes) {
    memcpy(dst, src, (size_t)bytes);
}

static void fake_cd_play(Sint16 track) {
    cd_play_values[cd_play_call_count] = track;
    ++cd_play_call_count;
}

static void fake_cd_pause(void) { ++cd_pause_call_count; }

static void fake_wave_request(Sint16 request) {
    wave_request_values[wave_request_call_count] = request;
    ++wave_request_call_count;
}

static Uint32 fake_score_read(Sint32 slot, char *data, Uint32 mode) {
    (void)slot;
    (void)data;
    (void)mode;
    return 0;
}

static Uint32 fake_score_write(Sint32 slot, char *data, Uint32 mode) {
    (void)slot;
    (void)data;
    (void)mode;
    return 0;
}

static Uint32 fake_score_open(Sint32 slot) {
    (void)slot;
    return 0;
}

static Uint32 fake_score_close(Uint32 handle) {
    (void)handle;
    return 0;
}

static Uint32 fake_score_create(void) { return 0; }

static Sint32 fake_index_read(Uint32 slot) {
    (void)slot;
    return 0;
}

static Uint32 fake_index_write(Sint32 slot, Uint32 value) {
    (void)slot;
    (void)value;
    return 0;
}

static draw_context *fake_get_draw_context(void) { return draw_context_return; }

static void fake_void_function(void) {}
static void *fake_alloc(Sint32 size) {
    (void)size;
    return 0;
}
static void fake_mem_free(void *ptr) { (void)ptr; }
static Sint32 fake_open_file(char *path) {
    (void)path;
    return 0;
}
static Sint32 fake_int_function(void) { return 0; }
static void fake_sprite_set_flags(hmx_sprite *sprite, Sint32 flags) {
    (void)sprite;
    (void)flags;
}

static void reset_fixture(void) {
    memset(&draw_ctx, 0, sizeof(draw_ctx));
    memset(tblPal1, 0, sizeof(tblPal1));
    memset(tblPal2, 0, sizeof(tblPal2));
    memset(tblPal3, 0, sizeof(tblPal3));
    memset(tblPal4, 0, sizeof(tblPal4));
    memset(clrset_palettes, 0, sizeof(clrset_palettes));
    memset(clrset_numbers, 0, sizeof(clrset_numbers));
    memset(cd_play_values, 0, sizeof(cd_play_values));
    memset(wave_request_values, 0, sizeof(wave_request_values));

    g_env_module = 0;
    g_loader_module = 0;
    s_ctx = 0;
    sOutputDebugString = 0;
    sPrintf = 0;
    sCloseFile = 0;
    sReadFile = 0;
    sOpenFile = 0;
    sGetFileSize = 0;
    sMemCpy = 0;
    sMemFree = 0;
    sMemAlloc = 0;
    get_draw_context_module = 0;
    FlipToScreen_module = 0;
    ld_bitmap_4to8_module = 0;
    ld_load_cmpfile_module = 0;
    hmx_grid_create_module = 0;
    hmx_sprite_set_flags_module = 0;
    WriteIndx = 0;
    ReadIndx = 0;
    CreateScore = 0;
    CloseScore = 0;
    OpenScore = 0;
    WriteScore = 0;
    ReadScore = 0;
    WaveRequest = 0;
    CDPause = 0;
    CDPlay = 0;
    lpUserKey = 0;
    lpbFullScreen = 0;
    bFirstTitle = 0;
    swData2 = 0;
    swData1 = 0;
    selectIndx = 0;
    memset(&crntScorData, 0, sizeof(crntScorData));
    lpColorwk = 0;
    hWnd = 0;
    hSurf = 0;
    nTimerCunt = 0;
    nSequenceNum = 0;

    clrset_call_count = 0;
    create_ea_call_count = 0;
    flash_sprites_create_call_count = 0;
    delete_ea_call_count = 0;
    open_sta_call_count = 0;
    sega_logo_call_count = 0;
    dark_call_count = 0;
    sonic_call_count = 0;
    title_sta_call_count = 0;
    title_call_count = 0;
    fade_out_call_count = 0;
    fade_in_call_count = 0;
    draw_call_count = 0;
    end_call_count = 0;
    end_return = 21;
    cd_play_call_count = 0;
    cd_pause_call_count = 0;
    wave_request_call_count = 0;
    draw_context_return = &draw_ctx;
}

static void fill_func_table(void **funcs) {
    Sint32 i;

    for (i = 0; i < 64; ++i) {
        funcs[i] = fake_void_function;
    }

    funcs[11] = fake_alloc;
    funcs[12] = fake_mem_free;
    funcs[14] = fake_mem_cpy;
    funcs[20] = fake_void_function;
    funcs[21] = fake_void_function;
    funcs[22] = fake_open_file;
    funcs[23] = fake_int_function;
    funcs[24] = fake_void_function;
    funcs[25] = fake_int_function;
    funcs[26] = fake_get_draw_context;
    funcs[49] = fake_sprite_set_flags;
}

static void fill_buffer_table(char ***buffers, char **env_slot,
                              char **loader_slot) {
    Sint32 i;
    static char *dummy = 0;
    static char **dummy_slot = &dummy;
    static char *env_ptr;
    static char *loader_ptr;

    env_ptr = &env_storage;
    loader_ptr = &loader_storage;
    *env_slot = env_ptr;
    *loader_slot = loader_ptr;

    for (i = 0; i < 16; ++i) {
        buffers[i] = dummy_slot;
    }
    buffers[11] = env_slot;
    buffers[12] = loader_slot;
}

static void test_oeset_sequence_sets_timer_to_minus_one(test_context *ctx) {
    reset_fixture();

    nTimerCunt = 99;
    OESetSequenceNum(6);

    TEST_ASSERT_EQ_INT(ctx, 6, nSequenceNum);
    TEST_ASSERT_EQ_INT(ctx, -1, nTimerCunt);
}

static void test_dllinit_binds_imports_score_callbacks_and_initializes(
    test_context *ctx) {
    engine_dll dll;
    void *funcs[64];
    char **buffers[16];
    char *env_slot;
    char *loader_slot;
    PALETTEENTRY palette[64];
    Uint32 fullscreen = 1;
    Uint16 user_key = 0x55AA;
    score_data initial_score;

    reset_fixture();
    memset(&dll, 0, sizeof(dll));
    memset(funcs, 0, sizeof(funcs));
    memset(&initial_score, 0, sizeof(initial_score));
    fill_func_table(funcs);
    fill_buffer_table(buffers, &env_slot, &loader_slot);
    initial_score.saved = 1;
    initial_score.roundNo = 4;
    initial_score.total = 12345;
    dll.hWnd = 101;
    dll.hSurf = 202;
    dll.lpColorwk = palette;
    dll.lpbFullScreen = &fullscreen;
    dll.bFirstTitle = 77;
    dll.lpCrntScorData = &initial_score;
    dll.selectIndx = 5;
    dll.ReadScoreData = fake_score_read;
    dll.WriteScoreData = fake_score_write;
    dll.OpenScoreData = fake_score_open;
    dll.CloseScoreData = fake_score_close;
    dll.CreateScoreData = fake_score_create;
    dll.ReadScoreIndx = fake_index_read;
    dll.WriteScoreIndx = fake_index_write;
    dll.CDPlay = fake_cd_play;
    dll.CDPause = fake_cd_pause;
    dll.WaveRequest = fake_wave_request;
    dll.lpUserKey = &user_key;

    DLLInit(&dll, buffers, funcs);

    TEST_ASSERT_TRUE(ctx, sMemAlloc == fake_alloc);
    TEST_ASSERT_TRUE(ctx, sMemFree == fake_mem_free);
    TEST_ASSERT_TRUE(ctx, sMemCpy == fake_mem_cpy);
    TEST_ASSERT_TRUE(ctx, sOpenFile == fake_open_file);
    TEST_ASSERT_TRUE(ctx, get_draw_context_module == fake_get_draw_context);
    TEST_ASSERT_TRUE(ctx, hmx_sprite_set_flags_module == fake_sprite_set_flags);
    TEST_ASSERT_TRUE(ctx, g_env_module == (hmx_environment *)&env_storage);
    TEST_ASSERT_TRUE(ctx, g_loader_module == (hmx_environment *)&loader_storage);
    TEST_ASSERT_TRUE(ctx, s_ctx == &draw_ctx);
    TEST_ASSERT_EQ_INT(ctx, 0, hWnd);
    TEST_ASSERT_EQ_INT(ctx, 202, hSurf);
    TEST_ASSERT_TRUE(ctx, lpColorwk == palette);
    TEST_ASSERT_TRUE(ctx, lpbFullScreen == &fullscreen);
    TEST_ASSERT_EQ_INT(ctx, 77, bFirstTitle);
    TEST_ASSERT_TRUE(ctx, lpUserKey == &user_key);
    TEST_ASSERT_EQ_INT(ctx, 5, selectIndx);
    TEST_ASSERT_EQ_INT(ctx, 1, crntScorData.saved);
    TEST_ASSERT_EQ_INT(ctx, 4, crntScorData.roundNo);
    TEST_ASSERT_EQ_INT(ctx, 12345, crntScorData.total);
    TEST_ASSERT_TRUE(ctx, ReadScore == fake_score_read);
    TEST_ASSERT_TRUE(ctx, WriteScore == fake_score_write);
    TEST_ASSERT_TRUE(ctx, OpenScore == fake_score_open);
    TEST_ASSERT_TRUE(ctx, CloseScore == fake_score_close);
    TEST_ASSERT_TRUE(ctx, CreateScore == fake_score_create);
    TEST_ASSERT_TRUE(ctx, ReadIndx == fake_index_read);
    TEST_ASSERT_TRUE(ctx, WriteIndx == fake_index_write);
    TEST_ASSERT_TRUE(ctx, CDPlay == fake_cd_play);
    TEST_ASSERT_TRUE(ctx, CDPause == fake_cd_pause);
    TEST_ASSERT_TRUE(ctx, WaveRequest == fake_wave_request);
    TEST_ASSERT_EQ_INT(ctx, 4, clrset_call_count);
    TEST_ASSERT_TRUE(ctx, clrset_palettes[0] == tblPal1);
    TEST_ASSERT_TRUE(ctx, clrset_palettes[1] == tblPal2);
    TEST_ASSERT_TRUE(ctx, clrset_palettes[2] == tblPal3);
    TEST_ASSERT_TRUE(ctx, clrset_palettes[3] == tblPal4);
    TEST_ASSERT_EQ_INT(ctx, 0, clrset_numbers[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, clrset_numbers[1]);
    TEST_ASSERT_EQ_INT(ctx, 2, clrset_numbers[2]);
    TEST_ASSERT_EQ_INT(ctx, 3, clrset_numbers[3]);
    TEST_ASSERT_EQ_INT(ctx, 1, create_ea_call_count);
    TEST_ASSERT_EQ_INT(ctx, 1, flash_sprites_create_call_count);
    TEST_ASSERT_EQ_INT(ctx, 1, nSequenceNum);
    TEST_ASSERT_EQ_INT(ctx, 0, nTimerCunt);
}

static void test_dllend_deletes_engine_actor_system(test_context *ctx) {
    reset_fixture();

    DLLEnd();

    TEST_ASSERT_EQ_INT(ctx, 1, delete_ea_call_count);
}

static void test_swdata_set_copies_both_words(test_context *ctx) {
    ushort_union sw1;
    ushort_union sw2;

    reset_fixture();
    sw1.w = 0x1234;
    sw2.w = 0x4321;

    SWdataSet(sw1, sw2);

    TEST_ASSERT_EQ_INT(ctx, 0x1234, swData1);
    TEST_ASSERT_EQ_INT(ctx, 0x4321, swData2);
}

static void test_dllmain_dispatches_all_opening_sequences(test_context *ctx) {
    reset_fixture();

    nSequenceNum = 1;
    nTimerCunt = 10;
    TEST_ASSERT_EQ_INT(ctx, 0, DLLMain());
    TEST_ASSERT_EQ_INT(ctx, 1, open_sta_call_count);
    TEST_ASSERT_EQ_INT(ctx, 11, nTimerCunt);
    TEST_ASSERT_EQ_INT(ctx, 1, draw_call_count);

    nSequenceNum = 2;
    TEST_ASSERT_EQ_INT(ctx, 0, DLLMain());
    TEST_ASSERT_EQ_INT(ctx, 1, sega_logo_call_count);

    nSequenceNum = 3;
    TEST_ASSERT_EQ_INT(ctx, 0, DLLMain());
    TEST_ASSERT_EQ_INT(ctx, 1, dark_call_count);

    nSequenceNum = 4;
    TEST_ASSERT_EQ_INT(ctx, 0, DLLMain());

    nSequenceNum = 5;
    TEST_ASSERT_EQ_INT(ctx, 0, DLLMain());
    TEST_ASSERT_EQ_INT(ctx, 1, sonic_call_count);

    nSequenceNum = 6;
    TEST_ASSERT_EQ_INT(ctx, 0, DLLMain());
    TEST_ASSERT_EQ_INT(ctx, 1, title_sta_call_count);

    nSequenceNum = 7;
    TEST_ASSERT_EQ_INT(ctx, 0, DLLMain());
    TEST_ASSERT_EQ_INT(ctx, 1, title_call_count);

    nSequenceNum = 8;
    TEST_ASSERT_EQ_INT(ctx, 0, DLLMain());
    TEST_ASSERT_EQ_INT(ctx, 1, fade_out_call_count);

    nSequenceNum = 9;
    TEST_ASSERT_EQ_INT(ctx, 0, DLLMain());
    TEST_ASSERT_EQ_INT(ctx, 1, fade_in_call_count);

    nSequenceNum = 999;
    TEST_ASSERT_EQ_INT(ctx, 21, DLLMain());
    TEST_ASSERT_EQ_INT(ctx, 1, end_call_count);

    nSequenceNum = 777;
    TEST_ASSERT_EQ_INT(ctx, 0, DLLMain());
    TEST_ASSERT_EQ_INT(ctx, 11, draw_call_count);

    nSequenceNum = 0;
    TEST_ASSERT_EQ_INT(ctx, 0, DLLMain());
    TEST_ASSERT_EQ_INT(ctx, 12, draw_call_count);

    nSequenceNum = 10;
    TEST_ASSERT_EQ_INT(ctx, 0, DLLMain());
    TEST_ASSERT_EQ_INT(ctx, 13, draw_call_count);

    nSequenceNum = 1000;
    TEST_ASSERT_EQ_INT(ctx, 0, DLLMain());
    TEST_ASSERT_EQ_INT(ctx, 14, draw_call_count);
}

static void test_dllmain_wraps_max_timer_to_5184000(test_context *ctx) {
    reset_fixture();

    nSequenceNum = 777;
    nTimerCunt = 0x7FFFFFFF;

    TEST_ASSERT_EQ_INT(ctx, 0, DLLMain());

    TEST_ASSERT_EQ_INT(ctx, 5184000, nTimerCunt);
    TEST_ASSERT_EQ_INT(ctx, 1, draw_call_count);
}

TEST_MAIN_BEGIN;
test_oeset_sequence_sets_timer_to_minus_one(&ctx);
test_dllinit_binds_imports_score_callbacks_and_initializes(&ctx);
test_dllend_deletes_engine_actor_system(&ctx);
test_swdata_set_copies_both_words(&ctx);
test_dllmain_dispatches_all_opening_sequences(&ctx);
test_dllmain_wraps_max_timer_to_5184000(&ctx);
TEST_MAIN_END
