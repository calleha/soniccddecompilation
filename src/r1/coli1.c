#include "../equ.h"
#include "coli1.h"
#include "../fcol.h"
#include "../loader2.h"
#include "../player.h"
#include "../player_work.h"
#include "../score.h"

extern Sint16 actwkchk(sprite_status **ppActwk);

Uint16 escoretbl[4] = {10, 20, 50, 100};
Uint8 colitbl[64][2] = {
    {20, 20}, {18, 12}, {12, 16}, {4, 16},  {12, 18},  {16, 16}, {6, 6},
    {24, 12}, {12, 16}, {16, 12}, {8, 8},   {20, 16},  {20, 8},  {14, 14},
    {24, 24}, {40, 16}, {16, 24}, {8, 16},  {32, 112}, {64, 32}, {128, 32},
    {32, 32}, {8, 8},   {4, 4},   {32, 8},  {12, 12},  {8, 4},   {24, 4},
    {40, 4},  {4, 8},   {4, 24},  {4, 40},  {4, 32},   {24, 24}, {12, 24},
    {72, 8},  {8, 12},  {16, 8},  {32, 16}, {32, 16},  {8, 16},  {16, 16},
    {12, 12}, {16, 16}, {4, 4},   {16, 16}, {22, 26},  {0, 0},   {0, 0},
    {0, 0},   {0, 0},   {0, 0},   {0, 0},   {0, 0},    {0, 0},   {0, 0},
    {0, 0},   {40, 36}, {18, 17}, {32, 24}, {12, 20},  {32, 12}, {12, 16},
    {0, 0}};
#ifdef R11A
extern Uint8 bCarry;
#else
static Uint8 bCarry;
#endif

Sint16 pcol(sprite_status *pActwk) {
    Sint16 iXwork = pActwk->xposi.w.h, iYwork = pActwk->yposi.w.h,
           iSprvs = pActwk->sprvsize - 3;
    Sint16 iXcollichk = 16;
    Sint16 i, iRet, iColino;
    sprite_status *pColliAct;

    iXwork -= 8;
    iYwork -= iSprvs;

    if (pActwk->patno == 57) {
        iYwork += 12;
        iSprvs = 10;
    }

    iXcollichk = 16;
    iSprvs += iSprvs;

    pColliAct = &actwk[32];

    for (i = 0; i < 96; ++i, ++pColliAct) {

        if (pColliAct->actflg & 128) {
            iColino = pColliAct->colino;
            if (iColino != 0) {
                iRet =
                    CollitblDataXchk(pActwk, pColliAct, iXwork, iYwork, iSprvs);
                if (iRet == -1)
                    return iRet;
            }
        }
    }
    iColino = 0;
    return iColino;
}

Sint16 CollitblDataXchk(sprite_status *pActwk, sprite_status *pColliAct,
                        Sint16 iXposi, Sint16 iYposi, Sint16 iD5) {
    Sint16 iColiNo = pColliAct->colino;
    Sint16 iColiwk;
    Sint16 iColiData;

    iColiNo = (iColiNo & 63) - 1;
    iColiData = colitbl[iColiNo][0];

    iColiwk = pColliAct->xposi.w.h - iColiData;
    bCarry = CCset(iColiwk, iXposi);
    iColiwk = iColiwk - iXposi;

    if (bCarry == 1) {
        iColiData += iColiData;
        bCarry = CSset(iColiwk, iColiData);
        iColiwk += iColiData;
        if (bCarry == 1)
            return CollitblDataYchk(pActwk, pColliAct, iXposi, iYposi, iColiNo,
                                    iD5);
        else
            return 0;
    }

    if (iColiwk > 16)
        return 0;

    return CollitblDataYchk(pActwk, pColliAct, iXposi, iYposi, iColiNo, iD5);
}

Sint16 CollitblDataYchk(sprite_status *pActwk, sprite_status *pColliAct,
                        Sint16 iXposi, Sint16 iYposi, Sint16 iColiNo,
                        Sint16 iD5) {
    Sint16 iColiData;
    Sint16 iYwork;

    iColiData = colitbl[iColiNo][1];

    iYwork = pColliAct->yposi.w.h - iColiData;
    bCarry = CCset(iYwork, iYposi);
    iYwork = iYwork - iYposi;
    if (bCarry == 1) {
        iColiData += iColiData;
        bCarry = CSset(iColiData, iYwork);
        iColiData += iYwork;
        if (bCarry == 1)
            return ColliHitChk(pActwk, pColliAct, iXposi, iYwork, iD5);
        else
            return 0;
    }

    if (iYwork > iD5)
        return 0;

    return ColliHitChk(pActwk, pColliAct, iXposi, iYwork, iD5);
}

