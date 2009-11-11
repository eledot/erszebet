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

static const char *buttons_name[] =
{
    "",
    "mouse1",
    "mouse2",
    "mouse3",
    "mouse4",
    "mouse5",
    "mouse6",
    "mouse7"
};

static cvar_t *m_sens;
static cvar_t *m_accel;

float cursor_x;
float cursor_y;

/*
=================
mouse_event
=================
*/
void mouse_event (int button, bool is_down, int dx, int dy)
{
    if (button > 0 && button < STSIZE(buttons_name))
    {
        /*
          sys_printf("%s %s\n", buttons_name[button], is_down ? "down" : "up");
        */
    }

    /* FIXME */
    if (video_grabbed && (dx || dy))
    {
        cursor_x += m_sens->f * (float)dx;
        cursor_y -= m_sens->f * (float)dy;

        cursor_x = CLAMP(cursor_x, 0, video_width);
        cursor_y = CLAMP(cursor_y, 0, video_height);
    }

    g_call_func("g_mouse_event", "ibdd", button, is_down, (double)cursor_x, (double)cursor_y);
}

/*
=================
mouse_init
=================
*/
bool mouse_init (void)
{
    cursor_x = cursor_y = 0.0;

    m_sens = cvar_get("m_sens", "3", CVAR_FL_SAVE);
    cvar_set_minmax(m_sens, 0.1f, 25.0f);

    m_accel = cvar_get("m_accel", "0", CVAR_FL_SAVE);
    cvar_set_minmax(m_accel, 0.0f, 25.0f);

    sys_printf("+mouse\n");

    return true;
}

/*
=================
mouse_shutdown
=================
*/
void mouse_shutdown (void)
{
    sys_printf("-mouse\n");
}
