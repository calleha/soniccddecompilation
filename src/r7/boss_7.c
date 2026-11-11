#include "../equ.h"
#include "boss_7.h"
#include "../action.h"
#include "../actset.h"
#include "../dircol.h"
#include "../goal.h"
#include "../io.h"
#include "../loader2.h"
#include "../playsub.h"
#include "../score.h"
#include "col7a.h"
#include "coli7.h"

static void msnc_ini(sprite_status *pActwk);
static void msnc_demo1(sprite_status *pActwk);
static void msnc_demo2(sprite_status *pActwk);
static void msnc_demo2_ini(sprite_status *pActwk);
static void kamae_anime(sprite_status *pActwk);
static void fire_start(sprite_status *pActwk);
static void door_open(sprite_status *pActwk);
static void msnc_demo2_next(sprite_status *pActwk);
static void msnc_egg_chk(sprite_status *pActwk);
static void goal_chk(sprite_status *pActwk);
static void goal_chk_snc_win(sprite_status *pActwk);
static void goal_chk_msnc_win(sprite_status *pActwk);
static void loser_posiset(sprite_status *pActwk);
static void msnc_low_move(sprite_status *pActwk);
static void msnc_atc_move(sprite_status *pActwk);
static void baisoku_mode(sprite_status *pActwk);
static void msnc_normal_move(sprite_status *pActwk);
static void msnc_hi_set(sprite_status *pActwk);
static void msnc_low_set(sprite_status *pActwk);
static void msnc_wk_clr(sprite_status *pActwk);
static void msnc_wk_clr2(sprite_status *pActwk);
static void msnc_atc(sprite_status *pActwk);
static void msnc_act_ini(sprite_status *pActwk);
static void msnc_act_next(sprite_status *pActwk);
static void msnc_f_atc(sprite_status *pActwk);
static void msnc_f_atc_tobi(sprite_status *pActwk);
static void msnc_f_atc_atack(sprite_status *pActwk);
static void msnc_f_atc_coli(sprite_status *pActwk);
static void msnc_f_atc_end(sprite_status *pActwk);
static void msnc_b_atc(sprite_status *pActwk);
static void msnc_b_atc_atack(sprite_status *pActwk);
static void msnc_b_atc_coli(sprite_status *pActwk);
static void msnc_b_atc_kill(sprite_status *pActwk);
static void msnc_ele_ini(sprite_status *pActwk);
static void msnc_ele_01(sprite_status *pActwk);
static void msnc_win(sprite_status *pActwk);
static void msnc_win_ini(sprite_status *pActwk);
static void msnc_win_wait(sprite_status *pActwk);
static void msnc_win_timer(sprite_status *pActwk);
static void msnc_win_chichi(sprite_status *pActwk);
static void msnc_lose(sprite_status *pActwk);
static void egg7_ini(sprite_status *pActwk);
static void egg7_demo1(sprite_status *pActwk);
static void snc_wait(sprite_status *pActwk);
static void r_l(sprite_status *pActwk);
static void l_r(sprite_status *pActwk);
static void r_l2(sprite_status *pActwk);
static void egg7_demo2(sprite_status *pActwk);
static void msnc_turn(sprite_status *pActwk);
static void beam_on(sprite_status *pActwk);
static void beam_on2(sprite_status *pActwk);
static void beam_off(sprite_status *pActwk);
static void move_start(sprite_status *pActwk);
static void beam_next(sprite_status *pActwk);
static void egg7_move(sprite_status *pActwk);
static void egg7_goal(sprite_status *pActwk);
static void egg7_goal_ini(sprite_status *pActwk);
static void egg7_goal_msnc_win(sprite_status *pActwk);
static void egg7_goal_snc_win(sprite_status *pActwk);
static void egg7_goal0(sprite_status *pActwk);
static void egg7_goal1(sprite_status *pActwk);
static void egg7_goal2(sprite_status *pActwk);
static void egg7_goal3(sprite_status *pActwk);
static void emie7clrset(void);
static void egg7_spdset(sprite_status *pActwk);
static void egg7_maxspdset(sprite_status *pActwk, Sint16 d1);
static void egg_beamchk(sprite_status *pActwk);
static void egg7_hscr(sprite_status *pActwk);
static void egg7_hscr_pat0(void);
static void egg7_hscr_pat1(void);
static void egg7_hscr_pat2(void);
static void egg7_hscr_pat3(void);
static void egg_hight(sprite_status *pActwk);
static void msnc_hight(sprite_status *pActwk);
static void hight_set2(sprite_status *pActwk);
static void hight_set(sprite_status *pActwk);
static Sint16 hight_set_retry(sprite_status *pActwk, Sint16 *d1);
static void egg7beam_ini(sprite_status *pActwk, sprite_status *a2);
static void egg7beam_01(sprite_status *pActwk, sprite_status *a2);
static Sint16 beam_posi(sprite_status *pActwk, sprite_status *a2);
static void egg7beam_kemuri1(sprite_status *pActwk, sprite_status *a2);
static void egg7beam_kemuri2(sprite_status *pActwk, sprite_status *a2);
static void kemuri_disp(sprite_status *pActwk);
static void egg7jet_ini(sprite_status *pActwk);
static void egg7jet_01(sprite_status *pActwk);
static void msnc_bara_ini(sprite_status *pActwk);
static void msnc_bara_01(sprite_status *pActwk);
static void add_spd3(sprite_status *pActwk);
static void add_spd2(sprite_status *pActwk);
static void add_spd(sprite_status *pActwk);
static void w_timer(sprite_status *pActwk);
static void b_timer(sprite_status *pActwk);
static void event_timer(sprite_status *pActwk, Sint16 *pTimer,
                        void (**func)(sprite_status *));
static void make_bara(sprite_status *pActwk);
static void make_ele(sprite_status *pActwk);
static void make_msnc(sprite_status *pActwk);
static void make_jet(sprite_status *pActwk);
static void make_beam(sprite_status *pActwk);
static void make_kemuri(sprite_status *pActwk);
static void make_bakuha(sprite_status *pActwk);
static void make_hahen(sprite_status *pActwk);

static Uint8 egg7_pchg0[4] = {5, 0, 1, 255};
static Uint8 *egg7_pchg[1] = {egg7_pchg0};
static sprite_pattern egg7_pat00 = {1, {{-40, -36, 0, 466}}};
static sprite_pattern egg7_pat01 = {4,
                                    {{22, 15, 0, 467},
                                     {-24, -21, 0, 468},
                                     {-32, -4, 0, 469},
                                     {-40, -36, 0, 470}}};
sprite_pattern *egg7_pat[2] = {&egg7_pat00, &egg7_pat01};
static Uint8 egg7beam_pchg0[4] = {1, 0, 1, 255};
static Uint8 *egg7beam_pchg[1] = {egg7beam_pchg0};
static sprite_pattern egg7b_pat00 = {1, {{-8, -16, 0, 471}}};
static sprite_pattern egg7b_pat01 = {1, {{-8, -16, 0, 472}}};
sprite_pattern *egg7beam_pat[2] = {&egg7b_pat00, &egg7b_pat01};
static Uint8 egg7jet_pchg0[6] = {1, 0, 1, 3, 4, 255};
static Uint8 egg7jet_pchg1[6] = {1, 0, 2, 3, 5, 255};
static Uint8 *egg7jet_pchg[2] = {egg7jet_pchg0, egg7jet_pchg1};
static sprite_pattern egg7j_pat00 = {1, {{-12, -8, 0, 473}}};
static sprite_pattern egg7j_pat01 = {1, {{-4, -8, 0, 474}}};
static sprite_pattern egg7j_pat02 = {1, {{-20, -8, 0, 475}}};
static sprite_pattern egg7j_pat03 = {1, {{-12, -8, 0, 476}}};
static sprite_pattern egg7j_pat04 = {1, {{-4, -8, 0, 477}}};
static sprite_pattern egg7j_pat05 = {1, {{-20, -8, 0, 478}}};
sprite_pattern *egg7jet_pat[6] = {&egg7j_pat00, &egg7j_pat01, &egg7j_pat02,
                                  &egg7j_pat03, &egg7j_pat04, &egg7j_pat05};
