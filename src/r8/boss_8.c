#include "../equ.h"
#include "boss_8.h"
#include "../action.h"
#include "../actset.h"
#include "../etc.h"
#include "../io.h"
#include "../loader2.h"
#include "../playsub.h"
#include "../player_work.h"
#include "../score.h"

extern void colchg_boss8(Uint8 *a3, Uint8 *a4);
extern void fadein_boss8(void);
extern void colorset2(Sint32 ColorNo);

typedef struct {
    Sint16 xOffs;
    Sint16 yOffs;
    Sint16 xDst;
    Sint16 yDst;
    Sint16 ColiNo;
} hane_offs;

typedef struct {
    Uint8 r_no;
    Uint8 Dummy;
    Sint16 param;
} egg8act;

typedef struct {
    Sint32 X_Speed;
    Sint32 Y_Speed;
    char Roll_Speed;
    char Dummy;
} egg8spd;

typedef struct {
    union {
        Sint16 timer;
        Uint16 timer_u;
        struct {
            Uint8 timer_low;
            Uint8 angle;
        };
    };
    Uint8 flags;
    Uint8 step;
    Sint16 parent_index;
    Sint16 target_x;
    Sint16 target_y;
    union {
        Sint32 x_velocity;
        struct {
            Uint8 hane_mode;
            Uint8 hane_base_patno;
            Uint8 hane_target;
            Uint8 roll_speed;
        };
    };
    union {
        Sint16 target_pos;
        struct {
            Uint8 wait_time;
            Uint8 spin_frame;
        };
    };
    union {
        Sint16 hane_count_word;
        struct {
            Uint8 hane_count;
            Uint8 action_index;
        };
    };
    union {
        Sint16 flash_word;
        struct {
            Uint8 hit_flash;
            Uint8 hit_timer;
        };
    };
    union {
        Sint16 color_word;
        struct {
            Uint8 color_a;
            Uint8 color_b;
        };
    };
} boss8_work;

static boss8_work *boss8_get_work(sprite_status *pActwk) {
    return (boss8_work *)pActwk->actfree;
}

