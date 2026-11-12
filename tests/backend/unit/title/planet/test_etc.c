/* <stdlib.h> declares POSIX random(); the decompiled title/planet headers
   declare the game's own Sint32 random(void).  Parse stdlib first, then rename
   the game symbol inside this harness so the two can coexist. */
#include <stdlib.h>

#define random game_random

#include <string.h>

#include "support/test_runner.h"
#include "types.h"

Sint32 ranum;
ushort_union swdata1;

#include "src/title/planet/etc.c"

static void reset_state(void) {
    ranum = 0;
    swdata1.w = 0;
}

static void test_sync_and_swclear_match_existing_noop_behavior(
    test_context *ctx) {
    reset_state();
    swdata1.w = 0x1234;

    sync();
    TEST_ASSERT_EQ_INT(ctx, 0x1234, swdata1.w);

    swclear();
    TEST_ASSERT_EQ_INT(ctx, 0xFF00, swdata1.w);
}

static void test_random_uses_default_seed_and_updates_state(test_context *ctx) {
    Sint32 result;

    reset_state();

    result = random();

    TEST_ASSERT_EQ_INT(ctx, 711819239, result);
    TEST_ASSERT_EQ_INT(ctx, 2145891434, ranum);
}

static void test_random_positive_and_negative_seed_paths(test_context *ctx) {
    reset_state();
    ranum = 1;
    TEST_ASSERT_EQ_INT(ctx, 41, random());
    TEST_ASSERT_EQ_INT(ctx, 2687017, ranum);

    ranum = -1;
    TEST_ASSERT_EQ_INT(ctx, -42, random());
    TEST_ASSERT_EQ_INT(ctx, -2687017, ranum);
}

static void test_planet_sine_quadrants_and_cosine_wrapper(test_context *ctx) {
    reset_state();

    TEST_ASSERT_EQ_INT(ctx, 0, sin_planet(0));
    TEST_ASSERT_EQ_INT(ctx, 181, sin_planet(64));
    TEST_ASSERT_EQ_INT(ctx, 256, sin_planet(128));
    TEST_ASSERT_EQ_INT(ctx, 179, sin_planet(192));
    TEST_ASSERT_EQ_INT(ctx, 0, sin_planet(256));
    TEST_ASSERT_EQ_INT(ctx, -181, sin_planet(320));
    TEST_ASSERT_EQ_INT(ctx, -256, sin_planet(384));
    TEST_ASSERT_EQ_INT(ctx, -179, sin_planet(448));
    TEST_ASSERT_EQ_INT(ctx, 0, sin_planet(512));

    TEST_ASSERT_EQ_INT(ctx, 256, cos_planet(0));
    TEST_ASSERT_EQ_INT(ctx, 0, cos_planet(128));
    TEST_ASSERT_EQ_INT(ctx, -256, cos_planet(256));
}

static void test_unlze_literal_run_then_terminal_code(test_context *ctx) {
    Uint8 compressed[] = {0x0B, 0x00, 'A', 'B', 0x00, 0x00, 0x00};
    Uint8 output[8];

    reset_state();
    memset(output, 0xCC, sizeof(output));

    unlze(compressed, output);

    TEST_ASSERT_EQ_INT(ctx, 'A', output[0]);
    TEST_ASSERT_EQ_INT(ctx, 'B', output[1]);
    TEST_ASSERT_EQ_INT(ctx, 0xCC, output[2]);
}

static void test_unlze_continue_code_skips_without_output(test_context *ctx) {
    Uint8 compressed[] = {0x0A, 0x00, 0x00, 0x00, 0x01, 'Z',
                          0x00, 0x00, 0x00};
    Uint8 output[8];

    reset_state();
    memset(output, 0xCC, sizeof(output));

    unlze(compressed, output);

    TEST_ASSERT_EQ_INT(ctx, 'Z', output[0]);
    TEST_ASSERT_EQ_INT(ctx, 0xCC, output[1]);
}

static void test_unlze_short_back_reference_copies_prior_bytes(
    test_context *ctx) {
    Uint8 compressed[] = {0x23, 0x00, 'A', 'B', 0xFE, 0x00, 0x00, 0x00};
    Uint8 output[8];

    reset_state();
    memset(output, 0xCC, sizeof(output));

    unlze(compressed, output);

    TEST_ASSERT_EQ_INT(ctx, 'A', output[0]);
    TEST_ASSERT_EQ_INT(ctx, 'B', output[1]);
    TEST_ASSERT_EQ_INT(ctx, 'A', output[2]);
    TEST_ASSERT_EQ_INT(ctx, 'B', output[3]);
    TEST_ASSERT_EQ_INT(ctx, 0xCC, output[4]);
}

