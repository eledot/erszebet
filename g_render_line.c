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

#include "g_private.h"
#include "g_render.h"
#include "r_texture.h"

typedef struct ent_render_line_s
{
    RENDER_PLUGIN_COMMON_DATA;

    r_texture_t *texture;
    bool draw;
    float verts[8];
    float texcoords[8];

    char *name;
    double width;
    double origin2[3];
}ent_render_line_t;

GNUC_NONNULL static void line_recalculate (g_entity_t *ent)
{
    const double zero[3] = { 0.0, 0.0, 1.0 };
    ent_render_line_t *r = ent->render_data;
    double length, v[3], n[3];

    if (r->width < 0.1)
    {
        r->draw = false;
        return;
    }

    vector_sub(r->origin2, ent->origin, v);
    length = vector_length(v);

    if (length < 0.1)
    {
        r->draw = false;
        return;
    }

    r->draw = true;

    vector_normal(v, zero, n);
    vector_normalize(n);
    vector_mul(n, r->scale * r->width / 2.0, n);

    r->verts[0] = n[0];
    r->verts[1] = n[1];
    r->verts[2] = -n[0];
    r->verts[3] = -n[1];

    vector_add(r->origin2, n, &r->verts[4]);
    vector_sub(&r->verts[4], ent->origin, &r->verts[4]);
    vector_sub(r->origin2, n, &r->verts[6]);
    vector_sub(&r->verts[6], ent->origin, &r->verts[6]);

    r->texcoords[0] = 0.0; r->texcoords[1] = length / r->texture->w;
    r->texcoords[2] = 1.0; r->texcoords[3] = r->texcoords[1];
    r->texcoords[4] = 0.0; r->texcoords[5] = 0.0;
    r->texcoords[6] = 1.0; r->texcoords[7] = 0.0;
}

GNUC_NONNULL static void line_set_texture_callback (g_entity_t *ent)
{
    ent_render_line_t *r = ent->render_data;

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

GNUC_NONNULL static void ent_render_line_unset (g_entity_t *ent)
{
    ent_render_line_t *r = ent->render_data;

    if (!ent->render_valid)
        return;

    r_texture_unload(r->texture);
}

GNUC_NONNULL static void ent_render_line_draw (const g_entity_t *ent)
{
    ent_render_line_t *r = ent->render_data;

    if (!r->draw)
        return;

    gl_draw_quad(r->texture->gltex, r->verts, r->texcoords);
}

static ent_field_t ent_fields_render_line[] =
{
    RENDER_FIELD(line, "texture", name, ENT_FIELD_TYPE_STRING, &line_set_texture_callback),
    RENDER_FIELD(line, "width", width, ENT_FIELD_TYPE_DOUBLE, &line_recalculate),
    RENDER_FIELD(line, "origin2", origin2, ENT_FIELD_TYPE_VECTOR, &line_recalculate),
    RENDER_FIELD(line, "scale", scale, ENT_FIELD_TYPE_DOUBLE, &line_recalculate),
    RENDER_FIELD_NULL
};

const ent_render_plugin_t ent_render_plugin_line =
{
    .name = "line",
    .render_data_size = sizeof(ent_render_line_t),
    .entity_fields = ent_fields_render_line,
    .unset = &ent_render_line_unset,
    .draw = &ent_render_line_draw
};
