#include "equ.h"
#include "actset.h"
#include "action.h"
#include "impfuncs.h"

#if !defined(R4) && !defined(R5) && !defined(R7) && !defined(R8)
Sint32 CKeyTogle;
#endif
aset_info *pAsetadr_dummy;
aset_info *pAsetadr_w;
asetz_info *pAsetadrz_w;
extern asetz_info asettblz[1];
extern aset_info asettbl[1];
extern Uint8 dummyarea[];

void flagwkclr(void) {
    flagworkcnt = flagworkcnt2 = 1;
    sMemSet(flagwork, 0, sizeof(flagwork));
}

void actsetchk(void) {
    if (actset_rno.w == 0)
        actsetinit();
    else
        actset();
}

void actsetinit(void) {
    Sint16 scra_h;
    Uint8 cnt;

    actset_rno.w += 2;

    pAsetadr_dummy = (aset_info *)dummyarea;
    asetadr = asetadr2 = pAsetadr_w = asettbl;
    asetadrz = asetadrz2 = pAsetadrz_w = asettblz;

    flagworkcnt = flagworkcnt2 = 1;
    cnt = 0;
    scra_h = scra_h_posit.w.h;

    if (scra_h >= 128)
        scra_h -= 128;
    else
        scra_h = 0;
    scra_h &= -128;

    while (1) {
        if (pAsetadr_w->x >= (Uint16)scra_h)
            break;
        if (pAsetadr_w->a & 128) {
            cnt = flagworkcnt;
            ++flagworkcnt;
        }
        ++pAsetadr_w;
    }
    asetadr = pAsetadr_w;

    pAsetadr_w = asetadr2;
    if (scra_h >= 128) {
        scra_h -= 128;
        while (1) {
            if (pAsetadr_w->x >= (Uint16)scra_h)
                break;
            if (pAsetadr_w->a & 128) {
                ++flagworkcnt2;
            }
            ++pAsetadr_w;
        }
    }
    asetadr2 = pAsetadr_w;
    asetposi = -1;

    actset();
}

void actset(void) {
    Sint16 scra_h;
    Uint16 index;
    Uint8 cnt;

    cnt = 0;
    scra_h = scra_h_posit.w.h & -128;
    if (asetposi == scra_h)
        return;

    if (asetposi > scra_h) {
        asetposi = scra_h;
        pAsetadr_w = asetadr2;
        scra_h -= 128;
        if (scra_h < 0)
            goto label1;

        while (1) {
            if (pAsetadr_w <= asettbl)
                goto label1;
            if ((Sint16)((pAsetadr_w - 1)->x) <= scra_h)
                goto label1;
            --pAsetadr_w;
            if (pAsetadr_w->a & 128) {
                --flagworkcnt2;
                cnt = flagworkcnt2;
            }
            if (actnoset(cnt, &index))
                break;

            --pAsetadr_w;
        }
        if (pAsetadr_w->a & 128) {
            ++flagworkcnt2;
            flagwork[index] &= 127;
        }
        ++pAsetadr_w;
    label1:
        asetadr2 = pAsetadr_w;

        pAsetadr_w = asetadr;
        scra_h += 768;

        while (1) {
            if ((Sint16)((pAsetadr_w - 1)->x) < scra_h)
                break;
            if ((pAsetadr_w - 1)->a & 128) {
                --flagworkcnt;
            }
            --pAsetadr_w;
        }
        asetadr = pAsetadr_w;
    } else {

        asetposi = scra_h;
        pAsetadr_w = asetadr;
        scra_h += 640;

        do {
            if (!(pAsetadr_w->x < (Uint16)scra_h))
                goto label2;
            if (pAsetadr_w->a & 128) {
                cnt = flagworkcnt;
                ++flagworkcnt;
            }
        } while (!actnoset(cnt, &index));

        if (pAsetadr_w->a & 128) {
            --flagworkcnt;
            flagwork[index] &= 127;
        }
    label2:
        asetadr = pAsetadr_w;

        pAsetadr_w = asetadr2;
        scra_h -= 768;
        if (scra_h >= 0) {

            while (1) {
                if (pAsetadr_w->x >= (Uint16)scra_h)
                    break;
                if (pAsetadr_w->a & 128) {
                    ++flagworkcnt2;
                }
                ++pAsetadr_w;
            }
        }
        asetadr2 = pAsetadr_w;
    }
}

