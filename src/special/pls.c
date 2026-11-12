#include "../types.h"
#include "common.h"
#include "sps_equ.h"
#include "pls.h"
#include "pls_work.h"
#include "act_s.h"
#include "coli_s.h"
#include "etc_s.h"
#include "game.h"
#include "sin.h"

void (*pl_acttbl[24])(sprite_status_sp *) = {
    &play0,  &play1,  &play2,  &play3,  &play4,  &play5,  &play6,  &play7,
    &play8,  &play9,  &play10, &play11, &play12, &play13, &play14, &play15,
    &play16, &play17, &play18, &play19, &play20, &play21, &play22, &play23};
extern sprite_patterns_sp *mpplayer[];
extern sprite_patterns_sp *mpsibuki[];
static sprite_patterns_sp *mpkemuri[1];
static sprite_patterns_sp *mpstone[3];

void player(sprite_status_sp *plwk) {

    swdata_pl.w = swdata1.w;
    speed_ud(plwk);
    plwk->actflg &= 219;
    plwk->spdcnt_max = 2560;
    pl_acttbl[plwk->exeno](plwk);
    if (!(plwk->actflg & 8)) {
        if (!(plwk->actflg & 192))

            if (game_start == 0) {
                plcolli(plwk);
                colli_evt(plwk);
            }
        if (!(plwk->actflg & 32))
            go_down(plwk);
        if (plwk->x_posi.w.h < 832)
            plwk->x_posi.w.h = 832;
    }
    if (plwk->x_posi.w.h >= 3264)
        plwk->x_posi.w.h = 3264;
    if (plwk->y_posi.w.h < 832)
        plwk->y_posi.w.h = 832;
    if (plwk->y_posi.w.h >= 3264)
        plwk->y_posi.w.h = 3264;
    zoomwk.x = plwk->x_posi.w.h;
    zoomwk.y = plwk->y_posi.w.h;
    zoomwk.z = plwk->z_posi.w.h;
    zoomwk.ax = special_player_get_work(plwk)->angle_x;
    zoomwk.az = special_player_get_work(plwk)->angle_z;
    if (plwk->exeno != 7 && plwk->exeno != 19) {
        ptrlget(plwk);
        plptsel(plwk);
    }
    patset(plwk);
    plcgchg(plwk);
    plwk->colliflg = 0;
}

void play0(sprite_status_sp *plwk) {

    plwk->pattbl = mpplayer;
    plwk->sx_posi.w.h = 256;
    plwk->sy_posi.w.h = 344;
    patinit(plwk, 9);
    plwk->exeno = 20;
    plwk->spdcnt = 0;
    init_xyget(plwk);
    plwk->z_posi.w.h = 352;
    special_player_get_work(plwk)->angle_x = 128;
}

void play20(sprite_status_sp *plwk) { plwk->actno = plwk->actno; }

void play21(sprite_status_sp *plwk) {
    patinit(plwk, 44);
    plwk->exeno = 22;
    special_player_get_work(plwk)->state_timer = 5;
}

void play22(sprite_status_sp *plwk) {
    if (--special_player_get_work(plwk)->state_timer == 0) {
        patinit(plwk, 10);
        plwk->exeno = 23;
    }
}

void play23(sprite_status_sp *plwk) { plwk->actno = plwk->actno; }

void play1(sprite_status_sp *plwk) {
    rlmove(plwk);
    jumpset(plwk);
    clear_chk();
}

void play2(sprite_status_sp *plwk) {
    plwk->sy_posi.l += plwk->sy_speed.l;
    plwk->sy_speed.l += 32767 + 8193;
    if (jmpswcnt != 0) {
        plwk->sy_speed.l += 32767 + 8193;
        --jmpswcnt;
        if (swdata_pl.b.h & 112)
            plwk->sy_speed.l -= 40960;
    }
    if (plwk->sy_posi.w.h >= 344) {
        plwk->exeno = 1;
        plwk->sy_posi.l = 22544384;
        plwk->sy_speed.l = 0;
        plwk->actflg &= 127;
        plwk->z_posi.w.h = 352;
    }
    rlmove(plwk);
    plwk->z_posi.w.h = (344 - plwk->sy_posi.w.h << 2) + 352;
}

