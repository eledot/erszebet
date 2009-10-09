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

#ifndef ENGINE_OS_IPHONE
#define GAME_MAIN_FILE "game/main.lua"
#else
#include "fs_helpers_apple.h"
#define GAME_MAIN_FILE fs_get_resource_path("main.lua", mempool)
#endif

static int g_i = 0;

static mem_pool_t mempool;

static lua_State *lst = NULL;
static double     g_start_time;
double            g_time;

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

    if (!status || !g_i)
        return status;

    err = lua_tostring(lst, -1);
    sys_printf_real(file, line, func, "%s (%s)\n", msg, err);
    lua_pop(lst, 1);

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
void g_call_cmd (const cmd_t *cmd, int source, int argc, const char **argv)
{
    lua_getref(lst, cmd->lua_func);
    lua_pushstring(lst, cmd->name);
    lua_pushinteger(lst, source);
    lua_pushinteger(lst, argc);
    g_push_strings(argv, argc);

    g_lua_call(4, 0);
    lua_pop(lst, 1);
}

/*
=================
g_destroy_cmd
=================
*/
void g_destroy_cmd (cmd_t *cmd)
{
    if (!g_i)
        return;

    if (cmd->lua_func)
        lua_unref(lst, cmd->lua_func);
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

    lua_getglobal(lst, func);

    if (lua_isnil(lst, -1))
    {
        lua_pop(lst, 1);
        return;
    }

    va_start(args, types);
    args_num = 0;

    while (*types)
    {
        switch (*types++)
        {
        case 'd':
            lua_pushnumber(lst, va_arg(args, double));
            break;

        case 'i':
            lua_pushinteger(lst, va_arg(args, int));
            break;

        case 's':
            lua_pushstring(lst, va_arg(args, char *));
            break;

        case '.':
            goto done;

        default:
            sys_printf("unknown argument type \"%c\"\n", *(types - 1));
            lua_pop(lst, 1);
            return;
        }

        args_num++;
        luaL_checkstack(lst, 1, "too many arguments");
    }

done:

    res_num = strlen(types);
    lua_pcall(lst, args_num, res_num, 0);

    pop = res_num;
    res_num = -res_num;

    while (*types)
    {
        switch (*types++)
        {
        case 'd':
            if (!lua_isnumber(lst, res_num))
            {
                sys_printf("result at %i is not a number when calling %s\n", res_num, func);
                return;
            }
            *va_arg(args, double *) = lua_tonumber(lst, res_num);
            break;

        case 'i':
            if (!lua_isnumber(lst, res_num))
            {
                sys_printf("result at %i is not a number when calling %s\n", res_num, func);
                return;
            }
            *va_arg(args, int *) = (int)lua_tointeger(lst, res_num);
            break;

        case 's':
            if (!lua_isstring(lst, res_num))
            {
                sys_printf("result at %i is not a string when calling %s\n", res_num, func);
                return;
            }
            *va_arg(args, const char **) = lua_tostring(lst, res_num);
            break;

        default:
            sys_printf("unknown argument type \"%c\"\n", *(types - 1));
            return;
        }

        res_num++;
    }

    lua_pop(lst, pop);

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

    lua_pushnumber(lst, value);
    lua_setglobal(lst, name);
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

    lua_pushinteger(lst, value);
    lua_setglobal(lst, name);
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

    lua_pushstring(lst, value);
    lua_setglobal(lst, name);
}

/*
=================
g_push_vector
=================
*/
void g_push_vector (const double *vector, int num)
{
    int i;

    lua_createtable(lst, num, 0);

    for (i = 0; i < num ;i++)
    {
        lua_pushinteger(lst, i + 1);
        lua_pushnumber(lst, vector[i]);
        lua_settable(lst, -3);
    }
}

/*
=================
g_pop_vector
=================
*/
int g_pop_vector (int index, double *vector, int num)
{
    int i;

    if (!lua_istable(lst, index))
        return -1;

    lua_pushnil(lst);

    for (i = 0; i < num && lua_next(lst, index) != 0 ;i++)
    {
        vector[i] = lua_tointeger(lst, -1);
        lua_pop(lst, 1);
    }

    lua_pop(lst, 1);

    return 0;
}

/*
=================
g_push_strings
=================
*/
void g_push_strings (const char **strings, int num)
{
    int i;

    lua_createtable(lst, num, 0);

    for (i = 0; i < num ;i++)
    {
        lua_pushinteger(lst, i + 1);
        lua_pushstring(lst, strings[i]);
        lua_settable(lst, -3);
    }
}

