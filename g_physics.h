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

void g_physics_update_ent (g_entity_t *ent) GNUC_NONNULL;
void g_physics_update_body (g_entity_t *ent) GNUC_NONNULL;

void g_physics_draw_collisions (void) GNUC_CONST;
void g_physics_frame (void);

void g_physics_init (void);
void g_physics_shutdown (void);

#endif /* !_G_PHYSICS_H */
