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

#ifndef _G_ENTITY_RENDER_H
#define _G_ENTITY_RENDER_H

#include "r_sprite.h"
#include "r_figure.h"

typedef enum
{
    RENDER_TYPE_SPRITE = 0,
    RENDER_TYPE_TEXT,
    RENDER_TYPE_MODEL,
    RENDER_TYPE_CIRCLE,
    RENDER_TYPE_LINE
}render_types_e;

/* ent_render_funcs array is in the end of file */

/*
=================
ent_render_load_sprite
=================
*/
GNUC_NONNULL static bool ent_render_load_sprite (const char *name, const double *parms, g_entity_t *ent)
{
    r_sprite_t *sprite;

    if (!r_sprite_load(name,
                       ((int)parms[0]) ? R_TEX_DEFAULT : R_TEX_SCREEN_UI,
                       &sprite))
    {
        return false;
    }

    ent->render_type = 0;
    ent->render_data = sprite;
    ent->frame = 0;
    ent->frames_num = sprite->frames_num;
    ent->width = parms[1];
    ent->height = parms[2];

    if (!(int)ent->width)
        ent->width = sprite->frames[0]->w / sprite->frames_num;

    if (!(int)ent->height)
        ent->height = sprite->frames[0]->h;

    return true;
}

/*
=================
ent_render_unload_sprite
=================
*/
GNUC_NONNULL static void ent_render_unload_sprite (g_entity_t *ent)
{
    r_sprite_unload(ent->render_data);
    ent->render_data = NULL;
}

/*
=================
ent_render_load_text
=================
*/
GNUC_NONNULL static bool ent_render_load_text (GNUC_UNUSED const char *name, GNUC_UNUSED const double *parms, GNUC_UNUSED g_entity_t *ent)
{
    /* FIXME */
    return false;
}

/*
=================
ent_render_unload_text
=================
*/
GNUC_NONNULL static void ent_render_unload_text (GNUC_UNUSED g_entity_t *ent)
{
    /* FIXME */
}

/*
=================
ent_render_load_model
=================
*/
GNUC_NONNULL static bool ent_render_load_model (GNUC_UNUSED const char *name, GNUC_UNUSED const double *parms, GNUC_UNUSED g_entity_t *ent)
{
    /* FIXME */
    return false;
}

/*
=================
ent_render_unload_model
=================
*/
GNUC_NONNULL static void ent_render_unload_model (GNUC_UNUSED g_entity_t *ent)
{
    /* FIXME */
}

GNUC_NONNULL static bool ent_render_load_circle (const char *name, const double *parms, g_entity_t *ent)
{
    if (!r_figure_set_circle(name, parms[0], parms[1], &ent->render_data))
        return false;

    ent->render_type = RENDER_TYPE_CIRCLE;

    return true;
}

GNUC_NONNULL static bool ent_render_load_line (const char *name, const double *parms, g_entity_t *ent)
{
    if (!r_figure_set_line(name, parms, parms[3], &ent->render_data))
        return false;

    ent->render_type = RENDER_TYPE_LINE;

    return true;
}

GNUC_NONNULL static void ent_render_unload_circle (g_entity_t *ent)
{
    r_figure_unset(ent->render_data);
}

GNUC_NONNULL static void ent_render_unload_line (g_entity_t *ent)
{
    r_figure_unset(ent->render_data);
}

GNUC_NONNULL static void ent_render_draw_sprite (const g_entity_t *ent)
{
    r_sprite_draw(ent->render_data,
                  ent->frame,
                  ent->origin,
                  ent->color,
                  ent->alpha,
                  ent->width,
                  ent->height,
                  ent->scale,
                  ent->angle);
}

GNUC_NONNULL static void ent_render_draw_text (GNUC_UNUSED const g_entity_t *ent)
{
}

GNUC_NONNULL static void ent_render_draw_model (GNUC_UNUSED const g_entity_t *ent)
{
}

GNUC_NONNULL static void ent_render_draw_figure (const g_entity_t *ent)
{
    r_figure_draw(ent->render_data,
                  ent->origin,
                  ent->color,
                  ent->alpha,
                  ent->scale);
}

#define RENDER_ADD(type, name, draw)                             \
    [RENDER_TYPE_##type] = {                                     \
        &ent_render_load_##name,                                 \
        &ent_render_unload_##name,                               \
        &ent_render_draw_##draw                                  \
    }

static const struct
{
    bool (*load) (const char *name, const double *parms, g_entity_t *ent);
    void (*unload) (g_entity_t *ent);
    void (*draw) (const g_entity_t *ent);
}ent_render_funcs[] =
{
    RENDER_ADD(SPRITE, sprite, sprite),
    RENDER_ADD(TEXT,   text,   text),
    RENDER_ADD(MODEL,  model,  model),
    RENDER_ADD(CIRCLE, circle, figure),
    RENDER_ADD(LINE,   line,   figure)
};

#undef RENDER_ADD

#endif /* !_G_ENTITY_RENDER_H */
