#include <string.h>

#include "support/test_runner.h"
#include "types.h"

Uint8 scdtblwk[257][16];
Uint8 scddirtbl[256];
Uint8 scdtblwk2[257][16];
Uint8 mapwka[8][64];
Uint8 prio_flag;
Uint8 *scdadr;
char dirstk[4];
Uint16 *pmapwk;
short_union stageno;

static Uint8 scdadr_storage[1024];
static Uint16 pmapwk_storage[32768];

#include "src/fcol.c"

static void reset_fcol_state(void) {
    memset(scdtblwk, 0, sizeof(scdtblwk));
    memset(scddirtbl, 0, sizeof(scddirtbl));
    memset(scdtblwk2, 0, sizeof(scdtblwk2));
    memset(mapwka, 0, sizeof(mapwka));
    memset(scdadr_storage, 0, sizeof(scdadr_storage));
    memset(dirstk, 0, sizeof(dirstk));
    memset(pmapwk_storage, 0, sizeof(pmapwk_storage));
    prio_flag = 0;
    scdadr = scdadr_storage;
    pmapwk = pmapwk_storage;
    memset(&stageno, 0, sizeof(stageno));
}

static void setup_actor(sprite_status *actor) {
    memset(actor, 0, sizeof(*actor));
    actor->actno = 1;
    actor->actflg = 128;
    actor->sproffset = 32768;
    actor->xposi.w.h = 100;
    actor->yposi.w.h = 100;
    actor->sprhs = 5;
    actor->sprvsize = 12;
}

static void fill_block(Uint8 map_block, Uint16 map_value) {
    int i;
    Uint16 *block = pmapwk_storage + ((map_block - 1) << 8);

    for (i = 0; i < 256; ++i) {
        block[i] = map_value;
    }
}

static void setup_flat_block(Uint16 map_value) {
    mapwka[0][0] = 1;
    fill_block(1, map_value);
    scdadr_storage[map_value & 1023] = 1;
    scddirtbl[1] = 0;
}

static Uint8 scd_data_for_result(Sint16 result, Sint16 coord_low) {
    return (Uint8)(char)(15 - result - coord_low);
}

static void set_down_probe_result(Sint16 x, Sint16 y, Sint16 result) {
    scdtblwk[1][x & 15] = scd_data_for_result(result, y & 15);
}

static void set_right_probe_result(Sint16 x, Sint16 result) {
    scdtblwk2[1][x & 15] = scd_data_for_result(result, x & 15);
}

static void set_rotated_probe_result(Sint16 y, Sint16 x, Sint16 result) {
    scdtblwk2[1][y & 15] = scd_data_for_result(result, x & 15);
}

static void test_fcol_skips_when_already_riding(test_context *ctx) {
    sprite_status actor;

    reset_fcol_state();
    setup_actor(&actor);
    actor.cddat = 8;
    dirstk[0] = 9;
    dirstk[2] = 9;

    TEST_ASSERT_EQ_INT(ctx, 0, fcol(&actor));
    TEST_ASSERT_EQ_INT(ctx, 0, dirstk[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, dirstk[2]);
}

static void test_fcol_dispatches_by_direction(test_context *ctx) {
    sprite_status actor;

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    set_down_probe_result(105, 112, 5);
    set_down_probe_result(95, 112, 7);
    actor.direc.b.h = 0;
    TEST_ASSERT_EQ_INT(ctx, 1, fcol(&actor));
    TEST_ASSERT_EQ_INT(ctx, 105, actor.yposi.w.h);

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 2048 | 1);
    set_rotated_probe_result(95, 87, -4);
    set_rotated_probe_result(105, 87, -4);
    actor.direc.b.h = 64;
    TEST_ASSERT_EQ_INT(ctx, 1, fcol(&actor));
    TEST_ASSERT_EQ_INT(ctx, 104, actor.xposi.w.h);

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 4096 | 1);
    set_down_probe_result(105, 87, 3);
    set_down_probe_result(95, 87, 6);
    actor.direc.b.h = 96;
    TEST_ASSERT_EQ_INT(ctx, 1, fcol(&actor));
    TEST_ASSERT_EQ_INT(ctx, 97, actor.yposi.w.h);

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    set_rotated_probe_result(95, 112, 2);
    set_rotated_probe_result(105, 112, 2);
    actor.direc.b.h = (char)192;
    TEST_ASSERT_EQ_INT(ctx, 1, fcol(&actor));
    TEST_ASSERT_EQ_INT(ctx, 102, actor.xposi.w.h);
}

