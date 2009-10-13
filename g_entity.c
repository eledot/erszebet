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

#include <limits.h>

#include "common.h"
#include "3rd/lua/lua.h"
#include "3rd/lua/lualib.h"
#include "3rd/lua/lauxlib.h"
#include "g_entity.h"
#include "g_physics.h"
#include "r_sprite.h"
#include "sglib.h"

#undef assert
#define assert(unused) (void)0

#define G_SPRITES_MASK 0x10000

#define ENT_VALID(ent) (ent->ref != LUA_REFNIL && ent->dataref != LUA_REFNIL)

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

#define FOFF(f) ((const void *)&((const g_entity_t *)NULL)->f - (const void *)NULL)

static void ent_set_frame_callback (g_entity_t *ent);
static void ent_set_origin_callback (g_entity_t *ent);
static void ent_set_group_layers_callback (g_entity_t *ent);

typedef struct ent_field_s
{
    const char *name;
    int         offset;
    int         type;
    void      (*callback) (g_entity_t *ent);
    struct ent_field_s *next;
}ent_field_t;

#define DOFF(str, type, callback)               \
    { #str, FOFF(str),   type, callback, NULL }
#define DOFF2(str, field, type, callback)       \
    { #str, FOFF(field), type, callback, NULL }

static ent_field_t ent_fields_known[] =
{
    DOFF(classname,   ENT_F_STRING,  NULL),
    DOFF(frame,       ENT_F_INTEGER, &ent_set_frame_callback),
    DOFF(flags,       ENT_F_INTEGER, NULL),
    DOFF(nextthink,   ENT_F_DOUBLE,  NULL),
    DOFF(origin,      ENT_F_VECTOR,  &ent_set_origin_callback),
    DOFF(velocity,    ENT_F_VECTOR,  NULL),
    DOFF(angle,       ENT_F_DOUBLE,  NULL),
    DOFF(rotation,    ENT_F_DOUBLE,  NULL),
    DOFF(gravity,     ENT_F_DOUBLE,  NULL),
    DOFF(elasticity,  ENT_F_DOUBLE,  NULL),
    DOFF(friction,    ENT_F_DOUBLE,  NULL),
    DOFF(mass,        ENT_F_DOUBLE,  NULL),
    DOFF(inertia,     ENT_F_DOUBLE,  NULL),
    DOFF(scale,       ENT_F_DOUBLE,  NULL),
    DOFF(width,       ENT_F_DOUBLE,  NULL),
    DOFF(height,      ENT_F_DOUBLE,  NULL),
    DOFF(phys_group,  ENT_F_INTEGER, &ent_set_group_layers_callback),
    DOFF(phys_layers, ENT_F_INTEGER, &ent_set_group_layers_callback),
    DOFF2(origin_x,   origin[0],   ENT_F_DOUBLE, NULL),
    DOFF2(origin_y,   origin[1],   ENT_F_DOUBLE, NULL),
    DOFF2(origin_z,   origin[2],   ENT_F_DOUBLE, &ent_set_origin_callback),
    DOFF2(velocity_x, velocity[0], ENT_F_DOUBLE, NULL),
    DOFF2(velocity_y, velocity[1], ENT_F_DOUBLE, NULL),
    DOFF2(velocity_z, velocity[2], ENT_F_DOUBLE, NULL)
};

#undef DOFF
#undef DOFF2

static ent_field_t *ent_fields;

#define ENT_FIELD_COMPARATOR(f1, f2) strcmp(f1->name, f2->name)

SGLIB_DEFINE_SORTED_LIST_PROTOTYPES(ent_field_t, ENT_FIELD_COMPARATOR, next);
SGLIB_DEFINE_SORTED_LIST_FUNCTIONS(ent_field_t, ENT_FIELD_COMPARATOR, next);

/* entity field offsets for strings that must be freed */
static const int ent_fields_free[] =
{
    FOFF(classname)
};

#undef FOFF

#define ENT_ZORDER_COMPARATOR(e1, e2) (e1->origin[2] == e2->origin[2] ? \
                                       -1 : \
                                       e1->origin[2] - e2->origin[2])

SGLIB_DEFINE_SORTED_LIST_PROTOTYPES(g_entity_t, ENT_ZORDER_COMPARATOR, next);
SGLIB_DEFINE_SORTED_LIST_FUNCTIONS(g_entity_t, ENT_ZORDER_COMPARATOR, next);

static int ent_render_load_sprite (const char *name, const double *parms, g_entity_t *ent);
static void ent_render_unload_sprite (g_entity_t *ent);
static int ent_render_load_model (GNUC_UNUSED const char *name, GNUC_UNUSED const double *parms, GNUC_UNUSED g_entity_t *ent);
static void ent_render_unload_model (GNUC_UNUSED g_entity_t *ent);

static const struct
{
    int (*load) (const char *name, const double *parms, g_entity_t *ent);
    void (*unload) (g_entity_t *ent);
}ent_render_load_unload_funcs[] =
{
    { &ent_render_load_sprite, &ent_render_unload_sprite },
    { &ent_render_load_model,  &ent_render_unload_model  }
};

static mem_pool_t mempool;

g_entity_t        *entities;
static g_entity_t *remove_entities;
static lua_State  *lst;
static int         point_query_shapes_num;

/*
=================
ent_render_load_sprite
=================
*/
static int ent_render_load_sprite (const char *name, const double *parms, g_entity_t *ent)
{
    r_sprite_t *sprite;
    int res;

    if (0 != (res = r_sprite_load(name,
                                  G_SPRITES_MASK,
                                  ((int)parms[0]) ? R_TEX_DEFAULT : R_TEX_SCREEN_UI,
                                  &sprite)))
    {
        return res;
    }

    ent->render_type = 0;
    ent->render_data = sprite;
    ent->frame = 0;
    ent->frames_num = sprite->frames_num;
    ent->width = parms[1];
    ent->height = parms[2];

    if (!ent->width)
        ent->width = sprite->frames[0]->w * sprite->inc;

    if (!ent->height)
        ent->height = sprite->frames[0]->h;

    return 0;
}

/*
=================
ent_render_unload_sprite
=================
*/
static void ent_render_unload_sprite (g_entity_t *ent)
{
    r_sprite_unload(ent->render_data);
    ent->render_data = NULL;
}

/*
=================
ent_render_load_model
=================
*/
static int ent_render_load_model (GNUC_UNUSED const char *name, GNUC_UNUSED const double *parms, GNUC_UNUSED g_entity_t *ent)
{
    /* FIXME */
    return 1;
}

/*
=================
ent_render_unload_model
=================
*/
static void ent_render_unload_model (GNUC_UNUSED g_entity_t *ent)
{
    /* FIXME */
}

/*
=================
ent_get_field
=================
*/
static int ent_get_field (const g_entity_t *ent, const char *field)
{
    const void  *data;
    ent_field_t *f, s;

    s.name = field;
    f = sglib_ent_field_t_find_member(ent_fields, &s);

    if (NULL != f)
    {
        data = (const void *)ent + f->offset;

        switch (f->type)
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
            g_push_vector((const double *)data, 3);
            break;

        default:
            return 0;
        }

        return 1;
    }

    return 0;
}

