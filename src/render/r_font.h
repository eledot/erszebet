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

#ifndef _R_FONT_H
#define _R_FONT_H

typedef struct r_font_s
{
    const char *name;
    int         ptsize;
}r_font_t;

erbool r_font_load (const char *name, int ptsize, r_font_t **font);
void r_font_unload (r_font_t *font);

void r_font_draw_to_texture (r_font_t *font,
                             const char *text,
                             int *gltex,
                             int *w,
                             int *h,
                             int *texw,
                             int *texh);

erbool r_font_init (void);
void r_font_shutdown (void);

#endif /* !_R_FONT_H */
