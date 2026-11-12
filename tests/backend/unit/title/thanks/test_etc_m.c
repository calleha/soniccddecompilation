/* <stdlib.h> declares POSIX random(); the decompiled thanks headers declare
   the game's own Sint32 random(void).  Parse stdlib first, then rename the game
   symbol inside this harness so the two can coexist. */
#include <stdlib.h>

#define random game_random

#include "support/test_runner.h"
#include "src/types.h"

Sint32 ranum;

#include "src/title/thanks/etc_m.c"

static Sint32 next_random_reference(Sint32 seed) {
    int_union d0;
    int_union d1;
    Sint16 tmp;

    d1.l = seed;
    if (d1.l == 0) {
        d1.l = 711800410;
    }
    d0.l = d1.l;
    d1.l <<= 2;
    d1.l += d0.l;
    d1.l <<= 3;
    d1.l += d0.l;
    d0.l = d1.l;
    tmp = d1.w.h;
    d1.w.h = d1.w.l;
    d0.w.l = tmp;
    d0.w.l += d1.w.l;
    d1.w.l = d0.w.l;
    tmp = d1.w.h;
    d1.w.h = d1.w.l;
    d0.w.l = tmp;
    return d0.l;
}

static Sint32 next_seed_reference(Sint32 seed) {
    int_union d0;
    int_union d1;
    Sint16 tmp;

    d1.l = seed;
    if (d1.l == 0) {
        d1.l = 711800410;
    }
    d0.l = d1.l;
    d1.l <<= 2;
    d1.l += d0.l;
    d1.l <<= 3;
    d1.l += d0.l;
    d0.l = d1.l;
    tmp = d1.w.h;
    d1.w.h = d1.w.l;
    d0.w.l = tmp;
    d0.w.l += d1.w.l;
    d1.w.l = d0.w.l;
    tmp = d1.w.h;
    d1.w.h = d1.w.l;
    d0.w.l = tmp;
    return d1.l;
}

static void test_random_uses_default_seed_and_updates_ranum(test_context *ctx) {
    Sint32 expected_value = next_random_reference(0);
    Sint32 expected_seed = next_seed_reference(0);

    ranum = 0;

    TEST_ASSERT_EQ_INT(ctx, expected_value, random());
    TEST_ASSERT_EQ_INT(ctx, expected_seed, ranum);
}

static void test_random_uses_existing_seed(test_context *ctx) {
    Sint32 seed = 0x12345678;
    Sint32 expected_value = next_random_reference(seed);
    Sint32 expected_seed = next_seed_reference(seed);

    ranum = seed;

    TEST_ASSERT_EQ_INT(ctx, expected_value, random());
    TEST_ASSERT_EQ_INT(ctx, expected_seed, ranum);
}

static void test_sign_quadrants_and_wrapping(test_context *ctx) {
    TEST_ASSERT_EQ_INT(ctx, 0, sign(0));
    TEST_ASSERT_EQ_INT(ctx, 3, sign(1));
    TEST_ASSERT_EQ_INT(ctx, 256, sign(127));
    TEST_ASSERT_EQ_INT(ctx, 256, sign(128));
    TEST_ASSERT_EQ_INT(ctx, 0, sign(255));
    TEST_ASSERT_EQ_INT(ctx, 0, sign(256));
    TEST_ASSERT_EQ_INT(ctx, -3, sign(257));
    TEST_ASSERT_EQ_INT(ctx, -256, sign(383));
    TEST_ASSERT_EQ_INT(ctx, -256, sign(384));
    TEST_ASSERT_EQ_INT(ctx, 0, sign(511));
    TEST_ASSERT_EQ_INT(ctx, 0, sign(512));
    TEST_ASSERT_EQ_INT(ctx, 0, sign(-1));
}

static void test_cosign_offsets_by_quarter_turn(test_context *ctx) {
    TEST_ASSERT_EQ_INT(ctx, sign(128), cosign(0));
    TEST_ASSERT_EQ_INT(ctx, sign(256), cosign(128));
    TEST_ASSERT_EQ_INT(ctx, sign(384), cosign(256));
    TEST_ASSERT_EQ_INT(ctx, sign(0), cosign(384));
}

TEST_MAIN_BEGIN;
test_random_uses_default_seed_and_updates_ranum(&ctx);
test_random_uses_existing_seed(&ctx);
test_sign_quadrants_and_wrapping(&ctx);
test_cosign_offsets_by_quarter_turn(&ctx);
TEST_MAIN_END
