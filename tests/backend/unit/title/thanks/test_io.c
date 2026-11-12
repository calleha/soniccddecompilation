/* <stdlib.h> declares POSIX random(); the decompiled thanks headers declare
   the game's own Sint32 random(void).  Parse stdlib first, then rename the game
   symbol inside this harness so the two can coexist. */
#include <stdlib.h>

#define random game_random

#include <string.h>

#include "support/test_runner.h"
#include "src/types.h"

static PALETTEENTRY palette[80];
PALETTEENTRY *lpcolorwk0;

#include "src/title/thanks/io.c"

static void reset_fixture(void) {
    memset(palette, 0x7b, sizeof(palette));
    lpcolorwk0 = palette;
}

static void assert_entry(test_context *ctx, PALETTEENTRY actual,
                         PALETTEENTRY expected) {
    TEST_ASSERT_EQ_INT(ctx, expected.peRed, actual.peRed);
    TEST_ASSERT_EQ_INT(ctx, expected.peGreen, actual.peGreen);
    TEST_ASSERT_EQ_INT(ctx, expected.peBlue, actual.peBlue);
    TEST_ASSERT_EQ_INT(ctx, expected.peFlags, actual.peFlags);
}

static PALETTEENTRY entry(Uint8 red, Uint8 green, Uint8 blue, Uint8 flags) {
    PALETTEENTRY pe;

    pe.peRed = red;
    pe.peGreen = green;
    pe.peBlue = blue;
    pe.peFlags = flags;
    return pe;
}

static void test_vdpinit_copies_all_64_palette_entries(test_context *ctx) {
    reset_fixture();

    vdpinit();

    for (Sint32 i = 0; i < 64; ++i) {
        assert_entry(ctx, palette[i], colortbl[i]);
    }
    assert_entry(ctx, palette[64], entry(0x7b, 0x7b, 0x7b, 0x7b));
    assert_entry(ctx, palette[79], entry(0x7b, 0x7b, 0x7b, 0x7b));
}

TEST_MAIN_BEGIN;
test_vdpinit_copies_all_64_palette_entries(&ctx);
TEST_MAIN_END
