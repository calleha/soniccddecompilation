#include "equ.h"
#include "fcol.h"

extern Uint8 scdtblwk[257][16];
extern Uint8 scddirtbl[256];
extern Uint8 scdtblwk2[257][16];
extern Uint8 mapwka[8][64];

typedef struct {
    Uint8 force_position_adjust;
} fcol_work;

static fcol_work *fcol_work_get(sprite_status *pActwk) {
    return (fcol_work *)pActwk->actfree;
}

Sint16 fcol(sprite_status *pActwk) {
    Sint16 iColliVal;
    Uint8 byDirec;
    char cDirecWork;

    if (pActwk->cddat & 8) {

        dirstk[0] = 0;
        dirstk[2] = 0;
        return 0;
    }

    dirstk[0] = 3;
    dirstk[2] = 3;

    cDirecWork = pActwk->direc.b.h;
    if ((char)(cDirecWork + 32) < 0) {

        if (cDirecWork < 0) {
            --cDirecWork;
        }
        cDirecWork += 32;
    } else {

        if (cDirecWork < 0) {
            ++cDirecWork;
        }
        cDirecWork += 31;
    }

    byDirec = cDirecWork & 192;

    switch (byDirec) {

    case 64:
        iColliVal = fcol_l(pActwk);
        break;
    case 128:
        iColliVal = fcol_u(pActwk);
        break;
    case 192:
        iColliVal = fcol_r(pActwk);
        break;
    default:
        iColliVal = fcol_d(pActwk);
        break;
    }
    if (iColliVal == 1)
        iColliVal = 1;
    else
        iColliVal = 0;
    return iColliVal;
}

Sint16 fcol_d(sprite_status *pActwk) {
    fcol_work *work = fcol_work_get(pActwk);
    Sint16 iXposi, iYposi;
    Sint16 iScd, iScd0, iScd1;
    char *cpDirStk;

    iXposi = pActwk->xposi.w.h + (Sint16)pActwk->sprhs;
    iYposi = pActwk->yposi.w.h + (Sint16)pActwk->sprvsize;
    cpDirStk = &dirstk[0];
    iScd0 = scdchk(pActwk, iXposi, iYposi, 16, 0, 13, cpDirStk);

    iXposi = pActwk->xposi.w.h - (Sint16)pActwk->sprhs;
    iYposi = pActwk->yposi.w.h + (Sint16)pActwk->sprvsize;
    cpDirStk = &dirstk[2];
    iScd1 = scdchk(pActwk, iXposi, iYposi, 16, 0, 13, cpDirStk);

    if (dirstk[2] != -1) {

        if (iScd0 == 0 && iScd1 == 0) {
            if (dirstk[2] >= -64 && dirstk[2] < -16) {

                dirstk[2] = -1;
            }
        }
    }

    iScd = dirchk(pActwk, iScd0, iScd1);

    if (iScd == 0)
        return 0;

    if (iScd < 0) {

        if (iScd >= -14)
            pActwk->yposi.w.h += iScd;
        return 1;
    }

    if (iScd <= 14) {

        pActwk->yposi.w.h += iScd;
        return 1;
    }

    if (work->force_position_adjust == 0) {

        pActwk->cddat |= 2;
        pActwk->cddat &= 223;
        pActwk->mstno.b.l = 1;
    } else {

        pActwk->yposi.w.h += iScd;
    }
    return 1;
}