static void test_fcol_down_position_and_state_branches(test_context *ctx) {
    sprite_status actor;

    reset_fcol_state();
    setup_actor(&actor);
    TEST_ASSERT_EQ_INT(ctx, 1, fcol_d(&actor));
    TEST_ASSERT_EQ_INT(ctx, 2, actor.cddat & 2);
    TEST_ASSERT_EQ_INT(ctx, 1, actor.mstno.b.l);

    reset_fcol_state();
    setup_actor(&actor);
    fcol_work_get(&actor)->force_position_adjust = 1;
    TEST_ASSERT_EQ_INT(ctx, 1, fcol_d(&actor));
    TEST_ASSERT_EQ_INT(ctx, 131, actor.yposi.w.h);

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    set_down_probe_result(105, 112, -5);
    set_down_probe_result(95, 112, -3);
    TEST_ASSERT_EQ_INT(ctx, 1, fcol_d(&actor));
    TEST_ASSERT_EQ_INT(ctx, 95, actor.yposi.w.h);

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    set_down_probe_result(105, 112, -15);
    set_down_probe_result(95, 112, -14);
    TEST_ASSERT_EQ_INT(ctx, 1, fcol_d(&actor));
    TEST_ASSERT_EQ_INT(ctx, 100, actor.yposi.w.h);

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    set_down_probe_result(105, 112, 0);
    set_down_probe_result(95, 112, 0);
    scddirtbl[1] = (Uint8)(char)-32;
    TEST_ASSERT_EQ_INT(ctx, 0, fcol_d(&actor));
    TEST_ASSERT_EQ_INT(ctx, -1, dirstk[2]);
}

static void test_fcol_side_and_up_branches(test_context *ctx) {
    sprite_status actor;

    reset_fcol_state();
    setup_actor(&actor);
    TEST_ASSERT_EQ_INT(ctx, 1, fcol_r(&actor));
    TEST_ASSERT_EQ_INT(ctx, 2, actor.cddat & 2);

    reset_fcol_state();
    setup_actor(&actor);
    fcol_work_get(&actor)->force_position_adjust = 1;
    TEST_ASSERT_EQ_INT(ctx, 1, fcol_r(&actor));
    TEST_ASSERT_EQ_INT(ctx, 131, actor.xposi.w.h);

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    set_rotated_probe_result(95, 112, -5);
    set_rotated_probe_result(105, 112, -5);
    TEST_ASSERT_EQ_INT(ctx, 1, fcol_r(&actor));
    TEST_ASSERT_EQ_INT(ctx, 95, actor.xposi.w.h);

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 2048 | 1);
    set_rotated_probe_result(95, 87, -4);
    set_rotated_probe_result(105, 87, -4);
    TEST_ASSERT_EQ_INT(ctx, 1, fcol_l(&actor));
    TEST_ASSERT_EQ_INT(ctx, 104, actor.xposi.w.h);

    reset_fcol_state();
    setup_actor(&actor);
    TEST_ASSERT_EQ_INT(ctx, 1, fcol_l(&actor));
    TEST_ASSERT_EQ_INT(ctx, 2, actor.cddat & 2);

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 4096 | 1);
    set_down_probe_result(105, 87, -3);
    set_down_probe_result(95, 87, -3);
    TEST_ASSERT_EQ_INT(ctx, 1, fcol_u(&actor));
    TEST_ASSERT_EQ_INT(ctx, 103, actor.yposi.w.h);

    reset_fcol_state();
    setup_actor(&actor);
    TEST_ASSERT_EQ_INT(ctx, 1, fcol_u(&actor));
    TEST_ASSERT_EQ_INT(ctx, 2, actor.cddat & 2);
}

