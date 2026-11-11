#include "../equ.h"
#include "hota8c.h"
#include "../action.h"
#include "../actset.h"
#include "../dircol.h"
#include "../playsub.h"
#include "tobira_work.h"

typedef struct {
    Uint8 phase_flags;
    Sint16 target_x;
    Sint16 target_y;
    Sint16 origin_y;
    Uint16 linked_actor_index;
    Sint16 origin_x;
    Uint8 timer;
    Uint8 sub_timer;
    Uint8 sequence_index;
    Uint16 door_actor_index;
    Uint8 beam_accumulator;
} hota8c_work;

static inline hota8c_work *hota8c_work_get(sprite_status *hotaru) {
    return (hota8c_work *)hotaru->actfree;
}

static void hotaru8_initx(sprite_status *hotaru, Uint8 pr_data);
static void hotaru8_init(sprite_status *hotaru);
static void hotaru8_dspin(sprite_status *hotaru);
static void hotaru8_setup(sprite_status *hotaru);
static void hotaru8_idou(sprite_status *hotaru);
static void hota8_zanzou(sprite_status *hotaru);
static void hotaru8_move2(sprite_status *hotaru);
static void hotaru8_move3(sprite_status *hotaru);
static void hotaru8_beemset(sprite_status *hotaru);
static void hotaru8_move4(sprite_status *hotaru);
static void hotaru8_beem(sprite_status *beemwk);
static void h8bem_init(sprite_status *beemwk);
static void h8bem_move1(sprite_status *beemwk);
static void h8bem_move2(sprite_status *beemwk);
static void hotaru8_zanact(sprite_status *shadowwk);
static void hotaru8_main(sprite_status *roomwk);
static void ht8main_init(sprite_status *roomwk);
static void ht8main_move1(sprite_status *roomwk);
static void ht8main_move2(sprite_status *roomwk);
static void ht8main_move3(sprite_status *roomwk);
static void ht8main_move4(sprite_status *roomwk);
static void ht8main_move5(sprite_status *roomwk);
static void ht8main_move6(sprite_status *roomwk);
static void colchg2(Uint8 tbl_number);
static void clrtrn(PALETTEENTRY *color_tbl);
static void colchg(sprite_status *roomwk, Uint8 *chgtbl);

extern Uint8 clrchgtbl1[16];
extern Uint8 clrchgtbl2[16];
extern PALETTEENTRY clrtblC1[48];
extern PALETTEENTRY clrtblD[108];

static Uint8 pchg0[6] = {1, 5, 0, 5, 0, 255};
static Uint8 pchg1[5] = {0, 4, 10, 10, 255};
static Uint8 pchg2[4] = {7, 11, 12, 255};
static Uint8 pchg3[4] = {3, 6, 15, 255};
static Uint8 pchg4[4] = {0, 4, 10, 255};
static Uint8 pchg5[4] = {7, 4, 10, 255};
static Uint8 pchg8[4] = {0, 6, 10, 255};
static Uint8 pchg9[6] = {0, 9, 10, 16, 10, 255};
static Uint8 *hotaru8_pchg[10] = {pchg0, pchg1, pchg2, pchg3, pchg4,
                                  pchg5, pchg8, pchg8, pchg8, pchg9};
static sprite_pattern hotaru00 = {2, {{-12, -26, 0, 293}, {-12, -10, 0, 295}}};
static sprite_pattern hotaru01 = {2, {{-12, -26, 0, 294}, {-12, -10, 0, 295}}};
static sprite_pattern hotaru04 = {1, {{-12, -12, 0, 296}}};
static sprite_pattern hotaru05 = {1, {{-12, -12, 0, 297}}};
static sprite_pattern hotaru06 = {2, {{-4, -16, 0, 298}, {-4, 0, 0, 298}}};
static sprite_pattern hotaru07 = {2, {{-4, -16, 0, 299}, {-4, 0, 0, 299}}};
static sprite_pattern hotaru08 = {2, {{-4, -16, 0, 300}, {-4, 0, 0, 300}}};
static sprite_pattern hotaru09 = {
    3, {{-12, 0, 0, 301}, {-4, -16, 0, 298}, {-4, 0, 0, 298}}};
