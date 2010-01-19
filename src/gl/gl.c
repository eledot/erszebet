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

#define GLPRIVATE
#include "gl/gl_private.h"
#include "render/r_texture.h"

static erbool gl_i = false;

mem_pool_t mempool;

static const char *extensions;
static GLdouble aspect;

#ifdef ENGINE_GL_DEBUG

int gl_error;

/*
=================
gl_check_error
=================
*/
void gl_check_error (const char *func, const char *file, int line)
{
    const char *msg;
    int         err;

    gl_error = 0;
    errno = 0;

    err = glGetError();

    if (GL_NO_ERROR != err)
    {
        switch (err)
        {
        case GL_INVALID_ENUM:      msg = "invalid enum";      break;
        case GL_INVALID_VALUE:     msg = "invalid value";     break;
        case GL_INVALID_OPERATION: msg = "invalid operation"; break;
        case GL_STACK_OVERFLOW:    msg = "stack overflow";    break;
        case GL_STACK_UNDERFLOW:   msg = "stack underflow";   break;
        case GL_OUT_OF_MEMORY:     msg = "out of memory";     break;
        default:                   msg = "unknown";           break;
        }

        gl_error = err;

        sys_printf("\"%s\" gl error in %s (%s:%i)\n", msg, func, file, line);
    }
}

#endif /* ENGINE_GL_DEBUG */

/*
=================
gl_clear
=================
*/
void gl_clear (void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLERROR();
}

/*
=================
gl_get_screen_rgb
=================
*/
erbool gl_get_screen_rgb (image_t *image)
{
    int size;

    if (NULL == image)
    {
        sys_printf("NULL image\n");
        return false;
    }

    size = video_width * video_height * 3;

    if (NULL == (image->data = mem_alloc(mempool, size)))
    {
        sys_printf("failed to allocate %i bytes\n", size);
        return false;
    }

    image->width = video_width;
    image->height = video_height;

    glReadPixels(0, 0, video_width, video_height, GL_RGB, GL_UNSIGNED_BYTE, image->data);
    GLERROR();

    return true;
}

/*
=================
gl_color
=================
*/
void gl_color (double r, double g, double b, double a)
{
    eglColor4(r, g, b, a);
    GLERROR();
}

/*
=================
gl_translate_rotate
=================
*/
void gl_translate_rotate (double x, double y, double angle)
{
    glTranslatef(x, y, 0);
    GLERROR();
    glRotatef(angle * 180.0 / M_PI, 0, 0, 1);
    GLERROR();
}

/*
=================
gl_scale
=================
*/
void gl_scale (double scalex, double scaley)
{
    glScalef(scalex, scaley, 0);
    GLERROR();
}

/*
=================
gl_matrix_push
=================
*/
void gl_matrix_push (void)
{
    glPushMatrix();
    GLERROR();
}

/*
=================
gl_matrix_pop
=================
*/
void gl_matrix_pop (void)
{
    glPopMatrix();
    GLERROR();
}

/*
=================
gl_draw_quad
=================
*/
void gl_draw_quad (int gltex,
                   const float *verts,
                   const float *texcoords)
{
    eglBindTexture(GL_TEXTURE_2D, gltex);
    GLERROR();

    glVertexPointer(2, GL_FLOAT, 0, verts);
    glTexCoordPointer(2, GL_FLOAT, 0, texcoords);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    GLERROR();
}

/*
=================
gl_disable_textures
=================
*/
void gl_disable_textures (void)
{
    eglDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    GLERROR();
}

/*
=================
gl_enable_textures
=================
*/
void gl_enable_textures (void)
{
    eglEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    GLERROR();
}

/*
=================
gl_draw_line2d_loop
=================
*/
void gl_draw_line2d_loop (const float *coords, int num)
{
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, coords);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_LINE_LOOP, 0, num);
    GLERROR();
}

/*
=================
gl_draw_line2d
=================
*/
void gl_draw_line2d (float x0, float y0, float x1, float y1)
{
    GLfloat coords[] = { x0, y0, x1, y1 };
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, coords);
    glDrawArrays(GL_LINE_LOOP, 0, 2);
    GLERROR();
}

/*
=================
gl_set_viewport
=================
*/
void gl_set_viewport (int x, int y, int w, int h)
{
    if (!gl_i)
        return;

    gl_clear();
    glViewport(x, y, w, h);
    aspect = (GLdouble)(w - x) / (GLdouble)(h - y);
    GLERROR();
}

