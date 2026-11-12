/* <stdlib.h> declares POSIX random(); the decompiled title/planet headers
   declare the game's own Sint32 random(void).  Parse stdlib first, then rename
   the game symbol inside this harness so the two can coexist. */
#include <stdlib.h>

#define random game_random

#include <string.h>

#include "support/test_runner.h"
#include "types.h"

#include "src/title/planet/actm.c"

typedef struct {
    Sint16 x;
    Sint16 y;
    Uint16 index;
    Uint16 link;
    Uint16 flags;
} sprite_call;

typedef struct {
    Sint16 mapnum;
    sprite_pattern_mapping_title aMapDat[80];
} large_sprite_pattern_mappings_title;

static sprite_call sprite_calls[96];
static int sprite_call_count;
static int sprinit_calls;
static int control_calls[7];

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

static void stub_memset(void *dest, Uint8 value, Sint32 count) {
    memset(dest, value, (size_t)count);
}

static void stub_easprset(Sint16 x, Sint16 y, Uint16 index, Uint16 link,
                          Uint16 flags) {
    if (sprite_call_count < (int)(sizeof(sprite_calls) / sizeof(sprite_calls[0]))) {
        sprite_calls[sprite_call_count].x = x;
        sprite_calls[sprite_call_count].y = y;
        sprite_calls[sprite_call_count].index = index;
        sprite_calls[sprite_call_count].link = link;
        sprite_calls[sprite_call_count].flags = flags;
    }
    ++sprite_call_count;
}

void (*EAsprset)(Sint16, Sint16, Uint16, Uint16, Uint16) = stub_easprset;
void (*sMemSet)(void *, Uint8, Sint32) = stub_memset;

void sprinit(void) { ++sprinit_calls; }

void flicky_ctrl(sprite_status_lpl *pActwk) {
    ++control_calls[0];
    pActwk->SPR_FLG |= 16;
}
void star_ctrl(sprite_status_lpl *pActwk) {
    ++control_calls[1];
    pActwk->SPR_FLG |= 16;
}
void eggm_ctrl(sprite_status_lpl *pActwk) {
    ++control_calls[2];
    pActwk->SPR_FLG |= 16;
}
void ufo_ctrl(sprite_status_lpl *pActwk) {
    ++control_calls[3];
    pActwk->SPR_FLG |= 16;
}
void msnc_ctrl(sprite_status_lpl *pActwk) {
    ++control_calls[4];
    pActwk->SPR_FLG |= 16;
}
void tails_ctrl(sprite_status_lpl *pActwk) {
    ++control_calls[5];
    pActwk->SPR_FLG |= 16;
}
void ascii_sprite(sprite_status_lpl *pActwk) {
    ++control_calls[6];
    pActwk->SPR_FLG |= 16;
}

static void reset_state(void) {
    memset(actwk, 0, sizeof(actwk));
    memset(sprite_calls, 0, sizeof(sprite_calls));
    memset(control_calls, 0, sizeof(control_calls));
    sprite_call_count = 0;
    sprinit_calls = 0;
    actset_flg = 0;
    EAsprset = stub_easprset;
    sMemSet = stub_memset;
}

static sprite_patterns_title make_pattern(sprite_pattern_mappings_title *map0,
                                          sprite_pattern_mappings_title *map1) {
    sprite_patterns_title pattern;

    memset(&pattern, 0, sizeof(pattern));
    pattern.patnum = 2;
    pattern.aPatDat[0].timer = 5;
    pattern.aPatDat[0].paMapTbl = map0;
    pattern.aPatDat[1].timer = 9;
    pattern.aPatDat[1].paMapTbl = map1;
    return pattern;
}

static void test_get_and_check_actor_slots(test_context *ctx) {
    sprite_status_lpl *slot;

    reset_state();
    TEST_ASSERT_EQ_INT(ctx, 0, chk_actwk());
    TEST_ASSERT_EQ_INT(ctx, 0, get_actwk(&slot));
    TEST_ASSERT_TRUE(ctx, slot == &actwk[0]);

    for (int i = 0; i < 16; ++i)
        actwk[i].ACT_NO = (Uint16)(i + 1);

    TEST_ASSERT_EQ_INT(ctx, -1, chk_actwk());
    TEST_ASSERT_EQ_INT(ctx, -1, get_actwk(&slot));

    actwk[7].ACT_NO = 0;
    TEST_ASSERT_EQ_INT(ctx, 0, get_actwk(&slot));
    TEST_ASSERT_TRUE(ctx, slot == &actwk[7]);
}

