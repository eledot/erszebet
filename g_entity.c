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

#include "g_private.h"
#include "g_entity_render.h"
#include "sglib.h"

typedef enum
{
    ENT_F_STRING = 0,
    ENT_F_INTEGER,
    ENT_F_DOUBLE,
    ENT_F_VECTOR
}ent_fields_types_e;

#define FOFF(f) ((const void *)&((const g_entity_t *)NULL)->f - (const void *)NULL)

GNUC_NONNULL static void ent_set_frame_callback (g_entity_t *ent);
GNUC_NONNULL static void ent_set_origin_callback (g_entity_t *ent);
GNUC_NONNULL static void ent_set_group_layers_callback (g_entity_t *ent);

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
    DOFF(color,       ENT_F_VECTOR,  NULL),
    DOFF(alpha,       ENT_F_DOUBLE,  NULL),
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
    DOFF2(velocity_z, velocity[2], ENT_F_DOUBLE, NULL),
    DOFF2(color_r,    color[0],    ENT_F_DOUBLE, NULL),
    DOFF2(color_g,    color[1],    ENT_F_DOUBLE, NULL),
    DOFF2(color_b,    color[2],    ENT_F_DOUBLE, NULL)
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

g_entity_t        *entities;
static g_entity_t *remove_entities;

/*
=================
ent_get_field
=================
*/
GNUC_NONNULL static int ent_get_field (const g_entity_t *ent, const char *field)
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
                lua_pushstring(lua_state, "");
            else
                lua_pushstring(lua_state, *(const char **)data);
            break;

        case ENT_F_INTEGER:
            lua_pushinteger(lua_state, *(const int *)data);
            break;

        case ENT_F_DOUBLE:
            lua_pushnumber(lua_state, *(const double *)data);
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
GNUC_NONNULL static void ent_set_frame_callback (g_entity_t *ent)
{
    if (ent->frames_num)
        ent->frame %= ent->frames_num;
}

/*
=================
ent_set_origin_callback
=================
*/
GNUC_NONNULL static void ent_set_origin_callback (GNUC_UNUSED g_entity_t *ent)
{
    sglib_g_entity_t_sort(&entities);
}

/*
=================
ent_set_group_layers_callback
=================
*/
GNUC_NONNULL static void ent_set_group_layers_callback (g_entity_t *ent)
{
    g_physics_update_body(ent);
}

/*
=================
ent_set_field
=================
*/
GNUC_NONNULL static int ent_set_field (g_entity_t *ent, const char *field, int index)
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

            *(char **)data = mem_strdup_static(luaL_checkstring(lua_state, index));
            break;

        case ENT_F_INTEGER:
            *(int *)data = lua_tointeger(lua_state, index);
            break;

        case ENT_F_DOUBLE:
            *(double *)data = lua_tonumber(lua_state, index);
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
GNUC_NONNULL static const char *ent_flags_string (const g_entity_t *ent)
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
GNUC_NONNULL static void ent_entity_string (const g_entity_t *ent, char *buffer, int size)
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
GNUC_NONNULL static int ent_lua_tostring (lua_State *lst)
{
    char tmp[512];
    const g_entity_t *ent;

    lua_getfield(lst, 1, "__ref");
    ent = (g_entity_t *)lua_touserdata(lst, -1);

    if (NULL == ent)
    {
        sys_printf("invalid entity\n");
        lua_pushstring(lst, "entity: NULL");
        return 1;
    }

    ent_entity_string(ent, tmp, sizeof(tmp));
    lua_pushstring(lst, tmp);

    return 1;
}

