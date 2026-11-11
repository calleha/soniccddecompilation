#include "../types.h"
#include "common.h"
#include "spm_equ.h"
#include "sps_equ.h"
#include "game.h"
#include "act_s.h"
#include "bact.h"
#include "coli_s.h"
#include "ens.h"
#include "etc_m.h"
#include "etc_s.h"
#include "fadein.h"
#include "kaiten.h"

Sint32 time_bonus = 0;
Sint32 ring_bonus = 0;
Uint8 seregiflg = 0;
Uint8 scroll_stop = 0;
static Uint16 hscrcnt = 256;
static Uint32 PauseIcon = 0;
static Sint32 scoreup_setm = 0;
int_union scrb_v_posiw = {0};
int_union scrb_h_posiw = {0};
int_union scra_v_posiw = {0};
int_union scra_h_posiw = {0};
int_union hscroll = {0};
int_union vscroll = {0};
Sint16 plring_dmy = 0;
Sint32 *lpFadeFlag = 0;
game_info *lpKeepWork = 0;
extern bmp_info SprBmp[700];
Uint16 *pmapwk = 0;
PALETTEENTRY *lpcolorwk4 = 0;
PALETTEENTRY *lpcolorwk3 = 0;
PALETTEENTRY *lpcolorwk2 = 0;
PALETTEENTRY *lpcolorwk = 0;
int_union *lphscrollbuff = 0;
ushort_union swdata2 = {0};
static Uint8 hscrflg = 0;
static Uint16 hscrcnt4 = 0;
static Uint16 clchgcnt[4] = {0};
void (*sCloseFile)(Sint32) = 0;
Sint32 (*sGetFileSize)(Sint32) = 0;
Sint32 (*sReadFile)(Sint32, void *, Sint32) = 0;
Sint32 (*sOpenFile)(char *) = 0;
void (*sMemCpy)(void *, void *, Sint32) = 0;
void (*sMemSet)(void *, Uint8, Sint32) = 0;
Sint32 (*sRandom)(void) = 0;
void (*sPrintf)(char *, const char *, ...) = 0;
void (*sOutputDebugString)(char *) = 0;
void (*WaveAllStop)(void) = 0;
void (*CDPause)(Sint16) = 0;
void (*CDPlay)(Sint16) = 0;
void (*WaveRequest)(Sint16) = 0;
void (*ClrSpriteDebug)(void) = 0;
void (*EAsprset)(Sint16, Sint16, Uint16, Uint16, Uint16) = 0;
Sint32 (*SetGrid)(Sint32, Sint32, Sint32, Sint32, Sint32) = 0;

void SetUseOk(Sint16 unknown1, Sint16 unknown2, Sint16 unknown3) {}

