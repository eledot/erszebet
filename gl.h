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

#include "image.h"

#define GL_INTERNAL_FONT_CW 8.0f
#define GL_INTERNAL_FONT_CH 12.0f

void gl_clear (void);
int gl_get_screen_rgb (image_t *image) GNUC_NONNULL;
void gl_color (float r, float g, float b, float a);
void gl_draw_stretched (int gltex);
void gl_draw_texture (int gltex,
                      double centerx,
                      double centery,
                      double width,
                      double height,
                      double angle);
void gl_draw_texture2 (int gltex,
                       double centerx,
                       double centery,
                       double width,
                       double height,
                       double angle,
                       const float *vt) GNUC_NONNULL;
void gl_disable_textures (void);
void gl_enable_textures (void);
void gl_draw_line2d_loop (const float *coords, int num) GNUC_NONNULL;
void gl_draw_line2d (float x0, float y0, float x1, float y1);
void gl_draw_text (const char *text, float x, float y) GNUC_NONNULL;
void gl_set_viewport (int x, int y, int w, int h);
void gl_switch_2d (void);
void gl_switch_3d (void);

int gl_init (void);
void gl_shutdown (void);

#endif /* !_GL_H */
