#include "../../types.h"
#include "grid.h"
#include "hmx_types.h"
#include "ld_types.h"
#include "hmx_oeeactl.h"
#include "../../impfuncs.h"

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
extern void *(*hmx_bitmap_get_scan0_module)(hmx_bitmap *);
extern sprite_bmp infoSprtBmp[];
extern hmx_bitmap *(*hmx_bitmap_create_module)(hmx_environment *, Sint32,
                                               Sint32);
extern void *(*ld_load_cmpfile_module)(hmx_environment *, char *);
extern Uint16 NUM_GRIDBMP;
extern grid_bmp infoGridBmp[];
extern void (*ld_bitmap_4to8_module)(void *, void *, Sint32, Sint32, Sint32,
                                     Sint32, Sint32);
extern hmx_environment *g_env_module;
extern char *TileCmpFileName;
extern hmx_environment *g_loader_module;
extern void (*hmx_bitmap_release_module)(hmx_environment *, hmx_bitmap *);
extern Uint16 NUM_BMP;
extern char *SprCmpFileName;
extern void (*hmx_sprite_set_flags_module)(hmx_sprite *, Sint32);
extern void (*hmx_sprite_set_bitmap_module)(hmx_sprite *, hmx_bitmap *);
extern void (*hmx_sprite_set_position_module)(hmx_sprite *, Sint32, Sint32);
extern void (*hmx_grid_release_module)(hmx_environment *, hmx_grid *);
extern hmx_grid *(*hmx_grid_create_module)(hmx_environment *, Sint32, Sint32,
                                           Sint32, Sint32);
static char ErrBuf[250] = {0};
Sint32 ErrRet = 0;
static Sint32 sprIdx[512] = {0};
#if defined(SAVEDATA)
extern Sint32 PRIO_MAX;
#else
#define PRIO_MAX 60
#endif

/* Sizes the sprite loaders created the bitmaps with, so that
   OESprBmpDuplicateUnfaded() can copy one without repeating their padding
   rules. */
static Sint32 sprBmpW[64];
static Sint32 sprBmpH[64];
/* Copies made by OESprBmpDuplicateUnfaded(), released by OESprBmpDelete(). */
#define MAX_SPR_UNFADED 16
static hmx_bitmap *sprUnfadedBmp[MAX_SPR_UNFADED];
static Sint32 sprUnfadedNum;

static void sprBmpSetSize(Sint32 indx, Sint32 w, Sint32 h) {

    if (indx < 0 || indx >= 64)
        return;
    sprBmpW[indx] = w;
    sprBmpH[indx] = h;
}

void EAError(Sint32 ret, Sint32 line, char *str) {}

void srfDraw(void) {
    Sint32 i;
    if (s_ctx->context != 0) {

        hmx_renderer_context_clear_module(s_ctx->context);

#if defined(OPENING)
        for (i = 0; i < 3; ++i) {

            if (*infoGridBmp[i].lphGrid != 0)
                hmx_renderer_context_add_module(
                    s_ctx->context, PRIO_MAX - infoGridBmp[i].order,
                    hmx_grid_base_module(s_ctx->grids[i]));
        }
#else
        if (s_ctx->grids[0] != 0) {
            hmx_renderer_context_add_module(
                s_ctx->context, 0, hmx_grid_base_module(s_ctx->grids[0]));
        }
#endif

        for (i = 255; i >= 0; --i) {
            if (s_ctx->spr_level[i] != 0) {
                hmx_renderer_context_add_module(
                    s_ctx->context, s_ctx->spr_level[i],
                    hmx_sprite_base_module(s_ctx->sprites[i]));
            }
        }

        hmx_renderer_context_draw_module(s_ctx->context, s_ctx->screen);
    }
    FlipToScreen_module();
}

