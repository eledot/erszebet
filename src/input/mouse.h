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
 * @file mouse.h
 *
 * @brief Mouse input interface header
 *
 * @author ftrvxmtrx
 */

#ifndef _MOUSE_H
#define _MOUSE_H

/**
 * @brief Receives all mouse-related events
 *
 * @param button Button index (0 if nothing pressed, etc.)
 * @param down false if it's 'up' event, true if 'down'
 * @param dx Relative cursor X offset in pixels
 * @param dy Relative cursor Y offset in pixels
 */
void mouse_event (int button, erbool is_down, int dx, int dy);

/**
 * @brief Mouse module initialization
 *
 * @return 0 on success
 */
erbool mouse_init (void);

/**
 * @brief Mouse module deinitialization
 */
void mouse_shutdown (void);

#endif
