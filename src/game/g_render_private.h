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

#ifndef _G_RENDER_PRIVATE_H
#define _G_RENDER_PRIVATE_H

#include "common.h"
#include "game/g_field.h"
#include "game/g_entity.h"

typedef enum
{
    G_TEXT_ALIGN_LEFT   = (1 << 0),
    G_TEXT_ALIGN_RIGHT  = (1 << 1),
    G_TEXT_ALIGN_TOP    = (1 << 2),
    G_TEXT_ALIGN_BOTTOM = (1 << 3)
}g_text_alignments_e;

#define RENDER_PLUGIN_COMMON_DATA               \
    double color[3];                            \
    double alpha;                               \
    double scale

typedef struct g_render_plugin_s
{
    const char * const name;
    g_field_t * const fields;
    const int render_data_size;

    void (* const set) (g_entity_t *ent);
    void (* const unset) (g_entity_t *ent);
    void (* const draw) (const g_entity_t *ent);

    bool (* const init) (void);
    void (* const shutdown) (void);
}g_render_plugin_t;

#endif /* !_G_RENDER_PRIVATE_H */
