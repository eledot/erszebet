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

#ifndef _GL_PRIVATE_H
#define _GL_PRIVATE_H

#include <GL/gl.h>
#include <GL/glext.h>

#include "common.h"
#include "gl_ext.h"
#include "gl_ext_funcs.h"
#include "gl_helpers.h"
#include "gl_texture.h"

extern mem_pool_t mempool;

#ifdef ENGINE_GL_DEBUG
extern int gl_error;
void gl_check_error (const char *func, const char *file, int line);
#define GLERROR() gl_check_error(__FUNCTION__, __FILE__, __LINE__)
#else
#define GLERROR()
#define gl_error glGetError()
#endif

#endif /* !_GL_PRIVATE_H */