#include "../equ.h"
#include "tobira.h"
#include "tobira_work.h"
#include "../action.h"
#include "../actset.h"
#include "../playsub.h"
#include "../ridechk.h"

#if defined(R82)
#define SPRITE_TOBIRA_BASE 432
#elif defined(R83)
#define SPRITE_TOBIRA_BASE 500
#else
#define SPRITE_TOBIRA_BASE 464
#endif

static char pchg00[4] = {2, 0, 1, -1};
static char *pchg[1] = {pchg00};
static sprite_pattern pat00 = {1, {{-8, -16, 0, SPRITE_TOBIRA_BASE}}};
static sprite_pattern pat01 = {1, {{-8, -16, 0, SPRITE_TOBIRA_BASE + 1}}};
static sprite_pattern pat02 = {1, {{-8, -16, 0, SPRITE_TOBIRA_BASE + 2}}};
static sprite_pattern pat03 = {1, {{-16, -16, 0, SPRITE_TOBIRA_BASE + 3}}};
static sprite_pattern pat04 = {1, {{-16, -16, 0, SPRITE_TOBIRA_BASE + 4}}};
sprite_pattern *pat_tobira0[2] = {&pat00, &pat00};
sprite_pattern *pat_tobira1[2] = {&pat01, &pat02};
sprite_pattern *pat_tobira2[2] = {&pat03, &pat04};

void tobira(sprite_status *actionwk) {
    tobira8_work *work = tobira8_get_work(actionwk);
    Sint16 a;
    a = work->parent_index;
    if (a != 0) {
        slave(actionwk, work->parent_index);
        return;
    }
    master(actionwk);
}

void master(sprite_status *actionwk) {
    switch (actionwk->r_no0) {
    case 0:
        m_init(actionwk);
        break;
    case 2:
        m_closed(actionwk);
        break;
    case 4:
        m_open(actionwk);
        break;
    case 6:
        m_opend(actionwk);
        break;
    case 8:
        m_close(actionwk);
        break;
    }
}

void m_init(sprite_status *actionwk) {
    tobira8_work *work = tobira8_get_work(actionwk);
    Sint16 d0, d1;
    sprite_status *a1;

    actionwk->r_no0 += 2;
    work->base_y = actionwk->yposi.w.h - 16;

    if (actionwk->userflag.b.h == 0)
        actionwk->patbase = pat_tobira0;
    if (actionwk->userflag.b.h == 1)
        actionwk->patbase = pat_tobira1;
    if (actionwk->userflag.b.h == 2)
        actionwk->patbase = pat_tobira2;
    if (actwkchk2(actionwk, &a1) != 0) {
        frameout(actionwk);
        return;
    }
    a1->actno = actionwk->actno;
    a1->xposi.w.h = actionwk->xposi.w.h;
    a1->patbase = actionwk->patbase;
    tobira8_get_work(a1)->parent_index = actionwk - actwk;
    work->slave_index = a1 - actwk;
    d0 = d1 = 0;
    ini_com(actionwk, a1, &d0, &d1);
    a1->yposi.w.h -= d0;
    tobira8_get_work(a1)->closed_y -= d0;
    tobira8_get_work(a1)->open_y -= d1;
    a1 = actionwk;
    ini_com(actionwk, a1, &d0, &d1);
    a1->yposi.w.h += d0;
    work->closed_y += d0;
    work->open_y += d1;
}

void ini_com(sprite_status *actionwk, sprite_status *a1, Sint16 *d0,
             Sint16 *d1) {
    a1->actflg |= 4;
    a1->sprpri = 3;
    a1->sproffset = 1150;
    a1->sprhsize = 8;
    a1->sprvsize = 16;
    a1->yposi.w.h = tobira8_get_work(actionwk)->base_y;
    tobira8_get_work(a1)->closed_y = tobira8_get_work(actionwk)->base_y;
    tobira8_get_work(a1)->open_y = tobira8_get_work(actionwk)->base_y;
    *d0 = 16;
    *d1 = 49;
}

void m_closed(sprite_status *actionwk) {
    tobira8_work *work = tobira8_get_work(actionwk);
    Sint16 d0, d4, d5;

    ms_hitchk(actionwk);
    if (work->trigger_open != 0) {
        work->trigger_open = 0;
    } else {
        d0 = actionwk->userflag.b.h;
        if (d0 == 0)
            goto label1;
        d4 = 8;
        d5 = 64;
        if (d0 != 1) {
            d4 = -72;
            d5 = 64;
        }
        if (m_area(actionwk, d4, d5) == 0)
            goto label1;
    }

    actionwk->r_no0 += 2;
label1:
    ms_end(actionwk);
}

void ms_end(sprite_status *actionwk) {
    tobira8_work *work = tobira8_get_work(actionwk);
    Sint16 ano;

    patchg(actionwk, (Uint8 **)pchg);
    ano = work->slave_index;
    actwk[ano].patno = actionwk->patno;

    actionsub(&actwk[ano]);
    actionsub(actionwk);
    frameout_s(actionwk);
}

void m_open(sprite_status *actionwk) {
    tobira8_work *work = tobira8_get_work(actionwk);
    Sint16 ano;

    ms_hitchk(actionwk);

    ano = work->slave_index;
    actionwk->yposi.w.h += 2;
    actwk[ano].yposi.w.h -= 2;
    if (work->open_y <= actionwk->yposi.w.h) {
        actionwk->r_no0 += 2;
        actionwk->yposi.w.h = work->open_y;
        actwk[ano].yposi.w.h = tobira8_get_work(&actwk[ano])->open_y;
    }

    ms_end(actionwk);
}

void m_opend(sprite_status *actionwk) {
    Sint16 d0, d4, d5;

    ms_hitchk(actionwk);

    d0 = actionwk->userflag.b.h;
    if (d0 != 0) {
        d4 = -40;
        d5 = 112;
        if (d0 != 1) {
            d4 = -72;
            d5 = 112;
        }
        if (m_area(actionwk, d4, d5) == 0)
            actionwk->r_no0 += 2;
    }

    ms_end(actionwk);
}

void m_close(sprite_status *actionwk) {
    tobira8_work *work = tobira8_get_work(actionwk);
    Sint16 ano;

    ms_hitchk(actionwk);

    ano = work->slave_index;
    actionwk->yposi.w.h -= 2;
    actwk[ano].yposi.w.h += 2;

    if (work->closed_y >= actionwk->yposi.w.h) {
        actionwk->r_no0 = 2;
        actionwk->yposi.w.h = work->closed_y;
        actwk[ano].yposi.w.h = tobira8_get_work(&actwk[ano])->closed_y;
    }
    ms_end(actionwk);
}

Sint16 ms_hitchk(sprite_status *actionwk) {
    tobira8_work *work = tobira8_get_work(actionwk);
    Sint16 ano;

    ano = work->slave_index;
    hitchk(&actwk[ano], &actwk[0]);
    hitchk(actionwk, &actwk[0]);
}

Sint16 m_area(sprite_status *actionwk, Sint16 d4, Sint16 d5) {
    Sint16 d0;

    d0 = actwk[0].xposi.w.h - actionwk->xposi.w.h - d4;
    if ((Uint16)d5 <= (Uint16)d0)
        return 0;
    d0 = actwk[0].yposi.w.h - tobira8_get_work(actionwk)->base_y + 48;

    if ((Uint16)d0 >= 96)
        return 0;
    return 1;
}

void slave(sprite_status *actionwk, Sint16 ano) {
    if (actwk[ano].actno != 41) {
        frameout(actionwk);
    }
}
