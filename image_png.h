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
 * @file image_png.h
 *
 * @brief PNG images support interface header
 *
 * @author ftrvxmtrx
 */

#ifndef _IMAGE_PNG_H
#define _IMAGE_PNG_H

/**
 * @brief Tries to load an image as if it was a PNG one
 *
 * @param name Filename
 * @param im Image structure to fill in
 *
 * @return 0 - success
 */
int image_png_load (const char *name, image_t *im, mem_pool_t pool) GNUC_NONNULL;

/**
 * @brief Tries to save an image in PNG format
 *
 * @param name Filename
 * @param im Image to save
 *
 * @return 0 - success
 */
int image_png_save (const char *name, image_t *im) GNUC_NONNULL;

/**
 * @brief PNG module initialization
 *
 * @return 0 on success
 */
int image_png_init (void);

/**
 * @brief PNG module deinitialization
 */
void image_png_shutdown (void);

#endif /* !_IMAGE_PNG_H */