void play3(sprite_status_sp *plwk) {
    plwk->sy_posi.l += plwk->sy_speed.l;
    plwk->sy_speed.l += 8192;
    if (plwk->sy_posi.w.h >= 344) {
        plwk->exeno = 1;
        plwk->sy_posi.l = 22544384;
        plwk->sy_speed.l = 0;
        plwk->actflg &= 191;
        plwk->z_posi.w.h = 352;
    }
    rlmove(plwk);
    plwk->z_posi.w.h = (344 - plwk->sy_posi.w.h << 2) + 352;
}

void play4(sprite_status_sp *plwk) {
    if (--plwk->spdcnt_shock == 0) {
        plwk->exeno = 1;
        plwk->x_speed.l = plwk->y_speed.l = 0;
    } else {
        plwk->x_posi.l += plwk->x_speed.l;
        plwk->y_posi.l += plwk->y_speed.l;
    }
    rlmove(plwk);
    jumpset(plwk);
    clear_chk();
}

void play5(sprite_status_sp *plwk) {
    plwk->sy_posi.w.h += 4;
    if (plwk->sy_posi.w.h >= 448)
        plwk->exeno = 6;
}

void play6(sprite_status_sp *plwk) { plwk->actno = plwk->actno; }

void play7(sprite_status_sp *plwk) {
    if (--special_player_get_work(plwk)->state_timer == 0) {
        plwk->exeno = 1;
        patinit(plwk, 0);
    }
}

void play8(sprite_status_sp *plwk) {
    if (plwk->sy_posi.w.h >= 344) {
        special_player_get_work(plwk)->state_timer = 60;
        patinit(plwk, 10);
        plwk->exeno = 9;
        game_start = 1;
        plwk->spdcnt = 0;
    } else {
        rlmove(plwk);
        jumpset(plwk);
    }
}

void play9(sprite_status_sp *plwk) {
    if (--special_player_get_work(plwk)->state_timer == 0) {
        actwk[5].actno = 14;
        actwk[6].actno = 15;
        actwk[7].actno = 16;
        plwk->exeno = 10;
        special_player_get_work(plwk)->state_timer = 6;
    }
}

void play10(sprite_status_sp *plwk) {
    rlscrflg |= 4;
    special_player_get_work(plwk)->angle_z -= 8;
    special_player_get_work(plwk)->angle_z &= 511;
    if (--special_player_get_work(plwk)->state_timer == 0) {
        plwk->exeno = 11;
        special_player_get_work(plwk)->state_timer = 4;
        patinit(plwk, 36);
    }
}

void play11(sprite_status_sp *plwk) {
    rlscrflg |= 4;
    special_player_get_work(plwk)->angle_z -= 8;
    special_player_get_work(plwk)->angle_z &= 511;
    if (--special_player_get_work(plwk)->state_timer == 0) {
        plwk->exeno = 12;
        special_player_get_work(plwk)->state_timer = 5;
        patinit(plwk, 37);
    }
}

void play12(sprite_status_sp *plwk) {
    rlscrflg |= 4;
    special_player_get_work(plwk)->angle_z -= 8;
    special_player_get_work(plwk)->angle_z &= 511;
    if (--special_player_get_work(plwk)->state_timer == 0) {
        plwk->exeno = 13;
        special_player_get_work(plwk)->state_timer = 4;
        patinit(plwk, 38);
    }
}

void play13(sprite_status_sp *plwk) {
    rlscrflg |= 4;
    special_player_get_work(plwk)->angle_z -= 8;
    special_player_get_work(plwk)->angle_z &= 511;
    if (--special_player_get_work(plwk)->state_timer == 0) {
        plwk->exeno = 14;
        special_player_get_work(plwk)->state_timer = 5;
        patinit(plwk, 39);
    }
}

