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

/**
 * @file image_jpeg_funcs.h
 *
 * @brief image_jpeg.c private header with JPEG funcs
 *
 * @author ftrvxmtrx
 */

#ifndef _IMAGE_JPEG_FUNCS_H
#define _IMAGE_JPEG_FUNCS_H

#ifndef JPEG_LIB_VERSION
#error This is internal header!
#endif

/* read */
static void                    (*ejpeg_create_decompress)  (j_decompress_ptr cinfo, int version, size_t structsize);
static void                    (*ejpeg_destroy_decompress) (j_decompress_ptr cinfo);
static boolean                 (*ejpeg_start_decompress)   (j_decompress_ptr cinfo);
static JDIMENSION              (*ejpeg_read_scanlines)     (j_decompress_ptr cinfo, JSAMPARRAY scanlines, JDIMENSION max_lines);
static boolean                 (*ejpeg_finish_decompress)  (j_decompress_ptr cinfo);
static int                     (*ejpeg_read_header)        (j_decompress_ptr cinfo, boolean require_image);
static struct jpeg_error_mgr * (*ejpeg_std_error)          (struct jpeg_error_mgr *err);
static boolean                 (*ejpeg_resync_to_restart)  (j_decompress_ptr cinfo, int desired);
/* write */
static void                    (*ejpeg_create_compress)    (j_compress_ptr cinfo, int version, size_t structsize);
static void                    (*ejpeg_destroy_compress)   (j_compress_ptr cinfo);
static void                    (*ejpeg_finish_compress)    (j_compress_ptr cinfo);
static void                    (*ejpeg_set_defaults)       (j_compress_ptr cinfo);
static void                    (*ejpeg_set_quality)        (j_compress_ptr cinfo, int quality, boolean force_baseline);
static void                    (*ejpeg_start_compress)     (j_compress_ptr cinfo, boolean write_all_tables);
static JDIMENSION              (*ejpeg_write_scanlines)    (j_compress_ptr cinfo, JSAMPARRAY scanlines, JDIMENSION num_lines);

static lib_func_t funcs[] =
{
    /* read */
    { "jpeg_CreateDecompress",   (void **)&ejpeg_create_decompress  },
    { "jpeg_destroy_decompress", (void **)&ejpeg_destroy_decompress },
    { "jpeg_start_decompress",   (void **)&ejpeg_start_decompress   },
    { "jpeg_read_scanlines",     (void **)&ejpeg_read_scanlines     },
    { "jpeg_finish_decompress",  (void **)&ejpeg_finish_decompress  },
    { "jpeg_read_header",        (void **)&ejpeg_read_header        },
    { "jpeg_std_error",          (void **)&ejpeg_std_error          },
    { "jpeg_resync_to_restart",  (void **)&ejpeg_resync_to_restart  },
    /* write */
    { "jpeg_CreateCompress",     (void **)&ejpeg_create_compress    },
    { "jpeg_destroy_compress",   (void **)&ejpeg_destroy_compress   },
    { "jpeg_finish_compress",    (void **)&ejpeg_finish_compress    },
    { "jpeg_set_defaults",       (void **)&ejpeg_set_defaults       },
    { "jpeg_set_quality",        (void **)&ejpeg_set_quality        },
    { "jpeg_start_compress",     (void **)&ejpeg_start_compress     },
    { "jpeg_write_scanlines",    (void **)&ejpeg_write_scanlines    },
    { NULL, NULL }
};

#endif /* !_IMAGE_JPEG_FUNCS_H */
