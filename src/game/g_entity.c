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

#include "game/g_private.h"
#include "sglib.h"

GNUC_NONNULL static void ent_set_origin_callback (g_entity_t *ent);

static g_entity_field_t ent_fields_base[] =
{
#define STRUCTURE_FOR_OFFSETS g_entity_t
    ENTITY_FIELD("classname", classname, STRING_COPY, NULL),
    ENTITY_FIELD("flags",     flags,     INTEGER,     NULL),
    ENTITY_FIELD("nextthink", nextthink, DOUBLE,      NULL),
    ENTITY_FIELD("lastthink", lastthink, DOUBLE,      NULL),
    ENTITY_FIELD("origin",    origin,    VECTOR,      &ent_set_origin_callback),
    ENTITY_FIELD("origin_x",  origin[0], DOUBLE,      &ent_set_origin_callback),
    ENTITY_FIELD("origin_y",  origin[1], DOUBLE,      &ent_set_origin_callback),
    ENTITY_FIELD("origin_z",  origin[2], DOUBLE,      &ent_set_origin_callback),
    ENTITY_FIELD("angle",     angle,     DOUBLE,      NULL),
    ENTITY_FIELD_NULL
};

static g_entity_field_t *ent_fields;

#define ENT_FIELD_COMPARATOR(f1, f2) (f2->index - f1->index) ? (f2->index - f1->index) : strcmp(f1->name, f2->name)

SGLIB_DEFINE_SORTED_LIST_PROTOTYPES(g_entity_field_t, ENT_FIELD_COMPARATOR, next);
SGLIB_DEFINE_SORTED_LIST_FUNCTIONS(g_entity_field_t, ENT_FIELD_COMPARATOR, next);

/* entity field offsets for strings that must be freed */
static const int ent_fields_free[] =
{
    offsetof(g_entity_t, classname)
};

#define ENT_ZORDER_COMPARATOR(e1, e2) ((int)e1->origin[2] == (int)e2->origin[2] ? \
                                       -1 : \
                                       (int)e1->origin[2] - (int)e2->origin[2])

SGLIB_DEFINE_SORTED_LIST_PROTOTYPES(g_entity_t, ENT_ZORDER_COMPARATOR, next);
SGLIB_DEFINE_SORTED_LIST_FUNCTIONS(g_entity_t, ENT_ZORDER_COMPARATOR, next);

static g_entity_t *entities;
static g_entity_t *remove_entities;

/*
=================
ent_get_field
=================
*/
GNUC_NONNULL static int ent_get_field (const g_entity_t *ent, const char *field)
{
    const g_entity_field_t *f;
    g_entity_field_t s = { .name = field };

    f = sglib_g_entity_field_t_find_member(ent_fields, &s);

    if (NULL != f)
    {
        g_push_field(ent, f->offset, f->type);
        return 1;
    }

    return 0;
}

/*
=================
ent_set_origin_callback
=================
*/
GNUC_NONNULL static void ent_set_origin_callback (GNUC_UNUSED g_entity_t *ent)
{
    sglib_g_entity_t_sort(&entities);
    g_physics_update_ent_origin(ent);
}

/*
=================
ent_set_field
=================
*/
GNUC_NONNULL static bool ent_set_field (g_entity_t *ent, const char *field, int index)
{
    const g_entity_field_t *f;
    void *data = ent;
    g_entity_field_t s = { .name = field };

    f = sglib_g_entity_field_t_find_member(ent_fields, &s);

    if (NULL != f)
    {
        if (f->index)
        {
            if (f->index == ent->render_index ||
                f->index == ENT_FIELD_INDEX_RENDER_COMMON)
            {
                data = ent->render_data;
            }
            else if (f->index == ENT_FIELD_INDEX_PHYSICS)
            {
                data = ent->physics_data;
            }
        }

        if (NULL != data)
        {
            if (G_FIELD_TYPE_CUSTOM_CALLBACK != f->type)
                g_pop_field(data, f->offset, f->type, index);

            if (NULL != f->callback)
                f->callback(ent);
        }

        return 1;
    }

    return 0;
}

