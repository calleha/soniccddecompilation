#include <SDL3/SDL.h>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <vector>
#include "hmx.h"
#include "szdd.hpp"

struct hmx_environment {
	int unused;
};

struct hmx_renderer_base {
	Sint32 type;
	void *owner;
};

struct hmx_bitmap {
	Sint32 width;
	Sint32 height;
	Sint32 transparency;
	std::vector<Uint8> pixels;
};

struct hmx_sprite {
	hmx_renderer_base base;
	hmx_bitmap *bitmap;
	Sint32 x;
	Sint32 y;
	Sint32 flags;
};

struct hmx_grid {
	hmx_renderer_base base;
	Sint32 tileWidth;
	Sint32 tileHeight;
	Sint32 columns;
	Sint32 rows;
	Sint32 x;
	Sint32 y;
	Sint32 viewX;
	Sint32 viewY;
	Sint32 viewWidth;
	Sint32 viewHeight;
	std::vector<hmx_bitmap *> tiles;
	std::vector<Sint32> horzOffsets;
};

struct HmxScan {
	Sint32 startX;
	Sint32 startY;
	Sint32 deltaX;
	Sint32 deltaY;
};

struct hmx_ddagrid {
	hmx_grid grid;
	Sint32 screenWidth;
	Sint32 screenHeight;
	std::vector<HmxScan> scans;
};

struct hmx_renderer_context {
	struct Item {
		Sint32 priority;
		hmx_renderer_base *base;
		Sint32 order;
	};
	std::vector<Item> items;
	Sint32 orderCounter;
};

struct hmx_surface {
	SDL_Surface *surface;
};

struct hmx_background {
	Sint32 color;
};

static SDL_Surface *hmxFramebuffer;
static SDL_Renderer *hmxRenderer;
static HmxUpdatePaletteFn hmxUpdatePalette;
static HmxPresentFrameFn hmxPresentFrame;
static int hmxActive;

static hmx_environment hmxEnvironment;
static hmx_environment hmxLoaderEnvironment;
static hmx_renderer_context hmxRendererContext;
static hmx_surface hmxScreen;
static hmx_background hmxBackground;
static hmx_sprite hmxSpriteObjects[1000];
static hmx_sprite *hmxSpritePointers[1000];
static draw_context hmxDrawContext;

static int PositiveMod(int value, int divisor) {
	value %= divisor;
	if (value < 0) {
		value += divisor;
	}
	return value;
}

static void BlitIndexedPixels(SDL_Surface *target, const hmx_bitmap *bitmap, Sint32 x, Sint32 y) {
	if (!bitmap || bitmap->pixels.empty()) {
		return;
	}
	SDL_LockSurface(target);
	for (Sint32 srcY = 0; srcY < bitmap->height; srcY++) {
		Sint32 dstY = y + srcY;
		if (dstY < 0 || dstY >= target->h) {
			continue;
		}
		Uint8 *dst = static_cast<Uint8 *>(target->pixels) + dstY * target->pitch;
		const Uint8 *src = bitmap->pixels.data() + srcY * bitmap->width;
		for (Sint32 srcX = 0; srcX < bitmap->width; srcX++) {
			Sint32 dstX = x + srcX;
			if (dstX < 0 || dstX >= target->w) {
				continue;
			}
			Uint8 pixel = src[srcX];
			if (bitmap->transparency >= 0 && pixel == bitmap->transparency) {
				continue;
			}
			dst[dstX] = pixel;
		}
	}
	SDL_UnlockSurface(target);
}