static void test_action_dispatches_controls_and_clears_killed_actors(
    test_context *ctx) {
    reset_state();

    for (int i = 0; i < 7; ++i)
        actwk[i].ACT_NO = (Uint16)(i + 1);

    action();

    TEST_ASSERT_EQ_INT(ctx, 1, sprinit_calls);
    for (int i = 0; i < 7; ++i) {
        TEST_ASSERT_EQ_INT(ctx, 1, control_calls[i]);
        TEST_ASSERT_EQ_INT(ctx, 0, actwk[i].ACT_NO);
    }
    TEST_ASSERT_EQ_INT(ctx, 80, sprite_call_count);
    TEST_ASSERT_EQ_INT(ctx, 0, sprite_calls[0].link);
    TEST_ASSERT_EQ_INT(ctx, 79, sprite_calls[79].link);
}

static void test_dummy_actor_toggles_flag_before_clear(test_context *ctx) {
    reset_state();

    actwk[0].ACT_NO = 8;
    actwk[0].ACT_FLG = 5;

    action();

    TEST_ASSERT_EQ_INT(ctx, 5, actset_flg);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].ACT_FLG);
}

static void test_spr_set_emits_exact_mapping_coordinates_and_flags(
    test_context *ctx) {
    sprite_pattern_mappings_title map0;
    sprite_pattern_mappings_title map1;
    sprite_patterns_title pattern;

    reset_state();
    memset(&map0, 0, sizeof(map0));
    memset(&map1, 0, sizeof(map1));
    map0.mapnum = 2;
    map0.aMapDat[0].index = 100;
    map0.aMapDat[0].etc = 4;
    map0.aMapDat[0].hoffs = 3;
    map0.aMapDat[0].hrevoffs = 30;
    map0.aMapDat[0].voffs = 5;
    map0.aMapDat[0].vrevoffs = 50;
    map0.aMapDat[1].index = 101;
    map0.aMapDat[1].etc = 8;
    map0.aMapDat[1].hoffs = -7;
    map0.aMapDat[1].hrevoffs = -70;
    map0.aMapDat[1].voffs = 9;
    map0.aMapDat[1].vrevoffs = 90;
    map1.mapnum = 1;
    map1.aMapDat[0].index = 200;
    map1.aMapDat[0].etc = 3;
    map1.aMapDat[0].hoffs = 11;
    map1.aMapDat[0].hrevoffs = 110;
    map1.aMapDat[0].voffs = 13;
    map1.aMapDat[0].vrevoffs = 130;
    pattern = make_pattern(&map0, &map1);

    actwk[0].ACT_NO = 1;
    actwk[0].PAT_ADR = &pattern;
    actwk[0].PAT_NO = 0;
    actwk[0].SPR_TIMER = 2;
    actwk[0].XPOSI.w.h = 20;
    actwk[0].YPOSI.w.h = 30;

    actwk[1].ACT_NO = 2;
    actwk[1].PAT_ADR = &pattern;
    actwk[1].PAT_NO = 0;
    actwk[1].SPR_TIMER = 2;
    actwk[1].SPR_FLG = 128 | 64 | 32;
    actwk[1].XPOSI.w.h = 200;
    actwk[1].YPOSI.w.h = 120;

    actwk[2].ACT_NO = 3;

    spr_set();

    TEST_ASSERT_EQ_INT(ctx, 4, sprite_call_count);
    TEST_ASSERT_EQ_INT(ctx, 145, sprite_calls[0].x);
    TEST_ASSERT_EQ_INT(ctx, 153, sprite_calls[0].y);
    TEST_ASSERT_EQ_INT(ctx, 100, sprite_calls[0].index);
    TEST_ASSERT_EQ_INT(ctx, 0, sprite_calls[0].link);
    TEST_ASSERT_EQ_INT(ctx, 4, sprite_calls[0].flags);
    TEST_ASSERT_EQ_INT(ctx, 155, sprite_calls[1].x);
    TEST_ASSERT_EQ_INT(ctx, 149, sprite_calls[1].y);
    TEST_ASSERT_EQ_INT(ctx, 101, sprite_calls[1].index);
    TEST_ASSERT_EQ_INT(ctx, 1, sprite_calls[1].link);
    TEST_ASSERT_EQ_INT(ctx, 8, sprite_calls[1].flags);
    TEST_ASSERT_EQ_INT(ctx, 298, sprite_calls[2].x);
    TEST_ASSERT_EQ_INT(ctx, 198, sprite_calls[2].y);
    TEST_ASSERT_EQ_INT(ctx, 100, sprite_calls[2].index);
    TEST_ASSERT_EQ_INT(ctx, 2, sprite_calls[2].link);
    TEST_ASSERT_EQ_INT(ctx, 32775, sprite_calls[2].flags);
    TEST_ASSERT_EQ_INT(ctx, 398, sprite_calls[3].x);
    TEST_ASSERT_EQ_INT(ctx, 158, sprite_calls[3].y);
    TEST_ASSERT_EQ_INT(ctx, 101, sprite_calls[3].index);
    TEST_ASSERT_EQ_INT(ctx, 3, sprite_calls[3].link);
    TEST_ASSERT_EQ_INT(ctx, 32779, sprite_calls[3].flags);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[0].PAT_NO);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].SPR_TIMER);
}

