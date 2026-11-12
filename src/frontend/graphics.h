#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <SDL3/SDL.h>
#include "../types.h"

extern Uint16 mapwk[32768];
extern PALETTEENTRY colorwk[64];
extern PALETTEENTRY colorwk2[64];
extern PALETTEENTRY colorwk3[64];
extern PALETTEENTRY colorwk4[64];
extern int_union hscrollbuff[256];
extern Sint32 fade_flag;

Sint32 SetGrid(Sint32 base, Sint32 x, Sint32 y, Sint32 block, Sint32 frip);
void EAsprset(Sint16 x, Sint16 y, Uint16 index, Uint16 linkdata, Uint16 reverse);
void ClrSpriteDebug(void);
void ChangeTileBmp(Sint32 tile_start, Sint32 bmp_no);

int Graphics_Init(SDL_Window **window, SDL_Renderer **renderer);
void Graphics_BeginRound(void);
int Graphics_LoadTiles(const char *path);
int Graphics_LoadChangeTiles(const char *path);
int Graphics_LoadSprites(const char *path, bmp_info *spriteInfo, int spriteInfoCount);
int Graphics_LoadWarp(bmp_info *spriteInfo, int spriteInfoCount);
int Graphics_LoadPlanet(const Uint16 *rotateMap, bmp_info *spriteInfo, int spriteInfoCount);
int Graphics_LoadThanks(bmp_info *spriteInfo, int spriteInfoCount);
int Graphics_LoadSpecialStage(int stageNumber, const Uint16 *rotateMap, bmp_info *spriteInfo, int spriteInfoCount);
int Graphics_LoadSpecialClearScreen(void);
void Graphics_UpdateSpecialGroundTiles(Uint16 hane1, Uint16 hane2, Uint16 dmg1, Uint16 dmg2);
void Graphics_Draw(SDL_Renderer *renderer, Sint32 scraHPosiw, Sint32 scrbHPosiw, Sint32 vscroll, const game_info *info);
void Graphics_DrawWarp(SDL_Renderer *renderer, Sint32 scraHPosiw, Sint32 scrbHPosiw, Sint32 vscroll);
void Graphics_DrawPlanet(SDL_Renderer *renderer, const game_info *info);
void Graphics_DrawThanks(SDL_Renderer *renderer, const game_info *info);
void Graphics_DrawSpecial(SDL_Renderer *renderer, const game_info *info);
void Graphics_Shutdown(void);
void Graphics_TitleSnapshotUnfadedPalette(void);

#ifdef __cplusplus
}
#endif