/*
=================
g_entity_flags_string
=================
*/
GNUC_NONNULL static const char *g_entity_flags_string (const g_entity_t *ent)
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

    if (ent->internal_flags & ENT_INTFL_PHYSICS_STATIC)
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
g_entity_string
=================
*/
GNUC_NONNULL static void g_entity_string (const g_entity_t *ent, char *buffer, int size)
{
    snprintf(buffer,
             size,
             "entity: %p "
             "classname=\"%s\" "
             "flags=( %s) "
             "nextthink=%-2.2lf "
             "lastthink=%-2.2lf "
             "origin={ %-2.2lf %-2.2lf %-2.2lf } "
             "angle=%-2.2lf",
             ent,
             ent->classname ? ent->classname : "-noname-",
             g_entity_flags_string(ent),
             ent->nextthink,
             ent->lastthink,
             ent->origin[0],
             ent->origin[1],
             ent->origin[2],
             ent->angle);
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

    g_entity_string(ent, tmp, sizeof(tmp));
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
             { "touch", ENT_INTFL_TOUCH }
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

    if (i < STSIZE(ent_newindex_internal_flags) || !ent_set_field(ent, key, 3))
    {
        lua_getref(lst, ent->lua_dataref);
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

    if (NULL == (ent = mem_alloc(g_mempool, sizeof(*ent))))
    {
        sys_printf("failed to allocate memory for entity\n");
        return NULL;
    }

    ent->lua_ref     = LUA_REFNIL;
    ent->lua_dataref = LUA_REFNIL;

    ent->origin[0] = 0.0;
    ent->origin[1] = 0.0;
    ent->origin[2] = 0.0;
    ent->angle     = 0.0;
    ent->nextthink = 0.0;
    ent->lastthink = 0.0;

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
    lua_getref(lua_state, ent->lua_ref);
    lua_pushlightuserdata(lua_state, NULL);
    lua_setfield(lua_state, -2, "__ref");
    lua_pop(lua_state, 1);

    lua_unref(lua_state, ent->lua_ref);
    lua_unref(lua_state, ent->lua_dataref);

    /* remove all handlers, mark invalid */
    ent->internal_flags -= ent->internal_flags & (ENT_INTFL_THINK | ENT_INTFL_TOUCH);
    ent->flags |= ENT_FL_NON_SOLID;

    ent->lua_ref = LUA_REFNIL;
    ent->lua_dataref = LUA_REFNIL;
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
    ent->lua_ref = lua_ref(lst, 1);
    lua_pushlightuserdata(lst, ent);
    lua_setfield(lst, -2, "__ref");

    lua_newtable(lst);
    lua_pushvalue(lst, -1);
    ent->lua_dataref = lua_ref(lst, -1);
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
g_entity_draw_entities
=================
*/
void g_entity_draw_entities (void)
{
    g_entity_t *ent;
    struct sglib_g_entity_t_iterator it;

    for (ent = sglib_g_entity_t_it_init(&it, entities);
         NULL != ent;
         ent = sglib_g_entity_t_it_next(&it))
    {
        if (ent->render_valid)
            g_render_entity(ent);
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
        if ((ent->internal_flags & ENT_INTFL_THINK) &&
            ent->nextthink > ent->lastthink &&
            ent->nextthink < g_time &&
            LUA_REFNIL != ent->lua_ref)
        {
            ent->lastthink = ent->nextthink;
            lua_getref(lua_state, ent->lua_dataref);
            lua_getfield(lua_state, -1, "think");
            lua_getref(lua_state, ent->lua_ref);
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
        g_entity_string(ent, tmp, sizeof(tmp));
        sys_printf("%s\n", tmp);
    }
}

/*
=================
g_entity_field_type_string
=================
*/
const char *g_entity_field_type_string (int type)
{
    const char const *field_types[] =
    {
        [G_FIELD_TYPE_DOUBLE] = "double",
        [G_FIELD_TYPE_INTEGER] = "integer",
        [G_FIELD_TYPE_VECTOR] = "vector",
        [G_FIELD_TYPE_STRING] = "string_callback",
        [G_FIELD_TYPE_STRING_COPY] = "string_copy",
        [G_FIELD_TYPE_BOOL] = "bool",
        [G_FIELD_TYPE_CUSTOM_CALLBACK] = "custom_callback"
    };

    if (type < 0 || type >= G_FIELD_TYPES_NUM)
        return "INVALID";

    return field_types[type];
}

/*
=================
g_entity_add_field
=================
*/
void g_entity_add_field (g_entity_field_t *field)
{
    if (NULL != sglib_g_entity_field_t_find_member(ent_fields, field))
    {
        sys_printf("duplicate field \"%s\" found\n", field->name);
    }
    else if (field->type < 0 || field->type >= G_FIELD_TYPES_NUM)
    {
        sys_printf("invalid field \"%s\" (type=%i)\n", field->name, field->type);
    }
    else if (G_FIELD_TYPE_CUSTOM_CALLBACK == field->type && NULL == field->callback)
    {
        sys_printf("custom field \"%s\" without callback\n", field->name);
    }

    sglib_g_entity_field_t_add(&ent_fields, field);
}

/*
=================
g_entity_add_field_list
=================
*/
void g_entity_add_field_list (g_entity_field_t *fields, int index)
{
    g_entity_field_t *f;

    for (f = fields; NULL != f->name ;f++)
    {
        f->index = index;
        g_entity_add_field(f);
    }
}

/*
=================
g_entity_delete_field
=================
*/
void g_entity_delete_field (g_entity_field_t *field)
{
    sglib_g_entity_field_t_delete(&ent_fields, field);
}

/*
=================
g_entity_delete_field_list
=================
*/
void g_entity_delete_field_list (g_entity_field_t *fields)
{
    g_entity_field_t *f;

    for (f = fields; NULL != f->name ;f++)
    {
        f->index = ENT_FIELD_INDEX_INVALID;
        g_entity_delete_field(f);
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

    entities = NULL;
    remove_entities = NULL;
    ent_fields = NULL;

    g_entity_add_field_list(ent_fields_base, ENT_FIELD_INDEX_BASE);

    luaL_newmetatable(lua_state, "entity");
    lua_pushvalue(lua_state, -1);
    lua_setfield(lua_state, -2, "__index");
    luaL_register(lua_state, NULL, entity_metamap);
    lua_pop(lua_state, 1);

    cmd_register("g_list_entities", NULL, &g_list_entities_f, 0);

    /* register funcs to work with entity */
    lua_register(lua_state, "ent_spawn", &ent_lua_spawn);
    lua_register(lua_state, "ent_remove", &ent_lua_remove);

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

    g_entity_delete_field_list(ent_fields_base);

    sys_printf("-g_entity\n");
}
