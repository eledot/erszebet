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

#ifndef _IMAGE_PVRTC_H
#define _IMAGE_PVRTC_H

bool image_pvrtc_load (const char *name, image_t *im, mem_pool_t pool) GNUC_NONNULL GNUC_WARN_UNUSED_RES;

bool image_pvrtc_init (void);
void image_pvrtc_shutdown (void);


#endif /* !_IMAGE_PVRTC_H */
