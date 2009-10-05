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

#ifdef ENGINE_VIDEO_SDL

#include "common.h"
#include "video_private.h"
#include "event_maps.h"

#define DEFAULT_FLAGS (SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_OPENGL)

static int video_i = 0;

static SDL_Surface *surf;
static SDL_Rect   **modes;

static cvar_t *r_mode;
static cvar_t *r_custom_width;
static cvar_t *r_custom_height;
static cvar_t *r_fullscreen;

static int nograb;

int video_orientation;
int video_width;
int video_height;
int video_fullscreen;
int video_grabbed = 0;
double video_aspect;

static int input_grabbed = 1;

/*
=================
video_grab_toggle
=================
*/
void video_grab_toggle (void)
{
    if (nograb)
        return;

    if (!video_grabbed || video_fullscreen)
    {
        SDL_ShowCursor(SDL_DISABLE);
        SDL_WM_GrabInput(SDL_GRAB_ON);
        video_grabbed = 1;
    }
    else
    {
        SDL_ShowCursor(SDL_ENABLE);
        SDL_WM_GrabInput(SDL_GRAB_OFF);
        video_grabbed = 0;
    }

    errno = 0;
}

/*
=================
video_fullscreen_toggle
=================
*/
void video_fullscreen_toggle (void)
{
    video_fullscreen = !video_fullscreen;

    SDL_WM_ToggleFullScreen(surf);

    if (video_fullscreen && !video_grabbed)
        video_grab_toggle();

    errno = 0;
}

/*
=================
video_frame
=================
*/
void video_frame (void)
{
    SDL_Event ev;
    int       mod, sym;

    if (!video_i)
        return;

    SDL_GL_SwapBuffers();

    while (SDL_PollEvent(&ev))
    {
        errno = 0;

        switch (ev.type)
        {
        case SDL_MOUSEMOTION:
            if (!input_grabbed)
                continue;

            mouse_event(0, 0, ev.motion.xrel, ev.motion.yrel);
            break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
            if (!input_grabbed)
                continue;

            mod = 0;

            if (ev.key.keysym.mod & KMOD_SHIFT)
                mod |= KEYMOD_SHIFT;

            if (ev.key.keysym.mod & KMOD_CTRL)
                mod |= KEYMOD_CTRL;

            if (ev.key.keysym.mod & KMOD_ALT)
                mod |= KEYMOD_ALT;

            if (ev.key.keysym.mod & KMOD_META)
                mod |= KEYMOD_META;

            sym = ev.key.keysym.sym;

            key_event(map_print[sym],
                      map_print_shift[sym],
                      map_normal[sym],
                      mod,
                      SDL_PRESSED == ev.key.state);
            break;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            if (!input_grabbed)
                continue;

            mouse_event(ev.button.button, SDL_PRESSED == ev.button.state, 0, 0);
            break;

        case SDL_ACTIVEEVENT:
            if (SDL_APPINPUTFOCUS == ev.active.state || SDL_APPACTIVE == ev.active.state)
                input_grabbed = ev.active.gain;
            break;

        case SDL_VIDEORESIZE:
            video_set_mode(ev.resize.w, ev.resize.h, video_fullscreen);
            break;

        case SDL_QUIT:
            cmdbuf_add("quit\n", CMD_SRC_ENGINE);
            break;

        default:
            break;
        }
    }

    errno = 0;
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

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    if (NULL == (surf = SDL_SetVideoMode(w, h, 32, flags)))
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
    int flags, w = 0, h = 0, i;

    nograb = sys_arg_find("-nograb");

    r_mode = cvar_get("r_mode", "c", CVAR_FL_SAVE);
    r_custom_width = cvar_get("r_custom_width", "480", CVAR_FL_SAVE);
    r_custom_height = cvar_get("r_custom_height", "320", CVAR_FL_SAVE);
    r_fullscreen = cvar_get("r_fullscreen", "0", CVAR_FL_SAVE);

    if (0 != SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE))
    {
        sys_printf("SDL_Init failed\n");
        return -1;
    }

    video_orientation = VIDEO_LANDSCAPE;

    video_i = 1;

    SDL_EventState(SDL_VIDEOEXPOSE, SDL_IGNORE);

    flags = DEFAULT_FLAGS | SDL_FULLSCREEN;

    if (NULL == (modes = SDL_ListModes(NULL, flags)))
    {
        sys_printf("no available video modes\n");
        SDL_Quit();
        return -2;
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
        SDL_Quit();
        return -3;
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

    SDL_Quit();
    errno = 0;

    sys_printf("-video\n");
}

#endif /* ENGINE_VIDEO_SDL */