static sprite_pattern hotaru0b = {
    3, {{-12, -12, 0, 297}, {-12, -26, 0, 293}, {-12, -10, 0, 295}}};
static sprite_pattern hotaru0c = {
    3, {{-12, -12, 0, 297}, {-12, -26, 0, 294}, {-12, -10, 0, 295}}};
static sprite_pattern hotaru0f = {1, {{-12, -8, 0, 301}}};
static sprite_pattern hotaru10 = {1, {{-12, -8, 8, 301}}};
static sprite_pattern hotaru = {1, {{-16, -20, 0, 0}}};
sprite_pattern *hotaru8pat[17] = {
    &hotaru00, &hotaru01, &hotaru,   &hotaru,   &hotaru04, &hotaru05,
    &hotaru06, &hotaru07, &hotaru08, &hotaru09, &hotaru,   &hotaru0b,
    &hotaru0c, &hotaru,   &hotaru,   &hotaru0f, &hotaru10};

void hotaru8(sprite_status *hotaru) {
    void (*tbl[7])(sprite_status *) = {
        &hotaru8_init,  &hotaru8_dspin, &hotaru8_setup, &hotaru8_idou,
        &hotaru8_move2, &hotaru8_move3, &hotaru8_move4};
    if (hotaru->userflag.b.h == -3) {
        hotaru8_zanact(hotaru);
        return;
    }
    if (hotaru->userflag.b.h == -2) {
        hotaru8_main(hotaru);
        return;
    }
    if (hotaru->userflag.b.h == -1) {
        hotaru8_beem(hotaru);
        return;
    }

    hotaru->userflag.b.h = (Uint8)hotaru->userflag.b.h & 127;
    tbl[hotaru->r_no0 / 2](hotaru);
    actionsub(hotaru);
}

static void hotaru8_initx(sprite_status *hotaru, Uint8 pr_data) {
    hota8c_work *work = hota8c_work_get(hotaru);

    hotaru->actflg |= 4;
    hotaru->sprpri = pr_data;
    hotaru->patbase = hotaru8pat;
    work->origin_x = hotaru->xposi.w.h;
    work->origin_y = hotaru->yposi.w.h;
}

static void hotaru8_init(sprite_status *hotaru) {
    hota8c_work *work = hota8c_work_get(hotaru);

    hotaru8_initx(hotaru, 1);
    hotaru->sprvsize = hotaru->sprhsize = 8;
    hotaru->r_no0 += 2;
    work->timer = 60;
}

static void hotaru8_dspin(sprite_status *hotaru) {
    hota8c_work *work = hota8c_work_get(hotaru);

    hotaru->mstno.b.h = 4;
    patchg(hotaru, hotaru8_pchg);
    if (--work->timer != 0)
        return;
    hotaru->mstno.b.h = 1;
    work->phase_flags = hotaru->userflag.b.h;
    hotaru->r_no0 += 2;
}

static void hotaru8_setup(sprite_status *hotaru) {
    Sint16 next_position, speed_calc;
    hota8c_work *work = hota8c_work_get(hotaru);

    if (!(work->phase_flags & 1))
        goto label1;

    next_position = actwk[0].xposi.w.h & 32760;
    if (next_position < 3008)
        goto label1;
    if (next_position >= 3232)
        goto label1;
    work->target_x = next_position;
    work->target_y = 344;
    goto label2;

label1:
    next_position = (gametimer.w & 127) + 3056;
    work->target_x = next_position;
    next_position = (actwk[0].xposi.w.h * 5 & 31) + 352;
    work->target_y = next_position;

label2:
    hotaru->yspeed.w = 0;
    speed_calc = work->target_y - hotaru->yposi.w.h;
    if (speed_calc != 0)
        hotaru->yspeed.w = speed_calc * 8;

    hotaru->xspeed.w = 0;
    speed_calc = work->target_x - hotaru->xposi.w.h;
    if (speed_calc != 0)
        hotaru->xspeed.w = speed_calc * 8;

    ++work->phase_flags;
    work->sub_timer = 32;
    hotaru->r_no0 += 2;
    hotaru->colino = 0;
}

