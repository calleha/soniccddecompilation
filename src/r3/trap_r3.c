#include "../equ.h"
#include "trap_r3.h"
#include "../action.h"
#include "../actset.h"
#include "../etc.h"
#include "../loader2.h"
#include "../playsub.h"
#include "../player_work.h"
#include "../ridechk.h"
#include "../spring.h"
#include "coli3.h"

typedef struct {
    Sint16 base_y;
    Sint16 base_x;
    Uint16 linked_actor_index;
    Sint16 velocity;
    Sint16 table_offset;
    union {
        Sint16 origin_x;
        Sint16 acceleration;
        Sint16 bullet_origin_x;
        struct {
            Uint8 origin_x_low;
            Uint8 drum_speed_high;
        };
    };
    Sint16 delta;
    Uint8 wait_timer;
    Uint8 phase;
    Uint8 timer;
    Uint8 travel;
    Uint8 reverse_flag;
    Sint16 bullet_acceleration;
    Uint8 bob_angle;
    Uint8 bob_enabled;
} trap3_work;

static trap3_work *trap3_get_work(sprite_status *pActwk) {
    return (trap3_work *)pActwk->actfree;
}

static sprite_pattern har00 = {1, {{-16, -16, 0, 288}}};
static sprite_pattern har01 = {1, {{-16, -16, 0, 289}}};
static sprite_pattern har02 = {1, {{-16, -16, 0, 290}}};
static sprite_pattern har03 = {1, {{-16, -16, 0, 291}}};
sprite_pattern *harir3pat[4] = {&har00, &har01, &har02, &har03};

void harir3(sprite_status *hariwk) {
    void (*harir3_acttbl[2])(sprite_status *) = {&harir3_init, &harir3_move};
    Uint16 ride_no, cal_x;

    harir3_acttbl[hariwk->r_no0 / 2](hariwk);
    actionsub(hariwk);

    if ((ride_no = trap3_get_work(hariwk)->linked_actor_index) != 0) {
        hariwk->xposi.w.h =
            (Sint16)trap3_get_work(hariwk)->wait_timer +
            actwk[ride_no].xposi.w.h;

        hariwk->yposi.w.h =
            (Sint16)trap3_get_work(hariwk)->phase +
            actwk[ride_no].yposi.w.h;
    }

    cal_x = (trap3_get_work(hariwk)->origin_x & 65408) -
            ((Uint16)(scra_h_posit.w.h - 128) & 65408);

    if (cal_x > 640)
        tr3_fout(hariwk);
}

void harir3_init(sprite_status *hariwk) {
    Uint8 harir3sztbl[12] = {16, 8, 0, 18, 12, 131, 12, 16, 130, 12, 16, 130};
    Uint8 cal_index;

    hariwk->r_no0 += 2;
    hariwk->actflg |= 4;

    hariwk->sprpri = 3;
    hariwk->patbase = harir3pat;

    trap3_get_work(hariwk)->origin_x = hariwk->xposi.w.h;
    trap3_get_work(hariwk)->base_x = hariwk->yposi.w.h;

    cal_index = hariwk->patno = (Uint8)hariwk->userflag.b.h & 3;
    cal_index *= 3;
    hariwk->sprhsize = harir3sztbl[cal_index];
    hariwk->sprvsize = harir3sztbl[cal_index + 1];
    hariwk->colino = harir3sztbl[cal_index + 2];
    harir3_move(hariwk);
}

void harir3_move(sprite_status *hariwk) {
    harir3_kind(hariwk);

    if (hariwk->patno != 0) {
        hitchk(hariwk, &actwk[0]);
        return;
    }

    if (hitchk(hariwk, &actwk[0]) == 0)
        return;
    if (!(hariwk->cddat & 8))
        return;

    if (plpower_a != 0)
        return;
    if (plpower_m != 0)
        return;

    if (actwk[0].r_no0 >= 4)
        return;

    if (player_work_get(&actwk[0])->damage_invulnerability_timer != 0)
        return;

    actwk[0].yposi.l -= actwk[0].yspeed.w << 8;
    playdamageset(&actwk[0], hariwk);
}

void harir3_kind(sprite_status *hariwk) {
    void (*harir3_kndtbl[4])(sprite_status *) = {&harir3_ymv1, &harir3_ymv2,
                                                 &harir3_xmv1, &harir3_xmv2};
    if (!(hariwk->userflag.b.h & 4))
        return;
    harir3_kndtbl[hariwk->userflag.b.h & 3](hariwk);
}

void harir3_ymv1(sprite_status *hariwk) {
    harir3_sub(hariwk);
    hariwk->yposi.w.h =
        trap3_get_work(hariwk)->base_x + (Sint16)trap3_get_work(hariwk)->travel;
}

void harir3_ymv2(sprite_status *hariwk) {
    harir3_sub(hariwk);
    hariwk->yposi.w.h =
        trap3_get_work(hariwk)->base_x - (Sint16)trap3_get_work(hariwk)->travel;
}

void harir3_xmv1(sprite_status *hariwk) {
    harir3_sub(hariwk);
    hariwk->xposi.w.h =
        trap3_get_work(hariwk)->origin_x - (Sint16)trap3_get_work(hariwk)->travel;
}

