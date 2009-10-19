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

#ifndef _R_FIGURE_H
#define _R_FIGURE_H

bool r_figure_set_circle (const char *name,
                          double radius,
                          double width,
                          void **data) GNUC_NONNULL GNUC_WARN_UNUSED_RES;

bool r_figure_set_line (const char *name,
                        const double *end,
                        double width,
                        void **data) GNUC_NONNULL GNUC_WARN_UNUSED_RES;

void r_figure_unset (void *data) GNUC_NONNULL;

void r_figure_draw (const void *data,
                    const double *origin,
                    const double *color,
                    double alpha,
                    double scale) GNUC_NONNULL;

#endif /* !_R_FIGURE_H */
