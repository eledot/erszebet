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

#ifndef _GL_TEXTURE_H
#define _GL_TEXTURE_H

typedef enum
{
    GL_TEX_FL_NOPICMIP   = (1 << 0),
    GL_TEX_FL_NOCOMPRESS = (1 << 1),
    GL_TEX_FL_NOANISO    = (1 << 2),
    GL_TEX_FL_NOFILTER   = (1 << 3),
    GL_TEX_FL_NOLOD      = (1 << 4),
    GL_TEX_FL_TEX3D      = (1 << 5),
    GL_TEX_FL_CUBEMAP    = (1 << 6),
    GL_TEX_FL_NOSCALE    = (1 << 7)
}gl_tex_create_flags_e;

#define GL_TEX_FL_UI (GL_TEX_FL_NOPICMIP |                              \
                      GL_TEX_FL_NOFILTER |                              \
                      GL_TEX_FL_NOCOMPRESS |                            \
                      GL_TEX_FL_NOANISO |                               \
                      GL_TEX_FL_NOLOD |                                 \
                      GL_TEX_FL_NOSCALE)

erbool gl_texture_create (image_t *image, int flags, int *gltex, int *texw, int *texh) GNUC_NONNULL GNUC_WARN_UNUSED_RES;
void gl_texture_delete (int gltex);

erbool gl_texture_init (void);
void gl_texture_shutdown (void);

#endif /* !_GL_TEXTURE_H */