void harir3_xmv2(sprite_status *hariwk) {
    harir3_sub(hariwk);
    hariwk->xposi.w.h =
        trap3_get_work(hariwk)->origin_x + (Sint16)trap3_get_work(hariwk)->travel;
}

void harir3_sub(sprite_status *hariwk) {
    if (trap3_get_work(hariwk)->timer != 0) {
        if (--trap3_get_work(hariwk)->timer != 0)
            return;
        if (hariwk->actflg & 128)
            soundset(183);
    }

    if (trap3_get_work(hariwk)->reverse_flag != 0) {
        if ((char)(trap3_get_work(hariwk)->travel -= 8) >= 0)
            return;

        trap3_get_work(hariwk)->travel = trap3_get_work(hariwk)->reverse_flag = 0;
        trap3_get_work(hariwk)->timer = 60;
        return;
    }

    if ((trap3_get_work(hariwk)->travel += 8) < 32)
        return;
    trap3_get_work(hariwk)->travel = 32;
    trap3_get_work(hariwk)->reverse_flag = 1;
    trap3_get_work(hariwk)->timer = 60;
}

static sprite_pattern frd00 = {1, {{-16, -16, 0, 292}}};
sprite_pattern *frdr3pat[1] = {&frd00};

void frdr3(sprite_status *floorwk) {
    void (*frdr3_acttbl[2])(sprite_status *) = {&frdr3_init, &frdr3_move};
    Uint16 cal_x;
    frdr3_acttbl[floorwk->r_no0 / 2](floorwk);
    actionsub(floorwk);
    cal_x = (trap3_get_work(floorwk)->base_x & 65408) -
            ((Uint16)(scra_h_posit.w.h - 128) & 65408);

    if (cal_x > 640)
        tr3_fout(floorwk);
}

void frdr3_init(sprite_status *floorwk) {
    sprite_status *new_actwk;

    floorwk->r_no0 += 2;
    floorwk->actflg |= 4;

    floorwk->sprpri = 3;
    floorwk->patbase = frdr3pat;
    floorwk->sprhsize = floorwk->sprvsize = 16;
    trap3_get_work(floorwk)->base_x = floorwk->xposi.w.h;
    trap3_get_work(floorwk)->base_y = floorwk->yposi.w.h;

    if (floorwk->userflag.b.h < 2) {
        frdr3_move(floorwk);
        return;
    }
    if (actwkchk2(floorwk, &new_actwk) != 0) {
        frameout(floorwk);
        return;
    }
    new_actwk->actno = 10;
    new_actwk->xposi.w.h = floorwk->xposi.w.h;
    new_actwk->yposi.w.h = floorwk->yposi.w.h;
    /* The child is a spring (actno 10), so its work area is a spring_work. */
    spring_work_get(new_actwk)->follow_y_offset = (Sint8)234;
    trap3_get_work(new_actwk)->linked_actor_index = floorwk - actwk;
    new_actwk->userflag.b.h = (floorwk->userflag.b.h & 1) << 1;
    frdr3_move(floorwk);
}

void frdr3_move(sprite_status *floorwk) {

    char frdr3_mvtbl0[8] = {64, -8, 64, -8, 64, 8, 64, 8};

    Sint16 frdr3_mvtbl1[4] = {512, 0, -512, 0};

    ride_on_chk(floorwk, &actwk[0]);
    if (time_flag == 0)
        return;

    do {
        if (floorwk->userflag.b.h == 1)
            return;
        if (trap3_get_work(floorwk)->timer == 0) {
            trap3_get_work(floorwk)->timer =
                frdr3_mvtbl0[trap3_get_work(floorwk)->travel * 2];
            trap3_get_work(floorwk)->delta =
                frdr3_mvtbl0[trap3_get_work(floorwk)->travel * 2 + 1];

            trap3_get_work(floorwk)->origin_x =
                frdr3_mvtbl1[trap3_get_work(floorwk)->travel];
            return;
        }

        trap3_get_work(floorwk)->origin_x += trap3_get_work(floorwk)->delta;
        floorwk->xspeed.w = trap3_get_work(floorwk)->origin_x;
        floorwk->xposi.l += floorwk->xspeed.w << 8;

        if (--trap3_get_work(floorwk)->timer != 0)
            return;
        trap3_get_work(floorwk)->travel =
            trap3_get_work(floorwk)->travel + 1 & 3;
    } while (1);
}

static sprite_pattern trd00 = {1, {{-16, -16, 0, 293}}};
static sprite_pattern trd01 = {1, {{-16, -16, 0, 294}}};
sprite_pattern *trapdr3pat[2] = {&trd00, &trd01};

void trapdr3(sprite_status *floorwk) {
    void (*trapdr3_acttbl[2])(sprite_status *) = {&trapdr3_init, &trapdr3_move};
    Uint16 cal_x;

    trapdr3_acttbl[floorwk->r_no0 / 2](floorwk);
    actionsub(floorwk);
    cal_x = (trap3_get_work(floorwk)->origin_x & 65408) -
            ((Uint16)(scra_h_posit.w.h - 128) & 65408);

    if (cal_x > 640)
        tr3_fout(floorwk);
}

