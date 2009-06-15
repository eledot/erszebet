/*
   Copyright (C) 2009 Serge "ftrvxmtrx" Ziryukin

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
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

static int gl_i = 0;

static lib_t libgl;
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

    err = eglGetError();

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
    eglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLERROR();
}

/*
=================
gl_get_screen_rgb
=================
*/
int gl_get_screen_rgb (image_t *image)
{
    int size;

    if (NULL == image)
    {
        sys_printf("NULL image\n");
        return -1;
    }

    size = video_width * video_height * 3;

    if (NULL == (image->data = mem_alloc(mempool, size)))
    {
        sys_printf("failed to allocate %i bytes\n", size);
        return -3;
    }

    image->width = video_width;
    image->height = video_height;

    eglReadPixels(0, 0, video_width, video_height, GL_RGB, GL_UNSIGNED_BYTE, image->data);
    GLERROR();

    return 0;
}

/*
=================
gl_color
=================
*/
void gl_color (float r, float g, float b, float a)
{
    eglColor4f(r, g, b, a);
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

    eglBegin(GL_QUADS);
    eglTexCoord2f(0, 0); eglVertex2f(0, 0);
    eglTexCoord2f(1, 0); eglVertex2f(video_width, 0);
    eglTexCoord2f(1, 1); eglVertex2f(video_width, video_height);
    eglTexCoord2f(0, 1); eglVertex2f(0, video_height);
    eglEnd();
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

    eglBegin(GL_LINE_LOOP);

    for(i = 0; i < num; i++)
        eglVertex2f(coords[(i << 1) + 0], coords[(i << 1) + 1]);

    eglEnd();
    GLERROR();
}

/*
=================
gl_draw_line2d
=================
*/
void gl_draw_line2d (float x0, float y0, float x1, float y1)
{
    eglBegin(GL_LINES);
    eglVertex2f(x0, y0);
    eglVertex2f(x1, y1);
    eglEnd();
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

    if (!*text)
        return;

    eglBindTexture(GL_TEXTURE_2D, internal_font);
    GLERROR();

    eglBegin(GL_QUADS);

    for (; *text && x < video_width ;text++, x += GL_INTERNAL_FONT_CW)
    {
        if (*text <= 32 || (*text & 128))
            continue;

        /* glyph size is 8x12 */
        tx = (*text % 16) / 16.0f;
        ty = (*text >> 4) / 8.0f;
        tx2 = tx + 1.0f/16.0f;
        ty2 = ty + 1.0f/8.0f - 1.0f/32.0f;

        eglTexCoord2f(tx, ty);
        eglVertex2f(x, y);

        eglTexCoord2f(tx2, ty);
        eglVertex2f(x + GL_INTERNAL_FONT_CW, y);

        eglTexCoord2f(tx2, ty2);
        eglVertex2f(x + GL_INTERNAL_FONT_CW, y - GL_INTERNAL_FONT_CH);

        eglTexCoord2f(tx, ty2);
        eglVertex2f(x, y - GL_INTERNAL_FONT_CH);
    }

    eglEnd();
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
    eglViewport(x, y, w, h);
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
    eglMatrixMode(GL_PROJECTION);
    GLERROR();
    eglLoadIdentity();
    GLERROR();
    eglOrtho(0, video_width, 0, video_height, -1, 1);
    GLERROR();
    eglMatrixMode(GL_MODELVIEW);
    GLERROR();
    eglLoadIdentity();
    GLERROR();

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
    gl_tex_env(GL_REPLACE);
    eglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
    eglMatrixMode(GL_PROJECTION);
    GLERROR();
    eglLoadIdentity();
    GLERROR();

    gl_set_perspective((GLdouble)r_fov->f, aspect, 4, 999999.0);

    eglCullFace(GL_FRONT);
    GLERROR();
    eglMatrixMode(GL_MODELVIEW);
    GLERROR();
    eglLoadIdentity();
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
  eglRotatef(-90, 1, 0, 0);
  GLERROR();
  eglRotatef( 90, 0, 0, 1);
  GLERROR();
  eglRotatef(-vid.viewangles[2],  1, 0, 0);
  GLERROR();
  eglRotatef(-vid.viewangles[0],  0, 1, 0);
  GLERROR();
  eglRotatef(-vid.viewangles[1],  0, 0, 1);
  GLERROR();
  eglTranslatef(-vid.vieworg[0],  -vid.vieworg[1], -vid.vieworg[2]);
  GLERROR();
*/
}

/*
=================
gl_create_internal_font
=================
*/
static int gl_create_internal_font (void)
{
    image_t im;
    int     i, k, pix;

    im.width  = 128;
    im.height = 128;

    pix = im.width * im.height;

    if (NULL == (im.data = mem_alloc_static(pix << 2)))
    {
        sys_printf("no memory for internal font\n");
        return -1;
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

    gl_texture_create(&im, GL_TEX_FL_UI, &internal_font);

    mem_free(im.data);

    return 0;
}

/*
=================
gl_init
=================
*/
int gl_init (void)
{
    const char *s;
    const char *names[] =
    {
        "libGL.so",
        NULL
    };

    if (LIB_HANDLE_INVALID == (libgl = lib_open(names, gl_funcs, 0)))
        return -1;

    mem_alloc_static_pool("gl", 0);

    gl_i = 1;

    s = (void *)eglGetString(GL_VENDOR);
    GLERROR();
    sys_printf("opengl vendor: %s\n", NULL != s ? s : "undefined");
    s = (void *)eglGetString(GL_VERSION);
    GLERROR();
    sys_printf("opengl version: %s\n", NULL != s ? s : "undefined");
    s = (void *)eglGetString(GL_RENDERER);
    GLERROR();
    sys_printf("opengl renderer: %s\n", NULL != s ? s : "undefined");

    extensions = (void *)eglGetString(GL_EXTENSIONS);
    GLERROR();

    sys_printf("opengl extensions: %s\n", extensions);

    gl_reset_helpers();
    gl_init_extensions(libgl, extensions);

    eglDepthFunc(GL_LEQUAL);
    GLERROR();
    eglDepthRange(0, 1);
    GLERROR();

    gl_tex_env(GL_REPLACE);
    eglEnable(GL_TEXTURE_2D);
    GLERROR();
    eglAlphaFunc(GL_GREATER, 0);
    GLERROR();
    eglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GLERROR();

    /* nicest */
    eglHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    GLERROR();
    eglHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    GLERROR();
    eglHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    GLERROR();
    eglHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    GLERROR();
    eglHint(GL_FOG_HINT, GL_NICEST);
    GLERROR();

    eglEnable(GL_DEPTH_TEST);
    GLERROR();
    eglEnable(GL_ALPHA_TEST);
    GLERROR();
    eglEnable(GL_CULL_FACE);
    GLERROR();

    eglLineWidth(1.0f);
    GLERROR();
    eglPointSize(1.0f);
    GLERROR();

    eglClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    GLERROR();
    eglClearDepth(1.0f);
    GLERROR();

    gl_set_viewport(0, 0, video_width, video_height);

    if (0 != gl_texture_init())
        return -2;

    if (0 != gl_create_internal_font())
        return -3;

    sys_printf("+gl\n");

    return 0;
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

    lib_close(libgl);

    gl_i = 0;

    sys_printf("-gl\n");
}
