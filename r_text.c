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

static void *texts;

/*
=================
r_text_set
=================
*/
bool r_text_set (const char *string, const char *font, void **text)
{
    if (NULL == string || NULL == font || NULL == text)
    {
        sys_printf("bad args (string=%p, font=%p, text=%p)\n",
                   string,
                   font,
                   text);
        return false;
    }

    return true;
}

/*
=================
r_text_delete
=================
*/
void r_text_delete (void *text)
{
    if (NULL == text)
    {
        sys_printf("bad args (text=%p)\n", text);
        return;
    }
}

/*
=================
r_text_draw
=================
*/
void r_text_draw (const void *text)
{
    if (NULL == text)
    {
        sys_printf("bad args (text=%p)\n", text);
        return;
    }
}

/*
=================
r_text_init
=================
*/
bool r_text_init (void)
{
    texts = NULL;

    return true;
}

/*
=================
r_text_shutdown
=================
*/
void r_text_shutdown (void)
{
    while (NULL != texts)
        r_text_delete(texts);
}
