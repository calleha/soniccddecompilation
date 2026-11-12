#include "../types.h"
#include "common.h"
#include "sps_equ.h"
#include "ens.h"
#include "act_s.h"
#include "coli_s.h"
#include "etc_s.h"
#include "game.h"
#include "pls_work.h"
#include "kaiten.h"

typedef struct {
    union {
        Uint16 timer;
        struct {
            Uint8 timer_low;
            Uint8 subtype;
        };
    };
    union {
        Uint16 short_timer;
        struct {
            Uint8 pattern;
            Uint8 direction;
        };
    };
    Uint16 linked_actor_index;
    Uint16 angle_z;
    Sint16 *movement_table_start;
    Sint16 *movement_table_cursor;
    Uint16 movement_count;
    Uint8 ufo_type;
    Uint8 ufo_direction;
    Uint8 flash_timer;
} special_enemy_work;

static special_enemy_work *special_enemy_get_work(sprite_status_sp *actionwk) {
    return (special_enemy_work *)actionwk->actfree;
}

static sprite_pattern seexp00 = {1, {{-10, -8, 0, 127}}};
static sprite_pattern seexp01 = {1, {{-20, -16, 0, 128}}};
static sprite_pattern seexp02 = {1, {{-20, -16, 0, 129}}};
static sprite_pattern seexp03 = {1, {{-25, -8, 0, 130}}};
static sprite_pattern seexp04 = {1, {{-25, -24, 0, 131}}};
static sprite_pattern stitle00 = {1, {{-90, -56, 0, 132}}};
static sprite_pattern spress00 = {1, {{0, 0, 0, 1}}};
static sprite_pattern sobi00 = {1, {{-15, -112, 0, 133}}};
static sprite_pattern sitem00 = {1, {{-10, -16, 0, 119}}};
static sprite_pattern sitem01 = {1, {{-10, -16, 0, 120}}};
static sprite_pattern sitem02 = {1, {{-10, -16, 0, 121}}};
static sprite_pattern sitem03 = {1, {{-10, -16, 0, 122}}};
static sprite_pattern sitem04 = {1, {{-10, -24, 0, 123}}};
static sprite_pattern sitem05 = {1, {{-10, -24, 0, 124}}};
static sprite_pattern sitem06 = {1, {{-5, -24, 0, 125}}};
static sprite_pattern sitem07 = {1, {{-10, -24, 0, 126}}};
static sprite_pattern skage00 = {1, {{-40, -8, 0, 71}}};
static sprite_pattern skage01 = {1, {{-35, -8, 0, 72}}};
static sprite_pattern skage02 = {1, {{-30, -4, 0, 73}}};
static sprite_pattern skage03 = {1, {{-30, -4, 0, 74}}};
static sprite_pattern skage04 = {1, {{-25, -4, 0, 75}}};
static sprite_pattern skage05 = {1, {{-20, -4, 0, 76}}};
static sprite_pattern skage06 = {1, {{-15, -4, 0, 77}}};
static sprite_pattern skage07 = {1, {{-15, -4, 0, 78}}};
static sprite_pattern skage08 = {1, {{-10, -4, 0, 79}}};
static sprite_pattern skage09 = {1, {{-5, -4, 0, 80}}};
static sprite_pattern Tufo00 = {1, {{-40, -56, 0, 81}}};
static sprite_pattern Tufo01 = {1, {{-30, -48, 0, 82}}};
static sprite_pattern Tufo02 = {1, {{-30, -40, 0, 83}}};
static sprite_pattern Tufo03 = {1, {{-30, -40, 0, 84}}};
static sprite_pattern Tufo04 = {1, {{-30, -32, 0, 85}}};
static sprite_pattern Tufo05 = {1, {{-20, -32, 0, 86}}};
static sprite_pattern Tufo06 = {1, {{-20, -32, 0, 87}}};
static sprite_pattern Tufo07 = {1, {{-15, -24, 0, 88}}};
static sprite_pattern Tufo08 = {1, {{-10, -16, 0, 89}}};
static sprite_pattern Tufo09 = {1, {{-5, -8, 0, 90}}};
static sprite_pattern sufo00 = {1, {{-40, -56, 0, 91}}};
static sprite_pattern sufo01 = {1, {{-30, -48, 0, 92}}};
static sprite_pattern sufo02 = {1, {{-30, -40, 0, 93}}};
static sprite_pattern sufo03 = {1, {{-30, -40, 0, 94}}};
static sprite_pattern sufo04 = {1, {{-30, -32, 0, 95}}};
static sprite_pattern sufo05 = {1, {{-20, -32, 0, 96}}};
static sprite_pattern sufo06 = {1, {{-20, -32, 0, 97}}};
static sprite_pattern sufo07 = {1, {{-15, -24, 0, 98}}};
static sprite_pattern sufo08 = {1, {{-10, -16, 0, 99}}};
static sprite_pattern sufo09 = {1, {{-5, -8, 0, 100}}};
static sprite_pattern sufo10 = {1, {{-40, -8, 0, 101}}};
static sprite_pattern sufo11 = {1, {{-30, -48, 0, 102}}};
static sprite_pattern sufo12 = {1, {{-30, -40, 0, 103}}};
static sprite_pattern sufo13 = {1, {{-30, -40, 0, 104}}};
static sprite_pattern sufo14 = {1, {{-30, -32, 0, 105}}};
static sprite_pattern sufo15 = {1, {{-20, -32, 0, 106}}};
static sprite_pattern sufo16 = {1, {{-20, -32, 0, 107}}};
static sprite_pattern sufo17 = {1, {{-15, -24, 0, 108}}};
static sprite_pattern sufo18 = {1, {{-10, -16, 0, 109}}};
static sprite_pattern sufo19 = {1, {{-5, -8, 0, 110}}};
sprite_patterns_sp mpeexpTBL0 = {
    5, 2, {&seexp00, &seexp01, &seexp02, &seexp03, &seexp04}};
