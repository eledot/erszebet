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
 * @file video.h
 *
 * @brief SDL video interface header
 *
 * @author ftrvxmtrx
 */

#ifndef _VIDEO_H
#define _VIDEO_H

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
extern int video_fullscreen;

extern int video_grabbed;

/**
 * @brief video_width/video_height
 */
extern double video_aspect;

/**
 * @brief Receives special events
 *
 * @param fullscreen_toggle Non-zero to toggle fullscreen mode (for alt+enter)
 * @param grab_toggle Non-zero to toggle input grabbing (console in windowed mode)
 * @param neww New main surface width (video resize event)
 * @param newh New main surface height (video resize event)
 */
void video_event (int fullscreen_toggle, int grab_toggle, int neww, int newh);

/**
 * @brief Flips main surface with second buffer
 */
void video_flip (void);

/**
 * @brief Switches video mode
 *
 * @param w Width
 * @param h Height
 * @param fullscreen Non-zero to switch fullscreen
 *
 * @return 0 on success
 */
int video_set_mode (int w, int h, int fullscreen);

/**
 * @brief Video module initialization
 *
 * @return 0 on success
 */
int video_init (void);

/**
 * @brief Video module deinitialization
 */
void video_shutdown (void);

#endif /* !_VIDEO_H */