static void test_fcol_zero_and_remaining_position_branches(test_context *ctx) {
    sprite_status actor;

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    set_down_probe_result(105, 112, 0);
    set_down_probe_result(95, 112, 0);
    actor.direc.b.h = (char)-16;
    TEST_ASSERT_EQ_INT(ctx, 0, fcol(&actor));

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    set_rotated_probe_result(95, 112, 0);
    set_rotated_probe_result(105, 112, 0);
    TEST_ASSERT_EQ_INT(ctx, 0, fcol_r(&actor));

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 4096 | 1);
    set_down_probe_result(105, 87, 0);
    set_down_probe_result(95, 87, 0);
    TEST_ASSERT_EQ_INT(ctx, 0, fcol_u(&actor));

    reset_fcol_state();
    setup_actor(&actor);
    fcol_work_get(&actor)->force_position_adjust = 1;
    TEST_ASSERT_EQ_INT(ctx, 1, fcol_u(&actor));
    TEST_ASSERT_EQ_INT(ctx, 76, actor.yposi.w.h);

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 2048 | 1);
    set_rotated_probe_result(95, 87, 0);
    set_rotated_probe_result(105, 87, 0);
    TEST_ASSERT_EQ_INT(ctx, 0, fcol_l(&actor));

    reset_fcol_state();
    setup_actor(&actor);
    fcol_work_get(&actor)->force_position_adjust = 1;
    TEST_ASSERT_EQ_INT(ctx, 1, fcol_l(&actor));
    TEST_ASSERT_EQ_INT(ctx, 76, actor.xposi.w.h);

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 2048 | 1);
    set_rotated_probe_result(95, 87, 4);
    set_rotated_probe_result(105, 87, 4);
    TEST_ASSERT_EQ_INT(ctx, 1, fcol_l(&actor));
    TEST_ASSERT_EQ_INT(ctx, 96, actor.xposi.w.h);
}

static void test_scdchk_fallbacks_and_collision_data(test_context *ctx) {
    sprite_status actor;
    char direction;

    reset_fcol_state();
    setup_actor(&actor);
    direction = 77;
    TEST_ASSERT_EQ_INT(ctx, 20, scdchk(&actor, 10, 11, 16, 0, 13, &direction));
    TEST_ASSERT_EQ_INT(ctx, 77, direction);

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(1);
    direction = 77;
    TEST_ASSERT_EQ_INT(ctx, 20, scdchk(&actor, 10, 11, 16, 0, 13, &direction));

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    direction = 77;
    TEST_ASSERT_EQ_INT(ctx, 20, scdchk(&actor, 10, 11, 16, 0, 13, &direction));

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    scdadr_storage[1] = 0;
    TEST_ASSERT_EQ_INT(ctx, 20, scdchk(&actor, 10, 11, 16, 0, 13, &direction));

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    scddirtbl[1] = 32;
    scdtblwk[1][10] = 4;
    TEST_ASSERT_EQ_INT(ctx, 0, scdchk(&actor, 10, 11, 16, 0, 13, &direction));
    TEST_ASSERT_EQ_INT(ctx, 32, direction);

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 2048 | 4096 | 1);
    scddirtbl[1] = 32;
    scdtblwk[1][~10 & 15] = 4;
    TEST_ASSERT_EQ_INT(ctx, -12, scdchk(&actor, 10, 11, 16, 0, 13, &direction));
    TEST_ASSERT_EQ_INT(ctx, 0, direction);

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    scdtblwk[1][10] = (Uint8)(char)-2;
    TEST_ASSERT_EQ_INT(ctx, 20, scdchk(&actor, 10, 11, -16, 0, 13, &direction));

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    scdtblwk[1][10] = (Uint8)(char)-12;
    TEST_ASSERT_EQ_INT(ctx, -28, scdchk(&actor, 10, 11, -16, 0, 13, &direction));

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    scdtblwk[1][10] = 16;
    TEST_ASSERT_EQ_INT(ctx, -28, scdchk(&actor, 10, 11, 16, 0, 13, &direction));
}

