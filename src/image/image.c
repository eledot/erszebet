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

#include "image/image_private.h"

static bool image_i = false;

mem_pool_t image_mempool;

extern const image_plugin_t image_plugin_png;
extern const image_plugin_t image_plugin_jpeg;
extern const image_plugin_t image_plugin_cg;
extern const image_plugin_t image_plugin_pvrtc;

static const image_plugin_t * const image_plugins[] =
{
#ifdef ENGINE_IMAGE_PNG
    &image_plugin_png,
#endif
#ifdef ENGINE_IMAGE_JPEG
    &image_plugin_jpeg,
#endif
#ifdef ENGINE_IMAGE_CG
    &image_plugin_cg,
#endif
#ifdef ENGINE_IMAGE_PVRTC
    &image_plugin_pvrtc
#endif
};

enum { image_plugins_num = STSIZE(image_plugins) };

static bool image_plugins_usable[image_plugins_num];

/*
=================
resample_lerp_line
=================
*/
static void resample_lerp_line (const uint8_t *in, uint8_t *out, int inwidth, int outwidth)
{
    int j, xi, oldx, f, fstep, endx;

    oldx  = 0;
    fstep = (int)(inwidth * 65536.0f / outwidth);
    endx  = inwidth - 1;

    for (j = f = 0; j < outwidth ;j++, f += fstep)
    {
        xi = f >> 16;

        if (xi != oldx)
        {
            in += (xi - oldx) << 2;
            oldx = xi;
        }

        if (xi < endx)
        {
            uint16_t lerp = f & 0xffff;
            *out++ = (uint8_t)((((in[4] - in[0]) * lerp) >> 16) + in[0]);
            *out++ = (uint8_t)((((in[5] - in[1]) * lerp) >> 16) + in[1]);
            *out++ = (uint8_t)((((in[6] - in[2]) * lerp) >> 16) + in[2]);
            *out++ = (uint8_t)((((in[7] - in[3]) * lerp) >> 16) + in[3]);
        }
        else
        {
            *out++ = in[0];
            *out++ = in[1];
            *out++ = in[2];
            *out++ = in[3];
        }
    }
}

/*
=================
image_load
=================
*/
bool image_load (const char *name, image_t *image)
{
    int i;
    const char * const *exts;
    char tmp[MISC_MAX_FILENAME];

    if (NULL == name || NULL == image)
    {
        sys_printf("bad args (name=%p, image=%p)\n", name, image);
        return false;
    }

    image->data = NULL;

    if (!filename_is_valid(name))
        return false;

    image->teximage2d = NULL;
    image->format = 0;
    image->miplevels = 0;

    for (i = 0; i < image_plugins_num ;i++)
    {
        if (!image_plugins_usable[i] || NULL == image_plugins[i]->load)
            continue;

        for (exts = image_plugins[i]->extensions; NULL != exts && NULL != *exts ;exts++)
        {
            snprintf(tmp, sizeof(tmp), "%s.%s", name, *exts);

            if (image_plugins[i]->load(tmp, image))
            {
                strlcpy(image->name, name, sizeof(image->name));
                return true;
            }
        }
    }

    sys_printf("failed to load \"%s\"\n", name);

    return false;
}

/*
=================
image_mipmap
=================
*/
int image_mipmap (image_t *image)
{
    int            i, j, row;
    const uint8_t *in;
    uint8_t       *out;

    if (NULL == image)
    {
        sys_printf("NULL image\n");
        return -1;
    }

    if (image->width < 2 || image->height < 2 || image->format || image->miplevels)
        return 1;

    in = out = image->data;
    row = image->width << 2;

    image->width  >>= 1;
    image->height >>= 1;

    for (i = 0; i < image->height ;i++, in += row)
    {
        for (j = 0; j < image->width ;j++, out += 4, in += 8)
        {
            out[0] = (in[0] + in[4] + in[row+0] + in[row+4]) >> 2;
            out[1] = (in[1] + in[5] + in[row+1] + in[row+5]) >> 2;
            out[2] = (in[2] + in[6] + in[row+2] + in[row+6]) >> 2;
            out[3] = (in[3] + in[7] + in[row+3] + in[row+7]) >> 2;
        }
    }

    return 0;
}