void play14(sprite_status_sp *plwk) {
    rlscrflg |= 4;
    special_player_get_work(plwk)->angle_z -= 8;
    special_player_get_work(plwk)->angle_z &= 511;
    if (--special_player_get_work(plwk)->state_timer == 0) {
        plwk->exeno = 15;
        special_player_get_work(plwk)->state_timer = 4;
        patinit(plwk, 40);
    }
}

void play15(sprite_status_sp *plwk) {
    rlscrflg |= 4;
    special_player_get_work(plwk)->angle_z -= 8;
    special_player_get_work(plwk)->angle_z &= 511;
    if (--special_player_get_work(plwk)->state_timer == 0) {
        plwk->exeno = 16;
        special_player_get_work(plwk)->state_timer = 5;
        patinit(plwk, 41);
    }
}

void play16(sprite_status_sp *plwk) {
    rlscrflg |= 4;
    special_player_get_work(plwk)->angle_z -= 8;
    special_player_get_work(plwk)->angle_z &= 511;
    if (--special_player_get_work(plwk)->state_timer == 0) {
        plwk->exeno = 17;
        patinit(plwk, 42);
    }
}

void play17(sprite_status_sp *plwk) { plwk->actno = plwk->actno; }

void play18(sprite_status_sp *plwk) { patinit(plwk, 43); }

void play19(sprite_status_sp *plwk) {
    if (--plwk->spdcnt_shock == 0) {
        plwk->exeno = 1;
        plwk->x_speed.l = plwk->y_speed.l = 0;
        patinit(plwk, 0);
    } else {
        plwk->x_posi.l += plwk->x_speed.l;
        plwk->y_posi.l += plwk->y_speed.l;
    }
    rlmove(plwk);
    jumpset(plwk);
}

void init_xyget(sprite_status_sp *plwk) {
    Sint16 tbl[24] = {1344, 1312, 128,  1280, 1280, 128,  1280, 1280,
                      128,  1280, 1280, 128,  1280, 1280, 128,  1216,
                      1216, 128,  1280, 1280, 128,  1024, 1152, 128};

    plwk->x_posi.w.h = tbl[stagenm * 3];
    plwk->y_posi.w.h = tbl[stagenm * 3 + 1];
    special_player_get_work(plwk)->angle_z = tbl[stagenm * 3 + 2];
}

void clear_chk(void) {
    if (ufoleft != 0)
        return;
    actwk[0].exeno = 8;
    if (actwk[39].actno != 0)
        actwk[39].actflg |= 1;
    if (actwk[47].actno != 0)
        actwk[47].actflg |= 1;
}

void colli_evt(sprite_status_sp *plwk) {
    Uint8 cal_jump_no;
    void (*jmptbl[10])(sprite_status_sp *) = {&noevt, &noevt, &evt01, &evt02,
                                              &evt03, &evt04, &evt05, &evt06,
                                              &evt07, &evt08};

    boundchk(plwk);
    if (ufoleft == 0)
        return;
    if ((cal_jump_no = plwk->scno_ce) >= 10)
        cal_jump_no = 0;
    jmptbl[cal_jump_no](plwk);
}

void noevt(sprite_status_sp *plwk) { plwk->actno = plwk->actno; }

void evt08(sprite_status_sp *plwk) { plwk->actno = plwk->actno; }

void evt01(sprite_status_sp *plwk) {
    plwk->exeno = 3;
    plwk->sy_speed.l = -0x40000;
    plwk->actflg |= 64;
    key_set(184);
    sibuki_clr();
}

void evt02(sprite_status_sp *plwk) {
    if (time_stop != 0)
        return;
    actwk[2].actno = 8;
    if (spgmmode & 2)
        plwk->spdcnt_max = 1280;
}