/*
=================
ent_lua_index
=================
*/
GNUC_NONNULL static int ent_lua_index (lua_State *lst)
{
    const char *key = luaL_checkstring(lst, 2);
    g_entity_t *ent;

    lua_getfield(lst, 1, "__ref");
    ent = (g_entity_t *)lua_touserdata(lst, -1);

    if (NULL == ent)
    {
        sys_printf("invalid entity\n");
        lua_pushnil(lst);
        return 1;
    }

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
GNUC_NONNULL static int ent_lua_newindex (lua_State *lst)
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
             { "block", ENT_INTFL_BLOCK }
         };

    lua_getfield(lst, 1, "__ref");
    ent = (g_entity_t *)lua_touserdata(lst, -1);

    if (NULL == ent)
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
GNUC_WARN_UNUSED_RES static g_entity_t *g_entity_create (void)
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

    ent->color[0]    = 1.0;
    ent->color[1]    = 1.0;
    ent->color[2]    = 1.0;
    ent->alpha       = 1.0;
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
GNUC_NONNULL static void g_entity_delete (g_entity_t *ent)
{
    sglib_g_entity_t_delete(&entities, ent);
    ent->next = remove_entities;
    remove_entities = ent;

    /* mark entity as removed (set pointer to NULL) */
    lua_getref(lua_state, ent->ref);
    lua_pushlightuserdata(lua_state, NULL);
    lua_setfield(lua_state, -2, "__ref");
    lua_pop(lua_state, 1);

    lua_unref(lua_state, ent->ref);
    lua_unref(lua_state, ent->dataref);

    /* remove all handlers, mark invalid */
    ent->internal_flags -= ent->internal_flags & (ENT_INTFL_THINK | ENT_INTFL_TOUCH | ENT_INTFL_BLOCK);
    ent->flags |= ENT_FL_NON_SOLID;

    ent->ref = LUA_REFNIL;
    ent->dataref = LUA_REFNIL;
}

/*
=================
g_entity_mem_free
=================
*/
GNUC_NONNULL static void g_entity_mem_free (g_entity_t *ent)
{
    int i;

    for (i = 0; i < STSIZE(ent_fields_free) ;i++)
    {
        char *data = *(char **)((void *)ent + ent_fields_free[i]);

        if (NULL != data)
            mem_free(data);
    }

    if (NULL != ent->render_data)
        ent_render_funcs[ent->render_type].unload(ent);

    mem_free(ent);
}