static void DrawHmxGrid(SDL_Surface *target, const hmx_grid *grid) {
	if (!grid || grid->tileWidth <= 0 || grid->tileHeight <= 0 || grid->columns <= 0 || grid->rows <= 0) {
		return;
	}

	Sint32 viewX = grid->viewWidth ? grid->viewX : grid->x;
	Sint32 viewY = grid->viewHeight ? grid->viewY : grid->y;
	Sint32 viewWidth = grid->viewWidth ? grid->viewWidth : grid->columns * grid->tileWidth;
	Sint32 viewHeight = grid->viewHeight ? grid->viewHeight : grid->rows * grid->tileHeight;

	for (Sint32 screenY = viewY; screenY < viewY + viewHeight; screenY++) {
		if (screenY < 0 || screenY >= target->h) {
			continue;
		}
		Sint32 rowOffset = 0;
		Sint32 localY = screenY - viewY;
		if (localY >= 0 && localY < static_cast<Sint32>(grid->horzOffsets.size())) {
			rowOffset = grid->horzOffsets[localY];
		}
		for (Sint32 screenX = viewX; screenX < viewX + viewWidth; screenX++) {
			if (screenX < 0 || screenX >= target->w) {
				continue;
			}
			Sint32 srcX = screenX - viewX - grid->x + rowOffset;
			Sint32 srcY = screenY - viewY - grid->y;
			Sint32 sourceWidth = grid->columns * grid->tileWidth;
			Sint32 sourceHeight = grid->rows * grid->tileHeight;
			if (sourceWidth <= 0 || sourceHeight <= 0) {
				continue;
			}
			srcX = PositiveMod(srcX, sourceWidth);
			/* The tile map wraps on both axes: the time attack panel grid
			   scrolls vertically during the act switch (with a view taller
			   than the map), so a row below the map's last one continues at
			   the top instead of leaving a hole that the next layer would
			   show through. */
			srcY = PositiveMod(srcY, sourceHeight);
			Sint32 tileX = srcX / grid->tileWidth;
			Sint32 tileY = srcY / grid->tileHeight;
			hmx_bitmap *tile = grid->tiles[tileY * grid->columns + tileX];
			if (!tile || tile->pixels.empty()) {
				continue;
			}
			Sint32 pixelX = srcX % grid->tileWidth;
			Sint32 pixelY = srcY % grid->tileHeight;
			if (pixelX >= tile->width || pixelY >= tile->height) {
				continue;
			}
			Uint8 pixel = tile->pixels[pixelY * tile->width + pixelX];
			if (tile->transparency >= 0 && pixel == tile->transparency) {
				continue;
			}
			static_cast<Uint8 *>(target->pixels)[screenY * target->pitch + screenX] = pixel;
		}
	}
}

static void DrawHmxDdaGrid(SDL_Surface *target, const hmx_ddagrid *dda) {
	if (!dda) {
		return;
	}
	const hmx_grid *grid = &dda->grid;
	Sint32 sourceWidth = grid->columns * grid->tileWidth;
	Sint32 sourceHeight = grid->rows * grid->tileHeight;
	if (sourceWidth <= 0 || sourceHeight <= 0) {
		return;
	}
	for (Sint32 screenY = 0; screenY < dda->screenHeight && screenY < target->h; screenY++) {
		HmxScan scan = {};
		if (screenY < static_cast<Sint32>(dda->scans.size())) {
			scan = dda->scans[screenY];
		}
		for (Sint32 screenX = 0; screenX < dda->screenWidth && screenX < target->w; screenX++) {
			Sint32 srcX = (scan.startX + scan.deltaX * screenX) >> 16;
			Sint32 srcY = (scan.startY + scan.deltaY * screenX) >> 16;
			srcX = PositiveMod(srcX, sourceWidth);
			/* The rotating grids (special stages, Little Planet) keep their
			   scan coordinates inside the map, so nothing is repeated here -
			   only the plain grid path below wraps on both axes. */
			if (srcY < 0 || srcY >= sourceHeight) {
				continue;
			}
			Sint32 tileX = srcX / grid->tileWidth;
			Sint32 tileY = srcY / grid->tileHeight;
			hmx_bitmap *tile = grid->tiles[tileY * grid->columns + tileX];
			if (!tile || tile->pixels.empty()) {
				continue;
			}
			Sint32 pixelX = srcX % grid->tileWidth;
			Sint32 pixelY = srcY % grid->tileHeight;
			if (pixelX >= tile->width || pixelY >= tile->height) {
				continue;
			}
			Uint8 pixel = tile->pixels[pixelY * tile->width + pixelX];
			if (tile->transparency >= 0 && pixel == tile->transparency) {
				continue;
			}
			static_cast<Uint8 *>(target->pixels)[screenY * target->pitch + screenX] = pixel;
		}
	}
}

void Hmx_Init(SDL_Surface *framebuffer, SDL_Renderer *renderer, HmxUpdatePaletteFn updatePalette, HmxPresentFrameFn presentFrame) {
	hmxFramebuffer = framebuffer;
	hmxRenderer = renderer;
	hmxUpdatePalette = updatePalette;
	hmxPresentFrame = presentFrame;
	hmxActive = 0;
	for (int i = 0; i < SDL_arraysize(hmxSpriteObjects); i++) {
		hmxSpriteObjects[i].base = {0, &hmxSpriteObjects[i]};
		hmxSpriteObjects[i].bitmap = nullptr;
		hmxSpriteObjects[i].x = 0;
		hmxSpriteObjects[i].y = 0;
		hmxSpriteObjects[i].flags = 0;
		hmxSpritePointers[i] = &hmxSpriteObjects[i];
	}
	hmxRendererContext.items.clear();
	hmxRendererContext.orderCounter = 0;
	hmxScreen.surface = framebuffer;
	hmxBackground.color = 0;
	memset(&hmxDrawContext, 0, sizeof(hmxDrawContext));
	hmxDrawContext.screen = &hmxScreen;
	hmxDrawContext.context = &hmxRendererContext;
	hmxDrawContext.background = &hmxBackground;
	hmxDrawContext.sprites = hmxSpritePointers;
}