static PALETTEENTRY colortbl[64] = {
    {64, 96, 224, 1},   {0, 0, 0, 1},       {0, 32, 128, 1},
    {32, 64, 192, 1},   {32, 96, 224, 1},   {64, 160, 224, 1},
    {224, 224, 224, 1}, {160, 160, 160, 1}, {128, 128, 128, 1},
    {64, 64, 64, 1},    {224, 160, 128, 1}, {160, 96, 64, 1},
    {224, 0, 0, 1},     {128, 0, 0, 1},     {64, 0, 0, 1},
    {224, 224, 0, 1},   {0, 0, 0, 1},       {224, 224, 224, 1},
    {128, 128, 128, 1}, {64, 64, 64, 1},    {0, 0, 0, 1},
    {224, 224, 0, 1},   {192, 32, 0, 1},    {224, 192, 128, 1},
    {192, 160, 96, 1},  {160, 224, 0, 1},   {96, 160, 0, 1},
    {32, 96, 0, 1},     {192, 224, 224, 1}, {128, 192, 224, 1},
    {64, 160, 224, 1},  {64, 96, 224, 1},   {0, 0, 0, 1},
    {0, 0, 0, 1},       {0, 0, 32, 1},      {0, 0, 64, 1},
    {0, 0, 96, 1},      {0, 0, 160, 1},     {0, 0, 224, 1},
    {0, 64, 224, 1},    {0, 96, 224, 1},    {0, 128, 224, 1},
    {32, 160, 224, 1},  {64, 192, 224, 1},  {128, 224, 224, 1},
    {192, 224, 224, 1}, {224, 224, 224, 1}, {0, 0, 0, 1},
    {0, 0, 0, 1},       {0, 0, 0, 1},       {64, 32, 128, 1},
    {96, 64, 192, 1},   {128, 96, 224, 1},  {160, 128, 224, 1},
    {224, 224, 224, 1}, {128, 192, 192, 1}, {96, 160, 128, 1},
    {32, 64, 96, 1},    {0, 0, 0, 1},       {224, 224, 224, 1},
    {224, 224, 0, 1},   {128, 128, 0, 1},   {64, 64, 0, 1},
    {0, 0, 0, 1},
};
static PALETTEENTRY dispcolor[64] = {
    {224, 224, 224, 1}, {0, 0, 0, 1},       {224, 224, 224, 1},
    {128, 224, 0, 1},   {64, 160, 0, 1},    {32, 96, 0, 1},
    {224, 0, 0, 1},     {128, 0, 0, 1},     {0, 0, 0, 1},
    {0, 0, 0, 1},       {64, 0, 32, 1},     {224, 224, 32, 1},
    {128, 128, 0, 1},   {64, 32, 0, 1},     {0, 0, 0, 1},
    {0, 0, 0, 1},       {224, 224, 224, 1}, {0, 0, 0, 1},
    {224, 224, 224, 1}, {96, 96, 224, 1},   {64, 64, 224, 1},
    {32, 32, 192, 1},   {64, 224, 128, 1},  {0, 160, 160, 1},
    {0, 64, 96, 1},     {224, 32, 224, 1},  {160, 0, 224, 1},
    {128, 0, 160, 1},   {224, 96, 0, 1},    {224, 64, 0, 1},
    {128, 0, 0, 1},     {0, 0, 0, 1},       {224, 224, 224, 1},
    {160, 224, 224, 1}, {0, 224, 160, 1},   {0, 128, 192, 1},
    {0, 0, 224, 1},     {192, 96, 224, 1},  {32, 0, 128, 1},
    {0, 0, 0, 1},       {0, 0, 0, 1},       {0, 0, 0, 1},
    {0, 0, 0, 1},       {0, 0, 0, 1},       {0, 0, 0, 1},
    {0, 0, 0, 1},       {0, 0, 0, 1},       {0, 0, 0, 1},
    {224, 224, 224, 1}, {0, 0, 0, 1},       {32, 32, 192, 1},
    {96, 96, 224, 1},   {128, 128, 224, 1}, {224, 224, 224, 1},
    {128, 128, 128, 1}, {64, 64, 64, 1},    {224, 160, 128, 1},
    {160, 96, 64, 1},   {224, 0, 0, 1},     {128, 0, 0, 1},
    {192, 224, 224, 1}, {160, 224, 224, 1}, {128, 224, 224, 1},
    {64, 224, 224, 1},
};
static PALETTEENTRY coltbl0[32] = {
    {0, 0, 0, 1},     {224, 224, 224, 1}, {224, 224, 0, 1},  {224, 128, 0, 1},
    {224, 32, 0, 1},  {160, 160, 128, 1}, {128, 128, 64, 1}, {64, 64, 32, 1},
    {0, 0, 0, 1},     {0, 0, 224, 1},     {96, 192, 224, 1}, {64, 128, 224, 1},
    {32, 96, 224, 1}, {64, 128, 32, 1},   {32, 96, 0, 1},    {0, 64, 0, 1},
    {0, 0, 0, 1},     {0, 0, 32, 1},      {0, 0, 64, 1},     {0, 0, 96, 1},
    {0, 0, 128, 1},   {0, 0, 160, 1},     {0, 0, 192, 1},    {0, 0, 224, 1},
    {0, 0, 0, 1},     {0, 0, 0, 1},       {0, 0, 0, 1},      {0, 0, 0, 1},
    {0, 0, 0, 1},     {0, 0, 0, 1},       {0, 0, 0, 1},      {0, 0, 0, 1},
};
static PALETTEENTRY coltbl1[32] = {
    {0, 0, 224, 1},     {224, 224, 224, 1}, {224, 224, 0, 1},
    {224, 128, 0, 1},   {224, 32, 0, 1},    {160, 160, 160, 1},
    {96, 96, 96, 1},    {64, 64, 64, 1},    {0, 0, 0, 1},
    {0, 0, 96, 1},      {0, 0, 224, 1},     {0, 96, 224, 1},
    {96, 160, 224, 1},  {160, 192, 224, 1}, {0, 224, 0, 1},
    {240, 240, 240, 1}, {64, 96, 224, 1},   {224, 224, 224, 1},
    {192, 224, 224, 1}, {160, 192, 224, 1}, {128, 160, 224, 1},
    {96, 128, 224, 1},  {0, 32, 224, 1},    {128, 128, 160, 1},
    {0, 0, 224, 1},     {160, 96, 64, 1},   {192, 128, 96, 1},
    {224, 192, 128, 1}, {224, 224, 160, 1}, {160, 64, 0, 1},
    {96, 64, 0, 1},     {0, 0, 0, 1},
};
static PALETTEENTRY coltbl2[32] = {
    {0, 0, 224, 1},     {224, 224, 224, 1}, {224, 224, 0, 1},
    {224, 128, 0, 1},   {224, 32, 0, 1},    {128, 160, 160, 1},
    {64, 96, 96, 1},    {32, 64, 64, 1},    {0, 0, 0, 1},
    {32, 128, 0, 1},    {0, 0, 224, 1},     {32, 64, 224, 1},
    {64, 128, 224, 1},  {64, 192, 224, 1},  {64, 224, 0, 1},
    {0, 64, 0, 1},      {0, 0, 224, 1},     {224, 224, 224, 1},
    {0, 32, 224, 1},    {0, 64, 224, 1},    {32, 96, 224, 1},
    {64, 128, 224, 1},  {0, 0, 160, 1},     {0, 0, 128, 1},
    {192, 224, 224, 1}, {0, 0, 32, 1},      {32, 224, 224, 1},
    {96, 160, 128, 1},  {64, 128, 96, 1},   {0, 64, 64, 1},
    {224, 32, 128, 1},  {224, 224, 64, 1},
};
static PALETTEENTRY coltbl3[32] = {
    {0, 32, 224, 1},    {224, 224, 224, 1}, {224, 224, 0, 1},
    {224, 128, 0, 1},   {224, 32, 0, 1},    {224, 192, 160, 1},
    {160, 128, 96, 1},  {128, 96, 32, 1},   {0, 0, 0, 1},
    {32, 128, 0, 1},    {0, 32, 224, 1},    {32, 64, 224, 1},
    {64, 128, 224, 1},  {64, 192, 224, 1},  {64, 224, 0, 1},
    {0, 64, 0, 1},      {0, 32, 224, 1},    {0, 0, 0, 1},
    {224, 224, 224, 1}, {224, 224, 128, 1}, {224, 160, 64, 1},
    {192, 128, 64, 1},  {128, 96, 64, 1},   {64, 32, 64, 1},
    {96, 192, 224, 1},  {64, 192, 192, 1},  {96, 96, 96, 1},
    {0, 32, 0, 1},      {32, 64, 0, 1},     {64, 128, 0, 1},
    {96, 160, 32, 1},   {160, 160, 160, 1},
};
static PALETTEENTRY coltbl4[32] = {
    {0, 0, 0, 1},     {224, 224, 224, 1}, {224, 224, 0, 1},   {224, 128, 0, 1},
    {224, 32, 32, 1}, {128, 128, 128, 1}, {96, 96, 96, 1},    {64, 64, 64, 1},
    {0, 0, 0, 1},     {0, 224, 0, 1},     {0, 32, 224, 1},    {32, 0, 0, 1},
    {96, 0, 0, 1},    {160, 0, 0, 1},     {160, 96, 0, 1},    {96, 32, 0, 1},
    {0, 0, 0, 1},     {0, 0, 0, 1},       {224, 224, 224, 1}, {224, 224, 0, 1},
    {128, 192, 0, 1}, {64, 160, 0, 1},    {32, 96, 0, 1},     {0, 64, 0, 1},
    {0, 0, 64, 1},    {0, 0, 128, 1},     {0, 32, 192, 1},    {0, 64, 224, 1},
    {0, 160, 224, 1}, {224, 224, 0, 1},   {0, 224, 224, 1},   {224, 0, 0, 1},
};
static PALETTEENTRY coltbl5[32] = {
    {96, 0, 64, 1},     {224, 224, 224, 1}, {224, 224, 0, 1},
    {224, 128, 0, 1},   {224, 32, 0, 1},    {128, 128, 160, 1},
    {96, 96, 128, 1},   {64, 64, 96, 1},    {0, 0, 0, 1},
    {32, 64, 0, 1},     {64, 128, 0, 1},    {128, 160, 0, 1},
    {64, 224, 0, 1},    {0, 0, 224, 1},     {64, 0, 32, 1},
    {160, 160, 192, 1}, {96, 0, 64, 1},     {224, 64, 0, 1},
    {224, 128, 0, 1},   {224, 192, 0, 1},   {160, 224, 64, 1},
    {128, 224, 128, 1}, {64, 224, 192, 1},  {64, 160, 224, 1},
    {128, 96, 224, 1},  {160, 0, 224, 1},   {224, 32, 224, 1},
    {224, 0, 160, 1},   {224, 0, 96, 1},    {224, 0, 32, 1},
    {192, 64, 128, 1},  {224, 160, 224, 1},
};
static PALETTEENTRY coltbl6[32] = {
    {0, 64, 0, 1},      {224, 224, 224, 1}, {224, 224, 0, 1},  {224, 128, 0, 1},
    {224, 32, 0, 1},    {96, 128, 128, 1},  {64, 96, 96, 1},   {32, 64, 64, 1},
    {0, 0, 0, 1},       {160, 224, 0, 1},   {96, 192, 0, 1},   {32, 128, 0, 1},
    {0, 96, 0, 1},      {128, 160, 160, 1}, {0, 64, 32, 1},    {0, 32, 192, 1},
    {0, 64, 0, 1},      {0, 0, 0, 1},       {32, 32, 32, 1},   {0, 192, 224, 1},
    {224, 224, 224, 1}, {224, 128, 224, 1}, {224, 64, 160, 1}, {160, 224, 0, 1},
    {224, 0, 64, 1},    {224, 192, 96, 1},  {32, 64, 0, 1},    {64, 128, 0, 1},
    {128, 160, 0, 1},   {224, 160, 32, 1},  {192, 128, 0, 1},  {128, 64, 0, 1},
};
static PALETTEENTRY coltbl7[32] = {
    {0, 0, 0, 1},       {224, 224, 224, 1}, {224, 224, 0, 1},
    {224, 128, 0, 1},   {224, 32, 0, 1},    {160, 160, 128, 1},
    {128, 128, 64, 1},  {64, 64, 32, 1},    {0, 0, 0, 1},
    {0, 0, 224, 1},     {96, 192, 224, 1},  {64, 128, 224, 1},
    {32, 96, 224, 1},   {64, 128, 32, 1},   {32, 96, 0, 1},
    {0, 64, 0, 1},      {0, 0, 0, 1},       {0, 0, 0, 1},
    {224, 224, 224, 1}, {224, 224, 160, 1}, {224, 192, 128, 1},
    {224, 160, 128, 1}, {192, 128, 128, 1}, {224, 96, 160, 1},
    {0, 0, 192, 1},     {224, 224, 0, 1},   {96, 96, 96, 1},
    {64, 32, 0, 1},     {96, 32, 0, 1},     {128, 64, 32, 1},
    {64, 96, 224, 1},   {0, 224, 224, 1},
};
static Uint8 cltbl[1008] = {
    0,   32,  64,  96,  128, 160, 192, 224, 0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   32,  64,  96,  128, 160, 192, 0,   0,   0,   0,   0,   0,
    0,   32,  0,   0,   0,   32,  64,  96,  128, 160, 0,   0,   0,   0,   0,
    0,   32,  64,  0,   0,   0,   0,   32,  64,  96,  128, 0,   0,   0,   0,
    0,   32,  64,  96,  0,   0,   0,   0,   0,   32,  64,  96,  0,   0,   0,
    0,   32,  64,  96,  128, 0,   0,   0,   0,   0,   0,   32,  64,  0,   0,
    0,   32,  64,  96,  128, 160, 0,   0,   0,   0,   0,   0,   0,   32,  0,
    0,   32,  64,  96,  128, 160, 192, 0,   0,   0,   0,   0,   0,   0,   0,
    0,   32,  64,  96,  128, 160, 192, 224, 0,   0,   0,   0,   0,   0,   0,
    0,   32,  64,  96,  128, 160, 192, 224, 192, 0,   32,  0,   0,   0,   0,
    0,   0,   64,  96,  128, 160, 192, 224, 192, 160, 0,   64,  32,  0,   0,
    0,   0,   0,   96,  128, 160, 192, 224, 192, 160, 128, 0,   96,  64,  32,
    0,   0,   0,   0,   128, 160, 192, 224, 192, 160, 128, 96,  0,   128, 96,
    64,  32,  0,   0,   0,   160, 192, 224, 192, 160, 128, 96,  64,  0,   160,
    128, 96,  64,  32,  0,   0,   192, 224, 192, 160, 128, 96,  64,  32,  0,
    192, 160, 128, 96,  64,  32,  0,   224, 192, 160, 128, 96,  64,  32,  0,
    0,   224, 192, 160, 128, 96,  64,  32,  192, 160, 128, 96,  64,  32,  0,
    0,   0,   192, 224, 192, 160, 128, 96,  64,  160, 128, 96,  64,  32,  0,
    0,   0,   0,   160, 192, 224, 192, 160, 128, 96,  128, 96,  64,  32,  0,
    0,   0,   0,   0,   128, 160, 192, 224, 192, 160, 128, 96,  64,  32,  0,
    0,   0,   0,   0,   0,   96,  128, 160, 192, 224, 192, 160, 64,  32,  0,
    0,   0,   0,   0,   0,   0,   64,  96,  128, 160, 192, 224, 192, 32,  0,
    0,   0,   0,   0,   0,   0,   0,   32,  64,  96,  128, 160, 192, 224, 0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   32,  64,  96,  128, 160, 192,
    0,   0,   0,   0,   0,   0,   0,   2,   0,   0,   0,   32,  64,  96,  128,
    160, 0,   0,   0,   0,   0,   0,   2,   4,   0,   0,   0,   0,   32,  64,
    96,  128, 0,   0,   0,   0,   0,   2,   4,   6,   0,   0,   0,   0,   0,
    32,  64,  96,  0,   0,   0,   0,   2,   4,   6,   8,   0,   0,   0,   0,
    0,   0,   32,  64,  0,   0,   0,   2,   4,   6,   8,   10,  0,   0,   0,
    0,   0,   0,   0,   32,  0,   0,   2,   4,   6,   8,   10,  12,  0,   0,
    0,   0,   0,   0,   0,   0,   0,   2,   4,   6,   8,   10,  12,  14,  0,
    0,   0,   0,   0,   0,   0,   0,   2,   4,   6,   8,   10,  12,  14,  12,
    0,   2,   0,   0,   0,   0,   0,   0,   4,   6,   8,   10,  12,  14,  12,
    10,  0,   4,   2,   0,   0,   0,   0,   0,   6,   8,   10,  12,  14,  12,
    10,  8,   0,   6,   4,   2,   0,   0,   0,   0,   8,   10,  12,  14,  12,
    10,  8,   6,   0,   8,   6,   4,   2,   0,   0,   0,   10,  12,  14,  12,
    10,  8,   6,   4,   0,   10,  8,   6,   4,   2,   0,   0,   12,  14,  12,
    10,  8,   6,   4,   2,   0,   12,  10,  8,   6,   4,   2,   0,   14,  12,
    10,  8,   6,   4,   2,   0,   0,   14,  12,  10,  8,   6,   4,   2,   12,
    10,  8,   6,   4,   2,   0,   0,   0,   12,  14,  12,  10,  8,   6,   4,
    10,  8,   6,   4,   2,   0,   0,   0,   0,   10,  12,  14,  12,  10,  8,
    6,   8,   6,   4,   2,   0,   0,   0,   0,   0,   8,   10,  12,  14,  12,
    10,  8,   6,   4,   2,   0,   0,   0,   0,   0,   0,   6,   8,   10,  12,
    14,  12,  10,  4,   2,   0,   0,   0,   0,   0,   0,   0,   4,   6,   8,
    10,  12,  14,  12,  2,   0,   0,   0,   0,   0,   0,   0,   0,   2,   4,
    6,   8,   10,  12,  14,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    2,   4,   6,   8,   10,  12,  0,   0,   0,   0,   0,   0,   0,   32,  0,
    0,   0,   2,   4,   6,   8,   10,  0,   0,   0,   0,   0,   0,   32,  64,
    0,   0,   0,   0,   2,   4,   6,   8,   0,   0,   0,   0,   0,   32,  64,
    96,  0,   0,   0,   0,   0,   2,   4,   6,   0,   0,   0,   0,   32,  64,
    96,  128, 0,   0,   0,   0,   0,   0,   2,   4,   0,   0,   0,   32,  64,
    96,  128, 160, 0,   0,   0,   0,   0,   0,   0,   2,   0,   0,   32,  64,
    96,  128, 160, 192, 0,   0,   0,   0,   0,   0,   0,   0,   0,   32,  64,
    96,  128, 160, 192, 224, 0,   0,   0,   0,   0,   0,   0,   0,   32,  64,
    96,  128, 160, 192, 224, 192, 0,   32,  0,   0,   0,   0,   0,   0,   64,
    96,  128, 160, 192, 224, 192, 160, 0,   64,  32,  0,   0,   0,   0,   0,
    96,  128, 160, 192, 224, 192, 160, 128, 0,   96,  64,  32,  0,   0,   0,
    0,   128, 160, 192, 224, 192, 160, 128, 96,  0,   128, 96,  64,  32,  0,
    0,   0,   160, 192, 224, 192, 160, 128, 96,  64,  0,   160, 128, 96,  64,
    32,  0,   0,   192, 224, 192, 160, 128, 96,  64,  32,  0,   192, 160, 128,
    96,  64,  32,  0,   224, 192, 160, 128, 96,  64,  32,  0,   0,   224, 192,
    160, 128, 96,  64,  32,  192, 160, 128, 96,  64,  32,  0,   0,   0,   192,
    224, 192, 160, 128, 96,  64,  160, 128, 96,  64,  32,  0,   0,   0,   0,
    160, 192, 224, 192, 160, 128, 96,  128, 96,  64,  32,  0,   0,   0,   0,
    0,   128, 160, 192, 224, 192, 160, 128, 96,  64,  32,  0,   0,   0,   0,
    0,   0,   96,  128, 160, 192, 224, 192, 160, 64,  32,  0,   0,   0,   0,
    0,   0,   0,   64,  96,  128, 160, 192, 224, 192, 32,  0,   0,   0,   0,
    0,   0,   0,
};
static Sint32 hdatatbl4[111] = {
    249856, 253952, 258048, 262144, 262144, 266240, 266240, 270336, 274432,
    274432, 278528, 282624, 282624, 286720, 286720, 290816, 290816, 294912,
    299008, 299008, 303104, 303104, 307200, 307200, 311296, 311296, 315392,
    315392, 319488, 319488, 323584, 323584, 323584, 327680, 327680, 331776,
    331776, 335872, 335872, 335872, 339968, 339968, 344064, 344064, 344064,
    348160, 348160, 348160, 352256, 352256, 352256, 356352, 356352, 356352,
    360448, 360448, 360448, 364544, 364544, 364544, 364544, 368640, 368640,
    368640, 368640, 372736, 372736, 372736, 372736, 376832, 376832, 376832,
    376832, 376832, 380928, 380928, 380928, 380928, 380928, 380928, 385024,
    385024, 385024, 385024, 385024, 385024, 385024, 389120, 389120, 389120,
    389120, 389120, 389120, 389120, 389120, 389120, 389120, 393216, 393216,
    393216, 393216, 393216, 393216, 393216, 393216, 393216, 393216, 393216,
    393216, 393216, 393216};
