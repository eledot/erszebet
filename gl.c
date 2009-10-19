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
#include "gl_private.h"
#include "r_texture.h"

static bool gl_i = false;

mem_pool_t mempool;

static const uint8_t internal_font_data[] =
{
#include "gl_internal_font.h"
};

static const char *extensions;
static GLdouble aspect;
static int internal_font;

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
bool gl_get_screen_rgb (image_t *image)
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
    eglColor4d(r, g, b, a);
    GLERROR();
}

/*
=================
gl_draw_stretched
=================
*/
void gl_draw_stretched (int gltex)
{
    eglBindTexture(GL_TEXTURE_2D, gltex);
    GLERROR();

#ifndef ENGINE_OS_IPHONE
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(0, 0);
    glTexCoord2f(1, 0); glVertex2f(video_width, 0);
    glTexCoord2f(1, 1); glVertex2f(video_width, video_height);
    glTexCoord2f(0, 1); glVertex2f(0, video_height);
    glEnd();
#else
    {
        GLfloat square[] = { 0, 0, video_width, 0, 0, video_height, video_width, video_height };
        GLfloat texcoords[] = { 0, 0, 1, 0, 0, 1, 1, 1 };
        glVertexPointer(2, GL_FLOAT, 0, square);
        glTexCoordPointer(2, GL_FLOAT, 0, texcoords);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
#endif
    GLERROR();
}

/*
=================
gl_draw_texture
=================
*/
void gl_draw_texture (int gltex,
                      double centerx,
                      double centery,
                      double width,
                      double height,
                      double angle)
{
    eglBindTexture(GL_TEXTURE_2D, gltex);
    GLERROR();

    glPushMatrix();
    GLERROR();
    glTranslatef(centerx, centery, 0);
    GLERROR();
    glRotatef(angle * 180.0 / M_PI, 0, 0, 1);
    GLERROR();

#ifndef ENGINE_OS_IPHONE
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-width/2, height/2);
    glTexCoord2f(1, 0); glVertex2f(width/2, height/2);
    glTexCoord2f(1, 1); glVertex2f(width/2, -height/2);
    glTexCoord2f(0, 1); glVertex2f(-width/2, -height/2);
    glEnd();
#else
    {
        GLfloat square[] = { -width/2, -height/2,
                             width/2, -height,
                             width/2, height/2,
                             -width/2, height/2 };
        GLfloat texcoords[] = { 0, 0, 1, 0, 0, 1, 1, 1 };
        glVertexPointer(2, GL_FLOAT, 0, square);
        glTexCoordPointer(2, GL_FLOAT, 0, texcoords);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
#endif
    GLERROR();

    glPopMatrix();
    GLERROR();
}

/*
=================
gl_draw_texture2
=================
*/
void gl_draw_texture2 (int gltex,
                       double centerx,
                       double centery,
                       double width,
                       double height,
                       double angle,
                       const float *vt)
{
    eglBindTexture(GL_TEXTURE_2D, gltex);
    GLERROR();

    glPushMatrix();
    GLERROR();
    glTranslatef(centerx, centery, 0);
    GLERROR();
    glRotatef(angle * 180.0 / M_PI, 0, 0, 1);
    GLERROR();

#ifndef ENGINE_OS_IPHONE
    glBegin(GL_QUADS);
    glTexCoord2f(vt[0], vt[1]); glVertex2f(-width/2, height/2);
    glTexCoord2f(vt[2], vt[3]); glVertex2f(width/2, height/2);
    glTexCoord2f(vt[4], vt[5]); glVertex2f(width/2, -height/2);
    glTexCoord2f(vt[6], vt[7]); glVertex2f(-width/2, -height/2);
    glEnd();
#else
    {
        GLfloat square[] = { -width/2, -height/2,
                             width/2, -height,
                             width/2, height/2,
                             -width/2, height/2 };
        GLfloat texcoords[] = { vt[0], vt[1], vt[2], vt[3], vt[6], vt[7], vt[4], vt[5] };
        glVertexPointer(2, GL_FLOAT, 0, square);
        glTexCoordPointer(2, GL_FLOAT, 0, texcoords);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
#endif
    GLERROR();

    glPopMatrix();
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
    GLERROR();
}

/*
=================
gl_draw_line2d_loop
=================
*/
void gl_draw_line2d_loop (const float *coords, int num)
{
    int i;

#ifndef ENGINE_OS_IPHONE
    glBegin(GL_LINE_LOOP);

    for(i = 0; i < num; i++)
        glVertex2f(coords[(i << 1) + 0], coords[(i << 1) + 1]);

    glEnd();
#else
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, coords);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_LINE_LOOP, 0, num);
#endif
    GLERROR();
}

