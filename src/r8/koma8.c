#include "../equ.h"
#include "koma8.h"
#include "../action.h"
#include "../actset.h"
#include "../dircol.h"
#include "../playsub.h"
#include "../spring_work.h"

#if defined(R82)
#define SPRITE_KOMA8_BASE 313
#elif defined(R83)
#define SPRITE_KOMA8_BASE 306
#else
#define SPRITE_KOMA8_BASE 304
#endif

typedef struct {
    Sint16 ground_y;
    Sint16 origin_x;
    Sint16 travel_range;
} koma8_work;

static koma8_work *koma8_get_work(sprite_status *komawk) {
    return (koma8_work *)komawk->actfree;
}

static void koma_init(sprite_status *komawk);
static void koma_move(sprite_status *komawk);
static void koma_move2(sprite_status *komawk);

static Uint8 komachg0[4] = {8, 0, 1, 255};
static Uint8 *komachg[1] = {komachg0};
static sprite_pattern kom00 = {1, {{-8, -8, 0, SPRITE_KOMA8_BASE}}};
static sprite_pattern kom01 = {1, {{-8, -8, 0, SPRITE_KOMA8_BASE + 1}}};
sprite_pattern *koma8pat[2] = {&kom00, &kom01};

void koma8(sprite_status *komawk) {
    koma8_work *work = koma8_get_work(komawk);
    void (*tbl[3])(sprite_status *) = {&koma_init, &koma_move, &koma_move2};

    tbl[komawk->r_no0 / 2](komawk);
    frameout_s00(komawk, work->origin_x);
}

static void koma_init(sprite_status *komawk) {
    koma8_work *work = koma8_get_work(komawk);
    sprite_status *sprwk;

    komawk->r_no0 += 2;
    komawk->actflg |= 4;
    komawk->sprpri = 4;
    komawk->patbase = koma8pat;
    komawk->sprhsize = 8;
    komawk->sprvsize = 6;
    work->origin_x = komawk->xposi.w.h;
    komawk->xspeed.w = 256;

    if (actwkchk2(komawk, &sprwk) != 0) {
        frameout(komawk);
        return;
    };
    sprwk->actno = 10;
    sprwk->xposi.w.h = komawk->xposi.w.h;
    sprwk->yposi.w.h = komawk->yposi.w.h - 16;
    /* The child is a spring (actno 10), so its work area is a spring_work. */
    spring_work_get(sprwk)->follow_y_offset = (Sint8)240;
    spring_work_get(sprwk)->parent_index = komawk - actwk;
    sprwk->userflag.b.h = komawk->userflag.b.h & 15;

    work->travel_range = 80;
    if (komawk->userflag.b.h < 0)
        work->travel_range = 64;
    koma_move(komawk);
}

static void koma_move(sprite_status *komawk) {
    koma8_work *work = koma8_get_work(komawk);
    Sint16 colli_data;

    if ((colli_data = emycol_d(komawk)) < 0) {
        komawk->yposi.w.h += colli_data;
        work->ground_y = komawk->yposi.w.h;
        komawk->r_no0 += 2;
    } else {
        ++komawk->yposi.w.h;
    }
}

static void koma_move2(sprite_status *komawk) {
    koma8_work *work = koma8_get_work(komawk);
    Sint16 colli_data, pos_data;

    if (komawk->xspeed.w >= 0)
        colli_data = emycol_r(komawk, 32);
    else
        colli_data = emycol_l(komawk, 232);
    if (colli_data < 7)
        goto label1;

    if ((pos_data = komawk->xposi.w.h - work->origin_x) < 0)
        pos_data = -pos_data;
    if (pos_data >= work->travel_range)
        goto label1;

    if ((colli_data = emycol_d(komawk)) < -7)
        goto label1;
    if (colli_data > 7)
        goto label1;
    komawk->yposi.w.h += colli_data;
    goto label2;
label1:
    komawk->xspeed.w = -komawk->xspeed.w;
label2:
    speedset2(komawk);
    patchg(komawk, komachg);
    actionsub(komawk);
}