static char hdatatbl5[128] = {
    0,   -5,  -10, -15, -20, -25, -30, -35, -40, -44, -48, -52, -56, -60, -64,
    -67, -70, -73, -76, -78, -80, -82, -84, -86, -88, -90, -92, -93, -94, -93,
    -92, -90, -88, -85, -82, -80, -78, -71, -64, -56, -48, -32, -16, 0,   16,
    21,  26,  30,  34,  35,  36,  37,  38,  37,  36,  34,  32,  28,  24,  20,
    16,  10,  5,   0,   -4,  -9,  -13, -18, -22, -24, -26, -28, -30, -31, -32,
    -33, -34, -33, -32, -31, -30, -28, -27, -25, -24, -20, -16, -12, -8,  0,
    8,   15,  22,  30,  38,  47,  56,  64,  72,  80,  88,  94,  100, 106, 112,
    115, 118, 120, 122, 123, 124, 125, 126, 125, 124, 122, 120, 114, 108, 102,
    96,  86,  76,  66,  56,  42,  28,  14};
static char map_filename[] = "../SPECIAL/MAP/SP%1dMAP.MAP";
static Uint16 mp01[4] = {1, 2, 3, 4};
static Uint16 mp02[4] = {5, 6, 7, 8};
static Uint16 mp03[4] = {9, 10, 11, 12};
static Uint16 mp04[4] = {8201, 8202, 8203, 8204};
static Uint16 mp05[4] = {8205, 8206, 8207, 8208};
static Uint16 mp06[4] = {8209, 8210, 8211, 8212};
static Uint16 mp07[4] = {8213, 8214, 8215, 8216};
static Uint16 mp08[10] = {16409, 16410, 16411, 16412, 16413,
                          16414, 16414, 16414, 16415, 16416};
