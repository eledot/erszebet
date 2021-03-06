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

#include "game/g_private.h"

#ifndef ENGINE_OS_IPHONE
#define GAME_MAIN_FILE "game/main.lua"
#else
#include "fs_helpers_apple.h"
#define GAME_MAIN_FILE fs_get_resource_path("luac.out")
#endif

static erbool g_i = false;

mem_pool_t g_mempool;
lua_State *lua_state;
double g_time;

static double  g_start_time;
static cvar_t *g_speed;

enum
{
    VIDEO_INFO_WIDTH = 1,
    VIDEO_INFO_HEIGHT,
    VIDEO_INFO_FULLSCREEN,
    VIDEO_INFO_GRABBED,
    VIDEO_INFO_ORIENTATION,
    VIDEO_INFO_LAST
};

#define g_error(status, msg) g_error_real(status, msg, __FILE__, __LINE__, __FUNCTION__)

/*
=================
g_error
=================
*/
static int g_error_real (int             status,
                         PUV const char *msg,
                         PUV const char *file,
                         PUV int         line,
                         PUV const char *func)
{
    const char *err;

    errno = 0;

    if (!status)
        return status;

    err = lua_tostring(lua_state, -1);
    sys_printf_real(file, line, func, "%s (%s)\n", msg, err);
    lua_pop(lua_state, 1);

    return status;
}

/*
=================
g_lua_traceback
=================
*/
static int g_lua_traceback (lua_State *lst)
{
    if (!lua_isstring(lst, 1))
        return 1;

    lua_getfield(lst, LUA_GLOBALSINDEX, "debug");

    if (!lua_istable(lst, -1))
    {
        lua_pop(lst, 1);
        return 1;
    }

    lua_getfield(lst, -1, "traceback");

    if (!lua_isfunction(lst, -1))
    {
        lua_pop(lst, 2);
        return 1;
    }

    lua_pushvalue(lst, 1);
    lua_pushinteger(lst, 2);
    lua_call(lst, 2, 1);

    sys_printf("%s\n", luaL_checkstring(lst, -1));

    return 1;
}

/*
=================
g_call_cmd
=================
*/
static void g_call_cmd (const cmd_t *cmd, int source, int argc, const char **argv)
{
    if (!g_i || !cmd->userdata)
        return;

    lua_getref(lua_state, cmd->userdata);
    lua_pushstring(lua_state, cmd->name);
    lua_pushinteger(lua_state, source);
    lua_pushinteger(lua_state, argc);
    g_push_strings(argv, argc);

    g_lua_call(4, 0);
    lua_pop(lua_state, 1);
}

/*
=================
g_destroy_cmd
=================
*/
static void g_destroy_cmd (cmd_t *cmd)
{
    if (!g_i)
        return;

    if (cmd->userdata)
        lua_unref(lua_state, cmd->userdata);
}

/*
=================
g_call_func
=================
*/
void g_call_func (const char *func, const char *types, ...)
{
    va_list args;
    int     args_num, res_num, pop;

    if (NULL == func || NULL == types)
    {
        sys_printf("bad args (%p, %p)\n", func, types);
        return;
    }

    lua_getglobal(lua_state, func);

    if (lua_isnil(lua_state, -1))
    {
        lua_pop(lua_state, 1);
        return;
    }

    va_start(args, types);
    args_num = 0;

    while (*types)
    {
        switch (*types++)
        {
        case 'd':
            lua_pushnumber(lua_state, va_arg(args, double));
            break;

        case 'i':
            lua_pushinteger(lua_state, va_arg(args, int));
            break;

        case 's':
            lua_pushstring(lua_state, va_arg(args, char *));
            break;

        case 'b':
            lua_pushboolean(lua_state, va_arg(args, erbool));
            break;

        case '.':
            goto done;

        default:
            sys_printf("unknown argument type \"%c\"\n", *(types - 1));
            lua_pop(lua_state, 1);
            return;
        }

        args_num++;
        luaL_checkstack(lua_state, 1, "too many arguments");
    }

done:

    res_num = strlen(types);
    lua_pcall(lua_state, args_num, res_num, 0);

    pop = res_num;
    res_num = -res_num;

    while (*types)
    {
        switch (*types++)
        {
        case 'd':
            if (!lua_isnumber(lua_state, res_num))
            {
                sys_printf("result at %i is not a number when calling %s\n", res_num, func);
                return;
            }
            *va_arg(args, double *) = lua_tonumber(lua_state, res_num);
            break;

        case 'i':
            if (!lua_isnumber(lua_state, res_num))
            {
                sys_printf("result at %i is not a number when calling %s\n", res_num, func);
                return;
            }
            *va_arg(args, int *) = (int)lua_tointeger(lua_state, res_num);
            break;

        case 's':
            if (!lua_isstring(lua_state, res_num))
            {
                sys_printf("result at %i is not a string when calling %s\n", res_num, func);
                return;
            }
            *va_arg(args, const char **) = lua_tostring(lua_state, res_num);
            break;

        case 'b':
            if (!lua_isboolean(lua_state, res_num))
            {
                sys_printf("result at %i is not a boolean when calling %s\n", res_num, func);
                return;
            }
            *va_arg(args, erbool *) = lua_toboolean(lua_state, res_num);
            break;

        default:
            sys_printf("unknown argument type \"%c\"\n", *(types - 1));
            return;
        }

        res_num++;
    }

    lua_pop(lua_state, pop);

    va_end(args);
}