static void hotaru8_idou(sprite_status *hotaru) {
    hota8c_work *work = hota8c_work_get(hotaru);

    hotaru->xposi.l += hotaru->xspeed.w << 8;
    hotaru->yposi.l += hotaru->yspeed.w << 8;

    hota8_zanzou(hotaru);
    if (--work->sub_timer == 0)
        hotaru->r_no0 += 2;
    patchg(hotaru, hotaru8_pchg);
}

static void hota8_zanzou(sprite_status *hotaru) {
    sprite_status *shadowwk;
    hota8c_work *work = hota8c_work_get(hotaru);

    if ((work->sub_timer & 3) != 3)
        return;
    if (actwkchk(&shadowwk) != 0)
        return;
    shadowwk->actno = 57;
    shadowwk->xposi.w.h = hotaru->xposi.w.h;
    shadowwk->yposi.w.h = hotaru->yposi.w.h;
    shadowwk->userflag.b.h = -3;
}

static void hotaru8_move2(sprite_status *hotaru) {
    char color_change_number;
    char tbl[32] = {0,  0,  0,  12, 12, 12, 36, 36, 36, 0, 0,
                    0,  12, 12, 12, 36, 36, 36, 0,  0,  0, 12,
                    12, 12, 36, 36, 36, 48, 60, 72, 84, -1};
    hota8c_work *work = hota8c_work_get(hotaru);

    hotaru->mstno.b.h = 0;
    patchg(hotaru, hotaru8_pchg);

    if ((color_change_number = tbl[work->sequence_index++]) != -1)
        colchg2(color_change_number);
    else {
        hotaru->r_no0 += 2;
        work->sequence_index = 0;
    }
}

static void hotaru8_move3(sprite_status *hotaru) {
    hota8c_work *work = hota8c_work_get(hotaru);

    if (work->sequence_index == 0) {
        work->sequence_index = 20;
        hotaru->mstno.b.h = 2;
        hotaru->colino = 11;
    }

    if ((work->beam_accumulator += 64) < 64)
        hotaru8_beemset(hotaru);

    patchg(hotaru, hotaru8_pchg);
    if (--work->sequence_index == 0)
        hotaru->r_no0 += 2;
}

static void hotaru8_beemset(sprite_status *hotaru) {
    sprite_status *beemwk;

    if (actwkchk(&beemwk) != 0)
        return;
    beemwk->actno = 57;
    beemwk->userflag.b.h = -1;
    beemwk->xposi.w.h = hotaru->xposi.w.h;
    beemwk->yposi.w.h = hotaru->yposi.w.h;
}

static void hotaru8_move4(sprite_status *hotaru) {
    hota8c_work *work = hota8c_work_get(hotaru);

    if (work->sequence_index == 0) {
        work->sequence_index = 16;
        hotaru->mstno.b.h = 0;
    }

    patchg(hotaru, hotaru8_pchg);

    if (--work->sequence_index != 0)
        return;

    hotaru->r_no0 = 4;
    hotaru->mstno.b.h = 1;
    colchg2(0);
}

static void hotaru8_beem(sprite_status *beemwk) {
    void (*tbl[3])(sprite_status *) = {&h8bem_init, &h8bem_move1, &h8bem_move2};
    tbl[beemwk->r_no0 / 2](beemwk);
    actionsub(beemwk);
}

static void h8bem_init(sprite_status *beemwk) {
    hotaru8_initx(beemwk, 3);
    beemwk->r_no0 += 2;
    beemwk->sprvsize = 16;
    beemwk->sprhsize = 4;
    beemwk->colino = 152;
}

static void h8bem_move1(sprite_status *beemwk) {
    hota8c_work *work = hota8c_work_get(beemwk);

    beemwk->mstno.b.h = 8;
    beemwk->yposi.w.h += 8;
    patchg(beemwk, hotaru8_pchg);
    if (emycol_d(beemwk) < 0) {
        beemwk->r_no0 += 2;
        work->timer = work->sub_timer = 0;
    }
}

