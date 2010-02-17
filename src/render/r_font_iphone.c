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

#ifdef ENGINE_OS_IPHONE

#include <QuartzCore/QuartzCore.h>
#include <CoreGraphics/CoreGraphics.h>
#include <CoreGraphics/CGColor.h>

#include "render/r_private.h"
#include "gl/gl_private.h"
#include "sglib.h"

typedef struct font_s
{
    r_font_t  parms;
    int       ref;

    int glyph_delta;
    int num_glyphs;
    int units_per_em;
    int ascent;
    int descent;
    int height;

    CGFontRef cg_font;

    struct font_s *next;
}font_t;

#define FONT_NAME_COMPARATOR(f1, f2) strcmp((f1)->parms.name, (f2)->parms.name)

SGLIB_DEFINE_SORTED_LIST_PROTOTYPES(font_t, FONT_NAME_COMPARATOR, next);
SGLIB_DEFINE_SORTED_LIST_FUNCTIONS(font_t, FONT_NAME_COMPARATOR, next);

static font_t *fonts;

/*
=================
r_font_load
=================
*/
erbool r_font_load (const char *name, int ptsize, r_font_t **font)
{
    char tmp[MISC_MAX_FILENAME], *namecopy;
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

    CGDataProviderRef provider = fs_get_data_provider(tmp);

    if (NULL == provider)
    {
        sys_printf("\"%s\": NULL provider\n", name);
        return false;
    }

    CGFontRef cg_font = CGFontCreateWithDataProvider(provider);
    CGDataProviderRelease(provider);

    if (NULL == cg_font)
    {
        sys_printf("\"%s\": failed to create font from file\n", name);
        return false;
    }

    int num_glyphs = CGFontGetNumberOfGlyphs(cg_font);
    int glyph_delta = CGFontGetGlyphWithGlyphName(cg_font, CFSTR("A")) - 'A';
    int units_per_em = CGFontGetUnitsPerEm(cg_font);
    int ascent = CGFontGetAscent(cg_font);
    int descent = CGFontGetDescent(cg_font);

    int namelen = strlen(name);

    realfont = mem_alloc(r_mempool, sizeof(*realfont) + namelen + 1);

    namecopy = (void *)realfont + sizeof(*realfont);
    realfont->parms.name = namecopy;
    realfont->parms.ptsize = ptsize;
    realfont->ref = 1;
    strlcpy(namecopy, name, namelen + 1);

    realfont->num_glyphs = num_glyphs;
    realfont->glyph_delta = glyph_delta;
    realfont->units_per_em = units_per_em;
    realfont->ascent = ascent;
    realfont->descent = descent;
    realfont->height = (ascent - descent) * ptsize / units_per_em;

    realfont->cg_font = cg_font;

    *font = (r_font_t *)realfont;
    sglib_font_t_add(&fonts, realfont);

    return true;
}

/*
=================
get_text_width
=================
*/
static int get_text_width (const CGGlyph *glyphs, int len, const font_t *realfont)
{
    int advances[len];
    CGFontGetGlyphAdvances(realfont->cg_font, glyphs, len, advances);

    int advance = 0;
    for (int i = 0; i < len ;i++)
        advance += advances[i];

    return advance * realfont->parms.ptsize / realfont->units_per_em;
}

extern void CGFontGetGlyphsForUnichars(CGFontRef, const UniChar[], const CGGlyph[], size_t);

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
    CFStringRef string = CFStringCreateWithBytes(nil, (const unsigned char *)text, strlen(text), kCFStringEncodingUTF8, false);
    int len = CFStringGetLength(string);
    CGGlyph glyphs[len];
    UniChar chars[len];

    for (int i = 0; i < len ;i++)
        chars[i] = CFStringGetCharacterAtIndex(string, i);

    CGFontGetGlyphsForUnichars(realfont->cg_font, chars, glyphs, len);

    *w = get_text_width(glyphs, len, realfont);
    *h = realfont->height * 1.25;

    unsigned char *data = mem_alloc(r_mempool, (*w) * (*h));
    CGColorSpaceRef space = CGColorSpaceCreateDeviceGray();
    CGContextRef context = CGBitmapContextCreate(data, *w, *h, 8, *w, space, kCGImageAlphaNone);
    float components[] = { 1, 1 };
    CGColorRef color = CGColorCreate(space, components);
    CGColorSpaceRelease(space);

    CGContextSetTextDrawingMode(context, kCGTextFill);
    CGContextSetFillColorWithColor(context, color);
    CGContextSetStrokeColorWithColor(context, color);

    CGContextSetFont(context, realfont->cg_font);
    CGContextSetFontSize(context, realfont->parms.ptsize);

    CGContextShowGlyphsAtPoint(context, 0, (*h) - realfont->parms.ptsize, glyphs, len);
    CGContextFlush(context);

    image_t im = { .name = "text", .miplevels = 0, .format = 0 };
    im.width = *w;
    im.height = *h;
    im.data_size = (*w) * (*h) << 2;
    im.data = mem_alloc(r_mempool, im.data_size);

    for (int i = 0; i < *h ;i++)
    {
        unsigned char *target = im.data + (*w) * (i << 2);

        for (int j = 0; j < *w ;j++)
        {
            unsigned char c = data[(*w) * i + j];
            target[(j << 2) + 0] = target[(j << 2) + 1] = target[(j << 2) + 2] = 0xff;
            target[(j << 2) + 3] = c;
        }
    }
 
    if (!gl_texture_create(&im, GL_TEX_FL_UI, gltex, texw, texh))
    {
        sys_printf("failed to draw to texture (gl_texture_create)\n");
    }

    if (NULL != im.data)
        mem_free(im.data);

    CGColorRelease(color);
    CGContextRelease(context);
    mem_free(data);
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

    CGFontRelease(realfont->cg_font);
    sglib_font_t_delete(&fonts, realfont);
    mem_free(realfont);
}

/*
=================
r_list_fonts_f
=================
*/
static void r_list_fonts_f (GNUC_UNUSED const struct cmd_s *cmd,
                            int source,
                            GNUC_UNUSED int argc,
                            GNUC_UNUSED const char **argv)
{
    struct sglib_font_t_iterator it;
    const font_t *font;

    if (source == CMD_SRC_KEY_UP)
        return;

    sys_printf("----------- fonts list -----------\n");

    for (font = sglib_font_t_it_init(&it, fonts);
         NULL != font;
         font = sglib_font_t_it_next(&it))
    {
        sys_printf("font: %p name=%s ptsize=%i ref=%i\n",
                   font,
                   font->parms.name,
                   font->parms.ptsize,
                   font->ref);
    }
}

/*
=================
r_font_init
=================
*/
erbool r_font_init (void)
{
    fonts = NULL;

    cmd_register("r_list_fonts", NULL, &r_list_fonts_f, 0);

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
}

#endif /* ENGINE_OS_IPHONE */
