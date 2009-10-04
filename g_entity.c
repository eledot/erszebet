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

#include "common.h"
#include "3rd/lua/lua.h"
#include "3rd/lua/lualib.h"
#include "3rd/lua/lauxlib.h"
#include "g_entity.h"
#include "g_physics.h"

typedef enum
{
    PHYS_BODY_EMPTY = -1,
    PHYS_BODY_CIRCLE = 0,
    PHYS_BODY_SEGMENT,
    PHYS_BODY_POLYGON,
    PHYS_BODY_POLYGON_CENTERED
}phys_body_types_e;

typedef enum
{
    ENT_F_STRING = 0,
    ENT_F_INTEGER,
    ENT_F_DOUBLE,
    ENT_F_VECTOR
}ent_fields_types_e;

/* entity field offsets */
static const struct
{
    const char *field;
    int         offset;
    int         type;
}ent_entity_fields[] =
{
#define DOFF(str, type)                                                 \
    { #str, ((const void *)&((const g_entity_t *)NULL)->str - (const void *)NULL), type }
#define DOFF2(str, field, type)                                         \
    { #str, ((const void *)&((const g_entity_t *)NULL)->field - (const void *)NULL), type }
    DOFF(classname,  ENT_F_STRING),
    DOFF(flags,      ENT_F_INTEGER),
    DOFF(nextthink,  ENT_F_DOUBLE),
    DOFF(origin,     ENT_F_VECTOR),
    DOFF(velocity,   ENT_F_VECTOR),
    DOFF(angle,      ENT_F_DOUBLE),
    DOFF(rotation,   ENT_F_DOUBLE),
    DOFF(gravity,    ENT_F_DOUBLE),
    DOFF(elasticity, ENT_F_DOUBLE),
    DOFF(friction,   ENT_F_DOUBLE),
    DOFF(mass,       ENT_F_DOUBLE),
    DOFF(inertia,    ENT_F_DOUBLE),
    DOFF2(origin_x,   origin[0],   ENT_F_DOUBLE),
    DOFF2(origin_y,   origin[1],   ENT_F_DOUBLE),
    DOFF2(velocity_x, velocity[0], ENT_F_DOUBLE),
    DOFF2(velocity_y, velocity[1], ENT_F_DOUBLE)
#undef DOFF
#undef DOFF2
};

/* entity field offsets for strings that must be freed */
static const int ent_entity_fields_free[] =
{
#define DOFF(str)                                                       \
    ((const void *)&((const g_entity_t *)NULL)->str - (const void *)NULL)
    DOFF(classname)
#undef DOFF
};

static mem_pool_t mempool;

g_entity_t        *entities;
static g_entity_t *remove_entities;
static lua_State  *lst;
static int         point_query_shapes_num;

/*
=================
ent_get_field
=================
*/
static int ent_get_field (const g_entity_t *ent, const char *field)
{
    int i;

    for (i = 0; i < STSIZE(ent_entity_fields) ;i++)
    {
        const void *data;

        if (!strcmp(ent_entity_fields[i].field, field))
        {
            data = (const void *)ent + ent_entity_fields[i].offset;

            switch (ent_entity_fields[i].type)
            {
            case ENT_F_STRING:
                if (NULL == *(char **)data)
                    lua_pushstring(lst, "");
                else
                    lua_pushstring(lst, *(const char **)data);
                break;

            case ENT_F_INTEGER:
                lua_pushinteger(lst, *(const int *)data);
                break;

            case ENT_F_DOUBLE:
                lua_pushnumber(lst, *(const double *)data);
                break;

            case ENT_F_VECTOR:
                g_push_vector((const double *)data, 2);
                break;

            default:
                return 0;
            }

            return 1;
        }
    }

    return 0;
}

/*
=================
ent_set_field
=================
*/
static int ent_set_field (g_entity_t *ent, const char *field, int index)
{
    int i;

    for (i = 0; i < STSIZE(ent_entity_fields) ;i++)
    {
        void *data;

        if (!strcmp(ent_entity_fields[i].field, field))
        {
            data = (void *)ent + ent_entity_fields[i].offset;

            switch (ent_entity_fields[i].type)
            {
            case ENT_F_STRING:
                if (NULL != *(char **)data)
                    mem_free(*(char **)data);

                *(char **)data = mem_strdup_static(luaL_checkstring(lst, index));
                break;

            case ENT_F_INTEGER:
                *(int *)data = lua_tointeger(lst, index);
                break;

            case ENT_F_DOUBLE:
                *(double *)data = lua_tonumber(lst, index);
                break;

            case ENT_F_VECTOR:
                g_pop_vector(index, (double *)data, 2);
                break;

            default:
                return 0;
            }

            return 1;
        }
    }

    return 0;
}

/*
=================
ent_flags_to_string
=================
*/
static const char *ent_flags_string (const g_entity_t *ent)
{
    static char flags[128];
    const int s = sizeof(flags);
    int unknown = ent->flags - (ent->flags & (ENT_FL_STATIC | ENT_FL_NON_SOLID));
    int len;

    flags[0] = 0;

    if (ent->flags & ENT_FL_STATIC)
        strlcat(flags, "static ", s);

    if (ent->flags & ENT_FL_NON_SOLID)
        strlcat(flags, "non_solid ", s);

    if (ent->cflags & ENT_CFL_THINK)
        strlcat(flags, "think ", s);

    if (ent->cflags & ENT_CFL_TOUCH)
        strlcat(flags, "touch ", s);

    if (ent->cflags & ENT_CFL_BLOCK)
        strlcat(flags, "block ", s);

    if (ent->cflags & ENT_CFL_PHYS_STATIC)
        strlcat(flags, "phys_static ", s);

    if (unknown)
    {
        len = strlen(flags);
        snprintf(flags + len, s - len, "%i", unknown);
    }

    return flags;
}

/*
=================
ent_lua_tostring
=================
*/
static int ent_lua_tostring (lua_State *lst)
{
    char        tmp[512];
    g_entity_t *ent;

    lua_getfield(lst, 1, "__ref");
    ent = (g_entity_t *)lua_touserdata(lst, -1);

    snprintf(tmp,
             sizeof(tmp),
             "entity: %p "
             "classname=\"%s\" "
             "flags=( %s) "
             "nextthink=%-2.2lf "
             "lastthink=%-2.2lf "
             "origin={ %-2.2lf %-2.2lf } "
             "velocity={ %-2.2lf %-2.2lf } "
             "angle=%-2.2lf "
             "rotation=%-2.2lf "
             "gravity=%-2.2lf "
             "elasticity=%-2.2lf "
             "friction=%-2.2lf "
             "mass=%-2.2lf "
             "inertia=%-2.2lf",
             ent,
             ent->classname ? ent->classname : "-noname-",
             ent_flags_string(ent),
             ent->nextthink,
             ent->lastthink,
             ent->origin[0],
             ent->origin[1],
             ent->velocity[0],
             ent->velocity[1],
             ent->angle,
             ent->rotation,
             ent->gravity,
             ent->elasticity,
             ent->friction,
             ent->mass,
             ent->inertia);

    lua_pushstring(lst, tmp);

    return 1;
}

/*
=================
ent_lua_index
=================
*/
static int ent_lua_index (lua_State *lst)
{
    const char *key = luaL_checkstring(lst, 2);
    g_entity_t *ent;

    lua_getfield(lst, 1, "__ref");
    ent = (g_entity_t *)lua_touserdata(lst, -1);

    if (!ent_get_field(ent, key))
    {
        lua_getfield(lst, 1, "__data");
        lua_pushstring(lst, key);
        lua_gettable(lst, -2);
    }

    return 1;
}

/*
=================
ent_lua_newindex
=================
*/
static int ent_lua_newindex (lua_State *lst)
{
    int         i;
    g_entity_t *ent;
    const char *key = luaL_checkstring(lst, 2);
    const struct
    {
        const char *field;
        int         flag;
    }ent_newindex_cflags[] =
         {
             { "think", ENT_CFL_THINK },
             { "touch", ENT_CFL_TOUCH },
             { "block", ENT_CFL_BLOCK }
         };

    lua_getfield(lst, 1, "__ref");
    ent = (g_entity_t *)lua_touserdata(lst, -1);

    for (i = 0; i < STSIZE(ent_newindex_cflags) ;i++)
    {
        if (!strcmp(key, ent_newindex_cflags[i].field))
        {
            if (!lua_toboolean(lst, 3))
                ent->cflags -= (ent->cflags & ent_newindex_cflags[i].flag);
            else
                ent->cflags |= ent_newindex_cflags[i].flag;

            break;
        }
    }

    if (i >= STSIZE(ent_newindex_cflags) && ent_set_field(ent, key, 3))
    {
        g_physics_update_body(ent);
    }
    else
    {
        lua_getref(lst, ent->dataref);
        lua_pushvalue(lst, 3);
        lua_setfield(lst, -2, key);
    }

    return 0;
}

/*
=================
g_entity_create
=================
*/
static g_entity_t *g_entity_create (void)
{
    g_entity_t *ent;

    if (NULL == (ent = mem_alloc_static(sizeof(*ent))))
    {
        sys_printf("failed to allocate memory for entity\n");
        return NULL;
    }

    memset(ent, 0, sizeof(*ent));

    ent->ref     = LUA_REFNIL;
    ent->dataref = LUA_REFNIL;

    ent->origin[0]   = 0.0;
    ent->origin[1]   = 0.0;
    ent->angle       = 0.0;
    ent->velocity[0] = 0.0;
    ent->velocity[1] = 0.0;
    ent->rotation    = 0.0;
    ent->gravity     = 0.0;
    ent->elasticity  = 0.0;
    ent->friction    = 0.6;
    ent->mass        = 1.0;
    ent->inertia     = 100.0;

    if (NULL != entities)
        entities->prev = ent;

    ent->next = entities;
    entities = ent;

    return ent;
}

/*
=================
g_entity_delete
=================
*/
static void g_entity_delete (g_entity_t *ent)
{
    if (NULL != ent->prev)
        ent->prev->next = ent->next;

    if (NULL != ent->next)
        ent->next->prev = ent->prev;

    if (entities == ent)
        entities = ent->next;

    ent->next = remove_entities;
    remove_entities = ent;

    /* remove all handlers, mark invalid */
    ent->flags -= ent->flags & (ENT_CFL_THINK | ENT_CFL_TOUCH | ENT_CFL_BLOCK);

    lua_unref(lst, ent->ref);
    lua_unref(lst, ent->dataref);
}

/*
=================
g_entity_mem_free
=================
*/
static void g_entity_mem_free (g_entity_t *ent)
{
    int i;

    for (i = 0; i < STSIZE(ent_entity_fields_free) ;i++)
    {
        char *data = *(char **)((void *)ent + ent_entity_fields[i].offset);

        if (NULL != data)
            mem_free(data);
    }

    mem_free(ent);
}

/*
=================
ent_lua_spawn
=================
*/
static int ent_lua_spawn (lua_State *lst)
{
    g_entity_t *ent = g_entity_create();

    lua_pushvalue(lst, -1);
    ent->ref = lua_ref(lst, 1);
    lua_pushlightuserdata(lst, ent);
    lua_setfield(lst, -2, "__ref");

    lua_newtable(lst);
    lua_pushvalue(lst, -1);
    ent->dataref = lua_ref(lst, -1);
    lua_setfield(lst, -2, "__data");

    luaL_getmetatable(lst, "entity");
    lua_setmetatable(lst, -2);

    return 0;
}

/*
=================
ent_lua_remove
=================
*/
static int ent_lua_remove (lua_State *lst)
{
    g_entity_t *ent;

    lua_getfield(lst, -1, "__ref");
    ent = (g_entity_t *)lua_touserdata(lst, -1);

    if (NULL == ent)
    {
        sys_printf("called \"ent_remove\" without entity\n");
        return 0;
    }

    g_entity_delete(ent);

    return 0;
}

/*
=================
ent_lua_set_sprite
=================
*/
static int ent_lua_set_sprite (lua_State *lst)
{
    g_entity_t *ent;

    lua_getfield(lst, -1, "__ref");
    ent = (g_entity_t *)lua_touserdata(lst, -1);

    if (NULL == ent)
    {
        sys_printf("called \"set_sprite\" without entity\n");
        return 0;
    }

    /* FIXME */

    return 0;
}

/*
=================
ent_lua_set_model
=================
*/
static int ent_lua_set_model (lua_State *lst)
{
    g_entity_t *ent;

    lua_getfield(lst, -1, "__ref");
    ent = (g_entity_t *)lua_touserdata(lst, -1);

    if (NULL == ent)
    {
        sys_printf("called \"set_model\" without entity\n");
        return 0;
    }

    /* FIXME */

    return 0;
}

/*
=================
ent_lua_attach_pin
=================
*/
static int ent_lua_attach_pin (lua_State *lst)
{
    g_entity_t *a, *b;

    lua_getfield(lst, 1, "__ref");
    a = (g_entity_t *)lua_touserdata(lst, -1);

    if (NULL == a)
    {
        sys_printf("called \"phys_attach_pin\" without entity\n");
        return 0;
    }

    lua_getfield(lst, 2, "__ref");
    b = (g_entity_t *)lua_touserdata(lst, -1);

    if (NULL == b)
    {
        sys_printf("called \"phys_attach_pin\" without entity\n");
        return 0;
    }

    g_physics_attach_pin(a, b);

    return 0;
}

/*
=================
ent_lua_detach
=================
*/
static int ent_lua_detach (lua_State *lst)
{
    int         top = lua_gettop(lst);
    g_entity_t *a, *b = NULL;

    lua_getfield(lst, 1, "__ref");
    a = (g_entity_t *)lua_touserdata(lst, -1);

    if (NULL == a)
    {
        sys_printf("called \"phys_detach\" without entity\n");
        return 0;
    }

    if (top > 1)
    {
        lua_getfield(lst, 2, "__ref");
        b = (g_entity_t *)lua_touserdata(lst, -1);
    }

    g_physics_detach(a, b);

    return 0;
}

/*
=================
ent_lua_point_query_callback
=================
*/
static void ent_lua_point_query_callback (g_entity_t *ent)
{
    point_query_shapes_num++;
    lua_pushinteger(lst, point_query_shapes_num);

    if (NULL == ent)
        lua_pushnil(lst);
    else
        lua_getref(lst, ent->ref);

    lua_settable(lst, -3);
}

/*
=================
ent_lua_point_query
=================
*/
static int ent_lua_point_query (lua_State *lst)
{
    double point[2];

    point_query_shapes_num = 0;

    g_pop_vector(1, point, 2);
    lua_newtable(lst);
    g_physics_point_query(point, &ent_lua_point_query_callback);

    return 1;
}

/*
=================
ent_lua_apply_impulse
=================
*/
static int ent_lua_apply_impulse (lua_State *lst)
{
    double point[2], impulse[2];
    g_entity_t *ent;

    lua_getfield(lst, 1, "__ref");
    ent = (g_entity_t *)lua_touserdata(lst, -1);

    if (NULL == ent)
    {
        sys_printf("called \"phys_apply_impulse\" without entity\n");
        return 0;
    }

    g_pop_vector(2, point, 2);
    g_pop_vector(3, impulse, 2);

    g_physics_apply_impulse(ent, point, impulse);

    return 0;
}

/*
=================
ent_lua_set_body
=================
*/
static int ent_lua_set_body (lua_State *lst)
{
    g_entity_t *ent;
    double      radius, points[4], *coords;
    int         arg, i, type, num, shapes_num, *vertices_num;

    arg = 1;
    lua_getfield(lst, arg++, "__ref");
    ent = (g_entity_t *)lua_touserdata(lst, -1);

    if (NULL == ent)
    {
        sys_printf("called \"set_physics\" without entity\n");
        return 0;
    }

    type = luaL_checknumber(lst, arg++);

    switch (type)
    {
    case PHYS_BODY_EMPTY:
        g_physics_free_obj(ent);
        break;

    case PHYS_BODY_CIRCLE:
        radius = luaL_checknumber(lst, arg++);
        g_physics_set_circle(ent, radius);
        break;

    case PHYS_BODY_SEGMENT:
        radius = luaL_checknumber(lst, arg++);
        g_pop_vector(arg, points, 4);
        g_physics_set_segment(ent, radius, points);
        break;

    case PHYS_BODY_POLYGON:
    case PHYS_BODY_POLYGON_CENTERED:
        shapes_num = lua_gettop(lst) - arg;

        vertices_num = mem_alloc_static(sizeof(*vertices_num) * shapes_num);

        for (num = i = 0; i < shapes_num ;i++)
        {
            vertices_num[i] = lua_objlen(lst, arg + i);
            num += vertices_num[i];

            if (vertices_num[i]/2 < 3)
            {
                sys_printf("bad polygon (got %i vertices)\n", vertices_num[i]/2);
                mem_free(vertices_num);
                break;
            }
        }

        coords = mem_alloc_static(sizeof(double) * num);

        for (num = i = 0; i < shapes_num ;i++)
        {
            g_pop_vector(arg + i, &coords[num], vertices_num[i]);
            num += vertices_num[i];
            vertices_num[i] >>= 1;
        }

        g_physics_set_poly(ent, shapes_num, coords, vertices_num, type == PHYS_BODY_POLYGON_CENTERED);
        mem_free(vertices_num);
        mem_free(coords);
        break;

    default:
        sys_printf("unknown body shape %i\n", type);
        break;
    }

    return 0;
}

/*
=================
g_entity_touch
=================
*/
int g_entity_touch (g_entity_t *self, g_entity_t *other, const double *origin, const double *normal)
{
    int ret;

    lua_getref(lst, self->dataref);
    lua_getfield(lst, -1, "touch");
    lua_getref(lst, self->ref);

    if (NULL == other)
        lua_pushnil(lst);
    else
        lua_getref(lst, other->ref);

    g_push_vector(origin, 2);
    g_push_vector(normal, 2);

    lua_pcall(lst, 4, 1, 0);
    ret = lua_toboolean(lst, -1);
    lua_pop(lst, 2);

    return ret;
}

/*
=================
g_entity_frame
=================
*/
void g_entity_frame (void)
{
    g_entity_t *ent;

    while (NULL != remove_entities)
    {
        ent = remove_entities->next;
        g_physics_free_obj(remove_entities);
        g_entity_mem_free(remove_entities);
        remove_entities = ent;
    }

    remove_entities = NULL;

    for (ent = entities; NULL != ent ;ent = ent->next)
    {
        if (!(ent->cflags & ENT_CFL_PHYS_STATIC))
        {
            g_physics_update_ent(ent);
        }

        if (ent->cflags & ENT_CFL_THINK &&
            ent->nextthink > ent->lastthink && ent->nextthink < g_time &&
            LUA_REFNIL != ent->ref)
        {
            ent->lastthink = ent->nextthink;
            lua_getref(lst, ent->dataref);
            lua_getfield(lst, -1, "think");
            lua_getref(lst, ent->ref);
            lua_pcall(lst, 1, 0, 0);
            lua_pop(lst, 1);
        }
    }
}

/*
=================
g_entity_init
=================
*/
void g_entity_init (void *_lst, mem_pool_t pool)
{
    const luaL_reg entity_metamap[] =
        {
            { "__index",    &ent_lua_index    },
            { "__newindex", &ent_lua_newindex },
            { "__tostring", &ent_lua_tostring },
            { NULL, NULL }
        };

    entities = NULL;
    remove_entities = NULL;
    lst = _lst;

    mempool = pool;

    luaL_newmetatable(lst, "entity");
    lua_pushvalue(lst, -1);
    lua_setfield(lst, -2, "__index");
    luaL_register(lst, NULL, entity_metamap);
    lua_pop(lst, 1);

    /* register funcs to work with entity */
    lua_register(lst, "ent_spawn", &ent_lua_spawn);
    lua_register(lst, "ent_remove", &ent_lua_remove);
    lua_register(lst, "ent_set_sprite", &ent_lua_set_sprite);
    lua_register(lst, "ent_set_model", &ent_lua_set_model);
    lua_register(lst, "phys_set_body", &ent_lua_set_body);
    lua_register(lst, "phys_attach_pin", &ent_lua_attach_pin);
    lua_register(lst, "phys_detach", &ent_lua_detach);
    lua_register(lst, "phys_point_query", &ent_lua_point_query);
    lua_register(lst, "phys_apply_impulse", &ent_lua_apply_impulse);

    /* register global constants */
    g_set_integer("BODY_EMPTY", PHYS_BODY_EMPTY);
    g_set_integer("BODY_CIRCLE", PHYS_BODY_CIRCLE);
    g_set_integer("BODY_SEGMENT", PHYS_BODY_SEGMENT);
    g_set_integer("BODY_POLYGON", PHYS_BODY_POLYGON);
    g_set_integer("BODY_POLYGON_CENTERED", PHYS_BODY_POLYGON_CENTERED);
    g_set_integer("FL_STATIC", ENT_FL_STATIC);
    g_set_integer("FL_NON_SOLID", ENT_FL_NON_SOLID);

    sys_printf("+g_entity\n");
}

/*
=================
g_entity_shutdown
=================
*/
void g_entity_shutdown (void)
{
    g_entity_t *ent;

    for (; NULL != entities ;)
        g_entity_delete(entities);

    while (NULL != remove_entities)
    {
        ent = remove_entities->next;
        g_physics_free_obj(remove_entities);
        mem_free(remove_entities);
        remove_entities = ent;
    }

    sys_printf("-g_entity\n");
}