void evt03(sprite_status_sp *plwk) {
    sprite_status_sp *new_actwk;

    plwk->spdcnt_max = 1280;
    if (plwk->spdcnt < 256)
        return;
    if (k_search(&new_actwk) == 0)
        new_actwk->actno = 7;

    key_set(214);
}

void evt04(sprite_status_sp *plwk) {
    plwk->exeno = 2;
    plwk->sy_speed.l = -0x100000;
    plwk->actflg |= 128;
    key_set(152);
    sibuki_clr();
}

void evt05(sprite_status_sp *plwk) {
    Sint16 cal_ring;

    if (plwk->exeno == 4 || plwk->exeno == 7)
        return;
    special_player_get_work(plwk)->state_timer = 46;
    plwk->exeno = 7;
    patinit(plwk, 13);
    cal_ring = ringno - (ringno >> 1);
    ringno >>= 1;
    if (cal_ring > 7)
        cal_ring = 7;
    if (cal_ring > 0) {
        key_set(148);
    }
    for (; cal_ring > 0; --cal_ring)
        actwk[24 - cal_ring].actno = 13;
}

void evt06(sprite_status_sp *plwk) {
    void (*tbl[8])(sprite_status_sp *) = {&tobi_b0, &tobi_b2, &tobi_b4,
                                          &tobi_b6, &tobi_b0, &tobi_b6,
                                          &tobi_b4, &tobi_b2};

    key_set(206);
    patinit(plwk, 14);

    tbl[(plwk->scno_sdir & 14) / 2](plwk);
}

void tobi_b0(sprite_status_sp *plwk) { tobi_b(plwk, 0, -24); }

void tobi_b2(sprite_status_sp *plwk) { tobi_b(plwk, -24, 0); }

void tobi_b4(sprite_status_sp *plwk) { tobi_b(plwk, 0, 24); }

void tobi_b6(sprite_status_sp *plwk) { tobi_b(plwk, 24, 0); }

void tobi_b(sprite_status_sp *plwk, Sint16 cal_x, Sint16 cal_y) {
    plwk->x_speed.w.h = cal_x;
    plwk->y_speed.w.h = cal_y;
    plwk->spdcnt_shock = 20;
    plwk->exeno = 19;
}

void evt07(sprite_status_sp *plwk) {
    void (*tbl[8])(sprite_status_sp *) = {&tobi_s0, &tobi_s2, &tobi_s4,
                                          &tobi_s6, &tobi_s0, &tobi_s6,
                                          &tobi_s4, &tobi_s2};

    key_set(195);

    tbl[(plwk->scno_sdir & 14) / 2](plwk);
}

void tobi_s0(sprite_status_sp *plwk) { tobi_s(plwk, 0, -16); }

void tobi_s2(sprite_status_sp *plwk) { tobi_s(plwk, -16, 0); }

void tobi_s4(sprite_status_sp *plwk) { tobi_s(plwk, 0, 16); }

void tobi_s6(sprite_status_sp *plwk) { tobi_s(plwk, 16, 0); }

void tobi_s(sprite_status_sp *plwk, Sint16 cal_x, Sint16 cal_y) {
    plwk->x_speed.w.h = cal_x;
    plwk->y_speed.w.h = cal_y;
    plwk->spdcnt_shock = 8;
    plwk->exeno = 4;
}

