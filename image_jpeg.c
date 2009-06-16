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

#include "common.h"

#ifdef ENGINE_IMAGE_JPEG

#include <setjmp.h>
#include <stdio.h>
#include <jpeglib.h>

static int image_jpeg_i = 0;

static jmp_buf   jpeg_jmpbuf;
static fs_file_t jpeg_file;

static uint8_t jpeg_buffer[1024];

/*
=================
ejpeg_error_exit
=================
*/
static void ejpeg_error_exit (j_common_ptr cinfo)
{
    char tmp[4096];
    cinfo->err->format_message(cinfo, tmp);
    sys_printf("libjpeg error: %s\n", tmp);

    longjmp(jpeg_jmpbuf, 1);
}

/*
=================
ejpeg_emit_message
=================
*/
static void ejpeg_emit_message (j_common_ptr cinfo UV, int msg_level UV)
{
    /* shut up warnings */
/*
    char tmp[4096];
    cinfo->err->format_message(cinfo, tmp);
    sys_printf("%s\n", tmp);
*/
}

/*
=================
ejpeg_source_mgr_init_source
=================
*/
static void ejpeg_source_mgr_init_source (j_decompress_ptr cinfo)
{
    cinfo->src->next_input_byte = NULL;
    cinfo->src->bytes_in_buffer = 0;
}

/*
=================
ejpeg_source_mgr_fill_input_buffer
=================
*/
static boolean ejpeg_source_mgr_fill_input_buffer (j_decompress_ptr cinfo)
{
    cinfo->src->next_input_byte = jpeg_buffer;

    if (1 > (cinfo->src->bytes_in_buffer = fs_read(jpeg_file, jpeg_buffer, sizeof(jpeg_buffer))))
        longjmp(jpeg_jmpbuf, 1);

    return TRUE;
}

/*
=================
ejpeg_source_mgr_skip_input_data
=================
*/
static void ejpeg_source_mgr_skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
    if (num_bytes > (long)cinfo->src->bytes_in_buffer)
    {
        cinfo->src->next_input_byte = NULL;
        cinfo->src->bytes_in_buffer = 0;
    }
    else
    {
        cinfo->src->next_input_byte += num_bytes;
        cinfo->src->bytes_in_buffer -= num_bytes;
    }
}

/*
=================
ejpeg_source_mgr_term_source
=================
*/
static void ejpeg_source_mgr_term_source (j_decompress_ptr cinfo UV)
{
}

/*
=================
ejpeg_destination_mgr_init_destination
=================
*/
static void ejpeg_destination_mgr_init_destination (j_compress_ptr cinfo)
{
    cinfo->dest->next_output_byte = jpeg_buffer;
    cinfo->dest->free_in_buffer = sizeof(jpeg_buffer);
}

/*
=================
ejpeg_destination_mgr_empty_output_buffer
=================
*/
static boolean ejpeg_destination_mgr_empty_output_buffer (j_compress_ptr cinfo)
{
    if (sizeof(jpeg_buffer) != fs_write(jpeg_file, jpeg_buffer, sizeof(jpeg_buffer)))
        longjmp(jpeg_jmpbuf, 1);

    cinfo->dest->next_output_byte = jpeg_buffer;
    cinfo->dest->free_in_buffer = sizeof(jpeg_buffer);

    return TRUE;
}

/*
=================
ejpeg_destination_mgr_term_destination
=================
*/
static void ejpeg_destination_mgr_term_destination (j_compress_ptr cinfo)
{
    int size;

    size = sizeof(jpeg_buffer) - cinfo->dest->free_in_buffer;

    if (size != fs_write(jpeg_file, jpeg_buffer, size))
        longjmp(jpeg_jmpbuf, 1);
}

/*
=================
ejpeg_src
=================
*/
static void ejpeg_src (j_decompress_ptr cinfo, void *infile)
{
    static struct jpeg_source_mgr mgr =
        {
            .next_input_byte = NULL,
            .bytes_in_buffer = 0,
            .init_source = &ejpeg_source_mgr_init_source,
            .fill_input_buffer = &ejpeg_source_mgr_fill_input_buffer,
            .skip_input_data = &ejpeg_source_mgr_skip_input_data,
            .term_source = &ejpeg_source_mgr_term_source
        };

    mgr.resync_to_restart = jpeg_resync_to_restart;

    cinfo->src = &mgr;
    jpeg_file  = infile;
}

/*
=================
ejpeg_dest
=================
*/
static void ejpeg_dest (j_compress_ptr cinfo, void *outfile)
{
    static struct jpeg_destination_mgr mgr =
        {
            .next_output_byte = NULL,
            .free_in_buffer = 0,
            .init_destination = &ejpeg_destination_mgr_init_destination,
            .empty_output_buffer = &ejpeg_destination_mgr_empty_output_buffer,
            .term_destination = &ejpeg_destination_mgr_term_destination
        };

    cinfo->dest = &mgr;
    jpeg_file   = outfile;
}

