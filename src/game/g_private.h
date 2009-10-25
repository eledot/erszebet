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

#ifndef _G_PRIVATE_H
#define _G_PRIVATE_H

#include "common.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "game/g_entity.h"
#include "game/g_physics.h"
#include "game/g_render.h"

void g_set_double (const char *name, double value) GNUC_NONNULL;
void g_set_integer (const char *name, int value) GNUC_NONNULL;
void g_set_string (const char *name, const char *value) GNUC_NONNULL;

void g_push_vector (const double *vector, int num) GNUC_NONNULL;
bool g_pop_vector (int index, double *vector, int num) GNUC_NONNULL;
void g_push_strings (const char **strings, int num) GNUC_NONNULL;
void g_push_field (const void *data, int offset, int type) GNUC_NONNULL;
void g_pop_field (void *data, int offset, int type, int index) GNUC_NONNULL;

bool g_lua_call_real (int args, int ret, PUV const char *file, PUV int line, PUV const char *func);
#define g_lua_call(args, ret) g_lua_call_real(args, ret, __FILE__, __LINE__, __FUNCTION__)

#define TRACE_STACKTOP(msg) sys_printf("lua stack top = %i (%s)\n", lua_gettop(lst), msg)

extern mem_pool_t g_mempool;
extern double g_time;
extern lua_State *lua_state;

#endif /* !_G_PRIVATE_H */