void boundchk(sprite_status_sp *plwk) {
    Sint16 branch_d0;
    Sint32 speed_d1, speed_d2, speed_d3;

    speed_d1 = (plwk->spdcnt << 8) + 0x20000;
    speed_d2 = -speed_d1;
    speed_d3 = 0;

    branch_d0 = 0;
    if (plwk->scno_ul == 1)
        branch_d0 |= 1;
    if (plwk->scno_ur == 1)
        branch_d0 |= 2;
    if (plwk->scno_dr == 1)
        branch_d0 |= 4;
    if (plwk->scno_dl == 1)
        branch_d0 |= 8;

    switch (branch_d0) {
    case 1:
        plwk->x_speed.l = plwk->y_speed.l = speed_d1;
        break;
    case 2:
        plwk->x_speed.l = speed_d2;
        plwk->y_speed.l = speed_d1;
        break;
    case 3:
        plwk->x_speed.l = speed_d3;
        plwk->y_speed.l = speed_d1;
        break;
    case 4:
        plwk->x_speed.l = plwk->y_speed.l = speed_d2;
        break;
    case 5:
        plwk->x_speed.l = speed_d1;
        plwk->y_speed.l = speed_d2;
        break;
    case 6:
        plwk->x_speed.l = speed_d2;
        plwk->y_speed.l = speed_d3;
        break;
    case 7:
        plwk->x_speed.l = speed_d2;
        plwk->y_speed.l = speed_d1;
        break;
    case 8:
        plwk->x_speed.l = speed_d1;
        plwk->y_speed.l = speed_d2;
        break;
    case 9:
        plwk->x_speed.l = speed_d1;
        plwk->y_speed.l = speed_d3;
        break;
    case 10:
        plwk->x_speed.l = plwk->y_speed.l = speed_d2;
        break;
    case 11:
        plwk->x_speed.l = plwk->y_speed.l = speed_d1;
        break;
    case 12:
        plwk->x_speed.l = speed_d3;
        plwk->y_speed.l = speed_d2;
        break;
    case 13:
        plwk->x_speed.l = speed_d1;
        plwk->y_speed.l = speed_d2;
        break;
    case 14:
        plwk->x_speed.l = plwk->y_speed.l = speed_d2;
        break;
    case 15:
        plwk->x_speed.l = plwk->y_speed.l = speed_d2;
        break;
    }

    if (branch_d0 == 0)
        return;

    plwk->spdcnt_shock = 16;
    plwk->exeno = 4;
    plwk->actflg |= 32;
    key_set(181);
    patinit(plwk, 0);
}

Sint16 k_search(sprite_status_sp **new_actwk) {
    Sint16 i;

    for (i = 8; i < 16; ++i)
        if ((*new_actwk = &actwk[i])->actno == 0)
            return 0;
    return -1;
}

void jumpset(sprite_status_sp *plwk) {
    if (game_start != 0)
        return;
    if (!(swdata_pl.b.l & 112))
        return;
    plwk->exeno = 2;
    plwk->sy_speed.l = -524288;
    jmpswcnt = 20;
    plwk->actflg |= 128;
    plwk->spdcnt_shock = 0;
    key_set(146);
    sibuki_clr();
}

void rlmove(sprite_status_sp *plwk) {
    if (game_start != 0)
        return;
    if (swdata_pl.b.h & 8) {
        special_player_get_work(plwk)->angle_z_accum -= 393216;
        special_player_get_work(plwk)->angle_z_accum &= 33554431;
        rlscrflg |= 8;
    }
    if (swdata_pl.b.h & 4) {
        special_player_get_work(plwk)->angle_z_accum += 393216;
        special_player_get_work(plwk)->angle_z_accum &= 33554431;
        rlscrflg |= 4;
    }
}

void rlmove_j(sprite_status_sp *plwk) {
    if (game_start != 0)
        return;
    if (swdata_pl.b.h & 8) {
        special_player_get_work(plwk)->angle_z_accum -= 0x40000;
        special_player_get_work(plwk)->angle_z_accum &= 33554431;
        rlscrflg |= 8;
    }
    if (swdata_pl.b.h & 4) {
        special_player_get_work(plwk)->angle_z_accum += 0x40000;
        special_player_get_work(plwk)->angle_z_accum &= 33554431;
        rlscrflg |= 4;
    }
}