void ld_bitmap_file(char *fname, void *dst, Sint32 wx, Sint32 wy, Sint32 palet,
                    Sint32 unknown) {
    Sint32 fp;
    Sint32 fsize;
    void *fbuf = 0;
    Uint32 offset;
    Sint32 iWidth, iHeight, iLength, y, x;
    Uint8 *src, *pDst = dst;

    fp = sOpenFile(fname);
    if (fp != -1) {
        fsize = sGetFileSize(fp);
        if (fsize != -1) {
            fbuf = sMemAlloc(fsize);
            if (sReadFile(fp, fbuf, fsize) != -1) {

                src = fbuf;
                offset = src[10] + (src[11] << 8) + (src[12] << 16) +
                         (src[13] << 24);
                iWidth = src[18] + (src[19] << 8) + (src[20] << 16) +
                         (src[21] << 24);
                iHeight = src[22] + (src[23] << 8) + (src[24] << 16) +
                          (src[25] << 24);
                if (iWidth % 4 == 0)
                    iLength = iWidth;
                else
                    iLength = iWidth + (4 - iWidth % 4);
                for (y = wy; y > 0; --y) {
                    for (x = 0; x < wx; ++x) {
                        if (x < wx - 1) {
                            if (palet != 0) {
                                if (src[offset + x + (y - 1) * iLength] != 0)
                                    *pDst = palet +
                                            src[offset + x + (y - 1) * iLength];
                                else
                                    *pDst = 0;
                            } else
                                *pDst = src[offset + x + (y - 1) * iLength];

                            if (*pDst == 0)
                                *pDst = 255;
                        } else
                            *pDst = 255;
                        ++pDst;
                    }
                }
            }
        }
        sCloseFile(fp);
    }
    if (fbuf != 0) {
        sMemFree(fbuf);
    }
}

void ld_bitmap_file2(char *fname, void *dst, Sint32 sx, Sint32 sy, Sint32 wx,
                     Sint32 wy, Sint32 dx, Sint32 dy, Sint32 unk1,
                     Sint32 unk2) {
    Uint8 *pDst;
    Sint32 fp;
    void *fbuf = 0;
    Uint8 *src;
    Sint32 i, fsize, x, y, iWidth, iHeight, iLength;
    Uint32 offset;
    pDst = dst;

    if (dx != 0) {
        for (i = 0; i < dx; ++pDst, ++i) {
        }
    }

    fp = sOpenFile(fname);
    if (fp != -1) {
        fsize = sGetFileSize(fp);
        if (fsize != -1) {
            fbuf = sMemAlloc(fsize);
            if (sReadFile(fp, fbuf, fsize) != -1) {

                src = fbuf;
                offset = src[10] + (src[11] << 8) + (src[12] << 16) +
                         (src[13] << 24);
                iWidth = src[18] + (src[19] << 8) + (src[20] << 16) +
                         (src[21] << 24);
                iHeight = src[22] + (src[23] << 8) + (src[24] << 16) +
                          (src[25] << 24);
                if (iWidth % 4 == 0)
                    iLength = iWidth;
                else
                    iLength = iWidth + (4 - iWidth % 4);
                for (y = wy; y > 0; --y) {
                    for (x = 0; x < wx; ++x) {
                        *pDst = src[offset + x + (y - 1) * iLength];
                        if (*pDst == 0)
                            *pDst = 255;
                        ++pDst;
                    }
                    if (dx != 0) {
                        for (i = 0; i < dx; ++pDst, ++i) {
                        }
                    } else if (iWidth != sx) {
                        for (i = 0; i < sx - iWidth; ++pDst, ++i) {
                        }
                    }
                }
            }
        }
        sCloseFile(fp);
    }
    if (fbuf != 0) {
        sMemFree(fbuf);
    }
}

void ld_bitmap_4to8_1(void *dst, void *src, Sint32 wx, Sint32 wy, Sint32 palet,
                      Sint32 zero_chg, Sint32 flag4) {
    Uint8 *pDst, *pSrc;
    Sint32 x, y;

    pDst = dst;
    pSrc = src;

    for (y = 0; y < wy; ++y) {
        for (x = 0; x < wx; ++x) {
            if (flag4 == 0 || x < wx - 4) {
                if (x & 1)
                    *pDst = (*pSrc & 15) + palet;
                else
                    *pDst = ((*pSrc & 240) >> 4) + palet;
                if (*pDst == palet) {
                    if (zero_chg != 0) {
                        *pDst = 255;
                    } else
                        *pDst = 0;
                }

                ++pDst;
            }
            if (x & 1)
                ++pSrc;
        }
    }
}