static void test_scdchk2_data_paths(test_context *ctx) {
    sprite_status actor;
    char direction;

    reset_fcol_state();
    setup_actor(&actor);
    direction = 5;
    TEST_ASSERT_EQ_INT(ctx, 4, scdchk2(&actor, 10, 11, 16, 0, 13, &direction));

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    scdtblwk[1][10] = 4;
    TEST_ASSERT_EQ_INT(ctx, 0, scdchk2(&actor, 10, 11, 16, 0, 13, &direction));

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    scdadr_storage[1] = 0;
    TEST_ASSERT_EQ_INT(ctx, 4, scdchk2(&actor, 10, 11, 16, 0, 13, &direction));

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 2048 | 1);
    scddirtbl[1] = 32;
    scdtblwk[1][~10 & 15] = 4;
    TEST_ASSERT_EQ_INT(ctx, 0, scdchk2(&actor, 10, 11, 16, 0, 13, &direction));
    TEST_ASSERT_EQ_INT(ctx, -32, direction);

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 4096 | 1);
    scdtblwk[1][10] = 4;
    TEST_ASSERT_EQ_INT(ctx, 0, scdchk2(&actor, 10, 11, 16, 4096, 13, &direction));

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 4096 | 1);
    scdtblwk[1][10] = 4;
    TEST_ASSERT_EQ_INT(ctx, 4, scdchk2(&actor, 10, 11, -16, 0, 13, &direction));

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    scdtblwk[1][10] = (Uint8)(char)-2;
    TEST_ASSERT_EQ_INT(ctx, 4, scdchk2(&actor, 10, 11, -16, 0, 13, &direction));

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    scdtblwk[1][10] = (Uint8)(char)-12;
    TEST_ASSERT_EQ_INT(ctx, -12, scdchk2(&actor, 10, 11, -16, 0, 13, &direction));

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    scdtblwk[1][10] = (Uint8)(char)-2;
    TEST_ASSERT_EQ_INT(ctx, -12, scdchk2(&actor, 10, 11, 16, 0, 13, &direction));
}

static void test_rotated_scd_paths(test_context *ctx) {
    sprite_status actor;
    char direction;

    reset_fcol_state();
    setup_actor(&actor);
    direction = 5;
    TEST_ASSERT_EQ_INT(ctx, 21, scdchk_r(&actor, 10, 11, 16, 0, 13, &direction));

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    scdtblwk2[1][11] = 4;
    TEST_ASSERT_EQ_INT(ctx, 1, scdchk_r(&actor, 10, 11, 16, 0, 13, &direction));

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    scdadr_storage[1] = 0;
    TEST_ASSERT_EQ_INT(ctx, 21, scdchk_r(&actor, 10, 11, 16, 0, 13, &direction));

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    scdtblwk2[1][11] = 0;
    TEST_ASSERT_EQ_INT(ctx, 21, scdchk_r(&actor, 10, 11, 16, 0, 13, &direction));

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 2048 | 4096 | 1);
    scddirtbl[1] = 32;
    scdtblwk2[1][~11 & 15] = 4;
    TEST_ASSERT_EQ_INT(ctx, 21, scdchk_r(&actor, 10, 11, 16, 0, 13, &direction));
    TEST_ASSERT_EQ_INT(ctx, -96, direction);

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    scdtblwk2[1][11] = 16;
    TEST_ASSERT_EQ_INT(ctx, -27, scdchk_r(&actor, 10, 11, 16, 0, 13, &direction));

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    scdtblwk2[1][11] = (Uint8)(char)-2;
    TEST_ASSERT_EQ_INT(ctx, 21, scdchk_r(&actor, 10, 11, -16, 0, 13, &direction));

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    scdtblwk2[1][11] = (Uint8)(char)-12;
    TEST_ASSERT_EQ_INT(ctx, -27, scdchk_r(&actor, 10, 11, -16, 0, 13, &direction));
}

static void test_rotated_scdchk2_data_paths(test_context *ctx) {
    sprite_status actor;
    char direction;

    reset_fcol_state();
    setup_actor(&actor);
    TEST_ASSERT_EQ_INT(ctx, 5, scdchk2_r(&actor, 10, 11, 0, 13, &direction));

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    scdtblwk2[1][11] = 4;
    TEST_ASSERT_EQ_INT(ctx, 1, scdchk2_r(&actor, 10, 11, 0, 13, &direction));

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    scdadr_storage[1] = 0;
    TEST_ASSERT_EQ_INT(ctx, 5, scdchk2_r(&actor, 10, 11, 0, 13, &direction));

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    scdtblwk2[1][11] = 0;
    TEST_ASSERT_EQ_INT(ctx, 5, scdchk2_r(&actor, 10, 11, 0, 13, &direction));

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 2048 | 1);
    scdtblwk2[1][11] = 4;
    TEST_ASSERT_EQ_INT(ctx, 1, scdchk2_r(&actor, 10, 11, 2048, 13, &direction));

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    scdtblwk2[1][11] = (Uint8)(char)-2;
    TEST_ASSERT_EQ_INT(ctx, 5, scdchk2_r(&actor, 10, 11, 0, 13, &direction));

    reset_fcol_state();
    setup_actor(&actor);
    setup_flat_block(8192 | 1);
    scdtblwk2[1][11] = (Uint8)(char)-12;
    TEST_ASSERT_EQ_INT(ctx, -11, scdchk2_r(&actor, 10, 11, 0, 13, &direction));
}

