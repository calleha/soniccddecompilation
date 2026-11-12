/* <stdlib.h> declares POSIX random(); the decompiled special stage declares
   its own Sint32 random(void) (that clash is also why the plugin is linked with
   -Wl,-Bsymbolic-functions: at run time libc's random() would win).  Parse
   stdlib first, then rename the game symbol inside this harness. */
#include <stdlib.h>

#define random game_random

#include <string.h>

#include "support/test_runner.h"
#include "src/types.h"

Uint8 ufoleft;
Uint16 ringno;
Uint8 spgmmode;
Uint8 tcnt20;
Uint8 tdecflg;
Uint8 time_stop;
Uint8 game_start;
Uint8 pldeadflg;
int_union spe_time;
uint_union se_no;
Uint32 ranum;
Uint8 stagenm;

static int tufo_initial_count;

void tufo_initial(void);

#include "src/special/etc_s.c"

void tufo_initial(void) { ++tufo_initial_count; }

static void reset_state(void) {
    ufoleft = 0;
    ringno = 0;
    spgmmode = 0;
    tcnt20 = 0;
    tdecflg = 0;
    time_stop = 0;
    game_start = 0;
    pldeadflg = 0;
    memset(&spe_time, 0, sizeof(spe_time));
    memset(&se_no, 0, sizeof(se_no));
    ranum = 0;
    stagenm = 0;
    tufo_initial_count = 0;
}

static void test_ufo_and_ring_counters_wrap_and_clamp(test_context *ctx) {
    reset_state();

    ufoleft = 2;
    ufo_dec();
    TEST_ASSERT_EQ_INT(ctx, 1, ufoleft);

    ufoleft = 0;
    ufo_dec();
    TEST_ASSERT_EQ_INT(ctx, 255, ufoleft);

    ringno = 990;
    ring_add(9);
    TEST_ASSERT_EQ_INT(ctx, 999, ringno);

    ringno = 990;
    ring_add(10);
    TEST_ASSERT_EQ_INT(ctx, 999, ringno);
}

static void test_time_dec_handles_normal_ticks_and_forced_decrements(
    test_context *ctx) {
    reset_state();
    tcnt20 = 2;
    spe_time.l = 50;

    time_dec();

    TEST_ASSERT_EQ_INT(ctx, 1, tcnt20);
    TEST_ASSERT_EQ_INT(ctx, 50, spe_time.l);
    TEST_ASSERT_EQ_INT(ctx, 0, tufo_initial_count);

    reset_state();
    tcnt20 = 1;
    spe_time.l = 50;

    time_dec();

    TEST_ASSERT_EQ_INT(ctx, 20, tcnt20);
    TEST_ASSERT_EQ_INT(ctx, 49, spe_time.l);
    TEST_ASSERT_EQ_INT(ctx, 1, tufo_initial_count);
    TEST_ASSERT_EQ_INT(ctx, 0, se_no.b.b1);

    reset_state();
    tcnt20 = 2;
    tdecflg = 2;
    spe_time.l = 50;

    time_dec();

    TEST_ASSERT_EQ_INT(ctx, 1, tcnt20);
    TEST_ASSERT_EQ_INT(ctx, 1, tdecflg);
    TEST_ASSERT_EQ_INT(ctx, 49, spe_time.l);
    TEST_ASSERT_EQ_INT(ctx, 1, tufo_initial_count);
}

static void test_time_dec_uses_time_attack_mode(test_context *ctx) {
    reset_state();
    spgmmode = 2;
    tcnt20 = 7;
    spe_time.b.b4 = 58;
    spe_time.b.b3 = 59;
    spe_time.b.b2 = 9;

    time_dec();

    TEST_ASSERT_EQ_INT(ctx, 7, tcnt20);
    TEST_ASSERT_EQ_INT(ctx, 604987, spe_time.l);
    TEST_ASSERT_EQ_INT(ctx, 1, pldeadflg);
}