/*
=================
ent_set_frame_callback
=================
*/
static void ent_set_frame_callback (g_entity_t *ent)
{
    if (ent->frames_num)
        ent->frame %= ent->frames_num;
}

/*
=================
ent_set_origin_callback
=================
*/
static void ent_set_origin_callback (GNUC_UNUSED g_entity_t *ent)
{
    sglib_g_entity_t_sort(&entities);
}

/*
=================
ent_set_group_layers_callback
=================
*/
static void ent_set_group_layers_callback (g_entity_t *ent)
{
    g_physics_update_body(ent);
}

/*
=================
ent_set_field
=================
*/
static int ent_set_field (g_entity_t *ent, const char *field, int index)
{
    void        *data;
    ent_field_t *f, s;

    s.name = field;
    f = sglib_ent_field_t_find_member(ent_fields, &s);

    if (NULL != f)
    {
        data = (void *)ent + f->offset;

        switch (f->type)
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
            g_pop_vector(index, (double *)data, 3);
            break;

        default:
            return 0;
        }

        if (NULL != f->callback)
            f->callback(ent);

        return 1;
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

    if (ent->internal_flags & ENT_INTFL_THINK)
        strlcat(flags, "think ", s);

    if (ent->internal_flags & ENT_INTFL_TOUCH)
        strlcat(flags, "touch ", s);

    if (ent->internal_flags & ENT_INTFL_BLOCK)
        strlcat(flags, "block ", s);

    if (ent->internal_flags & ENT_INTFL_PHYS_STATIC)
        strlcat(flags, "phys_static ", s);

    if (ent->internal_flags & ENT_INTFL_DRAW)
        strlcat(flags, "draw ", s);

    if (unknown)
    {
        len = strlen(flags);
        snprintf(flags + len, s - len, "%i", unknown);
    }

    return flags;
}