/*
=================
g_set_double
=================
*/
void g_set_double (const char *name, double value)
{
    if (NULL == name)
    {
        sys_printf("bad args (%p, %lf)\n", name, value);
        return;
    }

    lua_pushnumber(lua_state, value);
    lua_setglobal(lua_state, name);
}

/*
=================
g_set_integer
=================
*/
void g_set_integer (const char *name, int value)
{
    if (NULL == name)
    {
        sys_printf("bad args (%p, %i)\n", name, value);
        return;
    }

    lua_pushinteger(lua_state, value);
    lua_setglobal(lua_state, name);
}

/*
=================
g_set_string
=================
*/
void g_set_string (const char *name, const char *value)
{
    if (NULL == name || NULL == value)
    {
        sys_printf("bad args (%p, %p)\n", name, value);
        return;
    }

    lua_pushstring(lua_state, value);
    lua_setglobal(lua_state, name);
}

/*
=================
g_push_vector
=================
*/
void g_push_vector (const double *vector, int num)
{
    int i;

    lua_createtable(lua_state, num, 0);

    for (i = 0; i < num ;i++)
    {
        lua_pushinteger(lua_state, i + 1);
        lua_pushnumber(lua_state, vector[i]);
        lua_settable(lua_state, -3);
    }
}

/*
=================
g_pop_vector
=================
*/
erbool g_pop_vector (int index, double *vector, int num)
{
    int i;

    for (i = 0; i < num ;i++)
        vector[i] = 0.0;

    if (!lua_istable(lua_state, index))
        return false;

    for (i = 0; i < num ;i++)
    {
        lua_rawgeti(lua_state, index, i + 1);

        if (lua_isnumber(lua_state, -1))
            vector[i] = lua_tonumber(lua_state, -1);

        lua_pop(lua_state, 1);
    }

    return true;
}

/*
=================
g_push_strings
=================
*/
void g_push_strings (const char **strings, int num)
{
    int i;

    lua_createtable(lua_state, num, 0);

    for (i = 0; i < num ;i++)
    {
        lua_pushinteger(lua_state, i + 1);
        lua_pushstring(lua_state, strings[i]);
        lua_settable(lua_state, -3);
    }
}

/*
=================
g_pop_field
=================
*/
void g_pop_field (void *data, int offset, int type, int index, erbool check)
{
    void *field = data + offset;

    switch (type)
    {
    case G_FIELD_TYPE_DOUBLE:
        if (!check || lua_isnumber(lua_state, index))
            *(double *)field = lua_tonumber(lua_state, index);
        break;

    case G_FIELD_TYPE_INTEGER:
        if (!check || lua_isnumber(lua_state, index))
            *(int *)field = lua_tointeger(lua_state, index);
        break;

    case G_FIELD_TYPE_VECTOR:
        if (!check || lua_istable(lua_state, index))
            g_pop_vector(index, (double *)field, 3);
        break;

    case G_FIELD_TYPE_STRING:
        if (lua_isstring(lua_state, index))
            *(const char **)field = luaL_checkstring(lua_state, index);
        else if (!check)
            *(const char **)field = NULL;
        break;

    case G_FIELD_TYPE_STRING_COPY:
        if (!check && NULL != *(char **)field)
        {
            mem_free(*(char **)field);
            *(char **)field = NULL;
        }

        if (lua_isstring(lua_state, index))
            *(char **)field = mem_strdup(g_mempool, luaL_checkstring(lua_state, index));
        break;

    case G_FIELD_TYPE_ERBOOL:
        if (!check || lua_isboolean(lua_state, index))
            *(erbool *)field = lua_toboolean(lua_state, index);
        break;

    case G_FIELD_TYPE_CUSTOM_CALLBACK:
        break;

    default:
        sys_printf("unknown field type %i (%p->%i)\n", type, data, offset);
        break;
    }
}