static Uint16 mp09[8] = {16423, 16424, 16425, 16426,
                         16415, 16427, 16428, 16414};
static Uint16 mp10[8] = {16417, 16418, 16419, 16413,
                         16420, 16421, 16422, 16416};
static Uint16 mp11[12] = {16423, 16411, 16525, 16526, 16527, 16528,
                          16529, 16414, 16530, 16531, 16532, 16533};
static Uint16 mp14[30] = {
    16413, 16534, 16535, 16536, 16537, 16538, 16539, 16540, 16541, 16542,
    16543, 16544, 16545, 16546, 16547, 16416, 16548, 16549, 16550, 16551,
    16552, 16553, 16554, 16555, 16556, 16557, 16558, 16556, 16559, 16553};
static Uint16 mp21[46] = {
    16444, 16445, 16446, 16447, 16448, 16449, 16450, 16451, 16452, 16453,
    16454, 16455, 16384, 16384, 16384, 16456, 16457, 16458, 16459, 16460,
    16461, 16462, 16463, 16464, 16465, 16466, 16467, 16468, 16469, 16470,
    16471, 16472, 16473, 16474, 16475, 16476, 16384, 16384, 16477, 16478,
    16479, 16480, 16481, 16482, 16483, 16484};
static Uint16 mp22[42] = {
    16485, 16486, 16487, 16488, 16489, 16462, 16463, 16384, 16384, 16444, 16445,
    16490, 16491, 16492, 16493, 16494, 16495, 16448, 16449, 16444, 16445, 16496,
    16497, 16498, 16499, 16500, 16483, 16484, 16384, 16384, 16464, 16465, 16501,
    16502, 16503, 16504, 16505, 16506, 16468, 16469, 16464, 16465};
static Uint16 mp23[52] = {
    16507, 16508, 16458, 16459, 16490, 16509, 16384, 16384, 16490, 16509, 16510,
    16511, 16448, 16449, 16512, 16513, 16384, 16384, 16453, 16454, 16455, 16384,
    16514, 16384, 16514, 16514, 16515, 16516, 16479, 16480, 16501, 16517, 16384,
    16384, 16501, 16517, 16518, 16519, 16468, 16469, 16520, 16521, 16384, 16384,
    16473, 16474, 16475, 16476, 16522, 16523, 16524, 16524};
bwrt_info bwrtTbl[16] = {
    {mp03, 1428, 2, 2},  {mp07, 1434, 2, 2}, {mp01, 1440, 2, 2},
    {mp04, 1446, 2, 2},  {mp06, 1452, 2, 2}, {mp05, 1458, 2, 2},
    {mp02, 1464, 2, 2},  {mp08, 1810, 5, 2}, {mp09, 2194, 4, 2},
    {mp10, 2578, 4, 2},  {mp11, 2204, 6, 2}, {mp11, 2588, 6, 2},
    {mp14, 2962, 15, 2}, {mp21, 914, 23, 2}, {mp22, 916, 21, 2},
    {mp23, 910, 26, 2}};
static Uint8 stone_timer = 20;
static PALETTEENTRY stone_color_set_tbl[40] = {
    {0, 64, 0, 1},      {224, 224, 224, 1}, {128, 224, 0, 1},
    {64, 160, 0, 1},    {32, 96, 0, 1},     {0, 64, 0, 1},
    {224, 224, 224, 1}, {224, 96, 0, 1},    {224, 64, 0, 1},
    {128, 0, 0, 1},     {0, 64, 0, 1},      {224, 224, 224, 1},
    {224, 224, 32, 1},  {128, 128, 0, 1},   {64, 32, 0, 1},
    {0, 64, 0, 1},      {224, 224, 224, 1}, {96, 96, 224, 1},
    {64, 64, 224, 1},   {32, 32, 192, 1},   {0, 64, 0, 1},
    {224, 224, 224, 1}, {64, 224, 128, 1},  {0, 160, 160, 1},
    {0, 64, 96, 1},     {0, 64, 0, 1},      {224, 224, 224, 1},
    {128, 0, 224, 1},   {64, 0, 128, 1},    {32, 0, 64, 1},
    {0, 64, 0, 1},      {224, 224, 224, 1}, {224, 0, 0, 1},
    {128, 0, 0, 1},     {64, 0, 32, 1},     {0, 64, 0, 1},
    {224, 224, 224, 1}, {128, 224, 0, 1},   {64, 160, 0, 1},
    {32, 96, 0, 1},
};
void m2mainchk(void) {}

void m2subset(void) {}

void DLL_meminit(char ***pBufTbl, void **pFuncTbl) {
    Uint32 hf;
    char fn[80];

    SetGrid = *pFuncTbl++;
    EAsprset = *pFuncTbl++;
    ClrSpriteDebug = *pFuncTbl++;
    WaveRequest = *pFuncTbl++;
    CDPlay = *pFuncTbl++;
    CDPause = *pFuncTbl++;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    WaveAllStop = *pFuncTbl++;

    ++pFuncTbl;
    ++pFuncTbl;
    sMemSet = *pFuncTbl++;
    sMemCpy = *pFuncTbl++;
    ++pFuncTbl;
    sRandom = *pFuncTbl++;
    ++pFuncTbl;
    ++pFuncTbl;
    ++pFuncTbl;
    sPrintf = *pFuncTbl++;
    sOutputDebugString = *pFuncTbl++;
    sOpenFile = *pFuncTbl++;
    sReadFile = *pFuncTbl++;
    sCloseFile = *pFuncTbl++;
    ++pFuncTbl;

    pmapwk = (Uint16 *)**pBufTbl++;
    lpcolorwk = (PALETTEENTRY *)**pBufTbl++;
    lpcolorwk2 = (PALETTEENTRY *)**pBufTbl++;
    lpcolorwk3 = (PALETTEENTRY *)**pBufTbl++;
    lpcolorwk4 = (PALETTEENTRY *)**pBufTbl++;
    hdatabuf = (Uint32 *)**pBufTbl++;
    lpFadeFlag = (Sint32 *)*pBufTbl++;
    lpKeepWork = (game_info *)*pBufTbl++;

    plring_dmy = lpKeepWork->plring;
    stagenm = lpKeepWork->stagenm;
    clrspflg = lpKeepWork->clrspflg_save;
    clrspflg_save = lpKeepWork->clrspflg_save;
    extra_score = lpKeepWork->extrascore;
    game_score = lpKeepWork->plscore;
    pl_suu = lpKeepWork->pl_suu;

    spgmmode = 0;
    if (lpKeepWork->ta_flag) {
        spgmmode |= 2;
    }

    sPrintf(fn, map_filename, lpKeepWork->stagenm);
    hf = sOpenFile(fn);
    sReadFile(hf, sm_adr0, sizeof(sm_adr0));
    sCloseFile(hf);

    lpKeepWork->sm_adr0 = sm_adr0;
    lpKeepWork->pSprBmp = SprBmp;
    lpKeepWork->ptv_adr = (Uint16 *)tv_adr;
}

void DLL_memfree(void) {}

Sint32 Get_vscroll(void) { return vscroll.l; }

Sint32 Get_scra_h_posiw(void) { return scra_h_posiw.l; }

Sint32 Get_scrb_h_posiw(void) { return scrb_h_posiw.l; }

Sint32 game(void) {
    Sint32 i;

    if (lpKeepWork->SPEMode != 0 || spgmmode & 1) {
        SetUseOk(0, 103, 0);
        SetUseOk(0, 102, 0);
    } else {
        SetUseOk(0, 103, 1);
        if (spgmmode & 2)
            SetUseOk(0, 102, 1);
        else
            SetUseOk(0, 102, 0);
    }

    if (lpKeepWork->SPEMode != 0) {
        switch (lpKeepWork->SPEMode) {
        case 1:
            lpKeepWork->SPEMode = 2;
            disp();
            if (!(spgmmode & 2)) {
                sub_sync(107);
            }
            return 0;
        case 2:
            for (i = 0; i < 3; ++i) {
                if (disp_main()) {

                    lpKeepWork->extrascore = extra_score;
                    lpKeepWork->plscore = game_score;
                    lpKeepWork->pl_suu = pl_suu;
                    lpKeepWork->SPEMode = 3;
                    break;
                }
            }
            game_score_disp();
            time_bonus_disp();
            ring_bonus_disp();
            sonic_disp();
            return 0;
        case 3:
            lpKeepWork->SPEMode = 4;
            CDPause(2);
            return 3;
        case 5:
            lpKeepWork->SPEMode = 6;

            lpKeepWork->ta_time = (spe_time.l >> 16 & 255) * 60 * 60;
            lpKeepWork->ta_time += (spe_time.l >> 8 & 255) * 60;
            lpKeepWork->ta_time += spe_time.l & 255;
            CDPause(2);
            return 4;
        default:
            return 0;
        }
    }

    if (spgmmode & 2) {
        if ((swdata1.b.l & 112) == 112) {

            lpKeepWork->ta_time = -1;
            CDPause(2);
            lpKeepWork->SPEMode = 6;
            SetUseOk(0, 102, 0);
            SetUseOk(0, 103, 0);
            return 4;
        }
    }
    if (pause_chk())
        return 0;

    if (spgmmode & 1) {
        if (swdata1.b.h & 240) {
            lpKeepWork->ta_time = -1;
            CDPause(2);
            lpKeepWork->SPEMode = 7;
            return 4;
        }
    }

    rlscrflg_save = rlscrflg;
    rlscrflg = 0;

    init_3dpara();
    kaiten();
    scalinit();
    action();
    if (lpKeepWork->SpecialTime == 0)
        time_dec();
    if (ufochk() != 0 && spgmmode & 2)
        pldeadflg = 1;

    if (pldeadflg) {
        clwout();
        game_over();
        if (spgmmode & 2) {
            lpKeepWork->SPEMode = 5;
            return 0;
        }
        lpKeepWork->SPEMode = 1;
        return 1;
    }

    if (gmclrflg) {
        game_clear();
        clwout();
        lpKeepWork->SPEMode = 1;
        return 0;
    }
    hscal();
    intmode = 0;
    ringno_disp();
    ufoleft_disp();
    stone_color_set();
    spetime_disp();
    color_change();

    return 0;
}

