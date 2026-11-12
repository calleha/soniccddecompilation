#include <string.h>

#include "support/test_runner.h"
#include "src/title/common/hmx_types.h"
#include "src/types.h"

#ifdef _MSC_VER
#define __attribute__(x)
#endif

draw_context context;
draw_context *s_ctx;
sprite_bmp infoSprtBmp[64];
Uint16 NUM_BMP;
grid_bmp infoGridBmp[3];
Uint16 NUM_GRIDBMP;
hmx_environment *g_env_module;
hmx_environment *g_loader_module;
char *SprCmpFileName;
char *TileCmpFileName;
void (*FlipToScreen_module)(void);
void (*hmx_renderer_context_draw_module)(hmx_renderer_context *, hmx_surface *);
void (*hmx_renderer_context_add_module)(hmx_renderer_context *, Sint32,
                                        hmx_renderer_base *);
hmx_renderer_base *(*hmx_sprite_base_module)(hmx_sprite *);
hmx_renderer_base *(*hmx_grid_base_module)(hmx_grid *);
void (*hmx_renderer_context_clear_module)(hmx_renderer_context *);
void (*hmx_free_module)(hmx_environment *, void *);
void (*hmx_bitmap_set_transparency_module)(hmx_bitmap *, Sint32);
void *(*hmx_bitmap_get_scan0_module)(hmx_bitmap *);
hmx_bitmap *(*hmx_bitmap_create_module)(hmx_environment *, Sint32, Sint32);
void *(*ld_load_cmpfile_module)(hmx_environment *, char *);
void (*ld_bitmap_4to8_module)(void *, void *, Sint32, Sint32, Sint32, Sint32,
                              Sint32);
void (*hmx_bitmap_release_module)(hmx_environment *, hmx_bitmap *);
void (*hmx_sprite_set_flags_module)(hmx_sprite *, Sint32);
void (*hmx_sprite_set_bitmap_module)(hmx_sprite *, hmx_bitmap *);
void (*hmx_sprite_set_position_module)(hmx_sprite *, Sint32, Sint32);
void (*hmx_grid_release_module)(hmx_environment *, hmx_grid *);
hmx_grid *(*hmx_grid_create_module)(hmx_environment *, Sint32, Sint32, Sint32,
                                    Sint32);
Sint32 (*sOpenFile)(char *);
Sint32 (*sGetFileSize)(Sint32);
Sint32 (*sReadFile)(Sint32, void *, Sint32);
void (*sCloseFile)(Sint32);
void *(*sMemAlloc)(Sint32);
void (*sMemFree)(void *);

static char env_storage;
static char loader_storage;
static char renderer_storage;
static char screen_storage;
static char sprite_storage[1000];
static char grid_storage[8];
static char bitmap_storage[128];
static char base_storage[3000];
static hmx_sprite *sprite_table[1000];
static hmx_grid *grid_table[3];
static Uint32 sprite_handles[64];
static Uint32 grid_handles[3];
static Uint32 tile_handles[4];
static Uint32 *grid_bitmap_slots[4];
static unsigned char scan_storage[128][256];
static unsigned char file_storage[256];
static unsigned char file_alloc_storage[256];
/* Handed out by fake_mem_alloc(); the sprite copier asks for two blocks, so
   they have to be distinct. */
static unsigned char alloc_pool[8192];
static Sint32 alloc_pool_next;
static unsigned char cmp_storage[4096];
static unsigned char sprite_cmp_storage[4096];
static unsigned char grid_cmp_storage[4096];
static Sint32 file_open_return;
static Sint32 file_size_return;
static Sint32 file_read_return;
static Sint32 open_call_count;
static char open_file_name[64];
static Sint32 close_call_count;
static Sint32 alloc_call_count;
static Sint32 free_call_count;
static void *free_values[32];
static Sint32 flip_call_count;
static Sint32 clear_call_count;
static Sint32 draw_call_count;
static Sint32 add_call_count;
static Sint32 add_priorities[512];
static hmx_renderer_base *add_bases[512];
static Sint32 bitmap_create_call_count;
static Sint32 bitmap_create_widths[128];
static Sint32 bitmap_create_heights[128];
static Sint32 bitmap_release_call_count;
static hmx_bitmap *bitmap_release_values[1100];
static Sint32 bitmap_get_scan_call_count;
static Sint32 transparency_call_count;
static hmx_bitmap *transparency_bitmaps[128];
static Sint32 transparency_values[128];
static Sint32 load_cmp_call_count;
static char load_cmp_file_names[16][64];
static void *load_cmp_return;
static Sint32 bitmap_4to8_call_count;
static Sint32 bitmap_4to8_widths[64];
static Sint32 bitmap_4to8_heights[64];
static Sint32 position_call_count;
static hmx_sprite *position_sprites[64];
static Sint32 position_x[64];
static Sint32 position_y[64];
static Sint32 set_bitmap_call_count;
static hmx_sprite *set_bitmap_sprites[64];
static hmx_bitmap *set_bitmap_values[64];
static Sint32 flags_call_count;
static hmx_sprite *flags_sprites[64];
static Sint32 flags_values[64];
static Sint32 grid_release_call_count;
static hmx_grid *grid_release_values[16];
static Sint32 grid_create_call_count;
static Sint32 all_grid_delete_call_count;
static void *sprite_cmp_return;
static void *grid_cmp_return;

void OEAllGridDelete(void);

#include "src/title/common/hmx_oeeactl.c"

static hmx_sprite *sprite_handle(Sint32 handle) {
    return (hmx_sprite *)&sprite_storage[handle];
}

static hmx_grid *grid_handle(Sint32 handle) {
    return (hmx_grid *)&grid_storage[handle];
}

static hmx_bitmap *bitmap_handle(Sint32 handle) {
    return (hmx_bitmap *)&bitmap_storage[handle];
}