Sint16 ColliHitChk(sprite_status *pActwk, sprite_status *pColliAct,
                   Sint16 iXposi, Sint16 iChkPosi, Sint16 iD5) {
    Uint8 iColino = pColliAct->colino & 192;

    if (iColino == 0) {
        pcolnomal(pActwk, pColliAct);
        return 1;
    }
    if (iColino == 192) {
        pcolspecial(pActwk, pColliAct, iXposi, iChkPosi, iD5);
        return 1;
    }
    if ((char)iColino < 0) {
        pcolplay(pActwk, pColliAct);
        return 1;
    }

    return pcolitem(pActwk, pColliAct);
}

Sint16 pcolitem(sprite_status *pActwk, sprite_status *pColliAct) {
    Sint16 iD0;

    if ((pColliAct->colino & 63) != 6) {

        if (player_work_get(pActwk)->damage_invulnerability_timer < 90)
            pColliAct->r_no0 += 2;
    } else {

        if (pActwk->yspeed.w < 0) {
            iD0 = pActwk->yposi.w.h - 16;
            bCarry = CCset(iD0, pColliAct->yposi.w.h);
            if (bCarry == 0) {
                pActwk->yspeed.w = -pActwk->yspeed.w;
                pColliAct->yspeed.w = -384;
                if (pColliAct->r_no1 == 0)
                    pColliAct->r_no1 += 4;
            }
        } else {

            if (pActwk->mstno.b.h == 2) {
                pActwk->yspeed.w = -pActwk->yspeed.w;
                pColliAct->r_no0 += 2;
            }
        }
    }
    return -1;
}

Sint16 pcolnomal(sprite_status *pActwk, sprite_status *pColliAct) {
    Sint16 iScoreCntwk;
    Sint16 iScoreData;
    score_marker_work *score_marker = score_marker_work_get(pColliAct);

    if (plpower_a == 0 && plpower_m == 0 && pActwk->mstno.b.h != 2) {
        pcolplay(pActwk, pColliAct);
        return -1;
    }

    if (pColliAct->colicnt != 0) {
        pActwk->xspeed.w = -pActwk->xspeed.w;
        pActwk->yspeed.w = -pActwk->yspeed.w;
        pActwk->xspeed.w /= 2;
        pActwk->yspeed.w /= 2;
        pColliAct->colino = 0;
        --pColliAct->colicnt;
        if (pColliAct->colicnt == 0)
            pColliAct->cddat |= 128;
    } else {

        pColliAct->cddat |= 128;
        iScoreCntwk = emyscorecnt;
        emyscorecnt += 2;
        if ((Uint16)iScoreCntwk >= 6)
            iScoreCntwk = 6;

        score_marker->points_index = iScoreCntwk;
        iScoreData = escoretbl[iScoreCntwk / 2];
        if (emyscorecnt >= 32) {
            iScoreData = 1000;
            score_marker->points_index = 10;
        }

        scoreup(iScoreData);
        soundset(150);
        pColliAct->actno = 24;
        pColliAct->r_no0 = 0;
        pColliAct->userflag.b.h = 1;

        if (pActwk->yspeed.w >= 0) {
            if (pActwk->yposi.w.h < pColliAct->yposi.w.h)
                pActwk->yspeed.w = -pActwk->yspeed.w;
            else
                pActwk->yspeed.w -= 256;
        } else
            pActwk->yspeed.w += 256;
    }
    return -1;
}

Sint16 pcolplay2(sprite_status *pActwk, sprite_status *pColliAct) {
    pColliAct->cddat |= 128;
    return pcolplay(pActwk, pColliAct);
}

Sint16 pcolplay(sprite_status *pActwk, sprite_status *pColliAct) {
    if (plpower_m == 0)
        return pcole(pActwk, pColliAct);

    return -1;
}

Sint16 pcole(sprite_status *pActwk, sprite_status *pColliAct) {
    if (player_work_get(pActwk)->damage_invulnerability_timer != 0)
        return -1;

    return playdamageset(pActwk, pColliAct);
}