Sint16 fcol_r(sprite_status *pActwk) {
    fcol_work *work = fcol_work_get(pActwk);
    Sint16 iXposi, iYposi;
    Sint16 scd0, scd1;
    char *cpDirStk;

    iXposi = pActwk->xposi.w.h + (Sint16)pActwk->sprvsize;
    iYposi = pActwk->yposi.w.h - (Sint16)pActwk->sprhs;
    cpDirStk = &dirstk[0];
    scd0 = scdchk_r(pActwk, iXposi, iYposi, 16, 0, 13, cpDirStk);

    iXposi = pActwk->xposi.w.h + (Sint16)pActwk->sprvsize;
    iYposi = pActwk->yposi.w.h + (Sint16)pActwk->sprhs;
    cpDirStk = &dirstk[2];
    scd1 = scdchk_r(pActwk, iXposi, iYposi, 16, 0, 13, cpDirStk);

    if ((scd0 = dirchk(pActwk, scd0, scd1)) == 0)
        return 0;
    if (scd0 < 0) {

        if (scd0 > -14)
            pActwk->xposi.w.h += scd0;
        return 1;
    }

    if (scd0 > 14) {

        if (work->force_position_adjust == 0) {

            pActwk->cddat |= 2;
            pActwk->cddat &= 223;
            pActwk->mstno.b.l = 1;
        } else
            pActwk->xposi.w.h += scd0;
        return 1;
    }

    pActwk->xposi.w.h += scd0;
    return 1;
}

Sint16 fcol_u(sprite_status *pActwk) {
    fcol_work *work = fcol_work_get(pActwk);
    Sint16 iXposi, iYposi;
    Sint16 scd0, scd1;
    char *cpDirStk;

    iYposi = (pActwk->yposi.w.h - (Sint16)pActwk->sprvsize) ^ 15;
    iXposi = pActwk->xposi.w.h + (Sint16)pActwk->sprhs;
    cpDirStk = &dirstk[0];
    scd0 = scdchk(pActwk, iXposi, iYposi, -16, 4096, 13, cpDirStk);

    iYposi = (pActwk->yposi.w.h - (Sint16)pActwk->sprvsize) ^ 15;
    iXposi = pActwk->xposi.w.h - (Sint16)pActwk->sprhs;
    cpDirStk = &dirstk[2];
    scd1 = scdchk(pActwk, iXposi, iYposi, -16, 4096, 13, cpDirStk);

    if ((scd0 = dirchk(pActwk, scd0, scd1)) == 0)
        return 0;

    if (scd0 < 0) {

        if (scd0 > -14)
            pActwk->yposi.w.h -= scd0;
        return 1;
    }

    if (scd0 > 14) {

        if (work->force_position_adjust == 0) {

            pActwk->cddat |= 2;
            pActwk->cddat &= 223;
            pActwk->mstno.b.l = 1;
        } else
            pActwk->yposi.w.h -= scd0;
        return 1;
    }

    pActwk->yposi.w.h -= scd0;
    return 1;
}

Sint16 fcol_l(sprite_status *pActwk) {
    fcol_work *work = fcol_work_get(pActwk);
    Sint16 iXposi, iYposi;
    Sint16 scd0, scd1;
    char *cpDirStk;

    iYposi = pActwk->yposi.w.h - (Sint16)pActwk->sprhs;
    iXposi = (pActwk->xposi.w.h - (Sint16)pActwk->sprvsize) ^ 15;
    cpDirStk = &dirstk[0];
    scd0 = scdchk_r(pActwk, iXposi, iYposi, -16, 2048, 13, cpDirStk);

    iYposi = pActwk->yposi.w.h + (Sint16)pActwk->sprhs;
    iXposi = (pActwk->xposi.w.h - (Sint16)pActwk->sprvsize) ^ 15;
    cpDirStk = &dirstk[2];
    scd1 = scdchk_r(pActwk, iXposi, iYposi, -16, 2048, 13, cpDirStk);

    if ((scd0 = dirchk(pActwk, scd0, scd1)) == 0)
        return 0;
    if (scd0 < 0) {

        if (scd0 > -14)
            pActwk->xposi.w.h -= scd0;
        return 1;
    }

    if (scd0 > 14) {

        if (work->force_position_adjust == 0) {

            pActwk->cddat |= 2;
            pActwk->cddat &= 223;
            pActwk->mstno.b.l = 1;
        } else
            pActwk->xposi.w.h -= scd0;
        return 1;
    }
    pActwk->xposi.w.h -= scd0;
    return 1;
}

Sint16 scdend(sprite_status *pActwk, Sint16 iXposi, Sint16 iYposi,
              Sint16 iOffset, Sint16 iBlkMsk, Sint16 iRideon, char *cpDirStk) {
    Sint16 iScd;

    iYposi += iOffset;
    iScd = scdchk2(pActwk, iXposi, iYposi, iOffset, iBlkMsk, iRideon, cpDirStk);
    iYposi -= iOffset;
    return iScd + 16;
}

