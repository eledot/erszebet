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

typedef enum
{
    R_SPRITE_TYPE_TEXTURES = 0,
    R_SPRITE_TYPE_LINE
}r_sprite_type_e;

typedef enum
{
    SPRITE_ALIGN_LEFT   = (1 << 0),
    SPRITE_ALIGN_RIGHT  = (1 << 1),
    SPRITE_ALIGN_TOP    = (1 << 2),
    SPRITE_ALIGN_BOTTOM = (1 << 3)
}r_sprite_align_e;

typedef struct r_sprite_s
{
    const char *name;

    int   ref;
    int   frames_num;
    int   align;
    int   type;
    float inc;

    struct r_sprite_s *next;

    r_texture_t *frames[0];
}r_sprite_t;

bool r_sprite_load (const char  *name,
                    int          type,
                    r_sprite_t **sprite) GNUC_NONNULL GNUC_WARN_UNUSED_RES;
void r_sprite_unload (r_sprite_t *sprite) GNUC_NONNULL;

void r_sprite_draw (const r_sprite_t *sprite,
                    int frame,
                    double centerx,
                    double centery,
                    double width,
                    double height,
                    double scale,
                    double angle) GNUC_NONNULL;

bool r_sprite_init (void);
void r_sprite_shutdown (void);

#endif /* !_R_SPRITE_H */