void Hmx_Shutdown(void) {
	hmxFramebuffer = nullptr;
	hmxRenderer = nullptr;
	hmxUpdatePalette = nullptr;
	hmxPresentFrame = nullptr;
	hmxActive = 0;
}

void Hmx_SetActive(int active) {
	hmxActive = active;
}

int Hmx_IsActive(void) {
	return hmxActive;
}

void *hmx_get_environment(void) {
	return &hmxEnvironment;
}

void *hmx_get_loader_environment(void) {
	return &hmxLoaderEnvironment;
}

draw_context *get_draw_context(void) {
	Hmx_SetActive(1);
	return &hmxDrawContext;
}

void FlipToScreen(void) {
	if (hmxRenderer && hmxPresentFrame) {
		hmxPresentFrame(hmxRenderer);
	}
}

void ld_bitmap_4to8(void *dst, void *src, Sint32 wx, Sint32 wy, Sint32 palet, Sint32 zero_chg, Sint32 flag4) {
	Uint8 *pDst = static_cast<Uint8 *>(dst);
	Uint8 *pSrc = static_cast<Uint8 *>(src);
	for (Sint32 y = 0; y < wy; y++) {
		for (Sint32 x = 0; x < wx; x++) {
			if (flag4 == 0 || x < wx - 4) {
				Uint8 pixel = (x & 1) ? (*pSrc & 0x0f) : ((*pSrc & 0xf0) >> 4);
				*pDst = pixel + palet;
				if (*pDst == palet) {
					*pDst = zero_chg ? 255 : 0;
				}
				pDst++;
			}
			if (x & 1) {
				pSrc++;
			}
		}
	}
}

void *ld_load_cmpfile(hmx_environment *env, char *path) {
	(void)env;
	auto data = SZDD_Decompress(path);
	if (data.empty()) {
		return nullptr;
	}
	void *buffer = malloc(data.size());
	if (!buffer) {
		return nullptr;
	}
	memcpy(buffer, data.data(), data.size());
	return buffer;
}

void hmx_background_set_background(hmx_background *background, Sint32 color) {
	if (background) {
		background->color = color;
	}
}

hmx_bitmap *hmx_bitmap_create(hmx_environment *env, Sint32 width, Sint32 height) {
	(void)env;
	hmx_bitmap *bitmap = new hmx_bitmap;
	bitmap->width = width;
	bitmap->height = height;
	bitmap->transparency = -1;
	bitmap->pixels.assign(width * height, 0);
	return bitmap;
}

void hmx_bitmap_release(hmx_environment *env, hmx_bitmap *bitmap) {
	(void)env;
	delete bitmap;
}

void *hmx_bitmap_get_scan0(hmx_bitmap *bitmap) {
	return bitmap ? bitmap->pixels.data() : nullptr;
}

void hmx_bitmap_set_transparency(hmx_bitmap *bitmap, Sint32 color) {
	if (bitmap) {
		bitmap->transparency = color;
	}
}

void hmx_free(hmx_environment *env, void *ptr) {
	(void)env;
	free(ptr);
}

hmx_grid *hmx_grid_create(hmx_environment *env, Sint32 tileWidth, Sint32 tileHeight, Sint32 width, Sint32 height) {
	(void)env;
	hmx_grid *grid = new hmx_grid;
	grid->base = {1, grid};
	grid->tileWidth = tileWidth;
	grid->tileHeight = tileHeight;
	grid->columns = width;
	grid->rows = height;
	grid->x = 0;
	grid->y = 0;
	grid->viewX = 0;
	grid->viewY = 0;
	grid->viewWidth = width * tileWidth;
	grid->viewHeight = height * tileHeight;
	grid->tiles.assign(width * height, nullptr);
	return grid;
}

void hmx_grid_release(hmx_environment *env, hmx_grid *grid) {
	(void)env;
	if (grid && grid->base.type == 2) {
		delete reinterpret_cast<hmx_ddagrid *>(grid);
		return;
	}
	delete grid;
}

hmx_renderer_base *hmx_grid_base(hmx_grid *grid) {
	return grid ? &grid->base : nullptr;
}

void hmx_grid_set_position(hmx_grid *grid, Sint32 x, Sint32 y) {
	if (grid) {
		grid->x = x;
		grid->y = y;
	}
}

void hmx_grid_set_view(hmx_grid *grid, Sint32 x, Sint32 y, Sint32 width, Sint32 height) {
	if (grid) {
		grid->viewX = x;
		grid->viewY = y;
		grid->viewWidth = width;
		grid->viewHeight = height;
	}
}