static void test_dec_respects_stop_flags(test_context *ctx) {
    reset_state();
    time_stop = 1;
    spe_time.l = 10;

    dec();

    TEST_ASSERT_EQ_INT(ctx, 10, spe_time.l);
    TEST_ASSERT_EQ_INT(ctx, 0, tufo_initial_count);

    reset_state();
    game_start = 1;
    spe_time.l = 10;

    dec();

    TEST_ASSERT_EQ_INT(ctx, 10, spe_time.l);
    TEST_ASSERT_EQ_INT(ctx, 0, tufo_initial_count);
}

static void test_dec_underflow_and_low_time_sound(test_context *ctx) {
    reset_state();
    spe_time.l = 0;
    tdecflg = 3;

    dec();

    TEST_ASSERT_EQ_INT(ctx, 0, spe_time.l);
    TEST_ASSERT_EQ_INT(ctx, 0, tdecflg);
    TEST_ASSERT_EQ_INT(ctx, 1, pldeadflg);
    TEST_ASSERT_EQ_INT(ctx, 1, tufo_initial_count);
    TEST_ASSERT_EQ_INT(ctx, 223, se_no.b.b1);

    reset_state();
    spe_time.l = 16;

    dec();

    TEST_ASSERT_EQ_INT(ctx, 15, spe_time.l);
    TEST_ASSERT_EQ_INT(ctx, 1, tufo_initial_count);
    TEST_ASSERT_EQ_INT(ctx, 0, se_no.b.b1);

    reset_state();
    spe_time.l = 15;

    dec();

    TEST_ASSERT_EQ_INT(ctx, 14, spe_time.l);
    TEST_ASSERT_EQ_INT(ctx, 223, se_no.b.b1);
}

static void test_time_time_attack_rolls_centiseconds_seconds_and_minutes(
    test_context *ctx) {
    reset_state();
    spe_time.b.b4 = 56;

    time_time_attack();

    TEST_ASSERT_EQ_INT(ctx, 59, spe_time.b.b4);
    TEST_ASSERT_EQ_INT(ctx, 0, spe_time.b.b3);

    reset_state();
    spe_time.b.b4 = 58;

    time_time_attack();

    TEST_ASSERT_EQ_INT(ctx, 1, spe_time.b.b4);
    TEST_ASSERT_EQ_INT(ctx, 1, spe_time.b.b3);

    reset_state();
    spe_time.b.b4 = 58;
    spe_time.b.b3 = 59;

    time_time_attack();

    TEST_ASSERT_EQ_INT(ctx, 1, spe_time.b.b4);
    TEST_ASSERT_EQ_INT(ctx, 0, spe_time.b.b3);
    TEST_ASSERT_EQ_INT(ctx, 1, spe_time.b.b2);

    reset_state();
    spe_time.b.b4 = 58;
    spe_time.b.b3 = 59;
    spe_time.b.b2 = 9;

    time_time_attack();

    TEST_ASSERT_EQ_INT(ctx, 604987, spe_time.l);
    TEST_ASSERT_EQ_INT(ctx, 1, pldeadflg);
}

static void test_time_time_attack_respects_stop_flags(test_context *ctx) {
    reset_state();
    time_stop = 1;
    spe_time.b.b4 = 58;

    time_time_attack();

    TEST_ASSERT_EQ_INT(ctx, 58, spe_time.b.b4);

    reset_state();
    game_start = 1;
    spe_time.b.b4 = 58;

    time_time_attack();

    TEST_ASSERT_EQ_INT(ctx, 58, spe_time.b.b4);
}

static void test_angleget_reports_octant_and_direction_flags(test_context *ctx) {
    Uint8 angle = 255;
    Uint8 dir = 255;

    angleget(100, 50, 0, 0, &angle, &dir);
    TEST_ASSERT_EQ_INT(ctx, 32, angle);
    TEST_ASSERT_EQ_INT(ctx, 0, dir);

    angleget(90, 0, 100, 0, &angle, &dir);
    TEST_ASSERT_EQ_INT(ctx, 0, angle);
    TEST_ASSERT_EQ_INT(ctx, 1, dir);

    angleget(0, 0, -10, 0, &angle, &dir);
    TEST_ASSERT_EQ_INT(ctx, 0, angle);
    TEST_ASSERT_EQ_INT(ctx, 5, dir);

    angleget(-10, 0, 0, 0, &angle, &dir);
    TEST_ASSERT_EQ_INT(ctx, 0, angle);
    TEST_ASSERT_EQ_INT(ctx, 4, dir);

    angleget(0, -10, 0, 0, &angle, &dir);
    TEST_ASSERT_EQ_INT(ctx, 0, angle);
    TEST_ASSERT_EQ_INT(ctx, 6, dir);

    angleget(50, 100, 0, 0, &angle, &dir);
    TEST_ASSERT_EQ_INT(ctx, 32, angle);
    TEST_ASSERT_EQ_INT(ctx, 4, dir);

    angleget(100, 100, 0, 0, &angle, &dir);
    TEST_ASSERT_EQ_INT(ctx, 63, angle);
    TEST_ASSERT_EQ_INT(ctx, 4, dir);

    angleget(0, 0, 0, 0, &angle, &dir);
    TEST_ASSERT_EQ_INT(ctx, 0, angle);
    TEST_ASSERT_EQ_INT(ctx, 4, dir);
}

