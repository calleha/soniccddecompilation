#include "../../types.h"
#include "aviopndo.h"

#if defined(AVIOPEN)
/* Sized for the reference's "TITLE\\AVI\\OPN.AVI" plus the port's "../"
   prefix and the terminator. */
static char szAVIFile[24] = "../TITLE/AVI/OPN.AVI";
#endif
#if defined(AVIPEN)
static char szAVIFile[27] = "../TITLE/AVI/PENCIL.AVI";
#endif
static Uint32 bAVIStart;
extern Sint32 (*sMoviePlay)(Sint32);
extern Sint32 nTimerCunt;
extern Sint32 (*sMovieStop)(void);
static Uint32 hWndMovie;
extern Uint16 wMCIDeviceID;

void ReadDIB(char *fileName) {}

void FreeDIB(void) {}

void AVIOpeningMove(void) {
    if (nTimerCunt == 0) {

#if defined(AVIOPEN)
        sMoviePlay(0); /* Line 122 */
#elif defined(AVIPEN)
        sMoviePlay(1);
#endif

        bAVIStart = 1;
    }
}

void AVIEnd(void) {
    sMovieStop();

    bAVIStart = 0;
}

void AVIPaint(Uint32 hdc) {}