sprite_patterns_sp *mpeexp[1] = {&mpeexpTBL0};
sprite_patterns_sp mpitemTBL0 = {1, -1, {&sitem00}},
                   mpitemTBL1 = {1, -1, {&sitem01}},
                   mpitemTBL2 = {1, -1, {&sitem02}},
                   mpitemTBL3 = {1, -1, {&sitem03}},
                   mpitemTBL4 = {
                       4, 4, {&sitem04, &sitem05, &sitem06, &sitem07}};
sprite_patterns_sp *mpitem[5] = {&mpitemTBL0, &mpitemTBL1, &mpitemTBL2,
                                 &mpitemTBL3, &mpitemTBL4};
sprite_patterns_sp mptlogoTBL0 = {1, -1, {&stitle00}};
sprite_patterns_sp *mptlogo[1] = {&mptlogoTBL0};
sprite_patterns_sp mptobiTBL0 = {1, -1, {&sobi00}};
sprite_patterns_sp *mptobi[1] = {&mptobiTBL0};
sprite_patterns_sp mppressTBL0 = {1, -1, {&spress00}};
sprite_patterns_sp *mppress[1] = {&mppressTBL0};
sprite_patterns_sp mpkageTBL0 = {1, -1, {&skage00}};
sprite_patterns_sp mpkageTBL1 = {1, -1, {&skage01}};
sprite_patterns_sp mpkageTBL2 = {1, -1, {&skage02}};
sprite_patterns_sp mpkageTBL3 = {1, -1, {&skage03}};
sprite_patterns_sp mpkageTBL4 = {1, -1, {&skage04}};
sprite_patterns_sp mpkageTBL5 = {1, -1, {&skage05}};
sprite_patterns_sp mpkageTBL6 = {1, -1, {&skage06}};
sprite_patterns_sp mpkageTBL7 = {1, -1, {&skage07}};
sprite_patterns_sp mpkageTBL8 = {1, -1, {&skage08}};
sprite_patterns_sp mpkageTBL9 = {1, -1, {&skage09}};
sprite_patterns_sp mpufoxTBL0 = {1, -1, {&sufo00}};
sprite_patterns_sp mpufoxTBL1 = {1, -1, {&sufo01}};
sprite_patterns_sp mpufoxTBL2 = {1, -1, {&sufo02}};
sprite_patterns_sp mpufoxTBL3 = {1, -1, {&sufo03}};
sprite_patterns_sp mpufoxTBL4 = {1, -1, {&sufo04}};
sprite_patterns_sp mpufoxTBL5 = {1, -1, {&sufo05}};
sprite_patterns_sp mpufoxTBL6 = {1, -1, {&sufo06}};
sprite_patterns_sp mpufoxTBL7 = {1, -1, {&sufo07}};
sprite_patterns_sp mpufoxTBL8 = {1, -1, {&sufo08}};
sprite_patterns_sp mpufoxTBL9 = {1, -1, {&sufo09}};
sprite_patterns_sp tpufoxTBL0 = {1, -1, {&Tufo00}},
                   tpufoxTBL1 = {1, -1, {&Tufo01}},
                   tpufoxTBL2 = {1, -1, {&Tufo02}},
                   tpufoxTBL3 = {1, -1, {&Tufo03}},
                   tpufoxTBL4 = {1, -1, {&Tufo04}},
                   tpufoxTBL5 = {1, -1, {&Tufo05}},
                   tpufoxTBL6 = {1, -1, {&Tufo06}},
                   tpufoxTBL7 = {1, -1, {&Tufo07}},
                   tpufoxTBL8 = {1, -1, {&Tufo08}},
                   tpufoxTBL9 = {1, -1, {&Tufo09}};
sprite_patterns_sp *tpufox[10] = {
    &tpufoxTBL0, &tpufoxTBL1, &tpufoxTBL2, &tpufoxTBL3, &tpufoxTBL4,
    &tpufoxTBL5, &tpufoxTBL6, &tpufoxTBL7, &tpufoxTBL8, &tpufoxTBL9};
sprite_patterns_sp mpufoyTBL0 = {1, -1, {&sufo10}};
sprite_patterns_sp mpufoyTBL1 = {1, -1, {&sufo11}};
sprite_patterns_sp mpufoyTBL2 = {1, -1, {&sufo12}};
sprite_patterns_sp mpufoyTBL3 = {1, -1, {&sufo13}};
sprite_patterns_sp mpufoyTBL4 = {1, -1, {&sufo14}};
sprite_patterns_sp mpufoyTBL5 = {1, -1, {&sufo15}};
sprite_patterns_sp mpufoyTBL6 = {1, -1, {&sufo16}};
sprite_patterns_sp mpufoyTBL7 = {1, -1, {&sufo17}};
sprite_patterns_sp mpufoyTBL8 = {1, -1, {&sufo18}};
sprite_patterns_sp mpufoyTBL9 = {1, -1, {&sufo19}};
Sint16 mvtbl0_0[13] = {0,  0,    144,  1920, 2880, 2304, 2560,
                       96, 2304, 2560, 1920, 2880, -1},
       mvtbl0_1[18] = {0,    0,    144,  1536, 2176, 1920, 1664, 96,   1920,
                       1664, 1408, 1920, 96,   1408, 1920, 1536, 2176, -1},
       mvtbl0_2[18] = {1,    0,    144,  2688, 1408, 2304, 1280, 96,   2304,
                       1280, 2432, 1664, 240,  2432, 1664, 2688, 1408, -1},
       mvtbl0_3[13] = {0,  0,    240,  1920, 1152, 1408, 1280,
                       96, 1408, 1280, 1920, 1152, -1},
       mvtbl0_4[23] = {0,    0,    48,   2816, 2560, 2816, 2432, 144,
                       2816, 2432, 2560, 2560, 96,   2560, 2560, 2688,
                       2816, 96,   2688, 2816, 2816, 2560, -1},
       mvtbl0_5[18] = {1,    0,    144,  2432, 2176, 2304, 2432, 96,   2304,
                       2432, 2816, 2176, 240,  2816, 2176, 2432, 2176, -1},
       mvtbl1_0[23] = {0,    0,    240,  2816, 1408, 2240, 1216, 240,
                       2240, 1216, 2048, 1536, 240,  2048, 1536, 2560,
                       1728, 240,  2560, 1728, 2816, 1408, -1};