void trapdr3_init(sprite_status *floorwk) {
    sprite_status *new_actwk;

    floorwk->actflg |= 4;

    floorwk->sprpri = 3;
    floorwk->patbase = trapdr3pat;
    floorwk->patno = floorwk->userflag.b.h >> 2;

    floorwk->sprhsize = 16;
    floorwk->sprvsize = 14;
    trap3_get_work(floorwk)->origin_x = floorwk->xposi.w.h;
    trap3_get_work(floorwk)->base_x = floorwk->yposi.w.h;
    floorwk->r_no0 += 2;

    if ((floorwk->userflag.b.h & 3) == 0) {
        trapdr3_move(floorwk);
        return;
    }

    if (actwkchk2(floorwk, &new_actwk) != 0) {
        trapdr3_move(floorwk);
        return;
    }

    if ((floorwk->userflag.b.h & 3) == 1)
        new_actwk->actno = 32;
    else
        new_actwk->actno = 10;
    new_actwk->xposi.w.h = floorwk->xposi.w.h;
    new_actwk->yposi.w.h = floorwk->yposi.w.h;
    trap3_get_work(new_actwk)->linked_actor_index = floorwk - actwk;
    if (new_actwk->actno == 32) {
        trap3_get_work(new_actwk)->wait_timer = 32;
        new_actwk->userflag.b.h = 2;
    } else {
        /* This child is a spring (actno 10): put it on the block's right side. */
        spring_work_get(new_actwk)->follow_x_offset = 24;
        new_actwk->userflag.b.h = (floorwk->userflag.b.h & 1) << 1 | 4;
    }
    trapdr3_move(floorwk);
}

void trapdr3_move(sprite_status *floorwk) {
    trapdr3_kind(floorwk);
    hitchk(floorwk, &actwk[0]);
}

void trapdr3_kind(sprite_status *floorwk) {
    if (!(floorwk->userflag.b.h & 4)) {

        trapdr3_updown(floorwk);
        floorwk->yposi.w.h =
            trap3_get_work(floorwk)->base_x +
            (Sint16)trap3_get_work(floorwk)->travel;

    } else {
        trapdr3_updown(floorwk);
        floorwk->yposi.w.h =
            trap3_get_work(floorwk)->base_x -
            (Sint16)trap3_get_work(floorwk)->travel;
    }
}

void trapdr3_updown(sprite_status *floorwk) {
    Sint16 cal_x;

    cal_x =
        actwk[0].xposi.w.h - (floorwk->xposi.w.h - (Sint16)floorwk->sprhsize);
    if (cal_x >= 0) {
        if (cal_x < 80)
            return;

        if ((char)(trap3_get_work(floorwk)->travel -= 8) < 0)
            trap3_get_work(floorwk)->travel = 0;
        return;
    }

    if ((trap3_get_work(floorwk)->travel += 8) >= 32)
        trap3_get_work(floorwk)->travel = 32;
}

static sprite_pattern fo00 = {1, {{-32, -16, 0, 295}}};
sprite_pattern *for3pat[1] = {&fo00};

void for3(sprite_status *floorwk) {
    void (*tbl[2])(sprite_status *) = {&for3_init, &for3_move};

    tbl[floorwk->r_no0 / 2](floorwk);
    actionsub(floorwk);
}

void for3_init(sprite_status *floorwk) {
    floorwk->actflg |= 4;

    floorwk->sprpri = 3;
    floorwk->patbase = for3pat;
    floorwk->sprhsize = 32;
    floorwk->sprvsize = 16;

    trap3_get_work(floorwk)->base_x = floorwk->xposi.w.h;
    trap3_get_work(floorwk)->base_y = floorwk->yposi.w.h;
    floorwk->r_no0 += 2;
    for3_move(floorwk);
}

void for3_move(sprite_status *floorwk) {
    void (*for3_kndtbl[8])(sprite_status *) = {&for3_fix, &for3_rmv, &for3_lmv,
                                               &for3_umv, &for3_dmv, &for3_rup,
                                               &for3_rup, &for3_rup};
    Uint16 cal_x;
    for3_kndtbl[floorwk->userflag.b.h](floorwk);
    dai3sub(floorwk);
    cal_x = (trap3_get_work(floorwk)->base_x & 65408) -
            ((Uint16)(scra_h_posit.w.h - 128) & 65408);

    if (cal_x > 640)
        tr3_fout(floorwk);
}

void for3_ridechk(sprite_status *thingwk) { ridechk(thingwk, &actwk[0]); }

void dai3sub(sprite_status *floorwk) {
    Sint16 tmp_sin, tmp_cos;
    int_union cal_sin;

    if (trap3_get_work(floorwk)->bob_enabled == 0)
        return;

    if (!(floorwk->cddat & 8)) {
        if (trap3_get_work(floorwk)->bob_angle == 0)
            return;
        trap3_get_work(floorwk)->bob_angle -= 8;
    } else {
        if (trap3_get_work(floorwk)->bob_angle == 64)
            return;
        trap3_get_work(floorwk)->bob_angle += 8;
    }
    sinset(trap3_get_work(floorwk)->bob_angle, &tmp_sin, &tmp_cos);
    cal_sin.l = tmp_sin << 10;
    floorwk->yposi.w.h = trap3_get_work(floorwk)->base_y + cal_sin.w.h;
}

void for3_fix(sprite_status *floorwk) {
    trap3_get_work(floorwk)->bob_enabled = 1;
    for3_ridechk(floorwk);
}

void for3_lmv(sprite_status *floorwk) { for3_rmv(floorwk); }