static Uint8 msnc_pchg00[3] = {19, 0, 255};
static Uint8 msnc_pchg01[10] = {19, 2, 1, 2, 1, 2, 1, 2, 0, 255};
static Uint8 msnc_pchg02[5] = {9, 4, 5, 6, 255};
static Uint8 msnc_pchg03[3] = {9, 6, 255};
static Uint8 msnc_pchg04[6] = {1, 7, 6, 8, 6, 255};
static Uint8 msnc_pchg05[6] = {1, 10, 9, 11, 9, 255};
static Uint8 msnc_pchg06[6] = {1, 13, 12, 14, 12, 255};
static Uint8 msnc_pchg07[6] = {1, 16, 15, 17, 15, 255};
static Uint8 msnc_pchg08[3] = {19, 18, 255};
static Uint8 msnc_pchg09[4] = {1, 19, 20, 255};
static Uint8 msnc_pchg10[4] = {1, 21, 12, 255};
static Uint8 msnc_pchg11[3] = {254, 22, 255};
static Uint8 msnc_pchg12[4] = {19, 2, 1, 255};
static Uint8 msnc_pchg13[3] = {254, 23, 255};
static Uint8 *msnc_pchg[14] = {
    msnc_pchg00, msnc_pchg01, msnc_pchg02, msnc_pchg03, msnc_pchg04,
    msnc_pchg05, msnc_pchg06, msnc_pchg07, msnc_pchg08, msnc_pchg09,
    msnc_pchg10, msnc_pchg11, msnc_pchg12, msnc_pchg13};
static sprite_pattern msnc_pat00 = {1, {{-20, -20, 0, 479}}};
static sprite_pattern msnc_pat01 = {1, {{-20, -20, 0, 480}}};
static sprite_pattern msnc_pat02 = {1, {{-20, -20, 0, 481}}};
static sprite_pattern msnc_pat03 = {1, {{-20, -20, 0, 482}}};
static sprite_pattern msnc_pat04 = {1, {{-20, -20, 0, 483}}};
static sprite_pattern msnc_pat05 = {1, {{-20, -20, 0, 484}}};
static sprite_pattern msnc_pat06 = {1, {{-16, -16, 0, 485}}};
static sprite_pattern msnc_pat07 = {2, {{-16, -16, 0, 486}, {-16, 4, 0, 487}}};
static sprite_pattern msnc_pat08 = {2, {{-16, -16, 0, 488}, {-16, 4, 0, 489}}};
static sprite_pattern msnc_pat09 = {1, {{-20, -24, 0, 490}}};
static sprite_pattern msnc_pat10 = {2, {{-20, -24, 0, 491}, {-16, -4, 0, 492}}};
static sprite_pattern msnc_pat11 = {2, {{-20, -24, 0, 493}, {-20, -8, 0, 494}}};
static sprite_pattern msnc_pat12 = {1, {{-20, -24, 0, 495}}};
static sprite_pattern msnc_pat13 = {2, {{-20, -24, 0, 496}, {-20, -8, 0, 497}}};
static sprite_pattern msnc_pat14 = {2,
                                    {{-20, -24, 0, 498}, {-24, -12, 0, 499}}};
static sprite_pattern msnc_pat15 = {1, {{-20, -24, 0, 500}}};
static sprite_pattern msnc_pat16 = {2, {{-4, -8, 0, 501}, {-20, -24, 0, 502}}};
static sprite_pattern msnc_pat17 = {2, {{-4, -8, 0, 503}, {-20, -24, 0, 504}}};
static sprite_pattern msnc_pat18 = {1, {{-24, -4, 0, 505}}};
static sprite_pattern msnc_pat19 = {1, {{-24, -12, 0, 506}}};
static sprite_pattern msnc_pat20 = {1, {{-24, -12, 0, 507}}};
static sprite_pattern msnc_pat21 = {1, {{-16, -24, 0, 508}}};
static sprite_pattern msnc_pat22 = {1, {{-20, -20, 0, 509}}};
static sprite_pattern msnc_pat23 = {1, {{-20, -24, 0, 510}}};
sprite_pattern *msnc_pat[24] = {
    &msnc_pat00, &msnc_pat01, &msnc_pat02, &msnc_pat03, &msnc_pat04,
    &msnc_pat05, &msnc_pat06, &msnc_pat07, &msnc_pat08, &msnc_pat09,
    &msnc_pat10, &msnc_pat11, &msnc_pat12, &msnc_pat13, &msnc_pat14,
    &msnc_pat15, &msnc_pat16, &msnc_pat17, &msnc_pat18, &msnc_pat19,
    &msnc_pat20, &msnc_pat21, &msnc_pat22, &msnc_pat23};
static sprite_pattern msnc_ele_pat0 = {1, {{-24, -24, 0, 511}}};
static sprite_pattern msnc_ele_pat1 = {1, {{-24, -24, 0, 512}}};
sprite_pattern *msnc_ele_pat[2] = {&msnc_ele_pat0, &msnc_ele_pat1};
static sprite_pattern msnc_bara_pat0 = {1, {{-4, -4, 0, 513}}};
static sprite_pattern msnc_bara_pat1 = {1, {{-4, -4, 0, 514}}};
static sprite_pattern msnc_bara_pat2 = {1, {{-4, -4, 0, 515}}};
static sprite_pattern msnc_bara_pat3 = {1, {{-4, -4, 0, 516}}};
static sprite_pattern msnc_bara_pat4 = {1, {{-8, -8, 0, 517}}};
sprite_pattern *msnc_bara_pat[5] = {&msnc_bara_pat0, &msnc_bara_pat1,
                                    &msnc_bara_pat2, &msnc_bara_pat3,
                                    &msnc_bara_pat4};
static sprite_pattern hahen73_pat0 = {1, {{-4, -4, 0, 518}}};
static sprite_pattern hahen73_pat1 = {1, {{-4, -4, 0, 519}}};
static sprite_pattern hahen73_pat2 = {1, {{-4, -4, 0, 520}}};
static sprite_pattern hahen73_pat3 = {1, {{-4, -4, 0, 521}}};
sprite_pattern *hahen73_pat[4] = {&hahen73_pat0, &hahen73_pat1, &hahen73_pat2,
                                  &hahen73_pat3};
sprite_status *pPlayerwk;
sprite_status *pEggman;
sprite_status *pMsnc;
Sint16 GL_d5;
Sint16 QuickReturn;
extern Uint16 scr_dir_tbl[];
extern sprite_pattern *bakupat[7];

typedef struct {
    union {
        Sint16 timer;
        struct {
            Uint8 timer_low;
            Uint8 byte_timer;
        };
    };
    Uint8 flags;
    Uint8 step;
    Sint16 parent_index;
    Sint16 accel_x;
    Sint16 accel_y;
    Sint16 max_xspeed;
    Sint16 x_offset;
    Uint8 attack_mode;
    Sint16 y_offset;
    Uint8 hscroll_state;
    Uint8 goal_state;
    Sint16 owner_index;
    Sint16 close_counter;
    Sint16 boost_timer;
} boss7_work;

static boss7_work *boss7_get_work(sprite_status *pActwk) {
    return (boss7_work *)pActwk->actfree;
}

void msnc(sprite_status *pActwk) {
    void (*tbl[10])(sprite_status *) = {
        &msnc_ini, &msnc_demo1, &msnc_demo2, &msnc_low_move, &msnc_low_move,
        &msnc_atc, &msnc_f_atc, &msnc_b_atc, &msnc_win,      &msnc_lose};

    pEggman = &actwk[boss7_get_work(pActwk)->parent_index];
    goal_chk(pActwk);
    msnc_egg_chk(pActwk);

    tbl[pActwk->r_no0 / 2](pActwk);
    if (QuickReturn) {
        QuickReturn = 0;
        return;
    }
    patchg(pActwk, msnc_pchg);
    actionsub(pActwk);
}

static void msnc_ini(sprite_status *pActwk) {
    pActwk->actflg = 4;
    pActwk->sprpri = 5;
    pActwk->sprhsize = 20;
    pActwk->sprvsize = 24;
    pActwk->sproffset = 892;
    pActwk->patbase = msnc_pat;
    pActwk->r_no0 += 2;

    msnc_demo1(pActwk);
}

static void msnc_demo1(sprite_status *pActwk) {
    if (pActwk->mstno.b.h == 1) {
        if (pActwk->patcnt != 7)
            return;
        pActwk->mstno.b.h = 0;
    }

    if (!(pPlayerwk->cddat & 2))
        goto label1;
    if (!(pPlayerwk->cddat & 4))
        goto label1;
    if (boss7_get_work(pActwk)->step)
        return;
    boss7_get_work(pActwk)->step = 1;
    pActwk->mstno.b.h = 1;
    return;

label1:
    boss7_get_work(pActwk)->step = 0;
}

