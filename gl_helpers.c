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

#include "gl_private.h"

static GLboolean depth_mask_flag;
static GLdouble color_r, color_g, color_b, color_a;
static GLboolean alpha_test, blend, cull_face, depth_test, fog;
static GLuint texture;

/*
=================
eglDepthMask
=================
*/
void eglDepthMask (GLboolean flag)
{
    if (depth_mask_flag == flag)
        return;

    depth_mask_flag = flag;
    glDepthMask(depth_mask_flag);
}

/*
=================
eglBindTexture
=================
*/
void eglBindTexture (GLenum target, GLuint tex)
{
    if (texture == tex)
        return;

    texture = tex;
    glBindTexture(target, tex);
}

/*
=================
eglColor4
=================
*/
void eglColor4 (GLdouble r, GLdouble g, GLdouble b, GLdouble a)
{
    if (color_r == r && color_g == g && color_b == b && color_a == a)
        return;

    color_r = r;
    color_g = g;
    color_b = b;
    color_a = a;

    glColor4d(color_r, color_g, color_b, color_a);
}

/*
=================
eglEnable
=================
*/
void eglEnable (GLenum cap)
{
    switch (cap)
    {
    case GL_ALPHA_TEST:
        if (alpha_test)
            return;
        alpha_test = GL_TRUE;
        break;

    case GL_BLEND:
        if (blend)
            return;
        blend = GL_TRUE;
        break;

    case GL_CULL_FACE:
        if (cull_face)
            return;
        cull_face = GL_TRUE;
        break;

    case GL_DEPTH_TEST:
        if (depth_test)
            return;
        depth_test = GL_TRUE;
        break;

    case GL_FOG:
        if (fog)
            return;
        fog = GL_TRUE;
        break;

    default:
        break;
    }

    glEnable(cap);
}

/*
=================
eglDisable
=================
*/
void eglDisable (GLenum cap)
{
    switch (cap)
    {
    case GL_ALPHA_TEST:
        if (!alpha_test)
            return;
        alpha_test = GL_FALSE;
        break;

    case GL_BLEND:
        if (!blend)
            return;
        blend = GL_FALSE;
        break;

    case GL_CULL_FACE:
        if (!cull_face)
            return;
        cull_face = GL_FALSE;
        break;

    case GL_DEPTH_TEST:
        if (!depth_test)
            return;
        depth_test = GL_FALSE;
        break;

    case GL_FOG:
        if (!fog)
            return;
        fog = GL_FALSE;
        break;

    default:
        break;
    }

    glDisable(cap);
}

/*
=================
gl_set_perspective
=================
*/
void gl_set_perspective (GLdouble fov, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
    static GLdouble _fov = 0, _aspect = 0;
    GLdouble        fovy;
    static GLdouble xmin = 0, xmax = 0, ymin = 0, ymax = 0;

    if (_fov != fov || _aspect != aspect)
    {
        _fov = fov;
        _aspect = aspect;

        fovy = atan(tan(fov / 360.0 * M_PI) / aspect) * 360.0 / M_PI;

        ymax = zNear * tan(fovy * 0.008726646259971);
        ymin = -ymax;

        xmin = ymin * aspect;
        xmax = ymax * aspect;
    }

    glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
    GLERROR();
}

/*
=================
gl_tex_env
=================
*/
void gl_tex_env (GLint param)
{
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, param);
    GLERROR();
}

/*
=================
gl_reset_helpers
=================
*/
void gl_reset_helpers (void)
{
    depth_mask_flag = GL_FALSE;
    glDepthMask(depth_mask_flag);
    GLERROR();

    color_r = color_g = color_b = color_a = 0;
    glColor4f(color_r, color_g, color_b, color_a);
    GLERROR();

    alpha_test = blend = cull_face = depth_test = fog = GL_FALSE;
    glDisable(GL_ALPHA_TEST);
    GLERROR();
    glDisable(GL_BLEND);
    GLERROR();
    glDisable(GL_CULL_FACE);
    GLERROR();
    glDisable(GL_DEPTH_TEST);
    GLERROR();
    glDisable(GL_FOG);
    GLERROR();
}
