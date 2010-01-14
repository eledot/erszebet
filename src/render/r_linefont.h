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

#ifndef _R_LINEFONT_H
#define _R_LINEFONT_H

#include "render/r_sprite.h"

typedef struct r_linefont_s
{
    r_sprite_t *sprite;
    char       *chars;
}r_linefont_t;

bool r_linefont_load (const char *name, const char *chars, r_linefont_t **font);
void r_linefont_unload (r_linefont_t *font);

void r_linefont_draw (r_linefont_t *font, const char *text);
bool r_linefont_init (void);
void r_linefont_shutdown (void);

#endif /* !_R_LINEFONT_H */
