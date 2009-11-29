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

#include "game/g_render_private.h"
#include "gl/gl_private.h"

typedef struct ent_render_scissor_s
{
    RENDER_PLUGIN_COMMON_DATA;

    int width;
    int height;
    bool disabler;
}ent_render_scissor_t;

/*
=================
ent_render_scissor_draw
=================
*/
GNUC_NONNULL static void ent_render_scissor_draw (const g_entity_t *ent)
{
    ent_render_scissor_t *r = ent->render_data;

    if (r->disabler)
    {
        glDisable(GL_SCISSOR_TEST);
    }
    else
    {
        glEnable(GL_SCISSOR_TEST);
        glScissor(ent->origin[0], ent->origin[1], r->width, r->height);
    }
}

/*
=================
ent_render_scissor_set
=================
*/
GNUC_NONNULL static void ent_render_scissor_set (g_entity_t *ent)
{
    ent->render_valid = true;
}

static g_field_t ent_fields_render_scissor[] =
{
#define STRUCTURE_FOR_OFFSETS ent_render_scissor_t
    G_FIELD("width",    width,    INTEGER, 0,    NULL),
    G_FIELD("height",   height,   INTEGER, 0,    NULL),
    G_FIELD("disabler", disabler, BOOL,    true, NULL),
    G_FIELD_NULL
};

const g_render_plugin_t g_render_plugin_scissor =
{
    .name = "scissor",
    .render_data_size = sizeof(ent_render_scissor_t),
    .fields = ent_fields_render_scissor,
    .set = &ent_render_scissor_set,
    .draw = &ent_render_scissor_draw
};