static void h8bem_move2(sprite_status *beemwk) {
    hota8c_work *work = hota8c_work_get(beemwk);

    if (work->timer == 0) {
        work->timer = 2;
        beemwk->mstno.b.h = 9;
        if (work->sub_timer != 0)
            return;
    }
    patchg(beemwk, hotaru8_pchg);
    if (--work->timer != 0)
        return;
    if (++work->sub_timer >= 2)
        frameout(beemwk);
}

static void hotaru8_zanact(sprite_status *shadowwk) {
    hota8c_work *work = hota8c_work_get(shadowwk);

    if (shadowwk->r_no0 == 0) {
        hotaru8_initx(shadowwk, 4);
        work->timer = 24;
        shadowwk->mstno.b.h = 4;
        shadowwk->r_no0 += 2;
    }
    patchg(shadowwk, hotaru8_pchg);
    if (--work->timer)
        actionsub(shadowwk);
    else
        frameout(shadowwk);
}

static void hotaru8_main(sprite_status *roomwk) {
    void (*tbl[7])(sprite_status *) = {
        &ht8main_init,  &ht8main_move1, &ht8main_move2, &ht8main_move3,
        &ht8main_move4, &ht8main_move5, &ht8main_move6};

    tbl[roomwk->r_no0 / 2](roomwk);
}

static void ht8main_init(sprite_status *roomwk) {
    sprite_status *doorwk;
    hota8c_work *work = hota8c_work_get(roomwk);

    hotaru8_initx(roomwk, 1);

    if (actwk[0].xposi.w.h < 3024)
        return;

    shut_flag = 1;
    if (actwkchk(&doorwk) != 0)
        return;
    doorwk->actno = 41;
    doorwk->yposi.w.h = 464;
    doorwk->xposi.w.h = 3240;
    work->door_actor_index = doorwk - actwk;
    roomwk->r_no0 += 2;
}

static void ht8main_move1(sprite_status *roomwk) {
    hota8c_work *work = hota8c_work_get(roomwk);

    if (actwk[0].xposi.w.h < 3024)
        return;
    if (scra_vline >= 204)
        bossflag = 8;
    scra_vline += 6;
    if (bossflag == 8) {
        roomwk->r_no0 += 2;
        work->timer = work->sub_timer = 0;
    }
}

static void ht8main_move2(sprite_status *roomwk) {
    sprite_status *hotaru;
    hota8c_work *work = hota8c_work_get(roomwk);

    colchg(roomwk, clrchgtbl1);
    if (work->timer != 16)
        return;
    work->timer = work->sub_timer = 0;
    if (actwkchk(&hotaru) != 0)
        return;
    hotaru->actno = 57;
    hotaru->yposi.w.h = 400;
    hotaru->xposi.w.h = 3040;
    work->linked_actor_index = hotaru - actwk;
    work->timer = 60;
    roomwk->r_no0 += 2;
}

static void ht8main_move3(sprite_status *roomwk) {
    sprite_status *hotaru;
    hota8c_work *work = hota8c_work_get(roomwk);

    if (actwk[work->linked_actor_index].actno == 57)
        return;

    colchg2(0);
    if (--work->timer)
        return;

    if (actwkchk(&hotaru) != 0)
        return;
    hotaru->actno = 57;
    hotaru->yposi.w.h = 392;
    hotaru->xposi.w.h = 3040;
    work->linked_actor_index = hotaru - actwk;
    hotaru->userflag.b.h = 1;
    roomwk->r_no0 += 2;
    ht8main_move4(roomwk);
}

static void ht8main_move4(sprite_status *roomwk) {
    sprite_status *hotaru;
    hota8c_work *work = hota8c_work_get(roomwk);

    if (actwkchk(&hotaru) != 0)
        return;
    hotaru->actno = 57;
    hotaru->yposi.w.h = 392;
    hotaru->xposi.w.h = 3200;
    roomwk->xposi.w.h = (Sint16)(hotaru - actwk);
    hotaru->userflag.b.h = 0;
    work->timer = work->sub_timer = 0;
    roomwk->r_no0 += 2;
}

