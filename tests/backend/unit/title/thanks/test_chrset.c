/* <stdlib.h> declares POSIX random(); the decompiled thanks headers declare
   the game's own Sint32 random(void).  Parse stdlib first, then rename the game
   symbol inside this harness so the two can coexist. */
#include <stdlib.h>

#define random game_random

#include <string.h>

#include "support/test_runner.h"
#include "src/types.h"

static Sint32 random_return;
static Sint32 random_call_count;
static Sint16 get_actwk_return;
static Sint32 get_actwk_call_count;

#include "src/title/thanks/chrset.c"

Uint8 animal_flg;
Sint16 actset_tm[8];
sprite_status_thanks actwk[66];
static sprite_status_thanks new_actor;

Sint32 random(void) {
    ++random_call_count;
    return random_return;
}

Sint16 get_actwk(sprite_status_thanks **pNewActwk) {
    ++get_actwk_call_count;
    *pNewActwk = &new_actor;
    return get_actwk_return;
}

static void reset_fixture(void) {
    memset(actset_tm, 0, sizeof(actset_tm));
    memset(actwk, 0, sizeof(actwk));
    memset(&new_actor, 0, sizeof(new_actor));
    animal_flg = 0;
    random_return = 0;
    random_call_count = 0;
    get_actwk_return = 0;
    get_actwk_call_count = 0;
}

static void test_sprinit_decrements_active_timers_without_spawn(
    test_context *ctx) {
    reset_fixture();
    for (Sint32 i = 0; i < 8; ++i) {
        actset_tm[i] = (Sint16)(i + 1);
    }

    sprinit();

    for (Sint32 i = 0; i < 8; ++i) {
        TEST_ASSERT_EQ_INT(ctx, i, actset_tm[i]);
    }
    TEST_ASSERT_EQ_INT(ctx, 0, random_call_count);
    TEST_ASSERT_EQ_INT(ctx, 0, get_actwk_call_count);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].ACT_NO);
}

static void test_sprinit_zero_first_timer_spawns_sonic(test_context *ctx) {
    reset_fixture();
    random_return = 5;
    actset_tm[0] = 0;

    sprinit();

    TEST_ASSERT_EQ_INT(ctx, 305, actset_tm[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, random_call_count);
    TEST_ASSERT_EQ_INT(ctx, 0, get_actwk_call_count);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].ACT_NO);
}

static void test_sprinit_zero_last_timer_spawns_flick(test_context *ctx) {
    reset_fixture();
    random_return = 17;
    for (Sint32 i = 0; i < 7; ++i) {
        actset_tm[i] = 1;
    }
    actset_tm[7] = 0;

    sprinit();

    TEST_ASSERT_EQ_INT(ctx, 137, actset_tm[7]);
    TEST_ASSERT_EQ_INT(ctx, 1, random_call_count);
    TEST_ASSERT_EQ_INT(ctx, 1, get_actwk_call_count);
    TEST_ASSERT_EQ_INT(ctx, 4, new_actor.ACT_NO);
}

static void test_sprinit_many_animal_mode_skips_eighth_slot(test_context *ctx) {
    reset_fixture();
    animal_flg = 2;
    for (Sint32 i = 0; i < 8; ++i) {
        actset_tm[i] = 1;
    }

    sprinit();

    for (Sint32 i = 0; i < 7; ++i) {
        TEST_ASSERT_EQ_INT(ctx, 0, actset_tm[i]);
    }
    TEST_ASSERT_EQ_INT(ctx, 1, actset_tm[7]);
    TEST_ASSERT_EQ_INT(ctx, 0, random_call_count);
}

static void test_sprinit_many_animal_mode_first_slot_spawns_sheep(
    test_context *ctx) {
    reset_fixture();
    animal_flg = 2;
    random_return = 15;
    actset_tm[0] = 0;

    sprinit();

    TEST_ASSERT_EQ_INT(ctx, 55, actset_tm[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, get_actwk_call_count);
    TEST_ASSERT_EQ_INT(ctx, 10, new_actor.ACT_NO);
}

static void assert_setter_success(test_context *ctx, void (*setter)(void),
                                  Uint16 expected_act_no) {
    reset_fixture();

    setter();

    TEST_ASSERT_EQ_INT(ctx, 1, get_actwk_call_count);
    TEST_ASSERT_EQ_INT(ctx, expected_act_no, new_actor.ACT_NO);
}

static void test_actor_setters_assign_expected_actor_numbers(test_context *ctx) {
    assert_setter_success(ctx, flick_set, 4);
    assert_setter_success(ctx, pocky_set, 5);
    assert_setter_success(ctx, pecky_set, 11);
    assert_setter_success(ctx, ricky_set, 6);
    assert_setter_success(ctx, inco_set, 8);
    assert_setter_success(ctx, hato_set, 9);
    assert_setter_success(ctx, sheep_set, 10);

    reset_fixture();
    sonic_set();
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].ACT_NO);
}

static void test_actor_setter_failure_leaves_new_actor_clear(test_context *ctx) {
    reset_fixture();
    get_actwk_return = -1;

    flick_set();

    TEST_ASSERT_EQ_INT(ctx, 1, get_actwk_call_count);
    TEST_ASSERT_EQ_INT(ctx, 0, new_actor.ACT_NO);
}

TEST_MAIN_BEGIN;
test_sprinit_decrements_active_timers_without_spawn(&ctx);
test_sprinit_zero_first_timer_spawns_sonic(&ctx);
test_sprinit_zero_last_timer_spawns_flick(&ctx);
test_sprinit_many_animal_mode_skips_eighth_slot(&ctx);
test_sprinit_many_animal_mode_first_slot_spawns_sheep(&ctx);
test_actor_setters_assign_expected_actor_numbers(&ctx);
test_actor_setter_failure_leaves_new_actor_clear(&ctx);
TEST_MAIN_END
