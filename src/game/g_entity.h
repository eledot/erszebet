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
    ENT_INTFL_THINK       = (1 << 0),
    ENT_INTFL_TOUCH       = (1 << 1),
    ENT_INTFL_BLOCK       = (1 << 2),
    ENT_INTFL_PHYS_STATIC = (1 << 3)
}g_entity_internal_flags_e;

typedef enum
{
    ENT_FL_STATIC    = (1 << 0),
    ENT_FL_NON_SOLID = (1 << 1)
}g_entity_flags_e;

typedef struct g_entity_s
{
    char *classname;

    int internal_flags;
    int flags;

    double nextthink;
    double lastthink;

    double origin[3];
    double velocity[3];
    double angle;
    double rotation;
    double gravity;
    double elasticity;
    double friction;
    double mass;
    double inertia;

    int phys_group;
    int phys_layers;

    struct g_entity_s *next;

    /* lua stuff */
    int ref;
    int dataref;

    /* physics stuff */
    void  *body;
    void **shapes;
    int    shapes_num;

    /* graphics stuff */
    bool  render_valid;
    int   render_index;
    void *render_data;
}g_entity_t;

typedef enum
{
    ENT_FIELD_TYPE_STRING = 0,
    ENT_FIELD_TYPE_DOUBLE,
    ENT_FIELD_TYPE_INTEGER,
    ENT_FIELD_TYPE_VECTOR,
    ENT_FIELD_TYPE_BOOL,
}ent_fields_types_e;

typedef struct ent_field_s
{
    const char * const name;
    int         render_index;
    const int   offset;
    const int   type;

    void (* const callback) (g_entity_t *ent);

    struct ent_field_s *next;
}ent_field_t;

void g_entity_draw_entities (void);
void g_entity_frame (void);

void g_entity_init (void);
void g_entity_shutdown (void);

#endif /* !_G_ENTITY_H */
