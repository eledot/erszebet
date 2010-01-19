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

#include "render/r_private.h"
#include "gl/gl_private.h"
#include "sglib.h"

typedef struct font_s
{
    r_font_t parms;
    int      ref;

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
}

/*
=================
r_font_unload
=================
*/
void r_font_unload (r_font_t *font)
{
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