Sint32 ufochk(void) {
    Sint32 i;

    for (i = 0; i < 6; ++i) {
        if (actwk[i + 32].actno)
            return 0;
    }
    return -1;
}

void game_clear(void) {
    clrspflg |= (Uint8)(1 << stagenm);
    lpKeepWork->clrspflg_save = clrspflg;
    game_over();
}

void game_over(void) {
    Uint8 d0;

    d0 = stagenm;
    do {
        ++d0;
        if (d0 >= 7) {
            d0 = 0;
        }
        if (clrspflg == 127)
            return;

    } while ((clrspflg & 1 << d0) != 0);
}

void game_init(void) {
    PALETTEENTRY *lpPeDest;
    Sint32 i;

    overint.l = -1;

    sMemSet(actwk, 0, sizeof(actwk));

    lpPeDest = lpcolorwk;
    for (i = 0; i < 64; ++i)
        *lpPeDest++ = colortbl[i];

    stage_init();

    demoflg = 1;
    pauseflg = 128;
    overint.l = 0;

    spe_time.l = 0;
    ringno = 0;
    ufoleft = 6;

    time_stop = 0;

    kaiten_init();
    colli_init();

    actwk[0].actno = 1;
    actwk[1].actno = 6;
    actwk[3].actno = 10;
    actwk[4].actno = 11;

    ufo_initial();

    tcnt20 = 20;
    rufo_getnm = 20;
    game_start = 1;

    if (!(spgmmode & 2)) {
        spe_time.l = 100;
    }

    CDPlay(32);
}

void syspatchg(void) {}

void sdfdout(void) { soundset(171); }

void sdfdin(void) {}

void disp(void) {
    Sint16 i;
    PALETTEENTRY *lpPeSrc, *lpPeDest;

    ring_bonus = ringno * 20;
    time_bonus = spe_time.l * 20;

    demoflg = 0;
    specialflg = 0;

    clrspflg_save = clrspflg;

    lpPeSrc = dispcolor;
    lpPeDest = lpcolorwk;
    for (i = 0; i < 64; ++i) {
        *lpPeDest++ = *lpPeSrc++;
    }

    map_init();
    clwin_init();
    clwin();
    gmtime0 = 480;
    sonicup_no = 0;
}

Sint32 disp_main(void) {
    if (ring_bonus != 0) {

        gsc_up20();
        ring_bonus -= 20;
    } else {
        if (time_bonus != 0) {

            time_bonus -= 20;
            gsc_up20();
        }
    }
    if (seregiflg == 0) {

        if ((time_bonus + ring_bonus) == 0) {

            if (time_bonus == 0)
                WaveAllStop();
            seregiflg = 1;

            soundset(154);
        }
    }

    if (--gmtime0)
        return 0;

    gsc_up(ring_bonus + time_bonus);

    WaveAllStop();
    soundset(200);
    clwout();
    return 1;
}

void gsc_up20(void) {

    if (++scoreup_setm % 6 == 0)
        key_set(189);
    gsc_up(20);
}

void gsc_up(Sint32 lScore) {

    lScore += game_score;

    while (lScore > extra_score) {

        extra_score += 5000;
        ++pl_suu;
        ++sonicup_no;
        WaveAllStop();
        sub_sync(122);
        if (pl_suu >= 250)
            pl_suu = 249;
    }

    game_score = lScore;
    if ((Uint32)game_score >= 1000000) {
        game_score = 999999;
    }
}

Sint16 pause_chk(void) {

    if (pauseflg & 128) {
        if (pauseflg & 1) {

            if (swdata1.b.l & 128) {
                if (swdata2.w & 32896) {
                    if (PauseIcon != 0) {
                        pauseflg &= 254;
                        PauseIcon = 0;
                        return 0;
                    }
                } else {
                    pauseflg &= 254;
                    CDPause(1);
                    return 0;
                }
            }

            return 1;
        }

        if (!(spgmmode & 1)) {

            if (swdata1.b.l & 128) {
                if (swdata2.w & 32896)
                    PauseIcon = 1;
                pauseflg |= 1;
                CDPause(0);
                return 1;
            }
        }
    }

    return 0;
}

void map_init(void) {
    Uint32 d5;
    Sint32 i;

    if (time_bonus != 0) {
        if (clrspflg_save != 127)
            d5 = 15;
        else
            d5 = 16;
    } else
        d5 = 14;

    bwrtset(d5);

    for (i = 0; i < 7; ++i) {

        if (clrspflg_save & 1 << i) {
            bwrtset(i + 1);
        }
    }
    bwrtset(134810123);
    bwrtset(3085);
}

void bwrtset(Uint32 tblno4) {
    Sint32 i;
    Sint32 xbase, ybase, x, y;
    Sint32 tileno;
    Uint16 col, tblno;

    do {
        tblno = tblno4 & 255;
        if (!tblno)
            break;
        --tblno;
        xbase = (bwrtTbl[tblno].vadr & 127) / 2;
        ybase = bwrtTbl[tblno].vadr / 128;
        i = 0;
        for (y = 0; y < bwrtTbl[tblno].ysize; ++y) {
            for (x = 0; x < bwrtTbl[tblno].xsize; ++x) {
                tileno = bwrtTbl[tblno].tbl[i] & 2047;
                col = bwrtTbl[tblno].tbl[i] & 24576;
                ++i;
                if (col == 24576) {
                    tileno += 687;
                } else if (col == 16384) {
                    tileno += 458;
                } else if (col == 8192) {
                    tileno += 229;
                }
                SetGrid(0, xbase + x, ybase + y, tileno, 0);
            }
        }

        tblno4 >>= 8;
    } while (1);
}

void stone_color_set(void) {
    PALETTEENTRY *lpPeSrc, *lpPeDest;
    Sint16 i;

    if (ufoleft != 0)
        return;
    if (stone_timer == 0)
        return;
    if (--stone_timer != 0)
        return;

    lpPeSrc = &stone_color_set_tbl[stagenm * 5 + 1];
    lpPeDest = &lpcolorwk[50];
    for (i = 0; i < 4; ++i) {
        *lpPeDest++ = *lpPeSrc++;
    }

    intflg |= 2;
}

void panel_box(void) {}

void colset(PALETTEENTRY *lpPeSrc) {
    PALETTEENTRY *lpPeDest;
    Sint32 i;

    lpPeDest = &lpcolorwk[16];
    for (i = 0; i < 32; ++i) {
        *lpPeDest++ = *lpPeSrc++;
    }
}

void sinit0(void) {

    panel_box();

    colset(coltbl0);
}

void sinit1(void) {
    panel_box();

    colset(coltbl1);
}

void sinit2(void) {
    panel_box();

    colset(coltbl2);
}

void sinit3(void) {
    panel_box();

    colset(coltbl3);
}

void sinit4(void) {
    Sint16 i;

    panel_box();

    colset(coltbl4);

    for (i = 0; i < 128; ++i)
        hdatabuf[i] = 4286578688;
}

void sinit5(void) {
    panel_box();

    colset(coltbl5);
}

void sinit6(void) {
    panel_box();

    colset(coltbl6);
}

void sinit7(void) {
    panel_box();

    colset(coltbl7);
}

void stage_init(void) {
    void (*jptbl[8])(void) = {&sinit0, &sinit1, &sinit2, &sinit3,
                              &sinit4, &sinit5, &sinit6, &sinit7};

    jptbl[stagenm]();
}