static void test_unlze_reloads_control_word_after_literal_run(
    test_context *ctx) {
    Uint8 compressed[] = {0xFF, 0xFF, 'a',  'b',  'c',  'd',  'e',
                          'f',  'g',  'h',  'i',  'j',  'k',  'l',
                          'm',  'n',  'o',  0x02, 0x00, 'p',  0x00,
                          0x00, 0x00};
    Uint8 output[24];

    reset_state();
    memset(output, 0xCC, sizeof(output));

    unlze(compressed, output);

    TEST_ASSERT_EQ_INT(ctx, 'a', output[0]);
    TEST_ASSERT_EQ_INT(ctx, 'p', output[15]);
    TEST_ASSERT_EQ_INT(ctx, 0xCC, output[16]);
}

static void test_unlze_reloads_control_word_before_second_flag(
    test_context *ctx) {
    Uint8 compressed[] = {0xFF, 0xBF, 'a',  'b',  'c',  'd',  'e',
                          'f',  'g',  'h',  'i',  'j',  'k',  'l',
                          'm',  'n',  0x00, 0x00, 0x00, 0x00, 0x00};
    Uint8 output[24];

    reset_state();
    memset(output, 0xCC, sizeof(output));

    unlze(compressed, output);

    TEST_ASSERT_EQ_INT(ctx, 'a', output[0]);
    TEST_ASSERT_EQ_INT(ctx, 'n', output[13]);
    TEST_ASSERT_EQ_INT(ctx, 0xCC, output[14]);
}

static void test_unlze_reloads_control_word_inside_short_copy(
    test_context *ctx) {
    Uint8 compressed[] = {0xFF, 0x1F, 'a',  'b',  'c',  'd',  'e',
                          'f',  'g',  'h',  'i',  'j',  'k',  'l',
                          'm',  0x02, 0x00, 0xFF, 0x00, 0x00, 0x00};
    Uint8 output[24];

    reset_state();
    memset(output, 0xCC, sizeof(output));

    unlze(compressed, output);

    TEST_ASSERT_EQ_INT(ctx, 'a', output[0]);
    TEST_ASSERT_EQ_INT(ctx, 'm', output[12]);
    TEST_ASSERT_EQ_INT(ctx, 'm', output[13]);
    TEST_ASSERT_EQ_INT(ctx, 'm', output[14]);
    TEST_ASSERT_EQ_INT(ctx, 0xCC, output[15]);
}

static void test_unlze_long_copy_with_extended_count(test_context *ctx) {
    Uint8 compressed[] = {0x0A, 0x00, 0x00, 0x00, 0x02, 'Q',
                          0x00, 0x00, 0x00};
    Uint8 output[9000];
    Uint8 *dst = output + 8192;

    reset_state();
    memset(output, 0xCC, sizeof(output));
    output[0] = 'X';
    output[1] = 'Y';
    output[2] = 'Z';

    unlze(compressed, dst);

    TEST_ASSERT_EQ_INT(ctx, 'X', dst[0]);
    TEST_ASSERT_EQ_INT(ctx, 'Y', dst[1]);
    TEST_ASSERT_EQ_INT(ctx, 'Z', dst[2]);
    TEST_ASSERT_EQ_INT(ctx, 'Q', dst[3]);
}

static void test_unlze_long_copy_with_inline_count(test_context *ctx) {
    Uint8 compressed[] = {0x0A, 0x00, 0x00, 0x01, 'Q', 0x00, 0x00, 0x00};
    Uint8 output[9000];
    Uint8 *dst = output + 8192;

    reset_state();
    memset(output, 0xCC, sizeof(output));
    output[0] = 'X';
    output[1] = 'Y';
    output[2] = 'Z';

    unlze(compressed, dst);

    TEST_ASSERT_EQ_INT(ctx, 'X', dst[0]);
    TEST_ASSERT_EQ_INT(ctx, 'Y', dst[1]);
    TEST_ASSERT_EQ_INT(ctx, 'Z', dst[2]);
    TEST_ASSERT_EQ_INT(ctx, 'Q', dst[3]);
}

TEST_MAIN_BEGIN;
test_sync_and_swclear_match_existing_noop_behavior(&ctx);
test_random_uses_default_seed_and_updates_state(&ctx);
test_random_positive_and_negative_seed_paths(&ctx);
test_planet_sine_quadrants_and_cosine_wrapper(&ctx);
test_unlze_literal_run_then_terminal_code(&ctx);
test_unlze_continue_code_skips_without_output(&ctx);
test_unlze_short_back_reference_copies_prior_bytes(&ctx);
test_unlze_reloads_control_word_after_literal_run(&ctx);
test_unlze_reloads_control_word_before_second_flag(&ctx);
test_unlze_reloads_control_word_inside_short_copy(&ctx);
test_unlze_long_copy_with_extended_count(&ctx);
test_unlze_long_copy_with_inline_count(&ctx);
TEST_MAIN_END
