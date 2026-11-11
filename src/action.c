#include "equ.h"
#include "action.h"
#include "impfuncs.h"
#include "player_work.h"

extern void (*act_tbl[1])(sprite_status *);
extern bmp_info SprBmp[700];

/* Number of entries in the sprite dimension table. A pattern index at or
   beyond this is stale work-area data (the modules reuse actfree), and must
   not be used to index SprBmp. */
#define SPRITE_BANK_COUNT (sizeof(SprBmp) / sizeof(SprBmp[0]))

void action(void) {
    sprite_status *pActwk;
    Sint32 i;

    pActwk = &actwk[0];
    for (i = 0; i < 128; ++i, ++pActwk) {
        if (pActwk->actno) {

            act_tbl[pActwk->actno - 1](pActwk);
        }
    }
}

void speedset(sprite_status *pActwk) {
    player_work *work = player_work_get(pActwk);
    int_union xpos, ypos;
    short_union spd;

    xpos.l = pActwk->xposi.l;
    ypos.l = pActwk->yposi.l;
    spd.w = pActwk->xspeed.w;
    xpos.l += spd.w << 8;
    spd.w = pActwk->yspeed.w;
    if (work->status_flags & 8)
        goto label2;
    if (spd.w >= 0)
        goto label1;
    if (!(work->status_flags & 2))
        goto label1;
    if (pActwk->yspeed.w < -2048)
        goto label2;
label1:
    if (work->status_flags & 4)
        goto label2;
    pActwk->yspeed.w += 56;
label2:
    if (pActwk->yspeed.w < 0)
        goto label3;
    if (pActwk->yspeed.w < 4096)
        goto label3;
    pActwk->yspeed.w = 4096;
label3:
    ypos.l += spd.w << 8;
    pActwk->xposi.l = xpos.l;
    pActwk->yposi.l = ypos.l;
}

void speedset2(sprite_status *pActwk) {
    player_work *work = player_work_get(pActwk);
    int_union xpos, ypos;
    Sint32 spd;
    Sint32 actwkno;
    Sint16 d1;

    xpos.l = pActwk->xposi.l;
    ypos.l = pActwk->yposi.l;
    spd = pActwk->xspeed.w;
    if (pActwk->cddat & 8) {

        d1 = 0;
        actwkno = work->ride_actor_index;
        if (actwk[actwkno].actno == 30) {

            d1 = -256;

            if (actwk[actwkno].cddat & 1)
                d1 = -d1;

            spd += d1;
        }
    }
    spd = spd << 8;
    xpos.l += spd;
    spd = pActwk->yspeed.w;
    spd = spd << 8;
    ypos.l += spd;
    pActwk->xposi.l = xpos.l;
    pActwk->yposi.l = ypos.l;
}

void actionsub(sprite_status *pActwk) {
    Sint32 i;
    Uint16 flag;
    Sint16 hsiz, vsiz;
    Sint16 xpos, ypos;
    Uint8 pri;

    if (lpKeepWork->GamePass != 0)
        return;

    pActwk->actflg &= 127;
    flag = pActwk->actflg;
    flag &= 12;
    if (flag) {

        hsiz = (Uint16)pActwk->sprhsize;
        xpos = pActwk->xposi.w.h;
        xpos -= scra_h_posit.w.h;
        if (xpos + hsiz < 0)
            return;
        if (xpos - hsiz >= 320)
            return;

        vsiz = (Uint16)pActwk->sprvsize;
        ypos = pActwk->yposi.w.h;
        if ((Uint16)scra_v_posit.w.h < 256) {

            if ((Uint16)ypos >= 2048)
                ypos -= 2048;
        } else if ((Uint16)scra_v_posit.w.h >= 1792) {

            if ((Uint16)ypos < 256)
                ypos += 2048;
        }
        ypos -= scra_v_posit.w.h;
        if (ypos + vsiz < 0)
            return;
        if (ypos - vsiz >= 224)
            return;
    }

    pri = pActwk->sprpri;
    i = pbuffer[pri].cnt;
    if (i < 63) {

        pbuffer[pri].pActwk[i] = pActwk;
        pbuffer[pri].cnt += 1;
    }
}

void frameout(sprite_status *pActwk) { sMemSet(pActwk, 0, sizeof(*pActwk)); }

