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
physics_shape_create_polygon
=================
*/
static cpShape *physics_shape_create_polygon (cpBody *body)
{
    double offset[3], *data;
    int coords_num, i;
    cpVect *vertices;
    cpShape *shape;

    lua_getfield(lua_state, -1, "offset");
    g_pop_vector(-1, offset, 3);
    lua_pop(lua_state, 1);

    lua_getfield(lua_state, -1, "vertices");
    coords_num = lua_objlen(lua_state, -1);
    data = mem_alloc(g_mempool, sizeof(double) * coords_num);

    if (NULL == data)
        return NULL;

    vertices = mem_alloc(g_mempool, sizeof(cpVect) * (coords_num >> 1));

    if (NULL == vertices)
    {
        mem_free(data);
        return NULL;
    }

    g_pop_vector(-1, data, coords_num);

    for (i = 0; i < (coords_num >> 1) ;i++)
    {
        vertices[i].x = data[(i << 1) + 0];
        vertices[i].y = data[(i << 1) + 1];
    }

    shape = cpPolyShapeNew(body,
                           coords_num >> 1,
                           vertices,
                           cpv(offset[0], offset[1]));

    mem_free(vertices);
    mem_free(data);

    return shape;
}

const g_physics_shape_type_t g_physics_shape_type_polygon =
{
    .name = "polygon",
    .create = &physics_shape_create_polygon
};