void for3_rmv(sprite_status *floorwk) {

    char for3_rmvtbl0[16] = {28, 0,  8, -48, 8,  -48, 56, 0,
                             8,  48, 8, 48,  56, 0,   8,  -48};
    Sint16 for3_rmvtbl1[8] = {768, 768, 0, -768, -768, 0, 768, 768};

    char for3_lmvtbl0[16] = {28, 0,   8, 48,  8,  48, 56, 0,
                             8,  -48, 8, -48, 56, 0,  8,  48};
    Sint16 for3_lmvtbl1[8] = {-768, -768, 0, 768, 768, 0, -768, -768};

    do {
        if (trap3_get_work(floorwk)->wait_timer == 0) {

            trap3_get_work(floorwk)->bob_enabled = 1;
            if (floorwk->userflag.b.h == 1) {
                trap3_get_work(floorwk)->wait_timer =
                    for3_rmvtbl0[trap3_get_work(floorwk)->phase * 2];
                trap3_get_work(floorwk)->acceleration =
                    for3_rmvtbl0[trap3_get_work(floorwk)->phase * 2 + 1];

                trap3_get_work(floorwk)->velocity =
                    for3_rmvtbl1[trap3_get_work(floorwk)->phase];

                for3_ridechk(floorwk);
            } else {
                trap3_get_work(floorwk)->wait_timer =
                    for3_lmvtbl0[trap3_get_work(floorwk)->phase * 2];
                trap3_get_work(floorwk)->acceleration =
                    for3_lmvtbl0[trap3_get_work(floorwk)->phase * 2 + 1];

                trap3_get_work(floorwk)->velocity =
                    for3_lmvtbl1[trap3_get_work(floorwk)->phase];

                for3_ridechk(floorwk);
            }
            return;
        }
        floorwk->xposi.l +=
            (floorwk->xspeed.w = trap3_get_work(floorwk)->velocity) << 8;

        trap3_get_work(floorwk)->velocity +=
            trap3_get_work(floorwk)->acceleration;

        if (--trap3_get_work(floorwk)->wait_timer != 0) {
            for3_ridechk(floorwk);
            break;
        }
        if (++trap3_get_work(floorwk)->phase == 8)
            trap3_get_work(floorwk)->phase = 2;
    } while (1);
}

void for3_umv(sprite_status *floorwk) { for3_dmv(floorwk); }

void for3_dmv(sprite_status *floorwk) {

    char for3_umvtbl0[16] = {35, 0,   8, 64,  8,  64, 70, 0,
                             8,  -64, 8, -64, 70, 0,  8,  64};
    Sint16 for3_umvtbl1[8] = {-512, -512, 0, 512, 512, 0, -512, -512};

    char for3_dmvtbl0[16] = {35, 0,  8, -64, 8,  -64, 70, 0,
                             8,  64, 8, 64,  70, 0,   8,  -64};
    Sint16 for3_dmvtbl1[8] = {512, 512, 0, -512, -512, 0, 512, 512};

    do {
        for3_ridechk(floorwk);

        if (trap3_get_work(floorwk)->wait_timer == 0) {
            trap3_get_work(floorwk)->bob_enabled = 0;
            if (floorwk->userflag.b.h == 3) {
                trap3_get_work(floorwk)->wait_timer =
                    for3_umvtbl0[trap3_get_work(floorwk)->phase * 2];
                trap3_get_work(floorwk)->acceleration =
                    for3_umvtbl0[trap3_get_work(floorwk)->phase * 2 + 1];

                trap3_get_work(floorwk)->velocity =
                    for3_umvtbl1[trap3_get_work(floorwk)->phase];

            } else {
                trap3_get_work(floorwk)->wait_timer =
                    for3_dmvtbl0[trap3_get_work(floorwk)->phase * 2];
                trap3_get_work(floorwk)->acceleration =
                    for3_dmvtbl0[trap3_get_work(floorwk)->phase * 2 + 1];

                trap3_get_work(floorwk)->velocity =
                    for3_dmvtbl1[trap3_get_work(floorwk)->phase];
            }

            return;
        }
        floorwk->yposi.l += trap3_get_work(floorwk)->velocity << 8;
        trap3_get_work(floorwk)->velocity +=
            trap3_get_work(floorwk)->acceleration;
        floorwk->yspeed.w = trap3_get_work(floorwk)->velocity & 65280;
        if (--trap3_get_work(floorwk)->wait_timer != 0)
            break;
        if (++trap3_get_work(floorwk)->phase == 8)
            trap3_get_work(floorwk)->phase = 2;
    } while (1);
}

void for3_rup(sprite_status *floorwk) {
    void (*tbl[3])(sprite_status *) = {&for3_rup1, &for3_rup2, &for3_rup3};

    for3_ridechk(floorwk);

    tbl[trap3_get_work(floorwk)->timer / 2](floorwk);
}

void for3_rup1(sprite_status *floorwk) {
    if (trap3_get_work(floorwk)->wait_timer == 0) {

        trap3_get_work(floorwk)->bob_enabled = 1;
        if (ridechk(floorwk, &actwk[0]) != 0)
            trap3_get_work(floorwk)->wait_timer = 30;
        return;
    }
    if (--trap3_get_work(floorwk)->wait_timer != 0)
        return;
    trap3_get_work(floorwk)->bob_enabled = 0;
    trap3_get_work(floorwk)->timer += 2;
}

