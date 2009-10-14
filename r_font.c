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

#include "r_private.h"
#include "sglib.h"

static r_font_t *fonts;

/*
=================
r_font_load
=================
*/
int r_font_load (GNUC_UNUSED const char *name, GNUC_UNUSED int first, GNUC_UNUSED r_font_t **font)
{
    goto error;

    return 0;

error:

    return -3;
}

/*
=================
r_font_unload
=================
*/
void r_font_unload (GNUC_UNUSED r_font_t *font)
{
}

/*
=================
r_font_init
=================
*/
int r_font_init (void)
{
    fonts = NULL;

    return 0;
}

/*
=================
r_font_shutdown
=================
*/
void r_font_shutdown (void)
{
    while (NULL != fonts)
        r_font_unload(fonts);
}