static void test_scramapad_positive_and_negative_stage_rules(
    test_context *ctx) {
    sprite_status actor;

    reset_fcol_state();
    setup_actor(&actor);
    stageno.b.h = 2;
    fill_block(1, 0x1111);
    mapwka[0][0] = 1;
    TEST_ASSERT_EQ_INT(ctx, 0x1111, scramapad(&actor, 1, 1));

    reset_fcol_state();
    setup_actor(&actor);
    fill_block(2, 0x1234);
    mapwka[0][0] = 2;
    TEST_ASSERT_EQ_INT(ctx, 0x1234, scramapad(&actor, -1, -1));

    /* R8 (Metallic Madness): above the top of the level is empty space, as on
       Mega-CD, so the player can run along the level's top. */
    reset_fcol_state();
    setup_actor(&actor);
    stageno.b.h = 6;
    fill_block(2, 0x1234);
    mapwka[0][0] = 2;
    TEST_ASSERT_EQ_INT(ctx, 0, scramapad(&actor, 1, -1));

    reset_fcol_state();
    setup_actor(&actor);
    actor.sproffset = 0xFFFF;
    stageno.b.h = 5;
    fill_block(1, 0x2222);
    mapwka[0][0] = 1;
    TEST_ASSERT_EQ_INT(ctx, 0x2222, scramapad(&actor, 1, 1));
    TEST_ASSERT_EQ_INT(ctx, 0x7FFF, actor.sproffset);

    reset_fcol_state();
    setup_actor(&actor);
    actor.actflg = 255;
    stageno.b.h = 4;
    fill_block(1, 0x3333);
    mapwka[0][0] = 1;
    TEST_ASSERT_EQ_INT(ctx, 0x3333, scramapad(&actor, 1, 1));
    TEST_ASSERT_EQ_INT(ctx, 191, actor.actflg);

    reset_fcol_state();
    setup_actor(&actor);
    stageno.w = 1;
    mapwka[0][0] = (Uint8)(char)-1;
    fill_block(127, 0x4444);
    TEST_ASSERT_EQ_INT(ctx, 0x4444, scramapad(&actor, 1, 1));

    reset_fcol_state();
    setup_actor(&actor);
    stageno.b.h = 4;
    mapwka[0][0] = (Uint8)(char)-20;
    fill_block(108, 0x5555);
    TEST_ASSERT_EQ_INT(ctx, 0x5555, scramapad(&actor, 1, 1));

    reset_fcol_state();
    setup_actor(&actor);
    stageno.b.h = 5;
    mapwka[0][0] = (Uint8)(char)-40;
    fill_block(88, 0x6666);
    TEST_ASSERT_EQ_INT(ctx, 0x6666, scramapad(&actor, 1, 1));

    reset_fcol_state();
    setup_actor(&actor);
    stageno.b.h = 6;
    mapwka[0][0] = (Uint8)(char)-70;
    fill_block(58, 0x7777);
    TEST_ASSERT_EQ_INT(ctx, 0x7777, scramapad(&actor, 1, 1));
}