Sint16 scdchk(sprite_status *pActwk, Sint16 iXposi, Sint16 iYposi,
              Sint16 iOffset, Sint16 iBlkMsk, Sint16 iRideon, char *cpDirStk) {
    Sint16 iBlkNo;
    Sint16 iBlkOffset;
    Sint16 iXwork;
    Sint16 iScdData;
    Uint16 iScdNo;
    char cDirect;
    Uint8 bySCDwk;
    char cScdwk;

    iBlkNo = scramapad(pActwk, iXposi, iYposi);

    iBlkOffset = iBlkNo & 1023;
    if (iBlkOffset == 0 || !((1 << iRideon) & iBlkNo)) {

        return scdend(pActwk, iXposi, iYposi, iOffset, iBlkMsk, iRideon,
                      cpDirStk);
    }

    bySCDwk = scdadr[iBlkOffset];
    iScdNo = (Uint16)(bySCDwk & 255);
    if (iScdNo == 0) {

        return scdend(pActwk, iXposi, iYposi, iOffset, iBlkMsk, iRideon,
                      cpDirStk);
    }

    bySCDwk = scddirtbl[iScdNo];
    cDirect = bySCDwk;

    iXwork = iXposi;

    if (iBlkNo & 2048) {

        iXwork = ~iXwork;
        cDirect = -cDirect;
    }
    if (iBlkNo & 4096) {

        cDirect = -(cDirect + 64) - 64;
    }
    *cpDirStk = cDirect;
    iXwork &= 15;

    cScdwk = scdtblwk[iScdNo][iXwork];
    iScdData = cScdwk;

    iBlkNo ^= iBlkMsk;
    if (iBlkNo & 4096) {
        iScdData = -iScdData;
    }

    if (iScdData == 0)
        return scdend(pActwk, iXposi, iYposi, iOffset, iBlkMsk, iRideon,
                      cpDirStk);
    if (iScdData >= 0) {
        if (iScdData != 16) {
        label1:
            return 15 - (iScdData + (iYposi & 15));
        }
    } else if (iOffset == 16) {

        iScdData = 16;
        *cpDirStk = 0;
        goto label1;
    } else {
        iScdData += iYposi & 15;
        if (iScdData >= 0)
            return scdend(pActwk, iXposi, iYposi, iOffset, iBlkMsk, iRideon,
                          cpDirStk);
    }
    iYposi -= iOffset;
    iScdData =
        scdchk2(pActwk, iXposi, iYposi, iOffset, iBlkMsk, iRideon, cpDirStk);
    iYposi += iOffset;
    return iScdData - 16;
}

Sint16 scdchk2(sprite_status *pActwk, Sint16 iXposi, Sint16 iYposi,
               Sint16 iOffset, Sint16 iBlkMsk, Sint16 iRideon, char *cpDirStk) {
    Sint16 iBlkNo;
    Sint16 iBlkOffset;
    Sint16 iScd;
    Sint16 iXwork;
    Sint16 iScdData;
    char cDirect;
    Uint16 iScdNo;
    Uint8 bySCDwk;
    char cScdwk;

    iBlkNo = scramapad(pActwk, iXposi, iYposi);
    iBlkOffset = iBlkNo & 1023;
    if (iBlkOffset == 0 || !((1 << iRideon) & iBlkNo)) {

        return 15 - (iYposi & 15);
    }

    bySCDwk = scdadr[iBlkOffset];
    iScdNo = (Uint16)(bySCDwk & 255);
    if (iScdNo == 0) {

        return 15 - (iYposi & 15);
    }

    cDirect = scddirtbl[iScdNo];
    iXwork = iXposi;
    if (iBlkNo & 2048) {

        iXwork = ~iXwork;
        cDirect = -cDirect;
    }
    if (iBlkNo & 4096) {

        cDirect = -(cDirect + 64) - 64;
    }
    *cpDirStk = cDirect;

    iXwork &= 15;

    cScdwk = scdtblwk[iScdNo][iXwork];
    iScdData = cScdwk;

    iBlkNo ^= iBlkMsk;
    if (iBlkNo & 4096)
        iScdData *= -1;

    if (iScdData == 0)

        return 15 - (iYposi & 15);
    if (iScdData >= 0) {
    label1:
        iYposi &= 15;
        iScdData += iYposi;
        return 15 - iScdData;
    }

    if (iOffset == 16) {

        iScdData = 16;
        *cpDirStk = 0;
        goto label1;
    }

    iScd = iYposi & 15;
    if (iScd + iScdData > 0) {
        return 15 - iScd;
    }
    return ~iScd;
}

