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

#include "render/r_private.h"
#include "gl/gl_private.h"
#include "sglib.h"

typedef struct linefont_s
{
    r_linefont_t parms;
    int          ref;
    int          charslen;

    struct linefont_s *next;
}linefont_t;

#define LINEFONT_NAME_COMPARATOR(f1, f2) strcmp((f1)->parms.sprite->name, (f2)->parms.sprite->name)

SGLIB_DEFINE_SORTED_LIST_PROTOTYPES(linefont_t, LINEFONT_NAME_COMPARATOR, next);
SGLIB_DEFINE_SORTED_LIST_FUNCTIONS(linefont_t, LINEFONT_NAME_COMPARATOR, next);

static linefont_t *fonts;

/*
=================
r_linefont_load
=================
*/
bool r_linefont_load (const char *name, const char *chars, r_linefont_t **font)
{
    int charslen;
    r_sprite_t *sprite, tmpsprite = { .name = name, 0 };
    linefont_t *realfont, s = { .parms.sprite = &tmpsprite, 0 };

    realfont = sglib_linefont_t_find_member(fonts, &s);

    while (NULL != realfont)
    {
        if (!strcmp(chars, realfont->parms.chars))
        {
            realfont->ref++;
            *font = (r_linefont_t *)realfont;
            return true;
        }

        realfont = realfont->next;

        if (NULL != realfont && LINEFONT_NAME_COMPARATOR(&s, realfont))
            realfont = NULL;
    }

    if (!r_sprite_load(name, R_SPRITE_TYPE_LINE, true, &sprite))
    {
        sys_printf("can\'t load sprite \"%s\" for linefont\n", name);
        return false;
    }

    charslen = strlen(chars);
    realfont = mem_alloc(r_mempool, sizeof(*realfont) + charslen + 1);
    realfont->parms.sprite = sprite;
    realfont->parms.chars = (void *)realfont + sizeof(*realfont);
    realfont->charslen = charslen;
    realfont->ref = 1;
    strlcpy(realfont->parms.chars, chars, charslen + 1);

    *font = (r_linefont_t *)realfont;

    sglib_linefont_t_add(&fonts, realfont);

    return true;
}

/*
=================
r_linefont_unload
=================
*/
void r_linefont_unload (r_linefont_t *font)
{
    linefont_t *realfont = (linefont_t *)font;

    if (--realfont->ref > 0)
        return;

    r_sprite_unload(realfont->parms.sprite);

    sglib_linefont_t_delete(&fonts, realfont);
    mem_free(realfont);
}

/*
=================
r_list_linefonts_f
=================
*/
static void r_list_linefonts_f (GNUC_UNUSED const struct cmd_s *cmd,
                                int source,
                                GNUC_UNUSED int argc,
                                GNUC_UNUSED const char **argv)
{
    struct sglib_linefont_t_iterator it;
    const linefont_t *font;

    if (source == CMD_SRC_KEY_UP)
        return;

    sys_printf("----------- linefonts list -----------\n");

    for (font = sglib_linefont_t_it_init(&it, fonts);
         NULL != font;
         font = sglib_linefont_t_it_next(&it))
    {
        sys_printf("linefont: %p name=%s chars=\"%s\" ref=%i\n",
                   font,
                   font->parms.sprite->name,
                   font->parms.chars,
                   font->ref);
    }
}

/*
=================
r_linefont_draw
=================
*/
void r_linefont_draw (r_linefont_t *font, const char *text)
{
    linefont_t *realfont = (linefont_t *)font;
    int i, j;

    for (i = 0; text[i] ;i++)
    {
        for (j = 0; j < realfont->charslen ;j++)
        {
            if (text[i] == font->chars[j])
            {
                const float *coords = &font->sprite->frames_coords[j << 3];
                double width = (coords[6] - coords[0]) * font->sprite->frames[0]->w;
                double height = font->sprite->frames[0]->h;
                r_sprite_draw(font->sprite, j, width, height);
                gl_translate_rotate(width, 0.0, 0.0);
                break;
            }
        }
    }
}

/*
=================
r_linefont_init
=================
*/
bool r_linefont_init (void)
{
    fonts = NULL;

    cmd_register("r_list_linefonts", NULL, &r_list_linefonts_f, 0);

    return true;
}

/*
=================
r_linefont_shutdown
=================
*/
void r_linefont_shutdown (void)
{
    while (NULL != fonts)
        r_linefont_unload((r_linefont_t *)fonts);
}
