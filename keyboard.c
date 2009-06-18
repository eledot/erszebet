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

#include "common.h"
#include "video_private.h"

static const char *key_names[] =
{
    "",
    "",
    "backspace",
    "tab",
    "delete",
    "enter",
    "pause",
    "escape",
    "space",
    "quote",
    "plus",
    "comma",
    "minus",
    "period",
    "slash",
    "semicolon",
    "equals",
    "leftbracket",
    "backslash",
    "rightbracket",
    "backquote",
    "kp0",
    "kp1",
    "kp2",
    "kp3",
    "kp4",
    "kp5",
    "kp6",
    "kp7",
    "kp8",
    "kp9",
    "kp_period",
    "kp_slash",
    "kp_multiply",
    "kp_minus",
    "kp_plus",
    "kp_enter",
    "kp_equals",
    "up",
    "down",
    "right",
    "left",
    "insert",
    "home",
    "end",
    "pageup",
    "pagedown",
    "f1",
    "f2",
    "f3",
    "f4",
    "f5",
    "f6",
    "f7",
    "f8",
    "f9",
    "f10",
    "f11",
    "f12",
    "f13",
    "f14",
    "f15",
    "rshift",
    "lshift",
    "ralt",
    "lalt",
    "rctrl",
    "lctrl",
    "rmeta",
    "lmeta",
    "rsuper",
    "lsuper",
    "print",
    "break",
    "menu",
    "power",
    "euro",
    "numlock",
    "capslock",
    "scroll"
};

/*
=================
key_set_repeat
=================
*/
void key_set_repeat (int repeat)
{
#ifdef ENGINE_VIDEO_SDL
    if (repeat)
        SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
    else
        SDL_EnableKeyRepeat(0, 0);
#endif
}

/*
=================
key_event
=================
*/
void key_event (int printable, int printable_shift PUV, int normal, int mod, int down)
{
    static int repeat = 0;

    if (KEY_ENTER == normal && (mod & KEYMOD_ALT) && down)
    {
        video_fullscreen_toggle();
    }

    if (KEY_BACKQUOTE == normal && down)
    {
        repeat = !repeat;
        key_set_repeat(repeat);
        video_grab_toggle();
    }

    if (KEY_ALPHANUM == normal)
    {
        sys_printf("normal %c\n", printable);
    }
    else if (normal > 0 && normal < STSIZE(key_names))
    {
        sys_printf("normal %s\n", key_names[normal]);
    }

    if (printable)
    {
        if (mod & KEYMOD_SHIFT)
        {
            sys_printf("printable %c\n", printable_shift);
        }
        else
        {
            sys_printf("printable %c\n", printable);
        }

        if (down)
        {
            if ('q' == printable)
            {
                cmdbuf_add("quit\n", CMD_SRC_USER);
            }
            else if ('s' == printable)
            {
                cmdbuf_add("screenshot_jpeg\n", CMD_SRC_USER);
            }
        }
    }
}

/*
=================
keyboard_init
=================
*/
int keyboard_init (void)
{
    sys_printf("+keyboard\n");

    return 0;
}

/*
=================
keyboard_shutdown
=================
*/
void keyboard_shutdown (void)
{
    sys_printf("-keyboard\n");
}