void ld_bitmap_4to8_2(void *dst, void *src, Sint32 wx, Sint32 wy, Sint32 palet,
                      Sint32 zero_chg, Sint32 flag4) {
    Uint8 *pDst, *pSrc;
    Sint32 x, y;

    pDst = dst;
    pSrc = src;

    for (y = 0; y < wy; ++y) {
        for (x = 0; x < wx; ++x) {
            if (flag4 == 0 || x < wx - 4) {
                if (x < wx - 1) {
                    if (x & 1)
                        *pDst = (*pSrc & 15) + palet;
                    else
                        *pDst = ((*pSrc & 240) >> 4) + palet;
                } else
                    *pDst = palet;

                if (*pDst == palet) {
                    if (zero_chg != 0) {
                        *pDst = 255;
                    } else
                        *pDst = 0;
                }

                ++pDst;
            }
            if (x & 1)
                ++pSrc;
        }
    }
}

/* ld_bitmap_4to8_1() for a packed file whose rows are padded to a multiple of
   eight pixels: `wx` is the sprite's own width and `stride` the width of the
   stored row.  OPENINGS.CM_ leaves data in that padding, and drawn it showed as
   stray pixels along the right edge of the sprites (the fork fixed the same
   thing by clipping the opening's sprites to their real width), so only the
   sprite's own columns are kept.  That also covers the one sprite the reference
   loads with ld_bitmap_4to8_2(): the column that one blanks is padding. */
void ld_bitmap_4to8_clipped(void *dst, void *src, Sint32 wx, Sint32 wy,
                            Sint32 stride, Sint32 palet, Sint32 zero_chg) {
    Uint8 *pDst, *pSrc;
    Uint8 value;
    Sint32 x, y;

    pDst = dst;
    pSrc = src;

    for (y = 0; y < wy; ++y) {
        for (x = 0; x < stride; ++x) {
            if (x & 1)
                value = (Uint8)((*pSrc & 15) + palet);
            else
                value = (Uint8)(((*pSrc & 240) >> 4) + palet);

            if (x < wx) {
                if (value == palet) {
                    if (zero_chg != 0) {
                        *pDst = 255;
                    } else
                        *pDst = 0;
                } else
                    *pDst = value;
                ++pDst;
            }
            if (x & 1)
                ++pSrc;
        }
    }
}

Sint32 ld_load_sprite2(hmx_environment *buffer, char *file,
                       hmx_environment *env, hmx_bitmap **bitmaps,
                       Sint32 size) {
    Sint32 count = -1, read_count;
    Uint8 *pixbuf;
    void *expbuf = 0;
    ld_sprite_header *header;
    Sint32 i;
    hmx_bitmap *bmp;
    ld_sprite_inf *s;
    Sint32 wx, wy;

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

        ++read_count;
        for (i = 0; i < read_count; ++i) {

            if (i > 31)
                s = &header->spr[i - 1];
            else
                s = &header->spr[i];

            wx = s->wx;
            wy = s->wy;

            if (i == 3)
                bmp = hmx_bitmap_create_module(env, 248, 8);
            else if (i == 31) {
                bmp = hmx_bitmap_create_module(env, 40, 16);
                wx = wy = 0;
            } else {
                /* `wx` is the width of the stored row: the file pads each row
                   to a multiple of eight pixels.  The bitmap itself is only as
                   wide as the sprite (see ld_bitmap_4to8_clipped()). */
                wx = (infoSprtBmp[i].w + 7) / 8 * 8;
                wy = infoSprtBmp[i].h;
                bmp = hmx_bitmap_create_module(env, infoSprtBmp[i].w, wy);
                sprBmpSetSize(i, infoSprtBmp[i].w, wy);
            }

            if (i != 3 && i != 31)
                ld_bitmap_4to8_clipped(hmx_bitmap_get_scan0_module(bmp), pixbuf,
                                       infoSprtBmp[i].w, wy, wx,
                                       infoSprtBmp[i].palOff, 1);
            else if (i == 31)
                ld_bitmap_file("../TITLE/OPENING/EXIT.BMP",
                               hmx_bitmap_get_scan0_module(bmp), 40, 16,
                               infoSprtBmp[i].palOff, 1);
            else

                ld_bitmap_file("../TITLE/OPENING/1996SEGA.BMP",
                               hmx_bitmap_get_scan0_module(bmp), 248, 8, 0, 1);

            pixbuf += wx * wy / 2;

            hmx_bitmap_set_transparency_module(bmp, 255);

            bitmaps[i] = bmp;
        }

        count = read_count;
    }

    if (expbuf != 0) {
        hmx_free_module(buffer, expbuf);
    }

    return count;
}