void for3_rup2(sprite_status *floorwk) {

    char for3_8btbl0[6] = {16, -16, 48, 0, 16, 16};
    Sint16 for3_8btbl1[3] = {0, -512, -512};

    char for3_16btbl0[6] = {16, -48, 69, 0, 16, 48};
    Sint16 for3_16btbl1[3] = {0, -768, -768};

    char for3_28btbl0[6] = {16, -64, 88, 0, 16, 64};
    Sint16 for3_28btbl1[3] = {0, -1024, -1024};

    do {
        if (trap3_get_work(floorwk)->wait_timer == 0) {

            if (floorwk->userflag.b.h == 5) {
                trap3_get_work(floorwk)->wait_timer =
                    for3_8btbl0[trap3_get_work(floorwk)->phase * 2];
                trap3_get_work(floorwk)->acceleration =
                    for3_8btbl0[trap3_get_work(floorwk)->phase * 2 + 1];

                trap3_get_work(floorwk)->velocity =
                    for3_8btbl1[trap3_get_work(floorwk)->phase];

            } else if (floorwk->userflag.b.h == 6) {
                trap3_get_work(floorwk)->wait_timer =
                    for3_16btbl0[trap3_get_work(floorwk)->phase * 2];
                trap3_get_work(floorwk)->acceleration =
                    for3_16btbl0[trap3_get_work(floorwk)->phase * 2 + 1];

                trap3_get_work(floorwk)->velocity =
                    for3_16btbl1[trap3_get_work(floorwk)->phase];

            } else {
                trap3_get_work(floorwk)->wait_timer =
                    for3_28btbl0[trap3_get_work(floorwk)->phase * 2];
                trap3_get_work(floorwk)->acceleration =
                    for3_28btbl0[trap3_get_work(floorwk)->phase * 2 + 1];

                trap3_get_work(floorwk)->velocity =
                    for3_28btbl1[trap3_get_work(floorwk)->phase];
            }

            return;
        }
        trap3_get_work(floorwk)->velocity +=
            trap3_get_work(floorwk)->acceleration;
        floorwk->yspeed.w = trap3_get_work(floorwk)->velocity & 65280;
        floorwk->yposi.l += floorwk->yspeed.w << 8;

        if (--trap3_get_work(floorwk)->wait_timer != 0)
            return;
    } while (++trap3_get_work(floorwk)->phase != 3);

    floorwk->yspeed.w = 0;
    trap3_get_work(floorwk)->bob_enabled = 1;
    trap3_get_work(floorwk)->base_y = floorwk->yposi.w.h;
    trap3_get_work(floorwk)->timer += 2;
}

void for3_rup3(sprite_status *floorwk) { floorwk->actno = floorwk->actno; }

static sprite_pattern gtd00 = {1, {{-32, -8, 0, 296}}};
sprite_pattern *getdair3pat[1] = {&gtd00};

void getdair3(sprite_status *floorwk) {
    void (*tbl[2])(sprite_status *) = {&getdair3_init, &getdair3_move};
    Uint16 cal_x;
    tbl[floorwk->r_no0 / 2](floorwk);
    actionsub(floorwk);

    cal_x = (trap3_get_work(floorwk)->origin_x & 65408) -
            ((Uint16)scra_h_posit.w.h - 128 & 65408);

    if (cal_x > 640)
        tr3_fout(floorwk);
}

void getdair3_init(sprite_status *floorwk) {
    Uint8 getdair3_cnttbl[3] = {0, 30, 60};

    floorwk->actflg |= 4;

    floorwk->sprpri = 3;
    floorwk->patbase = getdair3pat;
    floorwk->sprhsize = 28;
    floorwk->sprvsize = 8;

    trap3_get_work(floorwk)->origin_x = floorwk->xposi.w.h;
    trap3_get_work(floorwk)->base_x = floorwk->yposi.w.h;

    trap3_get_work(floorwk)->timer =
        getdair3_cnttbl[floorwk->userflag.b.h & 15];

    floorwk->r_no0 += 2;
    getdair3_move(floorwk);
}

void getdair3_move(sprite_status *floorwk) {
    if (floorwk->userflag.b.h < 0) {

        getdair3_move_sub(floorwk);
        floorwk->xposi.w.h =
            trap3_get_work(floorwk)->origin_x +
            (Sint16)trap3_get_work(floorwk)->travel;

    } else {
        getdair3_move_sub(floorwk);
        floorwk->xposi.w.h =
            trap3_get_work(floorwk)->origin_x -
            (Sint16)trap3_get_work(floorwk)->travel;
    }

    ridechk(floorwk, &actwk[0]);
}

void getdair3_move_sub(sprite_status *floorwk) {
    if (actwk[0].yposi.w.h < floorwk->yposi.w.h) {

        if ((char)(trap3_get_work(floorwk)->travel -= 16) >= 0)
            return;
        trap3_get_work(floorwk)->travel = 0;

        if (floorwk->userflag.b.h & 15)
            return;
        if (trap3_get_work(floorwk)->reverse_flag != 0)
            return;
        trap3_get_work(floorwk)->timer = 60;
        trap3_get_work(floorwk)->reverse_flag = 255;
        return;
    }

    if ((trap3_get_work(floorwk)->travel += 16) < 64)
        return;
    trap3_get_work(floorwk)->travel = 64;
    trap3_get_work(floorwk)->timer = 8;
}

