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

#ifndef _R_SPRITE_H
#define _R_SPRITE_H

#include "r_texture.h"

typedef struct r_sprite_s
{
    char        *name;
    int          ref;
    int          frames_num;

    struct r_sprite_s *next;
    struct r_sprite_s *prev;

    r_texture_t *frames[0];
}r_sprite_t;

int r_sprite_load (const char *name,
                   int mask,
                   int type,
                   r_sprite_t **sprite);
void r_sprite_unload (r_sprite_t *sprite);

int r_sprite_init (void);
void r_sprite_shutdown (void);

#endif /* !_R_SPRITE_H */
