/*
   Copyright (C) 2009 Serge "ftrvxmtrx" Ziryukin

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
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

double g_time;

void g_call_func (const char *func, const char *types, ...);

void g_set_double (const char *name, double value);
void g_set_integer (const char *name, int value);
void g_set_string (const char *name, const char *value);

void g_push_vector (const double *vector, int num);
void g_pop_vector (int index, double *vector, int num);

void g_frame (void);

int g_init (void);
void g_shutdown (void);

#endif /* !_G_MAIN_H */
