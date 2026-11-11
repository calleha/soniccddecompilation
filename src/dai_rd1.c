#include "equ.h"
#include "dai_rd1.h"
#include "action.h"
#include "actset.h"
#include "dircol.h"
#include "dummy.h"
#include "etc.h"
#include "loader2.h"
#include "ridechk.h"
#include "spring.h"

typedef struct {
    Uint8 phase;
    Uint8 state;
    Uint8 ride_offset;
    Sint8 amplitude;
    Uint8 wait_timer;
    Uint16 parent_index;
    Sint16 frameout_x;
    Sint16 origin_x;
    Sint16 origin_y;
    Uint8 vfuta_x_offset;
    Sint8 vfuta_y_offset;
    Uint8 vfuta_phase;
    Uint8 vfuta_phase_low;
} dai_rd1_work;

static dai_rd1_work *dai_rd1_get_work(sprite_status *pActwk) {
    return (dai_rd1_work *)pActwk->actfree;
}

static void dodai_init(sprite_status *pActwk);
static void dodai_move(sprite_status *pActwk);
static void dodai_ud(sprite_status *pActwk);
static void dodai_lr(sprite_status *pActwk);
static Uint8 dodai_ride1(sprite_status *pActwk, Sint32 xpos);
static Uint8 dodai_ride2(sprite_status *pActwk);
static void dodai_nA(sprite_status *pActwk);
static void dodai_nB(sprite_status *pActwk);
static Uint8 dodai_fix(sprite_status *pActwk);
static void dodai_fal(sprite_status *pActwk);
static void dodai_up(sprite_status *pActwk);
static void dodai_upx(sprite_status *pActwk);
static void dodai_rm(sprite_status *pActwk);
static void dodai_lm(sprite_status *pActwk);
static Sint16 dodai_sub(sprite_status *pActwk);
static void dai_fout(sprite_status *pActwk);
static void vfuta_init(sprite_status *pActwk);
static void vfuta_move1(sprite_status *pActwk);
static void vfuta_move2(sprite_status *pActwk);

#if defined(R11B)
#define SPRITE_DAI11A_BASE 518
#define SPRITE_VFUTA_BASE 533
#elif defined(R11C) || defined(R11D)
#define SPRITE_DAI11A_BASE 499
#define SPRITE_VFUTA_BASE 514
#elif defined(R12A) || defined(R12B)
#define SPRITE_DAI11A_BASE 509
#define SPRITE_VFUTA_BASE 527
#elif defined(R12C) || defined(R12D)
#define SPRITE_DAI11A_BASE 500
#define SPRITE_VFUTA_BASE 518
#elif defined(R13C) || defined(R13D)
#define SPRITE_DAI11A_BASE 488
#define SPRITE_VFUTA_BASE 524
#else
#define SPRITE_DAI11A_BASE 533
#define SPRITE_VFUTA_BASE 548
#endif

static sprite_pattern dai11a_pat1 = {1, {{-16, -16, 0, SPRITE_DAI11A_BASE}}};
static sprite_pattern dai11a_pat2 = {1,
                                     {{-32, -16, 0, SPRITE_DAI11A_BASE + 1}}};
static sprite_pattern dai11a_pat3 = {1,
                                     {{-48, -16, 0, SPRITE_DAI11A_BASE + 2}}};
sprite_pattern *dai11a_pattbl[3] = {&dai11a_pat1, &dai11a_pat2, &dai11a_pat3};
static void (*dodai_acttbl[2])(sprite_status *) = {&dodai_init, &dodai_move};
static Uint8 dai_hstbl[3] = {16, 32, 48};
static Uint8 dai_mcnttbl[4] = {2, 3, 4, 6};
static void (*dodai_mtype[10])(sprite_status *) = {&dodai_ud,
                                                   &dodai_lr,
                                                   &dodai_nA,
                                                   &dodai_nB,
                                                   (void (*)(sprite_status *)) &
                                                       dodai_fix,
                                                   &dodai_fal,
                                                   &dodai_up,
                                                   &dodai_upx,
                                                   &dodai_rm,
                                                   &dodai_lm};
