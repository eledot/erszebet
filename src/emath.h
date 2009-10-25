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
 * @brief Maths interface header
 */

#ifndef _EMATH_H
#define _EMATH_H

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#undef CLAMP
#define CLAMP(v, min, max) (MAX(MIN((v), (max)), (min)))

/**
 * @brief Checks if a number is a power-of-2 number.
 * @param[in] n The number.
 * @retval true The number IS a power of 2.
 */
#define IS_PWROV2(n) (!((n) & ((n) - 1)))

/**
 * @brief Multiplies a vector by a number.
 * @param[in] a Vector to multiply.
 * @param[in] d The number.
 * @param[out] dest Destination vector.
 */
#define vector_mul(a, d, dest)                  \
    {                                           \
        (dest)[0] = (a)[0] * d;                 \
        (dest)[1] = (a)[1] * d;                 \
        (dest)[2] = (a)[2] * d;                 \
    }

/**
 * @brief Divides a vector by a number.
 * @param[in] a Vector to divide.
 * @param[in] d The number.
 * @param[out] dest Destination vector.
 */
#define vector_div(a, d, dest)                  \
    {                                           \
        (dest)[0] = (a)[0] / d;                 \
        (dest)[1] = (a)[1] / d;                 \
        (dest)[2] = (a)[2] / d;                 \
    }

/**
 * @brief Subtracts one vector from another.
 * @param[in] a First vector.
 * @param[in] d Second vector.
 * @param[out] dest Destination vector.
 */
#define vector_sub(a, b, dest)                  \
    {                                           \
        (dest)[0] = (a)[0] - (b)[0];            \
        (dest)[1] = (a)[1] - (b)[1];            \
        (dest)[2] = (a)[2] - (b)[2];            \
    }

/**
 * @brief Adds one vector to another.
 * @param[in] a First vector.
 * @param[in] d Second vector.
 * @param[out] dest Destination vector.
 */
#define vector_add(a, b, dest)                  \
    {                                           \
        (dest)[0] = (a)[0] + (b)[0];            \
        (dest)[1] = (a)[1] + (b)[1];            \
        (dest)[2] = (a)[2] + (b)[2];            \
    }

/**
 * @brief Calculates vector length.
 * @param[in] v A vector.
 * @return Vector length
 */
#define vector_length(v)                                        \
    (sqrt((v)[0]*(v)[0] + (v)[1]*(v)[1] + (v)[2]*(v)[2]))

/**
 * @brief Makes a normal to two vectors.
 * @param[in] a First vector.
 * @param[in] b Second vector.
 * @param[out] dest Vector to store normal.
 */
#define vector_normal(a, b, dest)                         \
    {                                                     \
        (dest)[0] = (a)[1]*(b)[2] - (a)[2]*(b)[1];        \
        (dest)[1] = (a)[2]*(b)[0] - (a)[0]*(b)[2];        \
        (dest)[2] = (a)[0]*(b)[1] - (a)[1]*(b)[0];        \
    }

/**
 * @brief Normalizes a vector.
 * @param[in,out] The vector.
 */
#define vector_normalize(v)                       \
    {                                             \
        double len = vector_length((v));          \
        (v)[0] /= len;                            \
        (v)[1] /= len;                            \
        (v)[2] /= len;                            \
    }

/**
 * @brief Gives a nearest power of 2 value which will be >= than input value.
 * @param[in] n The value.
 * @return Nearest power of 2 value.
 */
int ceil_pwrov2 (int n) GNUC_CONST;

#endif /* !_EMATH_H */