static hmx_renderer_base *base_handle(Sint32 handle) {
    return (hmx_renderer_base *)&base_storage[handle];
}

static Sint32 expected_prio_max(void) {
    return PRIO_MAX;
}

static Sint32 bitmap_handle_index(hmx_bitmap *bitmap) {
    Sint32 i;

    for (i = 0; i < 128; ++i) {
        if (bitmap == bitmap_handle(i))
            return i;
    }
    return 0;
}

static void fake_flip(void) { ++flip_call_count; }

static void fake_clear(hmx_renderer_context *context_arg) {
    (void)context_arg;
    ++clear_call_count;
}

static void fake_draw(hmx_renderer_context *context_arg, hmx_surface *screen) {
    (void)context_arg;
    (void)screen;
    ++draw_call_count;
}

static void fake_add(hmx_renderer_context *context_arg, Sint32 priority,
                     hmx_renderer_base *base) {
    (void)context_arg;
    add_priorities[add_call_count] = priority;
    add_bases[add_call_count] = base;
    ++add_call_count;
}

static hmx_renderer_base *fake_sprite_base(hmx_sprite *sprite) {
    Sint32 i;

    for (i = 0; i < 1000; ++i) {
        if (sprite_table[i] == sprite)
            return base_handle(1000 + i);
    }
    return base_handle(0);
}

static hmx_renderer_base *fake_grid_base(hmx_grid *grid) {
    Sint32 i;

    for (i = 0; i < 3; ++i) {
        if (grid_table[i] == grid)
            return base_handle(2000 + i);
    }
    return base_handle(0);
}

static void fake_hmx_free(hmx_environment *env, void *ptr) {
    (void)env;
    free_values[free_call_count] = ptr;
    ++free_call_count;
}

static hmx_bitmap *fake_bitmap_create(hmx_environment *env, Sint32 width,
                                      Sint32 height) {
    (void)env;
    bitmap_create_widths[bitmap_create_call_count] = width;
    bitmap_create_heights[bitmap_create_call_count] = height;
    ++bitmap_create_call_count;
    return bitmap_handle(bitmap_create_call_count);
}

static void *fake_bitmap_get_scan0(hmx_bitmap *bitmap) {
    ++bitmap_get_scan_call_count;
    return scan_storage[bitmap_handle_index(bitmap)];
}

static void fake_bitmap_set_transparency(hmx_bitmap *bitmap, Sint32 value) {
    transparency_bitmaps[transparency_call_count] = bitmap;
    transparency_values[transparency_call_count] = value;
    ++transparency_call_count;
}

static void *fake_load_cmpfile(hmx_environment *env, char *file) {
    (void)env;
    strcpy(load_cmp_file_names[load_cmp_call_count], file);
    ++load_cmp_call_count;
    if (sprite_cmp_return != 0 && strcmp(file, SprCmpFileName) == 0)
        return sprite_cmp_return;
    if (grid_cmp_return != 0 && strcmp(file, TileCmpFileName) == 0)
        return grid_cmp_return;
    return load_cmp_return;
}

static void fake_bitmap_4to8(void *dst, void *src, Sint32 wx, Sint32 wy,
                             Sint32 palet, Sint32 zero_chg, Sint32 flag4) {
    (void)dst;
    (void)src;
    (void)palet;
    (void)zero_chg;
    (void)flag4;
    bitmap_4to8_widths[bitmap_4to8_call_count] = wx;
    bitmap_4to8_heights[bitmap_4to8_call_count] = wy;
    ++bitmap_4to8_call_count;
}

static void fake_bitmap_release(hmx_environment *env, hmx_bitmap *bitmap) {
    (void)env;
    bitmap_release_values[bitmap_release_call_count] = bitmap;
    ++bitmap_release_call_count;
}

static void fake_sprite_set_position(hmx_sprite *sprite, Sint32 x, Sint32 y) {
    position_sprites[position_call_count] = sprite;
    position_x[position_call_count] = x;
    position_y[position_call_count] = y;
    ++position_call_count;
}

static void fake_sprite_set_bitmap(hmx_sprite *sprite, hmx_bitmap *bitmap) {
    set_bitmap_sprites[set_bitmap_call_count] = sprite;
    set_bitmap_values[set_bitmap_call_count] = bitmap;
    ++set_bitmap_call_count;
}

static void fake_sprite_set_flags(hmx_sprite *sprite, Sint32 flags) {
    flags_sprites[flags_call_count] = sprite;
    flags_values[flags_call_count] = flags;
    ++flags_call_count;
}

static void fake_grid_release(hmx_environment *env, hmx_grid *grid) {
    (void)env;
    grid_release_values[grid_release_call_count] = grid;
    ++grid_release_call_count;
}

static hmx_grid *fake_grid_create(hmx_environment *env, Sint32 a, Sint32 b,
                                  Sint32 c, Sint32 d) {
    (void)env;
    (void)a;
    (void)b;
    (void)c;
    (void)d;
    ++grid_create_call_count;
    return grid_handle(grid_create_call_count + 3);
}

void OEAllGridDelete(void) { ++all_grid_delete_call_count; }

static Sint32 fake_open_file(char *name) {
    strcpy(open_file_name, name);
    ++open_call_count;
    return file_open_return;
}

static Sint32 fake_get_file_size(Sint32 handle) {
    (void)handle;
    return file_size_return;
}

static Sint32 fake_read_file(Sint32 handle, void *dst, Sint32 bytes) {
    (void)handle;
    memcpy(dst, file_storage, (size_t)bytes);
    return file_read_return;
}

static void fake_close_file(Sint32 handle) {
    (void)handle;
    ++close_call_count;
}