Sint16 mvtbl1_1[18] = {0,    0,    144,  1408, 1280, 1280, 1408, 240,  1280,
                       1408, 1664, 1664, 240,  1664, 1664, 1408, 1280, -1},
       mvtbl1_2[23] = {1,    0,    240,  1664, 1792, 1408, 1792, 300,
                       1408, 1792, 1216, 2048, 300,  1216, 2048, 1664,
                       2048, 360,  1664, 2048, 1664, 1792, -1},
       mvtbl1_3[13] = {0,  0,    96,   1536, 2432, 1792, 2560,
                       48, 1792, 2560, 1536, 2432, -1},
       mvtbl1_4[18] = {0,    0,    300,  2560, 2304, 2112, 2752, 300,  2112,
                       2752, 2560, 2816, 240,  2560, 2816, 2560, 2304, -1},
       mvtbl1_5[18] = {1,    0,    240,  2880, 2048, 2560, 1920, 96,   2560,
                       1920, 2432, 2240, 240,  2432, 2240, 2880, 2048, -1};
Sint16 mvtbl2_0[23] = {0,    0,    64,   2816, 1280, 2560, 1280, 200,
                       2560, 1280, 2304, 1792, 96,   2304, 1792, 2560,
                       1664, 160,  2560, 1664, 2816, 1280, -1},
       mvtbl2_1[18] = {0,    0,    200,  1728, 1216, 1280, 1536, 160,  1280,
                       1536, 1600, 1408, 160,  1600, 1408, 1728, 1216, -1},
       mvtbl2_2[23] = {1,    0,    96,   1536, 1920, 1280, 1920, 200,
                       1280, 1920, 1280, 2176, 160,  1280, 2176, 1536,
                       2176, 160,  1536, 2176, 1536, 1920, -1},
       mvtbl2_3[18] = {0,    0,    96,   1536, 2432, 1280, 2432, 200,  1280,
                       2432, 1280, 2816, 96,   1280, 2816, 1536, 2432, -1},
       mvtbl2_4[23] = {0,    0,    200,  2240, 2560, 1792, 2496, 200,
                       1792, 2496, 1792, 2816, 160,  1792, 2816, 2240,
                       2816, 160,  2240, 2816, 2240, 2560, -1},
       mvtbl2_5[23] = {1,    0,    200,  2816, 2432, 2560, 2176, 200,
                       2560, 2176, 2432, 2560, 96,   2432, 2560, 2432,
                       2816, 240,  2432, 2816, 2816, 2432, -1};
Sint16 mvtbl3_0[18] = {0,    0,    160,  2752, 1216, 2176, 1280, 200,  2176,
                       1280, 2816, 1536, 160,  2816, 1536, 2752, 1216, -1},
       mvtbl3_1[23] = {0,    0,    200,  2208, 1472, 1856, 1472, 200,
                       1856, 1472, 1856, 1792, 200,  1856, 1792, 2208,
                       1792, 200,  2208, 1792, 2208, 1472, -1},
       mvtbl3_2[23] = {1,    0,    200,  1536, 1792, 1280, 2048, 200,
                       1280, 2048, 1536, 2304, 96,   1536, 2304, 1664,
                       2048, 96,   1664, 2048, 1536, 1792, -1},
       mvtbl3_3[13] = {0,   0,    200,  1536, 2688, 1088, 2688,
                       160, 1088, 2688, 1536, 2688, -1},
       mvtbl3_4[18] = {0,    0,    96,   2432, 2304, 2176, 2304, 160,  2176,
                       2304, 1856, 2688, 200,  1856, 2688, 2432, 2304, -1},
       mvtbl3_5[18] = {1,    0,    160,  2688, 1728, 2432, 1792, 160,  2432,
                       1792, 2816, 1920, 96,   2816, 1920, 2688, 1728, -1};
Sint16 mvtbl4_0[23] = {0,    0,    16,   2944, 1088, 2816, 1088, 80,
                       2816, 1088, 2560, 1408, 80,   2560, 1408, 3008,
                       1408, 48,   3008, 1408, 2944, 1088, -1},
       mvtbl4_1[23] = {0,    0,    48,   1792, 1088, 1472, 1088, 100,
                       1472, 1088, 1920, 1600, 48,   1920, 1600, 2112,
                       1408, 100,  2112, 1408, 1792, 1088, -1},
       mvtbl4_2[13] = {1,   0,    100,  2752, 1792, 2112, 1792,
                       100, 2112, 1792, 2752, 1792, -1},
       mvtbl4_3[28] = {0,    0,    48, 1472, 1920, 1344, 2176, 48, 1344, 2176,
                       1280, 2368, 48, 1280, 2368, 1472, 2432, 80, 1472, 2432,
                       1536, 2112, 48, 1536, 2112, 1472, 1920, -1},
       mvtbl4_4[23] = {0,    0,    32,   1856, 2176, 1696, 2304, 100,
                       1696, 2304, 1696, 3008, 48,   1696, 3008, 1856,
                       3008, 100,  1856, 3008, 1856, 2176, -1},
       mvtbl4_5[18] = {1,    0,    80,   2432, 2112, 2112, 2432, 100,  2112,
                       2432, 2752, 2432, 80,   2752, 2432, 2432, 2112, -1};
