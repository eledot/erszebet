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

#include <SDL.h>

#include "common.h"
#include "event_maps.h"

static int input_grabbed = 1;

extern int (*esdl_poll_event) (SDL_Event *event);

/*
=================
event_frame
=================
*/
void event_frame (void)
{
    SDL_Event ev;
    int       mod, sym;

    while (esdl_poll_event(&ev))
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
            video_event(0, 0, ev.resize.w, ev.resize.h);
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
