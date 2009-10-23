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

#ifdef ENGINE_IMAGE_PNG

#include <png.h>
#include <setjmp.h>

#include "image/image_private.h"

static void epng_error (png_structp png_ptr, png_const_charp msg);

/*
=================
epng_read
=================
*/
static void epng_read (png_structp png_ptr, png_bytep data, png_size_t length)
{
    char tmp[64];

    if ((int)length != fs_read(png_get_io_ptr(png_ptr), data, length))
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
static void epng_error (png_structp png_ptr, PUV png_const_charp msg)
{
    sys_printf("libpng error: %s\n", msg);

    longjmp(png_jmpbuf(png_ptr), 1);
}

/*
=================
epng_warn
=================
*/
static void epng_warn (GNUC_UNUSED png_structp png_ptr, GNUC_UNUSED png_const_charp msg)
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
    if ((int)length != fs_write(png_get_io_ptr(png_ptr), data, length))
        epng_error(png_ptr, "failed to write %i bytes\n");
}

/*
=================
epng_flush
=================
*/
static void epng_flush (GNUC_UNUSED png_structp png_ptr)
{
    /* do nothing */
}

/*
=================
image_png_load
=================
*/
GNUC_NONNULL static bool image_png_load (const char *name, image_t *im)
{
    fs_file_t   f;
    int         size, r, width, height, inc;
    png_byte   *image = NULL, *p = NULL;
    png_structp pngst;
    png_infop   info = NULL;
    png_byte    depth, color_type;

    if (NULL == (f = fs_open(name, FS_RDONLY, &size, 0)))
        return false;

    if (NULL == (pngst = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                                NULL,
                                                &epng_error,
                                                &epng_warn)))
    {
        sys_printf("failed to create png read struct\n");
        fs_close(f);
        return false;
    }

    if (NULL == (info = png_create_info_struct(pngst)))
    {
        sys_printf("failed to create png info struct\n");
        goto error;
    }

    if (setjmp(png_jmpbuf(pngst)))
        goto error;

    png_set_user_limits(pngst, IMAGE_MAX_WIDTH, IMAGE_MAX_HEIGHT);
    png_set_read_fn(pngst, f, &epng_read);
    png_read_info(pngst, info);

    width  = png_get_image_width(pngst, info);
    height = png_get_image_height(pngst, info);
    depth  = png_get_bit_depth(pngst, info);

    if (16 == depth)
    {
        /* 16 -> 8 */
        png_set_strip_16(pngst);
        depth = 8;
    }

    color_type = png_get_color_type(pngst, info);

    /* 1/2/4 gray -> 8 gray */
    if (PNG_COLOR_TYPE_GRAY == color_type && depth < 8)
        png_set_expand_gray_1_2_4_to_8(pngst);

    /* gray/palette -> rgb */
    if (PNG_COLOR_TYPE_GRAY == color_type || PNG_COLOR_TYPE_GRAY_ALPHA == color_type)
        png_set_gray_to_rgb(pngst);
    else if (PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(pngst);

    /* transparency -> alpha */
    if (png_get_valid(pngst, info, PNG_INFO_tRNS))
    {
        png_set_tRNS_to_alpha(pngst);
    }
    else
    {
        /* to rgba */
        if (PNG_COLOR_TYPE_RGB_ALPHA != color_type &&
            PNG_COLOR_TYPE_GRAY_ALPHA != color_type)
            png_set_add_alpha(pngst, 0xff, PNG_FILLER_AFTER);
    }

    /* deinterlace */
    png_set_interlace_handling(pngst);

    /* read */
    inc = width * 4;

    p = image = mem_alloc(image_mempool, height * inc);

    for (r = 0; r < height ;r++, p += inc)
        png_read_row(pngst, p, NULL);

    png_read_end(pngst, NULL);
    png_destroy_read_struct(&pngst, &info, NULL);

    fs_close(f);

    im->width  = width;
    im->height = height;
    im->data   = image;

    return true;

error:
    if (NULL != f)
        fs_close(f);

    png_destroy_read_struct(&pngst, NULL != info ? &info : NULL, NULL);

    if (NULL != image)
        mem_free(image);

    return false;
}

/*
=================
image_png_save
=================
*/
GNUC_NONNULL static bool image_png_save (const char *name, const image_t *im)
{
    fs_file_t   f;
    png_structp pngst;
    png_infop   info = NULL;
    int         r, inc;

    if (NULL == (f = fs_open(name, FS_WRONLY, NULL, 0)))
        return false;

    if (NULL == (pngst = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                 f,
                                                 &epng_error,
                                                 &epng_warn)))
    {
        sys_printf("failed to create png write struct\n");
        fs_close(f);
        return false;
    }

    if (setjmp(png_jmpbuf(pngst)))
        goto error;

    if (NULL == (info = png_create_info_struct(pngst)))
    {
        sys_printf("failed to create png info struct\n");
        goto error;
    }

    png_set_write_fn(pngst, f, &epng_write, &epng_flush);

    /* rgb8 with default compression settings */
    png_set_IHDR(pngst,
                 info,
                 im->width,
                 im->height,
                 8,
                 PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);

    png_write_info(pngst, info);

    inc = im->width * 3;

    for (r = im->height - 1; r >= 0 ;r--)
    {
        png_write_row(pngst, im->data + r * inc);
    }

    png_write_end(pngst, info);
    png_destroy_write_struct(&pngst, &info);

    fs_close(f);

    return true;

error:
    if (NULL != f)
        fs_close(f);

    png_destroy_write_struct(&pngst, NULL != info ? &info : NULL);

    return false;
}

static const char * const image_png_extensions[] = { "png", NULL };

const image_plugin_t image_plugin_png =
{
    .name = "image_png",
    .extensions = image_png_extensions,
    .load = image_png_load,
    .save = image_png_save
};

#endif /* ENGINE_IMAGE_PNG */