Sint16 mvtbl5_0[23] = {0,    0,    80,   2816, 1280, 2816, 1664, 80,
                       2816, 1664, 2688, 1984, 48,   2688, 1984, 3072,
                       1984, 120,  3072, 1984, 2816, 1280, -1},
       mvtbl5_1[18] = {0,    0,    100,  2432, 1152, 1984, 1408, 100,  1984,
                       1408, 2560, 1408, 48,   2560, 1408, 2432, 1152, -1},
       mvtbl5_2[23] = {1,    0,    48,   1216, 1152, 960,  1152, 100,
                       960,  1152, 960,  1536, 48,   960,  1536, 1216,
                       1536, 100,  1216, 1536, 1216, 1152, -1},
       mvtbl5_3[18] = {0,    0,    100,  1408, 2496, 1024, 2944, 100,  1024,
                       2944, 1664, 2688, 48,   1664, 2688, 1408, 2496, -1},
       mvtbl5_4[13] = {0,  0,    120,  2560, 2368, 1536, 2368,
                       80, 1536, 2368, 2560, 2368, -1},
       mvtbl5_5[18] = {1,    0,    100,  3072, 2176, 2688, 2176, 120,  2688,
                       2176, 3072, 2944, 80,   3072, 2944, 3072, 2176, -1};
Sint16 mvtbl6_0[33] = {0,    0,    32,   2560, 1792, 2816, 1536, 32,   2816,
                       1536, 2560, 1792, 48,   2560, 1792, 2688, 1536, 48,
                       2688, 1536, 2560, 1792, 80,   2560, 1792, 2560, 1536,
                       48,   2560, 1536, 2560, 1792, -1},
       mvtbl6_1[23] = {0,    0,    32,   1280, 1792, 1024, 1792, 80,
                       1024, 1792, 1280, 2368, 32,   1280, 2368, 1024,
                       2368, 80,   1024, 2368, 1280, 1792, -1},
       mvtbl6_2[23] = {1,    0,    48,   1216, 2944, 1408, 2816, 48,
                       1408, 2816, 1216, 2944, 48,   1216, 2944, 1024,
                       2816, 48,   1024, 2816, 1216, 2944, -1},
       mvtbl6_3[23] = {0,    0,    80,   1664, 2560, 1664, 2816, 80,
                       1664, 2816, 1664, 2560, 120,  1664, 2560, 2176,
                       2944, 120,  2176, 2944, 1664, 2560, -1},
       mvtbl6_4[18] = {0,    0,    80,   2688, 2432, 2304, 2816, 100,  2304,
                       2816, 3008, 3008, 100,  3008, 3008, 2688, 2432, -1},
       mvtbl6_5[18] = {1,    0,    80,   3072, 1664, 2816, 2048, 32,   2816,
                       2048, 3072, 2048, 48,   3072, 2048, 3072, 1664, -1};
Sint16 mvtbl7_0[18] = {0,    0,    120,  2816, 2944, 3072, 2432, 48,   3072,
                       2432, 3072, 2688, 80,   3072, 2688, 2816, 2944, -1},
       mvtbl7_1[18] = {1,    0,    120,  3072, 1792, 2560, 1280, 80,   2560,
                       1280, 2944, 1088, 120,  2944, 1088, 3072, 1792, -1},
       mvtbl7_2[53] = {0,    0,    120,  2432, 2368, 2688, 2176, 120,  2688,
                       2176, 2432, 2048, 120,  2432, 2048, 2688, 1920, 48,
                       2688, 1920, 2560, 1792, 80,   2560, 1792, 2560, 1536,
                       80,   2560, 1536, 2560, 1792, 48,   2560, 1792, 2688,
                       1920, 120,  2688, 1920, 2432, 2048, 120,  2432, 2048,
                       2688, 2176, 120,  2688, 2176, 2432, 2368, -1},
       mvtbl7_3[23] = {0,    0,    80,   2048, 1792, 1664, 1920, 120,
                       1664, 1920, 1792, 1280, 80,   1792, 1280, 1408,
                       1408, 120,  1408, 1408, 2048, 1792, -1},
       mvtbl7_4[23] = {1,    0,    80,   1664, 2944, 1024, 2688, 48,
                       1024, 2688, 1024, 2176, 48,   1024, 2176, 1536,
                       2304, 80,   1536, 2304, 1664, 2944, -1},
       mvtbl7_5[13] = {0,  0,    80,   1536, 1152, 1024, 1536,
                       80, 1024, 1536, 1536, 1152, -1};
extern sprite_patterns_sp *mpufoy[10];
extern sprite_patterns_sp *mpufox[10];
extern sprite_patterns_sp *mpkage[10];

void bara_ring(sprite_status_sp *actionwk) {
    switch (actionwk->exeno) {
    case 0:
        b_ring00(actionwk);
        break;
    case 1:
        b_ring01(actionwk);
    }

    patset(actionwk);
}

void b_ring00(sprite_status_sp *actionwk) {
    Sint32 d0, d1;

    actionwk->sprcolor = 59279;
    actionwk->pattbl = mpitem;
    special_enemy_get_work(actionwk)->pattern = 4;
    patinit(actionwk, special_enemy_get_work(actionwk)->pattern);
    actionwk->sx_posi.w.h = actwk[0].sx_posi.w.h;
    actionwk->sy_posi.w.h = actwk[0].sy_posi.w.h;
    ++actionwk->exeno;
    special_enemy_get_work(actionwk)->timer_low = 45;
    d0 = d1 = random();
    d1 &= 258048;
    if (pmflag & 1)
        d1 = -d1;

    pmflag ^= 1;
    actionwk->sx_speed.l = d1;
    d0 &= 15;
    actionwk->sy_speed.w.h = -(d0 + 10);
}