/*
=================
image_resize
=================
*/
bool image_resize (image_t *image, int outwidth, int outheight)
{
    uint8_t *outdata;
    int      inrowlen, outrowlen, i;

    if (NULL == image || outwidth < 1 || outheight < 1)
    {
        sys_printf("bad args (image=%p, outwidth=%i, outheight=%i)\n", image, outwidth, outheight);
        return false;
    }

    if (image->format || (outwidth == image->width && image->height == outheight))
        return true;

    if (NULL == (outdata = mem_alloc(image_mempool, (outwidth * outheight) << 2)))
    {
        sys_printf("not enough memory to resize image\n");
        return false;
    }

    inrowlen = image->width << 2;
    outrowlen = outwidth << 2;

    for (i = 0; i < image->height ;i++)
    {
        memcpy(outdata + i * outrowlen,
               image->data + i * inrowlen,
               inrowlen);
    }

    mem_free(image->data);

    image->data   = outdata;
    image->width  = outwidth;
    image->height = outheight;

    return true;
}

/*
=================
image_scale
=================
*/
bool image_scale (image_t *image, int outwidth, int outheight)
{
    int            i, j, yi, oldy, f, fstep, endy;
    uint8_t       *out, *outdata, *row1, *row2;
    const uint8_t *inrow, *indata;
    int            inwidth, inheight;

    if (NULL == image || outwidth < 1 || outheight < 1)
    {
        sys_printf("bad args (image=%p, outwidth=%i, outheight=%i)\n", image, outwidth, outheight);
        return false;
    }

    if (image->format || (outwidth == image->width && image->height == outheight))
        return true;

    indata   = image->data;
    inwidth  = image->width;
    inheight = image->height;

    if (NULL == (outdata = mem_alloc(image_mempool, (outwidth * outheight) << 2)))
    {
        sys_printf("not enough memory to scale image\n");
        return false;
    }

    endy  = inheight - 1;
    out   = outdata;
    fstep = (int)(inheight * 65536.0f / outheight);

    row1 = mem_alloc(image_mempool, outwidth << 2);
    row2 = mem_alloc(image_mempool, outwidth << 2);

    inrow = indata;
    oldy  = 0;

    resample_lerp_line(inrow,                  row1, inwidth, outwidth);
    resample_lerp_line(inrow + (inwidth << 2), row2, inwidth, outwidth);

    for (i = f = 0; i < outheight ;i++, f += fstep)
    {
        yi = f >> 16;

        if(yi < endy)
        {
            int lerp = f & 0xffff;

            if (yi != oldy)
            {
                inrow = (uint8_t *)indata + (inwidth << 2) * yi;

                if (yi == oldy + 1)
                    memcpy(row1, row2, outwidth << 2);
                else
                    resample_lerp_line(inrow, row1, inwidth, outwidth);

                resample_lerp_line(inrow + (inwidth << 2), row2, inwidth, outwidth);
                oldy = yi;
            }

            j = outwidth - 4;

            while (j >= 0)
            {

                out[ 0] = (uint8_t)((((row2[ 0] - row1[ 0]) * lerp) >> 16) + row1[ 0]);
                out[ 1] = (uint8_t)((((row2[ 1] - row1[ 1]) * lerp) >> 16) + row1[ 1]);
                out[ 2] = (uint8_t)((((row2[ 2] - row1[ 2]) * lerp) >> 16) + row1[ 2]);
                out[ 3] = (uint8_t)((((row2[ 3] - row1[ 3]) * lerp) >> 16) + row1[ 3]);
                out[ 4] = (uint8_t)((((row2[ 4] - row1[ 4]) * lerp) >> 16) + row1[ 4]);
                out[ 5] = (uint8_t)((((row2[ 5] - row1[ 5]) * lerp) >> 16) + row1[ 5]);
                out[ 6] = (uint8_t)((((row2[ 6] - row1[ 6]) * lerp) >> 16) + row1[ 6]);
                out[ 7] = (uint8_t)((((row2[ 7] - row1[ 7]) * lerp) >> 16) + row1[ 7]);
                out[ 8] = (uint8_t)((((row2[ 8] - row1[ 8]) * lerp) >> 16) + row1[ 8]);
                out[ 9] = (uint8_t)((((row2[ 9] - row1[ 9]) * lerp) >> 16) + row1[ 9]);
                out[10] = (uint8_t)((((row2[10] - row1[10]) * lerp) >> 16) + row1[10]);
                out[11] = (uint8_t)((((row2[11] - row1[11]) * lerp) >> 16) + row1[11]);
                out[12] = (uint8_t)((((row2[12] - row1[12]) * lerp) >> 16) + row1[12]);
                out[13] = (uint8_t)((((row2[13] - row1[13]) * lerp) >> 16) + row1[13]);
                out[14] = (uint8_t)((((row2[14] - row1[14]) * lerp) >> 16) + row1[14]);
                out[15] = (uint8_t)((((row2[15] - row1[15]) * lerp) >> 16) + row1[15]);
                out  += 16;
                row1 += 16;
                row2 += 16;
                j    -= 4;
            }

            if (j & 2)
            {
                out[ 0] = (uint8_t)((((row2[ 0] - row1[ 0]) * lerp) >> 16) + row1[ 0]);
                out[ 1] = (uint8_t)((((row2[ 1] - row1[ 1]) * lerp) >> 16) + row1[ 1]);
                out[ 2] = (uint8_t)((((row2[ 2] - row1[ 2]) * lerp) >> 16) + row1[ 2]);
                out[ 3] = (uint8_t)((((row2[ 3] - row1[ 3]) * lerp) >> 16) + row1[ 3]);
                out[ 4] = (uint8_t)((((row2[ 4] - row1[ 4]) * lerp) >> 16) + row1[ 4]);
                out[ 5] = (uint8_t)((((row2[ 5] - row1[ 5]) * lerp) >> 16) + row1[ 5]);
                out[ 6] = (uint8_t)((((row2[ 6] - row1[ 6]) * lerp) >> 16) + row1[ 6]);
                out[ 7] = (uint8_t)((((row2[ 7] - row1[ 7]) * lerp) >> 16) + row1[ 7]);
                out  += 8;
                row1 += 8;
                row2 += 8;
            }

            if (j & 1)
            {
                out[ 0] = (uint8_t)((((row2[ 0] - row1[ 0]) * lerp) >> 16) + row1[ 0]);
                out[ 1] = (uint8_t)((((row2[ 1] - row1[ 1]) * lerp) >> 16) + row1[ 1]);
                out[ 2] = (uint8_t)((((row2[ 2] - row1[ 2]) * lerp) >> 16) + row1[ 2]);
                out[ 3] = (uint8_t)((((row2[ 3] - row1[ 3]) * lerp) >> 16) + row1[ 3]);
                out  += 4;
                row1 += 4;
                row2 += 4;
            }

            row1 -= outwidth << 2;
            row2 -= outwidth << 2;
        }
        else
        {
            if (yi != oldy)
            {
                inrow = (uint8_t *)indata + yi * (inwidth << 2);

                if (yi == oldy + 1)
                    memcpy(row1, row2, outwidth << 2);
                else
                    resample_lerp_line(inrow, row1, inwidth, outwidth);

                oldy = yi;
            }

            memcpy(out, row1, outwidth << 2);
        }
    }

    mem_free(row1);
    mem_free(row2);
    mem_free(image->data);

    image->data   = outdata;
    image->width  = outwidth;
    image->height = outheight;

    return true;
}

/*
=================
image_init
=================
*/
bool image_init (void)
{
    int i;
    char tmp[16];

    image_mempool = mem_alloc_pool("image", 0);

    for (i = 0; i < image_plugins_num ;i++)
    {
        snprintf(tmp, sizeof(tmp), "-no%s", image_plugins[i]->name);

        image_plugins_usable[i] = !sys_arg_find(tmp) && (NULL == image_plugins[i]->init ||
                                                         image_plugins[i]->init());

        if (image_plugins_usable[i])
        {
            sys_printf("+%s\n", image_plugins[i]->name);
        }
    }

    image_i = true;
    sys_printf("+image\n");

    return true;
}

/*
=================
image_shutdown
=================
*/
void image_shutdown (void)
{
    int i;

    if (!image_i)
        return;

    for (i = 0; i < image_plugins_num ;i++)
    {
        if (image_plugins_usable[i])
        {
            if (NULL != image_plugins[i]->shutdown)
                image_plugins[i]->shutdown();

            sys_printf("-%s\n", image_plugins[i]->name);
        }
    }

    mem_free_pool(&image_mempool);

    image_i = false;
    sys_printf("-image\n");
}