static void msnc_demo2(sprite_status *pActwk) {
    void (*demo2_jmp[8])(sprite_status *) = {
        &msnc_demo2_ini, &kamae_anime, &w_timer, &fire_start,
        &w_timer,        &door_open,   &w_timer, &msnc_demo2_next};

    demo2_jmp[boss7_get_work(pActwk)->step](pActwk);
}

static void msnc_demo2_ini(sprite_status *pActwk) {
    pActwk->mstno.b.h = 2;
    ++boss7_get_work(pActwk)->step;
    pActwk->patno = 0;

    kamae_anime(pActwk);
}

static void kamae_anime(sprite_status *pActwk) {
    if (pActwk->patno == 6) {
        pActwk->mstno.b.h = 3;
        ++boss7_get_work(pActwk)->step;
        boss7_get_work(pActwk)->timer = 120;
    }
}

static void fire_start(sprite_status *pActwk) {
    ++boss7_get_work(pActwk)->step;
    pActwk->mstno.b.h = 4;
    boss7_get_work(pActwk)->timer = 150;
}

static void door_open(sprite_status *pActwk) {

    boss7_get_work(pEggman)->hscroll_state = 130;

    bossflag |= 128;
    ++boss7_get_work(pActwk)->step;
    boss7_get_work(pActwk)->timer = 60;

    /* The race against Metal Sonic always uses the bad future track
       (request 32 -> CD track 20 -> R7D), whatever timeframe the level is in.
       egg7_goal3() puts the timeframe's own track back once the race is over. */
    sub_sync(32);
}

static void msnc_demo2_next(sprite_status *pActwk) {
    boss7_get_work(pActwk)->flags |= 32;
    msnc_low_set(pActwk);
}

static void msnc_egg_chk(sprite_status *pActwk) {
    Sint16 d0;

    if (!(boss7_get_work(pActwk)->flags & 32))
        return;
    d0 = pActwk->xposi.w.h;
    d0 -= pEggman->xposi.w.h;
    if (d0 > 88)
        return;

    if (pActwk->xposi.w.h >= 13568) {
        pActwk->xspeed.w = pEggman->xspeed.w;
        boss7_get_work(pActwk)->accel_x = 8;
        boss7_get_work(pActwk)->max_xspeed = 1536;
        return;
    }

    d0 = pActwk->xspeed.w;
    if (d0 < pEggman->xspeed.w) {
        pActwk->xspeed.w = pEggman->xspeed.w;
        boss7_get_work(pActwk)->accel_x = 8;
        boss7_get_work(pActwk)->max_xspeed = 1536;
    }

    pActwk->r_no0 = 12;
    msnc_wk_clr(pActwk);
}

static void goal_chk(sprite_status *pActwk) {
    Sint16 d0;

    if (boss7_get_work(pEggman)->goal_state & 128)
        return;
    if (boss7_get_work(pEggman)->goal_state) {
        goal_chk_snc_win(pActwk);
        return;
    }

    d0 = 16016;
    if (d0 <= pActwk->xposi.w.h) {
        goal_chk_msnc_win(pActwk);
    } else {
        if (d0 <= pPlayerwk->xposi.w.h) {
            goal_chk_snc_win(pActwk);
        }
    }
}

static void goal_chk_snc_win(sprite_status *pActwk) {
    if (boss7_get_work(pEggman)->goal_state == 0) {
        boss7_get_work(pEggman)->goal_state = 1;
        bossflag |= 128;
        soundset(187);
        loser_posiset(pActwk);
        boss7_get_work(pActwk)->max_xspeed = 2048;
        boss7_get_work(pActwk)->accel_x = 16;
    }
    if (pActwk->xposi.w.h >= 15944) {
        boss7_get_work(pEggman)->goal_state |= 128;
        boss7_get_work(pActwk)->flags &= 223;
        pActwk->r_no0 = 18;
        make_bara(pActwk);
        boss7_get_work(pActwk)->timer = 0;
        boss7_get_work(pActwk)->step = 0;
    }
}

static void goal_chk_msnc_win(sprite_status *pActwk) {
    pActwk->xposi.w.h = 16016;
    boss7_get_work(pEggman)->goal_state = 130;
    bossflag |= 128;

    soundset(187);

    loser_posiset(pPlayerwk);

    boss7_get_work(pActwk)->flags &= 223;
    pActwk->r_no0 = 16;
    boss7_get_work(pActwk)->timer = 0;
    boss7_get_work(pActwk)->step = 0;
}

static void loser_posiset(sprite_status *pActwk) {
    Sint16 d1;

    if (pActwk->actno) {
        d1 = 15944;
        if (d1 < pActwk->xposi.w.h) {
            pActwk->xposi.w.h = d1;
        }
    }
}

static void msnc_low_move(sprite_status *pActwk) {
    Sint16 d4;

    add_spd3(pActwk);
    msnc_hight(pActwk);
    if (boss7_get_work(pEggman)->goal_state)
        return;

    ++boss7_get_work(pActwk)->y_offset;
    if (boss7_get_work(pActwk)->attack_mode) {
        baisoku_mode(pActwk);
        return;
    }

    GL_d5 = pPlayerwk->xposi.w.h;
    GL_d5 -= pActwk->xposi.w.h;
    d4 = GL_d5;
    if (d4 >= 0) {
        boss7_get_work(pActwk)->flags |= 64;
    } else {
        boss7_get_work(pActwk)->flags &= 191;
        d4 *= -1;
    }

    if (d4 < 160) {
        if (boss7_get_work(pActwk)->owner_index < 0) {
            boss7_get_work(pActwk)->owner_index = 0;
        }

        ++boss7_get_work(pActwk)->owner_index;
        if (d4 < 16) {
            boss7_get_work(pActwk)->close_counter = 0;
        } else {
            ++boss7_get_work(pActwk)->close_counter;
        }
    } else {
        if (boss7_get_work(pActwk)->owner_index >= 0) {
            boss7_get_work(pActwk)->owner_index = 0;
        }

        --boss7_get_work(pActwk)->owner_index;
    }

    if (boss7_get_work(pActwk)->close_counter >= 50) {
        msnc_atc_move(pActwk);
    } else {
        msnc_normal_move(pActwk);
    }
}

static void msnc_atc_move(sprite_status *pActwk) {
    if (pActwk->xposi.w.h < 13568) {
        pActwk->r_no0 = 10;
        msnc_wk_clr(pActwk);
        return;
    }

    if (!(boss7_get_work(pActwk)->flags & 64)) {
        msnc_wk_clr(pActwk);
        boss7_get_work(pActwk)->attack_mode = 2;

        boss7_get_work(pActwk)->boost_timer = 240;
        msnc_normal_move(pActwk);
    } else {
        msnc_wk_clr(pActwk);
        boss7_get_work(pActwk)->attack_mode = 1;

        boss7_get_work(pActwk)->boost_timer = 240;
        msnc_normal_move(pActwk);
    }
}

static void baisoku_mode(sprite_status *pActwk) {

    if (boss7_get_work(pActwk)->boost_timer < 2) {
        --boss7_get_work(pActwk)->boost_timer;
        boss7_get_work(pActwk)->attack_mode = 0;
        boss7_get_work(pActwk)->boost_timer = 0;
    } else {
        --boss7_get_work(pActwk)->boost_timer;
    }

    msnc_normal_move(pActwk);
}

static void msnc_normal_move(sprite_status *pActwk) {
    Sint16 d0;

    if (boss7_get_work(pActwk)->boost_timer)
        goto label1;
    if (boss7_get_work(pActwk)->owner_index > -120)
        goto label1;

    if (GL_d5 < 0)
        goto label2;
    msnc_atc_move(pActwk);
    return;
label1:
    d0 = boss7_get_work(pActwk)->y_offset;
    d0 = (Uint16)d0 % 60;
    if (d0 >= 30)
        goto label2;

    if (pActwk->r_no0 != 8) {
        msnc_hi_set(pActwk);
    }
    return;
label2:
    if (pActwk->r_no0 != 6) {
        msnc_low_set(pActwk);
    }
}

