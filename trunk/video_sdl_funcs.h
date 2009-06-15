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
 * @file video_sdl_funcs.h
 *
 * @brief video.c private header with SDL funcs
 *
 * @author ftrvxmtrx
 */

#ifndef _VIDEO_SDL_FUNCS_H
#define _VIDEO_SDL_FUNCS_H

#ifndef SDL_VERSION
#error This is internal header!
#endif

static Uint8         (*esdl_event_state)          (Uint8 type, int state);
static int           (*esdl_flip)                 (SDL_Surface *screen);
static int           (*esdl_gl_set_attribute)     (SDL_GLattr attr, int value);
static void          (*esdl_gl_swap_buffers)      (void);
static int           (*esdl_init)                 (Uint32 flags);
static SDL_Rect **   (*esdl_list_modes)           (SDL_PixelFormat *format, Uint32 flags);
int                  (*esdl_poll_event)           (SDL_Event *event);
static SDL_Surface * (*esdl_set_video_mode)       (int width, int height, int bpp, Uint32 flags);
static int           (*esdl_show_cursor)          (int toggle);
static void          (*esdl_quit)                 (void);
static SDL_GrabMode  (*esdl_wm_grab_input)        (SDL_GrabMode mode);
static int           (*esdl_wm_toggle_fullscreen) (SDL_Surface *surface);
int                  (*esdl_enable_key_repeat)    (int delay, int interval);

static lib_func_t funcs[] =
{
    { "SDL_EnableKeyRepeat",     (void **)&esdl_enable_key_repeat    },
    { "SDL_EventState",          (void **)&esdl_event_state          },
    { "SDL_Flip",                (void **)&esdl_flip                 },
    { "SDL_GL_SetAttribute",     (void **)&esdl_gl_set_attribute     },
    { "SDL_GL_SwapBuffers",      (void **)&esdl_gl_swap_buffers      },
    { "SDL_Init",                (void **)&esdl_init                 },
    { "SDL_ListModes",           (void **)&esdl_list_modes           },
    { "SDL_PollEvent",           (void **)&esdl_poll_event           },
    { "SDL_SetVideoMode",        (void **)&esdl_set_video_mode       },
    { "SDL_ShowCursor",          (void **)&esdl_show_cursor          },
    { "SDL_Quit",                (void **)&esdl_quit                 },
    { "SDL_WM_GrabInput",        (void **)&esdl_wm_grab_input        },
    { "SDL_WM_ToggleFullScreen", (void **)&esdl_wm_toggle_fullscreen },
    { NULL, NULL }
};

#endif /* !_VIDEO_SDL_FUNCS_H */