/*
=================
g_push_field
=================
*/
void g_push_field (const void *data, int offset, int type)
{
    const void *field = data + offset;

    switch (type)
    {
    case G_FIELD_TYPE_DOUBLE:
        lua_pushnumber(lua_state, *(const double *)field);
        break;

    case G_FIELD_TYPE_INTEGER:
        lua_pushinteger(lua_state, *(const int *)field);
        break;

    case G_FIELD_TYPE_VECTOR:
        g_push_vector((const double *)field, 3);
        break;

    case G_FIELD_TYPE_STRING:
    case G_FIELD_TYPE_STRING_COPY:
        if (NULL == *(char **)field)
            lua_pushnil(lua_state);
        else
            lua_pushstring(lua_state, *(const char **)field);
        break;

    case G_FIELD_TYPE_ERBOOL:
        lua_pushboolean(lua_state, *(const erbool *)field);
        break;

    case G_FIELD_TYPE_CUSTOM_CALLBACK:
        lua_pushnil(lua_state);
        break;

    default:
        sys_printf("unknown field type %i (%p->%i)\n", type, data, offset);
        lua_pushnil(lua_state);
        break;
    }
}

/*
=================
g_real_time
=================
*/
double g_real_time (void)
{
    return (sys_get_time() - g_start_time) * g_speed->f;
}

/*
=================
g_frame
=================
*/
void g_frame (void)
{
    g_time = (sys_time - g_start_time) * g_speed->f;
    g_set_double("time", g_time);
    g_physics_frame();
    g_entity_frame();
}

/*
=================
g_lua_call_real
=================
*/
erbool g_lua_call_real (int args, int ret, PUV const char *file, PUV int line, PUV const char *func)
{
    int base = lua_gettop(lua_state) - args;
    lua_pushcfunction(lua_state, g_lua_traceback);
    lua_insert(lua_state, base);
    g_error_real(lua_pcall(lua_state, args, ret, base), "lua_pcall failed", file, line, func);
    lua_remove(lua_state, base);

    return true;
}

/*
=================
game_lua_register_cmd
=================
*/
static int game_lua_register_cmd (lua_State *lst)
{
    const char *name = luaL_checkstring(lst, 1);
    int flags = luaL_checknumber(lst, 3);
    cmd_t *cmd = cmd_register(name, NULL, &g_call_cmd, flags);
    int func;

    if (NULL == cmd)
        return 0;

    lua_pushvalue(lst, 2);
    func = lua_ref(lst, 1);

    cmd->delete = &g_destroy_cmd;
    cmd->userdata = func;

    return 0;
}

/*
=================
game_lua_execute
=================
*/
static int game_lua_execute (lua_State *lst)
{
    const char *s = luaL_checkstring(lst, -1);

    cmdbuf_add(s, CMD_SRC_CONSOLE);
    cmdbuf_frame();

    return 0;
}

/*
=================
game_lua_get_cvar_string
=================
*/
static int game_lua_get_cvar_string (lua_State *lst)
{
    const char *name = luaL_checkstring(lst, -1);
    cvar_t *cvar = cvar_find(name);

    if (NULL == cvar)
        lua_pushnil(lst);
    else
        lua_pushstring(lst, cvar->s);

    return 1;
}

/*
=================
game_lua_get_cvar_integer
=================
*/
static int game_lua_get_cvar_integer (lua_State *lst)
{
    const char *name = luaL_checkstring(lst, -1);
    cvar_t *cvar = cvar_find(name);

    if (NULL == cvar)
        lua_pushnil(lst);
    else
        lua_pushinteger(lst, cvar->i);

    return 1;
}

/*
=================
game_lua_get_cvar_number
=================
*/
static int game_lua_get_cvar_number (lua_State *lst)
{
    const char *name = luaL_checkstring(lst, -1);
    cvar_t *cvar = cvar_find(name);

    if (NULL == cvar)
        lua_pushnil(lst);
    else
        lua_pushnumber(lst, cvar->f);

    return 1;
}

/*
=================
game_lua_set_orientation
=================
*/
static int game_lua_set_orientation (lua_State *lst)
{
    /* FIXME -- this won't work for sdl */
    video_orientation = luaL_checknumber(lst, 1);

    return 0;
}

/*
=================
game_lua_get_video_info
=================
*/
static int game_lua_get_video_info (lua_State *lst)
{
    lua_createtable(lst, VIDEO_INFO_LAST - 1, 0);
#ifdef ENGINE_OS_IPHONE
    lua_pushinteger(lst, (video_orientation == VIDEO_LANDSCAPE) ? VIDEO_INFO_HEIGHT : VIDEO_INFO_WIDTH);
#else
    lua_pushinteger(lst, VIDEO_INFO_WIDTH);
#endif
    lua_pushinteger(lst, video_width);
    lua_settable(lst, -3);
#ifdef ENGINE_OS_IPHONE
    lua_pushinteger(lst, (video_orientation == VIDEO_LANDSCAPE) ? VIDEO_INFO_WIDTH : VIDEO_INFO_HEIGHT);
#else
    lua_pushinteger(lst, VIDEO_INFO_HEIGHT);
#endif
    lua_pushinteger(lst, video_height);
    lua_settable(lst, -3);
    lua_pushinteger(lst, VIDEO_INFO_FULLSCREEN);
    lua_pushboolean(lst, video_fullscreen);
    lua_settable(lst, -3);
    lua_pushinteger(lst, VIDEO_INFO_GRABBED);
    lua_pushboolean(lst, video_grabbed);
    lua_settable(lst, -3);
    lua_pushinteger(lst, VIDEO_INFO_ORIENTATION);
    lua_pushboolean(lst, video_orientation);
    lua_settable(lst, -3);

    return 1;
}