Sint32 ld_load_sprite1(hmx_environment *buffer, char *file,
                       hmx_environment *env, hmx_bitmap **bitmaps,
                       Sint32 size) {
    Sint32 count = -1;

    void *expbuf = 0;
    ld_sprite_header *header;
    Sint32 read_count;
    Uint8 *pixbuf;
    Sint32 i;
    ld_sprite_inf *s;
    Sint32 flag4, wx, wy;
    hmx_bitmap *bmp;
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
            s = &header->spr[i];

            flag4 = 0;
            wx = s->wx;
            wy = s->wy;

            wx = (s->wx + 7) / 8 * 8;
            bmp = hmx_bitmap_create_module(env, wx, wy);
            sprBmpSetSize(i, wx, wy);

            if (wx & 4) {
                wx += 4;
                flag4 = 1;
            }

            ld_bitmap_4to8_1(hmx_bitmap_get_scan0_module(bmp), pixbuf, wx, wy,
                             infoSprtBmp[i].palOff, 1, flag4);

            pixbuf += wx * wy / 2;

            if (flag4 != 0) {
                wx -= 4;
            }

            hmx_bitmap_set_transparency_module(bmp, 255);

            bitmaps[i] = bmp;
        }

        count = read_count;
    }

    if (expbuf != 0) {
        hmx_free_module(buffer, expbuf);
    }

    return count;
}

Sint32 ld_load_grid2(hmx_environment *buffer, char *file, hmx_environment *env,
                     hmx_bitmap **bitmaps, Sint32 size) {
    Sint32 count = -1;

    void *expbuf = 0;
    ld_scroll_header *header;
    Sint32 read_count;
    Uint8 *pixbuf;
    Sint32 i, n, palet, paletcnt, palet_offs;
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

        for (i = 0; i < NUM_GRIDBMP; ++i) {

            palet_offs = infoGridBmp[i].palOff;
            for (n = 0; n < infoGridBmp[i].totalNum; ++n) {

                wx = (infoGridBmp[i].w + 7) / 8 * 8;
                wy = infoGridBmp[i].h;
                bmp = hmx_bitmap_create_module(env, wx, wy);

                ld_bitmap_4to8_module(hmx_bitmap_get_scan0_module(bmp), pixbuf,
                                      wx, wy, palet_offs, 0, 0);

                pixbuf += wx * wy / 2;

                hmx_bitmap_set_transparency_module(bmp, 0);

                bitmaps[i] = bmp;

                infoGridBmp[i].lphBmp[n] = (Uint32 *)bitmaps[i];
            }
        }

        count = read_count;
    }

    if (expbuf != 0) {
        hmx_free_module(buffer, expbuf);
    }

    return count;
}

Sint32 OEGridBmpCreate(void) {
    Sint32 i, count;

    s_ctx->grd_bitmap_count = 0;
    for (i = 0; i < 2048; ++i) {
        s_ctx->grd_bitmaps[i] = 0;
    }

    count = ld_load_grid2(g_loader_module, TileCmpFileName, g_env_module,
                          s_ctx->grd_bitmaps, 2048);

    if (count <= 0) {
        return -1;
    }

    s_ctx->grd_bitmap_count = count;

    return 0;
}

void OEGridBmpDelete(void) {
    Sint32 i, n;

    for (i = 0; i < NUM_GRIDBMP; ++i) {

        for (n = 0; n < infoGridBmp[i].totalNum; ++n) {

            if (infoGridBmp[i].lphBmp[n] != 0) {

                hmx_bitmap_release_module(
                    g_env_module, (hmx_bitmap *)infoGridBmp[i].lphBmp[n]);
                infoGridBmp[i].lphBmp[n] = 0;
            }
        }
    }
    s_ctx->grd_bitmap_count = 0;
    for (i = 0; i < 2048; ++i)
        s_ctx->grd_bitmaps[i] = 0;
}

