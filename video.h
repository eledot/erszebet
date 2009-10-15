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
 * @file video.h
 *
 * @brief Video interface header
 *
 * @author ftrvxmtrx
 */

#ifndef _VIDEO_H
#define _VIDEO_H

typedef enum
{
    VIDEO_LANDSCAPE = 0,
    VIDEO_PORTRAIT
}video_orientations_e;

extern int video_orientation;

/**
 * @brief Main surface width
 */
extern int video_width;

/**
 * @brief Main surface height
 */
extern int video_height;

/**
 * @brief Non-zero in fullscreen mode
 */
extern bool video_fullscreen;

/**
 * @brief Non-zero if input is grabbed
 */
extern bool video_grabbed;

/**
 * @brief video_width/video_height
 */
extern double video_aspect;

/**
 * @brief Flips main surface with second buffer and parses input/video events
 */
void video_frame (void);

bool video_set_mode (int w, int h, bool fullscreen);

/**
 * @brief Toggles input grabbing
 */
void video_grab_toggle (void);

/**
 * @brief Toggles fullscreen mode
 */
void video_fullscreen_toggle (void);

/**
 * @brief Video module initialization
 *
 * @return 0 on success
 */
bool video_init (void);

/**
 * @brief Video module deinitialization
 */
void video_shutdown (void);

#endif /* !_VIDEO_H */