/*
=================
g_init_cmd
=================
*/
static void g_init_cmd (void)
{
    g_set_integer("CMD_SERVER", CMD_FL_SERVER);
    g_set_integer("CMD_THREAD", CMD_FL_THREAD);
    g_set_integer("CMD_CHEATS", CMD_FL_CHEATS);

    g_set_integer("CMD_SRC_KEY_DOWN", CMD_SRC_KEY_DOWN);
    g_set_integer("CMD_SRC_KEY_UP", CMD_SRC_KEY_UP);
    g_set_integer("CMD_SRC_CONSOLE", CMD_SRC_CONSOLE);
    g_set_integer("CMD_SRC_ENGINE", CMD_SRC_ENGINE);
    g_set_integer("CMD_SRC_GAME", CMD_SRC_GAME);
    g_set_integer("CMD_SRC_REMOTE", CMD_SRC_REMOTE);

    lua_register(lua_state, "register_cmd", &game_lua_register_cmd);
    lua_register(lua_state, "execute", &game_lua_execute);
}

/*
=================
g_init_cvar
=================
*/
static void g_init_cvar (void)
{
    lua_register(lua_state, "get_cvar_string", &game_lua_get_cvar_string);
    lua_register(lua_state, "get_cvar_integer", &game_lua_get_cvar_integer);
    lua_register(lua_state, "get_cvar_number", &game_lua_get_cvar_number);
}

/*
=================
g_init_video
=================
*/
static void g_init_video (void)
{
    g_set_integer("VIDEO_LANDSCAPE", VIDEO_LANDSCAPE);
    g_set_integer("VIDEO_PORTRAIT", VIDEO_PORTRAIT);

    g_set_integer("VIDEO_INFO_WIDTH", VIDEO_INFO_WIDTH);
    g_set_integer("VIDEO_INFO_HEIGHT", VIDEO_INFO_HEIGHT);
    g_set_integer("VIDEO_INFO_FULLSCREEN", VIDEO_INFO_FULLSCREEN);
    g_set_integer("VIDEO_INFO_GRABBED", VIDEO_INFO_GRABBED);
    g_set_integer("VIDEO_INFO_ORIENTATION", VIDEO_INFO_ORIENTATION);

    lua_register(lua_state, "set_orientation", &game_lua_set_orientation);
    lua_register(lua_state, "get_video_info", &game_lua_get_video_info);
}

/*
=================
g_draw
=================
*/
void g_draw (void)
{
    g_entity_draw_entities();
}

/*
=================
g_init
=================
*/
erbool g_init (void)
{
    const char *file;

    g_speed = cvar_get("g_speed", "1.0", CVAR_FL_MIN);
    g_speed->min = 0.1;

    g_mempool = mem_alloc_pool("game", 0);

    if (NULL == (lua_state = lua_open()))
    {
        sys_printf("lua_open failed\n");
        return false;
    }

    luaL_openlibs(lua_state);

#ifdef ENGINE_LUA_JIT
    luaopen_jit(lua_state);
#endif

    g_init_cmd();
    g_init_cvar();
    g_init_video();
    g_set_double("time", 0.0);
    g_entity_init();
    g_physics_init();
    g_render_init();

    file = GAME_MAIN_FILE;
    sys_printf("loading gamecode from \"%s\"\n", file);
	
    if (0 != g_error(luaL_loadfile(lua_state, file), "failed to load gamecode"))
    {
        return false;
    }

    g_lua_call(0, LUA_MULTRET);
    lua_pop(lua_state, 1);

    g_i = true;

    g_start_time = sys_get_time();
    g_time = 0.0;

    sys_printf("+g\n");

    return true;
}

/*
=================
g_shutdown
=================
*/
void g_shutdown (void)
{
    if (!g_i)
        return;

    g_call_func("g_shutdown", "");
    g_render_shutdown();
    g_entity_shutdown();
    g_physics_shutdown();
    lua_close(lua_state);
    lua_state = NULL;

    mem_free_pool(&g_mempool);

    g_i = false;

    sys_printf("-g\n");
}