Sint32 OESprBmpCreate(void) {
    Sint32 i;
    Sint32 count;

    s_ctx->spr_bitmap_count = 0;
    for (i = 0; i < 1000; ++i) {
        s_ctx->spr_bitmaps[i] = 0;
        s_ctx->spr_level[i] = 0;
    }

#if defined(OPENING)
    count = ld_load_sprite2(g_loader_module, SprCmpFileName, g_env_module,
                            s_ctx->spr_bitmaps, 1000);
#endif

#if !defined(OPENING)
    count = ld_load_sprite1(g_loader_module, SprCmpFileName, g_env_module,
                            s_ctx->spr_bitmaps, 1000);
#endif

    if (count <= 0) {
        return -1;
    }

    s_ctx->spr_bitmap_count = count;
    for (i = 0; i < NUM_BMP; ++i) {
        infoSprtBmp[i].hBmp = (Uint32 *)s_ctx->spr_bitmaps[i];
    }

    return 0;
}

void OESprBmpDelete(void) {
    Sint32 i;

    s_ctx->spr_bitmap_count = 0;
    for (i = 0; i < 1000; ++i) {
        hmx_bitmap_release_module(g_env_module, s_ctx->spr_bitmaps[i]);
        s_ctx->spr_bitmaps[i] = 0;
        s_ctx->spr_level[i] = 0;
    }
    for (i = 0; i < sprUnfadedNum; ++i) {
        hmx_bitmap_release_module(g_env_module, sprUnfadedBmp[i]);
        sprUnfadedBmp[i] = 0;
    }
    sprUnfadedNum = 0;
}

/* Makes a copy of sprite bitmap `indx` whose colours are moved from the palette
   line the sprite was extracted with (`palet`) to the entries starting at
   `unfaded`, so a palette flash that rewrites every line of the working
   palette leaves the copy's colours alone.

   The art leaves holes in some sprites - Sonic's pupils and nose show the
   artwork behind his face - which the flash would otherwise show through, so a
   transparent pixel that the sprite does not open onto its own border is
   closed here with `unfaded`, the line's first (black) entry, which is what
   shows through those holes on the title screen.  The copy is released with
   the other sprite bitmaps by OESprBmpDelete(); 0 means no copy was made. */
hmx_bitmap *OESprBmpDuplicateUnfaded(Sint32 indx, Sint32 palet,
                                     Sint32 unfaded) {
    hmx_bitmap *src_bmp;
    hmx_bitmap *dst_bmp;
    Uint8 *pSrc, *pDst, *pOuter;
    Sint32 *pStack;
    Sint32 w, h, i, x, y, stack_size;

    if (indx < 0 || indx >= 64 || s_ctx->spr_bitmaps[indx] == 0)
        return 0;

    w = sprBmpW[indx];
    h = sprBmpH[indx];
    if (w <= 0 || h <= 0 || sprUnfadedNum >= MAX_SPR_UNFADED)
        return 0;

    src_bmp = s_ctx->spr_bitmaps[indx];
    pSrc = hmx_bitmap_get_scan0_module(src_bmp);
    dst_bmp = hmx_bitmap_create_module(g_env_module, w, h);
    pDst = hmx_bitmap_get_scan0_module(dst_bmp);
    pOuter = sMemAlloc(w * h);
    pStack = sMemAlloc(w * h * sizeof(Sint32));
    if (pOuter == 0 || pStack == 0) {

        if (pOuter != 0)
            sMemFree(pOuter);
        if (pStack != 0)
            sMemFree(pStack);
        hmx_bitmap_release_module(g_env_module, dst_bmp);
        return 0;
    }

    for (i = 0; i < w * h; ++i) {
        if (pSrc[i] == 255)
            pDst[i] = 255;
        else
            pDst[i] = (Uint8)(pSrc[i] - palet + unfaded);
        pOuter[i] = 0;
    }

    /* Mark the transparent pixels that can be reached from the border. */
    stack_size = 0;
    for (y = 0; y < h; ++y) {
        for (x = 0; x < w; ++x) {
            if (x != 0 && x != w - 1 && y != 0 && y != h - 1)
                continue;
            i = y * w + x;
            if (pSrc[i] == 255 && pOuter[i] == 0) {

                pOuter[i] = 1;
                pStack[stack_size++] = i;
            }
        }
    }
    while (stack_size > 0) {
        Sint32 p = pStack[--stack_size];
        Sint32 px = p % w;
        Sint32 py = p / w;
        Sint32 neighbours[4];
        Sint32 n;

        neighbours[0] = px > 0 ? p - 1 : -1;
        neighbours[1] = px < w - 1 ? p + 1 : -1;
        neighbours[2] = py > 0 ? p - w : -1;
        neighbours[3] = py < h - 1 ? p + w : -1;

        for (n = 0; n < 4; ++n) {
            Sint32 q = neighbours[n];
            if (q < 0 || pOuter[q] != 0 || pSrc[q] != 255)
                continue;
            pOuter[q] = 1;
            pStack[stack_size++] = q;
        }
    }

    /* Close the holes that are left. */
    for (i = 0; i < w * h; ++i) {
        if (pDst[i] == 255 && pOuter[i] == 0)
            pDst[i] = (Uint8)unfaded;
    }

    sMemFree(pOuter);
    sMemFree(pStack);

    hmx_bitmap_set_transparency_module(dst_bmp, 255);
    sprUnfadedBmp[sprUnfadedNum++] = dst_bmp;

    return dst_bmp;
}