static void *fake_mem_alloc(Sint32 bytes) {
    unsigned char *block;

    ++alloc_call_count;
    if (bytes < 0)
        return 0;
    if (alloc_pool_next + bytes > (Sint32)sizeof(alloc_pool))
        return 0;
    block = &alloc_pool[alloc_pool_next];
    alloc_pool_next += bytes;
    return block;
}

static void fake_mem_free(void *ptr) {
    free_values[free_call_count] = ptr;
    ++free_call_count;
}

static void write_le32(unsigned char *dst, Uint32 value) {
    dst[0] = (unsigned char)(value & 255);
    dst[1] = (unsigned char)((value >> 8) & 255);
    dst[2] = (unsigned char)((value >> 16) & 255);
    dst[3] = (unsigned char)((value >> 24) & 255);
}

static void reset_fixture(void) {
    Sint32 i;

    memset(&context, 0, sizeof(context));
    memset(infoSprtBmp, 0, sizeof(infoSprtBmp));
    memset(infoGridBmp, 0, sizeof(infoGridBmp));
    memset(sprite_handles, 0, sizeof(sprite_handles));
    memset(grid_handles, 0, sizeof(grid_handles));
    memset(tile_handles, 0, sizeof(tile_handles));
    memset(grid_bitmap_slots, 0, sizeof(grid_bitmap_slots));
    memset(scan_storage, 0, sizeof(scan_storage));
    memset(file_storage, 0, sizeof(file_storage));
    memset(file_alloc_storage, 0, sizeof(file_alloc_storage));
    memset(alloc_pool, 0, sizeof(alloc_pool));
    memset(cmp_storage, 0, sizeof(cmp_storage));
    memset(sprite_cmp_storage, 0, sizeof(sprite_cmp_storage));
    memset(grid_cmp_storage, 0, sizeof(grid_cmp_storage));
    memset(open_file_name, 0, sizeof(open_file_name));
    memset(free_values, 0, sizeof(free_values));
    memset(add_priorities, 0, sizeof(add_priorities));
    memset(add_bases, 0, sizeof(add_bases));
    memset(bitmap_create_widths, 0, sizeof(bitmap_create_widths));
    memset(bitmap_create_heights, 0, sizeof(bitmap_create_heights));
    memset(bitmap_release_values, 0, sizeof(bitmap_release_values));
    memset(transparency_bitmaps, 0, sizeof(transparency_bitmaps));
    memset(transparency_values, 0, sizeof(transparency_values));
    memset(load_cmp_file_names, 0, sizeof(load_cmp_file_names));
    memset(bitmap_4to8_widths, 0, sizeof(bitmap_4to8_widths));
    memset(bitmap_4to8_heights, 0, sizeof(bitmap_4to8_heights));
    memset(position_sprites, 0, sizeof(position_sprites));
    memset(position_x, 0, sizeof(position_x));
    memset(position_y, 0, sizeof(position_y));
    memset(set_bitmap_sprites, 0, sizeof(set_bitmap_sprites));
    memset(set_bitmap_values, 0, sizeof(set_bitmap_values));
    memset(flags_sprites, 0, sizeof(flags_sprites));
    memset(flags_values, 0, sizeof(flags_values));
    memset(grid_release_values, 0, sizeof(grid_release_values));
    memset(sprBmpW, 0, sizeof(sprBmpW));
    memset(sprBmpH, 0, sizeof(sprBmpH));
    memset(sprUnfadedBmp, 0, sizeof(sprUnfadedBmp));
    sprUnfadedNum = 0;
    alloc_pool_next = 0;

    for (i = 0; i < 1000; ++i) {
        sprite_table[i] = sprite_handle(i);
    }
    for (i = 0; i < 3; ++i) {
        grid_table[i] = grid_handle(i);
        context.grids[i] = grid_table[i];
        infoGridBmp[i].lphGrid = &grid_handles[i];
    }
    for (i = 0; i < 64; ++i) {
        infoSprtBmp[i].w = (Uint16)(8 + i);
        infoSprtBmp[i].h = (Uint16)(4 + i);
        infoSprtBmp[i].point.x = i * 3;
        infoSprtBmp[i].point.y = i * 5;
        infoSprtBmp[i].order = i;
        infoSprtBmp[i].palOff = (Uint8)i;
        infoSprtBmp[i].timing = (Uint16)(100 + i % 2);
        infoSprtBmp[i].lphSpr = &sprite_handles[i];
    }
    for (i = 0; i < 4; ++i) {
        grid_bitmap_slots[i] = &tile_handles[i];
    }
    infoGridBmp[0].totalNum = 2;
    infoGridBmp[0].w = 8;
    infoGridBmp[0].h = 4;
    infoGridBmp[0].palOff = 16;
    infoGridBmp[0].lphBmp = grid_bitmap_slots;
    infoGridBmp[1].totalNum = 1;
    infoGridBmp[1].w = 9;
    infoGridBmp[1].h = 5;
    infoGridBmp[1].palOff = 32;
    infoGridBmp[1].lphBmp = grid_bitmap_slots + 2;

    context.screen = (hmx_surface *)&screen_storage;
    context.context = (hmx_renderer_context *)&renderer_storage;
    context.sprites = sprite_table;
    s_ctx = &context;
    g_env_module = (hmx_environment *)&env_storage;
    g_loader_module = (hmx_environment *)&loader_storage;
    SprCmpFileName = "SPR.CM_";
    TileCmpFileName = "TILE.CM_";
    NUM_BMP = 4;
    NUM_GRIDBMP = 2;
    FlipToScreen_module = fake_flip;
    hmx_renderer_context_clear_module = fake_clear;
    hmx_renderer_context_draw_module = fake_draw;
    hmx_renderer_context_add_module = fake_add;
    hmx_sprite_base_module = fake_sprite_base;
    hmx_grid_base_module = fake_grid_base;
    hmx_free_module = fake_hmx_free;
    hmx_bitmap_create_module = fake_bitmap_create;
    hmx_bitmap_get_scan0_module = fake_bitmap_get_scan0;
    hmx_bitmap_set_transparency_module = fake_bitmap_set_transparency;
    ld_load_cmpfile_module = fake_load_cmpfile;
    ld_bitmap_4to8_module = fake_bitmap_4to8;
    hmx_bitmap_release_module = fake_bitmap_release;
    hmx_sprite_set_position_module = fake_sprite_set_position;
    hmx_sprite_set_bitmap_module = fake_sprite_set_bitmap;
    hmx_sprite_set_flags_module = fake_sprite_set_flags;
    hmx_grid_release_module = fake_grid_release;
    hmx_grid_create_module = fake_grid_create;
    sOpenFile = fake_open_file;
    sGetFileSize = fake_get_file_size;
    sReadFile = fake_read_file;
    sCloseFile = fake_close_file;
    sMemAlloc = fake_mem_alloc;
    sMemFree = fake_mem_free;

    file_open_return = 7;
    file_size_return = 64;
    file_read_return = 64;
    load_cmp_return = 0;
    sprite_cmp_return = 0;
    grid_cmp_return = 0;
    open_call_count = 0;
    close_call_count = 0;
    alloc_call_count = 0;
    free_call_count = 0;
    flip_call_count = 0;
    clear_call_count = 0;
    draw_call_count = 0;
    add_call_count = 0;
    bitmap_create_call_count = 0;
    bitmap_release_call_count = 0;
    bitmap_get_scan_call_count = 0;
    transparency_call_count = 0;
    load_cmp_call_count = 0;
    bitmap_4to8_call_count = 0;
    position_call_count = 0;
    set_bitmap_call_count = 0;
    flags_call_count = 0;
    grid_release_call_count = 0;
    grid_create_call_count = 0;
    all_grid_delete_call_count = 0;
    memset(sprIdx, 0, sizeof(sprIdx));
}