static Uint8 SeToWavTbl[80] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 20, 21, 22, 22, 23, 23, 24, 25, 26, 27,
    28, 29, 30, 31, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54,
    55, 56, 57, 58, 58, 59, 60, 61, 62, 63, 64, 65, 67, 68, 69, 70,
    71, 71, 72, 73, 74, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
};
static void (*hscrltbl[8])(void) = {&hscrl0, &hscrl1, &hscrl2, &hscrl3,
                                    &hscrl4, &hscrl5, &hscrl6, &hscrl7};

void soundset(Sint16 ReqNo) {
    if (ReqNo == 171)
        return;

    ReqNo -= 144;
    ReqNo = (Uint16)SeToWavTbl[ReqNo];

    WaveRequest(ReqNo);
}

void sub_sync(Sint16 ReqNo) {

    if (ReqNo >= 118 && ReqNo < 129) {
        WaveRequest(ReqNo - 43);
    } else if (ReqNo >= 15 && ReqNo < 35) {
        CDPlay(ReqNo - 12);
    } else if (ReqNo >= 102 && ReqNo < 113) {
        CDPlay(ReqNo - 79);
    } else if (ReqNo == 113) {
        CDPlay(2);
    } else if (ReqNo >= 114 && ReqNo < 116) {
        CDPlay(ReqNo - 80);
    } else if (ReqNo == 146) {
        WaveRequest(85);
    }
}

void key_set(Sint16 ReqNo) { soundset(ReqNo); }

void SWdataSet(ushort_union sw1, ushort_union sw2) {
    swdata1.w = sw1.w;
    swdata2.w = sw2.w;
}

void hscal_1(void) {
    if (!scroll_stop) {
        if (stagenm == 0)
            hscal1_0();
        else if (stagenm == 4)
            hscal1_4();
        else if (stagenm == 5)
            hscal1_5();
    }
}

void hscal_2(void) {
    if (!scroll_stop) {
        if (stagenm == 0)
            hscal2_0();
        else if (stagenm == 4)
            hscal2_4();
        else if (stagenm == 5)
            hscal2_5();
    }
}

void hscal0_0(void) {
    Sint32 d0, d1;
    Sint16 *a0, *a1;
    Sint16 d3, d4, d7;

    d0 = 524288;
    d1 = 0;
    rlspd_get(&d0, &d1);
    hsdata1 += d0;
    d0 >>= 1;
    hsdata0 += d0;

    if (!hscrflg) {
        ++hscrcnt;
        if (hscrcnt >= 512) {
            hscrcnt = 511;
            hscrflg = 1;
        }
    } else {

        if ((Sint16)--hscrcnt < 0) {
            hscrcnt = 0;
            hscrflg = 0;
        }
    }

    a0 = (Sint16 *)&hdatabuf[0];
    a1 = (Sint16 *)hdatabuf_l;
    d0 = 65536;
    d1 = 1024;
    d3 = 0;
    d4 = 0;
    d7 = 23;
    hscal0(a0, a1, d0, d1, d3, d4, d7);
}

void hscal1_0(void) {
    Sint32 d0, d1;
    Sint16 *a0, *a1;
    Sint16 d3, d4, d7;

    a0 = (Sint16 *)&hdatabuf[24];
    a1 = (Sint16 *)&hdatabuf_l[24];
    d0 = 40960;
    d1 = 1024;
    d3 = hsdata1 >> 16;
    d4 = 0;
    d7 = 71;

    hscal0(a0, a1, d0, d1, d3, d4, d7);
}

void hscal2_0(void) {
    Sint32 d0, d1;
    Sint16 *a0, *a1;
    Sint16 d3, d4, d7;

    a0 = (Sint16 *)&hdatabuf[96];
    a1 = (Sint16 *)&hdatabuf_l[96];
    d0 = -32768;
    d1 = 1024;
    d3 = hsdata0 >> 16;
    d4 = 0;
    d7 = 31;

    hscal0(a0, a1, d0, d1, d3, d4, d7);
}

void hscal0(Sint16 *a0, Sint16 *a1, Sint32 d0, Sint32 d1, Sint16 d3, Sint16 d4,
            Sint16 d7) {
    int_union d2;

    if (!hscrflg) {

        do {
            *a0++ = d3;
            *a1++ = d4;
            d2.w.h = *a0;
            d2.w.l = *a1;
            d2.l += d0;
            *a1++ = d2.w.l;
            *a0++ = d2.w.h;
            d0 -= d1;
        } while (--d7 >= 0);
    } else {

        do {
            *a0++ = d3;
            *a1++ = d4;
            d2.w.h = *a0;
            d2.w.l = *a1;
            d2.l -= d0;
            *a1++ = d2.w.l;
            *a0++ = d2.w.h;
            d0 -= d1;
        } while (--d7 >= 0);
    }
}

void hscal0_4(void) {
    Sint32 d0, d1;
    Sint16 *a0, *a1;
    Sint32 *a2;
    Sint16 d7;

    d0 = 0x40000;
    d1 = 0;
    rlspd_get(&d0, &d1);
    hsdata0 += d0;

    hscrflg = 0;
    if (rlscrflg_save & 4) {

        hscrflg = 1;
        ++hscrcnt4;
        hscrcnt4 &= 15;
        if (!hscrcnt4)
            hscrflg = 2;
    }

    if (rlscrflg_save & 8) {

        hscrflg = 255;
        --hscrcnt4;
        hscrcnt4 &= 15;
        if (!hscrcnt4)
            hscrflg = 2;
    }

    a0 = (Sint16 *)&hdatabuf[0];
    a1 = (Sint16 *)&hdatabuf_l[0];
    a2 = &hdatatbl4[0];
    d0 = 0;
    d7 = 16;

    do {
        *(Sint32 *)a0 = d0;
        *(Sint32 *)a1 = d0;
        a0 += 2;
        a1 += 2;
    } while (--d7 >= 0);
    d7 = 6;
    hscal4(a0, a1, a2, 0, d7);
}

void hscal1_4(void) {
    Sint16 *a0, *a1;
    Sint32 *a2;
    Sint16 d7;

    a0 = (Sint16 *)&hdatabuf[24];
    a1 = (Sint16 *)&hdatabuf_l[24];
    a2 = &hdatatbl4[7];

    d7 = 55;

    hscal4(a0, a1, a2, hsdata0 >> 16, d7);
}

void hscal2_4(void) {
    Sint16 *a0, *a1;
    Sint32 *a2;
    Sint16 d7;

    a0 = (Sint16 *)&hdatabuf[80];
    a1 = (Sint16 *)&hdatabuf_l[80];
    a2 = &hdatatbl4[63];

    d7 = 47;

    hscal4(a0, a1, a2, hsdata0 >> 16, d7);
}

void hscal4(Sint16 *a0, Sint16 *a1, Sint32 *a2, Sint16 d0, Sint16 d7) {
    int_union d2;

    if (hscrflg == 2) {

        do {
            *a0++ = d0;
            *a1++ = d0;
            *a0++ = -128;
            *a1++ = 0;
        } while (--d7 >= 0);
        return;
    }
    if (hscrflg == 0)
        return;
    if ((char)hscrflg > 0) {

        do {
            *a0++ = d0;
            *a1++ = d0;
            d2.w.h = *a0;
            d2.w.l = *a1;
            d2.l += *a2++;
            *a1++ = d2.w.l;
            *a0++ = d2.w.h;
        } while (--d7 >= 0);
    } else {

        do {
            *a0++ = d0;
            *a1++ = d0;
            d2.w.h = *a0;
            d2.w.l = *a1;
            d2.l -= *a2++;
            *a1++ = d2.w.l;
            *a0++ = d2.w.h;
        } while (--d7 >= 0);
    }
}

void hscal0_5(void) {
    Sint32 d0, d1;
    Sint16 *a0;
    char *a2;
    Sint16 d2, d7;

    d0 = 0x40000;
    d1 = 0;
    rlspd_get(&d0, &d1);
    hsdata0 += d0;

    a0 = (Sint16 *)&hdatabuf[0];
    a2 = hdatatbl5;
    d0 = 0;
    d2 = hscrcnt4;
    ++d2;
    d2 &= 127;
    hscrcnt4 = d2;
    d7 = 23;

    hscal5(a0, a2, 0, d2, d7);
}

void hscal1_5(void) {
    Sint16 *a0;
    char *a2;
    Sint16 d2, d7;

    a0 = (Sint16 *)&hdatabuf[24];
    a2 = hdatatbl5;

    d2 = hscrcnt4;
    d7 = 55;

    hscal5(a0, a2, hsdata0 >> 16, d2, d7);
}