/*
=================
ent_entity_string
=================
*/
static void ent_entity_string (const g_entity_t *ent, char *buffer, int size)
{
    snprintf(buffer,
             size,
             "entity: %p "
             "classname=\"%s\" "
             "flags=( %s) "
             "nextthink=%-2.2lf "
             "lastthink=%-2.2lf "
             "origin={ %-2.2lf %-2.2lf %-2.2lf } "
             "velocity={ %-2.2lf %-2.2lf %-2.2lf } "
             "angle=%-2.2lf "
             "rotation=%-2.2lf "
             "gravity=%-2.2lf "
             "elasticity=%-2.2lf "
             "friction=%-2.2lf "
             "mass=%-2.2lf "
             "inertia=%-2.2lf "
             "phys_group=%i "
             "phys_layers=%i "
             "scale=%-2.2lf "
             "frame=%i "
             "frames_num=%i",
             ent,
             ent->classname ? ent->classname : "-noname-",
             ent_flags_string(ent),
             ent->nextthink,
             ent->lastthink,
             ent->origin[0],
             ent->origin[1],
             ent->origin[2],
             ent->velocity[0],
             ent->velocity[1],
             ent->velocity[2],
             ent->angle,
             ent->rotation,
             ent->gravity,
             ent->elasticity,
             ent->friction,
             ent->mass,
             ent->inertia,
             ent->phys_group,
             ent->phys_layers,
             ent->scale,
             ent->frame,
             ent->frames_num);
}

/*
=================
ent_lua_tostring
=================
*/
static int ent_lua_tostring (lua_State *lst)
{
    char tmp[512];
    const g_entity_t *ent;

    lua_getfield(lst, 1, "__ref");
    ent = (g_entity_t *)lua_touserdata(lst, -1);
    ent_entity_string(ent, tmp, sizeof(tmp));
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
    }ent_newindex_internal_flags[] =
         {
             { "think", ENT_INTFL_THINK },
             { "touch", ENT_INTFL_TOUCH },
             { "block", ENT_INTFL_BLOCK },
             { "draw",  ENT_INTFL_DRAW  }
         };

    lua_getfield(lst, 1, "__ref");
    ent = (g_entity_t *)lua_touserdata(lst, -1);

    if (!ENT_VALID(ent))
    {
        sys_printf("invalid entity\n");
        return 0;
    }

    for (i = 0; i < STSIZE(ent_newindex_internal_flags) ;i++)
    {
        if (!strcmp(key, ent_newindex_internal_flags[i].field))
        {
            if (!lua_toboolean(lst, 3))
                ent->internal_flags -= (ent->internal_flags & ent_newindex_internal_flags[i].flag);
            else
                ent->internal_flags |= ent_newindex_internal_flags[i].flag;

            break;
        }
    }

    if (i >= STSIZE(ent_newindex_internal_flags) && ent_set_field(ent, key, 3))
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

    ent->ref     = LUA_REFNIL;
    ent->dataref = LUA_REFNIL;

    ent->nextthink = 0.0;
    ent->lastthink = 0.0;

    ent->origin[0]   = 0.0;
    ent->origin[1]   = 0.0;
    ent->origin[2]   = 0.0;
    ent->angle       = 0.0;
    ent->velocity[0] = 0.0;
    ent->velocity[1] = 0.0;
    ent->velocity[2] = 0.0;
    ent->rotation    = 0.0;
    ent->gravity     = 0.0;
    ent->elasticity  = 0.0;
    ent->friction    = 0.6;
    ent->mass        = 1.0;
    ent->inertia     = 100.0;
    ent->phys_group  = 0;
    ent->phys_layers = INT_MAX;
    ent->scale       = 1.0;
    ent->frame       = 0;
    ent->frames_num  = 0;
    ent->width       = 0.0;
    ent->height      = 0.0;
    ent->render_type = -1;
    ent->render_data = NULL;

    /* put to entities, taking into account zorder */
    sglib_g_entity_t_add(&entities, ent);

    return ent;
}