void playdamagechk(sprite_status *pActwk, sprite_status *pColliAct) {

    player_work_get(pActwk)->spin_dash_counter = 0;
    if ((plpower_b & 1) == 0)
        conbine_flag = 0;
    plpower_b &= 254;

    pActwk->r_no0 = 4;
    jumpcolsub();
    pActwk->cddat |= 2;

    if (pActwk->cddat & 64) {
        pActwk->yspeed.w = -512;
        pActwk->xspeed.w = -256;
    } else {
        pActwk->yspeed.w = -1024;
        pActwk->xspeed.w = -512;
    }

    if ((Uint16)pColliAct->xposi.w.h <= (Uint16)pActwk->xposi.w.h)
        pActwk->xspeed.w = -pActwk->xspeed.w;

    pActwk->mspeed.w = 0;
    pActwk->mstno.b.h = 26;
    player_work_get(pActwk)->damage_invulnerability_timer = 120;
}

Sint16 playdamageset(sprite_status *pActwk, sprite_status *pColliAct) {
    sprite_status *pFreeAct;
    Sint16 iRet;

    if (plpower_b != 0) {
        playdamagechk(pActwk, pColliAct);
        return -1;
    }
    if (plring != 0) {
        iRet = actwkchk(&pFreeAct);
        if (iRet == 0) {
            pFreeAct->actno = 17;
            pFreeAct->xposi.w.h = pActwk->xposi.w.h;
            pFreeAct->yposi.w.h = pActwk->yposi.w.h;
        }

        playdamagechk(pActwk, pColliAct);
        return -1;
    }

    if (debugflag.w != 0) {
        playdamagechk(pActwk, pColliAct);
        return -1;
    }
    return playdieset(pActwk);
}

Sint16 playdieset(sprite_status *pActwk) {
    if (editmode.w != 0)
        return -1;
    plpower_m = 0;
    pActwk->r_no0 = 6;
    jumpcolsub();
    pActwk->cddat |= 2;
    pActwk->yspeed.w = -1792;
    pActwk->xspeed.w = 0;
    pActwk->mspeed.w = 0;
    player_work_get(pActwk)->death_y = pActwk->yposi.w.h;
    pActwk->mstno.b.h = 24;
    pActwk->sproffset |= 32768;
    pActwk->sprpri = 0;
    soundset(147);
    return -1;
}

Sint16 pcolspecial(sprite_status *pActwk, sprite_status *pColliAct,
                   Sint16 iXposi, Sint16 iChkPosi, Sint16 iD5) {
    char cColiNo = pColliAct->colino & 63;

    switch (cColiNo) {
    case 11:
        pcolplay2(pActwk, pColliAct);
        break;
    case 12:
        yago(pActwk, pColliAct, iXposi, iChkPosi, iD5);
        break;
    case 23:
    case 33:
    case 31:
    case 35:
        ++pColliAct->colicnt;
        break;
    case 47:
    case 58:
    case 59:
        main_attack(pActwk, pColliAct);
        break;
    default:
        if (cColiNo < 60)
            return -1;
        if (cColiNo > 63)
            return -1;
        eggman_chk(pActwk, pColliAct);
    }
    return -1;
}

Sint16 eggman_chk(sprite_status *pActwk, sprite_status *pColliAct) {
    if (bossstart != 1)
        return -1;
    pcolnomal(pActwk, pColliAct);
    if (pColliAct->colino == 0)
        pColliAct->colicnt += 3;
    pColliAct->colino = 0;
    ++pColliAct->colicnt;
    return 1;
}

Sint16 yago(sprite_status *pActwk, sprite_status *pColliAct, Sint16 iXposi,
            Sint16 iChkPosi, Sint16 iD5) {
    iD5 -= iChkPosi;
    if (iD5 >= 8)
        return pcolnomal(pActwk, pColliAct);
    iChkPosi = pColliAct->xposi.w.h - 4;
    if (pColliAct->cddat & 1)
        iChkPosi -= 16;

    bCarry = CCset(iChkPosi, iXposi);
    iChkPosi -= iXposi;
    if (bCarry == 1) {
        bCarry = CSset(iChkPosi, 24);
        iChkPosi += 24;
        if (bCarry == 1)
            return pcolplay(pActwk, pColliAct);
        else
            return pcolnomal(pActwk, pColliAct);
    }

    if ((Uint16)iChkPosi >= 16)
        return pcolplay(pActwk, pColliAct);
    else
        return pcolnomal(pActwk, pColliAct);
}

Sint16 main_attack(sprite_status *pActwk, sprite_status *pColliAct) {
    if (pActwk->mstno.b.h == 2)
        ++pColliAct->colicnt;
    return 1;
}
