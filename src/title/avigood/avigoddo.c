#include "../../types.h"
#include "avigoddo.h"
#include "avigoden.h"

#if defined(AVIBAD)
static char *fileName[2] = {"../TITLE/BMP/BADAVI/MES1.BMP",
                            "../TITLE/BMP/GOODAVI/MES2.BMP"};
static POINT bmpSize[2] = {{72, 16}, {160, 24}};
static POINT bmpPosi[2] = {{124, 168}, {80, 100}};
#endif

#if defined(AVIGOOD)
static char *fileName[2] = {"../TITLE/BMP/GOODAVI/MES1.BMP",
                            "../TITLE/BMP/GOODAVI/MES2.BMP"};
static POINT bmpSize[2] = {{128, 16}, {160, 24}};
static POINT bmpPosi[2] = {{96, 168}, {80, 100}};
#endif

extern Sint32 (*sMovieStat)(void);
extern Sint32 (*sMoviePlay)(Sint32);
extern Sint32 nTimerCunt;
Sint32 nPlayFileNum;
extern Sint32 (*sMovieStop)(void);
static Sint32 nDispBMPFileNum;
extern Uint32 *lpbFullScreen;
extern Uint16 swData1;
static Sint32 hBmp[2];

void AVIOpeningMove(void) {
    Sint32 stat;

    if (nTimerCunt == 0) {

#if defined(AVIBAD)
        sMoviePlay(2); /* Line 94 */
#elif defined(AVIGOOD)
        sMoviePlay(4);
#endif
    } else {
        stat = sMovieStat();
        if (stat == 3) {
            PlayNextAVI();
        }
    }
}

void PlayNextAVI(void) {

    ++nPlayFileNum;
    sMovieStop();
    if (nPlayFileNum < 2) {

#if defined(AVIBAD)
        sMoviePlay(3); /* Line 119 */
#elif defined(AVIGOOD)
        sMoviePlay(5);
#endif
    } else {

        OESetSequenceNum(2);
    }
}

void AVIEnd(void) { sMovieStop(); }

void AVIEndWait(void) {

    if (*lpbFullScreen != 0) {
        OESetSequenceNum(3);
    } else if (nTimerCunt > 120) {

        OESetSequenceNum(3);
        nDispBMPFileNum = 1;
    }
}

Sint32 AVIEndKeyWait(void) {
    Sint32 ret = 0;

    if ((swData1 & 240) || *lpbFullScreen == 1) {

#if defined(AVIBAD)
        ret = 17;
#elif defined(AVIGOOD)
        ret = 16;
#endif
    }
    return ret;
}

void ReadDIB(void) {}

void FreeDIB(void) {}