static void test_speedget_converts_angle_direction_and_base_speed(
    test_context *ctx) {
    Sint32 xspeed = 123;
    Sint32 yspeed = 456;

    speedget(0, 0, 256, &xspeed, &yspeed);
    TEST_ASSERT_EQ_INT(ctx, 65536, xspeed);
    TEST_ASSERT_EQ_INT(ctx, 0, yspeed);

    speedget(0, 1, 256, &xspeed, &yspeed);
    TEST_ASSERT_EQ_INT(ctx, -65536, xspeed);
    TEST_ASSERT_EQ_INT(ctx, 0, yspeed);

    speedget(0, 4, 256, &xspeed, &yspeed);
    TEST_ASSERT_EQ_INT(ctx, 0, xspeed);
    TEST_ASSERT_EQ_INT(ctx, 65536, yspeed);

    speedget(0, 6, 256, &xspeed, &yspeed);
    TEST_ASSERT_EQ_INT(ctx, 0, xspeed);
    TEST_ASSERT_EQ_INT(ctx, -65536, yspeed);

    speedget(1, 0, 0, &xspeed, &yspeed);
    TEST_ASSERT_EQ_INT(ctx, 0, xspeed);
    TEST_ASSERT_EQ_INT(ctx, 0, yspeed);
}

static void test_dstnsget_uses_selected_axis_and_absolute_distance(
    test_context *ctx) {
    TEST_ASSERT_EQ_INT(ctx, 9, dstnsget(0, 0, 90, 0, 100, 0));
    TEST_ASSERT_EQ_INT(ctx, 9, dstnsget(0, 0, 110, 0, 100, 0));
    TEST_ASSERT_EQ_INT(ctx, 14, dstnsget(64, 4, 0, 90, 0, 100));
}

static void test_random_uses_default_seed_and_updates_state(test_context *ctx) {
    Sint32 value;

    reset_state();

    value = random();

    TEST_ASSERT_EQ_INT(ctx, 711819239, value);
    TEST_ASSERT_EQ_INT(ctx, 2145891434, (Sint32)ranum);

    value = random();

    TEST_ASSERT_EQ_INT(ctx, 2145870101, value);
    TEST_ASSERT_EQ_INT(ctx, 1628824826, (Sint32)ranum);

    ranum = 0x80000000;
    value = random();

    TEST_ASSERT_EQ_INT(ctx, -2147450880, value);
    TEST_ASSERT_EQ_INT(ctx, -2147483648, (Sint32)ranum);
}

TEST_MAIN_BEGIN;
test_ufo_and_ring_counters_wrap_and_clamp(&ctx);
test_time_dec_handles_normal_ticks_and_forced_decrements(&ctx);
test_time_dec_uses_time_attack_mode(&ctx);
test_dec_respects_stop_flags(&ctx);
test_dec_underflow_and_low_time_sound(&ctx);
test_time_time_attack_rolls_centiseconds_seconds_and_minutes(&ctx);
test_time_time_attack_respects_stop_flags(&ctx);
test_angleget_reports_octant_and_direction_flags(&ctx);
test_speedget_converts_angle_direction_and_base_speed(&ctx);
test_dstnsget_uses_selected_axis_and_absolute_distance(&ctx);
test_random_uses_default_seed_and_updates_state(&ctx);
TEST_MAIN_END