void hscal2_5(void) {
    Sint16 *a0;
    char *a2;
    Sint16 d2, d7;

    a0 = (Sint16 *)&hdatabuf[80];
    a2 = hdatatbl5;

    d2 = hscrcnt4;
    d7 = 47;

    hscal5(a0, a2, hsdata0 >> 16, d2, d7);
}

void hscal5(Sint16 *a0, char *a2, Sint16 d0, Sint16 d2, Sint16 d7) {
    Sint16 d1;

    do {
        *a0++ = d0;
        d1 = a2[d2];
        *a0++ = d1;
        ++d2;
        d2 &= 127;
    } while (--d7 >= 0);
    hscrcnt4 = d2;
}

void rlspd_get(Sint32 *d0, Sint32 *d1) {
    if (rlscrflg_save & 4)
        return;
    *d0 = -*d0;
    *d1 = -*d1;
    if (rlscrflg_save & 8)
        return;
    *d0 = 0;
    *d1 = 0;
}

void hscrspd_get(Sint32 *lD0, Sint32 *lD1) {

    *lD0 = 0x40000;
    rlspd_get(lD0, lD1);
    hsdata2 += *lD0;
    *lD0 /= 2;
    hsdata1 += *lD0;
    *lD0 /= 2;
    hsdata0 += *lD0;
}

void hscrl0(void) {
    hscal0_0();
    hscal1_0();
    hscal2_0();
}

void hscrl1(void) {
    Sint32 lD0;
    Sint32 lD1;
    Sint32 i;

    for (i = 0; i < 3; ++i) {
        hscrspd_get(&lD0, &lD1);
        hdatabuf[0] = hsdata1;
        hdatabuf[1] = hsdata2;
    }
}

void hscrl2(void) {
    Sint32 lD0;
    Sint32 lD1;
    Sint32 i;

    for (i = 0; i < 3; ++i) {
        lD0 = 524288;
        lD1 = 65536;
        rlspd_get(&lD0, &lD1);

        hsdata0 += lD0;
        lD0 -= lD1;
        hsdata1 += lD0;
        lD0 -= lD1;
        hsdata2 += lD0;
        lD0 -= lD1;
        hsdata3 += lD0;
        lD0 -= lD1;
        hsdata4 += lD0;
        lD0 -= lD1;
        hsdata5 += lD0;

        hdatabuf[0] = hsdata4;

        hdatabuf[1] = hsdata1;
        hdatabuf[2] = hsdata2;
        hdatabuf[3] = hsdata3;
        hdatabuf[4] = hsdata5;
    }
}

void hscrl3(void) {
    Sint32 lD0;
    Sint32 lD1;
    Sint32 i;

    for (i = 0; i < 3; ++i) {
        hscrspd_get(&lD0, &lD1);
        hdatabuf[0] = hsdata1;
        hdatabuf[1] = hsdata0;
        hdatabuf[2] = hsdata2;
    }
}

void hscrl4(void) {
    hscal0_4();
    hscal1_4();
    hscal2_4();
}

void hscrl5(void) {
    hscal0_5();
    hscal1_5();
    hscal2_5();
}

void hscrl6(void) {
    Sint32 lD0;
    Sint32 lD1;
    Sint32 i;

    for (i = 0; i < 3; ++i) {
        lD0 = 524288;
        lD1 = 98304;
        rlspd_get(&lD0, &lD1);

        hsdata0 += lD0;
        lD0 -= lD1;
        hsdata1 += lD0;
        lD0 -= lD1;
        hsdata2 += lD0;
        lD0 -= lD1;
        hsdata3 += lD0;

        hdatabuf[0] = hsdata1;
        hdatabuf[1] = hsdata3;
        hdatabuf[2] = hsdata0;
        hdatabuf[3] = hsdata2;
    }
}

void hscrl7(void) {}

void hscal(void) {
    if (!scroll_stop) {
        hscrltbl[stagenm]();
    }
}

void colchg0(void) {
    Uint16 d0, d1, d5, d6, d7;
    PALETTEENTRY *lpPeDest;
    PALETTEENTRY dummy;
    Sint32 i;

    d5 = 8;
    d6 = 0;
    d7 = 1000;
    d0 = (Sint16)cntplus(&clchgcnt[0], d5, d6, d7);
    d0 &= 65520;
    lpPeDest = &lpcolorwk[32];
    for (i = 0; i < 16; ++i) {
        d1 = cltbl[d0++];
        if (d1 > 16)
            d1 <<= 4;

        if (i == 0)
            d1 = 0;
        dummy.peRed = (d1 & 15) << 4;
        dummy.peGreen = d1 & 240;
        dummy.peBlue = (d1 & 3840) >> 4;
        dummy.peFlags = 1;
        *lpPeDest++ = dummy;
    }
}

void colchg1(void) {
    Sint16 d0;
    PALETTEENTRY *lpPeDest;
    PALETTEENTRY cltbl[8] = {
        {0, 0, 224, 1}, {0, 96, 224, 1}, {96, 160, 224, 1}, {160, 192, 224, 1},
        {0, 0, 224, 1}, {0, 96, 224, 1}, {96, 160, 224, 1}, {160, 192, 224, 1}};
    Sint32 i;

    d0 = clchgcnt[0]++ & 3;
    lpPeDest = &lpcolorwk[26];

    for (i = 0; i < 4; ++i) {
        lpPeDest->peRed = cltbl[d0 + i].peRed;
        lpPeDest->peGreen = cltbl[d0 + i].peGreen;
        lpPeDest->peBlue = cltbl[d0 + i].peBlue;
    }
}

void colchg2(void) {
    Uint16 d0, d5, d6, d7;
    PALETTEENTRY *lpPeDest;
    PALETTEENTRY cltbl0[24] = {
        {0, 0, 224, 1},    {32, 64, 224, 1},  {64, 128, 224, 1},
        {64, 192, 224, 1}, {32, 64, 224, 1},  {0, 0, 224, 1},
        {64, 192, 224, 1}, {64, 128, 224, 1}, {64, 128, 224, 1},
        {32, 64, 224, 1},  {64, 128, 224, 1}, {32, 64, 224, 1},
        {64, 192, 224, 1}, {64, 128, 224, 1}, {32, 64, 224, 1},
        {0, 0, 224, 1},    {64, 128, 224, 1}, {64, 192, 224, 1},
        {0, 0, 224, 1},    {32, 64, 224, 1},  {32, 64, 224, 1},
        {64, 128, 224, 1}, {32, 64, 224, 1},  {64, 128, 224, 1}};
    PALETTEENTRY cltbl1[6] = {{0, 0, 224, 1},   {96, 192, 224, 1},
                              {0, 128, 224, 1}, {0, 0, 224, 1},
                              {0, 128, 224, 1}, {96, 192, 224, 1}};
    d5 = 2;
    d6 = 0;
    d7 = 46;
    d0 = (Sint16)cntplus(&clchgcnt[0], d5, d6, d7);
    d0 &= 65528;
    d0 >>= 1;
    lpPeDest = &lpcolorwk[26];
    *lpPeDest++ = cltbl0[d0];
    *lpPeDest++ = cltbl0[d0 + 1];
    *lpPeDest++ = cltbl0[d0 + 2];
    *lpPeDest = cltbl0[d0 + 3];

    d5 = 1;
    d6 = 0;
    d7 = 4;
    d0 = (Sint16)cntplus(&clchgcnt[1], d5, d6, d7);
    if (d0 >> 1 != 0)
        return;
    d5 = 2;
    d6 = 0;
    d7 = 10;
    d0 = (Sint16)cntplus(&clchgcnt[2], d5, d6, d7);
    d0 >>= 1;
    lpPeDest = &lpcolorwk[33];
    *lpPeDest = cltbl1[d0];
}

void colchg3(void) {
    Sint16 d0, d5, d6, d7;
    PALETTEENTRY *lpPeDest;
    PALETTEENTRY cltbl0[16] = {
        {0, 32, 224, 1},   {32, 64, 224, 1},  {64, 128, 224, 1},
        {64, 192, 224, 1}, {64, 192, 224, 1}, {0, 32, 224, 1},
        {32, 64, 224, 1},  {64, 128, 224, 1}, {64, 128, 224, 1},
        {64, 192, 224, 1}, {0, 32, 224, 1},   {32, 64, 224, 1},
        {32, 64, 224, 1},  {64, 128, 224, 1}, {64, 192, 224, 1},
        {0, 32, 224, 1}};
    PALETTEENTRY cltbl1[8] = {{96, 192, 224, 1},  {128, 224, 224, 1},
                              {160, 224, 224, 1}, {192, 224, 224, 1},
                              {224, 224, 224, 1}, {192, 224, 224, 1},
                              {160, 224, 224, 1}, {128, 224, 224, 1}};
    d0 = clchgcnt[0];
    clchgcnt[0] += 2;
    d0 &= 24;
    d0 >>= 1;
    lpPeDest = &lpcolorwk[26];
    *lpPeDest++ = cltbl0[d0];
    *lpPeDest++ = cltbl0[d0 + 1];
    *lpPeDest++ = cltbl0[d0 + 2];
    *lpPeDest = cltbl0[d0 + 3];

    d5 = 1;
    d6 = 0;
    d7 = 2;
    d0 = (Sint16)cntplus(&clchgcnt[1], (Uint16)d5, (Uint16)d6,
                         (Uint16)d7);
    if (d0)
        return;

    d0 = clchgcnt[2];
    clchgcnt[2] += 2;
    d0 &= 14;
    d0 >>= 1;

    lpPeDest = &lpcolorwk[40];
    *lpPeDest++ = cltbl1[d0];
}