static Uint8 pchg1[4] = {2, 1, 2, 255};
static Uint8 pchg2[4] = {2, 3, 5, 255};
static Uint8 pchg3[4] = {2, 4, 5, 255};
static Uint8 pchg4[6] = {0, 3, 6, 5, 6, 255};
static Uint8 pchg5[6] = {0, 4, 6, 5, 6, 255};
Uint8 *gandair3_pchg[5] = {pchg1, pchg2, pchg3, pchg4, pchg5};
static sprite_pattern gnd00 = {1, {{-16, -4, 0, 297}}};
static sprite_pattern gnd01 = {1, {{-16, -20, 0, 298}}};
static sprite_pattern gnd02 = {1, {{-16, -20, 0, 299}}};
static sprite_pattern gnd03 = {1, {{-8, -8, 0, 300}}};
static sprite_pattern gnd04 = {1, {{-8, -8, 0, 301}}};
static sprite_pattern gnd05 = {1, {{-16, -20, 0, 0}}};
sprite_pattern *gandair3pat[7] = {&gnd00, &gnd01, &gnd02, &gnd03,
                                  &gnd04, &gnd05, &gnd05};

void gandair3(sprite_status *floorwk) {
    void (*tbl[2])(sprite_status *) = {&gandair3_daii, &gandair3_daim};
    Uint16 cal_x;

    if (floorwk->userflag.b.h == 1) {
        gandair3_core(floorwk);
        return;
    } else if (floorwk->userflag.b.h == 2) {
        gandair3_tama(floorwk);
        return;
    }

    tbl[floorwk->r_no0 / 2](floorwk);
    actionsub(floorwk);

    cal_x = (trap3_get_work(floorwk)->origin_x & 65408) -
            ((Uint16)(scra_h_posit.w.h - 128) & 65408);

    if (cal_x > 640)
        tr3_fout(floorwk);
}

void gandair3_daii(sprite_status *floorwk) {
    sprite_status *new_actwk;

    floorwk->actflg |= 4;

    floorwk->sprpri = 2;
    floorwk->patbase = gandair3pat;
    trap3_get_work(floorwk)->origin_x = floorwk->xposi.w.h;
    trap3_get_work(floorwk)->base_x = floorwk->yposi.w.h;
    floorwk->sprhsize = 16;

    if (actwkchk(&new_actwk) != 0)
        return;

    new_actwk->actno = 39;
    new_actwk->xposi.w.h = floorwk->xposi.w.h;
    new_actwk->yposi.w.h = floorwk->yposi.w.h + 8;
    new_actwk->userflag.b.h = 1;

    floorwk->r_no0 += 2;
    gandair3_daim(floorwk);
}

void gandair3_daim(sprite_status *floorwk) { floorwk->actno = floorwk->actno; }

void gandair3_core(sprite_status *corewk) {
    void (*tbl[5])(sprite_status *) = {&gandair3_init, &gandair3_wait,
                                       &gandair3_fire, &gandair3_end,
                                       &gandair3_wait2};
    Uint16 cal_x;

    tbl[corewk->r_no0 / 2](corewk);
    actionsub(corewk);
    cal_x = (trap3_get_work(corewk)->origin_x & 65408) -
            ((Uint16)(scra_h_posit.w.h - 128) & 65408);

    if (cal_x > 640)
        tr3_fout(corewk);
}

void gandair3_ridechk(sprite_status *floorwk) {
    ride_on_chk(floorwk, &actwk[0]);
}

void gandair3_init(sprite_status *corewk) {
    corewk->actflg |= 4;

    corewk->sprpri = 3;
    corewk->patbase = gandair3pat;
    corewk->sprhsize = 16;
    corewk->sprvsize = 20;
    trap3_get_work(corewk)->origin_x = corewk->xposi.w.h;
    trap3_get_work(corewk)->base_x = corewk->yposi.w.h;
    corewk->patno = 1;
    corewk->r_no0 += 2;
}

void gandair3_wait(sprite_status *corewk) {
    Sint16 cal_x;

    if (trap3_get_work(corewk)->timer == 0) {

        if ((cal_x = corewk->xposi.w.h - actwk[0].xposi.w.h) < 0)
            cal_x = -cal_x;
        if (cal_x >= 120) {
            gandair3_ridechk(corewk);
            return;
        }

        trap3_get_work(corewk)->timer = 60;
    }

    if (--trap3_get_work(corewk)->timer != 0) {

        if (time_flag != 0)
            if (time_flag == 1 || generate_flag == 0)
                patchg(corewk, gandair3_pchg);
        gandair3_ridechk(corewk);
        return;
    }

    trap3_get_work(corewk)->timer = trap3_get_work(corewk)->travel = 0;
    corewk->patno = 1;
    corewk->r_no0 += 2;
}

void gandair3_fire(sprite_status *corewk) {
    if ((trap3_get_work(corewk)->travel += 8) >= 32)
        trap3_get_work(corewk)->travel = 32;
    corewk->yposi.w.h =
        trap3_get_work(corewk)->base_x - (Sint16)trap3_get_work(corewk)->travel;

    if (trap3_get_work(corewk)->travel != 32) {
        gandair3_ridechk(corewk);
        return;
    }

    gandair3_tamaset(corewk);
    if (ride_on_chk(corewk, &actwk[0]) == 0) {
        trap3_get_work(corewk)->timer = 8;
        corewk->r_no0 += 2;
        return;
    }

    actwk[0].yspeed.w = -1792;
    actwk[0].cddat |= 2;
    actwk[0].cddat &= 247;
    actwk[0].mstno.b.h = 16;
    corewk->cddat &= 247;
}