void b_ring01(sprite_status_sp *actionwk) {
    if (--special_enemy_get_work(actionwk)->timer_low == 0)
        actionwk->actflg |= 1;

    actionwk->sx_posi.l += actionwk->sx_speed.l;
    actionwk->sy_posi.l += actionwk->sy_speed.l;
    if (actionwk->sy_posi.w.h < 345) {
        actionwk->sy_speed.l += 0x20000;
    } else {
        actionwk->sy_posi.w.h = 344;
        actionwk->sy_speed.l = ~actionwk->sy_speed.l;
    }
}

void item_ring(sprite_status_sp *actionwk) {
    switch (actionwk->exeno) {
    case 0:
        i_ring00(actionwk);
        break;
    case 1:
        i_ring01(actionwk);
        break;
    }
    patset(actionwk);
}

void i_ring00(sprite_status_sp *actionwk) {
    actionwk->sprcolor = 34703;
    actionwk->pattbl = mpitem;
    special_enemy_get_work(actionwk)->pattern =
        special_enemy_get_work(actionwk)->subtype;
    patinit(actionwk, special_enemy_get_work(actionwk)->pattern);
    ++actionwk->exeno;
    special_enemy_get_work(actionwk)->timer_low = 16;
    actionwk->sy_speed.w.h = -16;
    key_set(149);
}

void i_ring01(sprite_status_sp *actionwk) {
    if (--special_enemy_get_work(actionwk)->timer_low == 0)
        actionwk->actflg |= 1;

    actionwk->sy_posi.l += actionwk->sy_speed.l;
    actionwk->sy_speed.l += 0x20000;
}

void timeufo(sprite_status_sp *actionwk) {
    switch (actionwk->exeno) {
    case 0:
        tufo00(actionwk);
    case 1:
        tufo01(actionwk);
        break;
    case 2:
        tufo02(actionwk);
        break;
    }
    actionwk->z_posi.w.h = actwk[0].z_posi.w.h;
    actionwk->z_posi.w.h -= 320;
    if (special_enemy_get_work(actionwk)->flash_timer) {
        --special_enemy_get_work(actionwk)->flash_timer;
        actionwk->actflg |= 4;
    }
}

void tufo00(sprite_status_sp *actionwk) {
    actionwk->sprcolor = 33856;
    mvtblset(actionwk);

    actionwk->pattbl = tpufox;
    actionwk->z_posi.w.h = actwk[0].z_posi.w.h;
    actionwk->z_posi.w.h -= 320;
    ++actionwk->exeno;
    special_enemy_get_work(actionwk)->flash_timer = 2;
    key_set(188);
}

void tufo01(sprite_status_sp *actionwk) {
    Uint16 *mcnt;
    sprite_status_sp *a1;
    Uint8 d0;

    actionwk->x_posi.l += actionwk->x_speed.l;
    actionwk->y_posi.l += actionwk->y_speed.l;
    mcnt = &special_enemy_get_work(actionwk)->movement_count;
    if (--*mcnt == 0) {
        mvtblset(actionwk);
    }

    ptset_ufo(actionwk);
    scal(actionwk);
    ufovspl(actionwk);
    if (!actionwk->colliflg)
        return;
    if (time_stop)
        return;
    actionwk->exeno = 2;
    a1 = &actwk[special_enemy_get_work(actionwk)->linked_actor_index];
    a1->actflg |= 1;
    special_enemy_get_work(actionwk)->timer = 60;
    d0 = random() & 1;
    special_enemy_get_work(actionwk)->direction = d0;
    patinit(actionwk, 0);
    spe_time.l += 30;
    a1 = &actwk[16];
    a1->actno = 4;
    special_enemy_get_work(a1)->subtype = 3;
    a1->sx_posi.w.h = actionwk->sx_posi.w.h;
    a1->sy_posi.w.h = actionwk->sy_posi.w.h;
}

void tufo02(sprite_status_sp *actionwk) {
    Uint16 *tim, d0;
    sprite_status_sp *a1;

    actionwk->sx_posi.w.h -= 4;
    if (special_enemy_get_work(actionwk)->direction == 0)
        actionwk->sx_posi.w.h += 8;

    ++actionwk->sy_posi.w.h;
    actionwk->actflg &= 251;
    tim = &special_enemy_get_work(actionwk)->timer;
    if (--*tim == 0)
        actionwk->actflg |= 1;

    if (special_enemy_get_work(actionwk)->timer_low & 1) {
        if (exp_set(&a1) != 0)
            return;
        a1->actno = 12;
        a1->sx_posi.w.h = actionwk->sx_posi.w.h - 16;
        a1->sy_posi.w.h = actionwk->sy_posi.w.h;
        d0 = (Uint8)random();
        a1->sx_posi.w.h += d0 & 31;

        a1->sy_posi.w.h -= d0 & 31;
    }
}

void ufo0(sprite_status_sp *actionwk) {

    switch (actionwk->exeno) {

    case 0:
        ufo00(actionwk);

    case 1:
        ufo01(actionwk);
        break;

    case 2:
        ufo02(actionwk);
        break;
    }
    actionwk->z_posi.w.h = actwk[0].z_posi.w.h;
    actionwk->z_posi.w.h -= 320;
    if (special_enemy_get_work(actionwk)->flash_timer) {
        --special_enemy_get_work(actionwk)->flash_timer;
        actionwk->actflg |= 4;
    }
}

void ufo00(sprite_status_sp *actionwk) {
    actionwk->sprcolor = 58432;

    mvtblset(actionwk);

    actionwk->pattbl = mpufox;
    if (special_enemy_get_work(actionwk)->ufo_type != 0)
        actionwk->pattbl = mpufoy;

    actionwk->z_posi.w.h = actwk[0].z_posi.w.h;
    actionwk->z_posi.w.h -= 320;
    special_enemy_get_work(actionwk)->pattern = 0;
    patinit(actionwk, special_enemy_get_work(actionwk)->pattern);

    special_enemy_get_work(actionwk)->flash_timer = 2;
    ++actionwk->exeno;
}