static sprite_pattern vfuta_pat0 = {1, {{-4, -24, 0, SPRITE_VFUTA_BASE}}};
static sprite_pattern vfuta_pat1 = {1, {{-4, -24, 0, SPRITE_VFUTA_BASE + 1}}};
sprite_pattern *vfutapat[2] = {&vfuta_pat0, &vfuta_pat1};
static void (*vfuta_tbl[3])(sprite_status *) = {&vfuta_init, &vfuta_move1,
                                                &vfuta_move2};

void dodai(sprite_status *pActwk) {
    dodai_acttbl[pActwk->r_no0 / 2](pActwk);
    actionsub(pActwk);
}

static void dodai_init(sprite_status *pActwk) {
    char wk;
    sprite_status *pNewact;
    dai_rd1_work *pWork = dai_rd1_get_work(pActwk);

    pActwk->actflg |= 4;
    pActwk->sproffset = 17598;
    pActwk->sprpri = 2;

    pWork->origin_x = pActwk->xposi.w.h;
    pWork->origin_y = pActwk->yposi.w.h;
    pWork->frameout_x = pActwk->yposi.w.h;

    pActwk->patbase = dai11a_pattbl;
    wk = pActwk->userflag.b.h & 3;
    pActwk->patno = wk;

    pActwk->sprhsize = dai_hstbl[wk];
    pActwk->sprvsize = 8;

    wk = (pActwk->userflag.b.h >> 2) & 3;
    pWork->amplitude = dai_mcnttbl[wk];

    pActwk->r_no0 += 2;
    if ((wk = pActwk->userflag.b.l) != 0) {

        if (actwkchk(&pNewact) != 0) {

            dai_fout(pActwk);
        } else {
            pNewact->actno = 10;
            pNewact->xposi.w.h = pActwk->xposi.w.h;
            pNewact->yposi.w.h = pActwk->yposi.w.h - 16;
            /* The child is a spring (actno 10), so its work area is a
               spring_work, not dai_rd1_work. */
            spring_work_get(pNewact)->follow_y_offset = -16;
            spring_work_get(pNewact)->parent_index = pActwk - actwk;

            pNewact->userflag.b.h = pActwk->userflag.b.l & 2;
            wk = (Uint8)pActwk->userflag.b.l & 248;
            spring_work_get(pNewact)->follow_x_offset = (Sint8)wk;
            pNewact->xposi.w.h += (Uint16)wk;
        }
    }
}

static void dodai_move(sprite_status *pActwk) {
    Uint8 mtype;
    Uint16 xlen;

    if (time_stop) {
        ridechk(pActwk, &actwk[0]);
        return;
    }

    mtype = (pActwk->userflag.b.h >> 4) & 15;
    dodai_mtype[mtype](pActwk);

    xlen = dai_rd1_get_work(pActwk)->origin_x & 65408;
    xlen -= (scra_h_posit.w.h - 128) & -128;
    if (xlen > 640) {
        ride_on_clr(pActwk, &actwk[0]);
        dai_fout(pActwk);
    }
}

static void dodai_ud(sprite_status *pActwk) {
    ++dai_rd1_get_work(pActwk)->phase;
    pActwk->yposi.w.h = dai_rd1_get_work(pActwk)->origin_y + dodai_sub(pActwk);

    ridechk(pActwk, &actwk[0]);
}

static void dodai_lr(sprite_status *pActwk) {
    Sint32 xsav;

    xsav = pActwk->xposi.l;
    pActwk->xposi.w.h = dai_rd1_get_work(pActwk)->origin_x + dodai_sub(pActwk);

    ++dai_rd1_get_work(pActwk)->phase;

    pActwk->yposi.w.h =
        (dai_rd1_get_work(pActwk)->ride_offset >> 1) +
        dai_rd1_get_work(pActwk)->origin_y;

    dodai_ride1(pActwk, xsav);
}

static Uint8 dodai_ride1(sprite_status *pActwk, Sint32 xpos) {
    pActwk->xspeed.w = (pActwk->xposi.l - xpos) >> 8;
    return dodai_ride2(pActwk);
}