static void egg8_ini(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_scrset(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_open1(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_wait(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_move_r(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_move_l(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_move_d(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_move_u(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_move_c(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_move_escu(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_hane_roll(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_hane_stop(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_hane_chg1(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_hane_chg2(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_hane_chg3(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_hane_fire(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_hane_reset(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_spin_d(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_spin_l(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_spin_r(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_target(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8target_ini(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8target_roll(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8target_move(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_targetreset(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_tobi_d(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_tobi(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_tobi_u(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_dead(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_esc(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_esc2(sprite_status *pActwk, sprite_status *pMecawk);
static void egg8_warai_chk(sprite_status *pActwk);
static void egg8_colichk(sprite_status *pActwk, sprite_status *pPlayerwk,
                         sprite_status *pMecawk);
static void next_action(sprite_status *pActwk);
static void egg8meca_ini(sprite_status *pActwk, sprite_status *pEggwk);
static void egg8meca_normal(sprite_status *pActwk, sprite_status *pEggwk);
static void egg8meca_spin(sprite_status *pActwk, sprite_status *pEggwk);
static void egg8meca_chg1(sprite_status *pActwk, sprite_status *pEggwk);
static void egg8meca_chg2(sprite_status *pActwk, sprite_status *pEggwk);
static void egg8meca_chg3(sprite_status *pActwk, sprite_status *pEggwk);
static void egg8meca_fire(sprite_status *pActwk, sprite_status *pEggwk);
static void egg8meca_hane_reset(sprite_status *pActwk, sprite_status *pEggwk);
static void egg8meca_dead(sprite_status *pActwk, sprite_status *pEggwk);
static void egg8meca_tobi(sprite_status *pActwk, sprite_status *pEggwk);
static void hane_ctrl(sprite_status *pActwk);
static void hane_stop(sprite_status *pActwk);
static void hane_roll(sprite_status *pActwk);
static void futa_roll(sprite_status *pActwk);
static Sint32 egg8hane_ini(sprite_status *pActwk, sprite_status *pMecawk,
                           sprite_status *pEggwk);
static Sint32 egg8hane_demo(sprite_status *pActwk, sprite_status *pMecawk,
                            sprite_status *pEggwk);
static Sint32 egg8hane_normal(sprite_status *pActwk, sprite_status *pMecawk,
                              sprite_status *pEggwk);
static Sint32 egg8hane_fire1(sprite_status *pActwk, sprite_status *pMecawk,
                             sprite_status *pEggwk);
static Sint32 egg8hane_wait(sprite_status *pActwk, sprite_status *pMecawk,
                            sprite_status *pEggwk);
static Sint32 egg8hane_kill(sprite_status *pActwk, sprite_status *pMecawk,
                            sprite_status *pEggwk);
static void hane_no_reset(sprite_status *pActwk, sprite_status *pMecawk,
                          sprite_status *pEggwk);
static void egg8hane_posiset(sprite_status *pActwk, sprite_status *pMecawk);
static void hane_offs_set(sprite_status *pActwk, sprite_status *pMecawk);
static void hane_rol_l(sprite_status *pActwk);
static void hane_rol_r(sprite_status *pActwk);
static void rol_sound(sprite_status *pActwk);
static Uint32 egg8hibana_ini(sprite_status *pActwk);
static Uint32 egg8hibana_1(sprite_status *pActwk);
static void make_meca(sprite_status *pActwk);
static void make_hane(sprite_status *pActwk);
static void make_hibana(sprite_status *pActwk);
static void egg8_spd_set(sprite_status *pActwk, sprite_status *pMecawk);
static void tobi_set(sprite_status *pActwk);
static void bom_set(sprite_status *pActwk);

static Uint8 egg8_pchg0[3] = {255, 2, 255};
static Uint8 egg8_pchg1[6] = {3, 5, 6, 5, 7, 255};
static Uint8 egg8_pchg2[6] = {15, 8, 0, 1, 2, 255};
static Uint8 egg8_pchg3[3] = {255, 4, 255};
static Uint8 egg8_pchg4[4] = {7, 2, 3, 255};
static Uint8 egg8_pchg5[3] = {255, 8, 255};
static Uint8 *egg8_pchg[6] = {egg8_pchg0, egg8_pchg1, egg8_pchg2,
                              egg8_pchg3, egg8_pchg4, egg8_pchg5};
static sprite_pattern egg8_pat0 = {2, {{-12, -28, 0, 451}, {-24, -28, 0, 443}}};
static sprite_pattern egg8_pat1 = {2, {{-12, -28, 0, 451}, {-24, -28, 0, 444}}};
static sprite_pattern egg8_pat2 = {2, {{-12, -28, 0, 451}, {-24, -28, 0, 445}}};
static sprite_pattern egg8_pat3 = {2, {{-12, -28, 0, 451}, {-24, -28, 0, 446}}};
static sprite_pattern egg8_pat4 = {1, {{-24, -28, 0, 447}}};
static sprite_pattern egg8_pat5 = {2, {{-12, -28, 0, 451}, {-24, -28, 0, 448}}};
static sprite_pattern egg8_pat6 = {2, {{-12, -28, 0, 451}, {-24, -44, 0, 449}}};
static sprite_pattern egg8_pat7 = {2, {{-12, -28, 0, 451}, {-24, -44, 0, 450}}};
static sprite_pattern egg8_pat8 = {1, {{-12, -28, 0, 451}}};
sprite_pattern *egg8_pat[9] = {&egg8_pat0, &egg8_pat1, &egg8_pat2,
                               &egg8_pat3, &egg8_pat4, &egg8_pat5,
                               &egg8_pat6, &egg8_pat7, &egg8_pat8};
static void (*egg8_act_tbl[28])(sprite_status *, sprite_status *) = {
    &egg8_ini,         &egg8_scrset,    &egg8_open1,     &egg8_wait,
    &egg8_move_r,      &egg8_move_l,    &egg8_move_d,    &egg8_move_u,
    &egg8_move_c,      &egg8_move_escu, &egg8_hane_roll, &egg8_hane_stop,
    &egg8_hane_chg1,   &egg8_hane_chg2, &egg8_hane_chg3, &egg8_hane_fire,
    &egg8_hane_reset,  &egg8_spin_r,    &egg8_spin_l,    &egg8_target,
    &egg8_targetreset, &egg8_spin_d,    &egg8_tobi_d,    &egg8_tobi,
    &egg8_tobi_u,      &egg8_dead,      &egg8_esc,       &egg8_esc2};

void egg8(sprite_status *pActwk) {
    sprite_status *pPlayerwk, *pMecawk;
    Sint16 idx;

    pPlayerwk = &actwk[0];

    if ((idx = boss8_get_work(pActwk)->parent_index) != 0) {
        pMecawk = &actwk[idx];
        boss8_get_work(pMecawk)->flags &= 159;
    } /* else {
        pMecawk = NULL;
	} */

    egg8_warai_chk(pActwk);

    if (boss8_get_work(pActwk)->hit_flash && --boss8_get_work(pActwk)->hit_flash == 0) {
        pActwk->mstno.b.h = 0;
    }

    egg8_colichk(pActwk, pPlayerwk, pMecawk);

    if (boss8_get_work(pActwk)->flags & 1) {
        next_action(pActwk);
    }
    boss8_get_work(pActwk)->flags &= 254;

    egg8_act_tbl[pActwk->r_no0 / 2](pActwk, pMecawk);
    patchg(pActwk, egg8_pchg);

    colchg_boss8(&boss8_get_work(pActwk)->hit_timer, &boss8_get_work(pActwk)->color_a);

    pMecawk = &actwk[boss8_get_work(pActwk)->parent_index];

    if (!(boss8_get_work(pMecawk)->hit_flash / 2 & 1)) {
        actionsub(pActwk);
    }
}

static void egg8_ini(sprite_status *pActwk, sprite_status *pMecawk) {
    pActwk->r_no0 = 2;
    pActwk->actflg = 4;
    pActwk->sprpri = 5;
    pActwk->sprhsize = 24;
    pActwk->sprvsize = 16;
    pActwk->sproffset = 990;
    pActwk->patbase = egg8_pat;

    boss8_get_work(pActwk)->hane_count = 4;
    pActwk->yposi.w.h -= 256;
}

static void egg8_scrset(sprite_status *pActwk, sprite_status *pMecawk) {
    Sint16 limwk;

    scralim_right = scralim_n_right = 3680;
    if ((limwk = actwk[0].xposi.w.h - 160) >= scralim_left) {
        if (actwk[0].xposi.w.h >= 3840) {

            bossflag = bossstart = 8;
            make_meca(pActwk);

            pMecawk = &actwk[boss8_get_work(pActwk)->parent_index];
            egg8_spd_set(pActwk, pMecawk);

            boss8_get_work(pActwk)->flags &= 253;
            pActwk->r_no0 = 4;

            limwk = 3680;
        }

        scralim_left = scralim_n_left = limwk;
    }
}

static void egg8_open1(sprite_status *pActwk, sprite_status *pMecawk) {
    switch (boss8_get_work(pActwk)->step) {
    case 1:
        if (++boss8_get_work(pActwk)->timer == 240) {

            pActwk->yposi.w.h += 256;

            sub_sync(104);
            bossstart |= 128;

            boss8_get_work(pActwk)->timer_low = 0;
            boss8_get_work(pActwk)->angle = 0;
            ++boss8_get_work(pActwk)->step;
            pActwk->mstno.b.h = 2;
        }

        break;

    case 2:
        if ((Uint16)boss8_get_work(pActwk)->timer_low % 3 == 0) {
            fadein_boss8();
        }

        ++boss8_get_work(pActwk)->timer_low;
        if (boss8_get_work(pActwk)->timer_low == 63) {

            ++boss8_get_work(pActwk)->step;
            pActwk->mstno.b.h = 0;
            boss8_get_work(pActwk)->timer_low = 0;
        }

        break;

    case 3:
        if (++boss8_get_work(pActwk)->timer_low == 40) {

            boss8_get_work(pActwk)->flags |= 1;
            boss8_get_work(pActwk)->timer_low = 0;

            boss8_get_work(pActwk)->flags |= 8;
            pActwk->colino = 50;
            pActwk->colicnt = 2;
            pMecawk->colino = 51;
            pMecawk->colicnt = 2;
        }

        break;

    default:
        pActwk->mstno.b.h = 5;
        ++boss8_get_work(pActwk)->step;

        colorset2(7);
        colorset2(8);
        colorset2(9);
        break;
    }
}

static void egg8_wait(sprite_status *pActwk, sprite_status *pMecawk) {
    Uint8 wk;

    ++boss8_get_work(pActwk)->timer_low;
    wk = boss8_get_work(pActwk)->wait_time;
    if (boss8_get_work(pActwk)->timer_low == wk) {

        boss8_get_work(pActwk)->timer_low = 0;
        boss8_get_work(pActwk)->flags |= 1;
    }
}

static void egg8_move_r(sprite_status *pActwk, sprite_status *pMecawk) {
    pActwk->xposi.l += boss8_get_work(pActwk)->x_velocity;
    if (pActwk->xposi.w.h >= boss8_get_work(pActwk)->target_pos) {

        pActwk->xposi.w.h = boss8_get_work(pActwk)->target_pos;
        boss8_get_work(pActwk)->flags |= 1;
    }
}

static void egg8_move_l(sprite_status *pActwk, sprite_status *pMecawk) {
    pActwk->xposi.l -= boss8_get_work(pActwk)->x_velocity;
    if (pActwk->xposi.w.h <= boss8_get_work(pActwk)->target_pos) {

        pActwk->xposi.w.h = boss8_get_work(pActwk)->target_pos;
        boss8_get_work(pActwk)->flags |= 1;
    }
}

static void egg8_move_d(sprite_status *pActwk, sprite_status *pMecawk) {
    pActwk->yposi.l += sprite_status_get_xspeed_yspeed(pActwk);
    if (pActwk->yposi.w.h >= boss8_get_work(pActwk)->target_pos) {

        pActwk->yposi.w.h = boss8_get_work(pActwk)->target_pos;
        boss8_get_work(pActwk)->flags |= 1;
    }
}

static void egg8_move_u(sprite_status *pActwk, sprite_status *pMecawk) {
    pActwk->yposi.l -= sprite_status_get_xspeed_yspeed(pActwk);
    if (pActwk->yposi.w.h <= boss8_get_work(pActwk)->target_pos) {

        pActwk->yposi.w.h = boss8_get_work(pActwk)->target_pos;
        boss8_get_work(pActwk)->flags |= 1;
    }
}

static void egg8_move_c(sprite_status *pActwk, sprite_status *pMecawk) {
    Sint16 xOfs;
    char sw;

    sw = 110;

    if (!boss8_get_work(pActwk)->step) {

        boss8_get_work(pActwk)->step = 2;

        if (pActwk->xposi.w.h <= 3840) {

            sw = 114;
        } else {
            sw = 108;
        }
    } else {
        if (!(boss8_get_work(pActwk)->step & 1)) {

            pActwk->xposi.l -= boss8_get_work(pActwk)->x_velocity;

            if ((xOfs = boss8_get_work(pActwk)->target_x) < pActwk->xposi.w.h)
                return;

        } else {
            pActwk->xposi.l += boss8_get_work(pActwk)->x_velocity;

            if ((xOfs = boss8_get_work(pActwk)->target_x) > pActwk->xposi.w.h)
                return;
        }

        pActwk->xposi.w.h = xOfs;

        if (!boss8_get_work(pActwk)->target_pos) {

            boss8_get_work(pActwk)->flags |= 1;
        } else if (!(--boss8_get_work(pActwk)->target_pos)) {
            sw = 99;
        } else {
            if (!(boss8_get_work(pActwk)->step & 1)) {
                sw = 114;
            } else {
                sw = 108;
            }

            boss8_get_work(pActwk)->step ^= 1;
        }
    }

    switch (sw) {

    case 108:
        boss8_get_work(pActwk)->step &= 254;
        boss8_get_work(pActwk)->target_x = 3716;
        break;

    case 114:
        boss8_get_work(pActwk)->step |= 1;
        boss8_get_work(pActwk)->target_x = 3964;
        break;

    case 99:
        boss8_get_work(pActwk)->step ^= 1;
        boss8_get_work(pActwk)->target_x = 3840;
        break;
    }
}

static void egg8_move_escu(sprite_status *pActwk, sprite_status *pMecawk) {
    if (!boss8_get_work(pActwk)->step) {
        egg8_hane_chg3(pActwk, pMecawk);
        boss8_get_work(pActwk)->flags &= 254;
        boss8_get_work(pActwk)->step = 1;
    }

    pActwk->yposi.l -= 81920;
    if (pActwk->yposi.w.h <= boss8_get_work(pActwk)->target_pos) {

        pActwk->yposi.w.h = boss8_get_work(pActwk)->target_pos;
        boss8_get_work(pActwk)->flags |= 1;
    }
}

static void egg8_hane_roll(sprite_status *pActwk, sprite_status *pMecawk) {
    if (!(boss8_get_work(pMecawk)->hane_count & 64)) {
        boss8_get_work(pMecawk)->flags |= 16;
        boss8_get_work(pMecawk)->step = 0;
        boss8_get_work(pMecawk)->angle = 0;
    }

    boss8_get_work(pActwk)->flags |= 1;
}

static void egg8_hane_stop(sprite_status *pActwk, sprite_status *pMecawk) {
    if (!boss8_get_work(pActwk)->step) {

        ++boss8_get_work(pActwk)->step;
        boss8_get_work(pMecawk)->flags |= 16;
        boss8_get_work(pMecawk)->hane_count &= 127;
        boss8_get_work(pMecawk)->step = 0;
        boss8_get_work(pMecawk)->angle = 0;

        if (boss8_get_work(pActwk)->hane_count == 2) {

            boss8_get_work(pMecawk)->hane_target = 8;
        } else {
            boss8_get_work(pMecawk)->hane_target = 4;
        }
    }

    if (boss8_get_work(pMecawk)->hane_count & 128) {

        boss8_get_work(pActwk)->flags |= 1;
    }
}

static void egg8_hane_chg1(sprite_status *pActwk, sprite_status *pMecawk) {
    boss8_get_work(pMecawk)->step = 0;
    boss8_get_work(pMecawk)->timer_low = 0;
    boss8_get_work(pMecawk)->angle = 0;
    boss8_get_work(pMecawk)->hane_count |= 160;
    boss8_get_work(pMecawk)->hane_count &= 191;
    pMecawk->r_no0 = 4;

    boss8_get_work(pActwk)->timer_low = 0;
    boss8_get_work(pActwk)->angle = 0;
    boss8_get_work(pActwk)->flags |= 1;
}

static void egg8_hane_chg2(sprite_status *pActwk, sprite_status *pMecawk) {
    soundset(201);

    boss8_get_work(pMecawk)->step = 0;
    boss8_get_work(pMecawk)->timer_low = 0;
    boss8_get_work(pMecawk)->angle = 0;
    pMecawk->r_no0 = 6;

    boss8_get_work(pActwk)->flags |= 1;
}

static void egg8_hane_chg3(sprite_status *pActwk, sprite_status *pMecawk) {
    boss8_get_work(pMecawk)->step = 0;
    boss8_get_work(pMecawk)->timer_low = 0;
    boss8_get_work(pMecawk)->angle = 0;
    pMecawk->r_no0 = 8;

    boss8_get_work(pActwk)->flags |= 1;
}

static void egg8_hane_fire(sprite_status *pActwk, sprite_status *pMecawk) {
    switch (boss8_get_work(pActwk)->step) {

    case 1:
        boss8_get_work(pActwk)->target_pos = 316;
        egg8_move_u(pActwk, pMecawk);

        if (boss8_get_work(pActwk)->flags & 1) {

            ++boss8_get_work(pActwk)->step;
        }

        boss8_get_work(pActwk)->flags &= 254;
        break;

    case 2:
        if (boss8_get_work(pMecawk)->hit_timer == boss8_get_work(pActwk)->hane_count * 2) {

            boss8_get_work(pActwk)->flags |= 1;
        }
        break;

    default:
        ++boss8_get_work(pActwk)->step;
        boss8_get_work(pMecawk)->step = 0;
        boss8_get_work(pMecawk)->timer_low = 0;
        boss8_get_work(pMecawk)->angle = 0;
        pMecawk->r_no0 = 10;
        boss8_get_work(pMecawk)->hit_timer = 0;
        boss8_get_work(pMecawk)->color_a = 0;
        break;
    }
}

static void egg8_hane_reset(sprite_status *pActwk, sprite_status *pMecawk) {
    boss8_get_work(pMecawk)->step = 0;
    boss8_get_work(pMecawk)->timer_low = 0;
    boss8_get_work(pMecawk)->angle = 0;
    pMecawk->r_no0 = 12;

    boss8_get_work(pActwk)->flags |= 1;
}

static void egg8_spin_d(sprite_status *pActwk, sprite_status *pMecawk) {
    Sint32 tateini = 0;

    if (!boss8_get_work(pActwk)->step) {

        if (boss8_get_work(pActwk)->hane_count == 4) {

            boss8_get_work(pMecawk)->hane_target = 8;

            tateini = 1;
        } else {
            if (actwk[0].xposi.w.h < pActwk->xposi.w.h) {

                boss8_get_work(pMecawk)->hane_target = 8;

                boss8_get_work(pActwk)->target_pos = 3964;
                boss8_get_work(pActwk)->step = 1;
            } else {
                if (boss8_get_work(pActwk)->hane_count != 3) {

                    boss8_get_work(pMecawk)->hane_target = 4;
                } else {
                    boss8_get_work(pMecawk)->hane_target = 0;
                }

                boss8_get_work(pActwk)->target_pos = 3716;
                boss8_get_work(pActwk)->step = 2;
            }
        }
    }

    if (boss8_get_work(pActwk)->step == 3) {

        tateini = 2;
    } else if (boss8_get_work(pActwk)->step) {
        if (boss8_get_work(pActwk)->step == 1) {

            egg8_move_r(pActwk, pMecawk);
        } else {
            egg8_move_l(pActwk, pMecawk);
        }

        if (boss8_get_work(pActwk)->flags & 1) {
            tateini = 1;
        }

        boss8_get_work(pActwk)->flags &= 254;
    }

    switch (tateini) {

    case 1:
        boss8_get_work(pMecawk)->flags |= 16;
        boss8_get_work(pMecawk)->hane_count &= 127;
        boss8_get_work(pMecawk)->step = 0;
        boss8_get_work(pMecawk)->angle = 0;

        boss8_get_work(pActwk)->target_pos = 416;
        boss8_get_work(pActwk)->step = 3;

    case 2:
        egg8_move_d(pActwk, pMecawk);

        if (boss8_get_work(pActwk)->flags & 1) {

            boss8_get_work(pActwk)->step = 0;
        }
        break;
    }
}

static void egg8_spin_l(sprite_status *pActwk, sprite_status *pMecawk) {
    egg8_spin_r(pActwk, pMecawk);
}

static Sint16 spin_tbl1[32] = {26, -4, 26, 6,  26, -6, 26, 4,  26, -4, 26,
                               6,  26, -6, 26, 4,  26, -4, 26, 6,  26, -6,
                               26, 4,  26, -4, 26, 6,  26, -6, 26, 4};
static Sint16 spin_tbl3[32] = {26, -4, 26, 6,  26, -6, 26, 4,  26, -4, 26,
                               6,  26, -6, 26, 4,  26, -4, 26, 6,  26, -6,
                               26, 4,  26, -4, 26, 6,  21, 15, 26, 22};
static Sint16 *spin_tbl[5] = {0, spin_tbl1, spin_tbl1, spin_tbl3, spin_tbl1};

static void egg8_spin_r(sprite_status *pActwk, sprite_status *pMecawk) {
    Sint16 *pSpinTbl, xwk, ywk;
    Uint8 hanecntwk, para0wk;

    if ((char)boss8_get_work(pActwk)->step >= 5) {

        pMecawk->r_no0 = 2;
        boss8_get_work(pMecawk)->step = 0;
        boss8_get_work(pMecawk)->angle = 0;

        boss8_get_work(pActwk)->step = 0;
        boss8_get_work(pActwk)->timer_low = 0;
        boss8_get_work(pActwk)->angle = 0;
        boss8_get_work(pActwk)->flags |= 1;
    } else if (boss8_get_work(pActwk)->step & 128) {

        if (++boss8_get_work(pActwk)->timer_low == 60) {
            boss8_get_work(pActwk)->timer_low = 0;
            boss8_get_work(pActwk)->step &= 127;
        }

    } else {
        if (!boss8_get_work(pActwk)->step) {

            boss8_get_work(pActwk)->timer_low = 0;
            boss8_get_work(pActwk)->spin_frame = 0;

            if ((boss8_get_work(pActwk)->hane_count == 4 &&
                 actwk[0].xposi.w.h >= pActwk->xposi.w.h) ||
                (boss8_get_work(pActwk)->hane_count != 4 && pActwk->xposi.w.h < 3840)) {
                boss8_get_work(pActwk)->angle = 0;
                boss8_get_work(pActwk)->step = 2;
            } else {
                boss8_get_work(pActwk)->angle = 7;
                boss8_get_work(pActwk)->step = 1;
            }

            pMecawk->r_no0 = 14;
            boss8_get_work(pMecawk)->step = 0;
            boss8_get_work(pMecawk)->angle = 0;
        }

        if (++boss8_get_work(pActwk)->timer_low == 20) {

            ++boss8_get_work(pActwk)->spin_frame;

            if (!(boss8_get_work(pActwk)->spin_frame & 1)) {
                sub_sync(126);
            }

            boss8_get_work(pActwk)->timer_low = 0;

            pSpinTbl = spin_tbl[boss8_get_work(pActwk)->hane_count];
            pSpinTbl += (boss8_get_work(pActwk)->angle & 15) * 2;

            xwk = *pSpinTbl++;
            ywk = *pSpinTbl++;

            if (boss8_get_work(pActwk)->step & 1) {

                boss8_get_work(pMecawk)->flags |= 64;
                xwk *= -1;
                ywk *= -1;
            } else {
                boss8_get_work(pMecawk)->flags |= 32;
            }

            pActwk->xposi.w.h += xwk;
            pActwk->yposi.w.h += ywk;

            hanecntwk = boss8_get_work(pActwk)->hane_count;
            para0wk = boss8_get_work(pActwk)->spin_frame;

            if ((hanecntwk == 4 && (para0wk == 4 || para0wk == 12)) ||
                (hanecntwk != 4 && (para0wk == 4 || para0wk == 8))) {
                if ((char)boss8_get_work(pActwk)->step >= 3) {

                    boss8_get_work(pActwk)->step = 133;

                    if (boss8_get_work(pActwk)->step & 1) {

                        --boss8_get_work(pActwk)->angle;
                    } else {
                        ++boss8_get_work(pActwk)->angle;
                    }
                } else if (boss8_get_work(pActwk)->step == 1) {

                    boss8_get_work(pActwk)->step = 132;
                } else {
                    boss8_get_work(pActwk)->step = 131;
                }
            } else {
                if (boss8_get_work(pActwk)->step & 1) {

                    --boss8_get_work(pActwk)->angle;
                } else {
                    ++boss8_get_work(pActwk)->angle;
                }
            }
        }
    }
}

static void egg8_target(sprite_status *pActwk, sprite_status *pMecawk) {
    if (!boss8_get_work(pActwk)->step) {

        egg8target_ini(pActwk, pMecawk);
    }

    if (!(boss8_get_work(pActwk)->step & 2)) {

        egg8target_roll(pActwk, pMecawk);
    }

    if (!(boss8_get_work(pActwk)->step & 4)) {

        egg8target_move(pActwk, pMecawk);
    }

    if (boss8_get_work(pActwk)->step == 7) {

        boss8_get_work(pActwk)->flags |= 1;
    }
}

static void egg8target_ini(sprite_status *pActwk, sprite_status *pMecawk) {
    Sint16 xPos, xSav;
    int_union xwk, ywk;

    boss8_get_work(pActwk)->step |= 1;
    xwk.w.l = xSav = xPos = actwk[0].xposi.w.h - pActwk->xposi.w.h;

    if (xSav < 0) {
        xPos *= -1;
    }

    boss8_get_work(pMecawk)->flags |= 16;
    boss8_get_work(pMecawk)->hane_count &= 127;
    boss8_get_work(pMecawk)->step = 0;
    boss8_get_work(pMecawk)->angle = 0;

    if (xPos < 64) {

        xwk.l = xwk.w.l;
        xwk.l = xwk.l * 16 / 90 * 4096;
        boss8_get_work(pActwk)->x_velocity = xwk.l;

        ywk.l = (380 - pActwk->yposi.w.h) * 16 / 90 * 4096;
        sprite_status_set_xspeed_yspeed(pActwk, ywk.l);

        if (boss8_get_work(pActwk)->hane_count != 2) {

            boss8_get_work(pMecawk)->hane_target = 4;
        } else {
            boss8_get_work(pMecawk)->hane_target = 6;
        }
    } else {
        if (xSav >= 0) {

            xwk.w.l -= 16;
        } else {
            xwk.w.l += 16;
        }

        xwk.l = xwk.w.l;
        xwk.l = xwk.l * 16 / 90 * 4096;
        boss8_get_work(pActwk)->x_velocity = xwk.l;

        ywk.l = (394 - pActwk->yposi.w.h) * 16 / 90 * 4096;
        sprite_status_set_xspeed_yspeed(pActwk, ywk.l);

        if (boss8_get_work(pActwk)->hane_count != 2 && xSav >= 0) {
            boss8_get_work(pMecawk)->hane_target = 2;
        } else {
            boss8_get_work(pMecawk)->hane_target = 6;
        }
    }
}

static void egg8target_roll(sprite_status *pActwk, sprite_status *pMecawk) {
    if (boss8_get_work(pMecawk)->hane_count & 128) {

        boss8_get_work(pActwk)->step |= 2;
    }
}

static void egg8target_move(sprite_status *pActwk, sprite_status *pMecawk) {
    Sint16 ywk;

    if (!(boss8_get_work(pMecawk)->hane_target & 3)) {
        ywk = 380;
    } else {
        ywk = 394;
    }

    pActwk->xposi.l += boss8_get_work(pActwk)->x_velocity;
    pActwk->yposi.l += sprite_status_get_xspeed_yspeed(pActwk);

    if (pActwk->yposi.w.h >= ywk) {

        pActwk->yposi.w.h = ywk;
        boss8_get_work(pActwk)->x_velocity *= -1;
        sprite_status_set_xspeed_yspeed(
            pActwk, -sprite_status_get_xspeed_yspeed(pActwk));
        boss8_get_work(pActwk)->step |= 4;
    }
}

static void egg8_targetreset(sprite_status *pActwk, sprite_status *pMecawk) {
    pActwk->xposi.l += boss8_get_work(pActwk)->x_velocity;
    pActwk->yposi.l += sprite_status_get_xspeed_yspeed(pActwk);

    if (pActwk->yposi.w.h <= 316) {

        pActwk->yposi.w.h = 316;
        boss8_get_work(pActwk)->step = 0;
        boss8_get_work(pActwk)->flags |= 1;

        egg8_spd_set(pActwk, pMecawk);
    }
}

static void egg8_tobi_d(sprite_status *pActwk, sprite_status *pMecawk) {
    if (boss8_get_work(pActwk)->step == 0 || boss8_get_work(pActwk)->step > 5) {

        if (pActwk->xposi.w.h > actwk[0].xposi.w.h) {

            boss8_get_work(pActwk)->target_pos = 3964;
            boss8_get_work(pActwk)->step = 1;
        } else {
            boss8_get_work(pActwk)->target_pos = 3716;
            boss8_get_work(pActwk)->step = 2;
        }
    }

    if (boss8_get_work(pActwk)->step == 1) {

        egg8_move_r(pActwk, pMecawk);
    } else if (boss8_get_work(pActwk)->step == 2) {

        egg8_move_l(pActwk, pMecawk);
    }

    if (boss8_get_work(pActwk)->step == 1 || boss8_get_work(pActwk)->step == 2) {

        if (!(boss8_get_work(pActwk)->flags & 1))
            return;

        boss8_get_work(pActwk)->flags &= 254;

        boss8_get_work(pMecawk)->flags |= 16;
        boss8_get_work(pMecawk)->hane_count &= 127;
        boss8_get_work(pMecawk)->hane_target = 8;
        boss8_get_work(pMecawk)->step = 0;
        boss8_get_work(pMecawk)->angle = 0;

        boss8_get_work(pActwk)->step = 3;
    }

    if (boss8_get_work(pActwk)->step == 3) {

        if (!(boss8_get_work(pMecawk)->hane_count & 128))
            return;

        boss8_get_work(pActwk)->target_pos = 312;

        boss8_get_work(pActwk)->step = 4;
    }

    if (boss8_get_work(pActwk)->step == 4 || boss8_get_work(pActwk)->step > 5) {

        egg8_move_d(pActwk, pMecawk);

        if (!(boss8_get_work(pActwk)->flags & 1))
            return;

        boss8_get_work(pActwk)->flags &= 254;

        boss8_get_work(pActwk)->step = 5;
        boss8_get_work(pActwk)->target_x = 0;
        boss8_get_work(pActwk)->target_y = -104;
        boss8_get_work(pActwk)->timer = 16384;
    }

    if (boss8_get_work(pActwk)->step == 5) {

        tobi_set(pActwk);
        pActwk->xposi.w.h -= boss8_get_work(pActwk)->target_x;
        boss8_get_work(pActwk)->target_x = 0;

        if (boss8_get_work(pActwk)->timer == -32768) {

            boss8_get_work(pActwk)->step = 0;
            boss8_get_work(pActwk)->flags |= 1;

            soundset(180);
        }
    }
}

static void egg8_tobi(sprite_status *pActwk, sprite_status *pMecawk) {
    Sint16 xwk;

    if ((char)boss8_get_work(pActwk)->step < 5) {
        if (!boss8_get_work(pActwk)->step) {

            boss8_get_work(pActwk)->target_y = 0;
            boss8_get_work(pActwk)->timer = 0;

            if (pActwk->xposi.w.h < 3840) {

                boss8_get_work(pActwk)->target_x = -41;
                boss8_get_work(pActwk)->step = 2;
            } else {
                boss8_get_work(pActwk)->target_x = 41;
                boss8_get_work(pActwk)->step = 1;
            }

            pMecawk->r_no0 = 18;
            boss8_get_work(pMecawk)->hane_mode = 2;
            boss8_get_work(pMecawk)->step = 0;
            boss8_get_work(pMecawk)->angle = 0;
        }

        tobi_set(pActwk);

        if (boss8_get_work(pActwk)->timer != -32768)
            return;

        soundset(180);

        boss8_get_work(pActwk)->target_y = 0;
        boss8_get_work(pActwk)->timer = 0;

        if (boss8_get_work(pActwk)->step & 1) {

            boss8_get_work(pActwk)->target_x = 41;

            if ((xwk = pActwk->xposi.w.h - 3716) < 0) {
                xwk *= -1;
            }
        } else {
            boss8_get_work(pActwk)->target_x = -41;

            if ((xwk = pActwk->xposi.w.h - 3964) < 0) {
                xwk *= -1;
            }
        }

        if (xwk >= 41)
            return;

        boss8_get_work(pActwk)->target_x *= -1;
        if (boss8_get_work(pActwk)->step < 3) {
            if (boss8_get_work(pActwk)->step != 1) {

                boss8_get_work(pActwk)->step = 3;
            } else {
                boss8_get_work(pActwk)->step = 4;
            }

            return;
        }
    }

    boss8_get_work(pActwk)->step = 0;
    boss8_get_work(pActwk)->flags |= 1;

    pMecawk->r_no0 = 2;
    boss8_get_work(pMecawk)->step = 0;
    boss8_get_work(pMecawk)->angle = 0;
    boss8_get_work(pMecawk)->wait_time = 0;
}

static void egg8_tobi_u(sprite_status *pActwk, sprite_status *pMecawk) {
    if (!boss8_get_work(pActwk)->step) {

        boss8_get_work(pActwk)->step = 1;
        boss8_get_work(pActwk)->target_x = 0;
        boss8_get_work(pActwk)->target_y = 0;
        boss8_get_work(pActwk)->timer = 0;
    }

    tobi_set(pActwk);
    pActwk->xposi.w.h -= boss8_get_work(pActwk)->target_x;
    boss8_get_work(pActwk)->target_x = 0;

    if (boss8_get_work(pActwk)->timer >= 16384) {

        boss8_get_work(pActwk)->step = 0;
        boss8_get_work(pActwk)->flags |= 1;
        boss8_get_work(pActwk)->timer_low = 0;
        boss8_get_work(pActwk)->angle = 0;
        boss8_get_work(pActwk)->target_y = 0;
    }
}

static void egg8_dead(sprite_status *pActwk, sprite_status *pMecawk) {
    sprite_status *pNewact;

    if (!boss8_get_work(pActwk)->timer_low) {

        pActwk->mstno.b.h = 3;
        boss8_get_work(pActwk)->hit_flash = 0;

        pMecawk->r_no0 = 16;
        pMecawk->patno = 3;
        pActwk->colino = 0;
        pActwk->colicnt = 0;
        pMecawk->colino = 0;
        pMecawk->colicnt = 0;
        boss8_get_work(pActwk)->flags &= 247;
    }

    if (++boss8_get_work(pActwk)->timer_low >= 90) {

        if (actwkchk(&pNewact) != 0)
            return;
        pNewact->actno = 67;
        pNewact->xposi.w.h = 3656;
        pNewact->yposi.w.h = 460;

        pActwk->r_no0 = 52;

    } else {
        bom_set(pActwk);
    }
}

static void egg8_esc(sprite_status *pActwk, sprite_status *pMecawk) {
    Uint8 dirwk;
    Sint16 sinwk, coswk;

    if (!boss8_get_work(pActwk)->step) {

        scoreup(100);

        boss8_get_work(pActwk)->angle = 0;
        boss8_get_work(pActwk)->target_y = 0;
        ++boss8_get_work(pActwk)->step;
    }

    ++boss8_get_work(pActwk)->timer_low;
    bom_set(pActwk);

    if ((Uint16)boss8_get_work(pActwk)->timer_low % 60 == 0) {
        make_hibana(pActwk);
    }

    pActwk->yposi.w.h -= boss8_get_work(pActwk)->target_y;
    pActwk->yposi.l += 16384;

    dirwk = boss8_get_work(pActwk)->angle;
    boss8_get_work(pActwk)->angle += 3;
    sinset(dirwk, &sinwk, &coswk);

    sinwk = sinwk * 4 >> 8;
    boss8_get_work(pActwk)->target_y = sinwk;
    pActwk->yposi.w.h += sinwk;

    pActwk->xposi.l += boss8_get_work(pActwk)->x_velocity;

    if (pActwk->xposi.w.h >= 4064) {

        colorset2(11);

        pActwk->r_no0 = 54;
        boss8_get_work(pActwk)->timer_low = 0;
        bossstart = 0;
    }
}

static void egg8_esc2(sprite_status *pActwk, sprite_status *pMecawk) {
    ++boss8_get_work(pActwk)->timer_low;
    bom_set(pActwk);
}

static void egg8_warai_chk(sprite_status *pActwk) {
    if (!pActwk->mstno.b.h) {
        if (player_work_get(&actwk[0])->damage_invulnerability_timer || actwk[0].r_no0 == 6) {

            boss8_get_work(pActwk)->hit_flash = 120;
            pActwk->mstno.b.h = 4;
        }
    }
}

static void egg8_colichk(sprite_status *pActwk, sprite_status *pPlayerwk,
                         sprite_status *pMecawk) {
    Uint8 hane_patno;

    if (!(boss8_get_work(pActwk)->flags & 8))
        return;

    boss8_get_work(pMecawk)->action_index = 255;

    if (boss8_get_work(pMecawk)->hit_flash) {

        if (--boss8_get_work(pMecawk)->hit_flash != 0)
            return;

    } else if (!pActwk->colino || !pMecawk->colino) {

        if (pActwk->r_no0 != 30) {
            pActwk->colino = 0;
            pActwk->colicnt = 0;
            pMecawk->colino = 0;
            pMecawk->colicnt = 0;

            make_hibana(pActwk);
            boss8_get_work(pActwk)->hit_flash = 150;
            pActwk->mstno.b.h = 1;

            boss8_get_work(pMecawk)->hit_flash = 120;
            boss8_get_work(pActwk)->action_index = 0;
            boss8_get_work(pActwk)->step = 0;
            boss8_get_work(pActwk)->flags |= 1;

            soundset(172);

            --boss8_get_work(pActwk)->hane_count;
            egg8_spd_set(pActwk, pMecawk);

            hane_patno = boss8_get_work(pMecawk)->hane_base_patno;
            switch (boss8_get_work(pActwk)->hane_count) {

            case 0:
                pActwk->r_no0 = 50;
                boss8_get_work(pActwk)->flags &= 254;
                boss8_get_work(pActwk)->timer_low = 0;
                boss8_get_work(pActwk)->angle = 0;

                boss8_get_work(pMecawk)->action_index = 0;
                break;

            case 3:
                if (pPlayerwk->xposi.w.h < pActwk->xposi.w.h) {

                    if (hane_patno < 4) {

                        boss8_get_work(pMecawk)->action_index = 2;
                    } else if (hane_patno < 8) {

                        boss8_get_work(pMecawk)->action_index = 1;
                    } else if (hane_patno < 12) {

                        boss8_get_work(pMecawk)->action_index = 0;
                    } else {

                        boss8_get_work(pMecawk)->action_index = 3;
                    }
                } else {
                    if (hane_patno >= 13 || hane_patno == 0) {

                        boss8_get_work(pMecawk)->action_index = 2;
                    } else if (hane_patno < 5) {

                        boss8_get_work(pMecawk)->action_index = 1;
                    } else if (hane_patno < 9) {

                        boss8_get_work(pMecawk)->action_index = 0;
                    } else {

                        boss8_get_work(pMecawk)->action_index = 3;
                    }
                }

                break;

            case 2:
                if (pPlayerwk->xposi.w.h < pActwk->xposi.w.h) {

                    if (hane_patno >= 13 || hane_patno < 3) {

                        boss8_get_work(pMecawk)->action_index = 2;
                    } else if (hane_patno < 8) {

                        boss8_get_work(pMecawk)->action_index = 1;
                    } else {

                        boss8_get_work(pMecawk)->action_index = 0;
                    }
                } else {
                    if (hane_patno >= 12 || hane_patno == 0) {

                        boss8_get_work(pMecawk)->action_index = 2;
                    } else if (hane_patno < 5) {

                        boss8_get_work(pMecawk)->action_index = 1;
                    } else {

                        boss8_get_work(pMecawk)->action_index = 0;
                    }
                }

                break;

            case 1:
                if (pPlayerwk->xposi.w.h >= pActwk->xposi.w.h) {

                    boss8_get_work(pMecawk)->action_index = 0;
                } else {
                    boss8_get_work(pMecawk)->action_index = 1;
                }

                break;

            default:
                boss8_get_work(pActwk)->hane_count = 0;
                break;
            }

            return;
        }
    }

    pActwk->colino = 50;
    pActwk->colicnt = 2;
    pMecawk->colino = 51;
    pMecawk->colicnt = 2;
}

static egg8act egg8acttbl1[13] = {
    {18, 0, 316}, {20, 0, 0}, {16, 0, 2}, {30, 0, 0}, {32, 0, 0},
    {6, 0, 120},  {20, 0, 0}, {16, 0, 2}, {44, 0, 0}, {46, 0, 0},
    {48, 0, 0},   {20, 0, 0}, {0, 0, 0}};
static egg8act egg8acttbl2[23] = {
    {18, 0, 316}, {20, 0, 0}, {16, 0, 2}, {22, 0, 0},  {24, 0, 0}, {6, 0, 60},
    {20, 0, 0},   {30, 0, 0}, {32, 0, 0}, {6, 0, 120}, {20, 0, 0}, {38, 0, 0},
    {26, 0, 0},   {6, 0, 60}, {28, 0, 0}, {40, 0, 0},  {20, 0, 0}, {38, 0, 0},
    {26, 0, 0},   {6, 0, 60}, {28, 0, 0}, {40, 0, 0},  {0, 0, 0}};
static egg8act egg8acttbl3[29] = {
    {18, 0, 316}, {20, 0, 0},  {16, 0, 2}, {42, 0, 0},  {34, 0, 0}, {20, 0, 0},
    {14, 0, 348}, {6, 0, 120}, {16, 0, 2}, {22, 0, 0},  {24, 0, 0}, {6, 0, 40},
    {20, 0, 0},   {30, 0, 0},  {32, 0, 0}, {6, 0, 120}, {20, 0, 0}, {38, 0, 0},
    {26, 0, 0},   {6, 0, 90},  {28, 0, 0}, {40, 0, 0},  {20, 0, 0}, {38, 0, 0},
    {26, 0, 0},   {6, 0, 90},  {28, 0, 0}, {40, 0, 0},  {0, 0, 0}};
static egg8act egg8acttbl4[29] = {
    {18, 0, 316}, {20, 0, 0},  {16, 0, 2}, {42, 0, 0},  {34, 0, 0}, {20, 0, 0},
    {14, 0, 348}, {6, 0, 120}, {16, 0, 2}, {22, 0, 0},  {24, 0, 0}, {6, 0, 40},
    {20, 0, 0},   {30, 0, 0},  {32, 0, 0}, {6, 0, 120}, {20, 0, 0}, {38, 0, 0},
    {26, 0, 0},   {6, 0, 120}, {28, 0, 0}, {40, 0, 0},  {20, 0, 0}, {38, 0, 0},
    {26, 0, 0},   {6, 0, 120}, {28, 0, 0}, {40, 0, 0},  {0, 0, 0}};
static egg8act *egg8acttbl[5] = {0, egg8acttbl1, egg8acttbl2, egg8acttbl3,
                                 egg8acttbl4};

static void next_action(sprite_status *pActwk) {
    egg8act *pEggactTbl;
    Uint8 tblno, tblidx;

    do {
        tblno = boss8_get_work(pActwk)->hane_count;
        tblidx = boss8_get_work(pActwk)->action_index;
        pEggactTbl = &egg8acttbl[tblno][tblidx];

        if (!pEggactTbl->r_no) {
            boss8_get_work(pActwk)->action_index = 0;
        }
    } while (!pEggactTbl->r_no);

    ++boss8_get_work(pActwk)->action_index;
    pActwk->r_no0 = pEggactTbl->r_no;
    boss8_get_work(pActwk)->target_pos = pEggactTbl->param;
    boss8_get_work(pActwk)->step = 0;
}

static sprite_pattern meca_pat0 = {1, {{-36, -60, 0, 452}}};
static sprite_pattern meca_pat1 = {1, {{-36, -60, 0, 453}}};
static sprite_pattern meca_pat2 = {1, {{-36, -60, 0, 454}}};
static sprite_pattern meca_pat3 = {1, {{-36, -36, 0, 455}}};
sprite_pattern *egg8meca_pat[4] = {&meca_pat0, &meca_pat1, &meca_pat2,
                                   &meca_pat3};
static void (*meca_act_tbl[10])(sprite_status *, sprite_status *) = {
    &egg8meca_ini,  &egg8meca_normal, &egg8meca_chg1,       &egg8meca_chg2,
    &egg8meca_chg3, &egg8meca_fire,   &egg8meca_hane_reset, &egg8meca_spin,
    &egg8meca_dead, &egg8meca_tobi};

void egg8meca(sprite_status *pActwk) {
    sprite_status *pEggwk;

    pEggwk = &actwk[boss8_get_work(pActwk)->parent_index];

    meca_act_tbl[pActwk->r_no0 / 2](pActwk, pEggwk);

    if (!(boss8_get_work(pActwk)->hit_flash / 2 & 1)) {
        actionsub(pActwk);
    }
}

static void egg8meca_ini(sprite_status *pActwk, sprite_status *pEggwk) {
    pActwk->r_no0 = 2;
    pActwk->actflg = 4;
    pActwk->sprpri = 5;
    pActwk->sprhsize = 36;
    pActwk->sprvsize = 36;
    pActwk->sproffset = 9182;
    pActwk->patbase = egg8meca_pat;
    boss8_get_work(pActwk)->target_pos = 0;
    boss8_get_work(pActwk)->action_index = 255;
    make_hane(pActwk);
    boss8_get_work(pActwk)->hane_count |= 160;
    boss8_get_work(pActwk)->hane_count &= 191;
}

static void egg8meca_normal(sprite_status *pActwk, sprite_status *pEggwk) {
    pActwk->xposi.w.h = pEggwk->xposi.w.h;
    pActwk->yposi.w.h = pEggwk->yposi.w.h;
    hane_ctrl(pActwk);
}

static void egg8meca_spin(sprite_status *pActwk, sprite_status *pEggwk) {
    pActwk->xposi.w.h = pEggwk->xposi.w.h;
    pActwk->yposi.w.h = pEggwk->yposi.w.h;
    futa_roll(pActwk);
}

static void egg8meca_chg1(sprite_status *pActwk, sprite_status *pEggwk) {
    switch (boss8_get_work(pActwk)->step) {

    case 0:
        boss8_get_work(pActwk)->hane_mode = 1;
        boss8_get_work(pActwk)->target_pos = 0;
        boss8_get_work(pActwk)->step = 1;

    case 1:
        if (boss8_get_work(pActwk)->target_pos <= 170)
            break;

    default:
        ++boss8_get_work(pActwk)->step;

        if (boss8_get_work(pActwk)->step == 2) {

            boss8_get_work(pActwk)->hane_mode = 3;
        } else if (boss8_get_work(pActwk)->step == 6) {

            boss8_get_work(pActwk)->hane_mode = 4;
        }
        break;
    }

    if ((boss8_get_work(pActwk)->target_pos += 8) >= 256) {

        boss8_get_work(pActwk)->target_pos = 256;
        boss8_get_work(pActwk)->step = 0;
        pActwk->r_no0 = 2;
        boss8_get_work(pActwk)->hane_mode = 4;
    }
}

static void egg8meca_chg2(sprite_status *pActwk, sprite_status *pEggwk) {
    if (!boss8_get_work(pActwk)->step) {

        boss8_get_work(pActwk)->hane_mode = 1;
        ++boss8_get_work(pActwk)->step;
    }

    if ((boss8_get_work(pActwk)->target_pos += 16) >= 236) {
        boss8_get_work(pActwk)->hane_mode = 2;
    }

    if (boss8_get_work(pActwk)->target_pos >= 256) {

        boss8_get_work(pActwk)->target_pos = 256;
        boss8_get_work(pActwk)->step = 0;
        pActwk->r_no0 = 2;
    }
}

static void egg8meca_chg3(sprite_status *pActwk, sprite_status *pEggwk) {
    pActwk->xposi.w.h = pEggwk->xposi.w.h;
    pActwk->yposi.w.h = pEggwk->yposi.w.h;

    if (!boss8_get_work(pActwk)->step) {

        boss8_get_work(pActwk)->hane_mode = 2;
        ++boss8_get_work(pActwk)->step;
    }

    if ((boss8_get_work(pActwk)->target_pos -= 6) < 236) {
        boss8_get_work(pActwk)->hane_mode = 1;
    }

    if (boss8_get_work(pActwk)->target_pos <= 0) {

        boss8_get_work(pActwk)->target_pos = 0;
        boss8_get_work(pActwk)->step = 0;
        pActwk->r_no0 = 2;
        boss8_get_work(pActwk)->hane_mode = 0;
    }
}

static void egg8meca_fire(sprite_status *pActwk, sprite_status *pEggwk) {
    pActwk->xposi.w.h = pEggwk->xposi.w.h;
    pActwk->yposi.w.h = pEggwk->yposi.w.h;

    if (boss8_get_work(pEggwk)->hane_count == boss8_get_work(pActwk)->hit_timer) {

        boss8_get_work(pActwk)->color_b =
            (actwk[0].xposi.w.h + (Sint16)actwk[0].pattim & 3) * 8;

        pActwk->r_no0 = 2;
        boss8_get_work(pActwk)->flags |= 16;
    } else {
        hane_roll(pActwk);
    }
}

static void egg8meca_hane_reset(sprite_status *pActwk, sprite_status *pEggwk) {
    pActwk->xposi.w.h = pEggwk->xposi.w.h;
    pActwk->yposi.w.h = pEggwk->yposi.w.h;

    if (boss8_get_work(pActwk)->step != 1) {

        boss8_get_work(pActwk)->flags |= 8;
        ++boss8_get_work(pActwk)->step;
        boss8_get_work(pActwk)->target_pos = -256;
        boss8_get_work(pActwk)->hane_count |= 160;
        boss8_get_work(pActwk)->hane_count &= 191;
    }

    if ((boss8_get_work(pActwk)->target_pos += 6) >= 0) {

        boss8_get_work(pActwk)->flags &= 247;
        boss8_get_work(pActwk)->target_pos = 0;
        boss8_get_work(pActwk)->timer_low = 0;
        boss8_get_work(pActwk)->step = 0;
        pActwk->r_no0 = 2;
    }
}

static void egg8meca_dead(sprite_status *pActwk, sprite_status *pEggwk) {
    pActwk->xposi.w.h = pEggwk->xposi.w.h;
    pActwk->yposi.w.h = pEggwk->yposi.w.h;
}

static void egg8meca_tobi(sprite_status *pActwk, sprite_status *pEggwk) {
    Sint16 wk;

    pActwk->xposi.w.h = pEggwk->xposi.w.h;
    pActwk->yposi.w.h = pEggwk->yposi.w.h;

    if ((wk = 416 - pEggwk->yposi.w.h) > 36) {
        wk = 36;
    }

    boss8_get_work(pActwk)->target_pos = (wk << 8) / 36;
}

static void hane_ctrl(sprite_status *pActwk) {
    Uint8 FlagWk;

    FlagWk = boss8_get_work(pActwk)->flags;
    boss8_get_work(pActwk)->flags &= 239;

    if (!(FlagWk & 16)) {
        if (!(boss8_get_work(pActwk)->hane_count & 64)) {
            hane_stop(pActwk);
        } else {
            hane_roll(pActwk);
        }
    } else {
        boss8_get_work(pActwk)->angle = 0;
        boss8_get_work(pActwk)->hane_count &= 95;

        if (!(boss8_get_work(pActwk)->hane_count & 64)) {
            boss8_get_work(pActwk)->hane_count ^= 64;
            hane_roll(pActwk);
        } else {
            boss8_get_work(pActwk)->hane_count ^= 64;
            hane_stop(pActwk);
        }
    }
}

static void hane_stop(sprite_status *pActwk) {
    if (!(boss8_get_work(pActwk)->hane_count & 128)) {
        if (boss8_get_work(pActwk)->hane_count & 32) {

            if (boss8_get_work(pActwk)->hane_base_patno != boss8_get_work(pActwk)->hane_target)
                goto label1;

            boss8_get_work(pActwk)->hane_count |= 128;
            pActwk->patno = 0;
            return;
        }

        pActwk->patno = 1;

        if (++boss8_get_work(pActwk)->angle == 10) {
            boss8_get_work(pActwk)->hane_count |= 32;
            boss8_get_work(pActwk)->angle = 0;
        }

    label1:
        if (++boss8_get_work(pActwk)->timer_low >= boss8_get_work(pActwk)->roll_speed) {
            boss8_get_work(pActwk)->timer_low = 0;
            boss8_get_work(pActwk)->flags |= 64;
        }
    }
}

static void hane_roll(sprite_status *pActwk) {
    if (!(boss8_get_work(pActwk)->hane_count & 32)) {

        pActwk->patno = 1;

        if (++boss8_get_work(pActwk)->angle != 10)
            return;

        boss8_get_work(pActwk)->angle = 0;
        boss8_get_work(pActwk)->hane_count |= 32;
    }

    futa_roll(pActwk);

    if (++boss8_get_work(pActwk)->timer_low >= boss8_get_work(pActwk)->roll_speed) {
        boss8_get_work(pActwk)->timer_low = 0;
        boss8_get_work(pActwk)->flags |= 64;
    }
}

static void futa_roll(sprite_status *pActwk) {
    ++boss8_get_work(pActwk)->angle;

    if (boss8_get_work(pActwk)->angle == 6) {

        pActwk->patno = 2;
    } else if (boss8_get_work(pActwk)->angle == 12) {

        boss8_get_work(pActwk)->angle = 0;
        pActwk->patno = 1;
    }
}

static sprite_pattern hane0_1_pat00 = {1, {{-24, -24, 0, 456}}};
static sprite_pattern hane0_1_pat01 = {2,
                                       {{-30, -34, 0, 457}, {-30, -2, 0, 458}}};
static sprite_pattern hane0_1_pat02 = {2,
                                       {{-32, -32, 0, 459}, {-32, 0, 0, 460}}};
static sprite_pattern hane0_1_pat03 = {2,
                                       {{-32, -36, 0, 461}, {-16, -4, 0, 462}}};
static sprite_pattern hane0_1_pat04 = {1, {{-16, -24, 0, 463}}};
sprite_pattern *egg8hane0_pat1[16] = {
    &hane0_1_pat00, &hane0_1_pat01, &hane0_1_pat02, &hane0_1_pat03,
    &hane0_1_pat04, &hane0_1_pat03, &hane0_1_pat02, &hane0_1_pat01,
    &hane0_1_pat00, &hane0_1_pat01, &hane0_1_pat02, &hane0_1_pat03,
    &hane0_1_pat04, &hane0_1_pat03, &hane0_1_pat02, &hane0_1_pat01};
static sprite_pattern hane0_2_pat00 = {1, {{-24, -24, 0, 464}}};
static sprite_pattern hane0_2_pat01 = {1, {{-30, -34, 0, 465}}};
static sprite_pattern hane0_2_pat02 = {1, {{-32, -32, 0, 466}}};
static sprite_pattern hane0_2_pat03 = {1, {{-40, -36, 0, 467}}};
static sprite_pattern hane0_2_pat04 = {1, {{-40, -24, 0, 468}}};
sprite_pattern *egg8hane0_pat2[16] = {
    &hane0_2_pat00, &hane0_2_pat01, &hane0_2_pat02, &hane0_2_pat03,
    &hane0_2_pat04, &hane0_2_pat03, &hane0_2_pat02, &hane0_2_pat01,
    &hane0_2_pat00, &hane0_2_pat01, &hane0_2_pat02, &hane0_2_pat03,
    &hane0_2_pat04, &hane0_2_pat03, &hane0_2_pat02, &hane0_2_pat01};
static sprite_pattern hane0_3_pat00 = {2,
                                       {{-8, 32, 0, 470}, {-24, -24, 0, 469}}};
static sprite_pattern hane0_3_pat01 = {2,
                                       {{-26, 28, 0, 470}, {-30, -34, 0, 471}}};
static sprite_pattern hane0_3_pat02 = {2,
                                       {{-39, 24, 0, 470}, {-32, -32, 0, 472}}};
static sprite_pattern hane0_3_pat03 = {2,
                                       {{-46, 8, 0, 470}, {-40, -36, 0, 473}}};
static sprite_pattern hane0_3_pat04 = {2,
                                       {{-48, -8, 0, 470}, {-40, -24, 0, 474}}};
sprite_pattern *egg8hane0_pat3[16] = {
    &hane0_3_pat00, &hane0_3_pat01, &hane0_3_pat02, &hane0_3_pat03,
    &hane0_3_pat04, &hane0_3_pat03, &hane0_3_pat02, &hane0_3_pat01,
    &hane0_3_pat00, &hane0_3_pat01, &hane0_3_pat02, &hane0_3_pat03,
    &hane0_3_pat04, &hane0_3_pat03, &hane0_3_pat02, &hane0_3_pat01};
static sprite_pattern hane1_pat00 = {1, {{-16, -32, 0, 475}}};
static sprite_pattern hane1_pat01 = {1, {{-32, -16, 0, 476}}};
sprite_pattern *egg8hane1_pat[16] = {
    &hane1_pat00, 0, 0, 0, &hane1_pat01, 0, 0, 0,
    &hane1_pat00, 0, 0, 0, &hane1_pat01, 0, 0, 0};
static sprite_pattern hane2_1_pat00 = {1, {{-8, -32, 0, 477}}};
static sprite_pattern hane2_1_pat01 = {1, {{-20, -32, 0, 478}}};
static sprite_pattern hane2_1_pat02 = {1, {{-30, -28, 0, 479}}};
static sprite_pattern hane2_1_pat03 = {1, {{-37, -24, 0, 480}}};
static sprite_pattern hane2_1_pat04 = {1, {{-40, -8, 0, 481}}};
sprite_pattern *egg8hane2_pat1[16] = {
    &hane2_1_pat00, &hane2_1_pat01, &hane2_1_pat02, &hane2_1_pat03,
    &hane2_1_pat04, &hane2_1_pat03, &hane2_1_pat02, &hane2_1_pat01,
    &hane2_1_pat00, &hane2_1_pat01, &hane2_1_pat02, &hane2_1_pat03,
    &hane2_1_pat04, &hane2_1_pat03, &hane2_1_pat02, &hane2_1_pat01};
static sprite_pattern hane2_2_pat00 = {1, {{-8, -32, 0, 482}}};
static sprite_pattern hane2_2_pat01 = {1, {{-16, -32, 0, 483}}};
static sprite_pattern hane2_2_pat02 = {1, {{-28, -28, 0, 484}}};
static sprite_pattern hane2_2_pat03 = {1, {{-32, -24, 0, 485}}};
static sprite_pattern hane2_2_pat04 = {1, {{-32, -8, 0, 486}}};
sprite_pattern *egg8hane2_pat2[16] = {
    &hane2_2_pat00, &hane2_2_pat01, &hane2_2_pat02, &hane2_2_pat03,
    &hane2_2_pat04, &hane2_2_pat03, &hane2_2_pat02, &hane2_2_pat01,
    &hane2_2_pat00, &hane2_2_pat01, &hane2_2_pat02, &hane2_2_pat03,
    &hane2_2_pat04, &hane2_2_pat03, &hane2_2_pat02, &hane2_2_pat01};
static sprite_pattern **hane_base_tbl[6] = {egg8hane0_pat1, egg8hane0_pat2,
                                            egg8hane0_pat3, egg8hane1_pat,
                                            egg8hane2_pat1, egg8hane2_pat2};
static Sint32 (*hane_act_tbl[6])(sprite_status *, sprite_status *,
                                 sprite_status *) = {
    &egg8hane_ini,   &egg8hane_demo, &egg8hane_normal,
    &egg8hane_fire1, &egg8hane_wait, &egg8hane_kill};

void egg8hane(sprite_status *pActwk) {
    sprite_status *pMecawk, *pEggwk;
    Sint32 ret;

    pMecawk = &actwk[boss8_get_work(pActwk)->parent_index];
    pEggwk = &actwk[boss8_get_work(pMecawk)->parent_index];

    if (!(boss8_get_work(pMecawk)->action_index & 128)) {
        hane_no_reset(pActwk, pMecawk, pEggwk);
    }

    while ((ret = hane_act_tbl[pActwk->r_no0 / 2](pActwk, pMecawk, pEggwk)) < 0)
        ;

    if (ret == 0)
        return;

    if (pActwk->patno < 4) {

        pActwk->actflg &= 252;
    } else if (pActwk->patno < 8) {

        pActwk->actflg |= 2;
        pActwk->actflg &= 254;
    } else if (pActwk->patno < 12) {

        pActwk->actflg |= 3;
    } else {

        pActwk->actflg &= 253;
        pActwk->actflg |= 1;
    }

    actionsub(pActwk);
}

static Sint32 egg8hane_ini(sprite_status *pActwk, sprite_status *pMecawk,
                           sprite_status *pEggwk) {
    pActwk->r_no0 = 2;
    pActwk->actflg = 4;
    pActwk->sprpri = 5;
    pActwk->sprhsize = 32;
    pActwk->sprvsize = 32;
    pActwk->sproffset = 8736;
    pActwk->patbase = egg8hane0_pat1;

    if (egg8hane_normal(pActwk, pMecawk, pEggwk) < 0) {
        return -1;
    }

    pActwk->colino = 0;
    pActwk->colicnt = 0;

    return 1;
}

static Sint32 egg8hane_demo(sprite_status *pActwk, sprite_status *pMecawk,
                            sprite_status *pEggwk) {
    egg8hane_posiset(pActwk, pMecawk);

    if (pEggwk->r_no0 == 4) {
        pActwk->colino = 0;
        pActwk->colicnt = 0;
    } else {
        pActwk->r_no0 = 4;
    }

    return 1;
}

static Sint32 egg8hane_normal(sprite_status *pActwk, sprite_status *pMecawk,
                              sprite_status *pEggwk) {
    if (boss8_get_work(pActwk)->flags & 8) {
        boss8_get_work(pActwk)->step = 0;
        pActwk->r_no0 = 10;
        return -1;
    }

    if (pMecawk->r_no0 == 10 && pActwk->patno == 0) {
        boss8_get_work(pActwk)->step = 0;
        pActwk->r_no0 = 6;
        ++boss8_get_work(pMecawk)->hit_timer;
        return 1;
    }

    pActwk->patbase = hane_base_tbl[boss8_get_work(pMecawk)->hane_mode];

    if (boss8_get_work(pMecawk)->flags & 64) {
        hane_rol_l(pActwk);
    } else if (boss8_get_work(pMecawk)->flags & 32) {
        hane_rol_r(pActwk);
    }

    if (!boss8_get_work(pActwk)->wait_time) {
        boss8_get_work(pMecawk)->hane_base_patno = pActwk->patno;
    }

    egg8hane_posiset(pActwk, pMecawk);
    return 1;
}

static char reset_tbl_1mai[2] = {8, -1};
static char reset_tbl_2mai[2] = {8, 12};
static char reset_tbl_3mai[4] = {4, 8, 12, -1};
static char reset_tbl_4mai[4] = {0, 4, 8, 12};
static char *reset_pattbl[5] = {0, reset_tbl_1mai, reset_tbl_2mai,
                                reset_tbl_3mai, reset_tbl_4mai};

static Sint32 egg8hane_fire1(sprite_status *pActwk, sprite_status *pMecawk,
                             sprite_status *pEggwk) {
    Sint16 PosWk;
    char *pResetTbl;
    Sint32 lwk;
    static Sint16 hane_fire_tbl[16] = {3776, 3904, 3968, 3712, 3712, 3776,
                                       3840, 3904, 3968, 3904, 3840, 3712,
                                       3760, 3920, 3792, 3856};

    switch (boss8_get_work(pActwk)->step) {

    case 0:
        ++boss8_get_work(pActwk)->step;

        if (boss8_get_work(pEggwk)->hane_count == 1) {
            pActwk->patbase = egg8hane0_pat2;
        }

    case 1:
        pActwk->yposi.l -= 65536;
        if (pActwk->yposi.w.h <= 224) {

            ++boss8_get_work(pActwk)->step;
            ++boss8_get_work(pMecawk)->color_a;
        }

        break;

    case 2:
        if (boss8_get_work(pEggwk)->hane_count <= boss8_get_work(pMecawk)->color_a) {

            boss8_get_work(pActwk)->timer_low = 0;
            ++boss8_get_work(pActwk)->step;
        }

        break;

    case 3:
        if (++boss8_get_work(pActwk)->timer_low == (boss8_get_work(pActwk)->wait_time + 1) * 30) {

            pActwk->sprpri = 3;
            ++boss8_get_work(pActwk)->step;
            boss8_get_work(pActwk)->timer_low = 0;

            pActwk->patno = 0;

            if (boss8_get_work(pEggwk)->hane_count >= 3) {
                pActwk->patbase = egg8hane2_pat2;

                pActwk->xposi.w.h = hane_fire_tbl[boss8_get_work(pMecawk)->color_b / 2 +
                                                  boss8_get_work(pActwk)->wait_time];

                boss8_get_work(pActwk)->x_velocity = 0;
                sprite_status_set_xspeed_yspeed(pActwk, 229376);
            } else {
                pActwk->patbase = egg8hane0_pat2;

                if (boss8_get_work(pEggwk)->hane_count != 2) {

                    if (actwk[0].xposi.w.h < 3840) {

                        pActwk->patno = 3;

                        lwk = 4088;
                        lwk -= actwk[0].xposi.w.h;
                        lwk *= -27146;
                        lwk >>= 16;
                        lwk &= 65535;
                        lwk += 480;
                        pActwk->yposi.w.h = lwk;

                        pActwk->xposi.w.h = 4040;
                        boss8_get_work(pActwk)->x_velocity = -211897;
                        sprite_status_set_xspeed_yspeed(pActwk, 87759);
                    } else {
                        pActwk->patno = 13;

                        lwk = actwk[0].xposi.w.h;
                        lwk -= 3592;
                        lwk *= -27146;
                        lwk >>= 16;
                        lwk &= 65535;
                        lwk += 480;
                        pActwk->yposi.w.h = lwk;

                        pActwk->xposi.w.h = 3640;
                        boss8_get_work(pActwk)->x_velocity = 211897;
                        sprite_status_set_xspeed_yspeed(pActwk, 87759);
                    }
                } else if (!boss8_get_work(pActwk)->wait_time) {

                    pActwk->xposi.w.h = hane_fire_tbl[boss8_get_work(pMecawk)->color_b / 2 +
                                                      boss8_get_work(pActwk)->wait_time];

                    boss8_get_work(pActwk)->x_velocity = 0;
                    sprite_status_set_xspeed_yspeed(pActwk, 229376);
                } else {

                    pActwk->xposi.w.h = actwk[0].xposi.w.h;
                    boss8_get_work(pActwk)->x_velocity = 0;
                    sprite_status_set_xspeed_yspeed(pActwk, 229376);
                }
            }
        }

        break;

    case 4:
        pActwk->xposi.l += boss8_get_work(pActwk)->x_velocity;
        pActwk->yposi.l += sprite_status_get_xspeed_yspeed(pActwk);

        if (pActwk->patno != 0) {

            PosWk = 464;
        } else if (pActwk->patbase == egg8hane2_pat2) {
            PosWk = 448;
        } else {

            PosWk = 440;
        }

        if (pActwk->yposi.w.h >= PosWk) {

            pActwk->yposi.w.h = PosWk;
            ++boss8_get_work(pActwk)->step;
            boss8_get_work(pActwk)->timer_low = 0;

            soundset(180);
        }

        break;

    case 5:
        if (++boss8_get_work(pActwk)->timer_low == 30) {

            pActwk->sprpri = 5;
            ++boss8_get_work(pActwk)->step;
            pActwk->colino = 0;
            pActwk->colicnt = 0;
            boss8_get_work(pActwk)->timer_low = 0;
        }

        break;

    case 6:
        if (++boss8_get_work(pActwk)->timer_low == 60) {

            ++boss8_get_work(pMecawk)->hit_timer;

            boss8_get_work(pActwk)->timer_low = 0;
            boss8_get_work(pActwk)->step = 0;
            pActwk->r_no0 = 8;

            pResetTbl = reset_pattbl[boss8_get_work(pEggwk)->hane_count];

            if (!boss8_get_work(pActwk)->wait_time) {
                boss8_get_work(pMecawk)->hane_base_patno = pResetTbl[0];
            }

            pActwk->patno = pResetTbl[boss8_get_work(pActwk)->wait_time];
            boss8_get_work(pMecawk)->hane_mode = 0;

        } else if (!(boss8_get_work(pActwk)->timer_low / 2 & 1)) {

            return 1;
        }

        return 0;
    }

    return 1;
}

static Sint32 egg8hane_wait(sprite_status *pActwk, sprite_status *pMecawk,
                            sprite_status *pEggwk) {
    if (boss8_get_work(pActwk)->flags & 8) {
        frameout(pActwk);
        return 0;
    }

    if (boss8_get_work(pMecawk)->flags & 8) {

        pActwk->r_no0 = 4;
    }

    return 0;
}

static Sint32 egg8hane_kill(sprite_status *pActwk, sprite_status *pMecawk,
                            sprite_status *pEggwk) {
    if (!boss8_get_work(pActwk)->step) {

        pActwk->colino = 0;
        pActwk->colicnt = 0;

        boss8_get_work(pActwk)->x_velocity = 65536;
        sprite_status_set_xspeed_yspeed(pActwk, -196608);

        if (pActwk->patno > 8) {
            boss8_get_work(pActwk)->x_velocity *= -1;
        }

        boss8_get_work(pActwk)->timer_low = 0;
        boss8_get_work(pActwk)->angle = 0;
        ++boss8_get_work(pActwk)->step;

        if (pActwk->patbase != egg8hane1_pat) {

            pActwk->patbase = egg8hane0_pat3;
        } else {
            pActwk->patbase = egg8hane2_pat1;
        }
    }

    pActwk->xposi.l += boss8_get_work(pActwk)->x_velocity;
    pActwk->yposi.l += sprite_status_get_xspeed_yspeed(pActwk);
    sprite_status_add_xspeed_yspeed(pActwk, 12288);

    if (pActwk->yposi.w.h >= 480) {
        frameout(pActwk);
        return 0;
    }

    if (++boss8_get_work(pActwk)->timer_low == 4) {
        boss8_get_work(pActwk)->timer_low = 0;

        if (++pActwk->patno > 15) {
            pActwk->patno = 0;
        }
    }

    ++boss8_get_work(pActwk)->angle;

    if (boss8_get_work(pActwk)->angle / 2 & 1) {

        return 0;
    }
    return 1;
}

static char hane_reset_tbl_2mai[12] = {-1, 0,  1, -1, 0,  -1,
                                       1,  -1, 0, 1,  -1, -1};
static char hane_reset_tbl_3mai[16] = {-1, 0, 1,  2, 2, -1, 0, 1,
                                       1,  2, -1, 0, 0, 1,  2, -1};
static char *hane_no_reset_tbl[4] = {0, 0, hane_reset_tbl_2mai,
                                     hane_reset_tbl_3mai};

static void hane_no_reset(sprite_status *pActwk, sprite_status *pMecawk,
                          sprite_status *pEggwk) {
    char *pResetTbl;
    Sint32 HaneNum, KillNo;

    if (boss8_get_work(pMecawk)->action_index == boss8_get_work(pActwk)->wait_time) {

        boss8_get_work(pActwk)->flags |= 8;
        return;
    }

    if (boss8_get_work(pEggwk)->hane_count == 1) {

        boss8_get_work(pActwk)->wait_time = 0;
        return;
    }

    HaneNum = boss8_get_work(pEggwk)->hane_count;
    KillNo = boss8_get_work(pMecawk)->action_index;
    pResetTbl = hane_no_reset_tbl[HaneNum] + KillNo * 4;

    boss8_get_work(pActwk)->wait_time = pResetTbl[boss8_get_work(pActwk)->wait_time];
}

static void egg8hane_posiset(sprite_status *pActwk, sprite_status *pMecawk) {
    hane_offs_set(pActwk, pMecawk);
    pActwk->xposi.w.h = pMecawk->xposi.w.h + boss8_get_work(pActwk)->target_x;
    pActwk->yposi.w.h = pMecawk->yposi.w.h + boss8_get_work(pActwk)->target_y;
}

static hane_offs hane0_offs_tbl1[16] = {
    {0, -40, 0, -36, 180},     {19, -36, 18, -31, 181},
    {31, -32, 25, -25, 182},   {38, -16, 31, -18, 183},
    {40, 0, 36, 0, 184},       {38, 16, 31, 18, 183},
    {31, 32, 25, 25, 182},     {19, 36, 18, 31, 181},
    {0, 40, 0, 36, 180},       {-19, 36, -18, 31, 181},
    {-31, 32, -25, 25, 182},   {-38, 16, -31, 18, 183},
    {-40, 0, -36, 0, 184},     {-38, -16, -31, -18, 183},
    {-31, -32, -25, -25, 182}, {-19, -36, -18, -31, 181}};
static hane_offs hane1_offs_tbl[16] = {
    {0, -32, 0, -36, 185}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0},
    {32, 0, 36, 0, 186},   {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0},
    {0, 32, 0, 36, 185},   {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0},
    {-32, 0, -36, 0, 186}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
static hane_offs hane2_offs_tbl[16] = {
    {0, -32, 0, -36, 187},     {12, -29, 18, -31, 188},
    {23, -23, 25, -25, 189},   {29, -12, 31, -18, 190},
    {32, 0, 36, 0, 191},       {29, 12, 31, 18, 190},
    {23, 23, 25, 25, 189},     {12, 29, 18, 31, 188},
    {0, 32, 0, 36, 187},       {-12, 29, -18, 31, 188},
    {-23, 23, -25, 25, 189},   {-29, 12, -31, 18, 190},
    {-32, 0, -36, 0, 191},     {-29, -12, -31, -18, 190},
    {-23, -23, -25, -25, 189}, {-12, -29, -18, -31, 188}};
static hane_offs *hane_offs_tbl[6] = {hane0_offs_tbl1, hane0_offs_tbl1,
                                      hane0_offs_tbl1, hane1_offs_tbl,
                                      hane2_offs_tbl,  hane2_offs_tbl};

static void hane_offs_set(sprite_status *pActwk, sprite_status *pMecawk) {
    hane_offs *pOffsTbl;
    Sint16 hankeiwk;

    pOffsTbl = hane_offs_tbl[boss8_get_work(pMecawk)->hane_mode] + pActwk->patno;

    hankeiwk = boss8_get_work(pMecawk)->target_pos;
    boss8_get_work(pActwk)->target_x = (pOffsTbl->xDst * hankeiwk >> 8) + pOffsTbl->xOffs;

    boss8_get_work(pActwk)->target_y = (pOffsTbl->yDst * hankeiwk >> 8) + pOffsTbl->yOffs;

    pActwk->colino = pOffsTbl->ColiNo;
    pActwk->colicnt = 2;
}

static void hane_rol_l(sprite_status *pActwk) {
    if ((char)--pActwk->patno < 0) {
        pActwk->patno = 15;
    }

    rol_sound(pActwk);
}

static void hane_rol_r(sprite_status *pActwk) {
    if (++pActwk->patno >= 16) {
        pActwk->patno = 0;
    }

    rol_sound(pActwk);
}

static void rol_sound(sprite_status *pActwk) {
    if (!boss8_get_work(pActwk)->wait_time && (pActwk->patno & 3) != 2) {
        soundset(186);
    }
}

static Sint16 hibana0[6] = {-16, 0, -16, -8, -4, 0};
static Sint16 hibana1[6] = {4, 0, 16, -8, 8, 0};
static Sint16 *hibana_offs_tbl[2] = {hibana0, hibana1};
static sprite_pattern hibana_pat0 = {1, {{-8, -4, 0, 487}}};
static sprite_pattern hibana_pat1 = {1, {{-8, -4, 0, 488}}};
sprite_pattern *egg8hibana_pat[2] = {&hibana_pat0, &hibana_pat1};
static Uint32 (*hibana_act_tbl[2])(sprite_status *) = {&egg8hibana_ini,
                                                       &egg8hibana_1};

void egg8hibana(sprite_status *pActwk) {
    if (hibana_act_tbl[pActwk->r_no0 / 2](pActwk) != 0) {
        actionsub(pActwk);
    }
}

static Uint32 egg8hibana_ini(sprite_status *pActwk) {
    pActwk->r_no0 += 2;
    pActwk->actflg = 4;
    pActwk->sprpri = 4;
    pActwk->sprhsize = 8;
    pActwk->sprvsize = 8;
    pActwk->sproffset = 990;
    pActwk->patbase = egg8hibana_pat;

    egg8hibana_1(pActwk);
}

static Uint32 egg8hibana_1(sprite_status *pActwk) {
    sprite_status *pMecawk;
    Sint16 *pOffsTbl;

    pMecawk = &actwk[boss8_get_work(pActwk)->parent_index];
    pActwk->xposi.w.h = pMecawk->xposi.w.h;
    pActwk->yposi.w.h = pMecawk->yposi.w.h;

    pOffsTbl = hibana_offs_tbl[boss8_get_work(pActwk)->hane_mode] + boss8_get_work(pActwk)->step / 2;
    pActwk->xposi.w.h += *pOffsTbl++;
    pActwk->yposi.w.h += *pOffsTbl;

    if (++boss8_get_work(pActwk)->timer_low == 150) {
        frameout(pActwk);
        return 0;
    }

    if (++boss8_get_work(pActwk)->angle == 4) {
        if ((boss8_get_work(pActwk)->step += 4) > 8) {
            boss8_get_work(pActwk)->step = 0;
        }

        boss8_get_work(pActwk)->angle = 0;

        if (++pActwk->patno > 2) {
            pActwk->patno = 0;
        }
    }

    if (pActwk->patno == 2) {
        return 0;
    }
    return 1;
}

static void make_meca(sprite_status *pActwk) {
    sprite_status *pNewact;

    if (actwkchk2(pActwk, &pNewact) == 0) {
        boss8_get_work(pNewact)->parent_index = pActwk - actwk;
        boss8_get_work(pActwk)->parent_index = pNewact - actwk;
        pNewact->actno = 65;
        pNewact->xposi.w.h = pActwk->xposi.w.h;
        pNewact->yposi.w.h = pActwk->yposi.w.h;
    }
}

static void make_hane(sprite_status *pActwk) {
    sprite_status *pNewact;
    Uint8 hanecnt, hanepat;

    hanecnt = hanepat = 0;

    do {
        if (actwkchk2(pActwk, &pNewact) != 0)
            return;

        boss8_get_work(pNewact)->parent_index = pActwk - actwk;
        boss8_get_work(pNewact)->wait_time = hanecnt;
        pNewact->actno = 64;
        pNewact->xposi.w.h = pActwk->xposi.w.h;
        pNewact->yposi.w.h = pActwk->yposi.w.h;
        pNewact->patno = hanepat;
        ++hanecnt;
        hanepat += 4;
    } while (hanecnt != 4);
}

static void make_hibana(sprite_status *pActwk) {
    Uint8 cnt;
    sprite_status *pNewact;

    cnt = 0;

    do {
        if (actwkchk(&pNewact) != 0)
            break;

        boss8_get_work(pNewact)->parent_index = pActwk - actwk;
        pNewact->actno = 66;
        pNewact->xposi.w.h = pActwk->xposi.w.h;
        pNewact->yposi.w.h = pActwk->yposi.w.h;
        boss8_get_work(pNewact)->hane_mode = cnt++;

    } while (cnt != 2);
}

static void egg8_spd_set(sprite_status *pActwk, sprite_status *pMecawk) {
    egg8spd spd_tbl[5] = {{81920, 0, 4, 0},
                          {114688, 65536, 2, 0},
                          {106496, 57344, 3, 0},
                          {98304, 49152, 4, 0},
                          {90112, 40960, 5, 0}};

    boss8_get_work(pActwk)->x_velocity = spd_tbl[boss8_get_work(pActwk)->hane_count].X_Speed;

    sprite_status_set_xspeed_yspeed(
        pActwk, spd_tbl[boss8_get_work(pActwk)->hane_count].Y_Speed);

    boss8_get_work(pMecawk)->roll_speed = spd_tbl[boss8_get_work(pActwk)->hane_count].Roll_Speed;
}

static void tobi_set(sprite_status *pActwk) {
    Sint16 sinwk, coswk;

    pActwk->xposi.w.h -= boss8_get_work(pActwk)->target_x;
    pActwk->yposi.w.h -= boss8_get_work(pActwk)->target_y;

    if ((boss8_get_work(pActwk)->timer_u += 560) >= 32768) {
        boss8_get_work(pActwk)->timer_u = 32768;
    }

    sinset(boss8_get_work(pActwk)->angle, &sinwk, &coswk);

    if (!(boss8_get_work(pActwk)->step & 1)) {
        coswk *= -1;
    }

    coswk = coswk * 41 >> 8;
    sinwk = sinwk * -104 >> 8;
    boss8_get_work(pActwk)->target_x = coswk;
    boss8_get_work(pActwk)->target_y = sinwk;
    pActwk->xposi.w.h += coswk;
    pActwk->yposi.w.h += sinwk;
}

static void bom_set(sprite_status *pActwk) {
    sprite_status *pNewact;
    Sint32 rnd;
    int_union wk1, wk2;

    if (boss8_get_work(pActwk)->timer_low % 2 == 0) {

        if (actwkchk(&pNewact) == 0) {
            pNewact->actno = 24;
            pNewact->r_no1 = 255;
            pNewact->userflag.b.l = -1;

            rnd = random();
            wk2.l = (Sint16)(rnd & 65535);
            wk1.l = (Sint16)((rnd & 4294901760) >> 16);
            pNewact->xposi.w.h = wk1.l % 128 + 3840;
            pNewact->yposi.w.h = wk2.l % 80 + 384;

            soundset(158);
        }
    }
}