static void test_stage_block_remap_helpers(test_context *ctx) {
    sprite_status actor;

    reset_fcol_state();
    setup_actor(&actor);
    TEST_ASSERT_EQ_INT(ctx, 40, st_elseDataChk(40, &actor));
    actor.actflg = 64;
    TEST_ASSERT_EQ_INT(ctx, 81, st_elseDataChk(40, &actor));
    TEST_ASSERT_EQ_INT(ctx, 42, st_elseDataChk(41, &actor));

    reset_fcol_state();
    setup_actor(&actor);
    TEST_ASSERT_EQ_INT(ctx, 20, st_wackyDataChk(20, &actor));
    TEST_ASSERT_EQ_INT(ctx, 64, actor.actflg & 64);
    actor.actflg = 64;
    TEST_ASSERT_EQ_INT(ctx, 96, st_wackyDataChk(21, &actor));
    TEST_ASSERT_EQ_INT(ctx, 97, st_wackyDataChk(30, &actor));
    TEST_ASSERT_EQ_INT(ctx, 98, st_wackyDataChk(31, &actor));
    TEST_ASSERT_EQ_INT(ctx, 99, st_wackyDataChk(50, &actor));
    TEST_ASSERT_EQ_INT(ctx, 10, st_wackyDataChk(10, &actor));

    reset_fcol_state();
    setup_actor(&actor);
    TEST_ASSERT_EQ_INT(ctx, 4, st_starDataChk(4, &actor));
    TEST_ASSERT_EQ_INT(ctx, 0, actor.sproffset & 32768);
    actor.actflg = 64;
    TEST_ASSERT_EQ_INT(ctx, 7, st_starDataChk(6, &actor));
    prio_flag = 0;
    TEST_ASSERT_EQ_INT(ctx, 40, st_starDataChk(40, &actor));
    prio_flag = 1;
    TEST_ASSERT_EQ_INT(ctx, 41, st_starDataChk(40, &actor));
    TEST_ASSERT_EQ_INT(ctx, 61, st_starDataChk(60, &actor));
    TEST_ASSERT_EQ_INT(ctx, 56, st_starDataChk(55, &actor));
    TEST_ASSERT_EQ_INT(ctx, 48, st_starDataChk(47, &actor));
    TEST_ASSERT_EQ_INT(ctx, 23, st_starDataChk(22, &actor));
    TEST_ASSERT_EQ_INT(ctx, 10, st_starDataChk(10, &actor));

    reset_fcol_state();
    setup_actor(&actor);
    actor.actno = 3;
    TEST_ASSERT_EQ_INT(ctx, 70, st_metalDataChk(70, &actor));
    actor.actno = 1;
    prio_flag = 0;
    TEST_ASSERT_EQ_INT(ctx, 70, st_metalDataChk(70, &actor));
    prio_flag = 1;
    TEST_ASSERT_EQ_INT(ctx, 106, st_metalDataChk(70, &actor));
    TEST_ASSERT_EQ_INT(ctx, 107, st_metalDataChk(72, &actor));
    TEST_ASSERT_EQ_INT(ctx, 108, st_metalDataChk(74, &actor));
    TEST_ASSERT_EQ_INT(ctx, 109, st_metalDataChk(16, &actor));
    TEST_ASSERT_EQ_INT(ctx, 110, st_metalDataChk(99, &actor));
    TEST_ASSERT_EQ_INT(ctx, 111, st_metalDataChk(67, &actor));
    TEST_ASSERT_EQ_INT(ctx, 10, st_metalDataChk(10, &actor));
}

static void test_small_helpers(test_context *ctx) {
    sprite_status actor;

    reset_fcol_state();
    setup_actor(&actor);
    dirstk[0] = 8;
    dirstk[2] = 6;
    TEST_ASSERT_EQ_INT(ctx, 3, dirchk(&actor, 3, 7));
    TEST_ASSERT_EQ_INT(ctx, 8, actor.direc.b.h);

    dirstk[0] = 8;
    dirstk[2] = 7;
    actor.direc.b.h = 96;
    TEST_ASSERT_EQ_INT(ctx, 2, dirchk(&actor, 9, 2));
    TEST_ASSERT_EQ_INT(ctx, 128, (Uint8)actor.direc.b.h);

    TEST_ASSERT_EQ_INT(ctx, 0, scdcnv());
    TEST_ASSERT_EQ_INT(ctx, 1, CCset(1, 2));
    TEST_ASSERT_EQ_INT(ctx, 0, CCset(2, 1));
    TEST_ASSERT_EQ_INT(ctx, 1, CSset(65535, 1));
    TEST_ASSERT_EQ_INT(ctx, 0, CSset(10, 20));
    TEST_ASSERT_EQ_INT(ctx, 5, bchg(2, 1));
}

TEST_MAIN_BEGIN;
    test_fcol_skips_when_already_riding(&ctx);
    test_fcol_dispatches_by_direction(&ctx);
    test_fcol_down_position_and_state_branches(&ctx);
    test_fcol_side_and_up_branches(&ctx);
    test_fcol_zero_and_remaining_position_branches(&ctx);
    test_scdchk_fallbacks_and_collision_data(&ctx);
    test_scdchk2_data_paths(&ctx);
    test_rotated_scd_paths(&ctx);
    test_rotated_scdchk2_data_paths(&ctx);
    test_scramapad_positive_and_negative_stage_rules(&ctx);
    test_stage_block_remap_helpers(&ctx);
    test_small_helpers(&ctx);
TEST_MAIN_END;