static void msnc_hi_set(sprite_status *pActwk) {
    Sint16 d0;

    if (boss7_get_work(pActwk)->attack_mode == 1)
        d0 = 2048;
    else if (boss7_get_work(pActwk)->attack_mode == 2)
        d0 = 682;
    else
        d0 = 1024;

    boss7_get_work(pActwk)->max_xspeed = d0;
    if (d0 >= pActwk->xspeed.w) {
        d0 *= -1;
    }
    boss7_get_work(pActwk)->accel_x = d0;
    pActwk->r_no0 = 8;
    pActwk->mstno.b.h = 6;
    msnc_wk_clr2(pActwk);
}

static void msnc_low_set(sprite_status *pActwk) {
    Sint16 d0;

    if (boss7_get_work(pActwk)->attack_mode == 1)
        d0 = 1280;
    else if (boss7_get_work(pActwk)->attack_mode == 2)
        d0 = 426;
    else
        d0 = 640;

    boss7_get_work(pActwk)->max_xspeed = d0;
    if (d0 >= pActwk->xspeed.w) {
        d0 *= -1;
    }
    boss7_get_work(pActwk)->accel_x = d0;
    pActwk->r_no0 = 6;
    pActwk->mstno.b.h = 5;
    msnc_wk_clr2(pActwk);
}

static void msnc_wk_clr(sprite_status *pActwk) {
    boss7_get_work(pActwk)->close_counter = 0;
    msnc_wk_clr2(pActwk);
}

static void msnc_wk_clr2(sprite_status *pActwk) {
    boss7_get_work(pActwk)->step = 0;
    boss7_get_work(pActwk)->timer = 0;
}

static void msnc_atc(sprite_status *pActwk) {
    Sint16 event_tbl_t[2] = {0, 60};
    void (*event_tbl_f[2])(sprite_status *) = {&msnc_act_ini, &msnc_act_next};

    add_spd3(pActwk);
    msnc_hight(pActwk);
    event_timer(pActwk, event_tbl_t, event_tbl_f);
}

static void msnc_act_ini(sprite_status *pActwk) {
    pActwk->mstno.b.h = 4;
    if (pActwk->xposi.w.h > scra_h_posit.w.h - 160 &&
        pActwk->xposi.w.h < scra_h_posit.w.h + 320)
        soundset(201);
}

static void msnc_act_next(sprite_status *pActwk) {
    GL_d5 = pPlayerwk->xposi.w.h;
    GL_d5 -= pActwk->xposi.w.h;
    if (GL_d5 >= 0) {
        boss7_get_work(pActwk)->flags |= 64;

        pActwk->r_no0 = 12;
        msnc_wk_clr(pActwk);
    } else {
        boss7_get_work(pActwk)->flags &= 191;

        pActwk->r_no0 = 14;
        msnc_wk_clr(pActwk);
    }
}

static void msnc_f_atc(sprite_status *pActwk) {
    Sint16 event_tbl_t[4] = {0, 30, 40, 180};
    void (*event_tbl_f[4])(sprite_status *) = {
        &msnc_f_atc_tobi, &msnc_f_atc_atack, &msnc_f_atc_coli, &msnc_f_atc_end};

    add_spd3(pActwk);
    msnc_hight(pActwk);
    event_timer(pActwk, event_tbl_t, event_tbl_f);
}

static void msnc_f_atc_tobi(sprite_status *pActwk) {
    if (pActwk->xposi.w.h > scra_h_posit.w.h - 160 &&
        pActwk->xposi.w.h < scra_h_posit.w.h + 320)
        soundset(202);

    boss7_get_work(pActwk)->accel_x = 8;
    boss7_get_work(pActwk)->max_xspeed = 1536;
    pActwk->mstno.b.h = 8;
}

static void msnc_f_atc_atack(sprite_status *pActwk) { pActwk->mstno.b.h = 9; }

static void msnc_f_atc_coli(sprite_status *pActwk) {
    pActwk->colino = 188;
    pActwk->colicnt = 2;
}

static void msnc_f_atc_end(sprite_status *pActwk) {
    Sint16 d0;

    pActwk->colino = 0;
    pActwk->colicnt = 0;
    d0 = pEggman->xposi.w.h;
    if (d0 < pActwk->xposi.w.h)
        msnc_hi_set(pActwk);
    else
        msnc_low_set(pActwk);
}

static void msnc_b_atc(sprite_status *pActwk) {
    Sint16 event_tbl_t[3] = {0, 10, 150};
    void (*event_tbl_f[3])(sprite_status *) = {
        &msnc_b_atc_atack, &msnc_b_atc_coli, &msnc_b_atc_kill};

    add_spd3(pActwk);
    msnc_hight(pActwk);
    event_timer(pActwk, event_tbl_t, event_tbl_f);
}

static void msnc_b_atc_atack(sprite_status *pActwk) {
    boss7_get_work(pActwk)->accel_x = -8;
    boss7_get_work(pActwk)->max_xspeed = 512;
    make_ele(pActwk);
    pActwk->mstno.b.h = 10;

    soundset(208);
}

static void msnc_b_atc_coli(sprite_status *pActwk) {
    pActwk->colino = 189;
    pActwk->colicnt = 2;
}

static void msnc_b_atc_kill(sprite_status *pActwk) {
    pActwk->colino = 0;
    pActwk->colicnt = 0;
    msnc_hi_set(pActwk);
}

void msnc_ele(sprite_status *pActwk) {
    void (*act_tbl[2])(sprite_status *) = {&msnc_ele_ini, &msnc_ele_01};

    pMsnc = &actwk[boss7_get_work(pActwk)->parent_index];
    act_tbl[pActwk->r_no0 / 2](pActwk);
}

static void msnc_ele_ini(sprite_status *pActwk) {
    pActwk->actflg = 4;
    pActwk->sprpri = 5;
    pActwk->sprhsize = 24;
    pActwk->sprvsize = 24;
    pActwk->sproffset = 892;
    pActwk->patbase = msnc_ele_pat;
    pActwk->r_no0 += 2;

    msnc_ele_01(pActwk);
}

static void msnc_ele_01(sprite_status *pActwk) {
    if (pMsnc->mstno.b.h != 10) {
        frameout(pActwk);
        return;
    }
    pActwk->xposi.w.h = pMsnc->xposi.w.h;
    pActwk->yposi.w.h = pMsnc->yposi.w.h;

    ++boss7_get_work(pActwk)->byte_timer;
    if (boss7_get_work(pActwk)->byte_timer != 5) {
        actionsub(pActwk);
        return;
    }
    boss7_get_work(pActwk)->byte_timer = 0;
    ++pActwk->patno;
    if (pActwk->patno == 2) {
        pActwk->patno = 0;
    }
}

static void msnc_win(sprite_status *pActwk) {
    void (*jmp[4])(sprite_status *) = {&msnc_win_ini, &msnc_win_wait,
                                       &msnc_win_timer, &msnc_win_chichi};

    loser_posiset(pPlayerwk);

    jmp[boss7_get_work(pActwk)->step](pActwk);
}

static void msnc_win_ini(sprite_status *pActwk) {
    pActwk->mstno.b.h = 11;
    pActwk->xposi.w.h = 16016;
    pActwk->yposi.w.h = 460;
    ++boss7_get_work(pActwk)->step;
}

static void msnc_win_wait(sprite_status *pActwk) {
    if (pPlayerwk->xposi.w.h >= 15824) {
        ++boss7_get_work(pActwk)->step;
        boss7_get_work(pActwk)->byte_timer = 30;
    }
}

static void msnc_win_timer(sprite_status *pActwk) { b_timer(pActwk); }

static void msnc_win_chichi(sprite_status *pActwk) { pActwk->mstno.b.h = 12; }

static void msnc_lose(sprite_status *pActwk) {
    if (boss7_get_work(pActwk)->step == 0) {
        scoreup(100);

        boss7_get_work(pActwk)->step = 1;
        pActwk->mstno.b.h = 13;
        pActwk->xspeed.w = 0;
        boss7_get_work(pActwk)->accel_x = 0;
        pActwk->yspeed.w = -1280;
        boss7_get_work(pActwk)->accel_y = 40;
        make_bakuha(pActwk);
    }
    if (boss7_get_work(pActwk)->timer == 240) {
        QuickReturn = 1;
        frameout(pActwk);
        return;
    }
    add_spd2(pActwk);
    ++boss7_get_work(pActwk)->timer;
}

