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

    const char *fontname;
    const char *chars;

    char *text;
}ent_render_text2_t;

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

    ent->render_valid = true;
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

    if (NULL == r->text)
        return;

    r_linefont_draw(r->font, r->text);
}

static g_field_t ent_fields_render_text2[] =
{
#define STRUCTURE_FOR_OFFSETS ent_render_text2_t
    G_FIELD("font",  fontname, STRING,      NULL, &text2_set_font_callback),
    G_FIELD("chars", chars,    STRING,      NULL, &text2_set_font_callback),
    G_FIELD("text",  text,     STRING_COPY, NULL, NULL), 
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