void hmx_grid_set_tile(hmx_grid *grid, Sint32 x, Sint32 y, hmx_bitmap *bitmap, Sint32 flags) {
	(void)flags;
	if (!grid || x < 0 || y < 0 || x >= grid->columns || y >= grid->rows) {
		return;
	}
	grid->tiles[y * grid->columns + x] = bitmap;
}

void hmx_grid_set_horz_offsets(hmx_grid *grid, Sint32 *offsets) {
	if (!grid || !offsets) {
		return;
	}
	Sint32 count = grid->viewHeight > 0 ? grid->viewHeight : grid->rows * grid->tileHeight;
	if (count <= 0) {
		count = 64;
	}
	grid->horzOffsets.assign(offsets, offsets + count);
}

void hmx_renderer_context_add(hmx_renderer_context *context, Sint32 priority, hmx_renderer_base *base) {
	if (!context || !base) {
		return;
	}
	context->items.push_back({priority, base, context->orderCounter++});
}

void hmx_renderer_context_clear(hmx_renderer_context *context) {
	if (context) {
		context->items.clear();
		context->orderCounter = 0;
	}
}

void hmx_renderer_context_draw(hmx_renderer_context *context, hmx_surface *surface) {
	(void)surface;
	if (!context || !hmxFramebuffer) {
		return;
	}
	if (hmxUpdatePalette) {
		hmxUpdatePalette();
	}
	SDL_FillSurfaceRect(hmxFramebuffer, NULL, hmxBackground.color);
	std::sort(context->items.begin(), context->items.end(), [](const auto &a, const auto &b) {
		if (a.priority != b.priority) {
			return a.priority < b.priority;
		}
		return a.order < b.order;
	});
	for (const auto &item : context->items) {
		if (item.base->type == 0) {
			hmx_sprite *sprite = static_cast<hmx_sprite *>(item.base->owner);
			if (sprite && sprite->bitmap) {
				BlitIndexedPixels(hmxFramebuffer, sprite->bitmap, sprite->x, sprite->y);
			}
		} else if (item.base->type == 1) {
			DrawHmxGrid(hmxFramebuffer, static_cast<hmx_grid *>(item.base->owner));
		} else if (item.base->type == 2) {
			DrawHmxDdaGrid(hmxFramebuffer, static_cast<hmx_ddagrid *>(item.base->owner));
		}
	}
}

hmx_renderer_base *hmx_sprite_base(hmx_sprite *sprite) {
	return sprite ? &sprite->base : nullptr;
}

void hmx_sprite_set_position(hmx_sprite *sprite, Sint32 x, Sint32 y) {
	if (sprite) {
		sprite->x = x;
		sprite->y = y;
	}
}

void hmx_sprite_set_bitmap(hmx_sprite *sprite, hmx_bitmap *bitmap) {
	if (sprite) {
		sprite->bitmap = bitmap;
	}
}

void hmx_sprite_set_flags(hmx_sprite *sprite, Sint32 flags) {
	if (sprite) {
		sprite->flags = flags;
	}
}

hmx_ddagrid *hmx_ddagrid_create(hmx_environment *env, Sint32 tileWidth, Sint32 tileHeight, Sint32 columns, Sint32 rows, Sint32 width, Sint32 height) {
	(void)env;
	hmx_ddagrid *dda = new hmx_ddagrid;
	dda->grid.base = {2, dda};
	dda->grid.tileWidth = 1 << tileWidth;
	dda->grid.tileHeight = 1 << tileHeight;
	dda->grid.columns = columns * 8;
	dda->grid.rows = rows * 4;
	dda->grid.x = 0;
	dda->grid.y = 0;
	dda->grid.viewX = 0;
	dda->grid.viewY = 0;
	dda->grid.viewWidth = width;
	dda->grid.viewHeight = height;
	dda->grid.tiles.assign(dda->grid.columns * dda->grid.rows, nullptr);
	dda->screenWidth = width;
	dda->screenHeight = height;
	dda->scans.assign(height, {});
	return dda;
}

void hmx_ddagrid_release(hmx_environment *env, hmx_ddagrid *grid) {
	(void)env;
	delete grid;
}

void hmx_ddagrid_set_scan(hmx_ddagrid *grid, Sint32 line, Sint32 startX, Sint32 deltaX, Sint32 startY, Sint32 deltaY) {
	if (!grid || line < 0 || line >= static_cast<Sint32>(grid->scans.size())) {
		return;
	}
	grid->scans[line] = {startX, startY, deltaX, deltaY};
}

void hmx_ddagrid_set_tile(hmx_ddagrid *grid, Sint32 x, Sint32 y, hmx_bitmap *bitmap, Sint32 flags) {
	(void)flags;
	if (!grid || x < 0 || y < 0 || x >= grid->grid.columns || y >= grid->grid.rows) {
		return;
	}
	grid->grid.tiles[y * grid->grid.columns + x] = bitmap;
}
