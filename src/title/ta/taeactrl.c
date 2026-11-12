#include "../../types.h"
#include "../common/score_data_types.h"
#include "../common/hmx_types.h"
#include "../common/ld_types.h"
#include "ta_types.h"
#include "taeactrl.h"
#include "../../impfuncs.h"
#include "tacolor.h"

static grid_rect BkgRect = {0, 0, 320, 224};
static grid_rect GridRectPic = {0, 0, 128, 144};
static POINT ptSprLocPic[2] = {{24, 96}, {24, 24}};
static Uint8 TAGridSize[8][3] = {{64, 32, 0}, {64, 28, 0}, {64, 32, 0},
                                 {64, 28, 0}, {0, 0, 0},   {0, 0, 0},
                                 {0, 0, 0},   {0, 0, 0}};
static map_info MapInfo1[3] = {{1, 18, 0, 22, 28, 0, 0, "TIME_BGC.BIN"},
                               {3, 0, 0, 18, 28, 93, 0, "TIME_BGL.BIN"},
                               {3, 40, 0, 18, 28, 140, 0, "TIME_BGR.BIN"}};
static map_infos MapInfoTbl[1] = {{MapInfo1, 3}};
static Uint16 MapZoneMenuNormal[13] = {108, 109, 110, 111, 112, 108, 115,
                                       109, 110, 112, 108, 113, 114};
static Uint16 MapZoneMenuSP[13] = {108, 108, 108, 113, 114, 108, 109,
                                   110, 111, 112, 108, 108, 108};
static Uint16 MapZoneMenuSpace = 108;
static Uint16 MapRoundSpace[2][1] = {{6}, {11}};
static Uint16 MapExitSpace[2][1] = {{108}, {11}};
static Uint16 MapRound[2][8][2][18] = {
    {{{108, 108, 109, 108, 108, 110, 108, 108, 111, 108, 108, 112, 108, 108,
       108, 108, 108, 108},
      {11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11}},
     {{1, 2, 3, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
      {7, 8, 9, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11}},
     {{12, 13, 14, 15, 16, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
      {17, 18, 19, 20, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11}},
     {{21, 22, 23, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
      {24, 25, 26, 27, 28, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11}},
     {{29, 30, 31, 32, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
      {33, 34, 35, 36, 37, 38, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11}},
     {{39, 40, 41, 42, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
      {33, 43, 44, 45, 46, 47, 48, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11}},
     {{49, 50, 51, 52, 53, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
      {54, 55, 56, 57, 58, 59, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11}},
     {{60, 61, 62, 63, 64, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
      {33, 65, 66, 67, 68, 69, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
       11}}},
    {{{108, 108, 108, 108, 108, 108, 109, 108, 108, 110, 108, 108, 111, 108,
       108, 112, 108, 108},
      {11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11}},
     {{6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 70, 71, 72, 73, 74, 75, 76},
      {11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 77, 78, 79, 80, 81, 82, 83}},
     {{6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 70, 71, 72, 73, 74, 84, 85},
      {11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 77, 78, 79, 80, 81, 86, 87}},
     {{6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 70, 71, 72, 73, 74, 88, 89},
      {11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 77, 78, 79, 80, 81, 90, 91}},
     {{6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 70, 71, 72, 73, 74, 92, 93},
      {11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 77, 78, 79, 80, 81, 94, 95}},
     {{6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 70, 71, 72, 73, 74, 96, 97},
      {11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 77, 78, 79, 80, 81, 98, 99}},
     {{6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 70, 71, 72, 73, 74, 100, 101},
      {11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 77, 78, 79, 80, 81, 102,
       103}},
     {{6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 70, 71, 72, 73, 74, 104, 105},
      {11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 77, 78, 79, 80, 81, 106,
       107}}}};
static Uint16 MapRec[32][18] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 4},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 6, 4},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 4},
    {7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 6, 4},
    {9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 10, 3, 4},
    {11, 11, 11, 12, 13, 14, 12, 15, 16, 17, 18, 19, 11, 11, 11, 20, 6, 4},
    {11, 11, 11, 21, 22, 23, 24, 25, 26, 27, 28, 29, 11, 11, 11, 30, 3, 4},
    {11, 11, 11, 11, 11, 31, 32, 32, 33, 34, 11, 11, 11, 11, 11, 20, 6, 4},
    {11, 11, 11, 11, 11, 35, 36, 37, 38, 39, 11, 11, 11, 11, 11, 30, 3, 4},
    {11, 11, 11, 11, 11, 40, 41, 41, 42, 11, 11, 11, 11, 11, 11, 20, 6, 4},
    {11, 11, 11, 11, 11, 43, 44, 45, 46, 11, 11, 11, 11, 11, 11, 30, 3, 4},
    {47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 48, 6, 4},
    {1, 1, 1, 1, 49, 50, 51, 52, 49, 53, 54, 1, 1, 1, 1, 2, 3, 4},
    {1, 1, 1, 1, 55, 56, 57, 58, 55, 59, 60, 1, 1, 1, 1, 5, 6, 4},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 4},
    {61, 62, 63, 1, 1, 64, 1, 1, 65, 1, 1, 1, 1, 1, 1, 5, 6, 4},
    {66, 67, 68, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 4},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 6, 4},
    {69, 70, 71, 1, 1, 64, 1, 1, 65, 1, 1, 1, 1, 1, 1, 2, 3, 4},
    {72, 73, 74, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 6, 4},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 4},
    {69, 75, 71, 1, 1, 64, 1, 1, 65, 1, 1, 1, 1, 1, 1, 5, 6, 4},
    {76, 77, 74, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 4},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 6, 4},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 4},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 6, 4},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 4},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 6, 4},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 4},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 6, 4},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 4},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 6, 4}};