void patset(void) {
    sprite_status *pActwk;
    Uint8 flag;
    Sint16 i;
    Sint16 act;
    Sint16 xposi, yposi;
    Sint16 patno;
    Sint16 cnt;
    sprite_pattern *patadr;
    sprite_pattern **patbase;
    sprite_data *sprdat;
    int_union *pScrHposi, *pScrVposi;
    int_union *patsettbl[8] = {0, &scra_h_posit, &scrb_h_posit, &scrc_h_posit,
                               0, &scra_v_posit, &scrb_v_posit, &scrc_v_posit};

    if (lpKeepWork->GamePass != 0)
        return;

    linkdata = 0;
    for (i = 0; i < 8; ++i) {

        if (pbuffer[i].cnt == 0)
            continue;
        act = 0;
        do {

            pActwk = pbuffer[i].pActwk[act];
            if (pActwk->actno && pActwk->patbase != 0) {
                flag = pActwk->actflg >> 2;
                flag &= 3;
                if (flag) {

                    pScrHposi = patsettbl[flag];
                    pScrVposi = patsettbl[flag + 4];
                    xposi = pActwk->xposi.w.h;
                    xposi -= pScrHposi->w.h;
                    xposi += 128;
                    yposi = pActwk->yposi.w.h;
                    if ((Uint16)pScrVposi->w.h < 256) {

                        if ((Uint16)yposi >= 2048)
                            yposi -= 2048;
                    } else if ((Uint16)pScrVposi->w.h >= 1792) {

                        if ((Uint16)yposi < 256)
                            yposi += 2048;
                    }
                    yposi -= pScrVposi->w.h;
                    yposi += 128;
                } else {

                    xposi = pActwk->xposi.w.h;
                    yposi = pActwk->yposi.w.h;
                }

                patbase = pActwk->patbase;
                patno = pActwk->patno;
                patadr = patbase[patno];
                cnt = patadr->cnt;
                sprdat = &patadr->spra[0];
                if (pActwk->actflg & 32) {
                    cnt = 1;
                }
                if (cnt > 0) {
                    spatset(xposi, yposi, pActwk, sprdat, cnt);
                }
                pActwk->actflg |= 128;
            }
            ++act;
        } while (--pbuffer[i].cnt > 0);
    }

    for (i = (Uint16)linkdata; i < 80; ++i) {
        EAsprset(0, 0, 0, i, 0);
    }
}

void spatset(Sint16 xposi, Sint16 yposi, sprite_status *pActwk,
             sprite_data *sprdat, Sint16 cnt) {
    Uint8 flag;
    Sint16 x, y;
    Uint16 reverse;

    flag = pActwk->actflg;
    flag &= 3;
    for (; cnt > 0; --cnt) {
        if (sprdat->index && sprdat->index < SPRITE_BANK_COUNT) {
            if (linkdata >= 80)
                return;

            switch (flag) {
            case 0:
                x = xposi + (Sint16)sprdat->xoff;
                y = yposi + (Sint16)sprdat->yoff;
                break;
            case 1:
                x = xposi +
                    (Sint16)(-sprdat->xoff - (Sint16)SprBmp[sprdat->index].xs);
                y = yposi + (Sint16)sprdat->yoff;
                break;
            case 2:
                x = xposi + (Sint16)sprdat->xoff;
                y = yposi +
                    (Sint16)(-sprdat->yoff - (Sint16)SprBmp[sprdat->index].ys);
                break;
            case 3:
                x = xposi +
                    (Sint16)(-sprdat->xoff - (Sint16)SprBmp[sprdat->index].xs);
                y = yposi +
                    (Sint16)(-sprdat->yoff - (Sint16)SprBmp[sprdat->index].ys);
                break;
            }
            reverse = 0;
            if (pActwk->sproffset & 32768 || (sprdat->etc & 128) != 0) {
                reverse |= 32768;
            }
            if (((flag & 1) != 0) ^ ((sprdat->etc & 8) != 0)) {
                ++reverse;
            }
            if (((flag & 2) != 0) ^ ((sprdat->etc & 16) != 0)) {
                reverse += 2;
            }

            EAsprset(x, y, sprdat->index, linkdata, reverse);
            ++linkdata;
        }
        ++sprdat;
    }
}

Sint32 scronchk(sprite_status *pActwk) {
    Sint16 xposi, yposi;

    xposi = pActwk->xposi.w.h;
    xposi -= scra_h_posit.w.h;
    if (xposi < 0 || xposi >= 320)
        return -1;
    yposi = pActwk->yposi.w.h;
    yposi -= scra_v_posit.w.h;
    if (yposi < 0 || yposi >= 224)
        return -1;
    return 0;
}

Sint32 scronchk2(sprite_status *pActwk) {
    Sint16 xposi, yposi;
    Sint16 hsize;

    hsize = pActwk->sprhsize;
    xposi = pActwk->xposi.w.h;
    xposi -= scra_h_posit.w.h;
    xposi += hsize;
    if (xposi < 0)
        return -1;
    hsize += hsize;
    xposi -= hsize;
    if (xposi >= 320)
        return -1;
    yposi = pActwk->yposi.w.h;
    yposi -= scra_v_posit.w.h;
    if (yposi < 0 || yposi >= 224)
        return -1;
    return 0;
}