void ufo01(sprite_status_sp *actionwk) {
    Uint16 *mcnt;
    sprite_status_sp *a1;

    actionwk->x_posi.l += actionwk->x_speed.l;
    actionwk->y_posi.l += actionwk->y_speed.l;
    mcnt = &special_enemy_get_work(actionwk)->movement_count;
    if (--*mcnt == 0) {
        mvtblset(actionwk);
    }

    ptset_ufo(actionwk);
    scal(actionwk);
    ufovspl(actionwk);
    if (!actionwk->colliflg)
        return;
    if (ufoleft < 2)
        time_stop = 1;

    ufo_dec();
    actionwk->exeno = 2;
    a1 = &actwk[special_enemy_get_work(actionwk)->linked_actor_index];
    a1->actflg |= 1;
    special_enemy_get_work(actionwk)->timer = 60;
    special_enemy_get_work(actionwk)->direction = random() & 1;
    patinit(actionwk, 0);

    a1 = &actwk[16];
    a1->actno = 4;
    a1->sx_posi.w.h = actionwk->sx_posi.w.h;
    a1->sy_posi.w.h = actionwk->sy_posi.w.h;
    special_enemy_get_work(a1)->subtype =
        special_enemy_get_work(actionwk)->ufo_type;

    switch (special_enemy_get_work(actionwk)->ufo_type) {
    case 0:
    case 2:
    case 3:
        rufo_getnm *= 2;
        ring_add(rufo_getnm >> 1);
        break;
    case 1:
        /* The reference writes ((short *)&actwk[0])[45] - the player's
           speed_boost_timer - which speed_ud() burns down as a speed boost. */
        special_player_get_work(&actwk[0])->speed_boost_timer = 200;
        rufo_getnm = 20;
    }
}

void ufo02(sprite_status_sp *actionwk) {
    Uint16 *tim, d0;
    sprite_status_sp *a1;

    actionwk->sx_posi.w.h -= 4;
    if (special_enemy_get_work(actionwk)->direction == 0)
        actionwk->sx_posi.w.h += 8;

    ++actionwk->sy_posi.w.h;
    actionwk->actflg &= 251;
    tim = &special_enemy_get_work(actionwk)->timer;
    if (--*tim == 0)
        actionwk->actflg |= 1;

    if (special_enemy_get_work(actionwk)->subtype & 1)
        return;

    if (exp_set(&a1) != 0)
        return;
    a1->actno = 12;
    a1->sx_posi.w.h = actionwk->sx_posi.w.h - 16;
    a1->sy_posi.w.h = actionwk->sy_posi.w.h;
    d0 = (Uint8)random();
    a1->sx_posi.w.h += d0 & 31;

    a1->sy_posi.w.h -= d0 & 31;
}

void mvtblset(sprite_status_sp *actionwk) {
    Sint16 *a1;
    Sint16 d0, d1, d2, d3;
    Sint16 mcnt;
    Sint32 d2l, d3l;

    do {
        a1 = special_enemy_get_work(actionwk)->movement_table_cursor;

        mcnt = special_enemy_get_work(actionwk)->movement_count = *a1++;
        if (mcnt >= 0)
            break;

        special_enemy_get_work(actionwk)->movement_table_cursor =
            special_enemy_get_work(actionwk)->movement_table_start;

    } while (1);

    d0 = *a1++;
    d1 = *a1++;
    actionwk->x_posi.w.h = d0;
    actionwk->y_posi.w.h = d1;
    d2 = *a1++;
    d3 = *a1++;
    d2 -= d0;
    d3 -= d1;
    d2l = d2;
    d3l = d3;
    d2l <<= 4;
    d3l <<= 4;
    d2l /= mcnt;
    d3l /= mcnt;
    d2l <<= 12;
    d3l <<= 12;
    actionwk->x_speed.l = d2l;
    actionwk->y_speed.l = d3l;
    special_enemy_get_work(actionwk)->movement_table_cursor = a1;
}

void ufo_frout_chk(sprite_status_sp *actionwk) {
    Sint16 d0;

    actionwk->actflg &= 251;
    d0 = actionwk->sx_posi.w.h;
    if ((Uint16)d0 >= 512) {
        actionwk->actflg |= 4;
        return;
    }
    d0 = actionwk->sy_posi.w.h;
    if (d0 < 256) {
        actionwk->actflg |= 4;
        return;
    }
    if (d0 < 448)
        return;

    actionwk->actflg |= 4;
}

mvtbl mvtbl0 = {6,
                {mvtbl0_0, mvtbl0_1, mvtbl0_2, mvtbl0_3, mvtbl0_4, mvtbl0_5}};
mvtbl mvtbl1 = {6,
                {mvtbl1_0, mvtbl1_1, mvtbl1_2, mvtbl1_3, mvtbl1_4, mvtbl1_5}};
mvtbl mvtbl2 = {6,
                {mvtbl2_0, mvtbl2_1, mvtbl2_2, mvtbl2_3, mvtbl2_4, mvtbl2_5}};
mvtbl mvtbl3 = {6,
                {mvtbl3_0, mvtbl3_1, mvtbl3_2, mvtbl3_3, mvtbl3_4, mvtbl3_5}};
mvtbl mvtbl4 = {6,
                {mvtbl4_0, mvtbl4_1, mvtbl4_2, mvtbl4_3, mvtbl4_4, mvtbl4_5}};
mvtbl mvtbl5 = {6,
                {mvtbl5_0, mvtbl5_1, mvtbl5_2, mvtbl5_3, mvtbl5_4, mvtbl5_5}};
mvtbl mvtbl6 = {6,
                {mvtbl6_0, mvtbl6_1, mvtbl6_2, mvtbl6_3, mvtbl6_4, mvtbl6_5}};
