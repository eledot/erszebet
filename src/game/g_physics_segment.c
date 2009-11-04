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
physics_shape_create_segment
=================
*/
static cpShape *physics_shape_create_segment (cpBody *body)
{
    double radius, start[3], finish[3];

    lua_getfield(lua_state, -1, "radius");
    radius = lua_tonumber(lua_state, -1);
    lua_pop(lua_state, 1);

    lua_getfield(lua_state, -1, "start");
    g_pop_vector(-1, start, 3);
    lua_pop(lua_state, 1);

    lua_getfield(lua_state, -1, "finish");
    g_pop_vector(-1, finish, 3);
    lua_pop(lua_state, 1);

    return cpSegmentShapeNew(body, cpv(start[0], start[1]), cpv(finish[0], finish[1]), radius);
}

const g_physics_shape_type_t g_physics_shape_type_segment =
{
    .name = "segment",
    .create = &physics_shape_create_segment
};