static Uint8 dodai_ride2(sprite_status *pActwk) {
    if (!ridechk(pActwk, &actwk[0])) {

        if (dai_rd1_get_work(pActwk)->ride_offset) {
            --dai_rd1_get_work(pActwk)->ride_offset;
        }
        return 0;
    }

    if (dai_rd1_get_work(pActwk)->ride_offset < 8) {
        ++dai_rd1_get_work(pActwk)->ride_offset;
    }

    return 1;
}

static void dodai_nA(sprite_status *pActwk) {
    Sint32 xsav;

    xsav = pActwk->xposi.l;
    ++dai_rd1_get_work(pActwk)->phase;
    pActwk->yposi.w.h = dai_rd1_get_work(pActwk)->origin_y + dodai_sub(pActwk);

    pActwk->xposi.w.h = dai_rd1_get_work(pActwk)->origin_x + dodai_sub(pActwk);

    dodai_ride1(pActwk, xsav);
}

static void dodai_nB(sprite_status *pActwk) {
    Sint32 xsav;

    xsav = pActwk->xposi.l;
    ++dai_rd1_get_work(pActwk)->phase;
    pActwk->yposi.w.h = dai_rd1_get_work(pActwk)->origin_y + dodai_sub(pActwk);

    pActwk->xposi.w.h = dai_rd1_get_work(pActwk)->origin_x - dodai_sub(pActwk);

    dodai_ride1(pActwk, xsav);
}

static Uint8 dodai_fix(sprite_status *pActwk) {
    pActwk->yposi.w.h =
        ((char)dai_rd1_get_work(pActwk)->ride_offset >> 1) +
        dai_rd1_get_work(pActwk)->origin_y;

    return dodai_ride2(pActwk);
}

static void dodai_fal(sprite_status *pActwk) {
    if (!dai_rd1_get_work(pActwk)->state) {
        if (!dodai_fix(pActwk))
            return;

        dai_rd1_get_work(pActwk)->wait_timer = 30;
        dai_rd1_get_work(pActwk)->state += 2;
    }

    if (dai_rd1_get_work(pActwk)->wait_timer) {
        --dai_rd1_get_work(pActwk)->wait_timer;
        dodai_fix(pActwk);
        return;
    }

    ridechk(pActwk, &actwk[0]);
    pActwk->yposi.l += pActwk->yspeed.w << 8;

    if (pActwk->yspeed.w < 1024) {
        pActwk->yspeed.w += 64;
    }

    if ((scra_v_posit.w.h + 256) < pActwk->yposi.w.h) {

        ride_on_clr(pActwk, &actwk[0]);
        frameout(pActwk);
    }
}

static void dodai_up(sprite_status *pActwk) {
    dai_rd1_work *pWork = dai_rd1_get_work(pActwk);

    switch (pWork->state) {
    case 0:
        if (!dodai_fix(pActwk))
            break;

        pWork->state += 2;
    case 2:
        if (pWork->phase < 64) {
            pActwk->yposi.w.h = pWork->origin_y - dodai_sub(pActwk);

            pWork->phase += 2;
            ridechk(pActwk, &actwk[0]);
            break;
        }

        pWork->origin_y = pActwk->yposi.w.h;
        pWork->state += 2;
    case 4:
        dodai_fix(pActwk);
        break;
    }
}

static void dodai_upx(sprite_status *pActwk) {
    Sint16 coli;
    dai_rd1_work *pWork = dai_rd1_get_work(pActwk);

    switch (pWork->state) {
    case 0:
        if (!dodai_fix(pActwk))
            break;

        pWork->state += 2;
        pWork->wait_timer = 60;
    case 2:
        if (pWork->wait_timer) {
            --pWork->wait_timer;
            dodai_fix(pActwk);
            break;
        }

        speedset2(pActwk);
        pActwk->yspeed.w -= 8;

        if ((coli = emycol_u(pActwk)) >= 0) {
            dodai_ride2(pActwk);
            break;
        }

        pActwk->yposi.w.h -= coli;
        pWork->origin_y = pActwk->yposi.w.h;
        pWork->state += 2;
    case 4:
        dodai_fix(pActwk);
        break;
    }
}