/*
=================
ent_lua_spawn
=================
*/
GNUC_NONNULL static int ent_lua_spawn (lua_State *lst)
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
GNUC_NONNULL static int ent_lua_remove (lua_State *lst)
{
    g_entity_t *ent;

    lua_getfield(lst, -1, "__ref");
    ent = (g_entity_t *)lua_touserdata(lst, -1);

    /* don't remove entities twice */
    if (NULL == ent)
    {
        /* FIXME -- warn about double removal? */
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
GNUC_NONNULL static int ent_lua_set_sprite (lua_State *lst)
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

    if (NULL != ent->render_data && RENDER_TYPE_SPRITE != ent->render_type)
    {
        ent_render_funcs[ent->render_type].unload(ent);
        ent->render_data = NULL;
        ent->render_type = -1;
    }

    name = luaL_checkstring(lst, 2);
    g_pop_vector(3, parms, 3);
    ent_render_funcs[RENDER_TYPE_SPRITE].load(name, parms, ent);

    return 0;
}

/*
=================
ent_lua_set_text
=================
*/
GNUC_NONNULL static int ent_lua_set_text (lua_State *lst)
{
    g_entity_t *ent;
    const char *text;
    double parms[3] = { 0.0, 0.0, 0.0 };

    lua_getfield(lst, -1, "__ref");
    ent = (g_entity_t *)lua_touserdata(lst, -1);

    if (NULL == ent)
    {
        sys_printf("called \"set_text\" without entity\n");
        return 0;
    }

    if (NULL != ent->render_data && RENDER_TYPE_TEXT != ent->render_type)
    {
        ent_render_funcs[ent->render_type].unload(ent);
        ent->render_data = NULL;
        ent->render_type = -1;
    }

    text = luaL_checkstring(lst, 2);
    g_pop_vector(3, parms, 3);
    ent_render_funcs[RENDER_TYPE_TEXT].load(text, parms, ent);

    return 0;
}

/*
=================
ent_lua_set_model
=================
*/
GNUC_NONNULL static int ent_lua_set_model (lua_State *lst)
{
    g_entity_t *ent;

    lua_getfield(lst, -1, "__ref");
    ent = (g_entity_t *)lua_touserdata(lst, -1);

    if (NULL == ent)
    {
        sys_printf("called \"set_model\" without entity\n");
        return 0;
    }

    if (NULL != ent->render_data && RENDER_TYPE_MODEL != ent->render_type)
    {
        ent_render_funcs[ent->render_type].unload(ent);
        ent->render_data = NULL;
        ent->render_type = -1;
    }

    /* FIXME */

    return 0;
}

/*
=================
ent_lua_set_circle
=================
*/
GNUC_NONNULL static int ent_lua_set_circle (lua_State *lst)
{
    g_entity_t *ent;
    const char *name;
    double parms[3] = { 0.0, 0.0, 0.0 };

    lua_getfield(lst, 1, "__ref");
    ent = (g_entity_t *)lua_touserdata(lst, -1);

    if (NULL == ent)
    {
        sys_printf("called \"set_circle\" without entity\n");
        return 0;
    }

    if (NULL != ent->render_data && RENDER_TYPE_CIRCLE != ent->render_type)
    {
        ent_render_funcs[ent->render_type].unload(ent);
        ent->render_data = NULL;
        ent->render_type = -1;
    }

    name = luaL_checkstring(lst, 2);
    g_pop_vector(3, parms, 3);
    ent_render_funcs[RENDER_TYPE_CIRCLE].load(name, parms, ent);

    return 0;
}

/*
=================
ent_lua_set_line
=================
*/
GNUC_NONNULL static int ent_lua_set_line (lua_State *lst)
{
    g_entity_t *ent;
    const char *name;
    double parms[4] = { 0.0, 0.0, 0.0, 0.0 };

    lua_getfield(lst, 1, "__ref");
    ent = (g_entity_t *)lua_touserdata(lst, -1);

    if (NULL == ent)
    {
        sys_printf("called \"set_circle\" without entity\n");
        return 0;
    }

    if (NULL != ent->render_data && RENDER_TYPE_LINE != ent->render_type)
    {
        ent_render_funcs[ent->render_type].unload(ent);
        ent->render_data = NULL;
        ent->render_type = -1;
    }

    name = luaL_checkstring(lst, 2);
    g_pop_vector(3, parms, 4);
    ent_render_funcs[RENDER_TYPE_LINE].load(name, parms, ent);

    return 0;
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
            if (NULL != ent->render_data &&
                ent->render_type >= 0 &&
                ent->render_type < STSIZE(ent_render_funcs))
            {
                ent_render_funcs[ent->render_type].draw(ent);
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
            lua_getref(lua_state, ent->dataref);
            lua_getfield(lua_state, -1, "think");
            lua_getref(lua_state, ent->ref);
            g_lua_call(1, 0);
            lua_pop(lua_state, 1);
        }
    }
}

/*
=================
g_list_entities_f
=================
*/
GNUC_NONNULL static void g_list_entities_f (GNUC_UNUSED const struct cmd_s *cmd,
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
void g_entity_init (void)
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

    for (i = 0; i < STSIZE(ent_fields_known) ;i++)
        sglib_ent_field_t_add(&ent_fields, &ent_fields_known[i]);

    luaL_newmetatable(lua_state, "entity");
    lua_pushvalue(lua_state, -1);
    lua_setfield(lua_state, -2, "__index");
    luaL_register(lua_state, NULL, entity_metamap);
    lua_pop(lua_state, 1);

    cmd_register("g_list_entities", NULL, &g_list_entities_f, 0);

    /* register funcs to work with entity */
    lua_register(lua_state, "ent_spawn", &ent_lua_spawn);
    lua_register(lua_state, "ent_remove", &ent_lua_remove);
    lua_register(lua_state, "ent_set_sprite", &ent_lua_set_sprite);
    lua_register(lua_state, "ent_set_text", &ent_lua_set_text);
    lua_register(lua_state, "ent_set_model", &ent_lua_set_model);
    lua_register(lua_state, "ent_set_circle", &ent_lua_set_circle);
    lua_register(lua_state, "ent_set_line", &ent_lua_set_line);

    /* register global constants */
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