Sint16 scdend_r(sprite_status *pActwk, Sint16 iXposi, Sint16 iYposi,
                Sint16 iOffset, Sint16 iBlkMsk, Sint16 iRideon,
                char *cpDirstk) {
    Sint16 iScd;

    iXposi += iOffset;
    iScd = scdchk2_r(pActwk, iXposi, iYposi, iBlkMsk, iRideon, cpDirstk);
    iXposi -= iOffset;
    return iScd + 16;
}

Sint16 scdchk_r(sprite_status *pActwk, Sint16 iXposi, Sint16 iYposi,
                Sint16 iOffset, Sint16 iBlkMsk, Sint16 iRideon,
                char *cpDirstk) {
    Sint16 iBlkNo;
    Sint16 iBlkOffset;
    Sint16 iXwork;
    Sint16 iYwork;
    Sint16 iScdData;
    char cDirect;
    Uint16 iScdNo;
    Uint8 bySCDwk;
    char cScdwk;

    iBlkNo = scramapad(pActwk, iXposi, iYposi);
    iBlkOffset = iBlkNo & 1023;
    if (iBlkOffset == 0 || !(iBlkNo & (1 << iRideon))) {

        return scdend_r(pActwk, iXposi, iYposi, iOffset, iBlkMsk, iRideon,
                        cpDirstk);
    }

    bySCDwk = scdadr[iBlkOffset];
    iScdNo = (Uint16)(bySCDwk & 255);
    if (iScdNo == 0) {

        return scdend_r(pActwk, iXposi, iYposi, iOffset, iBlkMsk, iRideon,
                        cpDirstk);
    }

    cDirect = scddirtbl[iScdNo];
    iYwork = iYposi;

    if (iBlkNo & 4096) {

        iYwork = ~iYwork;
        cDirect = -(cDirect + 64) - 64;
    }
    if (iBlkNo & 2048) {

        cDirect = -cDirect;
    }
    *cpDirstk = cDirect;

    iYwork &= 15;

    cScdwk = scdtblwk2[iScdNo][iYwork];
    iScdData = cScdwk;

    iBlkNo ^= iBlkMsk;
    if (iBlkNo & 2048)
        iScdData *= -1;

    if (iScdData == 0)
        return scdend_r(pActwk, iXposi, iYposi, iOffset, iBlkMsk, iRideon,
                        cpDirstk);
    if (iScdData >= 0) {
        if (iScdData == 16)
            goto label1;

        iXposi &= 15;
        return 15 - (iScdData + iXposi);
    }
    iXwork = iXposi & 15;
    iScdData += iXwork;
    if (iScdData >= 0)
        return scdend_r(pActwk, iXposi, iYposi, iOffset, iBlkMsk, iRideon,
                        cpDirstk);
label1:
    iXposi -= iOffset;
    iScdData = scdchk2_r(pActwk, iXposi, iYposi, iBlkMsk, iRideon, cpDirstk);
    iXposi += iOffset;
    return iScdData - 16;
}

