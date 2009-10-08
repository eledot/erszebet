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

#ifndef _G_MAIN_H
#define _G_MAIN_H

extern double g_time;

void g_call_cmd (const cmd_t *cmd, int source, int argc, const char **argv);
void g_destroy_cmd (cmd_t *cmd);
void g_call_func (const char *func, const char *types, ...);

void g_set_double (const char *name, double value);
void g_set_integer (const char *name, int value);
void g_set_string (const char *name, const char *value);

void g_push_vector (const double *vector, int num);
int g_pop_vector (int index, double *vector, int num);
void g_push_strings (const char **strings, int num);

void g_frame (void);

int g_lua_call_real (int args, int ret, PUV const char *file, PUV int line, PUV const char *func);
#define g_lua_call(args, ret) g_lua_call_real(args, ret, __FILE__, __LINE__, __FUNCTION__)

#define TRACE_STACKTOP(msg) sys_printf("lua stack top = %i (%s)\n", lua_gettop(lst), msg)

int g_init (void);
void g_shutdown (void);

#endif /* !_G_MAIN_H */
