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
 * @file image_jpeg.h
 *
 * @brief JPEG images support interface header
 *
 * @author ftrvxmtrx
 */

#ifndef _IMAGE_JPEG_H
#define _IMAGE_JPEG_H

/**
 * @brief Tries to load an image as if it was a JPEG one
 *
 * @param name Filename
 * @param im Image structure to fill in
 *
 * @return 0 - success
 */
int image_jpeg_load (const char *name, image_t *im, mem_pool_t pool);

/**
 * @brief Tries to save an image in JPEG format
 *
 * @param name Filename
 * @param im Image to save
 *
 * @return 0 - success
 */
int image_jpeg_save (const char *name, image_t *im);

/**
 * @brief JPEG module initialization
 *
 * @return 0 on success
 */
int image_jpeg_init (void);

/**
 * @brief JPEG module deinitialization
 */
void image_jpeg_shutdown (void);

#endif /* !_IMAGE_JPEG_H */