Sint32 tm_setchk(Uint8 cnt, Uint16 *pIndex) {
    Uint16 tflag;
    Uint8 time;

    tflag = time_flag & 127;
    *pIndex = tflag + (Uint16)cnt * 3;
    time = pAsetadr_w->c;
    time = (time >> 5) & 7;
    switch (tflag) {
    case 0:
        return time & 1 ? 1 : 0;
    case 1:
        return time & 2 ? 1 : 0;
    case 2:
        return time & 4 ? 1 : 0;
    }

    while (1)
        ;
}

Sint32 actnoset(Uint8 cnt, Uint16 *pIndex) {
    sprite_status *pActwk;
    Sint16 zflag;
    Uint8 revflg;
    Uint8 an;

    if (tm_setchk(cnt, pIndex) != 0) {
        if (pAsetadr_w->a & 128) {
            zflag = flagwork[*pIndex] & 128 ? 0 : 1;
            flagwork[*pIndex] |= 128;
            if (zflag == 0) {
                ++pAsetadr_w;
                return 0;
            }
        }
    } else {
        ++pAsetadr_w;
        return 0;
    }

    if (actwkchk(&pActwk) != 0)
        return -1;

    pActwk->xposi.w.h = pAsetadr_w->x;
    pActwk->yposi.w.h = pAsetadr_w->y & 4095;
    revflg = (Uint16)(pAsetadr_w->y >> 14) & 3;
    pActwk->actflg = revflg;
    pActwk->cddat = revflg;
    an = pAsetadr_w->a;
    if (an & 128) {
        an &= 127;
        pActwk->cdsts = cnt;
    }
    pActwk->actno = an;

    pActwk->userflag.b.h = pAsetadr_w->b;
    pActwk->userflag.b.l = pAsetadr_w->d;
    ++pAsetadr_w;

    return 0;
}

Sint32 actwkchk(sprite_status **ppActwk) {
    Sint16 i;
    sprite_status *pActwk;

    pActwk = &actwk[32];
    for (i = 0; i < 96; ++i, ++pActwk) {

        if (pActwk->actno == 0) {
            *ppActwk = pActwk;
            return 0;
        }
    }
    return -1;
}

Sint32 actwkchk2(sprite_status *pActwk, sprite_status **ppNewActwk) {
    sprite_status *pNextActwk;
    sprite_status *pLastActwk;

    pNextActwk = pActwk + 1;
    pLastActwk = &actwk[127];

    while (1) {
        if (pNextActwk > pLastActwk)
            break;
        if (pNextActwk->actno == 0) {
            *ppNewActwk = pNextActwk;
            return 0;
        }
        ++pNextActwk;
    }
    return -1;
}

Sint32 frameout_s(sprite_status *pActwk) {
    return frameout_s00(pActwk, pActwk->xposi.w.h);
}

Sint32 frameout_s00(sprite_status *pActwk, Sint16 xposi) {
    Sint16 scra_h;
    Uint16 index;
    short_union tflag;
    Sint16 zflag;

    if (pActwk->actflg & 128) {
        if (!(time_flag & 128))
            return 0;
    } else {
        xposi &= -128;
        scra_h = scra_h_posit.w.h;
        scra_h -= 128;
        scra_h &= -128;
        xposi -= scra_h;
        if ((Uint16)xposi <= 640) {
            if (!(time_flag & 128))
                return 0;
        }
    }

    index = pActwk->cdsts;
    if (index) {
        index *= 3;
        tflag.b.h = 0;
        tflag.b.l = time_flag;
        zflag = (tflag.b.l & 128) ? 0 : 1;
        tflag.w &= -129;
        if (zflag == 0) {
            tflag.w += -(Sint16)time_item;
            if (tflag.w < 0)
                tflag.w = 0;
            else if (tflag.w >= 3)
                tflag.w = 2;
        }
        index += tflag.w;
        flagwork[index] &= 127;
    }
    frameout(pActwk);
    return -1;
}

Sint32 frameout_s0(sprite_status *pActwk) {
    Uint16 index;
    short_union tflag;
    Sint16 zflag;

    index = pActwk->cdsts;
    if (index) {
        index *= 3;
        tflag.b.h = 0;
        tflag.b.l = time_flag;
        zflag = (tflag.b.l & 128) ? 0 : 1;
        tflag.w &= -129;
        if (zflag == 0) {
            tflag.w += -(Sint16)time_item;
            if (tflag.w < 0)
                tflag.w = 0;
            else if (tflag.w >= 3)
                tflag.w = 2;
        }
        index += tflag.w;
        flagwork[index] &= 127;
    }
    frameout(pActwk);
    return -1;
}
