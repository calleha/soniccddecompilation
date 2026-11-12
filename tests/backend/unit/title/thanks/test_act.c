/* <stdlib.h> declares POSIX random(); the decompiled thanks headers declare
   the game's own Sint32 random(void).  Parse stdlib first, then rename the game
   symbol inside this harness so the two can coexist. */
#include <stdlib.h>

#define random game_random

#include <string.h>

#include "support/test_runner.h"
#include "src/types.h"

static Sint32 memset_call_count;
static void *memset_ptrs[128];
static Uint8 memset_values[128];
static Sint32 memset_sizes[128];
static Sint32 easprset_call_count;
static Sint16 easprset_x[256];
static Sint16 easprset_y[256];
static Uint16 easprset_index[256];
static Uint16 easprset_link[256];
static Uint16 easprset_rev[256];
static Sint32 sonic_call_count;
static Sint32 emie_call_count;
static Sint32 metals_call_count;

#include "src/title/thanks/act.c"

sprite_status_thanks actwk[66];
sprite_status_thanks *pribuf0[64];
sprite_status_thanks *pribuf1[64];
sprite_status_thanks *pribuf2[64];
sprite_status_thanks *pribuf3[64];
sprite_status_thanks **priptr[4];

void (*CDPause)(Sint16) = 0;
void (*CDPlay)(Sint16) = 0;
void (*ChangeTileBmp)(Sint32, Sint32) = 0;
void (*sCloseFile)(Sint32) = 0;
void (*ClrSpriteDebug)(void) = 0;
Sint32 (*sGetFileSize)(Sint32) = 0;
void *(*sMemAlloc)(Sint32) = 0;
Sint32 (*sMemCmp)(void *, void *, Sint32) = 0;
void (*sMemCpy)(void *, void *, Sint32) = 0;
void (*sMemFree)(void *) = 0;
Sint32 (*sReadFile)(Sint32, void *, Sint32) = 0;
Sint32 (*ReadIndx)(Uint32) = 0;
Uint32 (*ReadScore)(Sint32, char *, Uint32) = 0;
Sint32 (*sOpenFile)(char *) = 0;
Uint32 (*OpenScore)(Sint32) = 0;
void (*sOutputDebugString)(char *) = 0;
void (*sPrintf)(char *, const char *, ...) = 0;
Sint32 (*sRandom)(void) = 0;
Sint32 (*SetGrid)(Sint32, Sint32, Sint32, Sint32, Sint32) = 0;
char *(*sStrcpy)(char *, char *) = 0;
void (*WaveAllStop)(void) = 0;
void (*WaveRequest)(Sint16) = 0;
Uint32 (*WriteIndx)(Sint32, Uint32) = 0;
Uint32 (*WriteScore)(Sint32, char *, Uint32) = 0;

static void fake_memset(void *dst, Uint8 value, Sint32 size) {
    memset_ptrs[memset_call_count] = dst;
    memset_values[memset_call_count] = value;
    memset_sizes[memset_call_count] = size;
    ++memset_call_count;
    memset(dst, value, (size_t)size);
}

static void fake_easprset(Sint16 x, Sint16 y, Uint16 index, Uint16 link,
                          Uint16 rev) {
    if (easprset_call_count < 256) {
        easprset_x[easprset_call_count] = x;
        easprset_y[easprset_call_count] = y;
        easprset_index[easprset_call_count] = index;
        easprset_link[easprset_call_count] = link;
        easprset_rev[easprset_call_count] = rev;
    }
    ++easprset_call_count;
}

void (*EAsprset)(Sint16, Sint16, Uint16, Uint16, Uint16) = fake_easprset;
void (*sMemSet)(void *, Uint8, Sint32) = fake_memset;

void sonic_ctrl(sprite_status_thanks *pActwk) {
    ++sonic_call_count;
    pActwk->ACT_NO = 0;
}

void emie_ctrl(sprite_status_thanks *pActwk) {
    ++emie_call_count;
    pActwk->SPR_FLG |= 16;
}

void metals_ctrl(sprite_status_thanks *pActwk) {
    ++metals_call_count;
    pActwk->ACT_NO = 0;
}

void flick_ctrl(sprite_status_thanks *pActwk) { pActwk->ACT_NO = 0; }
void pocky_ctrl(sprite_status_thanks *pActwk) { pActwk->ACT_NO = 0; }
void pecky_ctrl(sprite_status_thanks *pActwk) { pActwk->ACT_NO = 0; }
void ricky_ctrl(sprite_status_thanks *pActwk) { pActwk->ACT_NO = 0; }
void sheep_ctrl(sprite_status_thanks *pActwk) { pActwk->ACT_NO = 0; }
void inco_ctrl(sprite_status_thanks *pActwk) { pActwk->ACT_NO = 0; }
void hato_ctrl(sprite_status_thanks *pActwk) { pActwk->ACT_NO = 0; }
void heart_ctrl(sprite_status_thanks *pActwk) { pActwk->ACT_NO = 0; }

