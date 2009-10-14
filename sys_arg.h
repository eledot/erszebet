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
 * @file sys_arg.h
 *
 * @brief Command line arguments interface header
 *
 * @author ftrvxmtrx
 */

#ifndef _SYS_ARG_H
#define _SYS_ARG_H

/**
 * @brief Searches for an argument in the cmd args list
 *
 * @param name The argument
 *
 * @return Argument index [1-...] or 0 if wasn't found
 */
int sys_arg_find (const char *name) GNUC_NONNULL;

/**
 * @brief Puts specific arguments into command buffer (like '+set fs_game blah')
 *
 * @param start First char of first argument in the chain
 */
void sys_arg_to_cmdbuf (char start);

/**
 * @brief Inits command line arguments (used only once on engine start)
 *
 * @param c argc
 * @param v argv
 */
void sys_arg_set (int c, char **v) GNUC_NONNULL;

#endif /* !_SYS_ARG_H */