static void ht8main_move5(sprite_status *roomwk) {
    hota8c_work *work = hota8c_work_get(roomwk);

    if (actwk[work->linked_actor_index].actno == 57)
        return;

    if (actwk[(Uint16)roomwk->xposi.w.h].actno == 57)
        return;

    roomwk->r_no0 += 2;
}

static void ht8main_move6(sprite_status *roomwk) {
    sprite_status *eggwk;
    hota8c_work *work = hota8c_work_get(roomwk);

    colchg(roomwk, clrchgtbl2);
    if (work->timer != 16)
        return;
    /* Unlock the door: it consumes trigger_open from its own work area. */
    tobira8_get_work(&actwk[work->door_actor_index])->trigger_open = 255;
    if (actwkchk(&eggwk) != 0)
        return;
    eggwk->actno = 63;
    eggwk->xposi.w.h = 3840;
    eggwk->yposi.w.h = 316;
    frameout(roomwk);
}

static void colchg2(Uint8 tbl_number) { clrtrn(&clrtblC1[tbl_number / 2]); }

static void clrtrn(PALETTEENTRY *color_tbl) {
    PALETTEENTRY *colorwk;

    colorwk = &lpcolorwk[49];
    *colorwk++ = *color_tbl++;
    *colorwk++ = *color_tbl++;
    colorwk = &lpcolorwk[60];
    *colorwk++ = *color_tbl++;
    *colorwk++ = *color_tbl++;
    *colorwk++ = *color_tbl++;
    *colorwk++ = *color_tbl++;
}

static void colchg(sprite_status *roomwk, Uint8 *chgtbl) {
    Uint8 clrtbl_index;
    hota8c_work *work = hota8c_work_get(roomwk);

    if (work->timer == 16)
        return;

    if (work->sub_timer != 0) {
        --work->sub_timer;
        return;
    }
    clrtbl_index = chgtbl[work->timer];
    work->sub_timer = chgtbl[work->timer + 1];
    clrtrn(&clrtblC1[clrtbl_index / 2]);
    work->timer += 2;
}

Uint8 clrchgtbl1[16] = {84, 64, 72, 4, 60, 5, 48, 6, 36, 7, 24, 8, 12, 9, 0, 0};
Uint8 clrchgtbl2[16] = {0,  64, 12, 9, 24, 8, 36, 7,
                        48, 6,  60, 5, 72, 4, 84, 32};
PALETTEENTRY clrtblC1[48] = {
    {0, 0, 0, 1},     {0, 0, 0, 1},     {0, 0, 0, 1},     {0, 0, 0, 1},
    {0, 0, 0, 1},     {0, 0, 0, 1},     {0, 0, 0, 1},     {0, 0, 32, 1},
    {0, 0, 0, 1},     {0, 0, 0, 1},     {0, 0, 32, 1},    {0, 0, 0, 1},
    {0, 0, 0, 1},     {0, 0, 64, 1},    {0, 0, 0, 1},     {0, 0, 32, 1},
    {0, 0, 64, 1},    {0, 0, 0, 1},     {0, 0, 0, 1},     {0, 0, 96, 1},
    {0, 0, 0, 1},     {0, 0, 64, 1},    {0, 0, 96, 1},    {0, 0, 0, 1},
    {0, 0, 0, 1},     {32, 32, 128, 1}, {0, 0, 32, 1},    {0, 0, 96, 1},
    {0, 0, 128, 1},   {0, 0, 0, 1},     {0, 0, 0, 1},     {64, 64, 160, 1},
    {0, 0, 64, 1},    {0, 0, 128, 1},   {32, 0, 160, 1},  {0, 0, 32, 1},
    {0, 0, 0, 1},     {96, 96, 192, 1}, {0, 0, 96, 1},    {32, 0, 160, 1},
    {64, 32, 192, 1}, {0, 0, 64, 1},    {0, 0, 32, 1},    {128, 128, 224, 1},
    {0, 32, 128, 1},  {64, 32, 192, 1}, {96, 64, 224, 1}, {0, 0, 96, 1}};
PALETTEENTRY clrtblD[108];