static sprite_pattern_mappings_title mappings_a = {
    2,
    {{10, 4, 2, 5, 3, 7}, {20, 8, -4, -9, 6, -11}},
};
static sprite_pattern_mappings_title mappings_b = {
    1,
    {{30, 1, 1, 2, 3, 4}, {0, 0, 0, 0, 0, 0}},
};
static sprite_patterns_title patterns = {
    2,
    {{2, &mappings_a}, {5, &mappings_b}},
};

static void reset_fixture(void) {
    memset(actwk, 0, sizeof(actwk));
    memset(pribuf0, 0, sizeof(pribuf0));
    memset(pribuf1, 0, sizeof(pribuf1));
    memset(pribuf2, 0, sizeof(pribuf2));
    memset(pribuf3, 0, sizeof(pribuf3));
    memset(priptr, 0, sizeof(priptr));
    memset(memset_ptrs, 0, sizeof(memset_ptrs));
    memset(memset_values, 0, sizeof(memset_values));
    memset(memset_sizes, 0, sizeof(memset_sizes));
    memset(easprset_x, 0, sizeof(easprset_x));
    memset(easprset_y, 0, sizeof(easprset_y));
    memset(easprset_index, 0, sizeof(easprset_index));
    memset(easprset_link, 0, sizeof(easprset_link));
    memset(easprset_rev, 0, sizeof(easprset_rev));
    memset_call_count = 0;
    easprset_call_count = 0;
    sonic_call_count = 0;
    emie_call_count = 0;
    metals_call_count = 0;
}

static void test_get_actwk_finds_first_free_slot_after_reserved_actors(
    test_context *ctx) {
    sprite_status_thanks *new_actor = 0;

    reset_fixture();
    for (Sint32 i = 3; i <= 63; ++i) {
        actwk[i].ACT_NO = 99;
    }
    actwk[7].ACT_NO = 0;

    TEST_ASSERT_EQ_INT(ctx, 0, get_actwk(&new_actor));
    TEST_ASSERT_TRUE(ctx, new_actor == &actwk[7]);

    actwk[7].ACT_NO = 99;
    TEST_ASSERT_EQ_INT(ctx, -1, get_actwk(&new_actor));
}

static void test_chk_actwk_scans_work_slots(test_context *ctx) {
    reset_fixture();
    for (Sint32 i = 0; i <= 60; ++i) {
        actwk[i].ACT_NO = 1;
    }

    TEST_ASSERT_EQ_INT(ctx, -1, chk_actwk());

    actwk[60].ACT_NO = 0;
    TEST_ASSERT_EQ_INT(ctx, 0, chk_actwk());
}

static void test_priset_groups_active_actors_by_priority(test_context *ctx) {
    reset_fixture();
    actwk[1].ACT_NO = 1;
    actwk[1].free[0] = 2;
    actwk[3].ACT_NO = 1;
    actwk[3].free[0] = 0;
    actwk[5].ACT_NO = 1;
    actwk[5].free[0] = 2;

    priset();

    TEST_ASSERT_TRUE(ctx, pribuf0[0] == &actwk[3]);
    TEST_ASSERT_TRUE(ctx, pribuf0[1] == 0);
    TEST_ASSERT_TRUE(ctx, pribuf1[0] == 0);
    TEST_ASSERT_TRUE(ctx, pribuf2[0] == &actwk[1]);
    TEST_ASSERT_TRUE(ctx, pribuf2[1] == &actwk[5]);
    TEST_ASSERT_TRUE(ctx, pribuf2[2] == 0);
    TEST_ASSERT_TRUE(ctx, pribuf3[0] == 0);
}