static void prepare_bmp(Sint32 width, Sint32 height, Uint8 a, Uint8 b,
                        Uint8 c, Uint8 d) {
    memset(file_storage, 0, sizeof(file_storage));
    file_storage[10] = 54;
    write_le32(&file_storage[18], (Uint32)width);
    write_le32(&file_storage[22], (Uint32)height);
    file_storage[54] = a;
    file_storage[55] = b;
    file_storage[56] = c;
    file_storage[57] = d;
}

static ld_sprite_header *prepare_sprite_cmp(Sint32 count) {
    ld_sprite_header *header = (ld_sprite_header *)cmp_storage;
    ld_sprite_inf *spr = header->spr;

    memset(cmp_storage, 0, sizeof(cmp_storage));
    header->header.count = count;
    header->header.offset = sizeof(ld_pack_header) +
                            sizeof(ld_sprite_inf) * (Uint32)count;
    for (Sint32 i = 0; i < count; ++i) {
        spr[i].wx = (Sint16)(8 + i);
        spr[i].wy = (Sint16)(4 + i);
    }
    return header;
}

static ld_scroll_header *prepare_grid_cmp(Sint32 count) {
    ld_scroll_header *header = (ld_scroll_header *)cmp_storage;

    memset(cmp_storage, 0, sizeof(cmp_storage));
    header->header.count = count;
    header->header.offset =
        sizeof(ld_pack_header) + sizeof(header->plt) + sizeof(ld_bitmap_inf);
    return header;
}

static ld_sprite_header *prepare_sprite_cmp_in(unsigned char *storage,
                                               Sint32 count) {
    ld_sprite_header *header = (ld_sprite_header *)storage;
    ld_sprite_inf *spr = header->spr;

    memset(storage, 0, 4096);
    header->header.count = count;
    header->header.offset = sizeof(ld_pack_header) +
                            sizeof(ld_sprite_inf) * (Uint32)count;
    for (Sint32 i = 0; i < count; ++i) {
        spr[i].wx = (Sint16)(8 + i);
        spr[i].wy = (Sint16)(4 + i);
    }
    return header;
}

static ld_scroll_header *prepare_grid_cmp_in(unsigned char *storage,
                                             Sint32 count) {
    ld_scroll_header *header = (ld_scroll_header *)storage;

    memset(storage, 0, 4096);
    header->header.count = count;
    header->header.offset =
        sizeof(ld_pack_header) + sizeof(header->plt) + sizeof(ld_bitmap_inf);
    return header;
}

static void test_srfdraw_adds_grid_sprites_draws_and_flips(test_context *ctx) {
    reset_fixture();
    EAError(1, 2, "ignored");
    context.spr_level[2] = 12;
    context.spr_level[10] = 4;
#if defined(OPENING)
    grid_handles[0] = 1;
    grid_handles[1] = 1;
    grid_handles[2] = 1;
    infoGridBmp[0].order = 2;
    infoGridBmp[1].order = 4;
    infoGridBmp[2].order = 6;
#endif

    srfDraw();

    TEST_ASSERT_EQ_INT(ctx, 1, clear_call_count);
#if defined(OPENING)
    TEST_ASSERT_EQ_INT(ctx, 5, add_call_count);
    TEST_ASSERT_EQ_INT(ctx, expected_prio_max() - 2, add_priorities[0]);
    TEST_ASSERT_TRUE(ctx, add_bases[0] == base_handle(2000));
    TEST_ASSERT_EQ_INT(ctx, expected_prio_max() - 4, add_priorities[1]);
    TEST_ASSERT_TRUE(ctx, add_bases[1] == base_handle(2001));
    TEST_ASSERT_EQ_INT(ctx, expected_prio_max() - 6, add_priorities[2]);
    TEST_ASSERT_TRUE(ctx, add_bases[2] == base_handle(2002));
    TEST_ASSERT_EQ_INT(ctx, 4, add_priorities[3]);
    TEST_ASSERT_TRUE(ctx, add_bases[3] == base_handle(1010));
    TEST_ASSERT_EQ_INT(ctx, 12, add_priorities[4]);
    TEST_ASSERT_TRUE(ctx, add_bases[4] == base_handle(1002));
#else
    TEST_ASSERT_EQ_INT(ctx, 3, add_call_count);
    TEST_ASSERT_EQ_INT(ctx, 0, add_priorities[0]);
    TEST_ASSERT_TRUE(ctx, add_bases[0] == base_handle(2000));
    TEST_ASSERT_EQ_INT(ctx, 4, add_priorities[1]);
    TEST_ASSERT_TRUE(ctx, add_bases[1] == base_handle(1010));
    TEST_ASSERT_EQ_INT(ctx, 12, add_priorities[2]);
    TEST_ASSERT_TRUE(ctx, add_bases[2] == base_handle(1002));
#endif
    TEST_ASSERT_EQ_INT(ctx, 1, draw_call_count);
    TEST_ASSERT_EQ_INT(ctx, 1, flip_call_count);

    context.context = 0;
    srfDraw();
    TEST_ASSERT_EQ_INT(ctx, 1, clear_call_count);
    TEST_ASSERT_EQ_INT(ctx, 2, flip_call_count);
}