void gandair3_end(sprite_status *corewk) {
    if (trap3_get_work(corewk)->timer != 0) {
        --trap3_get_work(corewk)->timer;
        gandair3_ridechk(corewk);
        return;
    }
    if ((char)(trap3_get_work(corewk)->travel -= 4) < 0)
        trap3_get_work(corewk)->timer = 0;
    corewk->yposi.w.h =
        trap3_get_work(corewk)->base_x - (Sint16)trap3_get_work(corewk)->travel;

    if (trap3_get_work(corewk)->travel == 0) {
        trap3_get_work(corewk)->timer = 60;
        corewk->r_no0 += 2;
    }
    gandair3_ridechk(corewk);
}

void gandair3_wait2(sprite_status *corewk) {
    if (trap3_get_work(corewk)->timer == 0) {
        corewk->r_no0 = 2;
        return;
    }
    --trap3_get_work(corewk)->timer;
    gandair3_ridechk(corewk);
}

void gandair3_tamaset(sprite_status *corewk) {
    sprite_status *new_actwk;

    if (time_flag == 0)
        return;
    if (time_flag != 1 && generate_flag != 0)
        return;

    if (actwkchk(&new_actwk) == 0) {

        new_actwk->actno = 39;
        new_actwk->userflag.b.h = 2;
        new_actwk->yposi.w.h = corewk->yposi.w.h + 8;
        new_actwk->xposi.w.h = corewk->xposi.w.h - 24;
    }
    if (actwkchk(&new_actwk) == 0) {

        new_actwk->actno = 39;
        new_actwk->userflag.b.h = 2;
        new_actwk->yposi.w.h = corewk->yposi.w.h + 8;
        new_actwk->xposi.w.h = corewk->xposi.w.h + 24;
        trap3_get_work(new_actwk)->timer = 1;
    }
}

void gandair3_tama(sprite_status *bulletwk) {
    void (*tbl[2])(sprite_status *) = {&gandair3_tami, &gandair3_tamm};

    tbl[bulletwk->r_no0 / 2](bulletwk);
    actionsub(bulletwk);
    frameout_s(bulletwk);
}

void gandair3_tami(sprite_status *bulletwk) {
    bulletwk->actflg |= 4;

    bulletwk->sprpri = 4;
    bulletwk->colino = 152;
    bulletwk->patbase = gandair3pat;
    trap3_get_work(bulletwk)->bullet_origin_x = bulletwk->xposi.w.h;
    if (trap3_get_work(bulletwk)->timer != 0) {
        bulletwk->xspeed.w = 256;
        trap3_get_work(bulletwk)->bullet_acceleration = 16;
        bulletwk->mstno.b.h = 2;
    } else {
        bulletwk->xspeed.w = -256;
        trap3_get_work(bulletwk)->bullet_acceleration = -16;
        bulletwk->mstno.b.h = 1;
    }
    bulletwk->r_no0 += 2;
    patchg(bulletwk, gandair3_pchg);
    gandair3_tamm(bulletwk);
}

void gandair3_tamm(sprite_status *bulletwk) {
    Sint16 cal_x;

    bulletwk->xspeed.w += trap3_get_work(bulletwk)->bullet_acceleration;
    bulletwk->xposi.l += bulletwk->xspeed.w << 8;
    patchg(bulletwk, gandair3_pchg);

    cal_x = bulletwk->xposi.w.h - trap3_get_work(bulletwk)->bullet_origin_x;
    if (cal_x < 0)
        cal_x = -cal_x;
    if (cal_x < 64)
        return;

    bulletwk->colino = 0;
    if (trap3_get_work(bulletwk)->travel == 0) {
        bulletwk->mstno.b.h += 2;
        trap3_get_work(bulletwk)->travel = 30;
    }

    if (--trap3_get_work(bulletwk)->travel != 0)
        return;

    frameout(bulletwk);
}

static sprite_pattern drm00 = {1, {{-24, -16, 0, 302}}};
static sprite_pattern drm01 = {1, {{-24, -12, 0, 303}}};
static sprite_pattern drm02 = {1, {{-24, -8, 0, 304}}};
static sprite_pattern drm03 = {1, {{-24, -4, 0, 305}}};
static sprite_pattern drm04 = {1, {{-24, -4, 0, 306}}};
static sprite_pattern drm05 = {1, {{-24, -4, 0, 307}}};
static sprite_pattern drm06 = {1, {{-24, -4, 0, 308}}};
static sprite_pattern drm07 = {1, {{-24, -4, 0, 309}}};
static sprite_pattern drm08 = {1, {{-24, 0, 0, 310}}};
static sprite_pattern drm09 = {1, {{-24, 0, 0, 311}}};
static sprite_pattern drm0a = {1, {{-16, -20, 0, 0}}};
static sprite_pattern drm0b = {1, {{-24, -8, 0, 312}}};
sprite_pattern *drumr3pat[12] = {&drm00, &drm01, &drm02, &drm03,
                                 &drm04, &drm05, &drm06, &drm07,
                                 &drm08, &drm09, &drm0a, &drm0b};