/*
=================
g_entity_delete
=================
*/
static void g_entity_delete (g_entity_t *ent)
{
    if (!ENT_VALID(ent))
    {
        sys_printf("invalid entity\n");
        return;
    }

    sglib_g_entity_t_delete(&entities, ent);
    ent->next = remove_entities;
    remove_entities = ent;

    lua_unref(lst, ent->ref);
    lua_unref(lst, ent->dataref);

    /* remove all handlers, mark invalid */
    ent->internal_flags -= ent->internal_flags & (ENT_INTFL_THINK | ENT_INTFL_TOUCH | ENT_INTFL_BLOCK | ENT_INTFL_DRAW);
    ent->flags |= ENT_FL_NON_SOLID;

    ent->ref = LUA_REFNIL;
    ent->dataref = LUA_REFNIL;
}

/*
=================
g_entity_mem_free
=================
*/
static void g_entity_mem_free (g_entity_t *ent)
{
    int i;

    for (i = 0; i < STSIZE(ent_fields_free) ;i++)
    {
        char *data = *(char **)((void *)ent + ent_fields_free[i]);

        if (NULL != data)
            mem_free(data);
    }

    if (NULL != ent->render_data)
        ent_render_load_unload_funcs[ent->render_type].unload(ent);

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

    if (!ENT_VALID(ent))
    {
        sys_printf("invalid entity\n");
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
    const char *name;
    double parms[3] = { 0.0, 0.0, 0.0 };

    lua_getfield(lst, 1, "__ref");
    ent = (g_entity_t *)lua_touserdata(lst, -1);

    if (NULL == ent)
    {
        sys_printf("called \"set_sprite\" without entity\n");
        return 0;
    }

    if (!ENT_VALID(ent))
    {
        sys_printf("invalid entity\n");
        return 0;
    }

    if (NULL != ent->render_data)
    {
        ent_render_load_unload_funcs[ent->render_type].unload(ent);
        ent->render_data = NULL;
        ent->render_type = -1;
    }

    name = luaL_checkstring(lst, 2);
    g_pop_vector(3, parms, 3);
    ent_render_load_unload_funcs[0].load(name, parms, ent);

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

    if (!ENT_VALID(ent))
    {
        sys_printf("invalid entity\n");
        return 0;
    }

    if (NULL != ent->render_data)
    {
        ent_render_load_unload_funcs[ent->render_type].unload(ent);
        ent->render_data = NULL;
        ent->render_type = -1;
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

    if (!ENT_VALID(a))
    {
        sys_printf("invalid entity\n");
        return 0;
    }

    lua_getfield(lst, 2, "__ref");
    b = (g_entity_t *)lua_touserdata(lst, -1);

    if (NULL == b)
    {
        sys_printf("called \"phys_attach_pin\" without entity\n");
        return 0;
    }

    if (!ENT_VALID(b))
    {
        sys_printf("invalid entity\n");
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

    if (!ENT_VALID(a))
    {
        sys_printf("invalid entity\n");
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
    double point[3];

    point_query_shapes_num = 0;

    if (0 != g_pop_vector(1, point, 3))
    {
        sys_printf("called phys_point_query without origin\n");
        return 0;
    }

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

    if (!ENT_VALID(ent))
    {
        sys_printf("invalid entity\n");
        return 0;
    }

    if (0 != g_pop_vector(2, point, 2))
    {
        sys_printf("called phys_apply_impulse without point\n");
        return 0;
    }

    if (0 != g_pop_vector(3, impulse, 2))
    {
        sys_printf("called phys_apply_impulse without impulse\n");
        return 0;
    }

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

    if (!ENT_VALID(ent))
    {
        sys_printf("invalid entity\n");
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

    g_push_vector(origin, 3);
    g_push_vector(normal, 3);

    g_lua_call(4, 1);
    lua_pop(lst, 1);
    ret = lua_toboolean(lst, 0);
    lua_pop(lst, 1);

    return ret;
}

/*
=================
g_entity_draw_entities
=================
*/
void g_entity_draw_entities (int draw2d)
{
    g_entity_t *ent;
    struct sglib_g_entity_t_iterator it;

    if (draw2d)
    {
        for (ent = sglib_g_entity_t_it_init(&it, entities);
             NULL != ent;
             ent = sglib_g_entity_t_it_next(&it))
        {
            if (NULL != ent->render_data)
            {
                switch (ent->render_type)
                {
                case 0:
                    /* 2d sprite */
                    r_sprite_draw(ent->render_data,
                                  ent->frame,
                                  ent->origin[0],
                                  ent->origin[1],
                                  ent->width,
                                  ent->height,
                                  ent->scale,
                                  ent->angle);
                    break;

                case 1:
                    /* 3d model */
                    /* FIXME */
                    break;

                default:
                    break;
                }
            }

            if (ent->internal_flags & ENT_INTFL_DRAW && LUA_REFNIL != ent->ref)
            {
                lua_getref(lst, ent->dataref);
                lua_getfield(lst, -1, "draw");
                lua_getref(lst, ent->ref);
                g_lua_call(1, 0);
                lua_pop(lst, 1);
            }
        }
    }
    else
    {
        /* FIXME */
    }
}

/*
=================
g_entity_frame
=================
*/
void g_entity_frame (void)
{
    g_entity_t *ent;
    struct sglib_g_entity_t_iterator it;

    while (NULL != remove_entities)
    {
        ent = remove_entities->next;
        g_physics_free_obj(remove_entities);
        g_entity_mem_free(remove_entities);
        remove_entities = ent;
    }

    remove_entities = NULL;

    for (ent = sglib_g_entity_t_it_init(&it, entities);
         NULL != ent;
         ent = sglib_g_entity_t_it_next(&it))
    {
        if (!(ent->internal_flags & ENT_INTFL_PHYS_STATIC))
        {
            g_physics_update_ent(ent);
        }

        if ((ent->internal_flags & ENT_INTFL_THINK) &&
            ent->nextthink > ent->lastthink &&
            ent->nextthink < g_time &&
            LUA_REFNIL != ent->ref)
        {
            ent->lastthink = ent->nextthink;
            lua_getref(lst, ent->dataref);
            lua_getfield(lst, -1, "think");
            lua_getref(lst, ent->ref);
            g_lua_call(1, 0);
            lua_pop(lst, 1);
        }
    }
}

/*
=================
g_list_entities_f
=================
*/
static void g_list_entities_f (GNUC_UNUSED const struct cmd_s *cmd,
                               int source,
                               GNUC_UNUSED int argc,
                               GNUC_UNUSED const char **argv)
{
    char tmp[512];
    struct sglib_g_entity_t_iterator it;
    const g_entity_t *ent;

    if (source == CMD_SRC_KEY_UP)
        return;

    sys_printf("----------- entities list -----------\n");

    for (ent = sglib_g_entity_t_it_init(&it, entities);
         NULL != ent;
         ent = sglib_g_entity_t_it_next(&it))
    {
        ent_entity_string(ent, tmp, sizeof(tmp));
        sys_printf("%s\n", tmp);
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
    int i;

    entities = NULL;
    remove_entities = NULL;
    ent_fields = NULL;
    lst = _lst;

    mempool = pool;

    for (i = 0; i < STSIZE(ent_fields_known) ;i++)
        sglib_ent_field_t_add(&ent_fields, &ent_fields_known[i]);

    luaL_newmetatable(lst, "entity");
    lua_pushvalue(lst, -1);
    lua_setfield(lst, -2, "__index");
    luaL_register(lst, NULL, entity_metamap);
    lua_pop(lst, 1);

    cmd_register("g_list_entities", NULL, &g_list_entities_f, 0);

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

    while (NULL != entities)
    {
        g_entity_delete(entities);
    }

    while (NULL != remove_entities)
    {
        ent = remove_entities->next;
        g_physics_free_obj(remove_entities);
        g_entity_mem_free(remove_entities);
        remove_entities = ent;
    }

    sys_printf("-g_entity\n");
}
