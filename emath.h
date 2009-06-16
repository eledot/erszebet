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
 * @file emath.h
 *
 * @brief Maths interface header
 *
 * @author ftrvxmtrx
 */

#ifndef _EMATH_H
#define _EMATH_H

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif /* !M_PI */

#define MIN(a, b)          ((a) < (b) ? (a) : (b))
#define MAX(a, b)          ((a) > (b) ? (a) : (b))
#define CLAMP(v, min, max) (MAX(MIN((v), (max)), (min)))
#define IS_PWROV2(n)       (!((n) & ((n) - 1)))

int ceil_pwrov2 (int n);

#endif /* !_EMATH_H */
