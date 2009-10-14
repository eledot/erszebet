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

#ifndef _G_PHYSICS_H
#define _G_PHYSICS_H

void g_physics_free_obj (g_entity_t *ent) GNUC_NONNULL;

void g_physics_set_segment (g_entity_t *ent, double radius, const double *coords) GNUC_NONNULL;
void g_physics_set_circle (g_entity_t *ent, double radius) GNUC_NONNULL;
void g_physics_set_poly (g_entity_t   *ent,
                         int           shapes_num,
                         const double *vertices,
                         const int    *vertices_num,
                         int           centered) GNUC_NONNULL;

void g_physics_attach_pin (g_entity_t *a, g_entity_t *b) GNUC_NONNULL;
void g_physics_detach (g_entity_t *a, g_entity_t *b) GNUC_NONNULL_ARGS(1);

void g_physics_point_query (const double *point, void (*callback) (g_entity_t *shape)) GNUC_NONNULL;
void g_physics_apply_impulse (g_entity_t *ent, const double *point, const double *impulse) GNUC_NONNULL;

void g_physics_update_ent (g_entity_t *ent) GNUC_NONNULL;
void g_physics_update_body (g_entity_t *ent) GNUC_NONNULL;

void g_physics_draw_collisions (void);
void g_physics_frame (void);

void g_physics_set_gravity (double gravity);
double g_physics_get_gravity (void);
void g_physics_set_speed (double speed);
double g_physics_get_speed (void);

void g_physics_init (mem_pool_t pool) GNUC_NONNULL;
void g_physics_shutdown (void);

#endif /* !_G_PHYSICS_H */