/*
=================
gl_switch_2d
=================
*/
void gl_switch_2d (void)
{
    glMatrixMode(GL_PROJECTION);
    GLERROR();
    glLoadIdentity();
    GLERROR();
#ifndef ENGINE_OS_IPHONE
    glOrtho(0.0f, video_width, 0.0f, video_height, -1.0f, 1.0f);
#else
    glOrthof(video_width - 1.0f, -1.0f, video_height, 0.0f, -1.0f, 1.0f);
#endif
    GLERROR();
    glMatrixMode(GL_MODELVIEW);
    GLERROR();
    glLoadIdentity();
    GLERROR();

#ifdef ENGINE_OS_IPHONE
    glRotatef(90, 0, 0, 1);
    glTranslatef(0, - video_width + 1.0f, 0);
#endif

    eglDepthMask(GL_FALSE);
    GLERROR();
    eglDisable(GL_DEPTH_TEST);
    GLERROR();
    eglEnable(GL_ALPHA_TEST);
    GLERROR();
    eglDisable(GL_CULL_FACE);
    GLERROR();
    eglEnable(GL_BLEND);
    GLERROR();
    gl_tex_env(GL_MODULATE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GLERROR();
    gl_color(1.0f, 1.0f, 1.0f, 1.0f);
}

/*
=================
gl_switch_3d
=================
*/
void gl_switch_3d (void)
{
    glMatrixMode(GL_PROJECTION);
    GLERROR();
    glLoadIdentity();
    GLERROR();

    gl_set_perspective(r_fov->f, aspect, 4, 999999.0);

    glCullFace(GL_FRONT);
    GLERROR();
    glMatrixMode(GL_MODELVIEW);
    GLERROR();
    glLoadIdentity();
    GLERROR();

    eglDepthMask(GL_FALSE);
    GLERROR();
    eglDisable(GL_BLEND);
    GLERROR();
    eglDisable(GL_ALPHA_TEST);
    GLERROR();
    eglEnable(GL_DEPTH_TEST);
    GLERROR();
    eglEnable(GL_CULL_FACE);
    GLERROR();

/*
  glRotatef(-90, 1, 0, 0);
  GLERROR();
  glRotatef( 90, 0, 0, 1);
  GLERROR();
  glRotatef(-vid.viewangles[2],  1, 0, 0);
  GLERROR();
  glRotatef(-vid.viewangles[0],  0, 1, 0);
  GLERROR();
  glRotatef(-vid.viewangles[1],  0, 0, 1);
  GLERROR();
  glTranslatef(-vid.vieworg[0],  -vid.vieworg[1], -vid.vieworg[2]);
  GLERROR();
*/
}

/*
=================
gl_init
=================
*/
erbool gl_init (void)
{
    const char *s;

    mem_alloc_static_pool("gl", 0);

    gl_i = true;

    s = (void *)glGetString(GL_VENDOR);
    GLERROR();
    sys_printf("opengl vendor: %s\n", NULL != s ? s : "undefined");
    s = (void *)glGetString(GL_VERSION);
    GLERROR();
    sys_printf("opengl version: %s\n", NULL != s ? s : "undefined");
    s = (void *)glGetString(GL_RENDERER);
    GLERROR();
    sys_printf("opengl renderer: %s\n", NULL != s ? s : "undefined");

    extensions = (void *)glGetString(GL_EXTENSIONS);
    GLERROR();

    sys_printf("opengl extensions: %s\n", extensions);

    gl_reset_helpers();
    gl_init_extensions(extensions);

    glDepthFunc(GL_LEQUAL);
    GLERROR();
    glDepthRange(0, 1);
    GLERROR();

    gl_tex_env(GL_REPLACE);
    eglEnable(GL_TEXTURE_2D);
    GLERROR();
    glAlphaFunc(GL_GREATER, 0);
    GLERROR();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GLERROR();

    /* nicest */
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    GLERROR();
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    GLERROR();
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    GLERROR();
#ifdef GL_POLYGON_SMOOTH_HINT
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    GLERROR();
#endif
    glHint(GL_FOG_HINT, GL_NICEST);
    GLERROR();

    eglEnable(GL_DEPTH_TEST);
    GLERROR();
    eglEnable(GL_ALPHA_TEST);
    GLERROR();
    eglEnable(GL_CULL_FACE);
    GLERROR();

    glLineWidth(1.0f);
    GLERROR();
    glPointSize(1.0f);
    GLERROR();

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    GLERROR();
    glClearDepth(1.0f);
    GLERROR();

    glEnableClientState(GL_VERTEX_ARRAY);

    gl_set_viewport(0, 0, video_width, video_height);

    if (!gl_texture_init())
        return false;

    sys_printf("+gl\n");

    return true;
}

/*
=================
gl_shutdown
=================
*/
void gl_shutdown (void)
{
    if (!gl_i)
        return;

    gl_texture_shutdown();

    gl_i = false;

    sys_printf("-gl\n");
}
