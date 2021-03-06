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

#ifndef _GL_H
#define _GL_H

#include "image/image.h"

void gl_clear (void);
erbool gl_get_screen_rgb (image_t *image) GNUC_NONNULL GNUC_WARN_UNUSED_RES;
void gl_color (double r, double g, double b, double a);
void gl_translate_rotate (double x, double y, double angle);
void gl_scale (double scalex, double scaley);
void gl_matrix_push (void);
void gl_matrix_pop (void);
void gl_draw_quad (int gltex,
                   const float *verts,
                   const float *texcoords) GNUC_NONNULL;
void gl_disable_textures (void);
void gl_enable_textures (void);
void gl_draw_line2d_loop (const float *coords, int num) GNUC_NONNULL;
void gl_draw_line2d (float x0, float y0, float x1, float y1);
void gl_set_viewport (int x, int y, int w, int h);
void gl_switch_2d (void);
void gl_switch_3d (void);

erbool gl_init (void);
void gl_shutdown (void);

#endif /* !_GL_H */
