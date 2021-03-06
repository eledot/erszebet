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

#ifdef ENGINE_IMAGE_CG

#include <CoreGraphics/CoreGraphics.h>

#include "image/image_private.h"

typedef CGImageRef (*im_prov_t) (CGDataProviderRef      source,
                                 const CGFloat          decode[],
                                 bool                   shouldInterpolate,
                                 CGColorRenderingIntent intent);

static const im_prov_t providers[] =
{
    CGImageCreateWithPNGDataProvider,
    CGImageCreateWithJPEGDataProvider
};

/*
=================
image_cg_load
=================
*/
GNUC_NONNULL static erbool image_cg_load (const char *name, image_t *im)
{
    CGDataProviderRef provider = NULL;
    CGImageRef        image = NULL;
    CGColorSpaceRef   color_space = NULL;
    CGContextRef      context = NULL;
    int               width, height, prov;
    unsigned char    *data = NULL;
    erbool            error = true;

    if (NULL == (provider = fs_get_data_provider(name)))
    {
        sys_printf("failed to get data provider\n");
        goto error;
    }

    for (prov = 0; prov < STSIZE(providers) ;prov++)
    {
        if (NULL != (image = providers[prov](provider, NULL, true, kCGRenderingIntentDefault)))
            break;
    }

    if (NULL == image)
    {
        sys_printf("no provider supporting \"%s\"\n", name);
        goto error;
    }

    width  = CGImageGetWidth(image);
    height = CGImageGetHeight(image);

    if (width > IMAGE_MAX_WIDTH || height > IMAGE_MAX_HEIGHT || width < 1 || height < 1)
    {
        sys_printf("bad image size (%ix%i)\n", width, height);
        goto error;
    }

    data = mem_alloc(image_mempool, height * width * 4);
    color_space = CGImageGetColorSpace(image);

    if (NULL == color_space)
    {
        sys_printf("failed to get color space\n");
        goto error;
    }

    if (NULL == (context = CGBitmapContextCreate(data,
                                                 width,
                                                 height,
                                                 8,
                                                 width << 2,
                                                 color_space,
                                                 kCGImageAlphaPremultipliedLast)))
    {
        sys_printf("failed to create bitmap context\n");
        goto error;
    }

    CGContextDrawImage(context, CGRectMake(0, 0, width, height), image);

    im->width  = width;
    im->height = height;
    im->data   = data;

    sys_printf("loaded \"%s\"\n", name);

    error = false;

error:
    if (NULL != context)
        CGContextRelease(context);

    if (NULL != color_space)
        CFRelease(color_space);

    if (NULL != image)
        CGImageRelease(image);

    if (NULL != provider)
        CGDataProviderRelease(provider);

    if (NULL != data && error)
        mem_free(data);

    return !error;
}

/* FIXME -- png with premultipled alpha (fix by hand?) */
static const char * const image_cg_extensions[] = { "png", "jpg", "jpeg", NULL };

const image_plugin_t image_plugin_cg =
{
    .name = "image_cg",
    .extensions = image_cg_extensions,
    .load = image_cg_load
};

#endif /* ENGINE_IMAGE_CG */
