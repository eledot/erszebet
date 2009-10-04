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

#ifndef _STRLCPY_H
#define _STRLCPY_H

/**
 * @brief More safe and useful strncpy
 *
 * @param dst Destination buffer
 * @param src Source buffer
 * @param n Size of destination buffer
 *
 * @return Length of the final string
 */
size_t strlcpy (char *dst, const char *src, size_t n);

#endif /* !_STRLCPY_H */