void egg7(sprite_status *pActwk) {
    void (*tbl[5])(sprite_status *) = {&egg7_ini, &egg7_demo1, &egg7_demo2,
                                       &egg7_move, &egg7_goal};

    pPlayerwk = &actwk[0];
    tbl[pActwk->r_no0 / 2](pActwk);
    if (QuickReturn) {
        QuickReturn = 0;
        return;
    }
    egg7_hscr(pActwk);
    egg_beamchk(pActwk);
    patchg(pActwk, egg7_pchg);
    actionsub(pActwk);
}

static void egg7_ini(sprite_status *pActwk) {
    pActwk->actflg = 4;
    pActwk->sprpri = 1;
    pActwk->sprhsize = 36;
    pActwk->sprvsize = 36;
    pActwk->sproffset = 768;
    pActwk->patbase = egg7_pat;

    bossflag += 7;
    bossstart = 7;

    colorset2(6);
    pActwk->r_no0 = 2;
    make_jet(pActwk);
    boss7_get_work(pActwk)->flags |= 8;

    make_msnc(pActwk);
    make_beam(pActwk);
}

static void egg7_demo1(sprite_status *pActwk) {
    void (*demo1_jmp[6])(sprite_status *) = {&snc_wait, &r_l,     &w_timer,
                                             &l_r,      &w_timer, &r_l2};

    egg_hight(pActwk);

    demo1_jmp[boss7_get_work(pActwk)->step](pActwk);
}

static void snc_wait(sprite_status *pActwk) {
    Sint16 d0;

    boss7_get_work(pActwk)->hscroll_state &= 240;
    ++boss7_get_work(pActwk)->hscroll_state;
    if (pPlayerwk->xposi.w.h >= 3040) {
        pActwk->xspeed.w = -1536;
        boss7_get_work(pActwk)->accel_x = 0;
        ++boss7_get_work(pActwk)->step;
        pActwk->cddat |= 1;

        d0 = 2880;
        scralim_left = d0;
        scralim_n_left = d0;

        sub_sync(103);
        return;
    }

    d0 = pPlayerwk->xposi.w.h;
    d0 -= 160;

    if (d0 < scralim_left)
        return;
    if (d0 <= 2880) {
        scralim_left = d0;
        scralim_n_left = d0;
    }
}

static void r_l(sprite_status *pActwk) {
    Sint16 d0;

    add_spd2(pActwk);
    d0 = 2840;
    if (d0 < pActwk->xposi.w.h)
        return;
    pActwk->xposi.w.h = d0;
    pActwk->xspeed.w = 1536;
    boss7_get_work(pActwk)->accel_x = 0;
    ++boss7_get_work(pActwk)->step;
    pActwk->cddat &= 254;
    boss7_get_work(pActwk)->timer = 120;
}

static void l_r(sprite_status *pActwk) {
    Sint16 d0;

    add_spd2(pActwk);
    d0 = 3240;
    if (d0 > pActwk->xposi.w.h)
        return;
    pActwk->xposi.w.h = d0;
    pActwk->xspeed.w = -1536;
    boss7_get_work(pActwk)->accel_x = 0;
    pActwk->cddat |= 1;
    ++boss7_get_work(pActwk)->step;
    boss7_get_work(pActwk)->timer = 120;
}

static void r_l2(sprite_status *pActwk) {
    Sint16 d0;

    add_spd2(pActwk);
    d0 = 3104;
    if (d0 < pActwk->xposi.w.h)
        return;
    pActwk->xposi.w.h = d0;
    pActwk->xspeed.w = 0;
    boss7_get_work(pActwk)->accel_x = 0;
    boss7_get_work(pActwk)->step = 0;
    boss7_get_work(pActwk)->timer = 0;
    boss7_get_work(pActwk)->flags &= 247;
    pActwk->r_no0 = 4;
}

static void egg7_demo2(sprite_status *pActwk) {
    Sint16 event_tbl_t[12] = {120, 180, 184, 185, 190, 220,
                              224, 230, 260, 264, 270, 420};

    void (*event_tbl_f[12])(sprite_status *) = {
        &msnc_turn, &beam_on2, &beam_on,  &move_start, &beam_off, &beam_on2,
        &beam_on,   &beam_off, &beam_on2, &beam_on,    &beam_off, &beam_next};
    Sint16 d0;

    add_spd2(pActwk);
    d0 = 2816;
    if (d0 >= pActwk->xposi.w.h) {
        pActwk->xposi.w.h = d0;
    }
    egg_hight(pActwk);

    event_timer(pActwk, event_tbl_t, event_tbl_f);
}

static void msnc_turn(sprite_status *pActwk) {
    sprite_status *a2;

    a2 = &actwk[boss7_get_work(pActwk)->parent_index];
    a2->r_no0 = 4;
    boss7_get_work(a2)->timer = 0;
    boss7_get_work(a2)->step = 0;
}

static void beam_on(sprite_status *pActwk) {
    boss7_get_work(pActwk)->flags |= 64;
    boss7_get_work(pActwk)->flags |= 32;
}

static void beam_on2(sprite_status *pActwk) {
    boss7_get_work(pActwk)->flags |= 64;
    boss7_get_work(pActwk)->flags &= 223;
}

static void beam_off(sprite_status *pActwk) {
    boss7_get_work(pActwk)->flags &= 191;
    boss7_get_work(pActwk)->flags &= 223;
}

static void move_start(sprite_status *pActwk) {
    boss7_get_work(pActwk)->flags |= 8;
    pActwk->xspeed.w = -512;
    boss7_get_work(pActwk)->accel_x = 0;
}

static void beam_next(sprite_status *pActwk) {
    beam_on2(pActwk);

    pActwk->r_no0 = 6;
    boss7_get_work(pActwk)->flags |= 16;

    pActwk->xspeed.w = 512;
    boss7_get_work(pActwk)->max_xspeed = 512;
    boss7_get_work(pActwk)->accel_x = 2;
    pActwk->xspeed.w = 0;
    boss7_get_work(pActwk)->accel_y = 0;

    pActwk->cddat &= 254;
    boss7_get_work(pActwk)->step = 0;
    boss7_get_work(pActwk)->timer = 0;
}

static void egg7_move(sprite_status *pActwk) {
    Sint16 d0;

    d0 = 15952;
    if (d0 > pActwk->xposi.w.h) {
        add_spd3(pActwk);
        egg7_spdset(pActwk);
        egg_hight(pActwk);
    } else {
        pActwk->xposi.w.h = d0;
        pActwk->r_no0 = 8;
        boss7_get_work(pActwk)->step = 0;
        pActwk->xspeed.w = 0;
        pActwk->yspeed.w = 0;
    }
}

static void egg7_goal(sprite_status *pActwk) {
    void (*jmp[10])(sprite_status *) = {
        &egg7_goal_ini, &egg7_goal_msnc_win, &w_timer, &egg7_goal0,
        &w_timer,       &egg7_goal1,         &w_timer, &egg7_goal2,
        &w_timer,       &egg7_goal3};

    jmp[boss7_get_work(pActwk)->step](pActwk);
}

static void egg7_goal_ini(sprite_status *pActwk) {
    Uint8 d0;

    d0 = boss7_get_work(pActwk)->goal_state;
    d0 &= 15;
    if (d0 == 1) {
        egg7_goal_snc_win(pActwk);
        return;
    }
    if (d0 == 2) {
        egg7_goal_msnc_win(pActwk);
    }
}

static void egg7_goal_msnc_win(sprite_status *pActwk) {
    boss7_get_work(pActwk)->step = 1;
}

static void egg7_goal_snc_win(sprite_status *pActwk) {
    boss7_get_work(pActwk)->flags &= 223;
    boss7_get_work(pActwk)->flags &= 239;
    boss7_get_work(pActwk)->hscroll_state &= 15;

    boss7_get_work(pActwk)->step = 2;
    boss7_get_work(pActwk)->timer = 30;
}

static void egg7_goal0(sprite_status *pActwk) {
    Sint16 d0;

    ++boss7_get_work(pActwk)->timer;
    if (boss7_get_work(pActwk)->timer < 120) {
        d0 = boss7_get_work(pActwk)->timer;
        d0 = (Uint16)d0 % 20;
        if (d0 == 0) {
            boss7_get_work(pActwk)->flags ^= 64;
        }
    } else {
        boss7_get_work(pActwk)->flags &= 191;
        ++boss7_get_work(pActwk)->step;
        boss7_get_work(pActwk)->timer = 30;
        pActwk->xspeed.w = 0;
        pActwk->yspeed.w = 512;
    }
}

