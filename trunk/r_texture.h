/*
   Copyright (C) 2009 Serge "ftrvxmtrx" Ziryukin

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General
   Public License along with this library; if not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301 USA
*/

#ifndef _R_TEXTURE_H
#define _R_TEXTURE_H

typedef enum
{
    R_TEX_DEFAULT = 0,
    R_TEX_SCREEN_UI
}r_texture_types_e;

typedef struct r_texture_s
{
    int mask;
    int type;
    int gltex;

    struct r_texture_s *next;
    struct r_texture_s *prev;
}r_texture_t;

int r_texture_load (const char *name, int mask, int type, r_texture_t **tex);
void r_texture_unload (r_texture_t *tex);
void r_texture_mask_unload (int mask);

int r_texture_init (void);
void r_texture_shutdown (void);

#endif /* !_R_TEXTURE_H */