/*
=================
gl_draw_line2d
=================
*/
void gl_draw_line2d (float x0, float y0, float x1, float y1)
{
#ifndef ENGINE_OS_IPHONE
    glBegin(GL_LINES);
    glVertex2f(x0, y0);
    glVertex2f(x1, y1);
    glEnd();
#else
    {
        GLfloat coords[] = { x0, y0, x1, y1 };
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(2, GL_FLOAT, 0, coords);
        glDrawArrays(GL_LINE_LOOP, 0, 2);
    }
#endif
    GLERROR();
}

/*
=================
gl_draw_text
=================
*/
void gl_draw_text (const char *text, float x, float y)
{
    float tx, ty, tx2, ty2;

    if (NULL == text)
    {
        sys_printf("NULL text\n");
        return;
    }

    if (!*text || internal_font < 0)
        return;

    eglBindTexture(GL_TEXTURE_2D, internal_font);
    GLERROR();

#ifndef ENGINE_OS_IPHONE
    glBegin(GL_QUADS);

    for (; *text && x < video_width ;text++, x += GL_INTERNAL_FONT_CW)
    {
        if (*text <= 32 || (*text & 128))
            continue;

        /* glyph size is 8x12 */
        tx = (*text % 16) / 16.0f;
        ty = (*text >> 4) / 8.0f;
        tx2 = tx + 1.0f/16.0f;
        ty2 = ty + 1.0f/8.0f - 1.0f/32.0f;

        glTexCoord2f(tx, ty);
        glVertex2f(x, y);

        glTexCoord2f(tx2, ty);
        glVertex2f(x + GL_INTERNAL_FONT_CW, y);

        glTexCoord2f(tx2, ty2);
        glVertex2f(x + GL_INTERNAL_FONT_CW, y - GL_INTERNAL_FONT_CH);

        glTexCoord2f(tx, ty2);
        glVertex2f(x, y - GL_INTERNAL_FONT_CH);
    }

    glEnd();
#else
    /* FIXME */
#endif

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
    gl_color(1, 1, 1, 1);
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

    gl_set_perspective((GLdouble)r_fov->f, aspect, 4, 999999.0);

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
gl_create_internal_font
=================
*/
static bool gl_create_internal_font (void)
{
    image_t im;
    int     i, k, pix;

    memset(&im, 0, sizeof(im));

    im.width  = 128;
    im.height = 128;

    pix = im.width * im.height;

    if (NULL == (im.data = mem_alloc_static(pix << 2)))
    {
        sys_printf("no memory for internal font\n");
        return false;
    }

    memset(im.data, 0xff, pix << 2);

    for (i = 0; i < (pix >> 3) ;i++)
    {
        for (k = 0; k < 8 ;k++)
        {
            if (!(internal_font_data[i] & (1 << k)))
                im.data[(i << 5) + (k << 2) + 3] = 0;
        }
    }

    if (!gl_texture_create(&im, GL_TEX_FL_UI, &internal_font))
        internal_font = -1;

    mem_free(im.data);

    return true;
}

/*
=================
gl_init
=================
*/
bool gl_init (void)
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

#ifdef ENGINE_OS_IPHONE
    glEnableClientState(GL_VERTEX_ARRAY);
#endif

    gl_set_viewport(0, 0, video_width, video_height);

    if (!gl_texture_init())
        return false;

    if (!gl_create_internal_font())
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

    gl_texture_delete(internal_font);
    gl_texture_shutdown();

    gl_i = false;

    sys_printf("-gl\n");
}
