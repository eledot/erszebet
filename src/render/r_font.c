/*
   Copyright (C) 2009 Serge "ftrvxmtrx" Ziryukin

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General
   Public License along with this library; if not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301 USA
*/

#include <SDL_ttf.h>

#include "render/r_private.h"
#include "gl/gl_private.h"
#include "sglib.h"

typedef struct font_s
{
    r_font_t         parms;
    TTF_Font        *ttf;
    struct SDL_RWops context;
    int              ref;

    struct font_s *next;
}font_t;

#define FONT_NAME_COMPARATOR(f1, f2) strcmp((f1)->parms.name, (f2)->parms.name)

SGLIB_DEFINE_SORTED_LIST_PROTOTYPES(font_t, FONT_NAME_COMPARATOR, next);
SGLIB_DEFINE_SORTED_LIST_FUNCTIONS(font_t, FONT_NAME_COMPARATOR, next);

static font_t *fonts;

static int sdl_rwops_seek (struct SDL_RWops *context, int offset, int whence)
{
    fs_file_t f = context->hidden.unknown.data1;
    int origin = FS_CURRENT;

    if (SEEK_SET == whence)
        origin = FS_START;
    else if (SEEK_END == whence)
        origin = FS_END;

    return fs_seek(f, offset, origin);
}

static int sdl_rwops_read (struct SDL_RWops *context, void *ptr, int size, int maxnum)
{
    fs_file_t f = context->hidden.unknown.data1;

    return fs_read(f, ptr, size * maxnum) / size;
}

static int sdl_rwops_write (GNUC_UNUSED struct SDL_RWops *context, GNUC_UNUSED const void *ptr, GNUC_UNUSED int size, GNUC_UNUSED int num)
{
    sys_printf("write RWop not available\n");
    return 0;
}

static int sdl_rwops_close (struct SDL_RWops *context)
{
    fs_file_t f = context->hidden.unknown.data1;
    fs_close(f);
    context->hidden.unknown.data1 = NULL;
    return 0;
}

/*
=================
r_font_load
=================
*/
bool r_font_load (const char *name, int ptsize, r_font_t **font)
{
    int namelen;
    char tmp[MISC_MAX_FILENAME], *namecopy;
    fs_file_t file = NULL;
    font_t *realfont, s = { .parms.name = name, 0 };

    realfont = sglib_font_t_find_member(fonts, &s);

    while (NULL != realfont)
    {
        if (realfont->parms.ptsize == ptsize)
        {
            realfont->ref++;
            *font = (r_font_t *)realfont;
            return true;
        }

        realfont = realfont->next;

        if (NULL != realfont && FONT_NAME_COMPARATOR(&s, realfont))
            realfont = NULL;
    }

    snprintf(tmp, sizeof(tmp), "fonts/%s.ttf", name);

    file = fs_open(tmp, FS_RDONLY, NULL, 1);

    if (NULL == file)
    {
        sys_printf("font \"%s\" not found\n", name);
        return false;
    }

    namelen = strlen(name);

    realfont = mem_alloc(r_mempool, sizeof(*realfont) + namelen + 1);

    realfont->context.hidden.unknown.data1 = file;
    realfont->context.seek = &sdl_rwops_seek;
    realfont->context.read = &sdl_rwops_read;
    realfont->context.write = &sdl_rwops_write;
    realfont->context.close = &sdl_rwops_close;

    realfont->ttf = TTF_OpenFontRW(&realfont->context, 1, ptsize);

    if (NULL == realfont->ttf)
    {
        sys_printf("failed to load font \"%s\"\n", name);
        goto error;
    }

    namecopy = (void *)realfont + sizeof(*realfont);
    realfont->parms.name = namecopy;
    realfont->parms.ptsize = ptsize;
    realfont->ref = 1;
    strlcpy(namecopy, name, namelen + 1);

    *font = (r_font_t *)realfont;

    sglib_font_t_add(&fonts, realfont);

    return true;

error:
    if (NULL != file)
        fs_close(file);

    if (NULL != realfont)
        mem_free(realfont);

    return false;
}

/*
=================
r_font_draw_to_texture
=================
*/
void r_font_draw_to_texture (r_font_t *font,
                             const char *text,
                             int *gltex,
                             int *w,
                             int *h,
                             int *texw,
                             int *texh)
{
    font_t *realfont = (font_t *)font;
    SDL_Color white = { 255, 255, 255, 0 };
    SDL_Surface *surface = TTF_RenderUTF8_Blended(realfont->ttf, text, white);
    image_t image = { .name = "text", .miplevels = 0, .format = 0 };

    *gltex = 0;

    if (NULL == surface)
    {
        sys_printf("failed to draw to texture (TTF_RenderUTF8_Blended)\n");
        return;
    }

    image.width = surface->w;
    image.height = surface->h;
    image.data_size = (surface->w * surface->h) << 2;

    image.data = mem_alloc(r_mempool, image.data_size);
    memcpy(image.data, surface->pixels, image.data_size);

    if (!gl_texture_create(&image, GL_TEX_FL_UI, gltex, texw, texh))
    {
        sys_printf("failed to draw to texture (gl_texture_create)\n");
    }

    mem_free(image.data);

    *w = surface->w;
    *h = surface->h;

    SDL_FreeSurface(surface);
}

/*
=================
r_font_unload
=================
*/
void r_font_unload (r_font_t *font)
{
    font_t *realfont = (font_t *)font;

    if (--realfont->ref > 0)
        return;

    TTF_CloseFont(realfont->ttf);
    sglib_font_t_delete(&fonts, realfont);
    mem_free(realfont);
}

/*
=================
r_font_init
=================
*/
bool r_font_init (void)
{
    const SDL_version *ver;

    ver = TTF_Linked_Version();

    sys_printf("sdl_ttf version: %i.%i.%i\n", ver->major, ver->minor, ver->patch);

    if (0 != TTF_Init())
    {
        sys_printf("TTF_Init failed\n");
        return false;
    }

    fonts = NULL;

    return true;
}

/*
=================
r_font_shutdown
=================
*/
void r_font_shutdown (void)
{
    while (NULL != fonts)
        r_font_unload((r_font_t *)fonts);

    TTF_Quit();
}