/*
=================
g_frame
=================
*/
void g_frame (void)
{
    g_time = sys_time - g_start_time;
    g_set_double("time", g_time);
    g_entity_frame();
    g_physics_frame();
}

/*
=================
g_lua_call_real
=================
*/
int g_lua_call_real (int args, int ret, PUV const char *file, PUV int line, PUV const char *func)
{
    int base = lua_gettop(lst) - args;
    lua_pushcfunction(lst, g_lua_traceback);
    lua_insert(lst, base);
    g_error_real(lua_pcall(lst, args, ret, base), "lua_pcall failed", file, line, func);
    lua_remove(lst, base);

    return 0;
}

/*
=================
game_lua_set_gravity
=================
*/
static int game_lua_set_gravity (lua_State *lst)
{
    double gravity = luaL_checknumber(lst, 1);

    g_physics_set_gravity(gravity);

    return 0;
}

/*
=================
game_lua_get_gravity
=================
*/
static int game_lua_get_gravity (lua_State *lst)
{
    lua_pushnumber(lst, g_physics_get_gravity());

    return 1;
}

/*
=================
game_lua_set_speed
=================
*/
static int game_lua_set_speed (lua_State *lst)
{
    double speed = luaL_checknumber(lst, 1);

    g_physics_set_speed(speed);

    return 0;
}

/*
=================
game_lua_get_speed
=================
*/
static int game_lua_get_speed (lua_State *lst)
{
    lua_pushnumber(lst, g_physics_get_speed());

    return 1;
}

/*
=================
game_lua_register_cmd
=================
*/
static int game_lua_register_cmd (lua_State *lst)
{
    const char *name  = luaL_checkstring(lst, 1);
    int         flags = luaL_checknumber(lst, 3);
    cmd_t      *cmd   = cmd_register(name, NULL, NULL, flags);
    int         func;

    lua_pushvalue(lst, 2);
    func = lua_ref(lst, 1);

    if (NULL == cmd)
        return 0;

    cmd->lua_func = func;

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
    lua_pushinteger(lst, VIDEO_INFO_WIDTH);
    lua_pushinteger(lst, video_width);
    lua_settable(lst, -3);
    lua_pushinteger(lst, VIDEO_INFO_HEIGHT);
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

    lua_register(lst, "register_cmd", &game_lua_register_cmd);
    lua_register(lst, "execute", &game_lua_execute);
}

/*
=================
g_init_cvar
=================
*/
static void g_init_cvar (void)
{
    lua_register(lst, "get_cvar_string", &game_lua_get_cvar_string);
    lua_register(lst, "get_cvar_integer", &game_lua_get_cvar_integer);
    lua_register(lst, "get_cvar_number", &game_lua_get_cvar_number);
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

    lua_register(lst, "set_orientation", &game_lua_set_orientation);
    lua_register(lst, "get_video_info", &game_lua_get_video_info);
}

/*
=================
g_init
=================
*/
int g_init (void)
{
    const char *file;

    mem_alloc_static_pool("game", 0);

    if (NULL == (lst = lua_open()))
    {
        sys_printf("lua_open failed\n");
        return -1;
    }

    luaL_openlibs(lst);

    g_init_cmd();
    g_init_cvar();
    g_init_video();
    g_set_double("time", g_time);
    g_set_double("RANDSEED", sys_get_time() * 10000);
    g_physics_init(mempool);
    g_entity_init(lst, mempool);

    lua_register(lst, "phys_set_gravity", &game_lua_set_gravity);
    lua_register(lst, "phys_get_gravity", &game_lua_get_gravity);
    lua_register(lst, "phys_set_speed", &game_lua_set_speed);
    lua_register(lst, "phys_get_speed", &game_lua_get_speed);

    file = GAME_MAIN_FILE;
    sys_printf("loading gamecode from \"%s\"\n", file);

    if (0 != g_error(luaL_loadfile(lst, file), "failed to load gamecode"))
    {
        return -2;
    }

    g_lua_call(0, LUA_MULTRET);
    lua_pop(lst, 1);

    g_i = 1;

    g_start_time = sys_get_time();
    g_time = 0.0;

    sys_printf("+g\n");

    return 0;
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
    g_entity_shutdown();
    g_physics_shutdown();
    lua_close(lst);

    mem_free_static_pool();

    g_i = 0;

    sys_printf("-g\n");
}
