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
 * @file misc.h
 *
 * @brief Misc funcs/macros/etc header
 *
 * @author ftrvxmtrx
 */

#ifndef _MISC_H
#define _MISC_H

#define UV __attribute__((unused))

/**
 * @brief Maximal filename length (passed to fs_* funcs) including \0
 */
#define MISC_MAX_FILENAME 256

/**
 * @brief Maximal cvar/cmd name length including \0
 */
#define MISC_MAX_VARNAME 64

/**
 * @brief Maximal token length including \0
 */
#define MISC_MAX_TOKLEN 1024

/**
 * @brief Number of elements in static array of any type
 */
#define STSIZE(a) ((int)(sizeof((a)) / sizeof((a)[0])))

#define BIGSHORT(b) (*((b)+1) | (*(b) << 8))
#define BIGLONG(b)  ((*(b) << 24) | (*((b)+1) << 16) | (*((b)+2) << 8) | *((b)+3))

#define LITTLESHORT(b) (*(b) | (*((b)+1) << 8))
#define LITTLELONG(b)  (*(b) | (*((b)+1) << 8) | (*((b)+2) << 16) | (*((b)+3) << 24))

#define LITTLESHORT2(b) (*(b) | (*((b)+1) << 8)); b += 2
#define LITTLELONG2(b)  (*(b) | (*((b)+1) << 8) | (*((b)+2) << 16) | (*((b)+3) << 24)); b += 4

extern int         tok_argc;
extern const char *tok_argv[];

int string_tokenize (const char *text);
int filename_is_valid (const char *name);
int varname_is_valid (const char *name);

#endif /* !_MISC_H */