static void egg7_goal1(sprite_status *pActwk) {
    add_spd(pActwk);
    if (pActwk->yposi.w.h >= 400) {
        pActwk->xspeed.w = 1536;
        pActwk->yspeed.w = 0;
        ++boss7_get_work(pActwk)->step;
        boss7_get_work(pActwk)->timer = 60;
    }
}

static void egg7_goal2(sprite_status *pActwk) {
    add_spd(pActwk);
    if (pActwk->xposi.w.h >= 16224) {
        ++boss7_get_work(pActwk)->step;
        boss7_get_work(pActwk)->timer = 60;
    }
}

static void egg7_goal3(sprite_status *pActwk) {
    Sint16 d0;

    d0 = 32;
    if (generate_flag) {
        d0 = 31;
    }
    sub_sync(d0);

    boss7_get_work(pActwk)->hscroll_state = 3;
    egg7_hscr(pActwk);

    genecolor();
    emie7clrset();

    QuickReturn = 1;
    frameout(pActwk);
}

static void emie7clrset(void) {
    Sint16 i;
    PALETTEENTRY *a3, *a4, *a5;

    PALETTEENTRY emie7_clr[16] = {
        {0, 0, 0, 1},       {0, 0, 0, 1},       {128, 32, 96, 1},
        {160, 64, 128, 1},  {224, 96, 224, 1},  {224, 160, 224, 1},
        {224, 224, 224, 1}, {160, 160, 160, 1}, {128, 128, 128, 1},
        {64, 64, 64, 1},    {224, 160, 128, 1}, {192, 96, 0, 1},
        {32, 192, 0, 1},    {0, 128, 0, 1},     {96, 0, 128, 1},
        {224, 0, 0, 1}};

    a3 = emie7_clr;
    a4 = &lpcolorwk[16];
    a5 = &lpcolorwk3[16];
    for (i = 0; i < 16; ++i) {
        *a5 = *a4 = *a3;
        ++a3;
        ++a4;
        ++a5;
    }
}

static void egg7_spdset(sprite_status *pActwk) {
    Sint16 d0, d1;

    d0 = (Uint16)pActwk->sprhsize;
    d1 = pActwk->xposi.w.h;
    d1 -= scra_h_posit.w.h;
    d1 += d0;
    if (d1 < 0) {

        ++boss7_get_work(pActwk)->close_counter;
        egg7_maxspdset(pActwk, d1);
        if (boss7_get_work(pActwk)->owner_index) {
            --boss7_get_work(pActwk)->owner_index;
            return;
        }

        boss7_get_work(pActwk)->flags &= 251;

    } else {
        boss7_get_work(pActwk)->close_counter = 0;

        egg7_maxspdset(pActwk, d1);

        if (!(boss7_get_work(pActwk)->flags & 4)) {
            boss7_get_work(pActwk)->flags |= 4;
            boss7_get_work(pActwk)->owner_index = 240;

            d1 = 256;
            d0 = pPlayerwk->xspeed.w;
            if (d0 < d1) {
                d0 = d1;
            }
            pActwk->xspeed.w = d0;
        } else {
            boss7_get_work(pActwk)->flags |= 4;
        }
    }
}

static void egg7_maxspdset(sprite_status *pActwk, Sint16 d1) {
    Sint16 d0;

    Sint16 spd_tbl[8] = {512, 2, 768, 2, 1088, 8, 1536, 4};

    if (d1 > -160) {
        if (pPlayerwk->xposi.w.h >= 12544) {
            d0 = 8;
        } else if (pPlayerwk->xposi.w.h >= 3520) {
            d0 = 4;
        } else {
            d0 = 0;
        }
    } else {
        d0 = 12;
    }

    boss7_get_work(pActwk)->max_xspeed = spd_tbl[d0 / 2];
    boss7_get_work(pActwk)->accel_x = spd_tbl[d0 / 2 + 1];
}

static void egg_beamchk(sprite_status *pActwk) {
    sprite_status *a0;
    Sint16 d0;

    if (!(boss7_get_work(pActwk)->flags & 16))
        return;
    if (pPlayerwk->actno == 0)
        return;
    if (pPlayerwk->r_no0 >= 6)
        return;
    d0 = pPlayerwk->xposi.w.h;
    d0 -= pActwk->xposi.w.h;
    if (d0 > 16)
        return;
    a0 = pPlayerwk;
    a0->cddat |= 128;
    playdieset(a0);
}

static void egg7_hscr(sprite_status *pActwk) {
    void (*rlim_jmpd0[4])(void) = {&egg7_hscr_pat0, &egg7_hscr_pat1,
                                   &egg7_hscr_pat2, &egg7_hscr_pat3};
    Sint16 d0;

    if (boss7_get_work(pActwk)->hscroll_state & 128) {
        d0 = pActwk->xposi.w.h;
        d0 -= 8;
        if (d0 > 15776) {
            d0 = 15776;
        }
        scralim_left = d0;
        scralim_n_left = d0;

        d0 = pPlayerwk->xposi.w.h;
        d0 -= scralim_left;
        if (d0 < 160) {
            d0 -= 160;
            d0 += 160;
            d0 &= -2;
            scra_hline = d0;
        }
    }
    rlim_jmpd0[boss7_get_work(pActwk)->hscroll_state & 15]();
}

static void egg7_hscr_pat0(void) {}

static void egg7_hscr_pat1(void) {
    Sint16 d0;

    d0 = 2880;
    scralim_right = d0;
    scralim_n_right = d0;
}

static void egg7_hscr_pat2(void) {
    Sint16 d0;

    d0 = 15776;
    scralim_right = d0;
    scralim_n_right = d0;
}

static void egg7_hscr_pat3(void) {
    scralim_right = scr_dir_tbl[2];
    scralim_n_right = scr_dir_tbl[2];
}

static void egg_hight(sprite_status *pActwk) {
    pActwk->yposi.w.h += 128;
    hight_set(pActwk);
    pActwk->yposi.w.h -= 128;
}

static void msnc_hight(sprite_status *pActwk) {
    pActwk + 1;
    hight_set2(pActwk);
    pActwk + 1;
}

static void hight_set2(sprite_status *pActwk) {
    Sint32 ld1;
    Sint16 d1;

    d1 = emycol_d(pActwk);
    if (d1 <= 8)
        goto label1;
    if (hight_set_retry(pActwk, &d1) != 0) {
        pActwk->userflag.w += 48;
        ld1 = pActwk->userflag.w;
        ld1 <<= 8;
        pActwk->yposi.l += ld1;
        return;
    }
label1:
    pActwk->userflag.w = 0;
    pActwk->yposi.w.h += d1;
}

static void hight_set(sprite_status *pActwk) {
    Sint16 d1;
    Sint16 bne;

    d1 = emycol_d(pActwk);
    if (d1 <= 8)
        goto label1;
    bne = hight_set_retry(pActwk, &d1);
    goto label2;

label1:
    if (d1 >= -8)
        goto label2;
    bne = hight_set_retry(pActwk, &d1);

label2:
    pActwk->yposi.w.h += d1;
}

static Sint16 hight_set_retry(sprite_status *pActwk, Sint16 *d1) {
    Sint16 egg_move_tbl[43] = {3120,  0, 0,   5648,  0, -2,  6232,  0, -16,
                               8736,  0, -2,  9360,  0, -16, 10272, 0, -2,
                               10873, 0, -16, 11360, 0, -16, 11872, 0, -16,
                               12224, 0, 0,   12320, 0, -2,  12336, 0, -2,
                               15920, 0, 0,   16304, 0, 0,   -1};
    Sint16 d2;
    Sint16 d3;
    Sint16 d4;
    Sint16 *a2;

    a2 = egg_move_tbl;
    do {
        d2 = *a2++;
        if (d2 < 0)
            return -1;
        d3 = *a2++;
        d4 = *a2++;

        if (d2 <= pActwk->xposi.w.h) {
            d2 -= pActwk->xposi.w.h;
            d2 *= -1;
        } else {
            d2 -= pActwk->xposi.w.h;
        }
    } while (d2 > 64);

    pActwk->xposi.w.h += d3;
    pActwk->yposi.w.h += d4;
    *d1 = 0;
    return 0;
}