void drumr3(sprite_status *drumwk) {
    Uint16 cal_x;
    void (*tbl[4])(sprite_status *) = {&drumr3_init, &drumr3_move1,
                                       &drumr3_move2, &drumr3_move3};
    tbl[drumwk->r_no0 / 2](drumwk);
    actionsub(drumwk);

    cal_x = (trap3_get_work(drumwk)->base_x & 65408) -
            ((Uint16)(scra_h_posit.w.h - 128) & 65408);

    if (cal_x > 640)
        tr3_fout(drumwk);
}

void drumr3_ridechk(sprite_status *drumwk) {
    if (!(drumwk->actflg & 128))
        return;

    if (time_flag == 0) {
        if (drumwk->patno == 2)
            drumwk->sprvsize = 8;
        if (drumwk->patno > 2 && drumwk->patno < 7)
            drumwk->sprvsize = 5;
    }
    ridechk(drumwk, &actwk[0]);
}

void drumr3_init(sprite_status *drumwk) {
    drumwk->actflg |= 4;

    drumwk->sprpri = 3;
    drumwk->patbase = drumr3pat;
    drumwk->sprhsize = 24;
    trap3_get_work(drumwk)->base_x = drumwk->xposi.w.h;
    trap3_get_work(drumwk)->base_y = drumwk->yposi.w.h;
    drumwk->r_no0 += 2;
    trap3_get_work(drumwk)->travel = drumwk->userflag.b.h;
    if (drumwk->userflag.b.h >= 2)
        drumwk->r_no0 += 2;
    if (drumwk->userflag.b.h >= 6)
        drumwk->r_no0 += 2;
}

void drumr3_move1(sprite_status *drumwk) {
    do {
        if (trap3_get_work(drumwk)->timer == 0) {
            ride_on_clr(drumwk, &actwk[0]);
            drumr3_cntset(drumwk);
            return;
        }
        if (time_flag == 0)
            return;
        if (gametimer.w % 10 != 0)
            return;

        drumr3_addspd(drumwk);
        if (--trap3_get_work(drumwk)->timer != 0)
            return;

    } while (++trap3_get_work(drumwk)->travel < 2);
    drumwk->r_no0 += 2;
}

void drumr3_move2(sprite_status *drumwk) {
    do {
        drumr3_ridechk(drumwk);
        if (trap3_get_work(drumwk)->timer == 0) {
            drumr3_cntset(drumwk);
            return;
        }

        if (time_flag == 0)
            return;
        if (gametimer.w % 10 != 0)
            return;

        drumr3_addspd(drumwk);
        if (--trap3_get_work(drumwk)->timer != 0)
            return;

    } while (++trap3_get_work(drumwk)->travel < 6);
    drumwk->r_no0 += 2;
}

void drumr3_move3(sprite_status *drumwk) {
    do {
        if (trap3_get_work(drumwk)->timer == 0) {
            ride_on_clr(drumwk, &actwk[0]);
            drumr3_cntset(drumwk);
            return;
        }
        if (time_flag == 0)
            return;
        if (gametimer.w % 10 != 0)
            return;

        drumr3_addspd(drumwk);
        if (--trap3_get_work(drumwk)->timer != 0)
            return;
    } while (++trap3_get_work(drumwk)->travel < 18);
    trap3_get_work(drumwk)->timer = trap3_get_work(drumwk)->travel = 0;
    drumwk->r_no0 = 2;
}

void drumr3_addspd(sprite_status *drumwk) {
    drumwk->yspeed.w = trap3_get_work(drumwk)->origin_x;
    drumwk->yposi.l += drumwk->yspeed.w << 8;
}

void drumr3_cntset(sprite_status *drumwk) {

    Uint8 drumr3_mvtbl[54] = {
        1,   8, 0,   5,   8,  1,   6,   12, 2,   8,   11, 3,   8, 11,
        4,   8, 11,  5,   6,  11,  6,   5,  8,   7,   1,  8,   8, 255,
        4,   9, 251, 4,   10, 250, 8,   10, 248, 8,   10, 248, 8, 10,
        248, 8, 10,  250, 8,  10,  251, 8,  10,  255, 4,  11};

    trap3_get_work(drumwk)->drum_speed_high =
        drumr3_mvtbl[trap3_get_work(drumwk)->travel * 3];

    drumwk->sprvsize =
        drumr3_mvtbl[trap3_get_work(drumwk)->travel * 3 + 1];
    drumwk->patno = drumr3_mvtbl[trap3_get_work(drumwk)->travel * 3 + 2];
    trap3_get_work(drumwk)->timer = 4;
    drumr3_priset(drumwk);
}

void drumr3_priset(sprite_status *drumwk) {
    Uint8 tbl[12] = {5, 4, 3, 2, 2, 2, 3, 4, 5, 6, 6, 6};

    drumwk->sprpri = tbl[drumwk->patno];
}

void tr3_fout(sprite_status *thingwk) {
    Uint16 flagwork_no;

    if ((flagwork_no = thingwk->cdsts) != 0) {
        flagwork_no = (Uint16)time_flag + flagwork_no * 3;
        flagwork[flagwork_no] &= 127;
    }

    frameout(thingwk);
}
