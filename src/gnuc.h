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

#ifndef _GNUC_H
#define _GNUC_H

#if __GNUC__ > 2
#define GNUC_MALLOC __attribute__((malloc))
#define GNUC_PURE __attribute__((pure))
#define GNUC_CONST __attribute__((const))
#define GNUC_UNUSED __attribute__((unused))
#define GNUC_NONNULL __attribute__((nonnull))
#define GNUC_WARN_UNUSED_RES __attribute__((warn_unused_result))
#define GNUC_NONNULL_ARGS(a...) __attribute__((nonnull(a)))
#define GNUC_PRINTF(f, p) __attribute__((format(printf, f, p)))
#else
#define GNUC_MALLOC
#define GNUC_PURE
#define GNUC_CONST
#define GNUC_UNUSED
#define GNUC_NONNULL
#define GNUC_WARN_UNUSED_RES
#define GNUC_NONNULL_ARGS(a...)
#define GNUC_PRINTF(format, parms)
#endif

#endif /* !_GNUC_H */
