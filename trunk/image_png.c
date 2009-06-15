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

#ifdef ENGINE_IMAGE_PNG

#include <png.h>
#include <setjmp.h>

#include "common.h"
#include "image_png_funcs.h"

static int image_png_i = 0;

static lib_t libpng;

static void epng_error (png_structp png_ptr, png_const_charp msg);

/*
=================
epng_read
=================
*/
static void epng_read (png_structp png_ptr, png_bytep data, png_size_t length)
{
    char tmp[64];

    if ((int)length != fs_read(epng_get_io_ptr(png_ptr), data, length))
    {
        snprintf(tmp, sizeof(tmp), "failed to read %i bytes\n", length);
        epng_error(png_ptr, tmp);
    }
}

/*
=================
epng_error
=================
*/
static void epng_error (png_structp png_ptr, png_const_charp msg)
{
    sys_printf("libpng error: %s\n", msg);

    longjmp(png_jmpbuf(png_ptr), 1);
}

/*
=================
epng_warn
=================
*/
static void epng_warn (png_structp png_ptr, png_const_charp msg)
{
    /* shut up warnings */
}

/*
=================
epng_write
=================
*/
static void epng_write (png_structp png_ptr, png_bytep data, png_size_t length)
{
    if ((int)length != fs_write(epng_get_io_ptr(png_ptr), data, length))
        epng_error(png_ptr, "failed to write %i bytes\n");
}

/*
=================
epng_flush
=================
*/
static void epng_flush (png_structp png_ptr)
{
    /* do nothing */
}

/*
=================
image_png_load
=================
*/
int image_png_load (const char *name, image_t *im, mem_pool_t pool)
{
    fs_file_t   f;
    int         size, r, width, height, inc;
    png_byte   *image = NULL, *p = NULL;
    png_structp pngst;
    png_infop   info = NULL;
    png_byte    depth, color_type;

    if (!image_png_i)
        return -1;

    if (NULL == (f = fs_open(name, FS_RDONLY, &size, 0)))
        return -2;

    if (NULL == (pngst = epng_create_read_struct(PNG_LIBPNG_VER_STRING,
                                                 NULL,
                                                 &epng_error,
                                                 &epng_warn)))
    {
        sys_printf("failed to create png read struct\n");
        fs_close(f);
        return -3;
    }

    if (NULL == (info = epng_create_info_struct(pngst)))
    {
        sys_printf("failed to create png info struct\n");
        goto error;
    }

    if (setjmp(png_jmpbuf(pngst)))
        goto error;

    epng_set_user_limits(pngst, IMAGE_MAX_WIDTH, IMAGE_MAX_HEIGHT);
    epng_set_read_fn(pngst, f, &epng_read);
    epng_read_info(pngst, info);

    width  = epng_get_image_width(pngst, info);
    height = epng_get_image_height(pngst, info);
    depth  = epng_get_bit_depth(pngst, info);

    if (16 == depth)
    {
        /* 16 -> 8 */
        epng_set_strip_16(pngst);
        depth = 8;
    }

    color_type = epng_get_color_type(pngst, info);

    /* 1/2/4 gray -> 8 gray */
    if (PNG_COLOR_TYPE_GRAY == color_type && depth < 8)
        epng_set_expand_gray_1_2_4_to_8(pngst);

    /* gray/palette -> rgb */
    if (PNG_COLOR_TYPE_GRAY == color_type || PNG_COLOR_TYPE_GRAY_ALPHA == color_type)
        epng_set_gray_to_rgb(pngst);
    else if (PNG_COLOR_TYPE_PALETTE)
        epng_set_palette_to_rgb(pngst);

    /* transparency -> alpha */
    if (epng_get_valid(pngst, info, PNG_INFO_tRNS))
    {
        epng_set_tRNS_to_alpha(pngst);
    }
    else
    {
        /* to rgba */
        if (PNG_COLOR_TYPE_RGB_ALPHA != color_type &&
            PNG_COLOR_TYPE_GRAY_ALPHA != color_type)
            epng_set_add_alpha(pngst, 0xff, PNG_FILLER_AFTER);
    }

    /* deinterlace */
    epng_set_interlace_handling(pngst);

    /* read */
    inc = width * 4;

    p = image = mem_alloc(pool, height * inc);

    for (r = 0; r < height ;r++, p += inc)
        epng_read_row(pngst, p, NULL);

    epng_read_end(pngst, NULL);
    epng_destroy_read_struct(&pngst, &info, NULL);

    fs_close(f);

    im->width  = width;
    im->height = height;
    im->data   = image;

    return 0;

error:
    if (NULL != f)
        fs_close(f);

    epng_destroy_read_struct(&pngst, NULL != info ? &info : NULL, NULL);

    if (NULL != image)
        mem_free(image);

    return -4;
}

/*
=================
image_png_save
=================
*/
int image_png_save (const char *name, image_t *im)
{
    fs_file_t   f;
    png_structp pngst;
    png_infop   info = NULL;
    int         r, inc;

    if (!image_png_i)
        return -1;

    if (NULL == (f = fs_open(name, FS_WRONLY, NULL, 0)))
        return -2;

    if (NULL == (pngst = epng_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                  f,
                                                  &epng_error,
                                                  &epng_warn)))
    {
        sys_printf("failed to create png write struct\n");
        fs_close(f);
        return -3;
    }

    if (setjmp(png_jmpbuf(pngst)))
        goto error;

    if (NULL == (info = epng_create_info_struct(pngst)))
    {
        sys_printf("failed to create png info struct\n");
        goto error;
    }

    epng_set_write_fn(pngst, f, &epng_write, &epng_flush);

    /* rgb8 with default compression settings */
    epng_set_IHDR(pngst,
                  info,
                  im->width,
                  im->height,
                  8,
                  PNG_COLOR_TYPE_RGB,
                  PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_DEFAULT,
                  PNG_FILTER_TYPE_DEFAULT);

    epng_write_info(pngst, info);

    inc = im->width * 3;

    for (r = im->height - 1; r >= 0 ;r--)
    {
        epng_write_row(pngst, im->data + r * inc);
    }

    epng_write_end(pngst, info);
    epng_destroy_write_struct(&pngst, &info);

    fs_close(f);

    return 0;

error:
    if (NULL != f)
        fs_close(f);

    epng_destroy_write_struct(&pngst, NULL != info ? &info : NULL);

    return -4;
}

/*
=================
image_png_init
=================
*/
int image_png_init (void)
{
    const char *names[] =
    {
        "libpng12.so.0",
        "libpng12.so",
        "libpng.so",
        NULL
    };

    if (sys_arg_find("-nolibpng"))
        return 0;

    if (LIB_HANDLE_INVALID == (libpng = lib_open(names, funcs, 0)))
    {
        sys_printf("png support disabled\n");
        return -1;
    }

    image_png_i = 1;

    sys_printf("+image_png\n");

    return 0;
}

/*
=================
image_png_shutdown
=================
*/
void image_png_shutdown (void)
{
    if (!image_png_i)
        return;

    lib_close(libpng);

    image_png_i = 0;

    sys_printf("-image_png\n");
}

#else /* !ENGINE_IMAGE_PNG */

#include "common.h"

int image_png_load (const char *name UV, image_t *im UV, mem_pool_t pool UV) { return -1; }
int image_png_save (const char *name UV, image_t *im UV) { sys_printf("png support was not compiled in\n"); return -1; }

int image_png_init (void) { return -1; }
void image_png_shutdown (void) { }

#endif /* ENGINE_IMAGE_PNG */