static void test_bitmap_4to8_conversion_variants(test_context *ctx) {
    Uint8 src[] = {0x12, 0x03, 0x40, 0x56};
    Uint8 dst[16];

    reset_fixture();
    memset(dst, 99, sizeof(dst));
    ld_bitmap_4to8_1(dst, src, 4, 1, 16, 1, 0);
    TEST_ASSERT_EQ_INT(ctx, 17, dst[0]);
    TEST_ASSERT_EQ_INT(ctx, 18, dst[1]);
    TEST_ASSERT_EQ_INT(ctx, 255, dst[2]);
    TEST_ASSERT_EQ_INT(ctx, 19, dst[3]);

    memset(dst, 99, sizeof(dst));
    ld_bitmap_4to8_1(dst, src, 4, 1, 16, 0, 0);
    TEST_ASSERT_EQ_INT(ctx, 0, dst[2]);

    memset(dst, 99, sizeof(dst));
    ld_bitmap_4to8_1(dst, src, 8, 1, 16, 1, 1);
    TEST_ASSERT_EQ_INT(ctx, 17, dst[0]);
    TEST_ASSERT_EQ_INT(ctx, 18, dst[1]);
    TEST_ASSERT_EQ_INT(ctx, 19, dst[3]);
    TEST_ASSERT_EQ_INT(ctx, 99, dst[4]);

    memset(dst, 99, sizeof(dst));
    ld_bitmap_4to8_2(dst, src, 4, 1, 16, 1, 0);
    TEST_ASSERT_EQ_INT(ctx, 17, dst[0]);
    TEST_ASSERT_EQ_INT(ctx, 18, dst[1]);
    TEST_ASSERT_EQ_INT(ctx, 255, dst[2]);
    TEST_ASSERT_EQ_INT(ctx, 255, dst[3]);

    memset(dst, 99, sizeof(dst));
    ld_bitmap_4to8_2(dst, src, 8, 1, 16, 1, 1);
    TEST_ASSERT_EQ_INT(ctx, 17, dst[0]);
    TEST_ASSERT_EQ_INT(ctx, 18, dst[1]);
    TEST_ASSERT_EQ_INT(ctx, 19, dst[3]);
    TEST_ASSERT_EQ_INT(ctx, 99, dst[4]);

    memset(dst, 99, sizeof(dst));
    ld_bitmap_4to8_2(dst, src, 4, 1, 16, 0, 0);
    TEST_ASSERT_EQ_INT(ctx, 0, dst[2]);
}

static void test_bitmap_file_loads_rows_palette_and_cleanup(test_context *ctx) {
    Uint8 dst[8];

    reset_fixture();
    prepare_bmp(2, 2, 1, 0, 2, 3);
    file_storage[58] = 2;
    memset(dst, 0, sizeof(dst));
    ld_bitmap_file("FILE.BMP", dst, 2, 2, 16, 0);
    TEST_ASSERT_TRUE(ctx, strcmp(open_file_name, "FILE.BMP") == 0);
    TEST_ASSERT_EQ_INT(ctx, 18, dst[0]);
    TEST_ASSERT_EQ_INT(ctx, 255, dst[1]);
    TEST_ASSERT_EQ_INT(ctx, 17, dst[2]);
    TEST_ASSERT_EQ_INT(ctx, 255, dst[3]);
    TEST_ASSERT_EQ_INT(ctx, 1, close_call_count);
    TEST_ASSERT_EQ_INT(ctx, 1, free_call_count);

    reset_fixture();
    prepare_bmp(2, 1, 0, 5, 0, 0);
    memset(dst, 0, sizeof(dst));
    ld_bitmap_file("ZERO.BMP", dst, 2, 1, 16, 0);
    TEST_ASSERT_EQ_INT(ctx, 255, dst[0]);
    TEST_ASSERT_EQ_INT(ctx, 255, dst[1]);

    reset_fixture();
    prepare_bmp(4, 1, 0, 5, 6, 7);
    memset(dst, 0, sizeof(dst));
    ld_bitmap_file("RAW.BMP", dst, 4, 1, 0, 0);
    TEST_ASSERT_EQ_INT(ctx, 255, dst[0]);
    TEST_ASSERT_EQ_INT(ctx, 5, dst[1]);
    TEST_ASSERT_EQ_INT(ctx, 6, dst[2]);
    TEST_ASSERT_EQ_INT(ctx, 255, dst[3]);

    reset_fixture();
    file_open_return = -1;
    ld_bitmap_file("MISS.BMP", dst, 2, 1, 0, 0);
    TEST_ASSERT_EQ_INT(ctx, 0, close_call_count);
    TEST_ASSERT_EQ_INT(ctx, 0, alloc_call_count);
}

