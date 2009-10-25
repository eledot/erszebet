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
#include "render/r_sprite.h"

typedef struct ent_render_sprite_s
{
    RENDER_PLUGIN_COMMON_DATA;

    r_sprite_t *sprite;

    char *name;
    double width;
    double height;
    bool ui;
    int frame;
}ent_render_sprite_t;

GNUC_NONNULL static void sprite_set_name_callback (g_entity_t *ent)
{
    ent_render_sprite_t *r = ent->render_data;

    if (NULL == r->name)
        return;

    if (ent->render_valid)
    {
        r_sprite_unload(r->sprite);
        ent->render_valid = false;
    }

    if (!r_sprite_load(r->name,
                       r->ui ? R_TEX_SCREEN_UI : R_TEX_DEFAULT,
                       &r->sprite))
    {
        return;
    }

    ent->render_valid = true;
    r->frame = 0;
    r->width = r->sprite->frames[0]->w / r->sprite->frames_num;
    r->height = r->sprite->frames[0]->h;
}

GNUC_NONNULL static void sprite_set_frame_callback (g_entity_t *ent)
{
    ent_render_sprite_t *r = ent->render_data;

    if (!ent->render_valid)
        return;

    if (r->sprite->frames_num)
        r->frame %= r->sprite->frames_num;
    else
        r->frame = 0;
}

GNUC_NONNULL static void ent_render_sprite_unset (g_entity_t *ent)
{
    ent_render_sprite_t *r = ent->render_data;

    if (!ent->render_valid)
        return;

    r_sprite_unload(r->sprite);
}

GNUC_NONNULL static void ent_render_sprite_draw (const g_entity_t *ent)
{
    ent_render_sprite_t *r = ent->render_data;

    r_sprite_draw(r->sprite,
                  r->frame,
                  ent->origin,
                  r->color,
                  r->alpha,
                  r->width,
                  r->height,
                  r->scale,
                  ent->angle);
}

static g_entity_field_t ent_fields_render_sprite[] =
{
#define STRUCTURE_FOR_OFFSETS ent_render_sprite_t
    ENTITY_FIELD("sprite", name,   STRING,  &sprite_set_name_callback),
    ENTITY_FIELD("width",  width,  DOUBLE,  NULL),
    ENTITY_FIELD("height", height, DOUBLE,  NULL),
    ENTITY_FIELD("frame",  frame,  INTEGER, &sprite_set_frame_callback),
    ENTITY_FIELD_NULL
};

const g_render_plugin_t g_render_plugin_sprite =
{
    .name = "sprite",
    .render_data_size = sizeof(ent_render_sprite_t),
    .entity_fields = ent_fields_render_sprite,
    .unset = &ent_render_sprite_unset,
    .draw = &ent_render_sprite_draw
};
