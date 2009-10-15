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

#ifdef ENGINE_OS_IPHONE

#include "common.h"

int video_orientation;
int video_width;
int video_height;
bool video_fullscreen;
bool video_grabbed = true;
double video_aspect;

/*
=================
video_grab_toggle
=================
*/
void video_grab_toggle (void)
{
}

/*
=================
video_fullscreen_toggle
=================
*/
void video_fullscreen_toggle (void)
{
}

/*
=================
video_frame
=================
*/
void video_frame (void)
{
}

/*
=================
video_set_mode
=================
*/
bool video_set_mode (int w, int h, bool fullscreen)
{
    return false;
}

/*
=================
video_init
=================
*/
bool video_init (void)
{
    /* FIXME -- take this from accelerometer? */
    video_orientation = VIDEO_LANDSCAPE;

    return true;
}

/*
=================
video_shutdown
=================
*/
void video_shutdown (void)
{
}

#endif /* ENGINE_OS_IPHONE */
