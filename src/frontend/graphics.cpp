#include <SDL3/SDL.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <array>
#include <format>
#include <stdexcept>
#include <string>
#include <vector>
#include "constants.h"
#include "file.h"
#include "graphics.h"
#include "hmx.h"
#include "movie.h"
#include "szdd.hpp"

Uint16 mapwk[32768];
PALETTEENTRY colorwk[64];
PALETTEENTRY colorwk2[64];
PALETTEENTRY colorwk3[64];
PALETTEENTRY colorwk4[64];
int_union hscrollbuff[256];
Sint32 fade_flag;

/* The title screen's intro flash rewrites all four palette lines, so the
   opening draws Sonic from copies of his sprites whose colours live at palette
   entries 144-159 instead (see OEFlashSpritesCreate() in
   src/title/opening/opndo.c).  Those entries hold the fourth palette line as
   the title module left it when it was loaded, i.e. Sonic's own colours; they
   are saved here because line 3 itself is what the flash overwrites. */
static PALETTEENTRY titleUnfadedPalette[16];

static SDL_Palette *sharedPalette;
static SDL_Surface *framebuffer;
static SDL_Surface *presentSurface;
static SDL_Texture *presentTexture;

/* Soft scanline filter, off unless the frontend is started with --scanlines
   (see Graphics_SetScanlines()). */
static bool scanlines;

/* Start in fullscreen, set by --fullscreen before Graphics_Init() (see
   Graphics_SetFullscreen()). */
static bool launchFullscreen;

#define ASSET_HEADER_ENCODED_LEN 16
#define ROTATE_TILE_EMPTY 0xffff
#define PLANET_ROTATE_GRID_W 10
#define PLANET_ROTATE_GRID_H 8
#define PLANET_ROTATE_SCREEN_Y 24
#define PLANET_ROTATE_SCREEN_H 176
typedef struct {
	char type[4];
	Sint32 endOffset;
	Sint32 count;
	Sint32 dataOffset;
} AssetHeader;

#define SPRITE_HEADER_ENCODED_LEN 12
typedef struct {
	Sint16 x;
	Sint16 y;
	Sint16 width;
	Sint16 paddedWidth;
	Sint16 height;
	Sint16 palette;
	Sint16 type;
} SpriteHeader;

struct BitmapData {
	Sint16 width;
	Sint16 height;
	std::array<SDL_Surface *, 4> surfaces {};
	std::array<std::vector<Uint8>, 4> pixels;

	BitmapData(Sint32 width, Sint32 height, const std::vector<Uint8> &pixelData) :
		width(width), height(height)
	{
		for (size_t i = 0; i < surfaces.size(); i++) {
			SDL_Surface *surface = createIndexedSurface(width, height, pixelData, static_cast<SDL_FlipMode>(i));
			if (!surface) {
				throw std::runtime_error(SDL_GetError());
			}
			auto &pixelVec = pixels.at(i);
			pixelVec.reserve(surface->w * surface->h);
			SDL_LockSurface(surface);
			Uint8 *surfacePixels = static_cast<Uint8 *>(surface->pixels);
			for (int y = 0; y < surface->h; y++) {
				pixelVec.insert(pixelVec.end(), surfacePixels, surfacePixels + surface->w);
				surfacePixels += surface->pitch;
			}
			SDL_UnlockSurface(surface);
			surfaces.at(i) = surface;
		}
	}

	~BitmapData() {
		for (auto &surface : surfaces) {
			SDL_DestroySurface(surface);
		}
	}

	BitmapData(const BitmapData &other) :
		width(other.width), height(other.height), pixels(other.pixels), surfaces{}
	{
		for (size_t i = 0; i < surfaces.size(); i++) {
			surfaces[i] = SDL_DuplicateSurface(other.surfaces[i]);
			if (!surfaces[i]) {
				for (size_t j = 0; j < i; j++) {
					SDL_DestroySurface(surfaces[j]);
				}
				throw std::runtime_error(SDL_GetError());
			}
		}
	}

	BitmapData(BitmapData &&other) noexcept
		: width(other.width),
		height(other.height),
		surfaces(other.surfaces),
		pixels(std::move(other.pixels))
	{
		other.surfaces.fill(nullptr);
	}

	friend void swap(BitmapData &a, BitmapData &b) noexcept {
		std::swap(a.width, b.width);
		std::swap(a.height, b.height);
		std::swap(a.surfaces, b.surfaces);
		std::swap(a.pixels, b.pixels);
	}

	BitmapData &operator=(BitmapData other) {
		std::swap(*this, other);
		return *this;
	}

private:
	SDL_Surface *createIndexedSurface(Sint32 width, Sint32 height, const std::vector<Uint8> &inPixels, SDL_FlipMode flip) {
		SDL_Surface *surface;
		surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_INDEX8);
		if (!surface) {
			SDL_Log("Couldn't create sprite surface: %s", SDL_GetError());
			return NULL;
		}
		SDL_SetSurfacePalette(surface, sharedPalette);
		SDL_SetSurfaceColorKey(surface, true, 0);
		SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);
		SDL_SetSurfaceRLE(surface, true);
		SDL_LockSurface(surface);
		for (int y = 0; y < height; y++) {
			memcpy((Uint8 *)surface->pixels + (y * surface->pitch), inPixels.data() + (y * width), width);
		}
		SDL_UnlockSurface(surface);
		if (flip != SDL_FLIP_NONE) {
			SDL_FlipSurface(surface, flip);
		}
		return surface;
	}
};

static std::vector<BitmapData> spriteBitmaps;
static std::vector<BitmapData> tileBitmaps;
static std::vector<BitmapData> changeTileBitmaps;
static std::vector<int> changeTiles;
static std::vector<BitmapData> rotateTileBitmaps;
static std::vector<Uint16> rotateTileLookup;
static Uint16 rotateGrid[128][128];
static Uint16 planetRotateGrid[PLANET_ROTATE_GRID_H][PLANET_ROTATE_GRID_W];
static bool planetMode;
static bool specialStageMode;
static bool specialClearMode;
static int specialStageIndex;
static Uint16 tileGrids[3][STAGE_GRID_H][STAGE_GRID_W];
static Uint16 specialTileGrids[8][STAGE_GRID_H][STAGE_GRID_W];

typedef struct {
	Uint8 width;
	Uint8 height;
	Uint8 pos;
} SpecialGridInfo;

typedef struct {
	Uint8 grid;
	Uint8 x;
	Uint8 y;
	Uint8 width;
	Uint8 height;
	Sint16 tileOffset;
	bool clearZero;
	const char *fileName;
} SpecialTileInfo;

typedef struct {
	const SpecialTileInfo *tiles;
	int count;
} SpecialTileSet;