Sint16 scdchk2_r(sprite_status *pActwk, Sint16 iXposi, Sint16 iYposi,
                 Sint16 iBlkMsk, Sint16 iRideon, char *cpDirstk) {
    Sint16 iBlkNo;
    Sint16 iBlkOffset;
    Sint16 iXwork;
    Sint16 iYwork;
    Sint16 iScdData;
    char cDirect;
    Uint16 iScdNo;
    Uint8 bySCDwk;
    char cScdwk;

    iBlkNo = scramapad(pActwk, iXposi, iYposi);
    iBlkOffset = iBlkNo & 1023;
    if (iBlkOffset == 0 || !((1 << iRideon) & iBlkNo)) {

        return 15 - (iXposi & 15);
    }

    bySCDwk = scdadr[iBlkOffset];
    iScdNo = (Uint16)(bySCDwk & 255);
    if (iScdNo == 0) {

        return 15 - (iXposi & 15);
    }

    cDirect = scddirtbl[iScdNo];
    iYwork = iYposi;
    if (iBlkNo & 4096) {

        iYwork = ~iYwork;
        cDirect = -(cDirect + 64) - 64;
    }
    if (iBlkNo & 2048) {

        cDirect = -cDirect;
    }
    *cpDirstk = cDirect;

    iYwork &= 15;

    cScdwk = scdtblwk2[iScdNo][iYwork];
    iScdData = cScdwk;

    iBlkNo ^= iBlkMsk;
    if (iBlkNo & 2048)
        iScdData *= -1;

    if (iScdData == 0) {

        return 15 - (iXposi & 15);
    }
    if (iScdData >= 0) {

        iXwork = iXposi & 15;
        iXwork += iScdData;
        iXwork = 15 - iXwork;
        return iXwork;
    }

    iXwork = iXposi & 15;
    if ((iXwork + iScdData) >= 0) {

        return 15 - (iXposi & 15);
    }
    return ~iXwork;
}

Sint16 scramapad(sprite_status *pActwk, Sint16 iXposi, Sint16 iYposi) {
    char cBlkNo;
    Sint16 iBlkNo;
    Sint16 iMapNo;
    Sint16 iXwork, iYwork;
    Uint16 *lpw;

    if (stageno.w == 0 || stageno.b.h == 6) {

        if (iYposi < 0) {
            /* Above the top of the level. The original indexed mapwka with a
               row of -1, which landed in the all-zero mapwkb buffer that sits
               directly before it - i.e. "nothing here", which is how the
               Mega-CD version lets Sonic fly above the level top in Metallic
               Madness (R8) and run along the top of that structure. Clamping y
               to 0 instead samples the stage's top band, whose block map is
               full of geometry, so the player is blocked horizontally there. */
            if (stageno.b.h == 6)
                return 0;
            iYposi = 0;
        }
        if (iXposi < 0)
            iXposi = 0;
    }

    iYwork = (iYposi >> 8) & 7;
    if (stageno.b.h == 2) {
        iYwork &= 63;
    }

    iXwork = iXposi & 63;

    iYwork = iYposi >> 1;
    iYwork &= 1920;
    iYwork >>= 7;
    iYwork &= 7;

    iXwork = iXposi >> 8;
    iXwork &= 127;
    iXwork &= 63;

    if (stageno.w != 0) {

        if (iYwork < 0)
            iYwork = 0;
        if (iXwork < 0)
            iXwork = 0;
    }

    cBlkNo = mapwka[iYwork][iXwork];
    if (cBlkNo == 0)
        return 0;
    if (cBlkNo > 0) {

        if (stageno.b.h == 5 || stageno.b.h == 6) {

            pActwk->sproffset &= 32767;
        }
        if (stageno.b.h == 4) {
            pActwk->actflg &= 191;
        }

        iBlkNo = cBlkNo & 127;

        iYwork = (Sint16)(iYposi & 240) >> 4;
        iXwork = (Sint16)(iXposi & 240) >> 4;

        iMapNo = 0;

        lpw = pmapwk;
        lpw += iXwork;
        lpw += iYwork << 4;
        lpw += (iBlkNo - 1 << 4) << 4;
        iMapNo = *lpw;
        return iMapNo;
    }

    iBlkNo = cBlkNo & 127;
    switch ((Uint8)stageno.b.h) {

    case 4:
        iBlkNo = st_wackyDataChk(iBlkNo, pActwk);
        break;
    case 5:
        iBlkNo = st_starDataChk(iBlkNo, pActwk);
        break;
    case 6:
        iBlkNo = st_metalDataChk(iBlkNo, pActwk);
        break;
    default:
        iBlkNo = st_elseDataChk(iBlkNo, pActwk);
    }
    iBlkNo = iBlkNo & 127;
    iYwork = (Sint16)(iYposi & 240) >> 4;
    iXwork = (Sint16)(iXposi & 240) >> 4;

    lpw = pmapwk;
    lpw += iXwork;
    lpw += iYwork << 4;
    lpw += (iBlkNo - 1 << 4) << 4;
    return *lpw;
}