void egg7beam(sprite_status *pActwk) {
    void (*tbl[4])(sprite_status *, sprite_status *) = {
        &egg7beam_ini, &egg7beam_01, &egg7beam_kemuri1, &egg7beam_kemuri2};
    sprite_status *a2;

    a2 = &actwk[boss7_get_work(pActwk)->parent_index];
    if (a2->actno == 0) {
        frameout(pActwk);
        return;
    }
    tbl[pActwk->r_no0 / 2](pActwk, a2);
}

static void egg7beam_ini(sprite_status *pActwk, sprite_status *a2) {
    if (pActwk->userflag.b.h == 0) {
        pActwk->actflg = 4;
        pActwk->sprpri = 2;
        pActwk->sprhsize = 8;
        pActwk->sprvsize = 16;
        pActwk->sproffset = 8960;
        pActwk->patbase = egg7beam_pat;
        pActwk->r_no0 = 2;
    } else {
        pActwk->actflg = 4;
        pActwk->sprpri = 1;
        pActwk->sprhsize = 16;
        pActwk->sprvsize = 16;
        pActwk->sproffset = 1664;
        pActwk->patbase = bakupat;

        if (pActwk->userflag.b.h != 2) {
            pActwk->r_no0 = 4;

            boss7_get_work(pActwk)->byte_timer = 15;
        } else {
            pActwk->r_no0 = 6;

            boss7_get_work(pActwk)->byte_timer = 30;
        }
    }
}

static void egg7beam_01(sprite_status *pActwk, sprite_status *a2) {
    pActwk->colino = 0;
    pActwk->colicnt = 0;
    if (boss7_get_work(a2)->flags & 32) {
        pActwk->colino = 190;
        pActwk->colicnt = 2;
    }
    if (beam_posi(pActwk, a2)) {
        if (pActwk->userflag.b.l) {
            if (boss7_get_work(pActwk)->byte_timer <= 0) {
                --boss7_get_work(pActwk)->byte_timer;
                make_kemuri(pActwk);
                boss7_get_work(pActwk)->byte_timer = 10;
            } else {
                --boss7_get_work(pActwk)->byte_timer;
            }

            if (boss7_get_work(pActwk)->timer_low <= 0) {
                --boss7_get_work(pActwk)->timer_low;
                make_hahen(pActwk);
                boss7_get_work(pActwk)->timer_low = 9;
            } else {
                --boss7_get_work(pActwk)->timer_low;
            }
        }

        patchg(pActwk, egg7beam_pchg);
        actionsub(pActwk);
    } else {
        boss7_get_work(pActwk)->timer = 0;
    }
}

static Sint16 beam_posi(sprite_status *pActwk, sprite_status *a2) {
    Sint16 d0, d1;

    pActwk->xposi.w.h = a2->xposi.w.h;
    pActwk->yposi.w.h = a2->yposi.w.h;
    d0 = boss7_get_work(pActwk)->x_offset;
    d1 = boss7_get_work(pActwk)->y_offset;
    pActwk->yposi.w.h += d1;

    if (!(a2->cddat & 1)) {
        pActwk->cddat &= 254;
        pActwk->xposi.w.h += d0;
    } else {
        pActwk->cddat |= 1;
        pActwk->xposi.w.h -= d0;
    }

    boss7_get_work(pActwk)->flags &= 223;

    if (boss7_get_work(a2)->flags & 32) {
        boss7_get_work(pActwk)->flags |= 32;
    }
    if (boss7_get_work(a2)->flags & 64) {
        boss7_get_work(pActwk)->flags |= 64;
        return -1;
    }

    boss7_get_work(pActwk)->flags &= 191;
    return 0;
}

static void egg7beam_kemuri1(sprite_status *pActwk, sprite_status *a2) {
    beam_posi(pActwk, a2);

    b_timer(pActwk);
    if (boss7_get_work(pActwk)->step) {
        boss7_get_work(pActwk)->step = 0;
        pActwk->r_no0 = 6;

        boss7_get_work(pActwk)->byte_timer = 15;
    }
    kemuri_disp(pActwk);
}

static void egg7beam_kemuri2(sprite_status *pActwk, sprite_status *a2) {
    b_timer(pActwk);
    if (boss7_get_work(pActwk)->step == 0)
        kemuri_disp(pActwk);
    else
        frameout(pActwk);
}

static void kemuri_disp(sprite_status *pActwk) {
    static Uint8 pchg0[7] = {5, 0, 1, 2, 3, 4, 255};

    static Uint8 *kemuri_pchg[1] = {pchg0};

    patchg(pActwk, kemuri_pchg);
    actionsub(pActwk);
}

void hahen73(sprite_status *pActwk) {
    int_union ld0, ld1;
    Sint16 d0, d1;

    if (!(pActwk->r_no0 & 1)) {
        pActwk->r_no0 |= 1;

        pActwk->actflg = 4;
        pActwk->sprpri = 5;
        pActwk->sprhsize = 4;
        pActwk->sprvsize = 4;
        pActwk->sproffset = 17692;
        pActwk->patbase = hahen73_pat;

        ld0.l = random();
        ld0.l = ld0.w.l;

        ld1.l = ld0.l;
        ld0.l &= 32767;
        ld1.w.l = ld1.l % 448;

        pActwk->xspeed.w += ld1.w.l;

        pActwk->yspeed.w = -384;

        boss7_get_work(pActwk)->accel_y = 16;
        ld1.w.l = ld0.w.l;
        ld1.w.l &= 15;
        boss7_get_work(pActwk)->accel_y += ld1.w.l;

        ld0.w.l &= 3;
        pActwk->patno = ld0.w.l;
    } else {
        pActwk->r_no0 |= 1;

        add_spd2(pActwk);

        d0 = pActwk->yposi.w.h;
        d1 = scra_v_posit.w.h;
        d1 += 232;
        d0 -= d1;
        if (d0 <= 0) {
            actionsub(pActwk);
        } else {
            frameout(pActwk);
        }
    }
}

void egg7jet(sprite_status *pActwk) {
    void (*tbl[2])(sprite_status *) = {&egg7jet_ini, &egg7jet_01};

    tbl[pActwk->r_no0 / 2](pActwk);
}

static void egg7jet_ini(sprite_status *pActwk) {
    pActwk->actflg = 4;
    pActwk->sprpri = 5;
    pActwk->sprhsize = 20;
    pActwk->sprvsize = 8;
    pActwk->sproffset = 8960;
    pActwk->patbase = egg7jet_pat;

    if (pActwk->userflag.b.h == 0) {
        boss7_get_work(pActwk)->x_offset = -51;
        boss7_get_work(pActwk)->y_offset = 26;
    } else {
        boss7_get_work(pActwk)->x_offset = -48;
        boss7_get_work(pActwk)->y_offset = -14;
    }
    pActwk->r_no0 += 2;

    egg7jet_01(pActwk);
}

static void egg7jet_01(sprite_status *pActwk) {
    sprite_status *a2;
    Sint16 d0;

    a2 = &actwk[boss7_get_work(pActwk)->parent_index];
    if (a2->actno == 0) {
        frameout(pActwk);
        return;
    }
    if (!(boss7_get_work(a2)->flags & 8))
        return;

    pActwk->cddat = a2->cddat;
    pActwk->xposi.w.h = a2->xposi.w.h;
    pActwk->yposi.w.h = a2->yposi.w.h;

    d0 = boss7_get_work(pActwk)->x_offset;
    if (pActwk->cddat & 1) {
        d0 *= -1;
    }
    pActwk->xposi.w.h += d0;
    d0 = boss7_get_work(pActwk)->y_offset;
    pActwk->yposi.w.h += d0;
    patchg(pActwk, egg7jet_pchg);
    actionsub(pActwk);
}

void msnc_bara(sprite_status *pActwk) {
    void (*tbl[2])(sprite_status *) = {&msnc_bara_ini, &msnc_bara_01};

    tbl[pActwk->r_no0 / 2](pActwk);
    if (QuickReturn) {
        QuickReturn = 0;
        return;
    }
    actionsub(pActwk);
}

static void msnc_bara_ini(sprite_status *pActwk) {
    pActwk->actflg = 4;
    pActwk->sprpri = 1;
    pActwk->sprhsize = 8;
    pActwk->sprvsize = 8;
    pActwk->sproffset = 892;
    pActwk->patbase = msnc_bara_pat;

    msnc_bara_01(pActwk);
}