static void test_bitmap_file2_loads_with_destination_stride(test_context *ctx) {
    Uint8 dst[16];

    reset_fixture();
    prepare_bmp(2, 2, 1, 0, 2, 3);
    file_storage[58] = 2;
    file_storage[59] = 3;
    memset(dst, 77, sizeof(dst));
    ld_bitmap_file2("FILE2.BMP", dst, 4, 0, 2, 2, 1, 0, 0, 0);
    TEST_ASSERT_EQ_INT(ctx, 77, dst[0]);
    TEST_ASSERT_EQ_INT(ctx, 2, dst[1]);
    TEST_ASSERT_EQ_INT(ctx, 3, dst[2]);
    TEST_ASSERT_EQ_INT(ctx, 77, dst[3]);
    TEST_ASSERT_EQ_INT(ctx, 1, dst[4]);
    TEST_ASSERT_EQ_INT(ctx, 255, dst[5]);

    reset_fixture();
    prepare_bmp(2, 1, 0, 5, 0, 0);
    memset(dst, 77, sizeof(dst));
    ld_bitmap_file2("PAD.BMP", dst, 4, 0, 2, 1, 0, 0, 0, 0);
    TEST_ASSERT_EQ_INT(ctx, 255, dst[0]);
    TEST_ASSERT_EQ_INT(ctx, 5, dst[1]);
    TEST_ASSERT_EQ_INT(ctx, 77, dst[2]);
    TEST_ASSERT_EQ_INT(ctx, 77, dst[3]);

    reset_fixture();
    prepare_bmp(4, 1, 1, 2, 0, 4);
    memset(dst, 77, sizeof(dst));
    ld_bitmap_file2("EVEN.BMP", dst, 4, 0, 4, 1, 0, 0, 0, 0);
    TEST_ASSERT_EQ_INT(ctx, 1, dst[0]);
    TEST_ASSERT_EQ_INT(ctx, 2, dst[1]);
    TEST_ASSERT_EQ_INT(ctx, 255, dst[2]);
    TEST_ASSERT_EQ_INT(ctx, 4, dst[3]);
}

