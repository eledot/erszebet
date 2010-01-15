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

#include "game/g_private.h"
#include "game/g_render_private.h"
#include "render/r_linefont.h"
#include "gl/gl_private.h"

typedef struct ent_render_text2_s
{
    RENDER_PLUGIN_COMMON_DATA;

    r_linefont_t *font;

    int w;
    int h;
    int align;

    int offset[2];

    const char *fontname;
    const char *chars;

    char *text;
}ent_render_text2_t;

enum { default_align = G_TEXT_ALIGN_LEFT | G_TEXT_ALIGN_TOP };

/*
=================
text2_set_align_callback
=================
*/
GNUC_NONNULL static void text2_set_align_callback (g_entity_t *ent)
{
    ent_render_text2_t *r = ent->render_data;

    if (NULL == r->font || NULL == r->text)
        return;

    r_linefont_get_text_window(r->font, r->text, &r->w, &r->h);

    r->offset[0] = r->offset[1] = 0;

    if (r->align & G_TEXT_ALIGN_BOTTOM)
        r->offset[1] -= r->h;
    else if (!(r->align & G_TEXT_ALIGN_TOP))
        r->offset[1] -= r->h / 2;

    if (r->align & G_TEXT_ALIGN_RIGHT)
        r->offset[0] -= r->w;
    else if (!(r->align & G_TEXT_ALIGN_LEFT))
        r->offset[0] -= r->w / 2;

    ent->render_valid = true;
}

/*
=================
text2_set_font_callback
=================
*/
GNUC_NONNULL static void text2_set_font_callback (g_entity_t *ent)
{
    ent_render_text2_t *r = ent->render_data;

    if (ent->render_valid)
    {
        ent->render_valid = false;
        r_linefont_unload(r->font);
        r->font = NULL;
    }

    if (NULL == r->fontname || NULL == r->chars)
        return;

    if (!r_linefont_load(r->fontname, r->chars, &r->font))
        return;

    ent->render_valid = (NULL != r->text);
    r->fontname = r->font->sprite->name;
    r->chars = r->font->chars;
}

/*
=================
ent_render_text2_unset
=================
*/
GNUC_NONNULL static void ent_render_text2_unset (g_entity_t *ent)
{
    ent_render_text2_t *r = ent->render_data;

    if (!ent->render_valid)
        return;

    r_linefont_unload(r->font);
}

/*
=================
ent_render_text2_draw
=================
*/
GNUC_NONNULL static void ent_render_text2_draw (const g_entity_t *ent)
{
    ent_render_text2_t *r = ent->render_data;

    gl_translate_rotate(r->offset[0], r->offset[1], 0);
    r_linefont_draw(r->font, r->text);
}

static g_field_t ent_fields_render_text2[] =
{
#define STRUCTURE_FOR_OFFSETS ent_render_text2_t
    G_FIELD("align", align,    INTEGER,     default_align, &text2_set_align_callback),
    G_FIELD("font",  fontname, STRING,      NULL,          &text2_set_font_callback),
    G_FIELD("chars", chars,    STRING,      NULL,          &text2_set_font_callback),
    G_FIELD("text",  text,     STRING_COPY, NULL,          &text2_set_align_callback), 
    G_FIELD_NULL
};

const g_render_plugin_t g_render_plugin_text2 =
{
    .name = "text2",
    .render_data_size = sizeof(ent_render_text2_t),
    .fields = ent_fields_render_text2,
    .unset = &ent_render_text2_unset,
    .draw = &ent_render_text2_draw,
};
