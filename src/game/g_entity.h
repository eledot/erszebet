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

#ifndef _G_ENTITY_H
#define _G_ENTITY_H

typedef enum
{
    ENT_INTFL_THINK          = (1 << 0),
    ENT_INTFL_TOUCH          = (1 << 1),
    ENT_INTFL_PHYSICS_STATIC = (1 << 2)
}g_entity_internal_flags_e;

typedef enum
{
    ENT_FL_STATIC    = (1 << 0),
    ENT_FL_NON_SOLID = (1 << 1)
}g_entity_flags_e;

typedef struct g_entity_s
{
    /* internal fields */
    int lua_ref;
    int lua_dataref;
    int internal_flags;
    void *physics_data;
    bool  render_valid;
    int   render_index;
    void *render_data;

    struct g_entity_s *next;

    /* fields accessible by lua code */
    char *classname;

    int flags;

    double origin[3];
    double angle;

    double nextthink;
    double lastthink;
}g_entity_t;

typedef enum
{
    ENT_FIELD_INDEX_INVALID = -3,
    ENT_FIELD_INDEX_PHYSICS = -2,
    ENT_FIELD_INDEX_RENDER_COMMON = -1,
    ENT_FIELD_INDEX_BASE = 0
}ent_field_indices_e;

typedef enum
{
    G_FIELD_TYPE_DOUBLE = 0,
    G_FIELD_TYPE_INTEGER,
    G_FIELD_TYPE_VECTOR,
    G_FIELD_TYPE_STRING,
    G_FIELD_TYPE_STRING_COPY,
    G_FIELD_TYPE_BOOL,
    G_FIELD_TYPE_CUSTOM_CALLBACK,
    G_FIELD_TYPES_NUM
}ent_fields_types_e;

typedef struct ent_field_s
{
    const char * const name;

    int       index;
    const int offset;
    const int type;

    void (* const callback) (g_entity_t *ent);

    struct ent_field_s *next;
}g_entity_field_t;

#define ENTITY_FIELD(_f_name, _f_real, _f_type, _callback)       \
    {                                                            \
        .name = _f_name,                                         \
        .index = ENT_FIELD_INDEX_INVALID,                        \
        .offset = FIELD_OFFSET(STRUCTURE_FOR_OFFSETS, _f_real),  \
        .type = G_FIELD_TYPE_##_f_type,                          \
        .callback = _callback                                    \
    }

#define ENTITY_FIELD_NULL { .name = NULL }

const char *g_entity_field_type_string (int type) GNUC_CONST;
void g_entity_add_field (g_entity_field_t *field) GNUC_NONNULL;
void g_entity_add_field_list (g_entity_field_t *fields, int index) GNUC_NONNULL;

void g_entity_delete_field (g_entity_field_t *field) GNUC_NONNULL;
void g_entity_delete_field_list (g_entity_field_t *fields) GNUC_NONNULL;

void g_entity_draw_entities (void);
void g_entity_frame (void);

void g_entity_init (void);
void g_entity_shutdown (void);

#endif /* !_G_ENTITY_H */
