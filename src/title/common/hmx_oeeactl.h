#include "hmx_types.h"

void EAError(Sint32 ret, Sint32 line, char *str);
void srfDraw(void);
void ld_bitmap_file(char *fname, void *dst, Sint32 wx, Sint32 wy, Sint32 palet,
                    Sint32 unknown);
void ld_bitmap_file2(char *fname, void *dst, Sint32 sx, Sint32 sy, Sint32 wx,
                     Sint32 wy, Sint32 dx, Sint32 dy, Sint32 unk1, Sint32 unk2);
void ld_bitmap_4to8_1(void *dst, void *src, Sint32 wx, Sint32 wy, Sint32 palet,
                      Sint32 zero_chg, Sint32 flag4);
void ld_bitmap_4to8_2(void *dst, void *src, Sint32 wx, Sint32 wy, Sint32 palet,
                      Sint32 zero_chg, Sint32 flag4);
void ld_bitmap_4to8_clipped(void *dst, void *src, Sint32 wx, Sint32 wy,
                            Sint32 stride, Sint32 palet, Sint32 zero_chg);
Sint32 ld_load_sprite2(hmx_environment *buffer, char *file,
                       hmx_environment *env, hmx_bitmap **bitmaps, Sint32 size);
Sint32 ld_load_sprite1(hmx_environment *buffer, char *file,
                       hmx_environment *env, hmx_bitmap **bitmaps, Sint32 size);
Sint32 ld_load_grid2(hmx_environment *buffer, char *file, hmx_environment *env,
                     hmx_bitmap **bitmaps, Sint32 size);
Sint32 OEGridBmpCreate(void);
void OEGridBmpDelete(void);
Sint32 OESprBmpCreate(void);
void OESprBmpDelete(void);
hmx_bitmap *OESprBmpDuplicateUnfaded(Sint32 indx, Sint32 palet, Sint32 unfaded);
Sint32 OESprCreate(Uint16 timing);
Sint32 OESprCreateIndx(Sint32 indx);
void OESprDelete(Uint16 timing);
void OESprDeleteIndx(Sint32 indx);
void OEAllSprDelete(void);
Sint32 OEGridDeleteAll(void);
Sint32 OECreateEA(void);
void OEDeleteEA(void);
void sprCreate(Uint32 *handle);
void sprDelete(Sint32 handle);
