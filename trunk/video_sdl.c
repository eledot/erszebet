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

#include "common.h"

#include <SDL.h>
#include "video_sdl_funcs.h"

#define DEFAULT_FLAGS (SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_OPENGL)

static lib_t libsdl;

static int video_i = 0;

static SDL_Surface *surf;
static SDL_Rect   **modes;

static cvar_t *r_mode;
static cvar_t *r_custom_width;
static cvar_t *r_custom_height;
static cvar_t *r_fullscreen;

static int nograb;

int video_width;
int video_height;
int video_fullscreen;
int video_grabbed = 0;
double video_aspect;

/*
=================
video_grab_toggle
=================
*/
static void video_grab_toggle (void)
{
    if (nograb)
        return;

    if (!video_grabbed || video_fullscreen)
    {
        esdl_show_cursor(SDL_DISABLE);
        esdl_wm_grab_input(SDL_GRAB_ON);

        video_grabbed = 1;
    }
    else
    {
        esdl_show_cursor(SDL_ENABLE);
        esdl_wm_grab_input(SDL_GRAB_OFF);

        video_grabbed = 0;
    }

    errno = 0;
}

/*
=================
video_fullscreen_toggle
=================
*/
static void video_fullscreen_toggle (void)
{
    video_fullscreen = !video_fullscreen;

    esdl_wm_toggle_fullscreen(surf);

    if (video_fullscreen && !video_grabbed)
        video_grab_toggle();

    errno = 0;
}

/*
=================
video_flip
=================
*/
void video_flip (void)
{
    esdl_gl_swap_buffers();
    errno = 0;
}

/*
=================
video_event
=================
*/
void video_event (int fullscreen_toggle, int grab_toggle, int neww, int newh)
{
    if (fullscreen_toggle)
        video_fullscreen_toggle();

    if (grab_toggle)
        video_grab_toggle();

    if (neww && newh)
        video_set_mode(neww, newh, video_fullscreen);
}

/*
=================
video_set_mode
=================
*/
int video_set_mode (int w, int h, int fullscreen)
{
    int flags = DEFAULT_FLAGS;

    if (fullscreen)
        flags |= SDL_FULLSCREEN;
/*
    else
        flags |= SDL_RESIZABLE;
*/

    sys_printf("using %ix%i video mode\n", w, h);

    esdl_gl_set_attribute(SDL_GL_RED_SIZE, 8);
    esdl_gl_set_attribute(SDL_GL_GREEN_SIZE, 8);
    esdl_gl_set_attribute(SDL_GL_BLUE_SIZE, 8);
    esdl_gl_set_attribute(SDL_GL_DOUBLEBUFFER, 1);

    if (NULL == (surf = esdl_set_video_mode(w, h, 32, flags)))
    {
        sys_printf("SDL_SetVideoMode failed on %ix%i\n", w, h);
        return -1;
    }

    video_width = w;
    video_height = h;
    video_aspect = video_width/video_height;

    gl_set_viewport(0, 0, video_width, video_height);

    video_fullscreen = fullscreen;

    errno = 0;

    return 0;
}

/*
=================
video_init
=================
*/
int video_init (void)
{
    int         flags, w = 0, h = 0, i;
    const char *names[] =
    {
        "libSDL-1.2.so.0",
        "libSDL.so",
        NULL
    };

    nograb = sys_arg_find("-nograb");

    if (LIB_HANDLE_INVALID == (libsdl = lib_open(names, funcs, 0)))
    {
        sys_printf("failed to load sdl library\n");
        return -1;
    }

    r_mode = cvar_get("r_mode", "c", CVAR_FL_SAVE);
    r_custom_width = cvar_get("r_custom_width", "480", CVAR_FL_SAVE);
    r_custom_height = cvar_get("r_custom_height", "320", CVAR_FL_SAVE);
    r_fullscreen = cvar_get("r_fullscreen", "0", CVAR_FL_SAVE);

    if (0 != esdl_init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE))
    {
        sys_printf("SDL_Init failed\n");
        return -2;
    }

    video_i = 1;

    esdl_event_state(SDL_VIDEOEXPOSE, SDL_IGNORE);

    flags = DEFAULT_FLAGS | SDL_FULLSCREEN;

    if (NULL == (modes = esdl_list_modes(NULL, flags)))
    {
        sys_printf("no available video modes\n");
        esdl_quit();
        return -3;
    }

    errno = 0;

    /* use custom mode by default */
    w = r_custom_width->i;
    h = r_custom_height->i;

    if ((SDL_Rect **)-1 == modes || NULL == modes[0])
    {
        sys_printf("all modes available\n");

        modes = NULL;

        cvar_set(r_mode, "c");
    }
    else
    {
        for (i = 0; NULL != modes[i] ;i++)
        {
            if (r_mode->s[0] != 'c' && r_mode->i == i)
            {
                w = modes[i]->w;
                h = modes[i]->h;
            }

            sys_printf("%2i: %ix%i\n", i, modes[i]->w, modes[i]->h);
        }

        if (!w || !h)
        {
            w = modes[0]->w;
            h = modes[0]->h;

            cvar_set(r_mode, "0");
        }
    }

    if (0 != video_set_mode(w, h, r_fullscreen->i))
    {
        esdl_quit();
        return -4;
    }

    video_grab_toggle();

    errno = 0;

    sys_printf("+video\n");

    return 0;
}

/*
=================
video_shutdown
=================
*/
void video_shutdown (void)
{
    if (!video_i)
        return;

    video_i = 0;

    esdl_quit();

    lib_close(libsdl);

    errno = 0;

    sys_printf("-video\n");
}
