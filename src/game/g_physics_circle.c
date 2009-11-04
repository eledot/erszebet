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

#include "game/g_physics_private.h"

/*
=================
physics_shape_create_circle
=================
*/
static cpShape *physics_shape_create_circle (cpBody *body)
{
    double radius, offset[3];

    lua_getfield(lua_state, -1, "radius");
    radius = lua_tonumber(lua_state, -1);
    lua_pop(lua_state, 1);

    lua_getfield(lua_state, -1, "offset");
    g_pop_vector(-1, offset, 3);
    lua_pop(lua_state, 1);

    return cpCircleShapeNew(body, radius, cpv(offset[0], offset[1]));
}

const g_physics_shape_type_t g_physics_shape_type_circle =
{
    .name = "circle",
    .create = &physics_shape_create_circle
};