mvtbl mvtbl7 = {6,
                {mvtbl7_0, mvtbl7_1, mvtbl7_2, mvtbl7_3, mvtbl7_4, mvtbl7_5}};

void ptset_ufo(sprite_status_sp *actionwk) {
    sprite_status_sp *a6;
    Uint16 d0, d1, d3, d4, d5, d6;
    Sint32 d0l;
    Uint8 d1b, d2b;
    Uint8 ufo_pat_tbl[81] = {0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5,
                             5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6,
                             6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7,
                             7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8,
                             8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9};

    a6 = &actwk[0];

    d4 = a6->x_posi.w.h;
    d5 = a6->y_posi.w.h;
    d0 = actionwk->x_posi.w.h;
    d1 = actionwk->y_posi.w.h;
    angleget(d0, d1, d4, d5, &d1b, &d2b);
    d5 = a6->x_posi.w.h;
    d6 = a6->y_posi.w.h;
    d3 = actionwk->x_posi.w.h;
    d4 = actionwk->y_posi.w.h;
    d0l = dstnsget(d1b, d2b, d3, d4, d5, d6);
    zbuf_set(actionwk, d0l);

    if ((Uint32)d0l >= 1280)
        d0l = 1280;
    d0l >>= 4;
    if (d0l > 80)
        d0l = 80;

    if (special_enemy_get_work(actionwk)->pattern == ufo_pat_tbl[d0l])
        return;
    special_enemy_get_work(actionwk)->pattern = ufo_pat_tbl[d0l];
    patinit1(actionwk, special_enemy_get_work(actionwk)->pattern);
}

mvtbl *mvtbl_tbl[8] = {&mvtbl0, &mvtbl1, &mvtbl2, &mvtbl3,
                       &mvtbl4, &mvtbl5, &mvtbl6, &mvtbl7};
Sint16 tufo_tbl[43] = {2,    0,    90,   2048, 2048, 1920, 2048, 90,   1920,
                       2048, 2048, 2048, 90,   2048, 2048, 2048, 1920, 90,
                       2048, 1920, 2048, 2048, 90,   2048, 2048, 2176, 2048,
                       90,   2176, 2048, 2048, 2048, 90,   2048, 2048, 2048,
                       2176, 90,   2048, 2176, 2048, 2048, -1};

void ufo_initial(void) {
    mvtbl *a1;
    sprite_status_sp *a2;
    char d7;
    Sint16 cnt;

    a2 = &actwk[32];
    a1 = mvtbl_tbl[stagenm];
    d7 = mvtbl_tbl[stagenm]->no;
    ufoleft = d7;

    cnt = 0;
    while (d7) {

        u_init(cnt++, a2++);
        --d7;
    }
}

void u_init(Sint16 cnt, sprite_status_sp *a2) {
    sprite_status_sp *a4;
    Uint16 dummy;
    Sint16 *a1tbl;

    a1tbl = mvtbl_tbl[stagenm]->mv_tbl[cnt];

    dummy = a2 - actwk;
    a4 = &actwk[dummy + 8];
    a2->actno = 2;
    special_enemy_get_work(a2)->ufo_type = (char)a1tbl[0];
    special_enemy_get_work(a2)->ufo_direction = (char)a1tbl[1];
    a1tbl += 2;
    special_enemy_get_work(a2)->movement_table_start = a1tbl;
    special_enemy_get_work(a2)->movement_table_cursor = a1tbl;
    special_enemy_get_work(a2)->linked_actor_index = dummy + 8;
    a4->actno = 5;
    special_enemy_get_work(a4)->linked_actor_index = a2 - actwk;
}

void tufo_initial(void) {
    sprite_status_sp *a2, *a4;
    Sint16 *a3, dummy;

    if (spe_time.l >= 21)
        return;
    a2 = &actwk[39];
    a3 = tufo_tbl;
    if (a2->actno)
        return;
    dummy = (Uint16)(a2 - actwk);
    a4 = &actwk[dummy + 8];

    a2->actno = 3;

    special_enemy_get_work(a2)->ufo_type = (Uint8)tufo_tbl[0];
    special_enemy_get_work(a2)->ufo_direction = (Uint8)tufo_tbl[1];
    a3 += 2;
    special_enemy_get_work(a2)->movement_table_start = a3;
    special_enemy_get_work(a2)->movement_table_cursor = a3;
    special_enemy_get_work(a2)->linked_actor_index = (Uint16)dummy + 8;
    a4->actno = 5;
    special_enemy_get_work(a4)->linked_actor_index = a2 - actwk;
}

void plkage(sprite_status_sp *actionwk) {
    switch (actionwk->exeno) {
    case 0:
        plkage00(actionwk);
    case 1:
        plkage01(actionwk);
    }

    patset(actionwk);
}

void plkage00(sprite_status_sp *actionwk) {
    actionwk->sprcolor = 59100;
    actionwk->pattbl = mpkage;
    special_enemy_get_work(actionwk)->pattern = 5;
    patinit(actionwk, special_enemy_get_work(actionwk)->pattern);
    ++actionwk->exeno;
}

void plkage01(sprite_status_sp *actionwk) {
    sprite_status_sp *a1;

    a1 = &actwk[0];
    actionwk->x_posi.w.h = a1->x_posi.w.h;
    actionwk->y_posi.w.h = a1->y_posi.w.h;
    actionwk->z_posi.w.h = actwk[0].z_posi.w.h;
    scal(actionwk);
}

void kage(sprite_status_sp *actionwk) {
    switch (actionwk->exeno) {
    case 0:
        kage00(actionwk);
    case 1:
        kage01(actionwk);
    }

    ufo_frout_chk(actionwk);
    patset(actionwk);
}