Sint32 OESprCreate(Uint16 timing) {
    Sint32 i;

    for (i = 0; i < NUM_BMP; ++i) {

        if (timing != infoSprtBmp[i].timing)
            continue;

        if (*infoSprtBmp[i].lphSpr != 0)
            continue;

        sprCreate(infoSprtBmp[i].lphSpr);
        hmx_sprite_set_position_module(s_ctx->sprites[*infoSprtBmp[i].lphSpr],
                                       infoSprtBmp[i].point.x,
                                       infoSprtBmp[i].point.y);
        hmx_sprite_set_bitmap_module(s_ctx->sprites[*infoSprtBmp[i].lphSpr],
                                     s_ctx->spr_bitmaps[i]);
        hmx_sprite_set_flags_module(s_ctx->sprites[*infoSprtBmp[i].lphSpr], 0);
        s_ctx->spr_level[*infoSprtBmp[i].lphSpr] =
            PRIO_MAX - infoSprtBmp[i].order;
    }

    return 0;
}

Sint32 OESprCreateIndx(Sint32 indx) {

    if (*infoSprtBmp[indx].lphSpr != 0)
        return 0;
    sprCreate(infoSprtBmp[indx].lphSpr);
    hmx_sprite_set_position_module(s_ctx->sprites[*infoSprtBmp[indx].lphSpr],
                                   infoSprtBmp[indx].point.x,
                                   infoSprtBmp[indx].point.y);
    hmx_sprite_set_bitmap_module(s_ctx->sprites[*infoSprtBmp[indx].lphSpr],
                                 s_ctx->spr_bitmaps[indx]);
    hmx_sprite_set_flags_module(s_ctx->sprites[*infoSprtBmp[indx].lphSpr], 0);
    s_ctx->spr_level[*infoSprtBmp[indx].lphSpr] =
        PRIO_MAX - infoSprtBmp[indx].order;
    return 0;
}

void OESprDelete(Uint16 timing) {
    Sint32 i;

    for (i = 0; i < NUM_BMP; ++i) {

        if (timing != infoSprtBmp[i].timing)
            continue;

        if (*infoSprtBmp[i].lphSpr == 0)
            continue;

        hmx_sprite_set_bitmap_module(s_ctx->sprites[*infoSprtBmp[i].lphSpr], 0);
        hmx_sprite_set_flags_module(s_ctx->sprites[*infoSprtBmp[i].lphSpr], 0);
        s_ctx->spr_level[*infoSprtBmp[i].lphSpr] = 0;
        sprDelete(*infoSprtBmp[i].lphSpr);
        *infoSprtBmp[i].lphSpr = 0;
    }
}

