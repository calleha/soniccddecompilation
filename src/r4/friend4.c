#include "../equ.h"
#include "friend4.h"
#include "../action.h"
#include "../actset.h"
#include "../etc.h"
#include "../suicide.h"
#include "playsub4.h"

typedef struct {
    Sint32 x_speed;
    Sint16 base_y;
    union {
        Sint32 phase;
        struct {
            Uint8 phase_lo;
            Uint8 angle;
            Uint8 phase_hi0;
            Uint8 phase_hi1;
        };
    };
    Sint32 phase_delta;
    Sint16 base_x;
    Sint16 movie_parent_index;
    Uint8 movie_done;
} friend4_work;

static friend4_work *friend4_get_work(sprite_status *pActwk) {
    return (friend4_work *)pActwk->actfree;
}

#if defined(R41A)
#define SPRITE_FRIEND4_BASE 494
#elif defined(R41B)
#define SPRITE_FRIEND4_BASE 455
#elif defined(R41C)
#define SPRITE_FRIEND4_BASE 472
#elif defined(R41D)
#define SPRITE_FRIEND4_BASE 462
#elif defined(R42A)
#define SPRITE_FRIEND4_BASE 500
#elif defined(R42B)
#define SPRITE_FRIEND4_BASE 453
#elif defined(R42C) || defined(R42D)
#define SPRITE_FRIEND4_BASE 457
#else
#define SPRITE_FRIEND4_BASE 435
#endif

static void m_init(sprite_status *pActwk);
static void m0move(sprite_status *pActwk);
static void m1move(sprite_status *pActwk);

static Uint8 pchg0[30] = {0, 0, 0, 2, 0, 0, 2, 1, 1, 2, 1, 1, 2, 0, 0,
                          2, 2, 0, 0, 2, 2, 1, 1, 2, 2, 1, 1, 2, 2, 255};
static Uint8 pchg1[4] = {19, 0, 1, 255};
static Uint8 *pchg[2] = {pchg0, pchg1};
static sprite_pattern spr_friend4_00 = {1, {{-8, -12, 0, SPRITE_FRIEND4_BASE}}};
static sprite_pattern spr_friend4_01 = {
    1, {{-8, -12, 0, SPRITE_FRIEND4_BASE + 1}}};
static sprite_pattern spr_friend4_10 = {
    1, {{-8, -12, 0, SPRITE_FRIEND4_BASE + 2}}};
static sprite_pattern spr_friend4_11 = {
    1, {{-8, -12, 0, SPRITE_FRIEND4_BASE + 3}}};
static sprite_pattern spr_friend4_99 = {
    0, {{-8, -12, 0, SPRITE_FRIEND4_BASE + 3}}};
sprite_pattern *pat_friend0[3] = {&spr_friend4_00, &spr_friend4_01,
                                  &spr_friend4_99};
sprite_pattern *pat_friend1[3] = {&spr_friend4_10, &spr_friend4_11,
                                  &spr_friend4_99};
/* The lookup in m_init() builds the index as 2 * (4 * stageno.b.l +
   time_flag), so in 43C it asks for entry 20 of a table the original binary
   only fills up to entry 10.  There the read runs into the words that follow
   the table - all zero apart from a 1 at entry 16, plus 0xF8F8 and 0x1ED at
   the odd entries 17 and 19 that the formula never produces - and here it
   would read whatever this build happens to place next, which
   AddressSanitizer rejects as a global-buffer-overflow.  The array keeps the
   original's values for every index the formula can reach. */
static Uint16 tbl0sproffset[32] = {
    1104, 1104, 1104, 0,    1104, 1104, 1104, 0,    0,    0,    1104,
    0,    0,    0,    0,    0,    1,    0xF8F8, 0,   0x1ED, 0,   0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0};

void friend4(sprite_status *pActwk) {
    if (!friend_suicide(pActwk)) {
        if ((char)pActwk->r_no0 < 0) {
            m0move(pActwk);
            return;
        }

        if (pActwk->r_no0 != 0) {
            m1move(pActwk);
            return;
        }
        m_init(pActwk);
    }
}