static const SpecialGridInfo specialGridInfo[8][8] = {
	{{40, 2, 1}, {64, 10, 2}, {64, 4, 12}, {64, 16, 0}, {64, 12, 16}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
	{{40, 2, 1}, {64, 16, 0}, {64, 16, 0}, {40, 12, 16}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
	{{40, 2, 1}, {64, 16, 0}, {64, 6, 0}, {64, 3, 6}, {64, 1, 9}, {64, 4, 10}, {64, 2, 14}, {40, 12, 16}},
	{{40, 2, 1}, {64, 5, 11}, {64, 16, 0}, {64, 11, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
	{{40, 2, 1}, {64, 14, 2}, {40, 12, 16}, {64, 16, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
	{{40, 2, 1}, {64, 14, 2}, {40, 12, 16}, {64, 16, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
	{{40, 2, 1}, {64, 6, 10}, {64, 14, 0}, {64, 2, 14}, {64, 10, 0}, {40, 12, 16}, {0, 0, 0}, {0, 0, 0}},
	{{40, 2, 1}, {40, 16, 0}, {40, 12, 16}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
};

static const SpecialGridInfo specialClearGridInfo[3] = {
	{64, 32, 0},
	{64, 32, 0},
	{64, 32, 0},
};

static const SpecialTileInfo specialStage0Tiles[] = {
	{3, 0, 0, 32, 16, 203, false, "S1B0.BIN"},
	{3, 32, 0, 32, 16, 203, false, "S1B0.BIN"},
	{4, 0, 0, 32, 12, 35, false, "S1B1.BIN"},
	{4, 32, 0, 32, 12, 35, false, "S1B1.BIN"},
	{1, 0, 0, 64, 10, 40, true, "S1A0.BIN"},
	{2, 0, 0, 64, 4, 40, true, "S1A1.BIN"},
};

static const SpecialTileInfo specialStage1Tiles[] = {
	{1, 0, 0, 32, 16, 35, false, "MPB1_A0.BIN"},
	{1, 32, 0, 32, 16, 35, false, "MPB1_A1.BIN"},
	{2, 0, 0, 32, 16, 35, true, "MPB1_B0.BIN"},
	{2, 32, 0, 32, 16, 35, true, "MPB1_B1.BIN"},
	{3, 0, 0, 32, 12, 35, false, "MPB1_B2.BIN"},
	{3, 8, 0, 32, 12, 35, false, "MPB1_B2.BIN"},
};

static const SpecialTileInfo specialStage2Tiles[] = {
	{1, 0, 0, 32, 16, 34, true, "MPB2_A0.BIN"},
	{1, 32, 0, 32, 16, 34, true, "MPB2_A1.BIN"},
	{2, 0, 0, 32, 6, 34, true, "MPB2_B00.BIN"},
	{2, 32, 0, 32, 6, 34, true, "MPB2_B20.BIN"},
	{3, 0, 0, 32, 3, 34, true, "MPB2_B01.BIN"},
	{3, 32, 0, 32, 3, 34, true, "MPB2_B21.BIN"},
	{4, 0, 0, 32, 1, 34, true, "MPB2_B02.BIN"},
	{4, 32, 0, 32, 1, 34, true, "MPB2_B22.BIN"},
	{5, 0, 0, 32, 4, 34, true, "MPB2_B03.BIN"},
	{5, 32, 0, 32, 4, 34, true, "MPB2_B23.BIN"},
	{6, 0, 0, 32, 2, 34, true, "MPB2_B1.BIN"},
	{6, 32, 0, 32, 2, 34, true, "MPB2_B1.BIN"},
	{7, 0, 0, 32, 12, 41, true, "MPB2_B4.BIN"},
	{7, 8, 0, 32, 12, 41, true, "MPB2_B4.BIN"},
};

static const SpecialTileInfo specialStage3Tiles[] = {
	{1, 0, 0, 32, 5, 35, true, "MPB3_B1.BIN"},
	{1, 32, 0, 32, 5, 35, true, "MPB3_B3.BIN"},
	{2, 0, 0, 32, 16, 35, true, "MPB3_A0.BIN"},
	{2, 32, 0, 32, 16, 35, true, "MPB3_A1.BIN"},
	{3, 0, 0, 32, 11, 35, true, "MPB3_B0.BIN"},
	{3, 32, 0, 32, 11, 35, true, "MPB3_B2.BIN"},
};

static const SpecialTileInfo specialStage4Tiles[] = {
	{1, 0, 0, 32, 14, 172, true, "MPB4_A0.BIN"},
	{1, 32, 0, 32, 14, 172, true, "MPB4_A1.BIN"},
	{3, 0, 0, 32, 16, -97, true, "MPB4_B0.BIN"},
	{3, 32, 0, 32, 16, -97, true, "MPB4_B1.BIN"},
	{2, 0, 0, 32, 12, 35, false, "MPB4_B2.BIN"},
	{2, 8, 0, 32, 12, 35, false, "MPB4_B2.BIN"},
};

static const SpecialTileInfo specialStage5Tiles[] = {
	{1, 0, 0, 32, 14, 35, true, "SP6_A01.BIN"},
	{1, 32, 0, 32, 14, 35, true, "SP6_A02.BIN"},
	{3, 0, 0, 32, 16, 55, true, "SP6_B0.BIN"},
	{3, 32, 0, 32, 16, 55, true, "SP6_B0.BIN"},
	{2, 0, 0, 32, 12, 52, false, "SP6_000.BIN"},
	{2, 8, 0, 32, 12, 52, false, "SP6_000.BIN"},
};

static const SpecialTileInfo specialStage6Tiles[] = {
	{1, 0, 0, 32, 6, 35, true, "MPB6B2L.BIN"},
	{1, 32, 0, 32, 6, 35, true, "MPB6B2R.BIN"},
	{2, 0, 0, 32, 14, 35, true, "MPB6A1R.BIN"},
	{2, 32, 0, 32, 14, 35, true, "MPB6A1L.BIN"},
	{3, 0, 0, 32, 2, 35, true, "MPB6A2R.BIN"},
	{3, 32, 0, 32, 2, 35, true, "MPB6A2L.BIN"},
	{4, 0, 0, 32, 10, 35, true, "MPB6B1L.BIN"},
	{4, 32, 0, 32, 10, 35, true, "MPB6B1R.BIN"},
	{5, 0, 0, 32, 12, 471, false, "MPB6_B8.BIN"},
	{5, 8, 0, 32, 12, 471, false, "MPB6_B8.BIN"},
};

static const SpecialTileInfo specialStage7Tiles[] = {
	{1, 0, 0, 40, 16, 41, false, "EGG.BIN"},
	{2, 0, 0, 32, 12, 35, false, "MPB7_B1.BIN"},
	{2, 8, 0, 32, 12, 35, false, "MPB7_B1.BIN"},
};

static const SpecialTileSet specialStageTiles[] = {
	{specialStage0Tiles, SDL_arraysize(specialStage0Tiles)},
	{specialStage1Tiles, SDL_arraysize(specialStage1Tiles)},
	{specialStage2Tiles, SDL_arraysize(specialStage2Tiles)},
	{specialStage3Tiles, SDL_arraysize(specialStage3Tiles)},
	{specialStage4Tiles, SDL_arraysize(specialStage4Tiles)},
	{specialStage5Tiles, SDL_arraysize(specialStage5Tiles)},
	{specialStage6Tiles, SDL_arraysize(specialStage6Tiles)},
	{specialStage7Tiles, SDL_arraysize(specialStage7Tiles)},
};

static const SpecialTileInfo specialClearTiles[] = {
	{1, 0, 0, 40, 28, 687, true, "CLRBACK.BIN"},
};

typedef struct {
	Sint16 x;
	Sint16 y;
	Uint16 index;
	Uint16 reverse;
} Sprite;

#define MAX_SPRITES 256
static Sprite sprites[MAX_SPRITES];

static Sint16 LoadSint16LE(Uint8 *data) {
	Sint16 out = data[0];
	out |= (data[1] << 8);
	return out;
}

static Sint32 LoadSint32LE(Uint8 *data) {
	Sint32 out = data[0];
	out |= (data[1] << 8);
	out |= (data[2] << 16);
	out |= (data[3] << 24);
	return out;
}

static void LoadAssetHeader(Uint8 *data, AssetHeader *out) {
	memcpy(out->type, data, sizeof(out->type)); data += sizeof(out->type);
	out->endOffset = LoadSint32LE(data); data += sizeof(Sint32);
	out->count = LoadSint32LE(data); data += sizeof(Sint32);
	out->dataOffset = LoadSint32LE(data);
}

static void LoadSpriteHeader(Uint8 *data, SpriteHeader *out) {
	out->x       = LoadSint16LE(data); data += sizeof(Sint16);
	out->y       = LoadSint16LE(data); data += sizeof(Sint16);
	out->width   = LoadSint16LE(data); data += sizeof(Sint16);
	out->paddedWidth = (out->width + 7) & ~7U;
	out->height  = LoadSint16LE(data); data += sizeof(Sint16);
	out->palette = LoadSint16LE(data) - 16; data += sizeof(Sint16);
	out->type    = LoadSint16LE(data); data += sizeof(Sint16);
}

static std::vector<Uint8> Convert4bpp(Uint8 *data, Sint32 width, Sint32 paddedWidth, Sint32 height, Sint32 palette) {
	std::vector<Uint8> out;
	out.reserve(width * height);
	int outCursor = 0;
	Uint8 pixel, pixels;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < paddedWidth; x++) {
			if ((x & 1) == 0) {
				pixels = *data++;
				pixel = pixels >> 4;
			}
			else {
				pixel = pixels & 0xf;
			}
			if (x < width) {
				out.push_back(pixel ? pixel + palette : 0);
			}
		}
	}
	return out;
}

static std::vector<Uint8> RotatePixelsRight(const std::vector<Uint8> &pixels, Sint32 width, Sint32 height) {
	std::vector<Uint8> out(width * height);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			out.at(y * width + x) = pixels.at((height - 1 - x) * width + y);
		}
	}
	return out;
}

static void DestroySpriteBitmaps(void) {
	spriteBitmaps.clear();
}

static void DestroyTileBitmaps(void) {
	tileBitmaps.clear();
}

static void DestroyChangeTileBitmaps(void) {
	changeTileBitmaps.clear();
}

static void DestroyRotateTileBitmaps(void) {
	rotateTileBitmaps.clear();
	rotateTileLookup.clear();
}

static int PositiveMod(int value, int divisor) {
	value %= divisor;
	if (value < 0) {
		value += divisor;
	}
	return value;
}

static Uint16 GetRotateTileVariant(Uint16 block);
static void UpdatePalette(void);
static void PresentFrame(SDL_Renderer *renderer);

static Uint16 GetTileVariant(Uint16 block, Uint16 frip) {
	Uint16 index = block & 0x7ff;
	if (!index) {
		return 0;
	}
	frip = (frip ^ block) & 0x1800;
	if (frip == 0) {
		return index * 4;
	}
	if (frip == 0x1800) {
		return index * 4 + 3;
	}
	if (frip & 0x1000) {
		return index * 4 + 2;
	}
	return index * 4 + 1;
}

static Uint16 GetSpecialTileVariant(Uint16 block) {
	Uint16 index = block & 0x7ff;
	if (!index) {
		return 0;
	}
	return index * 4;
}

static void SetPlanetRotateGridTile(Sint32 x, Sint32 y, Sint32 block) {
	if (x < 0 || x >= PLANET_ROTATE_GRID_W || y < 0 || y >= PLANET_ROTATE_GRID_H) {
		return;
	}
	planetRotateGrid[y][x] = GetRotateTileVariant((Uint16)block);
}

Sint32 SetGrid(Sint32 base, Sint32 x, Sint32 y, Sint32 block, Sint32 frip) {
	if (planetMode && base < 0) {
		SetPlanetRotateGridTile(x, y, block);
		return 0;
	}
	if (specialStageMode) {
		Uint16 tile = GetSpecialTileVariant((Uint16)block);
		x = PositiveMod(x, STAGE_GRID_W);
		y = PositiveMod(y, STAGE_GRID_H);
		specialTileGrids[0][y][x] = tile;
		return 0;
	}

	Uint16 tile = GetTileVariant((Uint16)block, (Uint16)frip);
	x = PositiveMod(x, STAGE_GRID_W);
	y = PositiveMod(y, STAGE_GRID_H);
	if (base != 0) {
		tileGrids[2][y][x] = tile;
	} else if (!tile) {
		tileGrids[0][y][x] = 0;
		tileGrids[1][y][x] = 0;
	} else if (block & 0x8000) {
		tileGrids[0][y][x] = tile;
		tileGrids[1][y][x] = 0;
	} else {
		tileGrids[0][y][x] = 0;
		tileGrids[1][y][x] = tile;
	}
	return 0;
}

void EAsprset(Sint16 x, Sint16 y, Uint16 index, Uint16 linkdata, Uint16 reverse) {
	if (linkdata >= MAX_SPRITES) {
		return;
	}
	/* Decompiled modules can hand us an index past the sprite bank (e.g. a
	   stale work-area value); drawing it would read outside spriteBitmaps. */
	if (index && index >= spriteBitmaps.size()) {
		static int reported;
		if (!reported) {
			reported = 1;
			SDL_Log("EAsprset: sprite index %u out of range (%zu loaded), skipping",
			        (unsigned)index, spriteBitmaps.size());
		}
		return;
	}
	sprites[linkdata].x = x;
	sprites[linkdata].y = y;
	sprites[linkdata].index = index;
	sprites[linkdata].reverse = reverse;
}

void ClrSpriteDebug(void) {
	memset(sprites, 0, sizeof(sprites));
}

void ChangeTileBmp(Sint32 tile_start, Sint32 bmp_no) {
	changeTiles.at(tile_start) = bmp_no;
}

void Graphics_SetFullscreen(int enable) { launchFullscreen = enable != 0; }

/* Fullscreen is desktop (borderless) fullscreen rather than a mode switch: the
   320x224 picture is scaled up and letterboxed by the logical presentation, so
   the desktop keeps its resolution.  A failure is logged and the game stays in
   its window. */
static void EnterFullscreen(SDL_Window *window) {

	if (!SDL_SetWindowFullscreenMode(window, NULL) ||
	    !SDL_SetWindowFullscreen(window, true)) {
		SDL_Log("Couldn't switch to fullscreen: %s", SDL_GetError());
	}
}

int Graphics_Init(SDL_Window **window, SDL_Renderer **renderer) {
	if (!SDL_CreateWindowAndRenderer("Sonic CD", SCREEN_WIDTH * 3, SCREEN_HEIGHT * 3, 0, window, renderer)) {
		SDL_Log("Failed to create window and renderer: %s", SDL_GetError());
		return 0;
	}
	SDL_SetRenderLogicalPresentation(*renderer, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
	SDL_SetRenderVSync(*renderer, 1);
	if (launchFullscreen) {
		EnterFullscreen(*window);
	}

	sharedPalette = SDL_CreatePalette(256);
	if (!sharedPalette) {
		SDL_Log("Couldn't create shared palette: %s", SDL_GetError());
		return 0;
	}

	framebuffer = SDL_CreateSurface(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_PIXELFORMAT_INDEX8);
	if (!framebuffer) {
		SDL_Log("Couldn't create framebuffer surface: %s", SDL_GetError());
		return 0;
	}
	SDL_SetSurfacePalette(framebuffer, sharedPalette);

	presentSurface = SDL_CreateSurface(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_PIXELFORMAT_ARGB8888);
	if (!presentSurface) {
		SDL_Log("Couldn't create presentation surface: %s", SDL_GetError());
		return 0;
	}
	SDL_SetSurfaceBlendMode(presentSurface, SDL_BLENDMODE_NONE);

	presentTexture = SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (!presentTexture) {
		SDL_Log("Couldn't create presentation texture: %s", SDL_GetError());
		return 0;
	}
	SDL_SetTextureScaleMode(presentTexture, SDL_SCALEMODE_PIXELART);
	Hmx_Init(framebuffer, *renderer, UpdatePalette, PresentFrame);
	return 1;
}

int Graphics_LoadSprites(const char *path, bmp_info *spriteInfo, int spriteInfoCount) {
	AssetHeader ah;
	SpriteHeader sh;
	auto data = SZDD_Decompress(path);
	if (data.empty()) {
		return 0;
	}
	LoadAssetHeader(data.data(), &ah);
	spriteBitmaps.clear();
	memset(spriteInfo, 0, spriteInfoCount * sizeof(*spriteInfo));
	Uint8 *spriteHeaders = data.data() + ASSET_HEADER_ENCODED_LEN;
	Uint8 *spriteGraphics = data.data() + ah.dataOffset;
	spriteBitmaps.reserve(ah.count);
	for (int i = 0; i < ah.count; i++) {
		LoadSpriteHeader(spriteHeaders, &sh);
		if (i < spriteInfoCount) {
			spriteInfo[i].xs = (Uint8)sh.width;
			spriteInfo[i].ys = (Uint8)sh.height;
			spriteInfo[i].ofs = 0;
		}
		auto pixels = Convert4bpp(spriteGraphics, sh.width, sh.paddedWidth, sh.height, sh.palette);
		BitmapData bitmap(sh.width, sh.height, pixels);
		spriteBitmaps.emplace_back(sh.width, sh.height, pixels);
		spriteHeaders += SPRITE_HEADER_ENCODED_LEN;
		spriteGraphics += ((sh.paddedWidth / 2) * sh.height);
	}
	return 1;
}

static int LoadTileSet(const char *path, std::vector<BitmapData> &outBitmaps) {
	AssetHeader ah;
	auto data = SZDD_Decompress(path);
	if (data.empty()) {
		return 0;
	}
	LoadAssetHeader(data.data(), &ah);
	outBitmaps.clear();

	Uint8 *metadata = data.data() + ASSET_HEADER_ENCODED_LEN;
	int paletteCounts[4];
	for (int i = 0; i < 4; i++) {
		paletteCounts[i] = LoadSint16LE(metadata);
		metadata += sizeof(Sint16);
	}
	int palette = 0;
	Uint8 *dimensions = metadata;
	Uint8 *graphics = data.data() + ah.dataOffset;
	for (int i = 0; i < ah.count; i++) {
		while (paletteCounts[palette] == 0) {
			palette++;
			if (palette >= 4) {
				palette = 3;
				break;
			}
		}
		paletteCounts[palette]--;

		Sint16 width = LoadSint16LE(dimensions); dimensions += sizeof(Sint16);
		Sint16 height = LoadSint16LE(dimensions); dimensions += sizeof(Sint16);
		auto pixels = Convert4bpp(graphics, width, width, height, palette * 16);
		outBitmaps.emplace_back(width, height, pixels);
		graphics += width * height / 2;
	}

	return ah.count;
}

static int LoadRotateTileSet(const char *path) {
	AssetHeader ah;
	auto data = SZDD_Decompress(path);
	if (data.empty()) {
		return 0;
	}
	LoadAssetHeader(data.data(), &ah);
	DestroyRotateTileBitmaps();

	Uint8 *metadata = data.data() + ASSET_HEADER_ENCODED_LEN;
	int paletteCounts[4];
	for (int i = 0; i < 4; i++) {
		paletteCounts[i] = LoadSint16LE(metadata);
		metadata += sizeof(Sint16);
	}
	int palette = 0;
	Uint8 *dimensions = metadata;
	Uint8 *graphics = data.data() + ah.dataOffset;
	for (int i = 0; i < ah.count; i++) {
		while (paletteCounts[palette] == 0) {
			palette++;
			if (palette >= 4) {
				palette = 3;
				break;
			}
		}
		paletteCounts[palette]--;

		Sint16 width = LoadSint16LE(dimensions); dimensions += sizeof(Sint16);
		Sint16 height = LoadSint16LE(dimensions); dimensions += sizeof(Sint16);
		auto pixels = Convert4bpp(graphics, width, width, height, palette * 16);
		auto rotatedPixels = RotatePixelsRight(pixels, width, height);
		rotateTileBitmaps.emplace_back(width, height, pixels);
		rotateTileBitmaps.emplace_back(width, height, rotatedPixels);
		graphics += width * height / 2;
	}

	rotateTileLookup.reserve(rotateTileBitmaps.size() * 4);
	for (Uint16 i = 0; i < rotateTileBitmaps.size() * 4; i++) {
		rotateTileLookup.push_back(i / 4);
	}
	return static_cast<int>(rotateTileBitmaps.size());
}

static Uint16 GetRotateTileVariant(Uint16 block) {
	Uint16 low = block & 0x7ff;
	Sint32 tileIndex = (low / 4) - 1;
	Uint16 variant;
	if (!low) {
		return ROTATE_TILE_EMPTY;
	}
	if (tileIndex < 0 || static_cast<size_t>(tileIndex) >= rotateTileBitmaps.size() / 2) {
		return ROTATE_TILE_EMPTY;
	}
	switch (block & 0xe000) {
	case 0x0000:
		variant = 0;
		break;
	case 0x8000:
		variant = 1;
		break;
	case 0xc000:
		variant = 2;
		break;
	case 0x4000:
		variant = 3;
		break;
	case 0x6000:
		variant = 4;
		break;
	case 0xa000:
		variant = 6;
		break;
	case 0xe000:
		variant = 5;
		break;
	case 0x2000:
	default:
		variant = 7;
		break;
	}
	if (static_cast<size_t>(tileIndex * 8 + variant) >= rotateTileLookup.size()) {
		return ROTATE_TILE_EMPTY;
	}
	return (Uint16)(tileIndex * 8 + variant);
}

static void Graphics_SetRotateGrid(const Uint16 *map) {
	if (!map) {
		for (int y = 0; y < 128; y++) {
			for (int x = 0; x < 128; x++) {
				rotateGrid[y][x] = ROTATE_TILE_EMPTY;
			}
		}
		return;
	}
	for (int y = 0; y < 128; y++) {
		for (int x = 0; x < 128; x++) {
			rotateGrid[y][x] = GetRotateTileVariant(map[y * 128 + x]);
		}
	}
}

static void Graphics_SetPlanetRotateGrid(const Uint16 *map) {
	for (int y = 0; y < PLANET_ROTATE_GRID_H; y++) {
		for (int x = 0; x < PLANET_ROTATE_GRID_W; x++) {
			planetRotateGrid[y][x] = ROTATE_TILE_EMPTY;
		}
	}
	if (!map) {
		return;
	}
	for (int y = 0; y < 7; y++) {
		for (int x = 0; x < PLANET_ROTATE_GRID_W; x++) {
			planetRotateGrid[y][x] = GetRotateTileVariant(map[y * PLANET_ROTATE_GRID_W + x]);
		}
	}
}

static Uint16 GetTileHandle(Sint32 tileIndex, Uint16 flags) {
	flags &= 0x1800;
	if (tileIndex <= 0 || static_cast<size_t>(tileIndex) >= tileBitmaps.size()) {
		return 0;
	}
	if (flags == 0) {
		return (Uint16)(tileIndex * 4);
	}
	if (flags == 0x1800) {
		return (Uint16)(tileIndex * 4 + 3);
	}
	if (flags & 0x1000) {
		return (Uint16)(tileIndex * 4 + 2);
	}
	return (Uint16)(tileIndex * 4 + 1);
}

static int LoadSpecialGridBin(const SpecialTileInfo *info) {
	std::string path = std::format("../SPECIAL/BMP/FIX/{}", info->fileName);
	Uint8 encoded[2];
	FILE *file = File_Open(path.c_str(), "rb");
	if (!file) {
		SDL_Log("couldn't open %s", path);
		return 0;
	}

	for (int y = 0; y < info->height; y++) {
		for (int x = 0; x < info->width; x++) {
			Uint16 block;
			Uint16 low;
			Uint16 tile = 0;
			int gridX = info->x + x;
			int gridY = info->y + y;
			if (fread(encoded, 1, sizeof(encoded), file) != sizeof(encoded)) {
				fclose(file);
				return 0;
			}
			block = (Uint16)(encoded[0] | (encoded[1] << 8));
			low = block & 0x7ff;
			if (low) {
				tile = GetTileHandle(info->tileOffset + low, block);
			} else if (!info->clearZero) {
				tile = GetTileHandle(info->tileOffset, 0);
			}
			if (info->grid < SDL_arraysize(specialTileGrids) &&
			    gridX >= 0 && gridX < STAGE_GRID_W &&
			    gridY >= 0 && gridY < STAGE_GRID_H) {
				specialTileGrids[info->grid][gridY][gridX] = tile;
			}
		}
	}
	fclose(file);
	return 1;
}

static int Graphics_LoadSpecialGrids(int stageNumber) {
	static const SpecialTileInfo panel2 = {0, 0, 0, 40, 2, 0, false, "PANEL2.BIN"};
	const SpecialTileSet *tileSet;
	memset(specialTileGrids, 0, sizeof(specialTileGrids));
	if (!LoadSpecialGridBin(&panel2)) {
		return 0;
	}
	if (stageNumber < 0 || stageNumber >= SDL_arraysize(specialStageTiles)) {
		return 1;
	}
	tileSet = &specialStageTiles[stageNumber];
	for (int i = 0; i < tileSet->count; i++) {
		if (!LoadSpecialGridBin(&tileSet->tiles[i])) {
			return 0;
		}
	}
	return 1;
}

static int Graphics_LoadSpecialClearGrids(void) {
	memset(specialTileGrids, 0, sizeof(specialTileGrids));
	for (int i = 0; i < SDL_arraysize(specialClearTiles); i++) {
		if (!LoadSpecialGridBin(&specialClearTiles[i])) {
			return 0;
		}
	}
	return 1;
}

int Graphics_LoadTiles(const char *path) {
	DestroyTileBitmaps();
	memset(tileGrids, 0, sizeof(tileGrids));
	LoadTileSet(path, tileBitmaps);
	changeTiles.clear();
	changeTiles.reserve(tileBitmaps.size());
	for (size_t i = 0; i < tileBitmaps.size(); i++) {
		changeTiles.push_back(-1);
	}
	return static_cast<int>(tileBitmaps.size());
}

void Graphics_BeginRound(void) {
	planetMode = false;
	specialStageMode = false;
	specialClearMode = false;
	Hmx_SetActive(0);
	ClrSpriteDebug();
}

int Graphics_LoadChangeTiles(const char *path) {
	DestroyChangeTileBitmaps();
	LoadTileSet(path, changeTileBitmaps);
	return static_cast<int>(changeTileBitmaps.size());
}

static int Graphics_LoadWarpGridRows(int dstY, int rows) {
	Uint8 encoded[2];
	FILE *file = File_Open("../WARP/CGWARP.BIN", "rb");
	if (!file) {
		SDL_Log("couldn't open WARP/CGWARP.BIN");
		return 0;
	}

	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < 40; x++) {
			Uint16 block;
			Uint16 low;
			Uint16 tile = 0;
			if (fread(encoded, 1, sizeof(encoded), file) != sizeof(encoded)) {
				fclose(file);
				return 0;
			}
			block = (Uint16)(encoded[0] | (encoded[1] << 8));
			low = block & 0x7ff;
			if (low) {
				tile = GetTileHandle(low, block);
			}
			tileGrids[1][dstY + y][x] = tile;
		}
	}
	fclose(file);
	return 1;
}

static int Graphics_LoadWarpGrid(void) {
	return Graphics_LoadWarpGridRows(0, 28) && Graphics_LoadWarpGridRows(28, 4);
}

int Graphics_LoadWarp(bmp_info *spriteInfo, int spriteInfoCount) {
	planetMode = false;
	specialStageMode = false;
	specialClearMode = false;
	Hmx_SetActive(0);
	ClrSpriteDebug();
	if (!Graphics_LoadTiles("../WARP/TCMPWARP.CM_")) {
		return 0;
	}
	if (!Graphics_LoadWarpGrid()) {
		return 0;
	}
	if (!Graphics_LoadSprites("../WARP/SCMPWARP.CM_", spriteInfo, spriteInfoCount)) {
		return 0;
	}
	return 1;
}

int Graphics_LoadPlanet(const Uint16 *rotateMap, bmp_info *spriteInfo, int spriteInfoCount) {	planetMode = true;
	specialStageMode = false;
	specialClearMode = false;
	Hmx_SetActive(0);
	memset(tileGrids, 0, sizeof(tileGrids));
	ClrSpriteDebug();
	if (!Graphics_LoadTiles("../TITLE/PLANET/CG/LPBS.CM_")) {
		return 0;
	}
	if (!LoadRotateTileSet("../TITLE/PLANET/CG/PLANET.CM_")) {
		return 0;
	}
	Graphics_SetPlanetRotateGrid(rotateMap);
	if (!Graphics_LoadSprites("../TITLE/PLANET/CG/SCMPLP.CM_", spriteInfo, spriteInfoCount)) {
		return 0;
	}
	return 1;
}

/* The thanks / ending message screen (src/title/thanks) draws into the two
   plane A grids and sprites like a stage, but it has no background plane of its
   own. */
int Graphics_LoadThanks(bmp_info *spriteInfo, int spriteInfoCount) {
	planetMode = false;
	specialStageMode = false;
	specialClearMode = false;
	Hmx_SetActive(0);
	memset(tileGrids, 0, sizeof(tileGrids));
	ClrSpriteDebug();
	if (!Graphics_LoadTiles("../TITLE/THANKS/CG/SCR_A.CM_")) {
		return 0;
	}
	if (!Graphics_LoadSprites("../TITLE/THANKS/CG/SCMPTH.CM_", spriteInfo, spriteInfoCount)) {
		return 0;
	}
	return 1;
}

int Graphics_LoadSpecialStage(int stageNumber, const Uint16 *rotateMap, bmp_info *spriteInfo, int spriteInfoCount) {
	planetMode = false;
	specialStageMode = true;
	specialClearMode = false;
	Hmx_SetActive(0);
	specialStageIndex = stageNumber;
	std::string fixedPath = std::format("../SPECIAL/BMP/FIX/TCMPS{}.CM_", stageNumber + 1);
	std::string rotatePath = std::format("../SPECIAL/CG/SP{}CG32.CM_", stageNumber + 1);
	if (!Graphics_LoadTiles(fixedPath.c_str())) {
		return 0;
	}
	if (!Graphics_LoadSpecialGrids(stageNumber)) {
		return 0;
	}
	if (!LoadRotateTileSet(rotatePath.c_str())) {
		return 0;
	}
	Graphics_SetRotateGrid(rotateMap);
	if (!Graphics_LoadSprites("../SPECIAL/SCMPSPE.CM_", spriteInfo, spriteInfoCount)) {
		return 0;
	}
	return 1;
}

int Graphics_LoadSpecialClearScreen(void) {
	planetMode = false;
	specialStageMode = true;
	specialClearMode = true;
	Hmx_SetActive(0);
	ClrSpriteDebug();
	if (!Graphics_LoadTiles("../SPECIAL/BMP/FIX/SCLEAR.CM_")) {
		return 0;
	}
	if (!Graphics_LoadSpecialClearGrids()) {
		return 0;
	}
	DestroySpriteBitmaps();
	DestroyRotateTileBitmaps();
	return 1;
}

void Graphics_UpdateSpecialGroundTiles(Uint16 hane1, Uint16 hane2, Uint16 dmg1, Uint16 dmg2) {
	Uint16 haneDest = hane1 * 8;
	Uint16 haneSrc = hane2 * 2;
	Uint16 dmgDest = dmg1 * 8;
	Uint16 dmgSrc = dmg2 * 2;

	for (int i = 0; i < 8; i++) {
		if (i == 4) {
			haneSrc++;
			dmgSrc++;
		}
		if (haneDest < rotateTileLookup.size() && haneSrc < rotateTileBitmaps.size()) {
			rotateTileLookup.at(haneDest) = haneSrc;
		}
		if (dmgDest < rotateTileLookup.size() && dmgSrc < rotateTileBitmaps.size()) {
			rotateTileLookup.at(dmgDest) = (dmgSrc);
		}
		haneDest++;
		dmgDest++;
	}
}

void Graphics_Shutdown(void) {
	DestroySpriteBitmaps();
	DestroyTileBitmaps();
	DestroyChangeTileBitmaps();
	DestroyRotateTileBitmaps();
	Hmx_Shutdown();
	SDL_DestroySurface(framebuffer);
	framebuffer = NULL;
	SDL_DestroySurface(presentSurface);
	presentSurface = NULL;
	SDL_DestroyTexture(presentTexture);
	presentTexture = NULL;
	SDL_DestroyPalette(sharedPalette);
	sharedPalette = NULL;
}

void Graphics_TitleSnapshotUnfadedPalette(void) {
	/* Called right after a title module has been initialised, while its
	   palette lines are the ones it set up for itself.  The opening keeps
	   Sonic's colours (its fourth line) out of reach of the intro flash this
	   way; see titleUnfadedPalette above. */
	memcpy(titleUnfadedPalette, &colorwk[48], sizeof(titleUnfadedPalette));
}

static void UpdatePalette(void) {
	static PALETTEENTRY lastColorwk[64];
	static PALETTEENTRY lastColorwk3[64];
	static PALETTEENTRY lastTitleUnfaded[16];
	static int paletteInitialized;
	static bool lastSpecialStageMode;
	static bool lastTitleMode;
	bool titleMode = Hmx_IsActive() != 0;
	SDL_Color colors[256];
	if (paletteInitialized &&
	    lastSpecialStageMode == specialStageMode &&
	    lastTitleMode == titleMode &&
	    memcmp(colorwk, lastColorwk, sizeof(colorwk)) == 0 &&
	    memcmp(colorwk3, lastColorwk3, sizeof(colorwk3)) == 0 &&
	    memcmp(titleUnfadedPalette, lastTitleUnfaded,
	           sizeof(titleUnfadedPalette)) == 0) {
		return;
	}
	memcpy(lastColorwk, colorwk, sizeof(colorwk));
	memcpy(lastColorwk3, colorwk3, sizeof(colorwk3));
	memcpy(lastTitleUnfaded, titleUnfadedPalette, sizeof(titleUnfadedPalette));
	lastSpecialStageMode = specialStageMode;
	lastTitleMode = titleMode;
	for (int i = 0; i < SDL_arraysize(colors); i++) {
		colors[i].r = 0;
		colors[i].g = 0;
		colors[i].b = 0;
		colors[i].a = 255;
	}
	if (titleMode) {
		for (int i = 0; i < SDL_arraysize(colorwk); i++) {
			colors[16 + i].r = colorwk[i].peRed;
			colors[16 + i].g = colorwk[i].peGreen;
			colors[16 + i].b = colorwk[i].peBlue;
			colors[16 + i].a = 255;

			colors[80 + i].r = colorwk[i].peRed;
			colors[80 + i].g = colorwk[i].peGreen;
			colors[80 + i].b = colorwk[i].peBlue;
			colors[80 + i].a = 255;
		}
		/* The opening draws Sonic from copies of his sprites whose
		   colours are kept here while the flash rewrites line 3 (see
		   titleUnfadedPalette above). */
		for (int i = 0; i < SDL_arraysize(titleUnfadedPalette); i++) {
			colors[144 + i].r = titleUnfadedPalette[i].peRed;
			colors[144 + i].g = titleUnfadedPalette[i].peGreen;
			colors[144 + i].b = titleUnfadedPalette[i].peBlue;
			colors[144 + i].a = 255;
		}
	} else {
		for (int i = 0; i < SDL_arraysize(colorwk); i++) {
			colors[i].r = colorwk[i].peRed;
			colors[i].g = colorwk[i].peGreen;
			colors[i].b = colorwk[i].peBlue;
			colors[i].a = 255;

			colors[64 + i].r = colorwk3[i].peRed;
			colors[64 + i].g = colorwk3[i].peGreen;
			colors[64 + i].b = colorwk3[i].peBlue;
			colors[64 + i].a = 255;
		}
	}
	SDL_SetPaletteColors(sharedPalette, colors, 0, SDL_arraysize(colors));
	paletteInitialized = 1;
}

static void ApplyR4WaterPaletteSplit(const game_info *info) {
	Uint16 splitY;
	if (!info || info->stageno.b.h != 2) {
		return;
	}
	if (info->waterflag) {
		splitY = 0;
	} else {
		splitY = (Uint16)info->hintposi.w;
		if (splitY >= SCREEN_HEIGHT - 1) {
			return;
		}
	}
	if (!SDL_LockSurface(framebuffer)) {
		SDL_Log("couldn't lock framebuffer for R4 water palette split: %s", SDL_GetError());
		return;
	}
	for (int y = splitY; y < SCREEN_HEIGHT; y++) {
		Uint8 *row = (Uint8 *)framebuffer->pixels + y * framebuffer->pitch;
		for (int x = 0; x < SCREEN_WIDTH; x++) {
			if (row[x] < 64) {
				row[x] += 64;
			}
		}
	}
	SDL_UnlockSurface(framebuffer);
}

static void DrawSprites(SDL_Surface *target, bool highPriority) {
	for (int i = MAX_SPRITES - 1; i >= 0; --i) {
		Sprite *sprite = &sprites[i];
		BitmapData *bitmap;
		SDL_Surface *surface;
		SDL_Rect rect;
		if (!sprite->index) {
			continue;
		}
		if ((!!(sprite->reverse & 0x8000)) != highPriority) {
			continue;
		}
		if (sprite->index >= spriteBitmaps.size()) {
			continue;
		}
		bitmap = &spriteBitmaps[sprite->index];
		surface = bitmap->surfaces[sprite->reverse & 3];
		if (!surface) {
			continue;
		}
		rect.x = sprite->x - 128;
		rect.y = sprite->y - 128;
		rect.w = bitmap->width;
		rect.h = bitmap->height;
		SDL_BlitSurface(surface, NULL, target, &rect);
	}
}

static void BlitTile(SDL_Surface *target, Uint16 tile, const SDL_Rect *src, const SDL_Rect *dst) {
	int tileIndex = tile / 4;
	int flip = tile & 3;
	int changeIndex = changeTiles.at(tileIndex);
	BitmapData &bitmap = changeIndex >= 0 ? changeTileBitmaps.at(changeIndex) : tileBitmaps.at(tileIndex);
	SDL_Surface *surface = bitmap.surfaces[flip];
	if (!surface) {
		return;
	}
	SDL_BlitSurface(surface, src, target, dst);
}

static void DrawGridPlane(SDL_Surface *target, int plane, int viewX, int viewY) {
	int firstTileX = viewX / TILE_SIZE;
	int firstTileY = viewY / TILE_SIZE;
	int offsetX = viewX % TILE_SIZE;
	int offsetY = viewY % TILE_SIZE;
	for (int screenY = -offsetY, tileY = 0; screenY < SCREEN_HEIGHT; screenY += TILE_SIZE, tileY++) {
		int gridY = PositiveMod(firstTileY + tileY, STAGE_GRID_H);
		for (int screenX = -offsetX, tileX = 0; screenX < SCREEN_WIDTH; screenX += TILE_SIZE, tileX++) {
			int gridX = PositiveMod(firstTileX + tileX, STAGE_GRID_W);
			SDL_Rect dst = {screenX, screenY, TILE_SIZE, TILE_SIZE};
			BlitTile(target, tileGrids[plane][gridY][gridX], NULL, &dst);
		}
	}
}

static void DrawGridPlaneSheared(SDL_Surface *target, int plane, int viewY) {
	for (int screenY = 0; screenY < SCREEN_HEIGHT; screenY++) {
		int viewX = (-hscrollbuff[screenY].w.l) % (STAGE_GRID_W * TILE_SIZE);
		int worldY = viewY + screenY;
		int gridY = PositiveMod(worldY / TILE_SIZE, STAGE_GRID_H);
		int srcY = PositiveMod(worldY, TILE_SIZE);
		int firstTileX = viewX / TILE_SIZE;
		int offsetX = viewX % TILE_SIZE;
		for (int screenX = -offsetX, tileX = 0; screenX < SCREEN_WIDTH; screenX += TILE_SIZE, tileX++) {
			int gridX = PositiveMod(firstTileX + tileX, STAGE_GRID_W);
			SDL_Rect src = {0, srcY, TILE_SIZE, 1};
			SDL_Rect dst = {screenX, screenY, TILE_SIZE, 1};
			BlitTile(target, tileGrids[plane][gridY][gridX], &src, &dst);
		}
	}
}

static int GetSpecialGridScroll(int hscrollIndex, int pixelWidth) {
	if (hscrollIndex < 0 || hscrollIndex >= SDL_arraysize(hscrollbuff) || pixelWidth <= 0) {
		return 0;
	}
	return PositiveMod(-hscrollbuff[hscrollIndex].w.h, pixelWidth);
}

static int GetSpecialGridViewX(int grid, int pixelWidth) {
	if (specialStageIndex == 0 || specialStageIndex == 4 || specialStageIndex == 5) {
		if (grid == 1) {
			return PositiveMod(-hscrollbuff[32].w.l, pixelWidth);
		}
		if (grid == 2 && specialStageIndex == 0) {
			return PositiveMod(-hscrollbuff[96].w.l, pixelWidth);
		}
	}
	switch (specialStageIndex) {
	case 1:
		if (grid == 1) {
			return GetSpecialGridScroll(0, pixelWidth);
		}
		if (grid == 2) {
			return GetSpecialGridScroll(1, pixelWidth);
		}
		break;
	case 2:
		if (grid == 1) {
			return GetSpecialGridScroll(0, pixelWidth);
		}
		if (grid >= 3 && grid <= 6) {
			return GetSpecialGridScroll(grid - 2, pixelWidth);
		}
		break;
	case 3:
		if (grid == 1) {
			return GetSpecialGridScroll(2, pixelWidth);
		}
		if (grid == 2) {
			return GetSpecialGridScroll(0, pixelWidth);
		}
		if (grid == 3) {
			return GetSpecialGridScroll(1, pixelWidth);
		}
		break;
	case 6:
		if (grid == 1) {
			return GetSpecialGridScroll(2, pixelWidth);
		}
		if (grid == 2) {
			return GetSpecialGridScroll(3, pixelWidth);
		}
		if (grid == 3) {
			return GetSpecialGridScroll(0, pixelWidth);
		}
		if (grid == 4) {
			return GetSpecialGridScroll(1, pixelWidth);
		}
		break;
	default:
		break;
	}
	return 0;
}

static bool IsSpecialGridSheared(int grid) {
	return !specialClearMode && grid == 3 && (specialStageIndex == 0 || specialStageIndex == 4 || specialStageIndex == 5);
}

static int GetSpecialGridLineViewX(int screenY) {
	Sint16 lineScroll;
	if (screenY < 0 || screenY >= 128) {
		return 0;
	}
	lineScroll = hscrollbuff[screenY].w.h;
	if (specialStageIndex == 4) {
		lineScroll += 32;
	}
	return (-lineScroll) & 0x1ff;
}

static void DrawSpecialGrid(SDL_Surface *target, int grid) {
	const SpecialGridInfo *info;
	int viewX;
	int pixelWidth;
	int pixelHeight;
	if (grid < 0 || grid >= SDL_arraysize(specialTileGrids)) {
		return;
	}
	if (specialClearMode) {
		if (grid >= SDL_arraysize(specialClearGridInfo)) {
			return;
		}
		info = &specialClearGridInfo[grid];
	} else {
		if (specialStageIndex < 0 || specialStageIndex >= SDL_arraysize(specialGridInfo)) {
			return;
		}
		info = &specialGridInfo[specialStageIndex][grid];
	}
	if (!info->width || !info->height) {
		return;
	}
	pixelWidth = info->width * TILE_SIZE;
	pixelHeight = info->height * TILE_SIZE;
	viewX = specialClearMode ? 0 : GetSpecialGridViewX(grid, pixelWidth);
	if (IsSpecialGridSheared(grid)) {
		for (int screenY = 0; screenY < pixelHeight; screenY++) {
			int dstY = info->pos * TILE_SIZE + screenY;
			int tileY = screenY / TILE_SIZE;
			int srcY = screenY % TILE_SIZE;
			if (dstY < 0 || dstY >= SCREEN_HEIGHT || tileY >= STAGE_GRID_H) {
				continue;
			}
			viewX = GetSpecialGridLineViewX(screenY);
			for (int screenX = 0; screenX < SCREEN_WIDTH; screenX += TILE_SIZE) {
				int srcX = PositiveMod(viewX + screenX, pixelWidth);
				int tileX = srcX / TILE_SIZE;
				int tilePixelX = srcX % TILE_SIZE;
				if (tileX >= STAGE_GRID_W) {
					continue;
				}
				SDL_Rect src = {tilePixelX, srcY, TILE_SIZE - tilePixelX, 1};
				SDL_Rect dst = {screenX, dstY, src.w, 1};
				BlitTile(target, specialTileGrids[grid][tileY][tileX], &src, &dst);
				if (src.w < TILE_SIZE) {
					int nextTileX = (tileX + 1) % info->width;
					SDL_Rect src2 = {0, srcY, TILE_SIZE - src.w, 1};
					SDL_Rect dst2 = {screenX + src.w, dstY, src2.w, 1};
					BlitTile(target, specialTileGrids[grid][tileY][nextTileX], &src2, &dst2);
				}
			}
		}
		return;
	}
	for (int screenY = 0; screenY < pixelHeight; screenY += TILE_SIZE) {
		int dstY = info->pos * TILE_SIZE + screenY;
		int tileY = screenY / TILE_SIZE;
		if (dstY <= -TILE_SIZE || dstY >= SCREEN_HEIGHT || tileY >= STAGE_GRID_H) {
			continue;
		}
		for (int screenX = 0; screenX < SCREEN_WIDTH; screenX += TILE_SIZE) {
			int srcX = PositiveMod(viewX + screenX, pixelWidth);
			int tileX = srcX / TILE_SIZE;
			int tilePixelX = srcX % TILE_SIZE;
			if (tileX >= STAGE_GRID_W) {
				continue;
			}
			SDL_Rect src = {tilePixelX, 0, TILE_SIZE - tilePixelX, TILE_SIZE};
			SDL_Rect dst = {screenX, dstY, src.w, TILE_SIZE};
			BlitTile(target, specialTileGrids[grid][tileY][tileX], &src, &dst);
			if (src.w < TILE_SIZE) {
				int nextTileX = (tileX + 1) % info->width;
				SDL_Rect src2 = {0, 0, TILE_SIZE - src.w, TILE_SIZE};
				SDL_Rect dst2 = {screenX + src.w, dstY, src2.w, TILE_SIZE};
				BlitTile(target, specialTileGrids[grid][tileY][nextTileX], &src2, &dst2);
			}
		}
	}
}

static void DrawSpecialGrids(SDL_Surface *target) {
	int firstGrid = specialClearMode ? 2 : 7;
	for (int grid = firstGrid; grid >= 0; grid--) {
		DrawSpecialGrid(target, grid);
	}
}

static Uint8 SampleRotatePixel(Uint16 handle, int srcX, int srcY) {
	int variant = handle & 7;
	int bitmapIndex;
	int flip = variant & 3;
	if (handle == ROTATE_TILE_EMPTY) {
		return 0;
	}
	bitmapIndex = rotateTileLookup.at(handle);
	auto &bitmap = rotateTileBitmaps.at(bitmapIndex);
	auto &pixels = bitmap.pixels.at(flip);
	srcX = PositiveMod(srcX, bitmap.width);
	srcY = PositiveMod(srcY, bitmap.height);
	return pixels.at(srcY * bitmap.width + srcX);
}

static void DrawSpecialRotateGrid(SDL_Surface *target, const game_info *info) {
	Sint16 *tv;
	if (!info || !info->ptv_adr) {
		return;
	}
	tv = (Sint16 *)info->ptv_adr;
	if (!SDL_LockSurface(target)) {
		return;
	}
	for (int screenY = 128; screenY < SCREEN_HEIGHT; screenY++) {
		int tvRow = screenY - 128;
		Sint16 *row = tv + tvRow * 4;
		Sint32 srcXStart = row[0] * 8192;
		Sint32 srcYStart = row[1] * 8192;
		Sint32 srcXStep = (row[2] / 5) * 128;
		Sint32 srcYStep = (row[3] / 5) * 128;
		Uint8 *dst = (Uint8 *)target->pixels + screenY * target->pitch;
		for (int screenX = 0; screenX < SCREEN_WIDTH; screenX++) {
			Sint32 srcX = (srcXStart + srcXStep * screenX) >> 16;
			Sint32 srcY = (srcYStart + srcYStep * screenX) >> 16;
			int wrappedX = srcX & 0xfff;
			int wrappedY = srcY & 0xfff;
			Uint16 handle = rotateGrid[wrappedY / 32][wrappedX / 32];
			Uint8 pixel = SampleRotatePixel(handle, wrappedX & 31, wrappedY & 31);
			if (pixel) {
				dst[screenX] = pixel;
			}
		}
	}
	SDL_UnlockSurface(target);
}

static void DrawPlanetRotateGrid(SDL_Surface *target, const game_info *info) {
	Sint16 *tv;
	if (!info || !info->ptv_adr) {
		return;
	}
	tv = (Sint16 *)info->ptv_adr;
	if (!SDL_LockSurface(target)) {
		return;
	}
	for (int screenY = PLANET_ROTATE_SCREEN_Y; screenY < SCREEN_HEIGHT; screenY++) {
		int tvRow = screenY - PLANET_ROTATE_SCREEN_Y;
		Sint16 *row = tv + tvRow * 4;
		Sint32 srcXStart = row[0] * 8192;
		Sint32 srcYStart = row[1] * 8192;
		Sint32 srcXStep = row[2] * 32;
		Sint32 srcYStep = row[3] * 32;
		Uint8 *dst = (Uint8 *)target->pixels + screenY * target->pitch;
		for (int screenX = 0; screenX < SCREEN_WIDTH; screenX++) {
			Sint32 srcX = (srcXStart + srcXStep * screenX) >> 16;
			Sint32 srcY = (srcYStart + srcYStep * screenX) >> 16;
			Uint8 pixel;
			if (srcX < 0 || srcX >= PLANET_ROTATE_GRID_W * 32 ||
			    srcY < 0 || srcY >= PLANET_ROTATE_GRID_H * 32) {
				continue;
			}
			Uint16 handle = planetRotateGrid[srcY / 32][srcX / 32];
			pixel = SampleRotatePixel(handle, srcX & 31, srcY & 31);
			if (pixel) {
				dst[screenX] = pixel;
			}
		}
	}
	SDL_UnlockSurface(target);
}

static int lastFgViewX;
static int lastFgViewY;
static int lastBgViewX;
static int lastBgViewY;

/* Soft scanlines: every odd output row has its RGB scaled by three quarters,
   which reads as a soft darkening between lines rather than the hard black
   gaps of a CRT shader (the fork's apply_scanlines() does the same).  The
   filter lives in the present step, so it covers every screen the game draws -
   title, menus, stages, special stages, Time Attack - and nothing else.  It is
   applied to the copy of the frame that goes to the screen, which is also what
   FRAME_DUMP saves, so dumps show the filtered picture.  The FMV cutscenes
   present themselves through src/frontend/movie.c and are left unfiltered. */
static void ApplyScanlines(SDL_Surface *surface) {
	const int rowStride = surface->pitch / (int)sizeof(Uint32);
	Uint32 *row = (Uint32 *)surface->pixels;

	for (int y = 0; y < surface->h; ++y, row += rowStride) {
		if (!(y & 1)) {
			continue;
		}
		for (int x = 0; x < surface->w; ++x) {
			Uint32 pixel = row[x];
			Uint32 red = ((pixel >> 16) & 0xff) * 3 / 4;
			Uint32 green = ((pixel >> 8) & 0xff) * 3 / 4;
			Uint32 blue = (pixel & 0xff) * 3 / 4;
			row[x] = (pixel & 0xff000000) | (red << 16) | (green << 8) | blue;
		}
	}
}

void Graphics_SetScanlines(int enable) { scanlines = enable != 0; }

static void PresentFrame(SDL_Renderer *renderer) {
	/* While a movie plays it owns the screen; the game still runs and draws
	   into the framebuffer, but presenting it would stomp the movie frame. */
	if (Movie_IsPlaying()) {
		return;
	}
	SDL_BlitSurface(framebuffer, NULL, presentSurface, NULL);
	if (scanlines) {
		ApplyScanlines(presentSurface);
	}
	Uint32 *srcPixels = (Uint32 *)presentSurface->pixels;
	Uint32 *dstPixels;
	int pitch;
	if (!SDL_LockTexture(presentTexture, NULL, (void **)&dstPixels, &pitch)) {
		SDL_Log("couldn't lock texture: %s", SDL_GetError());
		return;
	}
	pitch /= sizeof(Uint32);
	for (int y = 0; y < presentSurface->h; y++) {
		memcpy(dstPixels, srcPixels, presentSurface->w * sizeof(Uint32));
		srcPixels += presentSurface->w;
		dstPixels += pitch;
	}
	SDL_UnlockTexture(presentTexture);
	/* Clear first: the texture only covers the letterboxed picture, so without
	   this whatever was in the rest of the window stays there - which showed as
	   a stale strip beside the game after a resize or when starting fullscreen
	   (the movie path clears, which is why a cutscene used to clean it up). */
	SDL_RenderClear(renderer);
	SDL_RenderTexture(renderer, presentTexture, NULL, NULL);
	SDL_RenderPresent(renderer);
	/* FRAME_DUMP=<frame number> saves that frame as /tmp/frame.bmp, and
	   FRAME_DUMP_EVERY=<n> saves every n-th frame as /tmp/frame_<n>.bmp -
	   handy for checking a screen from a headless run. */
	{
		static Uint32 frameCount;
		const char *dump = SDL_getenv("FRAME_DUMP");
		const char *every = SDL_getenv("FRAME_DUMP_EVERY");
		++frameCount;
		if (dump && frameCount == (Uint32)SDL_atoi(dump)) {
			SDL_SaveBMP(presentSurface, "/tmp/frame.bmp");
			SDL_Log("dumped frame %u", frameCount);
		}
		if (every && (Uint32)SDL_atoi(every) > 0 && frameCount % (Uint32)SDL_atoi(every) == 0) {
			char path[64];
			SDL_snprintf(path, sizeof(path), "/tmp/frame_%05u.bmp", frameCount);
			SDL_SaveBMP(presentSurface, path);
			/* GRID_DUMP=1 also writes the tile grids and the scroll offsets. */
			if (SDL_getenv("GRID_DUMP")) {
				SDL_snprintf(path, sizeof(path), "/tmp/grid_%05u.txt", frameCount);
				FILE *f = fopen(path, "w");
				if (f) {
					fprintf(f, "viewA %d %d viewB %d %d\n", lastFgViewX, lastFgViewY,
					        lastBgViewX, lastBgViewY);
					for (int plane = 0; plane < 3; ++plane) {
						fprintf(f, "plane %d\n", plane);
						for (int y = 0; y < STAGE_GRID_H; ++y) {
							for (int x = 0; x < STAGE_GRID_W; ++x) {
								fprintf(f, "%5u", (unsigned)tileGrids[plane][y][x]);
							}
							fputc('\n', f);
						}
					}
					fclose(f);
				}
			}
		}
	}
}

void Graphics_Draw(SDL_Renderer *renderer, Sint32 scraHPosiw, Sint32 scrbHPosiw, Sint32 vscroll, const game_info *info) {
	int fgViewX = (scraHPosiw >> 16) & 0x1ff;
	int bgViewX = (scrbHPosiw >> 16) & 0x1ff;
	lastFgViewX = fgViewX;
	lastBgViewX = bgViewX;
	int fgViewY = (vscroll >> 16) & 0xff;
	int bgViewY = vscroll & 0xff;
	lastFgViewY = fgViewY;
	lastBgViewY = bgViewY;
	UpdatePalette();
	SDL_FillSurfaceRect(framebuffer, NULL, 48);
	if (hscrollbuff[0].w.l || hscrollbuff[1].w.l || hscrollbuff[32].w.l || hscrollbuff[96].w.l) {
		DrawGridPlaneSheared(framebuffer, 2, bgViewY);
	}
	else {
		DrawGridPlane(framebuffer, 2, bgViewX, bgViewY);
	}
	DrawGridPlane(framebuffer, 1, fgViewX, fgViewY);
	DrawSprites(framebuffer, false);
	DrawGridPlane(framebuffer, 0, fgViewX, fgViewY);
	DrawSprites(framebuffer, true);
	ApplyR4WaterPaletteSplit(info);
	PresentFrame(renderer);
}

void Graphics_DrawWarp(SDL_Renderer *renderer, Sint32 scraHPosiw, Sint32 scrbHPosiw, Sint32 vscroll) {
	int fgViewX = (scraHPosiw >> 16) & 0x1ff;
	int bgViewX = (scrbHPosiw >> 16) & 0x1ff;
	int fgViewY = (vscroll >> 16) & 0xff;
	int bgViewY = vscroll & 0xff;
	UpdatePalette();
	SDL_FillSurfaceRect(framebuffer, NULL, 16);
	DrawGridPlane(framebuffer, 2, bgViewX, bgViewY);
	DrawGridPlane(framebuffer, 1, fgViewX, fgViewY);
	DrawSprites(framebuffer, false);
	DrawGridPlane(framebuffer, 0, fgViewX, fgViewY);
	DrawSprites(framebuffer, true);
	PresentFrame(renderer);
}

void Graphics_DrawPlanet(SDL_Renderer *renderer, const game_info *info) {
	UpdatePalette();
	SDL_FillSurfaceRect(framebuffer, NULL, 48);
	DrawGridPlane(framebuffer, 2, 0, 0);
	DrawGridPlane(framebuffer, 1, 0, 0);
	DrawPlanetRotateGrid(framebuffer, info);
	DrawSprites(framebuffer, false);
	DrawGridPlane(framebuffer, 0, 0, 0);
	DrawSprites(framebuffer, true);
	PresentFrame(renderer);
}

void Graphics_DrawThanks(SDL_Renderer *renderer, const game_info *info) {
	(void)info;
	UpdatePalette();
	SDL_FillSurfaceRect(framebuffer, NULL, 16);
	/* cgmwrt_b() fills the base-1 grid (plane 2), cgmwrt_a() the two plane A
	   halves; sprites sit between the low and the high half like in a stage. */
	DrawGridPlane(framebuffer, 2, 0, 0);
	DrawGridPlane(framebuffer, 1, 0, 0);
	DrawSprites(framebuffer, false);
	DrawGridPlane(framebuffer, 0, 0, 0);
	DrawSprites(framebuffer, true);
	PresentFrame(renderer);
}

void Graphics_DrawSpecial(SDL_Renderer *renderer, const game_info *info) {
	UpdatePalette();
	SDL_FillSurfaceRect(framebuffer, NULL, 16);
	DrawSpecialGrids(framebuffer);
	if (!specialClearMode) {
		DrawSpecialRotateGrid(framebuffer, info);
	}
	DrawSprites(framebuffer, false);
	DrawSprites(framebuffer, true);
	/* The special stage runs at 20 fps in the original: WINEXE/WINMAIN.C
	   specialStageLoop() only advances it - and only calls readController(),
	   which is what feeds a demo recording - when a counter reaches 3.  So the
	   frame is presented once per vblank for three vblanks, and a recorded
	   input frame is consumed once per logic frame.  Do not collapse this to a
	   single present: that triples the game speed (and would make the
	   special stage demos run three times too fast). */
	PresentFrame(renderer);
	PresentFrame(renderer);
	PresentFrame(renderer);
}
