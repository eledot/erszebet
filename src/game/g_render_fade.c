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
#include "game/g_private.h"
#include "gl/gl.h"

typedef struct ent_render_fade_s
{
    RENDER_PLUGIN_COMMON_DATA;

    erbool done;
    double alpha_end;
    double alpha_start;
    double length;
    double time_start;
}ent_render_fade_t;

/*
=================
fade_set_alpha_end_callback
=================
*/
GNUC_NONNULL static void fade_set_alpha_end_callback (g_entity_t *ent)
{
    ent_render_fade_t *r = ent->render_data;

    r->alpha_start = r->alpha;
    r->done = false;
}

/*
=================
fade_set_length_callback
=================
*/
GNUC_NONNULL static void fade_set_length_callback (g_entity_t *ent)
{
    ent_render_fade_t *r = ent->render_data;

    r->time_start = g_real_time();
    ent->render_valid = (r->length > 0);
    r->done = false;
}

/*
=================
ent_render_fade_draw
=================
*/
GNUC_NONNULL static void ent_render_fade_draw (g_entity_t *ent)
{
    ent_render_fade_t *r = ent->render_data;
    const float verts[8] = { -512, -512, 512, -512, -512, 512, 512, 512 };
    const float texcoords[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

    gl_draw_quad(0, verts, texcoords);

    if (r->done)
        return;

    if (r->time_start + r->length < g_real_time())
    {
        r->done = true;
        r->alpha = r->alpha_end;
        ent->forced_think = true;
    }
    else
    {
        double scale = 1.0 - (r->time_start - g_real_time() + r->length) / r->length;
        r->alpha = r->alpha_start + scale * (r->alpha_end - r->alpha_start);
    }
}

static g_field_t ent_fields_render_fade[] =
{
#define STRUCTURE_FOR_OFFSETS ent_render_fade_t
    G_FIELD("alpha_end", alpha_end, DOUBLE,  0.0, &fade_set_alpha_end_callback),
    G_FIELD("length",    length,    DOUBLE,  0.0, &fade_set_length_callback),
    G_FIELD_NULL
};

const g_render_plugin_t g_render_plugin_fade =
{
    .name = "fade",
    .render_data_size = sizeof(ent_render_fade_t),
    .fields = ent_fields_render_fade,
    .draw = &ent_render_fade_draw
};