void kage00(sprite_status_sp *actionwk) {
    actionwk->sprcolor = 59100;
    actionwk->pattbl = mpkage;
    special_enemy_get_work(actionwk)->pattern = 0;
    patinit(actionwk, special_enemy_get_work(actionwk)->pattern);
    ++actionwk->exeno;
}

void kage01(sprite_status_sp *actionwk) {
    sprite_status_sp *a1;

    a1 = &actwk[special_enemy_get_work(actionwk)->linked_actor_index];
    actionwk->x_posi.w.h = a1->x_posi.w.h;
    actionwk->y_posi.w.h = a1->y_posi.w.h;
    a1->actflg |= 4;
    if ((actionwk->actflg & 4) == 0) {
        a1->actflg &= 251;
    }

    actionwk->z_posi.w.h = actwk[0].z_posi.w.h;
    ptset_ufo(actionwk);
    scal(actionwk);
}

void press(sprite_status_sp *actionwk) {
    switch (actionwk->exeno) {
    case 0:
        press00(actionwk);
    case 1:
        press01(actionwk);
    }

    patset(actionwk);
}

void press00(sprite_status_sp *actionwk) {
    actionwk->sprcolor = 34154;
    actionwk->pattbl = mppress;
    actionwk->sx_posi.w.h = 212;
    actionwk->sy_posi.w.h = 208;
    patinit(actionwk, 0);
    ++actionwk->exeno;
}

void press01(sprite_status_sp *actionwk) {
    ++special_enemy_get_work(actionwk)->timer_low;
    actionwk->actflg |= 4;
    if (special_enemy_get_work(actionwk)->timer_low & 16)
        return;
    actionwk->actflg &= 251;
}

void title_logo(sprite_status_sp *actionwk) {
    switch (actionwk->exeno) {
    case 0:
        tlogo00(actionwk);
        break;
    case 1:
        tlogo01(actionwk);
        break;
    case 2:
        tlogo02(actionwk);
        break;
    case 3:
        tlogo03(actionwk);
    }

    patset(actionwk);
}

void tlogo00(sprite_status_sp *actionwk) {
    actionwk->sprcolor = 34070;
    actionwk->pattbl = mptlogo;
    actionwk->sx_posi.w.h = 456;
    actionwk->sy_posi.w.h = 240;
    patinit(actionwk, 0);
    ++actionwk->exeno;
}

void tlogo01(sprite_status_sp *actionwk) {
    actionwk->sx_posi.w.h -= 32;
    if (actionwk->sx_posi.w.h >= 333)
        return;
    actionwk->sx_posi.w.h = 332;
    special_enemy_get_work(actionwk)->timer = 80;
    ++actionwk->exeno;
}

void tlogo02(sprite_status_sp *actionwk) {
    if (--special_enemy_get_work(actionwk)->timer)
        return;
    ++actionwk->exeno;
}

void tlogo03(sprite_status_sp *actionwk) {
    actionwk->sx_posi.w.h += 32;
    if (actionwk->sx_posi.w.h < 464)
        return;
    actionwk->actflg |= 1;
}

void title_obi(sprite_status_sp *actionwk) {
    switch (actionwk->exeno) {
    case 0:
        tobi00(actionwk);
    case 1:
        tobi01(actionwk);
        break;
    case 2:
        tobi02(actionwk);
        break;
    case 3:
        tobi03(actionwk);
        break;
    case 4:
        tobi04(actionwk);
    }

    patset(actionwk);
}

void tobi00(sprite_status_sp *actionwk) {
    actionwk->sprcolor = 34070;
    actionwk->pattbl = mptobi;
    actionwk->sx_posi.w.h = 248;
    actionwk->sy_posi.w.h = 32;
    patinit(actionwk, 0);
    ++actionwk->exeno;
}

void tobi01(sprite_status_sp *actionwk) {
    actionwk->sy_posi.w.h += 32;
    if (actionwk->sy_posi.w.h < 240)
        return;
    actionwk->sy_posi.w.h = 240;
    special_enemy_get_work(actionwk)->timer = 80;
    ++actionwk->exeno;
}

void tobi02(sprite_status_sp *actionwk) {
    if (--special_enemy_get_work(actionwk)->timer == 0)
        ++actionwk->exeno;

    if (special_enemy_get_work(actionwk)->timer != 50)
        return;
    actwk[0].exeno = 21;
}

void tobi03(sprite_status_sp *actionwk) {
    actionwk->sy_posi.w.h -= 32;
    if (actionwk->sy_posi.w.h > 0)
        return;
    special_enemy_get_work(actionwk)->timer = 3;
    actionwk->actflg |= 4;
    ++actionwk->exeno;
    actwk[0].exeno = 1;
}

void tobi04(sprite_status_sp *actionwk) {
    if (--special_enemy_get_work(actionwk)->timer)
        return;
    actionwk->actflg |= 1;
    game_start = 0;
}

Sint32 exp_set(sprite_status_sp **a1) {
    Sint16 d7;
    sprite_status_sp *pActwk;

    pActwk = &actwk[24];
    d7 = 7;
    while (d7) {
        if (pActwk->actno == 0) {
            *a1 = pActwk;
            return 0;
        }
        ++pActwk;
        --d7;
    }
    return 1;
}

void explosion(sprite_status_sp *actionwk) {
    switch (actionwk->exeno) {
    case 0:
        eexp00(actionwk);
    case 1:
        eexp01(actionwk);
    }

    patset(actionwk);
}

void eexp00(sprite_status_sp *actionwk) {
    actionwk->sprcolor = 34734;
    actionwk->pattbl = mpeexp;
    patinit(actionwk, 0);
    special_enemy_get_work(actionwk)->timer = 12;
    ++actionwk->exeno;
    key_set(163);
}

void eexp01(sprite_status_sp *actionwk) {
    if (--special_enemy_get_work(actionwk)->timer)
        return;
    actionwk->actflg |= 1;
}
