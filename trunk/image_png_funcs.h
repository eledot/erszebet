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

/**
 * @file image_png_funcs.h
 *
 * @brief image_png.c private header with PNG funcs
 *
 * @author ftrvxmtrx
 */

#ifndef _IMAGE_PNG_FUNCS_H
#define _IMAGE_PNG_FUNCS_H

#ifndef PNG_LIBPNG_VER_STRING
#error This is internal header!
#endif

/* read */
static png_infop   (*epng_create_info_struct)         (png_structp png_ptr);
static png_structp (*epng_create_read_struct)         (png_const_charp user_png_ver, png_voidp error_ptr, png_error_ptr error_fn, png_error_ptr warn_fn);
static void        (*epng_destroy_read_struct)        (png_structpp png_ptr_ptr, png_infopp info_ptr_ptr, png_infopp end_info_ptr_ptr);
static png_byte    (*epng_get_bit_depth)              (png_structp png_ptr, png_infop info_ptr);
static png_byte    (*epng_get_color_type)             (png_structp png_ptr, png_infop info_ptr);
static png_uint_32 (*epng_get_image_height)           (png_structp png_ptr, png_infop info_ptr);
static png_uint_32 (*epng_get_image_width)            (png_structp png_ptr, png_infop info_ptr);
static png_voidp   (*epng_get_io_ptr)                 (png_structp png_ptr);
static png_uint_32 (*epng_get_valid)                  (png_structp png_ptr, png_infop info_ptr, png_uint_32 flag);
static void        (*epng_read_end)                   (png_structp png_ptr, png_infop info_ptr);
static void        (*epng_read_info)                  (png_structp png_ptr, png_infop info_ptr);
static void        (*epng_read_row)                   (png_structp png_ptr, png_bytep row, png_bytep display_row);
static void        (*epng_set_add_alpha)              (png_structp png_ptr, png_uint_32 filler, int flags);
static void        (*epng_set_expand_gray_1_2_4_to_8) (png_structp png_ptr);
static void        (*epng_set_gray_to_rgb)            (png_structp png_ptr);
static void        (*epng_set_interlace_handling)     (png_structp png_ptr);
static void        (*epng_set_palette_to_rgb)         (png_structp png_ptr);
static void        (*epng_set_read_fn)                (png_structp png_ptr, png_voidp io_ptr, png_rw_ptr read_data_fn);
static void        (*epng_set_strip_16)               (png_structp png_ptr);
static void        (*epng_set_tRNS_to_alpha)          (png_structp png_ptr);
static void        (*epng_set_user_limits)            (png_structp png_ptr, png_uint_32 user_width_max, png_uint_32 user_height_max);
/* write */
static png_structp (*epng_create_write_struct)        (png_const_charp user_png_ver, png_voidp error_ptr, png_error_ptr error_fn, png_error_ptr warn_fn);
static void        (*epng_destroy_write_struct)       (png_structpp png_ptr_ptr, png_infopp info_ptr_ptr);
static void        (*epng_set_IHDR)                   (png_structp  png_ptr, png_infop info_ptr, png_uint_32 width, png_uint_32 height, int bit_depth, int color_type, int interlace_type, int compression_type, int filter_type);
static void        (*epng_set_write_fn)               (png_structp png_ptr, png_voidp io_ptr, png_rw_ptr write_data_fn, png_flush_ptr output_flush_fn);
static void        (*epng_write_end)                  (png_structp png_ptr, png_infop info_ptr);
static void        (*epng_write_info)                 (png_structp png_ptr, png_infop info_ptr);
static void        (*epng_write_row)                  (png_structp png_ptr, png_bytep row);

static lib_func_t funcs[] =
{
    /* read */
    { "png_create_info_struct",         (void **)&epng_create_info_struct         },
    { "png_create_read_struct",         (void **)&epng_create_read_struct         },
    { "png_destroy_read_struct",        (void **)&epng_destroy_read_struct        },
    { "png_get_bit_depth",              (void **)&epng_get_bit_depth              },
    { "png_get_color_type",             (void **)&epng_get_color_type             },
    { "png_get_image_width",            (void **)&epng_get_image_width            },
    { "png_get_image_height",           (void **)&epng_get_image_height           },
    { "png_get_io_ptr",                 (void **)&epng_get_io_ptr                 },
    { "png_get_valid",                  (void **)&epng_get_valid                  },
    { "png_read_end",                   (void **)&epng_read_end                   },
    { "png_read_info",                  (void **)&epng_read_info                  },
    { "png_read_row",                   (void **)&epng_read_row                   },
    { "png_set_add_alpha",              (void **)&epng_set_add_alpha              },
    { "png_set_expand_gray_1_2_4_to_8", (void **)&epng_set_expand_gray_1_2_4_to_8 },
    { "png_set_gray_to_rgb",            (void **)&epng_set_gray_to_rgb            },
    { "png_set_interlace_handling",     (void **)&epng_set_interlace_handling     },
    { "png_set_palette_to_rgb",         (void **)&epng_set_palette_to_rgb         },
    { "png_set_read_fn",                (void **)&epng_set_read_fn                },
    { "png_set_strip_16",               (void **)&epng_set_strip_16               },
    { "png_set_tRNS_to_alpha",          (void **)&epng_set_tRNS_to_alpha          },
    { "png_set_user_limits",            (void **)&epng_set_user_limits            },
    /* write */
    { "png_create_write_struct",        (void **)&epng_create_write_struct        },
    { "png_destroy_write_struct",       (void **)&epng_destroy_write_struct       },
    { "png_set_IHDR",                   (void **)&epng_set_IHDR                   },
    { "png_set_write_fn",               (void **)&epng_set_write_fn               },
    { "png_write_end",                  (void **)&epng_write_end                  },
    { "png_write_info",                 (void **)&epng_write_info                 },
    { "png_write_row",                  (void **)&epng_write_row                  },
    { NULL, NULL }
};

#endif /* !_IMAGE_PNG_FUNCS_H */