static void dodai_rm(sprite_status *pActwk) {
    Sint32 xsav;
    dai_rd1_work *pWork = dai_rd1_get_work(pActwk);

    switch (pWork->state) {
    case 0:
        if (!dodai_fix(pActwk))
            break;

        pWork->state += 2;
        pWork->wait_timer = 60;
    case 2:
        if (pWork->wait_timer) {
            --pWork->wait_timer;
            dodai_fix(pActwk);
            break;
        }

        if (pWork->phase < 64) {
            xsav = pActwk->xposi.l;
            pActwk->xposi.w.h = pWork->origin_x + dodai_sub(pActwk);

            ++pWork->phase;
            pActwk->yposi.w.h =
                ((char)pWork->ride_offset >> 1) + pWork->origin_y;

            dodai_ride1(pActwk, xsav);
            break;
        }

        pWork->origin_x = pActwk->xposi.w.h;
        pWork->state += 2;
    case 4:
        dodai_fix(pActwk);
        break;
    }
}

static void dodai_lm(sprite_status *pActwk) {
    Sint32 xsav;
    dai_rd1_work *pWork = dai_rd1_get_work(pActwk);

    switch (pWork->state) {
    case 0:
        if (!dodai_fix(pActwk))
            break;

        pWork->state += 2;
        pWork->wait_timer = 60;
    case 2:
        if (pWork->wait_timer) {
            --pWork->wait_timer;
            dodai_fix(pActwk);
            break;
        }

        if (pWork->phase < 64) {
            xsav = pActwk->xposi.l;
            pActwk->xposi.w.h = pWork->origin_x - dodai_sub(pActwk);

            ++pWork->phase;
            pActwk->yposi.w.h =
                ((char)pWork->ride_offset >> 1) + pWork->origin_y;

            dodai_ride1(pActwk, xsav);
            break;
        }

        pWork->origin_x = pActwk->xposi.w.h;
        pWork->state += 2;
    case 4:
        dodai_fix(pActwk);
        break;
    }
}

static Sint16 dodai_sub(sprite_status *pActwk) {
    Sint16 sinwk, coswk;

    sinset((Uint8)(char)dai_rd1_get_work(pActwk)->phase, &sinwk, &coswk);
    return (sinwk * (Sint16)dai_rd1_get_work(pActwk)->amplitude) >> 4;
}

static void dai_fout(sprite_status *pActwk) {
    Sint16 s;

    if (pActwk->cdsts) {
        s = time_flag + pActwk->cdsts * 3;
        flagwork[s] &= 127;
    }

    frameout(pActwk);
}

void vfuta(sprite_status *pActwk) {
    vfuta_tbl[pActwk->r_no0 / 2](pActwk);
    actionsub(pActwk);
    frameout_s(pActwk);
}

static void vfuta_init(sprite_status *pActwk) {
    pActwk->r_no0 += 2;
    pActwk->patbase = vfutapat;
    pActwk->sprpri = 1;
    pActwk->actflg |= 4;
    pActwk->sprhsize = 4;
    pActwk->sprvsize = 24;

    vfuta_move1(pActwk);
}

static void vfuta_move1(sprite_status *pActwk) {
    Sint16 wk;

    if ((wk = pActwk->yposi.w.h - actwk[0].yposi.w.h) < 0) {
        ;
        wk = -wk;
    }

    if (wk < 32) {
        wk = pActwk->xposi.w.h - actwk[0].xposi.w.h;

        if (0 <= wk && wk < 49) {
            dai_rd1_get_work(pActwk)->vfuta_phase =
                dai_rd1_get_work(pActwk)->vfuta_phase_low = 0;
            pActwk->r_no0 = 4;

            if (pActwk->actflg & 128) {
                soundset(164);
                pActwk->patno = 1;
            }
        }
    }

    ride_on_chk(pActwk, &actwk[0]);
}

static void vfuta_move2(sprite_status *pActwk) {
    Sint16 wk;

    wk = dai_rd1_get_work(pActwk)->vfuta_phase;
    wk += 8;
    dai_rd1_get_work(pActwk)->vfuta_phase += 8;

    if (wk > 255) {
        pActwk->r_no0 -= 2;
        pActwk->patno = 0;
    }
}