void speed_ud(sprite_status_sp *plwk) {
    Sint16 max_speed;

    if (game_start != 0)
        return;
    if ((swdata_pl.b.h & 15) != 2) {
        if (special_player_get_work(plwk)->speed_boost_timer != 0) {
            --special_player_get_work(plwk)->speed_boost_timer;
            max_speed = 3584;
        } else {
            if (plwk->exeno == 7)
                max_speed = 512;
            else
                max_speed = plwk->spdcnt_max;
        }
        plwk->spdcnt += 32;
        if (max_speed < plwk->spdcnt)
            plwk->spdcnt = max_speed;
        return;
    }

    plwk->spdcnt -= 64;
    if (plwk->spdcnt < 512)
        plwk->spdcnt = 512;
}

void go_down(sprite_status_sp *plwk) { go_x_plus(plwk, 384, plwk->spdcnt); }

void go_up(sprite_status_sp *plwk) { go_x_plus(plwk, 128, plwk->spdcnt_shock); }

void go_right(sprite_status_sp *plwk) {
    go_x_plus(plwk, 0, plwk->spdcnt_shock);
}

void go_left(sprite_status_sp *plwk) {
    go_x_plus(plwk, 256, plwk->spdcnt_shock);
}

void go_x_plus(sprite_status_sp *plwk, Uint16 cal_plus, Sint16 cal_speed) {
    Sint16 cal_z_kaku;
    Sint32 cal_plus_position;

    cal_z_kaku = special_player_get_work(plwk)->angle_z + cal_plus;
    cal_z_kaku &= 511;
    cal_plus_position = sp_cos(cal_z_kaku);
    cal_plus_position *= cal_speed;
    plwk->x_posi.l += cal_plus_position;
    plwk->x_posi.l = plwk->x_posi.l & 0xFFFFFFF;
    cal_plus_position = sp_sin(cal_z_kaku);
    cal_plus_position *= cal_speed;
    plwk->y_posi.l += cal_plus_position;
    plwk->y_posi.l = plwk->y_posi.l & 0xFFFFFFF;
}

void ptrlget(sprite_status_sp *plwk) {
    if (swdata_pl.b.h & 4) {
        if ((Sint8)--special_player_get_work(plwk)->facing_frame < 0)
            special_player_get_work(plwk)->facing_frame = 0;
        return;
    }
    if (swdata_pl.b.h & 8) {
        if (++special_player_get_work(plwk)->facing_frame >= 10)
            special_player_get_work(plwk)->facing_frame = 9;
        return;
    }

    special_player_get_work(plwk)->facing_frame = 5;
}

void plptsel(sprite_status_sp *plwk) {
    Uint8 cal_pattblno;
    Uint8 tbl[20] = {4,  25, 26, 27, 3,  22, 23, 24, 0,  16,
                     17, 18, 2,  19, 20, 21, 5,  28, 29, 30};

    if (game_start != 0)
        return;
    cal_pattblno = 6;
    if (plwk->actflg & 128)
        goto label1;
    cal_pattblno = 11;
    if (plwk->actflg & 64)
        goto label1;
    cal_pattblno = 10;
    if (plwk->spdcnt == 0)
        goto label1;
    cal_pattblno =
        tbl[(special_player_get_work(plwk)->facing_frame * 2 & 28) + 3];
    if (plwk->spdcnt < 768)
        goto label3;
    cal_pattblno =
        tbl[(special_player_get_work(plwk)->facing_frame * 2 & 28) + 2];
    if (plwk->spdcnt < 1344)
        goto label3;
    cal_pattblno =
        tbl[(special_player_get_work(plwk)->facing_frame * 2 & 28) + 1];
    if (plwk->spdcnt < 1920)
        goto label3;
    cal_pattblno =
        tbl[special_player_get_work(plwk)->facing_frame * 2 & 28];
    if (plwk->spdcnt < 2816)
        goto label3;
    cal_pattblno = 1;
    goto label2;

label1:
    plwk->actflg &= 239;
label2:
    if (cal_pattblno != plwk->pattblno)
        patinit(plwk, cal_pattblno);
    return;
label3:
    if (!(plwk->actflg & 16)) {
        plwk->actflg |= 16;
        goto label2;
    }
    if (cal_pattblno != plwk->pattblno)
        patinit1(plwk, cal_pattblno);
}

