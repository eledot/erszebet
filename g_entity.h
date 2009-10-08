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
    ENT_CFL_THINK       = (1 << 0),
    ENT_CFL_TOUCH       = (1 << 1),
    ENT_CFL_BLOCK       = (1 << 2),
    ENT_CFL_PHYS_STATIC = (1 << 3),
    ENT_CFL_DRAW        = (1 << 4)
}g_entity_cflags_e;

typedef enum
{
    ENT_FL_STATIC    = (1 << 0),
    ENT_FL_NON_SOLID = (1 << 1)
}g_entity_flags_e;

typedef struct g_entity_s
{
    char *classname;

    int cflags;
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

    /* model_t *model; */

    struct g_entity_s *prev;
    struct g_entity_s *next;

    /* lua stuff */
    int ref;
    int dataref;

    /* physics stuff */
    void  *body;
    void **shapes;
    int    shapes_num;

    /* graphics stuff */
    double scale;
    int    frame;
    int    frames_num;
    double width;
    double height;
    int    render_type;
    void  *render_data;
}g_entity_t;

extern g_entity_t *entities;

int g_entity_touch (g_entity_t *self, g_entity_t *other, const double *origin, const double *normal);

void g_entity_draw_entities (int draw2d);
void g_entity_frame (void);

void g_entity_init (void *lst, mem_pool_t pool);
void g_entity_shutdown (void);

#endif /* !_G_ENTITY_H */
