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

/**
 * @file sys.h
 *
 * @brief Platform-independent system-related stuff header
 *
 * @author ftrvxmtrx
 */

#ifndef _SYS_H
#define _SYS_H

/**
 * @brief Time on last sys_get_time call
 */
extern double sys_time;

void init_time (void);

/**
 * @brief Func to get current time
 *
 * @return Time from engine start in seconds
 */
double sys_get_time (void);

int sys_mkdir (char *path);

int sys_unlink (const char *filename);

#ifdef ENGINE_SYS_PRINTF

void sys_printf_real (const char *file, int line, const char *func, const char *format, ...)
    GNUC_PRINTF(4, 5);
#define sys_printf(a...) sys_printf_real(__FILE__, __LINE__, __FUNCTION__, a)
#define TRACE sys_printf("------ trace ------\n")

#else /* !ENGINE_SYS_PRINTF */

#define sys_printf(a...)
#define sys_printf_real(a...)
#define TRACE

#endif /* ENGINE_SYS_PRINTF */

#endif /* !_SYS_H */