void plcgchg(sprite_status_sp *plwk) { plwk->actno = plwk->actno; }

void sibuki(sprite_status_sp *sibukiwk) {
    void (*acttbl[3])(sprite_status_sp *) = {&sibuki0, &sibuki1, &sibuki2};

    acttbl[sibukiwk->exeno](sibukiwk);
    patset(sibukiwk);
    if (time_stop != 0)
        sibukiwk->actflg |= 1;
}

void sibuki0(sprite_status_sp *sibukiwk) {

    sibukiwk->pattbl = mpsibuki;
    sibukiwk->sx_posi.w.h = 256;
    sibukiwk->sy_posi.w.h = 344;
    patinit(sibukiwk, 0);
    special_player_get_work(sibukiwk)->timer = 14;
    ++sibukiwk->exeno;
    key_set(162);
    if (!(spgmmode & 2))
        tdecflg = 10;
    sibuki1(sibukiwk);
}

void sibuki1(sprite_status_sp *sibukiwk) {
    if (--special_player_get_work(sibukiwk)->timer != 0)
        return;
    if (actwk[0].scno_ce != 3) {
        sibukiwk->actflg |= 1;
        return;
    }
    patinit(sibukiwk, 1);
    sibukiwk->exeno = 2;
}

void sibuki2(sprite_status_sp *sibukiwk) {
    if (actwk[0].scno_ce != 3)
        sibukiwk->actflg |= 1;
    else if (sibukiwk->patno == 0)
        tdecflg = 2;
}

void sibuki_clr(void) {
    if (actwk[2].actno != 0)
        actwk[2].actflg |= 1;
}

static sprite_pattern kem00 = {1, {{-4, -4, 0, 111}}};
static sprite_pattern kem01 = {1, {{-4, -4, 0, 112}}};
static sprite_pattern kem02 = {1, {{-4, -4, 0, 113}}};
static sprite_patterns_sp kemuri00 = {3, 1, {&kem02, &kem01, &kem00}};
static sprite_patterns_sp *mpkemuri[1] = {&kemuri00};

void kemuri(sprite_status_sp *kemuriwk) {
    void (*acttbl[2])(sprite_status_sp *) = {&kemuri0, &kemuri1};

    acttbl[kemuriwk->exeno](kemuriwk);
    patset(kemuriwk);
}

void kemuri0(sprite_status_sp *kemuriwk) {
    uint_union cal_random;

    kemuriwk->pattbl = mpkemuri;
    kemuriwk->sx_posi.w.h = 240;
    kemuriwk->sy_posi.w.h = 340;
    patinit(kemuriwk, 0);
    special_player_get_work(kemuriwk)->timer = 6;
    ++kemuriwk->exeno;
    cal_random.l = random();
    kemuriwk->sx_posi.w.h += cal_random.w.l & 31;
    kemuriwk->sy_posi.w.h += cal_random.w.l & 7;
    if (swdata_pl.b.h & 4)
        kemuriwk->sx_speed.w.h = 3;
    else if (swdata_pl.b.h & 8)
        kemuriwk->sx_speed.w.h = -3;
    else
        kemuriwk->sx_speed.w.h = 0;
    kemuri1(kemuriwk);
}

void kemuri1(sprite_status_sp *kemuriwk) {
    if (--special_player_get_work(kemuriwk)->timer == 0)
        kemuriwk->actflg |= 1;
    kemuriwk->sx_posi.l += kemuriwk->sx_speed.l;
    --kemuriwk->sy_posi.l;
}