static void test_load_sprite1_success_failure_and_sprite_bitmap_create(
    test_context *ctx) {
    hmx_bitmap *bitmaps[8];

    reset_fixture();
    TEST_ASSERT_EQ_INT(ctx, -1,
                       ld_load_sprite1(g_loader_module, "none", g_env_module,
                                       bitmaps, 4));
    TEST_ASSERT_EQ_INT(ctx, 0, bitmaps[0]);

    reset_fixture();
    load_cmp_return = prepare_sprite_cmp(3);
    TEST_ASSERT_EQ_INT(ctx, 2,
                       ld_load_sprite1(g_loader_module, "sprites", g_env_module,
                                       bitmaps, 2));
    TEST_ASSERT_EQ_INT(ctx, 2, bitmap_create_call_count);
    TEST_ASSERT_EQ_INT(ctx, 8, bitmap_create_widths[0]);
    TEST_ASSERT_EQ_INT(ctx, 16, bitmap_create_widths[1]);
    TEST_ASSERT_EQ_INT(ctx, 255, transparency_values[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, free_call_count);

    reset_fixture();
    load_cmp_return = prepare_sprite_cmp(2);
    TEST_ASSERT_EQ_INT(ctx, 0, OESprBmpCreate());
#if defined(OPENING)
    TEST_ASSERT_EQ_INT(ctx, 3, context.spr_bitmap_count);
#else
    TEST_ASSERT_EQ_INT(ctx, 2, context.spr_bitmap_count);
#endif
    TEST_ASSERT_TRUE(ctx, infoSprtBmp[0].hBmp == (Uint32 *)context.spr_bitmaps[0]);

    reset_fixture();
    TEST_ASSERT_EQ_INT(ctx, -1, OESprBmpCreate());
}

static void test_load_sprite2_opening_variant_paths(test_context *ctx) {
    hmx_bitmap *bitmaps[40];
    ld_sprite_header *header;

    reset_fixture();
    header = prepare_sprite_cmp_in(sprite_cmp_storage, 32);
    header->spr[3].wx = 12;
    header->spr[3].wy = 2;
    load_cmp_return = header;
    file_read_return = -1;

    TEST_ASSERT_EQ_INT(ctx, 33,
                       ld_load_sprite2(g_loader_module, "opening", g_env_module,
                                       bitmaps, 32));
    TEST_ASSERT_EQ_INT(ctx, 33, bitmap_create_call_count);
    TEST_ASSERT_EQ_INT(ctx, 248, bitmap_create_widths[3]);
    TEST_ASSERT_EQ_INT(ctx, 40, bitmap_create_widths[31]);
    TEST_ASSERT_TRUE(ctx, bitmaps[32] == bitmap_handle(33));
    TEST_ASSERT_EQ_INT(ctx, 33, transparency_call_count);
    TEST_ASSERT_EQ_INT(ctx, 2, open_call_count);
    /* The bitmap sizes are kept, so a copy can be made without repeating the
       loader's padding rules (see OESprBmpDuplicateUnfaded()); the two
       bitmaps that do not come from the sprite table have none. */
    TEST_ASSERT_EQ_INT(ctx, 8, sprBmpW[0]);
    TEST_ASSERT_EQ_INT(ctx, 4, sprBmpH[0]);
    TEST_ASSERT_EQ_INT(ctx, 16, sprBmpW[1]);
    TEST_ASSERT_EQ_INT(ctx, 5, sprBmpH[1]);
    TEST_ASSERT_EQ_INT(ctx, 0, sprBmpW[3]);
    TEST_ASSERT_EQ_INT(ctx, 0, sprBmpW[31]);

    reset_fixture();
    load_cmp_return = prepare_sprite_cmp_in(sprite_cmp_storage, 5);
    TEST_ASSERT_EQ_INT(ctx, 3,
                       ld_load_sprite2(g_loader_module, "small", g_env_module,
                                       bitmaps, 2));
    TEST_ASSERT_EQ_INT(ctx, 3, bitmap_create_call_count);
    TEST_ASSERT_TRUE(ctx, bitmaps[2] == bitmap_handle(3));
}

static void test_duplicate_unfaded_moves_colours_and_closes_holes(
    test_context *ctx) {
    hmx_bitmap *copy;
    Uint8 *src;
    Uint8 *dst;
    Sint32 i;
    Sint32 released;

    /* Nothing loaded: no copy. */
    reset_fixture();
    TEST_ASSERT_TRUE(ctx, OESprBmpDuplicateUnfaded(0, 64, 144) == 0);
    TEST_ASSERT_EQ_INT(ctx, 0, bitmap_create_call_count);

    /* Loaded but without a recorded size: no copy either. */
    reset_fixture();
    s_ctx->spr_bitmaps[0] = bitmap_handle(5);
    TEST_ASSERT_TRUE(ctx, OESprBmpDuplicateUnfaded(0, 64, 144) == 0);
    TEST_ASSERT_EQ_INT(ctx, 0, bitmap_create_call_count);

    /* A 5x5 sprite whose colours come from the fourth palette line.  The
       transparent pixel in the middle is a hole - all four of its neighbours
       are drawn - and has to be closed, while the border stays transparent. */
    reset_fixture();
    s_ctx->spr_bitmaps[0] = bitmap_handle(5);
    sprBmpW[0] = 5;
    sprBmpH[0] = 5;
    src = scan_storage[5];
    for (i = 0; i < 25; ++i)
        src[i] = 255;
    src[6] = 64;
    src[7] = 65;
    src[8] = 64;
    src[11] = 65;
    src[13] = 65;
    src[16] = 64;
    src[17] = 65;
    src[18] = 64;

    copy = OESprBmpDuplicateUnfaded(0, 64, 144);

    TEST_ASSERT_TRUE(ctx, copy == bitmap_handle(1));
    TEST_ASSERT_EQ_INT(ctx, 1, bitmap_create_call_count);
    TEST_ASSERT_EQ_INT(ctx, 5, bitmap_create_widths[0]);
    TEST_ASSERT_EQ_INT(ctx, 5, bitmap_create_heights[0]);
    dst = scan_storage[1];
    /* Colours moved from line 3 (64-79) to the spare entries (144-159)... */
    TEST_ASSERT_EQ_INT(ctx, 144, dst[6]);
    TEST_ASSERT_EQ_INT(ctx, 145, dst[7]);
    TEST_ASSERT_EQ_INT(ctx, 145, dst[11]);
    /* ...the hole closed with the line's black... */
    TEST_ASSERT_EQ_INT(ctx, 144, dst[12]);
    /* ...and the transparent pixels that open onto the border left alone. */
    TEST_ASSERT_EQ_INT(ctx, 255, dst[0]);
    TEST_ASSERT_EQ_INT(ctx, 255, dst[24]);
    TEST_ASSERT_EQ_INT(ctx, 2, alloc_call_count);
    TEST_ASSERT_EQ_INT(ctx, 2, free_call_count);
    TEST_ASSERT_TRUE(ctx, transparency_bitmaps[0] == copy);
    TEST_ASSERT_EQ_INT(ctx, 255, transparency_values[0]);

    /* The copy is released with the other sprite bitmaps. */
    released = 0;
    OESprBmpDelete();
    for (i = 0; i < bitmap_release_call_count; ++i) {
        if (bitmap_release_values[i] == copy)
            released = 1;
    }
    TEST_ASSERT_EQ_INT(ctx, 1, released);
}

static void test_load_grid_success_failure_and_grid_bitmap_create_delete(
    test_context *ctx) {
    hmx_bitmap *bitmaps[8];

    reset_fixture();
    TEST_ASSERT_EQ_INT(ctx, -1,
                       ld_load_grid2(g_loader_module, "none", g_env_module,
                                     bitmaps, 8));

    reset_fixture();
    load_cmp_return = prepare_grid_cmp(5);
    TEST_ASSERT_EQ_INT(ctx, 2,
                       ld_load_grid2(g_loader_module, "small-grid",
                                     g_env_module, bitmaps, 2));

    reset_fixture();
    load_cmp_return = prepare_grid_cmp(5);
    TEST_ASSERT_EQ_INT(ctx, 5,
                       ld_load_grid2(g_loader_module, "grid", g_env_module,
                                     bitmaps, 8));
    TEST_ASSERT_EQ_INT(ctx, 3, bitmap_create_call_count);
    TEST_ASSERT_TRUE(ctx, infoGridBmp[0].lphBmp[0] == (Uint32 *)bitmap_handle(1));
    TEST_ASSERT_TRUE(ctx, infoGridBmp[0].lphBmp[1] == (Uint32 *)bitmap_handle(2));
    TEST_ASSERT_TRUE(ctx, infoGridBmp[1].lphBmp[0] == (Uint32 *)bitmap_handle(3));
    TEST_ASSERT_EQ_INT(ctx, 3, bitmap_4to8_call_count);
    TEST_ASSERT_EQ_INT(ctx, 0, transparency_values[0]);

    reset_fixture();
    load_cmp_return = prepare_grid_cmp(4);
    TEST_ASSERT_EQ_INT(ctx, 0, OEGridBmpCreate());
    TEST_ASSERT_EQ_INT(ctx, 4, context.grd_bitmap_count);
    TEST_ASSERT_TRUE(ctx, strcmp(load_cmp_file_names[0], "TILE.CM_") == 0);
    OEGridBmpDelete();
    TEST_ASSERT_EQ_INT(ctx, 3, bitmap_release_call_count);
    TEST_ASSERT_EQ_INT(ctx, 0, infoGridBmp[0].lphBmp[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, context.grd_bitmap_count);

    reset_fixture();
    TEST_ASSERT_EQ_INT(ctx, -1, OEGridBmpCreate());
}

static void test_sprite_create_delete_and_handle_reuse(test_context *ctx) {
    reset_fixture();
    context.spr_bitmaps[0] = bitmap_handle(10);
    context.spr_bitmaps[2] = bitmap_handle(12);
    infoSprtBmp[0].timing = 9;
    infoSprtBmp[2].timing = 9;

    OESprCreate(9);
    TEST_ASSERT_EQ_INT(ctx, 2, position_call_count);
    TEST_ASSERT_EQ_INT(ctx, 1, sprite_handles[0]);
    TEST_ASSERT_EQ_INT(ctx, 2, sprite_handles[2]);
    TEST_ASSERT_TRUE(ctx, set_bitmap_values[0] == bitmap_handle(10));
    TEST_ASSERT_EQ_INT(ctx, expected_prio_max() - infoSprtBmp[0].order,
                       context.spr_level[1]);

    OESprCreate(9);
    TEST_ASSERT_EQ_INT(ctx, 2, position_call_count);

    infoSprtBmp[1].timing = 9;
    OESprDelete(9);
    TEST_ASSERT_EQ_INT(ctx, 4, set_bitmap_call_count);
    TEST_ASSERT_EQ_INT(ctx, 4, flags_call_count);
    TEST_ASSERT_EQ_INT(ctx, 0, sprite_handles[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, context.spr_level[1]);

    OESprCreateIndx(3);
    TEST_ASSERT_EQ_INT(ctx, 1, sprite_handles[3]);
    OESprCreateIndx(3);
    TEST_ASSERT_EQ_INT(ctx, 3, position_call_count);
    OESprDeleteIndx(3);
#if defined(OPENING)
    TEST_ASSERT_EQ_INT(ctx, 1, sprite_handles[3]);
#else
    TEST_ASSERT_EQ_INT(ctx, 0, sprite_handles[3]);
#endif
    OESprDeleteIndx(3);
    TEST_ASSERT_EQ_INT(ctx, 3, position_call_count);

    sprite_handles[3] = 4;
    context.spr_level[4] = 20;
    OEAllSprDelete();
    TEST_ASSERT_EQ_INT(ctx, 0, sprite_handles[3]);
    TEST_ASSERT_EQ_INT(ctx, 0, context.spr_level[4]);
}

static void test_bitmap_delete_grid_delete_create_and_delete_ea(
    test_context *ctx) {
    reset_fixture();
    context.spr_bitmap_count = 77;
    context.spr_bitmaps[0] = bitmap_handle(1);
    context.spr_bitmaps[999] = bitmap_handle(2);
    context.spr_level[999] = 9;
    OESprBmpDelete();
    TEST_ASSERT_EQ_INT(ctx, 1000, bitmap_release_call_count);
    TEST_ASSERT_EQ_INT(ctx, 0, context.spr_bitmap_count);
    TEST_ASSERT_EQ_INT(ctx, 0, context.spr_bitmaps[999]);
    TEST_ASSERT_EQ_INT(ctx, 0, context.spr_level[999]);

    reset_fixture();
    OEGridDeleteAll();
    TEST_ASSERT_EQ_INT(ctx, 3, grid_release_call_count);
    TEST_ASSERT_EQ_INT(ctx, 0, context.grids[0]);

    reset_fixture();
    TEST_ASSERT_EQ_INT(ctx, 0, OECreateEA());

    reset_fixture();
    sprite_cmp_return = prepare_sprite_cmp_in(sprite_cmp_storage, 2);
    TEST_ASSERT_EQ_INT(ctx, 0, OECreateEA());

    reset_fixture();
    sprite_cmp_return = prepare_sprite_cmp_in(sprite_cmp_storage, 2);
    grid_cmp_return = prepare_grid_cmp_in(grid_cmp_storage, 3);
    TEST_ASSERT_EQ_INT(ctx, 1, OECreateEA());
    TEST_ASSERT_EQ_INT(ctx, 3, grid_release_call_count);
#if defined(OPENING)
    TEST_ASSERT_EQ_INT(ctx, 3, context.spr_bitmap_count);
#else
    TEST_ASSERT_EQ_INT(ctx, 2, context.spr_bitmap_count);
#endif

    reset_fixture();
    sprite_handles[0] = 1;
    tile_handles[0] = (Uint32)bitmap_handle(3);
    context.spr_bitmaps[0] = bitmap_handle(1);
    OEDeleteEA();
    TEST_ASSERT_EQ_INT(ctx, 1, all_grid_delete_call_count);
    TEST_ASSERT_EQ_INT(ctx, 3, grid_create_call_count);
    TEST_ASSERT_TRUE(ctx, context.grids[0] == grid_handle(4));
}

TEST_MAIN_BEGIN;
test_srfdraw_adds_grid_sprites_draws_and_flips(&ctx);
test_bitmap_4to8_conversion_variants(&ctx);
test_bitmap_file_loads_rows_palette_and_cleanup(&ctx);
test_bitmap_file2_loads_with_destination_stride(&ctx);
test_load_sprite1_success_failure_and_sprite_bitmap_create(&ctx);
test_load_sprite2_opening_variant_paths(&ctx);
test_duplicate_unfaded_moves_colours_and_closes_holes(&ctx);
test_load_grid_success_failure_and_grid_bitmap_create_delete(&ctx);
test_sprite_create_delete_and_handle_reuse(&ctx);
test_bitmap_delete_grid_delete_create_and_delete_ea(&ctx);
TEST_MAIN_END