static void msnc_bara_01(sprite_status *pActwk) {
    add_spd2(pActwk);
    if (pActwk->yposi.w.h >= 624) {
        frameout(pActwk);
        QuickReturn = 1;
    }
}

static void add_spd3(sprite_status *pActwk) {
    Sint16 d0;

    d0 = boss7_get_work(pActwk)->accel_y;
    pActwk->yspeed.w += d0;

    d0 = boss7_get_work(pActwk)->accel_x;
    pActwk->xspeed.w += d0;

    if (boss7_get_work(pActwk)->accel_x == 0) {
        add_spd(pActwk);
        return;
    }
    if (boss7_get_work(pActwk)->accel_x >= 0) {
        d0 = boss7_get_work(pActwk)->max_xspeed;
        if (d0 <= pActwk->xspeed.w) {
            pActwk->xspeed.w = d0;
        }
        add_spd(pActwk);
    } else {
        d0 = boss7_get_work(pActwk)->max_xspeed;
        if (d0 >= pActwk->xspeed.w) {
            pActwk->xspeed.w = d0;
        }
        add_spd(pActwk);
    }
}

void hari_spdadd(sprite_status *pActwk) { add_spd2(pActwk); }

static void add_spd2(sprite_status *pActwk) {
    Sint16 d0;

    d0 = boss7_get_work(pActwk)->accel_x;
    pActwk->xspeed.w += d0;
    d0 = boss7_get_work(pActwk)->accel_y;
    pActwk->yspeed.w += d0;

    add_spd(pActwk);
}

static void add_spd(sprite_status *pActwk) {
    Sint32 ld0;

    ld0 = pActwk->xspeed.w;
    ld0 <<= 8;
    pActwk->xposi.l += ld0;
    ld0 = pActwk->yspeed.w;
    ld0 <<= 8;
    pActwk->yposi.l += ld0;
}

static void w_timer(sprite_status *pActwk) {
    --boss7_get_work(pActwk)->timer;
    if (boss7_get_work(pActwk)->timer == 0) {
        ++boss7_get_work(pActwk)->step;
    }
}

static void b_timer(sprite_status *pActwk) {
    --boss7_get_work(pActwk)->byte_timer;
    if (boss7_get_work(pActwk)->byte_timer == 0) {
        ++boss7_get_work(pActwk)->step;
    }
}

static void event_timer(sprite_status *pActwk, Sint16 *pTimer,
                        void (**func)(sprite_status *)) {
    Sint16 d0;

    d0 = (Uint16)boss7_get_work(pActwk)->step;
    d0 = pTimer[d0];
    if (d0 != boss7_get_work(pActwk)->timer) {
        ++boss7_get_work(pActwk)->timer;
    } else {
        ++boss7_get_work(pActwk)->timer;

        d0 = (Uint16)boss7_get_work(pActwk)->step;
        ++boss7_get_work(pActwk)->step;
        func[d0](pActwk);
    }
}

static void make_bara(sprite_status *pActwk) {
    sprite_status *pNewActwk;
    int_union ld0, ld1;
    Sint16 d0, d3;

    d3 = 9;
    do {
        if (actwkchk2(pActwk, &pNewActwk) != 0)
            return;
        d0 = d3;
        d0 = d0 % 5;
        pNewActwk->patno = (Uint16)d0;

        boss7_get_work(pNewActwk)->parent_index = (Uint16)(pActwk - actwk);
        pNewActwk->actno = 49;
        pNewActwk->xposi.w.h = pActwk->xposi.w.h;
        pNewActwk->yposi.w.h = pActwk->yposi.w.h;

        ld0.l = random();
        ld0.l = ld0.w.l;

        ld1.l = ld0.l;
        ld1.l |= -32768;
        ld1.w.l = ld1.l % 640;
        pNewActwk->xspeed.w = ld1.w.l;

        pNewActwk->yspeed.w = -512;

        boss7_get_work(pNewActwk)->accel_y = 12;
        ld1.w.l = ld0.w.l;
        ld1.w.l &= 15;
        boss7_get_work(pNewActwk)->accel_y += ld1.w.l;
    } while (d3--);
}

static void make_ele(sprite_status *pActwk) {
    sprite_status *pNewActwk;

    if (actwkchk2(pActwk, &pNewActwk) == 0) {
        boss7_get_work(pNewActwk)->parent_index = (Uint16)(pActwk - actwk);
        pNewActwk->actno = 48;
    }
}

static void make_msnc(sprite_status *pActwk) {
    sprite_status *pNewActwk;

    if (actwkchk2(pActwk, &pNewActwk) == 0) {
        boss7_get_work(pNewActwk)->parent_index = (Uint16)(pActwk - actwk);
        boss7_get_work(pActwk)->parent_index = (Uint16)(pNewActwk - actwk);
        pNewActwk->actno = 47;
        pNewActwk->xposi.w.h = 3120;
        pNewActwk->yposi.w.h = 461;
    }
}

static void make_jet(sprite_status *pActwk) {
    sprite_status *pNewActwk;

    if (actwkchk2(pActwk, &pNewActwk) == 0) {
        boss7_get_work(pNewActwk)->parent_index = (Uint16)(pActwk - actwk);
        pNewActwk->actno = 45;
        pNewActwk->userflag.b.h = 0;
        pNewActwk->mstno.b.h = 0;
        if (actwkchk2(pActwk, &pNewActwk) == 0) {
            boss7_get_work(pNewActwk)->parent_index = (Uint16)(pActwk - actwk);
            pNewActwk->actno = 45;
            pNewActwk->userflag.b.h = 1;
            pNewActwk->mstno.b.h = 1;
        }
    }
}

static void make_beam(sprite_status *pActwk) {
    sprite_status *pNewActwk;
    sprite_status *a3;
    Sint16 d2;

    d2 = 3;
    a3 = pActwk;
    do {
        if (actwkchk2(pActwk, &pNewActwk) != 0)
            break;
        boss7_get_work(pNewActwk)->parent_index = (Uint16)(a3 - actwk);
        boss7_get_work(pNewActwk)->owner_index = (Uint16)(pActwk - actwk);

        pNewActwk->actno = 46;
        a3 = pNewActwk;
        if (d2 == 3)
            goto label1;
        if (d2 == 0) {

            pNewActwk->userflag.b.l = 1;
        }
        boss7_get_work(pNewActwk)->x_offset = 2;
        boss7_get_work(pNewActwk)->y_offset = 32;
        continue;
    label1:
        boss7_get_work(pNewActwk)->x_offset = 0;
        boss7_get_work(pNewActwk)->y_offset = 52;
    } while (d2--);
}

static void make_kemuri(sprite_status *pActwk) {
    sprite_status *pNewActwk;

    if (actwkchk2(pActwk, &pNewActwk) == 0) {
        boss7_get_work(pNewActwk)->parent_index = (Uint16)(pActwk - actwk);
        pNewActwk->actno = 46;
        pNewActwk->userflag.b.h = 1;
        boss7_get_work(pNewActwk)->x_offset = 2;
        boss7_get_work(pNewActwk)->y_offset = 16;
    }
}

static void make_bakuha(sprite_status *pActwk) {
    sprite_status *pNewActwk;

    if (actwkchk2(pActwk, &pNewActwk) == 0) {
        boss7_get_work(pNewActwk)->parent_index = (Uint16)(pActwk - actwk);
        pNewActwk->actno = 46;
        pNewActwk->userflag.b.h = 2;
        pNewActwk->xposi.w.h = pActwk->xposi.w.h;
        pNewActwk->yposi.w.h = pActwk->yposi.w.h;

        soundset(158);
    }
}

static void make_hahen(sprite_status *pActwk) {
    sprite_status *pNewActwk;
    sprite_status *a2;

    if (actwkchk2(pActwk, &pNewActwk) == 0) {
        pNewActwk->actno = 55;
        pNewActwk->xposi.w.h = pActwk->xposi.w.h;
        pNewActwk->yposi.w.h = pActwk->yposi.w.h;
        pNewActwk->xposi.w.h += 2;
        pNewActwk->yposi.w.h += 16;
        a2 = &actwk[boss7_get_work(pActwk)->owner_index];
        pNewActwk->xspeed.w = a2->xspeed.w;
    }
}
