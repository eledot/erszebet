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
 * @file image.h
 *
 * @brief Images interface header
 *
 * @author ftrvxmtrx
 */

#ifndef _IMAGE_H
#define _IMAGE_H

/**
 * @brief Maximal image width
 */
#define IMAGE_MAX_WIDTH  4096

/**
 * @brief Maximal image height
 */
#define IMAGE_MAX_HEIGHT 4096

/**
 * @brief Image structure
 */
typedef struct image_s
{
    char name[MISC_MAX_FILENAME];  /**< Image filename passed to image_load   */
    int  width;                    /**< Image width                           */
    int  height;                   /**< Image height                          */
    int  miplevels;                /**< Number of mip levels in image         */
    int  format;                   /**< Image data GL format                  */
    unsigned char *data;           /**< Pixels (NULL in case texture created) */
    int  data_size;

    void (*teximage2d) (struct image_s *im);

    struct image_s *next;
}image_t;

/**
 * @brief Tries to load an image
 *
 * @param name Filename
 * @param image Where to store image data
 *
 * @return 0 on success, -1 otherwise
 */
int image_load (const char *name, image_t *image) GNUC_NONNULL;

int image_mipmap (image_t *image) GNUC_NONNULL;

int image_resize (image_t *image, int width, int height) GNUC_NONNULL;

/**
 * @brief Image module initialization
 *
 * @return 0 on success
 */
int image_init (void);

/**
 * @brief Image module deinitialization
 */
void image_shutdown (void);

#endif /* !_IMAGE_H */