Sint16 dirchk(sprite_status *pActwk, Sint16 scd0, Sint16 scd1) {
    Uint8 cDirectWk;

    cDirectWk = dirstk[2];
    if (scd1 > scd0) {

        cDirectWk = dirstk[0];
        scd1 = scd0;
    }

    if (cDirectWk & 1)
        goto label1;
    pActwk->direc.b.h = cDirectWk;
    goto label2;

label1:
    cDirectWk = pActwk->direc.b.h;
    cDirectWk += 32;
    cDirectWk &= 192;
    pActwk->direc.b.h = cDirectWk;

label2:
    return scd1;
}

Sint16 scdcnv(void) { return 0; }

Sint16 st_elseDataChk(Sint16 iBlkNo, sprite_status *pActwk) {
    if (!(pActwk->actflg & 64))
        return iBlkNo;
    if (++iBlkNo == 41)
        return 81;
    return iBlkNo;
}

Sint16 st_wackyDataChk(Sint16 iBlkNo, sprite_status *pActwk) {
    if (pActwk->actflg & 64) {

        switch (iBlkNo) {

        case 21:
            iBlkNo = 96;
            break;
        case 30:
            iBlkNo = 97;
            break;
        case 31:
            iBlkNo = 98;
            break;
        case 50:
            iBlkNo = 99;
            break;
        }
    } else {

        if (iBlkNo == 20) {

            pActwk->actflg |= 64;
            pActwk->sproffset &= 32767;
        }
    }
    return iBlkNo;
}

Sint16 st_starDataChk(Sint16 iBlkNo, sprite_status *pActwk) {

    pActwk->sproffset |= 32768;
    if (iBlkNo == 4 || iBlkNo == 6) {

        pActwk->sproffset &= 32767;
        if (pActwk->actflg & 64)
            ++iBlkNo;
        return iBlkNo;
    }
    if (prio_flag == 0)
        return iBlkNo;

    pActwk->sproffset &= 32767;
    switch (iBlkNo) {

    case 40:
    case 60:
    case 55:
    case 47:
    case 22:
        ++iBlkNo;
        break;
    }
    return iBlkNo;
}

Sint16 st_metalDataChk(Sint16 iBlkNo, sprite_status *pActwk) {

    if (pActwk->actno >= 3)
        return iBlkNo;
    pActwk->sproffset |= 32768;
    if (prio_flag == 0)
        return iBlkNo;
    pActwk->sproffset &= 32767;

    switch (iBlkNo) {

    case 70:
        iBlkNo = 106;
        break;
    case 72:
        iBlkNo = 107;
        break;
    case 74:
        iBlkNo = 108;
        break;
    case 16:
        iBlkNo = 109;
        break;
    case 99:
        iBlkNo = 110;
        break;
    case 67:
        iBlkNo = 111;
        break;
    }
    return iBlkNo;
}

Uint8 CCset(Sint32 lSrc, Sint32 lDst) {
    lSrc -= lDst;
    if (lSrc < 0)
        return 1;
    return 0;
}

Uint8 CSset(Uint16 wSrc, Uint16 wDst) {
    Uint32 wk, lSrc, lDst;

    lSrc = wSrc;
    lDst = wDst;
    wk = lSrc + lDst;
    if (wk > 65535)
        return 1;
    return 0;
}

Uint8 bchg(Uint8 bySrc, Uint8 byDst) {
    bySrc = 1 << bySrc;
    byDst = bySrc ^ byDst;
    return byDst;
}