void colchg4(void) {
    PALETTEENTRY tbl0[7] = {
        {0, 32, 224, 1},  {0, 64, 224, 1},  {0, 96, 224, 1}, {0, 128, 224, 1},
        {0, 160, 224, 1}, {0, 128, 224, 1}, {0, 96, 224, 1}};
    PALETTEENTRY tbl1[26] = {
        {224, 224, 0, 1}, {192, 224, 0, 1}, {160, 224, 0, 1}, {128, 224, 0, 1},
        {96, 224, 0, 1},  {64, 224, 0, 1},  {32, 224, 0, 1},  {0, 224, 32, 1},
        {0, 224, 64, 1},  {0, 224, 96, 1},  {0, 224, 128, 1}, {0, 224, 160, 1},
        {0, 224, 192, 1}, {0, 224, 224, 1}, {0, 224, 192, 1}, {0, 224, 160, 1},
        {0, 224, 128, 1}, {0, 224, 96, 1},  {0, 224, 64, 1},  {0, 224, 32, 1},
        {32, 224, 0, 1},  {64, 224, 0, 1},  {96, 224, 0, 1},  {128, 224, 0, 1},
        {160, 224, 0, 1}, {192, 224, 0, 1}};
    PALETTEENTRY tbl2[26] = {
        {0, 224, 224, 1}, {0, 224, 192, 1}, {0, 224, 160, 1}, {0, 224, 128, 1},
        {0, 224, 96, 1},  {0, 224, 64, 1},  {0, 224, 32, 1},  {32, 224, 0, 1},
        {64, 224, 0, 1},  {96, 224, 0, 1},  {128, 224, 0, 1}, {160, 224, 0, 1},
        {192, 224, 0, 1}, {224, 224, 0, 1}, {192, 224, 0, 1}, {160, 224, 0, 1},
        {128, 224, 0, 1}, {96, 224, 0, 1},  {64, 224, 0, 1},  {32, 224, 0, 1},
        {0, 224, 32, 1},  {0, 224, 64, 1},  {0, 224, 96, 1},  {0, 224, 128, 1},
        {0, 224, 160, 1}, {0, 224, 192, 1}};
    PALETTEENTRY tbl3[28] = {
        {224, 0, 0, 1}, {192, 0, 0, 1}, {160, 0, 0, 1}, {128, 0, 0, 1},
        {96, 0, 0, 1},  {64, 0, 0, 1},  {32, 0, 0, 1},  {0, 0, 0, 1},
        {32, 0, 0, 1},  {64, 0, 0, 1},  {96, 0, 0, 1},  {128, 0, 0, 1},
        {160, 0, 0, 1}, {192, 0, 0, 1}, {224, 0, 0, 1}, {192, 0, 0, 1},
        {160, 0, 0, 1}, {128, 0, 0, 1}, {96, 0, 0, 1},  {64, 0, 0, 1},
        {32, 0, 0, 1},  {0, 0, 0, 1},   {32, 0, 0, 1},  {64, 0, 0, 1},
        {96, 0, 0, 1},  {128, 0, 0, 1}, {160, 0, 0, 1}, {192, 0, 0, 1}};
    Uint16 d0, d5, d6, d7;
    PALETTEENTRY *lpPeDest;

    d5 = 2;
    d6 = 0;
    d7 = 12;
    d0 = (Sint16)cntplus(&clchgcnt[0], d5, d6, d7);
    d0 >>= 1;
    lpPeDest = &lpcolorwk[26];
    *lpPeDest = tbl0[d0];

    d5 = 2;
    d6 = 0;
    d7 = 50;
    d0 = (Sint16)cntplus(&clchgcnt[1], d5, d6, d7);
    d0 >>= 1;
    lpPeDest = &lpcolorwk[45];
    *lpPeDest = tbl1[d0];

    d5 = 1;
    d6 = 0;
    d7 = 51;
    d0 = (Sint16)cntplus(&clchgcnt[2], d5, d6, d7);
    d0 >>= 1;
    lpPeDest = &lpcolorwk[46];
    *lpPeDest = tbl2[d0];

    d5 = 1;
    d6 = 0;
    d7 = 55;
    d0 = (Sint16)(cntplus(&clchgcnt[3], d5, d6, d7) & 65534);
    d0 >>= 1;
    lpPeDest = &lpcolorwk[47];
    *lpPeDest = tbl3[d0];
}

void colchg5(void) {
    Uint16 d0, d5, d6, d7;
    PALETTEENTRY *lpPeDest;
    Sint16 i;
    static PALETTEENTRY tbl0[26] = {
        {224, 64, 0, 1},   {224, 128, 0, 1},   {224, 192, 0, 1},
        {160, 224, 64, 1}, {128, 224, 128, 1}, {64, 224, 192, 1},
        {64, 160, 224, 1}, {128, 96, 224, 1},  {160, 0, 224, 1},
        {224, 32, 224, 1}, {224, 0, 160, 1},   {224, 0, 96, 1},
        {224, 0, 32, 1},   {224, 64, 0, 1},    {224, 128, 0, 1},
        {224, 192, 0, 1},  {160, 224, 64, 1},  {128, 224, 128, 1},
        {64, 224, 192, 1}, {64, 160, 224, 1},  {128, 96, 224, 1},
        {160, 0, 224, 1},  {224, 32, 224, 1},  {224, 0, 160, 1},
        {224, 0, 96, 1},   {224, 0, 32, 1}};
    static PALETTEENTRY tbl1[12] = {
        {0, 0, 224, 1},   {64, 0, 32, 1}, {0, 32, 224, 1},  {96, 0, 64, 1},
        {32, 64, 224, 1}, {64, 0, 32, 1}, {64, 96, 224, 1}, {32, 0, 0, 1},
        {32, 64, 224, 1}, {0, 0, 0, 1},   {0, 32, 224, 1},  {32, 0, 0, 1}};

    d5 = 2;
    d6 = 0;
    d7 = 24;
    d0 = (Sint16)(cntplus(&clchgcnt[0], d5, d6, d7) & 65534);

    d0 >>= 1;
    lpPeDest = &lpcolorwk[33];

    for (i = 0; i < 12; ++i) {
        *lpPeDest++ = tbl0[d0++];
    }
    d5 = 2;
    d6 = 0;
    d7 = 22;
    d0 = (Sint16)(cntplus(&clchgcnt[1], d5, d6, d7) & 65532);
    d0 >>= 1;
    lpPeDest = &lpcolorwk[29];
    *lpPeDest++ = tbl1[d0++];
    *lpPeDest = tbl1[d0];
}

void colchg6(void) {
    PALETTEENTRY tbl[12] = {
        {0, 32, 192, 1}, {0, 32, 192, 1}, {0, 64, 224, 1},  {0, 64, 224, 1},
        {0, 96, 224, 1}, {0, 96, 224, 1}, {0, 128, 224, 1}, {0, 128, 224, 1},
        {0, 96, 224, 1}, {0, 96, 224, 1}, {0, 64, 224, 1},  {0, 64, 224, 1}};
    Uint16 d0, d5, d6, d7;
    PALETTEENTRY *lpPeDest;

    d5 = 1;
    d6 = 0;
    d7 = 23;
    d0 = (Sint16)(cntplus(&clchgcnt[0], d5, d6, d7) & 65534);
    d0 >>= 1;
    lpPeDest = &lpcolorwk[31];
    *lpPeDest = tbl[d0];
}

void colchg7(void) {}

void color_change(void) {
    void (*jptbl[8])(void) = {&colchg0, &colchg1, &colchg2, &colchg3,
                              &colchg4, &colchg5, &colchg6, colchg7};

    jptbl[stagenm]();
}

dlink_export ExportedFunctions = {&game_init,
                                  (void (*)(void)) & game,
                                  &DLL_meminit,
                                  &DLL_memfree,
                                  (void (*)(Sint16, Sint16)) & SWdataSet,
                                  &Get_vscroll,
                                  &Get_scra_h_posiw,
                                  &Get_scrb_h_posiw,
                                  &FadeProc,
                                  0,
                                  0,
                                  &Special_block_chg};

