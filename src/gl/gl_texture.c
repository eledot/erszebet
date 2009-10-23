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

static cvar_t *gl_trilinear;
static cvar_t *gl_anisotropy_level;
static cvar_t *gl_lod_bias;
static cvar_t *gl_picmip;

/* FIXME -- implement compressed textures & 3d textures support */

#define GL_MIN_TEXTURE_DIMENSION 64

#define GL_IMAGE_DATA2D(mip, im)                                        \
    {                                                                   \
        glTexImage2D(GL_TEXTURE_2D,                                     \
                     mip,                                               \
                     GL_RGBA,                                           \
                     im->width,                                         \
                     im->height,                                        \
                     0,                                                 \
                     GL_RGBA,                                           \
                     GL_UNSIGNED_BYTE,                                  \
                     im->data);                                         \
    }


/*
=================
gl_texture_create
=================
*/
bool gl_texture_create (image_t *image, int flags, int *gltex, int *texw, int *texh)
{
    int    max, sw, sh, mip;
    GLuint tex;

    if (NULL == image || NULL == gltex || NULL == texw || NULL == texh)
    {
        sys_printf("bad args (image=%p, flags=%i, gltex=%p, texw=%p, texh=%p)\n",
                   image, flags, gltex, texw, texh);
        return false;
    }

    if (flags & GL_TEX_FL_TEX3D)
    {
        max = gl_texture3d_size_max;
    }
    else if (flags & GL_TEX_FL_CUBEMAP)
    {
        max = gl_texture_cube_map_size_max;
    }
    else
    {
        max = gl_max_texture_size;
    }

    if (GL_TEX_FL_NOPICMIP)
    {
        sw = CLAMP(image->width,  1, max);
        sh = CLAMP(image->height, 1, max);
    }
    else
    {
        sw = CLAMP(image->width  >> gl_picmip->i, 1, max);
        sh = CLAMP(image->height >> gl_picmip->i, 1, max);
    }

    if (!ext_gl_arb_texture_non_power_of_two || !gl_arb_texture_non_power_of_two->i)
    {
        sw = ceil_pwrov2(sw);
        sh = ceil_pwrov2(sh);
    }

    sw = CLAMP(sw, GL_MIN_TEXTURE_DIMENSION, max);
    sh = CLAMP(sh, GL_MIN_TEXTURE_DIMENSION, max);

    if (flags & GL_TEX_FL_NOSCALE)
    {
        *texw = sw;
        *texh = sh;

        if (!image_resize(image, sw, sh))
            return false;
    }
    else
    {
        *texw = image->width;
        *texh = image->height;

        if (!image_scale(image, sw, sh))
            return false;
    }

    glGenTextures(1, &tex);
    GLERROR();
    eglBindTexture(GL_TEXTURE_2D, tex);
    GLERROR();

    *gltex = tex;

    if (NULL != image->teximage2d)
    {
        image->teximage2d(image);
    }
    else
    {
        if (ext_gl_sgis_generate_mipmap && gl_sgis_generate_mipmap->i)
        {
#ifdef ENGINE_OS_IPHONE
            glGenerateMipmapOES(GL_TEXTURE_2D);
#else
            glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
#endif
            GLERROR();
            GL_IMAGE_DATA2D(0, image);
            GLERROR();
        }
        else
        {
            GL_IMAGE_DATA2D(0, image);
            GLERROR();

            for (mip = 1; image->width > 1 || image->height > 1 ;mip++)
            {
                int status;

                if (0 > (status = image_mipmap(image)))
                {
                    sys_printf("mipmap failed\n");
                    goto error;
                }
                else if (status > 0)
                {
                    break;
                }

                GL_IMAGE_DATA2D(mip, image);
                GLERROR();
            }
        }
    }

    if (flags & GL_TEX_FL_NOFILTER)
    {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        GLERROR();
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        GLERROR();
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D,
                         GL_TEXTURE_MIN_FILTER,
                         gl_trilinear->i ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_NEAREST);
        GLERROR();
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GLERROR();
    }

    if (!(flags & GL_TEX_FL_NOANISO) &&
        ext_gl_ext_texture_filter_anisotropic &&
        gl_ext_texture_filter_anisotropic->i)
    {
        GLfloat ani = CLAMP(gl_anisotropy_level->f, 1, gl_anisotropy_max);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, ani);
        GLERROR();
    }

    if (!(flags & GL_TEX_FL_NOLOD) &&
        ext_gl_ext_texture_lod_bias &&
        gl_ext_texture_lod_bias->i)
    {
        GLfloat lod = CLAMP(gl_lod_bias->f, 0, gl_lod_bias_max);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS_EXT, lod);
        GLERROR();
    }

    return true;

error:
    glDeleteTextures(1, &tex);
    GLERROR();

    return false;
}

/*
=================
gl_texture_delete
=================
*/
void gl_texture_delete (int gltex)
{
    GLuint tex = gltex;

    glDeleteTextures(1, &tex);
}

/*
=================
gl_texture_init
=================
*/
bool gl_texture_init (void)
{
    gl_trilinear = cvar_get("gl_trilinear", "1", CVAR_FL_SAVE);
    gl_anisotropy_level = cvar_get("gl_anisotropy_level", "1", CVAR_FL_SAVE);
    gl_lod_bias = cvar_get("gl_lod_bias", "0", CVAR_FL_SAVE);
    gl_picmip = cvar_get("gl_picmip", "0", CVAR_FL_SAVE);

    cvar_set_min(gl_picmip, 0.0f);

    return true;
}

/*
=================
gl_texture_shutdown
=================
*/
void gl_texture_shutdown (void)
{
}
