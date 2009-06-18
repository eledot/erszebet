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

#define GAME_MAIN_FILE "game/main.lua"

#define g_error(status, msg) g_error_real(status, msg, __FILE__, __LINE__, __FUNCTION__)

static int g_i = 0;

static mem_pool_t mempool;

static lua_State *lst = NULL;
static double     g_start_time;
double            g_time;

/*
=================
g_error
=================
*/
static int g_error_real (int         status,
                         const char *msg PUV,
                         const char *file PUV,
                         int         line PUV,
                         const char *func PUV)
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
g_call_func
=================
*/
void g_call_func (const char *func, const char *types, ...)
{
    va_list args;
    int     args_num, res_num;

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

    if (0 != g_error(lua_pcall(lst, args_num, res_num, 0), "lua_pcall failed"))
    {
        sys_printf("tried to call %s with %i arg(s) and %i result(s)\n", func, args_num, res_num);
        return;
    }

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
void g_pop_vector (int index, double *vector, int num)
{
    int i;

    lua_pushnil(lst);

    for (i = 0; i < num && lua_next(lst, index) != 0 ;i++)
    {
        vector[i] = lua_tointeger(lst, -1);
        lua_pop(lst, 1);
    }

    lua_pop(lst, 1);
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
game_lua_set_gravity
=================
*/
static int game_lua_set_gravity (lua_State *lst)
{
    double gravity = luaL_checknumber(lst, -1);

    g_physics_set_gravity(gravity);

    return 0;
}

/*
=================
g_init
=================
*/
int g_init (void)
{
    mem_alloc_static_pool("game", 0);

    if (NULL == (lst = lua_open()))
    {
        sys_printf("lua_open failed\n");
        return -1;
    }

    luaL_openlibs(lst);

    g_set_double("time", g_time);
    g_physics_init(mempool);
    g_entity_init(lst, mempool);

    lua_register(lst, "set_gravity", &game_lua_set_gravity);

    if (0 != g_error(luaL_loadfile(lst, GAME_MAIN_FILE), "failed to load " GAME_MAIN_FILE))
    {
        return -2;
    }

    g_error(lua_pcall(lst, 0, LUA_MULTRET, 0), "lua_pcall failed");
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