static void test_spr_set_advances_and_wraps_patterns(test_context *ctx) {
    sprite_pattern_mappings_title map0;
    sprite_pattern_mappings_title map1;
    sprite_patterns_title pattern;

    reset_state();
    memset(&map0, 0, sizeof(map0));
    memset(&map1, 0, sizeof(map1));
    map0.mapnum = 1;
    map0.aMapDat[0].index = 10;
    map1.mapnum = 1;
    map1.aMapDat[0].index = 20;
    pattern = make_pattern(&map0, &map1);

    actwk[0].ACT_NO = 1;
    actwk[0].PAT_ADR = &pattern;
    actwk[0].PAT_NO = 0;
    actwk[0].SPR_TIMER = 0;
    actwk[0].XPOSI.w.h = 40;
    actwk[0].YPOSI.w.h = 50;

    actwk[1].ACT_NO = 2;
    actwk[1].PAT_ADR = &pattern;
    actwk[1].PAT_NO = 1;
    actwk[1].SPR_TIMER = 0;
    actwk[1].XPOSI.w.h = 60;
    actwk[1].YPOSI.w.h = 70;

    spr_set();

    TEST_ASSERT_EQ_INT(ctx, 2, sprite_call_count);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].PAT_NO);
    TEST_ASSERT_EQ_INT(ctx, 9, actwk[0].SPR_TIMER);
    TEST_ASSERT_EQ_INT(ctx, 20, sprite_calls[0].index);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[1].PAT_NO);
    TEST_ASSERT_EQ_INT(ctx, 5, actwk[1].SPR_TIMER);
    TEST_ASSERT_EQ_INT(ctx, 10, sprite_calls[1].index);
}

static void test_spr_set_stops_after_eighty_sprite_links(test_context *ctx) {
    large_sprite_pattern_mappings_title large_map;
    sprite_patterns_title pattern;

    reset_state();
    memset(&large_map, 0, sizeof(large_map));
    large_map.mapnum = 80;
    for (int i = 0; i < 80; ++i) {
        large_map.aMapDat[i].index = (Sint16)(300 + i);
        large_map.aMapDat[i].hoffs = (Sint16)i;
    }
    memset(&pattern, 0, sizeof(pattern));
    pattern.patnum = 1;
    pattern.aPatDat[0].timer = 1;
    pattern.aPatDat[0].paMapTbl = (sprite_pattern_mappings_title *)&large_map;

    actwk[0].ACT_NO = 1;
    actwk[0].PAT_ADR = &pattern;
    actwk[0].SPR_TIMER = 1;
    actwk[0].XPOSI.w.h = 100;
    actwk[0].YPOSI.w.h = 40;

    spr_set();

    TEST_ASSERT_EQ_INT(ctx, 80, sprite_call_count);
    TEST_ASSERT_EQ_INT(ctx, 0, sprite_calls[0].link);
    TEST_ASSERT_EQ_INT(ctx, 300, sprite_calls[0].index);
    TEST_ASSERT_EQ_INT(ctx, 228, sprite_calls[0].x);
    TEST_ASSERT_EQ_INT(ctx, 79, sprite_calls[79].link);
    TEST_ASSERT_EQ_INT(ctx, 379, sprite_calls[79].index);
    TEST_ASSERT_EQ_INT(ctx, 149, sprite_calls[79].x);
}

TEST_MAIN_BEGIN;
    test_get_and_check_actor_slots(&ctx);
    test_action_dispatches_controls_and_clears_killed_actors(&ctx);
    test_dummy_actor_toggles_flag_before_clear(&ctx);
    test_spr_set_emits_exact_mapping_coordinates_and_flags(&ctx);
    test_spr_set_advances_and_wraps_patterns(&ctx);
    test_spr_set_stops_after_eighty_sprite_links(&ctx);
TEST_MAIN_END