static void test_spr_set_advances_patterns_and_applies_sprite_flags(
    test_context *ctx) {
    reset_fixture();
    actwk[0].ACT_NO = 1;
    actwk[0].XPOSI.w.h = 100;
    actwk[0].YPOSI.w.h = 50;
    actwk[0].SPR_TIMER = 1;
    actwk[0].PAT_NO = 0;
    actwk[0].PAT_ADR = &patterns;
    pribuf0[0] = &actwk[0];

    actwk[1].ACT_NO = 1;
    actwk[1].XPOSI.w.h = 200;
    actwk[1].YPOSI.w.h = 90;
    actwk[1].SPR_TIMER = 0;
    actwk[1].PAT_NO = 1;
    actwk[1].SPR_FLG = 128 | 64 | 32;
    actwk[1].PAT_ADR = &patterns;
    pribuf1[0] = &actwk[1];

    actwk[2].ACT_NO = 0;
    pribuf2[0] = &actwk[2];

    spr_set();

    TEST_ASSERT_EQ_INT(ctx, 3, easprset_call_count);
    TEST_ASSERT_EQ_INT(ctx, 227, easprset_x[0]);
    TEST_ASSERT_EQ_INT(ctx, 175, easprset_y[0]);
    TEST_ASSERT_EQ_INT(ctx, 30, easprset_index[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, easprset_link[0]);
    TEST_ASSERT_EQ_INT(ctx, 1, easprset_rev[0]);
    TEST_ASSERT_EQ_INT(ctx, 323, easprset_x[1]);
    TEST_ASSERT_EQ_INT(ctx, 211, easprset_y[1]);
    TEST_ASSERT_EQ_INT(ctx, 10, easprset_index[1]);
    TEST_ASSERT_EQ_INT(ctx, 1, easprset_link[1]);
    TEST_ASSERT_EQ_INT(ctx, 32775, easprset_rev[1]);
    TEST_ASSERT_EQ_INT(ctx, 337, easprset_x[2]);
    TEST_ASSERT_EQ_INT(ctx, 229, easprset_y[2]);
    TEST_ASSERT_EQ_INT(ctx, 20, easprset_index[2]);
    TEST_ASSERT_EQ_INT(ctx, 2, easprset_link[2]);
    TEST_ASSERT_EQ_INT(ctx, 32779, easprset_rev[2]);
    TEST_ASSERT_EQ_INT(ctx, 1, actwk[0].PAT_NO);
    TEST_ASSERT_EQ_INT(ctx, 5, actwk[0].SPR_TIMER);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[1].PAT_NO);
    TEST_ASSERT_EQ_INT(ctx, 2, actwk[1].SPR_TIMER);
}

static void test_action_dispatches_actors_clears_erased_and_blanks_tail_links(
    test_context *ctx) {
    reset_fixture();
    actwk[0].ACT_NO = 1;
    actwk[1].ACT_NO = 2;
    actwk[2].ACT_NO = 3;
    actwk[4].ACT_NO = 12;

    action();

    TEST_ASSERT_EQ_INT(ctx, 1, sonic_call_count);
    TEST_ASSERT_EQ_INT(ctx, 1, emie_call_count);
    TEST_ASSERT_EQ_INT(ctx, 1, metals_call_count);
    TEST_ASSERT_EQ_INT(ctx, 1, memset_call_count);
    TEST_ASSERT_TRUE(ctx, memset_ptrs[0] == &actwk[1]);
    TEST_ASSERT_EQ_INT(ctx, sizeof(actwk[1]), memset_sizes[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, actwk[4].ACT_NO);
    TEST_ASSERT_EQ_INT(ctx, 80, easprset_call_count);
    TEST_ASSERT_EQ_INT(ctx, 0, easprset_x[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, easprset_y[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, easprset_index[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, easprset_link[0]);
    TEST_ASSERT_EQ_INT(ctx, 0, easprset_rev[0]);
    TEST_ASSERT_EQ_INT(ctx, 79, easprset_link[79]);
}

static void test_spr_set_stops_inner_mapping_loop_at_link_limit(
    test_context *ctx) {
    reset_fixture();
    for (Sint32 i = 0; i < 40; ++i) {
        actwk[i].ACT_NO = 1;
        actwk[i].XPOSI.w.h = (Sint16)i;
        actwk[i].YPOSI.w.h = (Sint16)i;
        actwk[i].SPR_TIMER = 99;
        actwk[i].PAT_NO = 0;
        actwk[i].PAT_ADR = &patterns;
        pribuf0[i] = &actwk[i];
    }

    spr_set();

    TEST_ASSERT_EQ_INT(ctx, 80, easprset_call_count);
    TEST_ASSERT_EQ_INT(ctx, 79, easprset_link[79]);
}

static void test_clear_helpers_use_memset_for_requested_work(test_context *ctx) {
    reset_fixture();
    actwk[5].ACT_NO = 77;

    clrwork(&actwk[5]);

    TEST_ASSERT_EQ_INT(ctx, 1, memset_call_count);
    TEST_ASSERT_TRUE(ctx, memset_ptrs[0] == &actwk[5]);
    TEST_ASSERT_EQ_INT(ctx, sizeof(actwk[5]), memset_sizes[0]);

    reset_fixture();
    action_all_clr();

    TEST_ASSERT_EQ_INT(ctx, 64, memset_call_count);
    TEST_ASSERT_TRUE(ctx, memset_ptrs[0] == &actwk[0]);
    TEST_ASSERT_TRUE(ctx, memset_ptrs[63] == &actwk[63]);
    TEST_ASSERT_EQ_INT(ctx, sizeof(actwk[0]), memset_sizes[63]);
}

TEST_MAIN_BEGIN;
test_get_actwk_finds_first_free_slot_after_reserved_actors(&ctx);
test_chk_actwk_scans_work_slots(&ctx);
test_priset_groups_active_actors_by_priority(&ctx);
test_spr_set_advances_patterns_and_applies_sprite_flags(&ctx);
test_action_dispatches_actors_clears_erased_and_blanks_tail_links(&ctx);
test_spr_set_stops_inner_mapping_loop_at_link_limit(&ctx);
test_clear_helpers_use_memset_for_requested_work(&ctx);
TEST_MAIN_END
