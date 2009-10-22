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

#ifndef _G_RENDER_H
#define _G_RENDER_H

#define RENDER_PLUGIN_COMMON_DATA               \
    double color[3];                            \
    double alpha;                               \
    double scale

#define RENDER_FIELD(render_name, field_name, field_real, field_type, set_callback) \
    {                                                                     \
        .name = field_name,                                               \
        .offset = FIELD_OFFSET(ent_render_##render_name##_t, field_real), \
        .type = field_type,                                               \
        .callback = set_callback                                          \
    }

#define RENDER_FIELD_NULL { .name = NULL }

typedef struct ent_render_plugin_s
{
    const char * const name;
    ent_field_t * const entity_fields;
    const int render_data_size;

    void (* const set) (g_entity_t *ent);
    void (* const unset) (g_entity_t *ent);
    void (* const draw) (const g_entity_t *ent);

    bool (* const init) (void);
    void (* const shutdown) (void);
}ent_render_plugin_t;

void g_render_entity (const g_entity_t *ent) GNUC_NONNULL;

void g_render_init (void);
void g_render_shutdown (void);

#endif /* !_G_RENDER_H */
