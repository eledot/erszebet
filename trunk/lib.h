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
 * @file lib.h
 *
 * @brief Dynlib interface header
 *
 * @author ftrvxmtrx
 */

#ifndef _LIB_H
#define _LIB_H

/**
 * @brief Library handle
 */
typedef void * lib_t;

/**
 * @brief Library func in the list
 */
typedef struct
{
    char  *name; /**< Func name                         */
    void **func; /**< Pointer to store function address */
}lib_func_t;

/**
 * @brief Invalid library handle
 */
#define LIB_HANDLE_INVALID NULL

/**
 * @brief Gets library functions
 *
 * @param lib Library handle
 * @param funcs Functions list
 * @param ignore_errors Non-zero means it will ignore missing functions and continue to get funcs
 *
 * @return Number of missing funcstions (0 is a complete success)
 */
int lib_get_funcs (lib_t lib, lib_func_t *funcs, int ignore_errors);

/**
 * @brief Opens a library and gets needed funcs
 *
 * @param names Array of possible library names to try
 * @param funcs Funcstions list
 * @param ignore_errors Non-zero means it will ignore missing functions and continue to get funcs
 *
 * @return Library handle or LIB_HANDLE_INVALID
 */
lib_t lib_open (const char **names, lib_func_t *funcs, int ignore_errors);

/**
 * @brief Closes dynlib handle
 *
 * @param lib Library handle
 */
void lib_close (lib_t lib);

#endif /* !_LIB_H */