static void m_init(sprite_status *pActwk) {
    friend4_work *work = friend4_get_work(pActwk);
    short_union temp;

    pActwk->actflg |= 4;
    pActwk->sprpri = 5;
    pActwk->sprhs = 8;
    pActwk->sprhsize = 8;
    pActwk->sprvsize = 12;

    temp.w = 0;
    temp.b.l = stageno.b.l;
    temp.w <<= 2;
    temp.b.l = temp.b.l + (char)time_flag;
    temp.w <<= 1;
    pActwk->sproffset = tbl0sproffset[temp.w];
    if (pActwk->userflag.b.h & 1) {
        pActwk->sproffset += 8192;
    }

    pActwk->patbase = pat_friend0;
    if (pActwk->userflag.b.h & 1)
        pActwk->patbase = pat_friend1;
    work->base_y = pActwk->yposi.w.h;
    if (pActwk->userflag.b.h < 0) {

        pActwk->r_no0 -= 2;
        pActwk->actflg ^= 1;
        pActwk->cddat ^= 1;
        work->base_x = pActwk->xposi.w.h;
        work->phase_delta = 512;
        if (pActwk->userflag.b.h & 1) {

            pActwk->actflg ^= 1;
            pActwk->cddat ^= 1;
            work->phase_delta *= -1;
            work->phase = 12288;
        }
    } else {

        pActwk->r_no0 += 2;
        pActwk->mstno.b.h = 1;
        work->x_speed = 65536;
        work->phase_delta = 256;
    }
}

static void m0move(sprite_status *pActwk) {
    friend4_work *work = friend4_get_work(pActwk);
    sprite_status *tempact;
    Uint16 sin, cos;
    Sint32 sinl, cosl;

    tempact = &actwk[work->movie_parent_index];
    if (tempact->actno != 82) {

        frameout(pActwk);
        return;
    }
    if (friend4_get_work(tempact)->movie_done) {

        frameout(pActwk);
        return;
    }

    sinset(work->angle, (Sint16 *)&sin, (Sint16 *)&cos);
    cosl = 0, sinl = 0;
    cosl = cos;
    sinl = sin;
    cosl = (Uint32)cosl >> 16 & 65535 | cosl << 16 & -65536;
    sinl = (Uint32)sinl >> 16 & 65535 | sinl << 16 & -65536;
    cosl >>= 3;
    sinl >>= 4;
    cosl = cosl + 32768;
    sinl = sinl + 32768;
    cosl = (Uint32)cosl >> 16 & 65535 | cosl << 16 & -65536;
    sinl = (Uint32)sinl >> 16 & 65535 | sinl << 16 & -65536;
    pActwk->xposi.w.h = work->base_x + (Sint16)(cosl & 65535);
    pActwk->yposi.w.h = work->base_y + (Sint16)(sinl & 65535);

    work->phase += work->phase_delta;
    if (!(work->phase & 32767)) {

        pActwk->actflg ^= 1;
        pActwk->cddat ^= 1;
        if (pActwk->sprpri != 5) {

            pActwk->sprpri = 5;
        } else {

            pActwk->sprpri = 3;
        }
    }

    patchg(pActwk, pchg);
    actionsub(pActwk);
}

static void m1move(sprite_status *pActwk) {
    friend4_work *work = friend4_get_work(pActwk);
    Uint16 sin, cos;
    Sint32 sinl;

    pActwk->xposi.l += work->x_speed;

    sinset(work->angle, (Sint16 *)&sin, (Sint16 *)&cos);
    sinl = 0;
    sinl = sin;
    sinl = (Uint32)sinl >> 16 & 65535 | sinl << 16 & -65536;
    sinl >>= 3;
    sinl = sinl + 32768;
    sinl = (Uint32)sinl >> 16 & 65535 | sinl << 16 & -65536;
    pActwk->yposi.w.h = work->base_y + (Sint16)(sinl & 65535);

    work->phase += work->phase_delta;
    if (work->phase < 0) {

        work->phase_delta *= -1;
        work->x_speed *= -1;
        pActwk->actflg ^= 1;
        pActwk->cddat ^= 1;
    } else {

        if (work->phase >= 65536) {

            work->phase_delta *= -1;
            work->x_speed *= -1;
            pActwk->actflg ^= 1;
            pActwk->cddat ^= 1;
        }
    }

    patchg(pActwk, pchg);
    actionsub(pActwk);
    frameout_s(pActwk);
}