static Uint16 MapRecSP[32][18] = {
    {4, 232, 233, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {4, 234, 235, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {4, 232, 233, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {4, 234, 236, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},
    {4, 232, 237, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9},
    {4, 234, 238, 11, 11, 11, 11, 239, 240, 241, 242, 243, 98, 110, 11, 11, 11,
     11},
    {4, 232, 244, 11, 11, 11, 11, 245, 246, 247, 248, 249, 104, 114, 11, 11, 11,
     11},
    {4, 234, 238, 11, 11, 11, 11, 250, 251, 147, 41, 252, 253, 11, 11, 11, 11,
     11},
    {4, 232, 244, 11, 11, 11, 11, 254, 255, 151, 160, 256, 257, 11, 11, 11, 11,
     11},
    {4, 234, 238, 11, 11, 11, 11, 11, 40, 41, 41, 42, 11, 11, 11, 11, 11, 11},
    {4, 232, 244, 11, 11, 11, 11, 11, 43, 44, 45, 46, 11, 11, 11, 11, 11, 11},
    {4, 234, 258, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47},
    {4, 232, 233, 1, 1, 1, 1, 49, 50, 51, 52, 49, 53, 54, 1, 1, 1, 1},
    {4, 234, 235, 1, 1, 1, 1, 55, 56, 57, 58, 55, 59, 60, 1, 1, 1, 1},
    {4, 232, 233, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {4, 234, 235, 61, 62, 63, 1, 1, 64, 1, 1, 65, 1, 1, 1, 1, 1, 1},
    {4, 232, 233, 66, 67, 68, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {4, 234, 235, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {4, 232, 233, 69, 70, 71, 1, 1, 64, 1, 1, 65, 1, 1, 1, 1, 1, 1},
    {4, 234, 235, 72, 73, 74, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {4, 232, 233, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {4, 234, 235, 69, 75, 71, 1, 1, 64, 1, 1, 65, 1, 1, 1, 1, 1, 1},
    {4, 232, 233, 76, 77, 74, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {4, 234, 235, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {4, 232, 233, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {4, 234, 235, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {4, 232, 233, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {4, 234, 235, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {4, 232, 233, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {4, 234, 235, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {4, 232, 233, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {4, 234, 235, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};
static Uint16 MapRecRound[7][4][18] = {
    {{11, 11, 11, 12, 13, 14, 12, 15, 16, 17, 18, 19, 11, 11, 11, 20, 6, 4},
     {11, 11, 11, 21, 22, 23, 24, 25, 26, 27, 28, 29, 11, 11, 11, 30, 3, 4},
     {11, 11, 11, 11, 11, 31, 32, 32, 33, 34, 11, 11, 11, 11, 11, 20, 6, 4},
     {11, 11, 11, 11, 11, 35, 36, 37, 38, 39, 11, 11, 11, 11, 11, 30, 3, 4}},
    {{11, 11, 11, 78, 79, 80, 81, 82, 83, 84, 85, 86, 11, 11, 11, 20, 6, 4},
     {11, 11, 11, 87, 88, 89, 90, 91, 92, 93, 25, 94, 11, 11, 11, 30, 3, 4},
     {11, 11, 11, 11, 95, 96, 97, 98, 41, 99, 100, 11, 11, 11, 11, 20, 6, 4},
     {11, 11, 11, 11, 101, 102, 103, 104, 44, 105, 106, 11, 11, 11, 11, 30, 3,
      4}},
    {{11, 11, 11, 11, 11, 107, 108, 109, 98, 110, 11, 11, 11, 11, 11, 20, 6, 4},
     {11, 11, 11, 11, 11, 111, 112, 113, 104, 114, 11, 11, 11, 11, 11, 30, 3,
      4},
     {11, 11, 11, 115, 116, 117, 118, 119, 120, 121, 122, 123, 11, 11, 11, 20,
      6, 4},
     {11, 11, 11, 11, 124, 125, 38, 126, 127, 128, 129, 11, 11, 11, 11, 30, 3,
      4}},
    {{11, 11, 11, 11, 130, 131, 85, 132, 133, 134, 135, 11, 11, 11, 11, 20, 6,
      4},
     {11, 11, 11, 11, 136, 137, 138, 139, 140, 141, 142, 11, 11, 11, 11, 30, 3,
      4},
     {11, 11, 11, 130, 131, 85, 143, 144, 17, 145, 146, 147, 11, 11, 11, 20, 6,
      4},
     {11, 11, 11, 136, 137, 138, 23, 148, 27, 149, 150, 151, 11, 11, 11, 30, 3,
      4}},
    {{11, 11, 11, 11, 154, 80, 41, 155, 156, 157, 110, 11, 11, 11, 11, 20, 6,
      4},
     {11, 11, 11, 11, 158, 159, 160, 161, 162, 163, 164, 11, 11, 11, 11, 30, 3,
      4},
     {11, 11, 95, 33, 119, 165, 166, 167, 168, 169, 84, 170, 110, 11, 11, 20, 6,
      4},
     {11, 11, 101, 171, 172, 173, 174, 175, 176, 177, 93, 178, 179, 11, 11, 30,
      3, 4}},
    {{11, 11, 11, 180, 181, 182, 132, 183, 184, 82, 185, 186, 11, 11, 11, 20, 6,
      4},
     {11, 11, 11, 187, 188, 189, 139, 190, 191, 192, 193, 94, 11, 11, 11, 30, 3,
      4},
     {11, 11, 11, 180, 194, 195, 196, 169, 197, 33, 198, 199, 11, 11, 11, 20, 6,
      4},
     {11, 11, 11, 187, 138, 200, 201, 202, 203, 204, 205, 206, 11, 11, 11, 30,
      3, 4}},
    {{11, 11, 11, 207, 208, 209, 210, 211, 212, 213, 85, 214, 11, 11, 11, 20, 6,
      4},
     {11, 11, 11, 215, 216, 217, 218, 219, 220, 221, 89, 222, 11, 11, 11, 30, 3,
      4},
     {11, 11, 11, 207, 208, 31, 119, 223, 224, 196, 225, 226, 11, 11, 11, 20, 6,
      4},
     {11, 11, 11, 215, 216, 219, 172, 227, 228, 229, 230, 231, 11, 11, 11, 30,
      3, 4}}};
static Sint32 TileCnt = 0;
static Sint32 SprBmpCnt = 0;
Sint32 gZure = 0;
static Sint32 ErrRet = 0;
static char ErrBuf[250] = {0};
static char *lpPicBits8 = 0;
static char *lpPicBits4[2][8] = {0};
static grid_rect GridRect[8] = {0};
static Uint32 *hBmpPic[2] = {0};
static Uint32 *hBmpBkg = 0;
static Uint32 *hGridBmp[768] = {0};
static Uint32 hGrid[8] = {0};
static Uint32 hBkg = 0;
static Uint32 hSprPic[2] = {0};
static Uint32 hTile[768] = {0};
game_info *lpKeepWork = 0;
Sint32 *lpFadeFlag = 0;
int_union *lphscrollbuff = 0;
Uint16 *pmapwk = 0;
PALETTEENTRY *lpcolorwk4 = 0;
PALETTEENTRY *lpcolorwk3 = 0;
PALETTEENTRY *lpcolorwk2 = 0;
PALETTEENTRY *lpcolorwk = 0;
extern void (*FlipToScreen_module)(void);
extern void (*hmx_renderer_context_draw_module)(hmx_renderer_context *,
                                                hmx_surface *);
extern draw_context *s_ctx;
extern void (*hmx_renderer_context_add_module)(hmx_renderer_context *, Sint32,
                                               hmx_renderer_base *);
extern hmx_renderer_base *(*hmx_sprite_base_module)(hmx_sprite *);
extern hmx_renderer_base *(*hmx_grid_base_module)(hmx_grid *);
extern void (*hmx_renderer_context_clear_module)(hmx_renderer_context *);
extern void (*hmx_free_module)(hmx_environment *, void *);
extern void (*hmx_bitmap_set_transparency_module)(hmx_bitmap *, Sint32);
extern void (*ld_bitmap_4to8_module)(void *, void *, Sint32, Sint32, Sint32,
                                     Sint32, Sint32);
extern void *(*hmx_bitmap_get_scan0_module)(hmx_bitmap *);
extern hmx_bitmap *(*hmx_bitmap_create_module)(hmx_environment *, Sint32,
                                               Sint32);
extern void *(*ld_load_cmpfile_module)(hmx_environment *, char *);
extern hmx_environment *g_env_module;
extern hmx_environment *g_loader_module;
extern void (*hmx_grid_set_view_module)(hmx_grid *, Sint32, Sint32, Sint32,
                                        Sint32);
extern void (*hmx_grid_set_position_module)(hmx_grid *, Sint32, Sint32);
extern hmx_grid *(*hmx_grid_create_module)(hmx_environment *, Sint32, Sint32,
                                           Sint32, Sint32);
extern void (*hmx_grid_release_module)(hmx_environment *, hmx_grid *);
extern void (*hmx_grid_set_tile_module)(hmx_grid *, Sint32, Sint32,
                                        hmx_bitmap *, Sint32);
extern void (*hmx_background_set_background_module)(hmx_background *, Sint32);
extern void (*hmx_sprite_set_bitmap_module)(hmx_sprite *, hmx_bitmap *);
extern void (*hmx_sprite_set_position_module)(hmx_sprite *, Sint32, Sint32);
extern Sint32 gMenuRound;
extern Sint32 gMenu1;
extern Sint32 gNewMenu2;
extern Sint32 gMenuZone;
extern Sint32 gMenu2;
extern void (*hmx_bitmap_release_module)(hmx_environment *, hmx_bitmap *);
extern score_data *lpScoreData;
extern Sint32 gRankY;
extern Sint32 gRankX;
extern Uint32 gTimer;
extern Sint32 gMove;
extern Sint32 gNewRankX;
extern Sint32 gNewMenuZone;
extern Sint32 gNewMenuRound;
extern Sint32 gNewMenu1;

void EAError(Sint32 ret, Sint32 line, char *str) {
    char tmpstr[20];

    if (ret == 1)
        sPrintf(tmpstr, "EA_FAILED");
    else if (ret == 2)
        sPrintf(tmpstr, "EA_OUT_OF_MEMORY");
    else if (ret == 3)
        sPrintf(tmpstr, "EA_BAD_POINTER");
    else if (ret == 4)
        sPrintf(tmpstr, "EA_OUT_OF_RANGE");
    else if (ret == 5)
        sPrintf(tmpstr, "EA_INVALID_SIZE");
    else if (ret == 6)
        sPrintf(tmpstr, "EA_BAD_TYPE");

    sPrintf(ErrBuf, "\nError:\n%s on line %d\n\n%ld = %s\n", tmpstr, line, ret,
            str);

    sOutputDebugString(ErrBuf);
}

void srfDraw(void) {
    if (s_ctx->context != 0) {
        Sint32 i;

        hmx_renderer_context_clear_module(s_ctx->context);

        for (i = 0; i < 8; ++i) {
            if (hGrid[i] != 0)
                hmx_renderer_context_add_module(
                    s_ctx->context, 8 - i,
                    hmx_grid_base_module(s_ctx->grids[i]));
        }

        for (i = 255; i >= 0; --i) {
            if (s_ctx->spr_level[i] != 0)
                hmx_renderer_context_add_module(
                    s_ctx->context, s_ctx->spr_level[i],
                    hmx_sprite_base_module(s_ctx->sprites[i]));
        }

        hmx_renderer_context_draw_module(s_ctx->context, s_ctx->screen);
    }
    FlipToScreen_module();
}

Sint32 ld_load_grid_module(hmx_environment *buffer, char *file,
                           hmx_environment *env, hmx_bitmap **bitmaps,
                           Sint32 size) {
    Sint32 count = -1;

    void *expbuf = 0;
    ld_scroll_header *header;
    Sint32 read_count;
    Uint8 *pixbuf;
    Sint32 i, palet, paletcnt, palet_offs;
    ld_bitmap_inf *s;
    Sint32 wx, wy;
    hmx_bitmap *bmp;
    palet = 0;
    paletcnt = 0;
    palet_offs = 0;

    for (i = 0; i < size; ++i) {
        bitmaps[i] = 0;
    }

    expbuf = ld_load_cmpfile_module(buffer, file);
    if (expbuf != 0) {

        header = expbuf;
        read_count = header->header.count;
        if (size < read_count) {
            read_count = size;
        }

        pixbuf = (Uint8 *)expbuf + header->header.offset;

        for (i = 0; i < read_count; ++i) {
            s = &header->bmp[i];
            wx = s->wx;
            wy = s->wy;

            bmp = hmx_bitmap_create_module(env, wx, wy);

            if (--paletcnt <= 0) {
                do {
                    paletcnt = header->plt[palet++];
                } while (paletcnt == 0);
                switch (palet) {
                case 1:
                    palet_offs = 16;
                    break;
                case 2:
                    palet_offs = 32;
                    break;
                case 3:
                    palet_offs = 48;
                    break;
                case 4:
                    palet_offs = 64;
                    break;
                }
            }

            ld_bitmap_4to8_module(hmx_bitmap_get_scan0_module(bmp), pixbuf, wx,
                                  wy, palet_offs, 0, 0);

            pixbuf += wx * wy / 2;

            hmx_bitmap_set_transparency_module(bmp, 0);

            bitmaps[i] = bmp;
        }

        count = read_count;
    }

    if (expbuf != 0) {
        hmx_free_module(buffer, expbuf);
    }

    return count;
}

Sint32 TAGridBmpCreate(void) {
    Sint32 i, count;

    s_ctx->grd_bitmap_count = 0;
    for (i = 0; i < 2048; ++i) {
        s_ctx->grd_bitmaps[i] = 0;
    }

    count = ld_load_grid_module(g_loader_module, "../TITLE/BMP/TCMPTA.CM_",
                                g_env_module, s_ctx->grd_bitmaps, 2048);

    if (count <= 0) {
        return -1;
    }

    s_ctx->grd_bitmap_count = count;
    TileCnt = count;

    return 0;
}

Sint32 TAGridCreate(void) {
    POINT pt;
    Sint32 i;

    for (i = 0; i < 3; ++i) {
        hmx_grid_release_module(g_env_module, s_ctx->grids[i]);
        s_ctx->grids[i] = 0;
    }
    for (i = 0; i < 8; ++i) {
        if (GridRect[i].dx != 0) {
            s_ctx->grids[i] = hmx_grid_create_module(
                g_env_module, 8, 8, (Uint16)(GridRect[i].dx / 8),
                (Uint16)(GridRect[i].dy / 8));
            hGrid[i] = (Uint32)s_ctx->grids[i];

            pt.x = 0;
            pt.y = TAGridSize[i][2] * 8;
            hmx_grid_set_position_module(s_ctx->grids[i], pt.x, pt.y);
            hmx_grid_set_view_module(s_ctx->grids[i], GridRect[i].x,
                                     GridRect[i].y, GridRect[i].dx,
                                     GridRect[i].dy);
        }
    }
    return 0;
}

Sint32 GridInitSub(char *path, map_info *pInfo) {
    Uint16 *lpReadWk;
    Uint32 hf;
    char fn[80];
    char buf[80];
    Uint16 *wp1;
    Uint16 tileno;
    Sint32 x, y;
    tile_location loc;
    Sint32 grid;

    lpReadWk = sMemAlloc(4096);

    sPrintf(fn, "%s%s", path, pInfo->fn);

    if ((hf = sOpenFile(fn)) == -1) {
        sPrintf(buf, "Read Error %s\n", fn);
        sOutputDebugString(buf);
        sMemFree(lpReadWk);
        return -1;
    }
    sReadFile(hf, lpReadWk, pInfo->xs * pInfo->ys * 2);
    sCloseFile(hf);

    wp1 = lpReadWk;
    grid = 1;
    for (y = 0; y < pInfo->ys; ++y) {
        if (pInfo->grid == 3) {
            if (y < 3 || y > 23)
                grid = 1;
            else
                grid = 3;
        }
        loc.nTile = y + pInfo->y;
        for (x = 0; x < pInfo->xs; ++x) {
            loc.mTile = x + pInfo->x;
            tileno = *wp1 & 2047;
            if (tileno) {
                tileno += pInfo->ofs;
                if (tileno >= (Uint16)TileCnt) {
                    sPrintf(buf, "Out of Tile Data x%d y%d %s %0x\n", x, y,
                            pInfo->fn, tileno);
                    sOutputDebugString(buf);
                } else {
                    hmx_grid_set_tile_module(s_ctx->grids[grid], loc.mTile,
                                             loc.nTile,
                                             s_ctx->grd_bitmaps[tileno], 0);
                }
            } else {
                if (pInfo->transp) {
                    hmx_grid_set_tile_module(s_ctx->grids[grid], loc.mTile,
                                             loc.nTile, 0, 0);
                } else {
                    hmx_grid_set_tile_module(s_ctx->grids[grid], loc.mTile,
                                             loc.nTile,
                                             s_ctx->grd_bitmaps[pInfo->ofs], 0);
                }
            }
            ++wp1;
        }
    }
    sMemFree(lpReadWk);
    return 0;
}

Sint32 TAGridInit(void) {
    Sint32 i, cntInfo;
    map_info *pInfo;
    char *Path = "../TITLE/BMP/TA/BG/";

    pInfo = MapInfoTbl->info;
    cntInfo = MapInfoTbl->cnt;

    for (i = 0; i < cntInfo; ++i) {
        if (GridInitSub(Path, pInfo) != 0)
            break;

        ++pInfo;
    }
    return 0;
}

Sint32 BkgBmpCreate_TA(void) { return 0; }

Sint32 BkgBmpDelete_TA(void) { return 0; }

Sint32 BkgCreate_TA(void) {
    hmx_background_set_background_module(s_ctx->background, 16);
    return 0;
}

Sint32 BkgDelete_TA(void) { return 0; }

Sint32 LoadPicBmp(void) {
    Sint32 i;
    Sint32 mode, round;
    bmp_header *hdr;
    Uint8 *p;
    char *lp1;
    void *expbuf;

    expbuf = 0;

    expbuf = ld_load_cmpfile_module(g_loader_module, "../TITLE/BMP/SCMPTA.CM_");
    if (expbuf != 0) {
        hdr = expbuf;
        p = (Uint8 *)expbuf + hdr->dwOff;

        for (mode = 0; mode < 2; ++mode) {
            for (round = 0; round < 8; ++round) {
                if (mode != 1 || round != 0) {
                    lp1 = lpPicBits4[mode][round];
                    for (i = 0; i < 9216; ++i) {
                        *lp1 = *p;
                        ++lp1;
                        ++p;
                    }
                }
            }
        }

        hmx_free_module(g_loader_module, expbuf);
    }
    return 0;
}

Uint32 CreatePic(void) {
    Uint32 mask[4] = {0, 0, 0, 0};
    POINTL pt = {0, 0};
    Sint32 i;
    Sint32 j;

    for (i = 0; i < 2; ++i) {
        for (j = 0; j < 8; ++j) {
            if (i == 1 && j == 0) {
                lpPicBits4[i][j] = lpPicBits4[0][0];
            } else {
                lpPicBits4[i][j] = sMemAlloc(9216);
                if (lpPicBits4[i][j] == 0) {
                    return 0;
                }
            }
        }
    }
    lpPicBits8 = sMemAlloc(18432);
    if (lpPicBits8 == 0) {
        return 0;
    }

    if (LoadPicBmp() != 0)
        return 0;

    for (i = 0; i < 2; ++i) {
        for (j = 0; j < 8; ++j) {
            if (i == 1)
                j == 0;
        }
    }

    for (i = 0; i < 2; ++i) {
        s_ctx->spr_bitmaps[i] =
            hmx_bitmap_create_module(g_loader_module, 128, 72);
        hmx_sprite_set_position_module(s_ctx->sprites[i], ptSprLocPic[i].x,
                                       ptSprLocPic[i].y);
        hmx_sprite_set_bitmap_module(s_ctx->sprites[i], s_ctx->spr_bitmaps[i]);
        s_ctx->spr_level[i] = i + 10;
    }
    return 1;
}

void SetPic(Sint32 mode, Sint32 round, Sint32 zurasi) {
    Sint32 x, y;
    char *lp4;
    char *lp8;
    RECTL rect = {0, 0, 128, 72};
    POINTL pt = {0, 0};
    Uint8 buf[8];
    Uint8 buf2[8];
    Sint32 i;
    Sint32 j;
    Uint8 *cg;

    lp4 = lpPicBits4[mode][round];
    lp8 = lpPicBits8;

    for (y = 0; y < 144; ++y) {
        for (x = 0; x < 16; ++x) {
            for (i = 0; i < 4; ++i) {

                buf[i * 2] = ((Uint8)(*lp4 & 240) >> 4) + 48;
                buf[i * 2 + 1] = ((Uint8)*lp4 & 15) + 48;
                ++lp4;
            }
            for (i = 0; i < 4; ++i) {
                j = i - zurasi;
                if (j < 0)
                    j += 4;
                buf2[i] = buf[j];
            }
            for (i = 4; i < 8; ++i) {
                j = i + zurasi;
                if (j > 7)
                    j -= 4;
                buf2[i] = buf[j];
            }
            for (i = 0; i < 8; ++i) {
                *lp8 = buf2[i];
                ++lp8;
            }
        }
    }
    cg = hmx_bitmap_get_scan0_module(s_ctx->spr_bitmaps[0]);
    for (i = 0; i < 9216; ++i)
        cg[i] = ((Uint8 *)lpPicBits8 + 9216)[i];
    cg = hmx_bitmap_get_scan0_module(s_ctx->spr_bitmaps[1]);
    for (i = 0; i < 9216; ++i)
        cg[i] = ((Uint8 *)lpPicBits8)[i];
}

Uint32 TA_EACreate(void) {
    Sint32 i;
    Sint32 j;

    for (i = 0; i < 768; ++i)
        hTile[i] = 0;
    for (i = 0; i < 2; ++i)
        hSprPic[i] = 0;
    hBkg = 0;
    for (i = 0; i < 8; ++i)
        hGrid[i] = 0;
    for (i = 0; i < 768; ++i)
        hGridBmp[i] = 0;
    hBmpBkg = 0;
    for (i = 0; i < 2; ++i)
        hBmpPic[i] = 0;
    for (i = 0; i < 2; ++i)
        for (j = 0; j < 8; ++j)
            lpPicBits4[i][j] = 0;
    lpPicBits8 = 0;

    for (i = 0; i < 8; ++i) {
        GridRect[i].x = 0;
        GridRect[i].y = 0;
        GridRect[i].dx = TAGridSize[i][0] * 8;
        GridRect[i].dy = TAGridSize[i][1] * 8;
    }

    BkgBmpCreate_TA();
    BkgCreate_TA();
    TAGridBmpCreate();
    TAGridCreate();
    TAGridInit();

    if (gMenu1 == 0) {

        GridRect[1].x = GridRect[0].x = GridRect[2].x = GridRect[3].x = 144;
    }

    for (i = 0; i <= 1; ++i) {
        for (j = 0; j <= 7; ++j) {
            PrintRoundName(i, j, 1, 3);
        }
        PrintTotalTime(i, 1);
    }
    if (gMenu2 != 0) {

        PrintZoneSelect(gMenu1, gMenuRound, gMenuZone);
        PrintMenu2(gMenu1, gNewMenu2, 1);
        if (gMenu1 == 0) {
            GridRect[2].x = 0;
        } else {
            GridRect[2].x = 144;
        }
    }

    CreatePic();
    SetPic(gMenu1, gMenuRound, 0);
    TAPicColorSet(gMenu1, gMenuRound);
    return 1;
}

void DeleteEA(void) {
    Sint32 i, j;

    BkgBmpDelete_TA();
    BkgDelete_TA();

    lpPicBits4[1][0] = 0;
    for (i = 0; i < 2; ++i) {
        for (j = 0; j < 8; ++j) {
            if (lpPicBits4[i][j] != 0) {
                sMemFree(lpPicBits4[i][j]);
            }
        }
    }
    if (lpPicBits8 != 0)
        sMemFree(lpPicBits8);

    for (i = 0; i < 2; ++i) {
        hBmpPic[i];
    }

    for (i = 0; i < 2; ++i) {

        hmx_sprite_set_bitmap_module(s_ctx->sprites[i], 0);
        hmx_bitmap_release_module(g_env_module, s_ctx->spr_bitmaps[i]);
        s_ctx->spr_bitmaps[i] = 0;
        s_ctx->spr_level[i] = 0;
    }
    for (i = 0; i < TileCnt; ++i) {
        hGridBmp[i];

        hTile[i];
    }
    for (i = 0; i < 8; ++i) {
        if (hGrid[i] != 0) {
            hmx_grid_release_module(g_env_module, s_ctx->grids[i]);
            s_ctx->grids[i] = 0;
        }
    }

    for (i = 0; i < 2048; ++i) {
        hmx_bitmap_release_module(g_env_module, s_ctx->grd_bitmaps[i]);
        s_ctx->grd_bitmaps[i] = 0;
    }
    for (i = 0; i < 3; ++i) {
        s_ctx->grids[i] = hmx_grid_create_module(g_env_module, 8, 8, 64, 32);
    }

    s_ctx->grd_bitmap_count = 0;
}

void DrawTA(void) {
    Sint32 i;


    GridRect[1].x = GridRect[3].x;
    GridRect[0].x = GridRect[2].x;
    GridRect[0].y = GridRect[2].y;

    for (i = 0; i < 2; ++i) {
        ptSprLocPic[i].x = 168 - GridRect[3].x;
        ptSprLocPic[i].y = GridRect[3].y + 24;
        if (i == 0)
            ptSprLocPic[i].y += 72;
        hmx_sprite_set_position_module(s_ctx->sprites[i], ptSprLocPic[i].x,
                                       ptSprLocPic[i].y);
    }

    for (i = 0; i < 8; ++i) {
        if (hGrid[i] != 0) {

            if (i == 2)
                hmx_grid_set_view_module(s_ctx->grids[i], -GridRect[i].x,
                                         -GridRect[i].y, GridRect[i].dx,
                                         GridRect[i].dy + 320);
            else
                hmx_grid_set_view_module(s_ctx->grids[i], -GridRect[i].x,
                                         -GridRect[i].y, GridRect[i].dx,
                                         GridRect[i].dy);
        }
    }
    srfDraw();
}

Uint16 GetLFont(char c) {
    if (c == 39) {
        return 527;
    }
    if (c == 32) {
        return 529;
    }
    if (c == 34) {
        return 525;
    }
    if (c >= 48 && c < 58) {
        return (Uint16)(c - 48) * 2 + 453;
    }
    if (c >= 65 && c < 91) {
        return ((Uint16)(c - 65) + 10) * 2 + 453;
    }
    return 0;
}

Uint16 GetSFont(char c) {
    if (c == 39) {
        return 698;
    }
    if (c == 34) {
        return 696;
    }
    if (c == 32) {
        return 700;
    }
    if (c >= 48 && c < 58) {
        return (Uint16)(c - 48) * 2 + 676;
    }
    return 0;
}

void TimeToAsc8(Uint32 Time, char *p) {
    Uint16 min, sec, msec;

    min = Time / 3600;
    if (min > 99) {
        min = 99;
        sec = 99;
        msec = 99;
    } else {
        sec = Time % 3600 / 60;
        msec = Time % 60 * 100 / 60;
    }
    p[0] = min / 10 + 48;
    p[1] = min % 10 + 48;
    p[2] = 39;
    p[3] = sec / 10 + 48;
    p[4] = sec % 10 + 48;
    p[5] = 34;
    p[6] = msec / 10 + 48;
    p[7] = msec % 10 + 48;
}

void TimeToAsc9(Uint32 Time, char *p) {
    Uint16 min, sec, msec;

    min = Time / 3600;
    if (min > 999) {
        min = 999;
        sec = 99;
        msec = 99;
    } else {
        sec = Time % 3600 / 60;
        msec = Time % 60 * 100 / 60;
    }
    p[0] = min / 100 + 48;
    p[1] = min % 100 / 10 + 48;
    p[2] = min % 10 + 48;
    p[3] = 39;
    p[4] = sec / 10 + 48;
    p[5] = sec % 10 + 48;
    p[6] = 34;
    p[7] = msec / 10 + 48;
    p[8] = msec % 10 + 48;
}

void PrintTotalTime(Sint32 mode, Uint32 b) {
    Sint32 i, j;
    char buf[9];
    Uint32 Time = 0;
    tile_location loc;
    Uint16 fontno;

    if (mode == 0) {
        for (i = 0; i < 7; ++i) {
            for (j = 0; j < 3; ++j) {
                Time += lpScoreData->timeattack[i][j][0].time;
            }
        }
    } else {
        for (i = 0; i < 7; ++i) {
            Time += lpScoreData->special[i][0].time;
        }
    }

    TimeToAsc9(Time, buf);

    if (mode == 0) {
        loc.mTile = 46;
    } else {
        loc.mTile = 8;
    }
    for (i = 0; i < 9; ++i) {
        if (b != 0) {
            fontno = GetLFont(buf[i]);
        } else {
            fontno = GetLFont(32);
        }
        loc.nTile = 21;
        hmx_grid_set_tile_module(s_ctx->grids[3], loc.mTile, loc.nTile,
                                 s_ctx->grd_bitmaps[fontno++], 0);
        loc.nTile = 22;
        hmx_grid_set_tile_module(s_ctx->grids[3], loc.mTile, loc.nTile,
                                 s_ctx->grd_bitmaps[fontno], 0);
        ++loc.mTile;
    }
}

void PrintRoundName(Sint32 mode, Sint32 round, Uint32 b, Sint32 grid) {
    Uint16 MapBuf[2][18];
    Sint32 x, y;
    Sint32 i;
    tile_location loc;
    Uint32 Time = 0;
    char buf[8];
    Uint16 fontno;

    if (mode == 0) {
        for (i = 0; i < 3; ++i) {
            Time += lpScoreData->timeattack[round - 1][i][0].time;
        }
    } else {
        Time = lpScoreData->special[round - 1][0].time;
    }
    if (b != 0) {
        for (y = 0; y < 2; ++y) {
            for (x = 0; x < 18; ++x) {
                MapBuf[y][x] = MapRound[mode][round][y][x] + 531;
            }
        }
        if (round == 0)
            goto label1;
        if (mode == 0)
            x = 7;
        else
            x = 3;

        TimeToAsc8(Time, buf);

        for (i = 0; i < 8; ++i) {
            fontno = GetSFont(buf[i]);
            MapBuf[0][x] = fontno++;
            MapBuf[1][x++] = fontno;
        }

    } else {
        for (y = 0; y < 2; ++y) {
            for (x = 0; x < 18; ++x) {
                if (round != 0)
                    MapBuf[y][x] = MapRoundSpace[y][0] + 531;
                else
                    MapBuf[y][x] = MapExitSpace[y][0] + 531;
            }
        }
    }
label1:
    for (y = 0; y < 2; ++y) {
        loc.nTile = round * 2 + y + 3;
        for (x = 0; x < 18; ++x) {
            loc.mTile = (Uint32)x;
            if (mode == 0) {
                loc.mTile += 40;
            }
            if (grid == 2 && b == 0) {
                hmx_grid_set_tile_module(s_ctx->grids[grid], loc.mTile,
                                         loc.nTile, 0, 0);
            } else {
                hmx_grid_set_tile_module(s_ctx->grids[grid], loc.mTile,
                                         loc.nTile,
                                         s_ctx->grd_bitmaps[MapBuf[y][x]], 0);
            }
        }
    }
}

void PrintZoneNo(Sint32 mode, Sint32 zone) {
    tile_location loc;
    Uint16 tileno;

    loc.nTile = 9;
    if (mode == 0)
        loc.mTile = 31;
    else
        loc.mTile = 30;
    tileno = GetLFont(zone + 49);
    tileno = zone * 2 + 446;
    hmx_grid_set_tile_module(s_ctx->grids[2], loc.mTile, loc.nTile,
                             s_ctx->grd_bitmaps[tileno++], 0);
    ++loc.nTile;
    hmx_grid_set_tile_module(s_ctx->grids[2], loc.mTile, loc.nTile,
                             s_ctx->grd_bitmaps[tileno], 0);
}

void PrintZoneSelect(Sint32 mode, Sint32 round, Sint32 zone) {
    Sint32 x, y;
    tile_location loc;
    Uint16 tileno;

    for (y = 0; y < 32; ++y) {
        loc.nTile = (Uint32)y;
        for (x = 0; x < 18; ++x) {
            if (mode == 0) {
                loc.mTile = x + 22;
                if (y >= 5 && y < 9) {
                    tileno = MapRecRound[round - 1][y - 5][x] + 187;
                } else {
                    tileno = MapRec[y][x] + 187;
                }
            } else {
                loc.mTile = x + 18;
                tileno = MapRecSP[y][x] + 187;
            }
            hmx_grid_set_tile_module(s_ctx->grids[2], loc.mTile, loc.nTile,
                                     s_ctx->grd_bitmaps[tileno], 0);
        }
    }
    if (mode == 0)
        PrintZoneNo(mode, zone);

    PrintZoneRecordAll(mode, round, zone);
}

void PrintZoneRecord(Sint32 mode, Sint32 round, Sint32 zone, Sint32 rank) {
    Sint32 x, y;
    Sint32 i;
    tile_location loc;
    Uint32 Time;
    char buf[12];
    Uint16 fontno;
    Uint16 MapBuf[2][12];

    --round;

    if (mode == 0) {
        Time = lpScoreData->timeattack[round][zone][rank].time;
        buf[8] = 32;
        buf[9] = lpScoreData->timeattack[round][zone][rank].name[0];
        buf[10] = lpScoreData->timeattack[round][zone][rank].name[1];
        buf[11] = lpScoreData->timeattack[round][zone][rank].name[2];
    } else {
        Time = lpScoreData->special[round][rank].time;
        buf[8] = 32;
        buf[9] = lpScoreData->special[round][rank].name[0];
        buf[10] = lpScoreData->special[round][rank].name[1];
        buf[11] = lpScoreData->special[round][rank].name[2];
    }

    TimeToAsc8(Time, buf);

    for (i = 0; i < 12; ++i) {
        fontno = GetLFont(buf[i]);
        MapBuf[0][i] = fontno++;
        MapBuf[1][i] = fontno;
    }

    for (y = 0; y < 2; ++y) {
        loc.nTile = y + rank * 3 + 15;
        for (x = 0; x < 12; ++x) {
            loc.mTile = (Uint32)x;
            if (mode == 0)
                loc.mTile = x + 25;
            else
                loc.mTile = x + 24;
            hmx_grid_set_tile_module(s_ctx->grids[2], loc.mTile, loc.nTile,
                                     s_ctx->grd_bitmaps[MapBuf[y][x]], 0);
        }
    }
}

void PrintZoneRecordAll(Sint32 mode, Sint32 round, Sint32 zone) {
    Sint32 i;

    for (i = 0; i < 3; ++i) {
        PrintZoneRecord(mode, round, zone, i);
    }
}

void PrintMenu2(Sint32 mode, Sint32 cur, Uint32 b) {
    Sint32 x;
    Sint32 i;
    Sint32 j;
    Sint32 k;
    tile_location loc;
    Uint16 MapBuf[13];
    Uint16 *pw;

    if (mode == 0)
        pw = MapZoneMenuNormal;
    else
        pw = MapZoneMenuSP;
    for (i = 0; i < 13; ++i)
        MapBuf[i] = *pw++ + 531;

    if (b == 0) {
        if (mode == 0) {
            switch (cur) {
            case 1:
                j = 1, k = 4;
                break;
            case 2:
                j = 6, k = 9;
                break;
            case 3:
            default:
                j = 11, k = 12;
                break;
            }
        } else {
            switch (cur) {
            case 3:
                j = 3, k = 4;
                break;
            case 1:
            default:
                j = 6, k = 9;
                break;
            }
        }
        for (i = j; k >= i; ++i)
            MapBuf[i] = MapZoneMenuSpace + 531;
    }

    loc.nTile = 3;
    for (x = 0; x < 13; ++x) {
        loc.mTile = (Uint32)x;
        if (mode == 0)
            loc.mTile = x + 22;
        else
            loc.mTile = x + 22;
        hmx_grid_set_tile_module(s_ctx->grids[2], loc.mTile, loc.nTile,
                                 s_ctx->grd_bitmaps[MapBuf[x]], 0);
    }
}

void PrintRank(Sint32 mode, Sint32 rank, Uint32 b) {
    Sint32 x, y;
    tile_location loc;

    for (y = 0; y < 2; ++y) {
        loc.nTile = y + rank * 3 + 15;
        for (x = 0; x < 3; ++x) {
            if (mode == 0)
                loc.mTile = x + 22;
            else
                loc.mTile = x + 21;
            if (b != 0) {
                hmx_grid_set_tile_module(s_ctx->grids[0], loc.mTile, loc.nTile,
                                         s_ctx->grd_bitmaps[188], 0);
            } else {
                hmx_grid_set_tile_module(s_ctx->grids[0], loc.mTile, loc.nTile,
                                         0, 0);
            }
        }
    }
}

void PrintRankName(Sint32 mode, Sint32 ranky, Sint32 rankx, char c, Uint32 b) {
    tile_location loc;
    Uint16 fontno;

    fontno = GetLFont(c);

    loc.nTile = ranky * 3 + 15;
    if (mode == 0)
        loc.mTile = rankx + 34;
    else
        loc.mTile = rankx + 33;
    if (b != 0) {
        hmx_grid_set_tile_module(s_ctx->grids[2], loc.mTile, loc.nTile,
                                 s_ctx->grd_bitmaps[fontno++], 0);
        ++loc.nTile;
        hmx_grid_set_tile_module(s_ctx->grids[2], loc.mTile, loc.nTile,
                                 s_ctx->grd_bitmaps[fontno], 0);
    } else {
        hmx_grid_set_tile_module(s_ctx->grids[2], loc.mTile, loc.nTile,
                                 s_ctx->grd_bitmaps[188], 0);
        ++loc.nTile;
        hmx_grid_set_tile_module(s_ctx->grids[2], loc.mTile, loc.nTile,
                                 s_ctx->grd_bitmaps[188], 0);
    }
}

void Tenmetu(void) {
    char c;

    if (gTimer % 30 == 0) {
        if (gMenu2 == 5) {

            if (gMenu1 == 0) {
                c = lpScoreData->timeattack[gMenuRound - 1][gMenuZone][gRankY]
                        .name[gRankX];
            } else {
                c = lpScoreData->special[gMenuRound - 1][gRankY].name[gRankX];
            }
            PrintRankName(gMenu1, gRankY, gRankX, c, 0);
        } else if (gMenu2 == 4) {

            PrintRank(gMenu1, gRankY, 0);
        } else if (gMenu2 == 0) {

            PrintRoundName(gMenu1, gMenuRound, 0, 3);
        } else {
            PrintMenu2(gMenu1, gMenu2, 0);
        }
    } else if (gTimer % 30 == 10) {
        if (gMenu2 == 5) {

            if (gMenu1 == 0) {
                c = lpScoreData->timeattack[gMenuRound - 1][gMenuZone][gRankY]
                        .name[gRankX];
            } else {
                c = lpScoreData->special[gMenuRound - 1][gRankY].name[gRankX];
            }
            PrintRankName(gMenu1, gRankY, gRankX, c, 1);
        } else if (gMenu2 == 4) {

            PrintRank(gMenu1, gRankY, 1);
        } else if (gMenu2 == 0) {

            PrintRoundName(gMenu1, gMenuRound, 1, 3);
        } else {
            PrintMenu2(gMenu1, gMenu2, 1);
        }
    }
}

Sint32 TAMove(void) {
    char c;

    switch (gMove & -32769) {

    case 6:
        if (!(gMove & 32768)) {
            gMove |= 32768;
            PrintRoundName(gMenu1, gMenuRound, 1, 3);

            TAPicColorSet(gNewMenu1, gMenuRound);
            gZure = 7;
            SetPic(gNewMenu1, gMenuRound, gZure / 2);
        } else {
            if (gZure != 0) {
                --gZure;
                SetPic(gNewMenu1, gMenuRound, gZure / 2);
            }
        }
        GridRect[3].x += 8;
        GridRect[2].x = GridRect[3].x;
        if (GridRect[3].x == 144) {
            gMenu1 = gNewMenu1;
            gMove = 0;
        }
        break;

    case 5:
        if (!(gMove & 32768)) {
            gMove |= 32768;
            PrintRoundName(gMenu1, gMenuRound, 1, 3);

            TAPicColorSet(gNewMenu1, gMenuRound);
            gZure = 7;
            SetPic(gNewMenu1, gMenuRound, gZure / 2);
        } else {
            if (gZure != 0) {
                --gZure;
                SetPic(gNewMenu1, gMenuRound, gZure / 2);
            }
        }
        GridRect[3].x -= 8;
        GridRect[2].x = GridRect[3].x;
        if (GridRect[3].x == 0) {
            gMenu1 = gNewMenu1;
            gMove = 0;
        }
        break;

    case 7:
        if (!(gMove & 32768)) {
            gMove |= 32768;
            gZure = 7;
            TAPicColorSet(gMenu1, gNewMenuRound);
            PrintRoundName(gMenu1, gMenuRound, 1, 3);
            gMenuRound = gNewMenuRound;
        } else {
            --gZure;
        }
        SetPic(gMenu1, gNewMenuRound, gZure / 2);
        if (gZure == 0) {
            gMove = 0;
        }
        break;

    case 1:
        if (!(gMove & 32768)) {
            gMove |= 32768;

            PrintRoundName(gMenu1, gMenuRound, 1, 2);
            PrintZoneSelect(gMenu1, gMenuRound, gNewMenuZone);
            PrintMenu2(gMenu1, gNewMenu2, 1);
        }
        GridRect[2].x -= 8;
        if (GridRect[2].x < 0)
            GridRect[2].x += 512;
        if (GridRect[2].x == 0) {
            gMenu2 = gNewMenu2;
            gMove = 0;
        }
        break;

    case 2:
        if (!(gMove & 32768)) {
            gMove |= 32768;

            PrintRoundName(gMenu1, gMenuRound, 1, 2);
            PrintZoneSelect(gMenu1, gMenuRound, gNewMenuZone);
            PrintMenu2(gMenu1, gNewMenu2, 1);
        }
        GridRect[2].x += 8;
        if (GridRect[2].x == 144) {
            gMenu2 = gNewMenu2;
            gMove = 0;
        }
        break;

    case 3:
        GridRect[2].x += 8;
        if (GridRect[2].x == 144) {

            PrintRoundName(gMenu1, gMenuRound, 0, 2);
            gMenu2 = gNewMenu2;
            gMove = 0;
        }
        break;

    case 4:
        GridRect[2].x -= 8;
        if (GridRect[2].x == 0) {

            PrintRoundName(gMenu1, gMenuRound, 0, 2);
            gMenu2 = gNewMenu2;
            gMove = 0;
        }
        break;

    case 8:
        GridRect[2].y -= 8;
        if (GridRect[2].y < 0)
            GridRect[2].y += 256;
        if (GridRect[2].y == 128) {
            PrintZoneNo(gMenu1, gNewMenuZone);
        } else if (GridRect[2].y == 184) {
            PrintZoneRecord(gMenu1, gMenuRound, gNewMenuZone, 0);
        } else if (GridRect[2].y == 208) {
            PrintZoneRecord(gMenu1, gMenuRound, gNewMenuZone, 1);
        } else if (GridRect[2].y == 232) {
            PrintZoneRecord(gMenu1, gMenuRound, gNewMenuZone, 2);
        } else if (GridRect[2].y == 0) {
            gMenuZone = gNewMenuZone;
            gMove = 0;
        }
        break;

    case 9:
        GridRect[2].y += 8;
        if (GridRect[2].y >= 256)
            GridRect[2].y -= 256;
        if (GridRect[2].y == 64) {
            PrintZoneNo(gMenu1, gNewMenuZone);
        } else if (GridRect[2].y == 128) {
            PrintZoneRecord(gMenu1, gMenuRound, gNewMenuZone, 0);
        } else if (GridRect[2].y == 152) {
            PrintZoneRecord(gMenu1, gMenuRound, gNewMenuZone, 1);
        } else if (GridRect[2].y == 176) {
            PrintZoneRecord(gMenu1, gMenuRound, gNewMenuZone, 2);
        } else if (GridRect[2].y == 0) {
            gMenuZone = gNewMenuZone;
            gMove = 0;
        }
        break;
    case 10:
        PrintMenu2(gMenu1, gMenu2, 1);
        PrintMenu2(gMenu1, gNewMenu2, 0);
        gMenu2 = gNewMenu2;
        gMove = 0;
        break;
    case 11:
        PrintRank(gMenu1, gRankY, 0);
        gMenu2 = gNewMenu2;
        gMove = 0;
        break;
    case 12:
        if (gMenu1 == 0) {
            c = lpScoreData->timeattack[gMenuRound - 1][gMenuZone][gRankY]
                    .name[gRankX];
        } else {
            c = lpScoreData->special[gMenuRound - 1][gRankY].name[gRankX];
        }
        if (gNewMenu2 == 3) {

            gMenu2 = gNewMenu2;
            PrintRankName(gMenu1, gRankY, gRankX, c, 1);
        } else {
            if (gNewRankX == gRankX) {

                PrintRankName(gMenu1, gRankY, gRankX, c, 1);
            } else {

                PrintRankName(gMenu1, gRankY, gRankX, c, 1);
                gRankX = gNewRankX;
            }
        }
        gMove = 0;
        break;
    default:
        gMove = 0;
        break;
    }
    return -1;
}