static sprite_pattern stn00 = {1, {{-10, -24, 0, 114}}};
static sprite_pattern stn01 = {1, {{-10, -24, 0, 115}}};
static sprite_pattern stn02 = {1, {{-10, -24, 8, 115}}};
static sprite_pattern stn03 = {1, {{-10, -24, 0, 116}}};
static sprite_pattern stn04 = {1, {{-12, -24, 0, 117}}};
static sprite_pattern stn05 = {1, {{-12, -24, 16, 117}}};
static sprite_pattern stn06 = {1, {{-12, -24, 24, 117}}};
static sprite_pattern stn07 = {1, {{-12, -24, 8, 117}}};
static sprite_pattern stn08 = {1, {{-8, -16, 0, 118}}};
static sprite_pattern stn09 = {1, {{-8, -16, 16, 118}}};
static sprite_pattern stn0a = {1, {{-8, -16, 24, 118}}};
static sprite_pattern stn0b = {1, {{-8, -16, 8, 118}}};
static sprite_patterns_sp stone00 = {4, 3, {&stn00, &stn01, &stn03, &stn02}};
static sprite_patterns_sp stone01 = {4, 1, {&stn04, &stn05, &stn06, &stn07}};
static sprite_patterns_sp stone02 = {4, 1, {&stn0b, &stn0a, &stn09, &stn08}};
static sprite_patterns_sp *mpstone[3] = {&stone00, &stone01, &stone02};

void time_stone(sprite_status_sp *stonewk) {
    void (*acttbl[4])(sprite_status_sp *) = {&t_stone0, &t_stone1, &t_stone2,
                                             &t_stone3};

    acttbl[stonewk->exeno](stonewk);
    patset(stonewk);
}

void t_stone0(sprite_status_sp *stonewk) {

    stonewk->pattbl = mpstone;
    stonewk->sx_posi.w.h = 257;
    stonewk->sy_posi.w.h = 112;
    patinit(stonewk, 0);
    special_player_get_work(stonewk)->timer = 30;
    ++stonewk->exeno;
    t_stone1(stonewk);
}

void t_stone1(sprite_status_sp *stonewk) {
    if (--special_player_get_work(stonewk)->timer == 0)
        ++stonewk->exeno;
}

void t_stone2(sprite_status_sp *stonewk) {
    stonewk->sy_posi.w.h += 4;
    if (stonewk->sy_posi.w.h < 336)
        return;
    ++stonewk->exeno;
    actwk[6].actflg |= 1;
    actwk[7].actflg |= 1;
    special_player_get_work(stonewk)->timer = 60;
    actwk[0].exeno = 18;
    key_set(217);
}

void t_stone3(sprite_status_sp *stonewk) {
    if (--special_player_get_work(stonewk)->timer == 0)
        gmclrflg = 1;
}

void tskira0(sprite_status_sp *kirawk) {
    void (*acttbl[2])(sprite_status_sp *) = {&tskira00, &tskira01};

    acttbl[kirawk->exeno](kirawk);
    patset(kirawk);
}

void tskira00(sprite_status_sp *kirawk) {

    kirawk->pattbl = mpstone;
    patinit(kirawk, 1);
    ++kirawk->exeno;
    tskira01(kirawk);
}

void tskira01(sprite_status_sp *kirawk) {
    kirawk->sx_posi.w.h = actwk[5].sx_posi.w.h;
    kirawk->sy_posi.w.h = actwk[5].sy_posi.w.h - 16;
}

void tskira1(sprite_status_sp *kirawk) {
    void (*acttbl[2])(sprite_status_sp *) = {&tskira10, &tskira11};

    acttbl[kirawk->exeno](kirawk);
    patset(kirawk);
}

void tskira10(sprite_status_sp *kirawk) {

    kirawk->pattbl = mpstone;
    patinit(kirawk, 2);
    ++kirawk->exeno;
    tskira11(kirawk);
}

void tskira11(sprite_status_sp *kirawk) {
    kirawk->sx_posi.w.h = actwk[5].sx_posi.w.h;
    kirawk->sy_posi.w.h = actwk[5].sy_posi.w.h - 32;
}