void OESprDeleteIndx(Sint32 indx) {

#if !defined(OPENING)
    if (*infoSprtBmp[indx].lphSpr == 0)
        return;

    hmx_sprite_set_bitmap_module(s_ctx->sprites[*infoSprtBmp[indx].lphSpr], 0);
    hmx_sprite_set_flags_module(s_ctx->sprites[*infoSprtBmp[indx].lphSpr], 0);
    s_ctx->spr_level[*infoSprtBmp[indx].lphSpr] = 0;
    sprDelete(*infoSprtBmp[indx].lphSpr);
    *infoSprtBmp[indx].lphSpr = 0;
#endif
}

void OEAllSprDelete(void) {
    Sint32 i;

    for (i = 0; i < NUM_BMP; ++i) {

        if (*infoSprtBmp[i].lphSpr == 0)
            continue;

        hmx_sprite_set_bitmap_module(s_ctx->sprites[*infoSprtBmp[i].lphSpr], 0);
        hmx_sprite_set_flags_module(s_ctx->sprites[*infoSprtBmp[i].lphSpr], 0);
        s_ctx->spr_level[*infoSprtBmp[i].lphSpr] = 0;
        sprDelete(*infoSprtBmp[i].lphSpr);
        *infoSprtBmp[i].lphSpr = 0;
    }
}

Sint32 OEGridDeleteAll(void) {
    Sint32 i;
    for (i = 0; i < 3; ++i) {
        hmx_grid_release_module(g_env_module, s_ctx->grids[i]);
        s_ctx->grids[i] = 0;
    }

    return 0;
}

Sint32 OECreateEA(void) {
    Sint32 i;
    for (i = 0; i < 512; ++i)
        sprIdx[i] = 0;
    OEGridDeleteAll();
    if (OESprBmpCreate() != 0)
        return 0;
    if (OEGridBmpCreate() != 0)
        return 0;

    return 1;
}

void OEDeleteEA(void) {
    Sint32 i;
    OESprBmpDelete();
    OEAllSprDelete();
    OEGridBmpDelete();
    OEAllGridDelete();
    for (i = 0; i < 3; ++i) {
        s_ctx->grids[i] = hmx_grid_create_module(g_env_module, 8, 8, 64, 32);
    }

    s_ctx->grd_bitmap_count = 0;
}

void sprCreate(Uint32 *handle) {
    Sint32 i;
    for (i = 1; i < 512; ++i) {
        if (sprIdx[i] == 0) {
            sprIdx[i] = 1;
            *handle = i;

            return;
        }
    }
}

void sprDelete(Sint32 handle) {

    if (sprIdx[handle] == 1)
        sprIdx[handle] = 0;
}

#ifdef SAVEDATA
POINT nPosiXSprFile[56] __attribute__((aligned(128))) = {
    {128, 0},  {136, 0},  {144, 0},  {152, 0},  {160, 0},  {168, 0},  {176, 0},
    {184, 0},  {192, 0},  {200, 0},  {208, 0},  {216, 0},  {144, 10}, {152, 10},
    {160, 10}, {168, 10}, {176, 10}, {184, 10}, {192, 10}, {212, 4},  {220, 4},
    {228, 4},  {236, 4},  {244, 4},  {252, 4},  {260, 4},  {268, 4},  {276, 4},
    {284, 4},  {292, 4},  {228, 14}, {236, 14}, {244, 14}, {252, 14}, {260, 14},
    {268, 14}, {276, 14}, {284, 14}, {292, 14}, {300, 14}, {308, 14}, {316, 14},
    {324, 14}, {332, 14}, {160, 8},  {168, 8},  {176, 8},  {184, 8},  {192, 8},
    {200, 8},  {208, 8},  {216, 8},  {224, 8},  {232, 8},  {240, 8},  {248, 8}};
Sint32 PRIO_MAX = 160;
Uint16 NUM_BMP = 209;
Sint32 PLAYERCHAR_W = 8;
Uint16 NUM_GRIDBMP = 1;
#endif

#ifdef SOUNDTST
Uint16 NUM_BMP __attribute__((aligned(128))) = 100;
Uint16 NUM_GRIDBMP = 1;
#endif

#ifdef STAGETST
Uint16 NUM_BMP __attribute__((aligned(128))) = 64;
Uint16 NUM_GRIDBMP = 1;
Sint32 MENUITEMMAX = 70;
#endif