/*
=================
image_jpeg_load
=================
*/
int image_jpeg_load (const char *name, image_t *im, mem_pool_t pool)
{
    fs_file_t                     f;
    unsigned char                *p, *image = NULL;
    int                           size, r, inc, width, height, x;
    struct jpeg_decompress_struct ds;
    struct jpeg_error_mgr         em;

    if (!image_jpeg_i)
        return -1;

    if (NULL == (f = fs_open(name, FS_RDONLY, &size, 0)))
        return -2;

    ds.err = jpeg_std_error(&em);

    ds.err->error_exit   = ejpeg_error_exit;
    ds.err->emit_message = ejpeg_emit_message;

    if (setjmp(jpeg_jmpbuf))
    {
        fs_close(f);
        return -3;
    }

    jpeg_CreateDecompress(&ds, JPEG_LIB_VERSION, sizeof(ds));

    if (setjmp(jpeg_jmpbuf))
        goto error;

    ejpeg_src(&ds, f);
    jpeg_read_header(&ds, TRUE);

    ds.out_color_space = JCS_RGB;
    jpeg_start_decompress(&ds);

    width  = ds.output_width;
    height = ds.output_height;

    inc = width * 4;
    p = image = mem_alloc(pool, height * inc);

    for (r = 0; r < height ;r++, p += inc)
    {
        JSAMPARRAY scanlines[1] = { (void *)p };
        jpeg_read_scanlines(&ds, (void *)scanlines, 1);
    }

    jpeg_finish_decompress(&ds);
    jpeg_destroy_decompress(&ds);

    fs_close(f);

    p = image;

    for (r = 0; r < height ;r++, p += inc)
    {
        for (x = width - 1; x >= 0 ;x--)
        {
            int index4 = (x << 2);
            int index3 = (x * 3);

            p[index4 + 0] = p[index3 + 0];
            p[index4 + 1] = p[index3 + 1];
            p[index4 + 2] = p[index3 + 2];
            p[index4 + 3] = 0xff;
        }
    }

    im->width  = width;
    im->height = height;
    im->data   = image;

    return 0;

error:
    jpeg_destroy_decompress(&ds);

    fs_close(f);

    if (NULL != image)
        mem_free(image);

    return -4;
}

/*
=================
image_jpeg_save
=================
*/
int image_jpeg_save (const char *name, image_t *im)
{
    fs_file_t                   f;
    struct jpeg_compress_struct cs;
    struct jpeg_error_mgr       em;
    int                         r, inc;

    if (!image_jpeg_i)
        return -1;

    if (NULL == (f = fs_open(name, FS_WRONLY, NULL, 0)))
        return -2;

    cs.err = jpeg_std_error(&em);

    cs.err->error_exit   = ejpeg_error_exit;
    cs.err->emit_message = ejpeg_emit_message;

    if (setjmp(jpeg_jmpbuf))
    {
        fs_close(f);
        return -3;
    }

    jpeg_CreateCompress(&cs, JPEG_LIB_VERSION, sizeof(cs));

    if (setjmp(jpeg_jmpbuf))
        goto error;

    ejpeg_dest(&cs, f);

    cs.image_width      = im->width;
    cs.image_height     = im->height;
    cs.input_components = 3;
    cs.in_color_space   = JCS_RGB;

    jpeg_set_defaults(&cs);
    jpeg_set_quality(&cs, 100, TRUE);
    jpeg_start_compress(&cs, TRUE);

    inc = im->width * 3;

    for (r = im->height - 1; r >= 0 ;r--)
    {
        JSAMPROW scanlines[1] = { (void *)im->data + inc * r };

        jpeg_write_scanlines(&cs, scanlines, 1);
    }

    jpeg_finish_compress(&cs);
    jpeg_destroy_compress(&cs);

    fs_close(f);

    return 0;

error:
    jpeg_destroy_compress(&cs);

    fs_close(f);

    return -4;
}

/*
=================
image_jpeg_init
=================
*/
int image_jpeg_init (void)
{
    if (sys_arg_find("-nolibjpeg"))
        return 0;

    image_jpeg_i = 1;
    sys_printf("+image_jpeg\n");

    return 0;
}

/*
=================
image_jpeg_shutdown
=================
*/
void image_jpeg_shutdown (void)
{
    if (!image_jpeg_i)
        return;

    image_jpeg_i = 0;
    sys_printf("-image_jpeg\n");
}

#else /* !ENGINE_IMAGE_JPEG */

#include "common.h"

int image_jpeg_load (const char *name UV, image_t *im UV, mem_pool_t pool UV) { return -1; }
int image_jpeg_save (const char *name UV, image_t *im UV) { sys_printf("jpeg support was not compiled in\n"); return -1; }

int image_jpeg_init (void) { return -1; }
void image_jpeg_shutdown (void) { }

#endif /* ENGINE_IMAGE_JPEG */
