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
#include "render/r_texture.h"

#define CIRCLE_SEGMENTS 24

typedef struct ent_render_circle_s
{
    RENDER_PLUGIN_COMMON_DATA;

    r_texture_t *texture;
    bool draw;
    float coords[(CIRCLE_SEGMENTS + 1) << 2];

    char *name;
    double width;
    double radius;
}ent_render_circle_t;

static const float circle_segment_texcoords[8] = { 0.0, 0.0, 0.0, 0.9, 1.0, 0.0, 1.0, 0.9 };

GNUC_NONNULL static void circle_recalculate (g_entity_t *ent)
{
    const double coeff = 2.0 * M_PI / CIRCLE_SEGMENTS;
    ent_render_circle_t *r = ent->render_data;
    int i;

    if (r->width < 0.1 || r->radius < 0.1)
    {
        r->draw = false;
        return;
    }

    r->draw = true;

    for (i = 0; i < CIRCLE_SEGMENTS ;i++)
    {
        double rads = i * coeff;
        double x = cos(rads);
        double y = sin(rads);

        r->coords[(i << 2) + 0] = r->radius * x;
        r->coords[(i << 2) + 1] = r->radius * y;
        r->coords[(i << 2) + 2] = (r->radius + r->width) * x;
        r->coords[(i << 2) + 3] = (r->radius + r->width) * y;
    }

    r->coords[(CIRCLE_SEGMENTS << 2) + 0] = r->coords[0];
    r->coords[(CIRCLE_SEGMENTS << 2) + 1] = r->coords[1];
    r->coords[(CIRCLE_SEGMENTS << 2) + 2] = r->coords[2];
    r->coords[(CIRCLE_SEGMENTS << 2) + 3] = r->coords[3];
}

GNUC_NONNULL static void circle_set_texture_callback (g_entity_t *ent)
{
    ent_render_circle_t *r = ent->render_data;

    if (NULL == r->name)
        return;

    if (ent->render_valid)
    {
        r_texture_unload(r->texture);
        ent->render_valid = false;
    }

    if (!r_texture_load(r->name,
                        R_TEX_DEFAULT,
                        &r->texture))
    {
        return;
    }

    ent->render_valid = true;
}

GNUC_NONNULL static void ent_render_circle_unset (g_entity_t *ent)
{
    ent_render_circle_t *r = ent->render_data;

    if (!ent->render_valid)
        return;

    r_texture_unload(r->texture);
}

GNUC_NONNULL static void ent_render_circle_draw (const g_entity_t *ent)
{
    ent_render_circle_t *r = ent->render_data;
    int i;

    if (!r->draw)
        return;

    for (i = 0; i < CIRCLE_SEGMENTS ;i++)
    {
        gl_draw_quad(r->texture->gltex, &r->coords[i << 2], circle_segment_texcoords);
    }
}

static g_field_t ent_fields_render_circle[] =
{
#define STRUCTURE_FOR_OFFSETS ent_render_circle_t
    G_FIELD("texture", name,   STRING, NULL, &circle_set_texture_callback),
    G_FIELD("width",   width,  DOUBLE, 1.0,  &circle_recalculate),
    G_FIELD("radius",  radius, DOUBLE, 1.0,  &circle_recalculate),
    G_FIELD_NULL
};

const g_render_plugin_t g_render_plugin_circle =
{
    .name = "circle",
    .render_data_size = sizeof(ent_render_circle_t),
    .fields = ent_fields_render_circle,
    .unset = &ent_render_circle_unset,
    .draw = &ent_render_circle_draw
};
