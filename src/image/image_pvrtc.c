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

#ifdef ENGINE_IMAGE_PVRTC

#include "image/image_private.h"
#include "gl/gl_private.h"

typedef enum
{
    PVRTC_FORMAT_PVRTC2 = 0x0c,
    PVRTC_FORMAT_PVRTC4 = 0x0d
}pvrtc_pixel_formats_e;

typedef enum
{
    PVRTC_FL_MIPMAPS     = (1 <<  8),
    PVRTC_FL_INTERLEAVED = (1 <<  9),
    PVRTC_FL_NORMALMAP   = (1 << 10),
    PVRTC_FL_BORDER      = (1 << 11),
    PVRTC_FL_CUBEMAP     = (1 << 12),
    PVRTC_FL_FALSE_COLOR = (1 << 13),
    PVRTC_FL_VOLUME      = (1 << 14),
    PVRTC_FL_HAS_ALPHA   = (1 << 15)
}pvrtc_pixel_format_flags_e;

typedef struct pvrtc_header_s
{
    unsigned int header_size;
    unsigned int height;
    unsigned int width;
    unsigned int mipmaps_num;
    unsigned int pixel_format; /* both pixel format and flags */
    unsigned int data_size;
    unsigned int bpp;
    unsigned int mask_r;
    unsigned int mask_g;
    unsigned int mask_b;
    unsigned int mask_a;
    char         signature[4]; /* PVR format signature */
    unsigned int surfs_num; /* number of surfaces */
}pvrtc_header_t;

/*
=================
image_pvrtc_teximage2d
=================
*/
GNUC_NONNULL static void image_pvrtc_teximage2d (image_t *im)
{
    int            i, off, width, height;
    unsigned char *data;

    width = im->width;
    height = im->height;

    if (GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG == im->format ||
        GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG == im->format)
    {
        off = (width * height) >> 2;
    }
    else
    {
        off = (width * height) >> 1;
    }

    for (data = im->data, i = 0; i < im->miplevels ;i++)
    {
        glCompressedTexImage2D(GL_TEXTURE_2D,
                               i,
                               im->format,
                               width,
                               height,
                               0,
                               off,
                               data);

        if (GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG == im->format ||
            GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG == im->format)
        {
            data += ((width * height << 1) + 7) >> 3;
            off = (width * height) >> 2;
        }
        else
        {
            data += ((width * height << 2) + 7) >> 3;
            off = (width * height) >> 1;
        }

        width  > 1 ? width  >>= 1 : 1;
        height > 1 ? height >>= 1 : 1;
    }
}

/*
=================
image_pvrtc_load
=================
*/
GNUC_NONNULL static erbool image_pvrtc_load (const char *name, image_t *im)
{
    fs_file_t      f;
    int            size, format, mipmaps_num = 1, pf;
    unsigned char *data = NULL;
    pvrtc_header_t header;

    if (NULL == (f = fs_open(name, FS_RDONLY, &size, false)))
        return false;

    if (size <= (int)sizeof(header))
    {
        sys_printf("\"%s\" has invaid size for pvrtc image format\n", name);
        goto error;
    }

    fs_read(f, &header, sizeof(header));

    if (header.signature[0] != 'P' ||
        header.signature[1] != 'V' ||
        header.signature[2] != 'R' ||
        header.signature[3] != '!')
    {
        sys_printf("\"%s\" has bad signature\n", name);
        goto error;
    }

    if (header.width  > IMAGE_MAX_WIDTH  ||
        header.height > IMAGE_MAX_HEIGHT ||
        header.width  < 1 ||
        header.height < 1)
    {
        sys_printf("bad image size (%ux%u)\n", header.width, header.height);
        goto error;
    }

    if (header.data_size < 1)
    {
        sys_printf("\"%s\" has bad data size\n", name);
        goto error;
    }

    if (header.pixel_format & PVRTC_FL_MIPMAPS)
        mipmaps_num = header.mipmaps_num;

    pf = header.pixel_format;
    format = pf & 0xff;

    switch (format)
    {
    case PVRTC_FORMAT_PVRTC2:
        format = ((pf & PVRTC_FL_HAS_ALPHA)
                  ? GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG
                  : GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG);
        break;

    case PVRTC_FORMAT_PVRTC4:
        format = ((pf & PVRTC_FL_HAS_ALPHA)
                  ? GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG
                  : GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG);
        break;

    default:
        sys_printf("format 0x%x not supported\n", format);
        goto error;
    }

    data = mem_alloc(image_mempool, header.data_size);

    if ((int)header.data_size != fs_read(f, data, header.data_size))
    {
        sys_printf("failed to read %i bytes from \"%s\"\n", header.data_size, name);
        goto error;
    }

    im->width      = header.width;
    im->height     = header.height;
    im->data       = data;
    im->data_size  = header.data_size;
    im->format     = format;
    im->miplevels  = mipmaps_num;
    im->teximage2d = &image_pvrtc_teximage2d;

    fs_close(f);

    return true;

error:
    fs_close(f);

    if (NULL != data)
        mem_free(data);

    return false;
}

static const char * const image_pvrtc_extensions[] = { "pvr", "pvrtc", NULL };

const image_plugin_t image_plugin_pvrtc =
{
    .name = "image_pvrtc",
    .extensions = image_pvrtc_extensions,
    .load = image_pvrtc_load
};

#endif /* ENGINE_IMAGE_PVRTC */
