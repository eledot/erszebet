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
#include "render/r_font.h"
#include "gl/gl_private.h"

typedef enum
{
    G_TEXT_ALIGN_LEFT   = (1 << 0),
    G_TEXT_ALIGN_RIGHT  = (1 << 1),
    G_TEXT_ALIGN_TOP    = (1 << 2),
    G_TEXT_ALIGN_BOTTOM = (1 << 3)
}g_text_alignments_e;

typedef struct ent_render_text_s
{
    RENDER_PLUGIN_COMMON_DATA;

    r_font_t *font;
    int gltex;
    int w;
    int h;
    int texw;
    int texh;

    float verts[8];

    const char *fontname;
    int ptsize;
    int align;
    char *text;
}ent_render_text_t;

static const int default_align = G_TEXT_ALIGN_LEFT | G_TEXT_ALIGN_TOP;

GNUC_NONNULL static void text_redraw_callback (g_entity_t *ent);

GNUC_NONNULL static void text_set_font_callback (g_entity_t *ent)
{
    ent_render_text_t *r = ent->render_data;

    if (ent->render_valid)
    {
        ent->render_valid = false;
        r_font_unload(r->font);
        gl_texture_delete(r->gltex);
        r->gltex = 0;
        r->w = r->h = 0;
        r->fontname = NULL;
    }

    if (NULL == r->fontname || r->ptsize < 1)
        return;

    if (!r_font_load(r->fontname, r->ptsize, &r->font))
        return;

    ent->render_valid = true;
    r->fontname = r->font->name;
    r->ptsize = r->font->ptsize;

    text_redraw_callback(ent);
}

GNUC_NONNULL static void text_set_align_callback (g_entity_t *ent)
{
    ent_render_text_t *r = ent->render_data;
    float originx = 0.0f, originy = 0.0f;

    if (!ent->render_valid || !r->gltex)
        return;

    if (r->align & G_TEXT_ALIGN_TOP)
        originy -= r->h;
    else if (!(r->align & G_TEXT_ALIGN_BOTTOM))
        originy -= r->h / 2.0;

    if (r->align & G_TEXT_ALIGN_RIGHT)
        originx -= r->w;
    else if (!(r->align & G_TEXT_ALIGN_LEFT))
        originx -= r->w / 2.0;

    r->verts[0] = r->verts[4] = originx;
    r->verts[1] = r->verts[3] = originy;
    r->verts[5] = r->verts[7] = originy + r->texh;
    r->verts[6] = r->verts[2] = originx + r->texw;

    sys_printf("%-2.2f %-2.2f\n", originx, originy);
}

GNUC_NONNULL static void text_redraw_callback (g_entity_t *ent)
{
    ent_render_text_t *r = ent->render_data;

    if (!ent->render_valid)
        return;

    gl_texture_delete(r->gltex);
    r->gltex = 0;

    if (NULL == r->text)
        return;

    r_font_draw_to_texture(r->font,
                           r->text,
                           &r->gltex,
                           &r->w,
                           &r->h,
                           &r->texw,
                           &r->texh);

    text_set_align_callback(ent);
}

GNUC_NONNULL static void ent_render_text_unset (g_entity_t *ent)
{
    ent_render_text_t *r = ent->render_data;

    if (!ent->render_valid)
        return;

    r_font_unload(r->font);
    gl_texture_delete(r->gltex);
}

GNUC_NONNULL static void ent_render_text_draw (const g_entity_t *ent)
{
    ent_render_text_t *r = ent->render_data;
    const float texcoords[8] = { 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f };

    if (!r->gltex)
        return;

    gl_draw_quad(r->gltex, r->verts, texcoords);
}

/*
=================
ent_render_text_init
=================
*/
static bool ent_render_text_init (void)
{
    g_set_integer("G_TEXT_ALIGN_LEFT", G_TEXT_ALIGN_LEFT);
    g_set_integer("G_TEXT_ALIGN_RIGHT", G_TEXT_ALIGN_RIGHT);
    g_set_integer("G_TEXT_ALIGN_TOP", G_TEXT_ALIGN_TOP);
    g_set_integer("G_TEXT_ALIGN_BOTTOM", G_TEXT_ALIGN_BOTTOM);

    return true;
}

static g_field_t ent_fields_render_text[] =
{
#define STRUCTURE_FOR_OFFSETS ent_render_text_t
    G_FIELD("font",   fontname, STRING,      NULL,          &text_set_font_callback),
    G_FIELD("ptsize", ptsize,   INTEGER,     0,             &text_set_font_callback),
    G_FIELD("align",  align,    INTEGER,     default_align, &text_set_align_callback),
    G_FIELD("width",  w,        INTEGER,     0,             NULL),
    G_FIELD("height", h,        INTEGER,     0,             NULL),
    G_FIELD("text",   text,     STRING_COPY, NULL,          &text_redraw_callback), 
    G_FIELD_NULL
};

const g_render_plugin_t g_render_plugin_text =
{
    .name = "text",
    .render_data_size = sizeof(ent_render_text_t),
    .fields = ent_fields_render_text,
    .unset = &ent_render_text_unset,
    .draw = &ent_render_text_draw,
    .init = ent_render_text_init
};
