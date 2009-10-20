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

#ifndef _GL_HELPERS_H
#define _GL_HELPERS_H

void eglDepthMask (GLboolean flag);
void eglBindTexture (GLenum target, GLuint tex);
void eglColor4 (GLdouble r, GLdouble g, GLdouble b, GLdouble a);
void eglEnable (GLenum cap);
void eglDisable (GLenum cap);

void gl_set_perspective (GLdouble fov, GLdouble aspect, GLdouble zNear, GLdouble zFar);
void gl_tex_env (GLint param);

void gl_reset_helpers (void);

#endif /* !_GL_HELPERS_H */
