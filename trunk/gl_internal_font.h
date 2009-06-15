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

#ifndef _GL_INTERNAL_FONT_H
#define _GL_INTERNAL_FONT_H

0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x08, 0x14, 0x14, 0x08, 0x24, 0x08, 0x08, 0x10, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20,
0x00, 0x08, 0x14, 0x14, 0x1c, 0x2a, 0x14, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20,
0x00, 0x08, 0x00, 0x3e, 0x2a, 0x14, 0x14, 0x00, 0x04, 0x10, 0x14, 0x08, 0x00, 0x00, 0x00, 0x10,
0x00, 0x08, 0x00, 0x14, 0x0a, 0x10, 0x08, 0x00, 0x04, 0x10, 0x08, 0x08, 0x00, 0x00, 0x00, 0x10,
0x00, 0x08, 0x00, 0x14, 0x1c, 0x08, 0x2c, 0x00, 0x04, 0x10, 0x3e, 0x3e, 0x00, 0x3e, 0x00, 0x08,
0x00, 0x00, 0x00, 0x3e, 0x28, 0x28, 0x12, 0x00, 0x04, 0x10, 0x08, 0x08, 0x00, 0x00, 0x00, 0x08,
0x00, 0x08, 0x00, 0x14, 0x2a, 0x54, 0x12, 0x00, 0x08, 0x08, 0x14, 0x08, 0x08, 0x00, 0x08, 0x04,
0x00, 0x08, 0x00, 0x14, 0x1c, 0x24, 0x2c, 0x00, 0x10, 0x04, 0x00, 0x00, 0x08, 0x00, 0x08, 0x04,
0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x1c, 0x08, 0x1c, 0x1c, 0x20, 0x3e, 0x1c, 0x3e, 0x1c, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c,
0x22, 0x0c, 0x22, 0x22, 0x30, 0x02, 0x02, 0x20, 0x22, 0x22, 0x00, 0x00, 0x20, 0x00, 0x04, 0x22,
0x32, 0x08, 0x22, 0x20, 0x28, 0x02, 0x02, 0x20, 0x22, 0x22, 0x08, 0x08, 0x10, 0x3e, 0x08, 0x22,
0x2a, 0x08, 0x10, 0x18, 0x24, 0x1e, 0x1e, 0x10, 0x1c, 0x22, 0x08, 0x08, 0x08, 0x00, 0x10, 0x10,
0x26, 0x08, 0x08, 0x20, 0x22, 0x20, 0x22, 0x10, 0x22, 0x3c, 0x00, 0x00, 0x04, 0x00, 0x20, 0x08,
0x22, 0x08, 0x04, 0x20, 0x3e, 0x20, 0x22, 0x08, 0x22, 0x20, 0x00, 0x00, 0x08, 0x3e, 0x10, 0x00,
0x22, 0x08, 0x02, 0x22, 0x20, 0x22, 0x22, 0x08, 0x22, 0x20, 0x08, 0x08, 0x10, 0x00, 0x08, 0x08,
0x1c, 0x1c, 0x3e, 0x1c, 0x20, 0x1c, 0x1c, 0x08, 0x1c, 0x1c, 0x08, 0x08, 0x20, 0x00, 0x04, 0x08,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x1c, 0x1c, 0x1e, 0x1c, 0x0e, 0x3e, 0x3e, 0x1c, 0x22, 0x1c, 0x38, 0x22, 0x02, 0x22, 0x22, 0x1c,
0x22, 0x22, 0x22, 0x22, 0x12, 0x02, 0x02, 0x22, 0x22, 0x08, 0x10, 0x12, 0x02, 0x36, 0x22, 0x22,
0x32, 0x22, 0x22, 0x02, 0x22, 0x02, 0x02, 0x02, 0x22, 0x08, 0x10, 0x0a, 0x02, 0x2a, 0x26, 0x22,
0x2a, 0x22, 0x1e, 0x02, 0x22, 0x1e, 0x1e, 0x02, 0x3e, 0x08, 0x10, 0x06, 0x02, 0x22, 0x2a, 0x22,
0x2a, 0x3e, 0x22, 0x02, 0x22, 0x02, 0x02, 0x3a, 0x22, 0x08, 0x10, 0x06, 0x02, 0x22, 0x32, 0x22,
0x32, 0x22, 0x22, 0x02, 0x22, 0x02, 0x02, 0x22, 0x22, 0x08, 0x12, 0x0a, 0x02, 0x22, 0x22, 0x22,
0x02, 0x22, 0x22, 0x22, 0x12, 0x02, 0x02, 0x22, 0x22, 0x08, 0x12, 0x12, 0x02, 0x22, 0x22, 0x22,
0x3c, 0x22, 0x1e, 0x1c, 0x0e, 0x3e, 0x02, 0x1c, 0x22, 0x1c, 0x0c, 0x22, 0x3e, 0x22, 0x22, 0x1c,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00,
0x1e, 0x1c, 0x1e, 0x1c, 0x3e, 0x22, 0x22, 0x22, 0x22, 0x22, 0x3e, 0x1c, 0x04, 0x1c, 0x14, 0x00,
0x22, 0x22, 0x22, 0x22, 0x08, 0x22, 0x22, 0x22, 0x22, 0x22, 0x20, 0x04, 0x04, 0x10, 0x22, 0x00,
0x22, 0x22, 0x22, 0x02, 0x08, 0x22, 0x22, 0x22, 0x14, 0x14, 0x10, 0x04, 0x08, 0x10, 0x00, 0x00,
0x22, 0x22, 0x22, 0x1c, 0x08, 0x22, 0x14, 0x22, 0x08, 0x14, 0x08, 0x04, 0x08, 0x10, 0x00, 0x00,
0x1e, 0x22, 0x1e, 0x20, 0x08, 0x22, 0x14, 0x22, 0x08, 0x08, 0x04, 0x04, 0x10, 0x10, 0x00, 0x00,
0x02, 0x22, 0x0a, 0x20, 0x08, 0x22, 0x14, 0x2a, 0x14, 0x08, 0x02, 0x04, 0x10, 0x10, 0x00, 0x00,
0x02, 0x2a, 0x12, 0x22, 0x08, 0x22, 0x08, 0x36, 0x22, 0x08, 0x02, 0x04, 0x20, 0x10, 0x00, 0x00,
0x02, 0x1c, 0x22, 0x1c, 0x08, 0x1c, 0x08, 0x22, 0x22, 0x08, 0x3e, 0x1c, 0x20, 0x1c, 0x00, 0x00,
0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x02, 0x00, 0x20, 0x00, 0x30, 0x00, 0x02, 0x08, 0x20, 0x04, 0x0c, 0x00, 0x00, 0x00,
0x00, 0x00, 0x02, 0x00, 0x20, 0x00, 0x08, 0x00, 0x02, 0x00, 0x00, 0x04, 0x08, 0x00, 0x00, 0x00,
0x00, 0x1c, 0x1e, 0x1c, 0x3c, 0x1c, 0x1c, 0x3c, 0x1e, 0x0c, 0x30, 0x24, 0x08, 0x1e, 0x1e, 0x1c,
0x00, 0x20, 0x22, 0x22, 0x22, 0x22, 0x08, 0x22, 0x22, 0x08, 0x20, 0x14, 0x08, 0x2a, 0x22, 0x22,
0x00, 0x3c, 0x22, 0x02, 0x22, 0x3e, 0x08, 0x22, 0x22, 0x08, 0x20, 0x0c, 0x08, 0x2a, 0x22, 0x22,
0x00, 0x22, 0x22, 0x02, 0x22, 0x02, 0x08, 0x22, 0x22, 0x08, 0x20, 0x0c, 0x08, 0x2a, 0x22, 0x22,
0x00, 0x22, 0x22, 0x22, 0x22, 0x22, 0x08, 0x22, 0x22, 0x08, 0x20, 0x14, 0x08, 0x2a, 0x22, 0x22,
0x00, 0x3c, 0x1e, 0x1c, 0x3c, 0x1c, 0x08, 0x3c, 0x22, 0x1c, 0x20, 0x24, 0x1c, 0x2a, 0x22, 0x1c,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x00,
0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x08, 0x0c, 0x2a, 0x00,
0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x10, 0x12, 0x00,
0x1e, 0x3c, 0x3a, 0x3c, 0x1c, 0x22, 0x22, 0x22, 0x22, 0x22, 0x3e, 0x08, 0x08, 0x10, 0x00, 0x00,
0x22, 0x22, 0x06, 0x02, 0x08, 0x22, 0x22, 0x22, 0x14, 0x22, 0x10, 0x04, 0x08, 0x20, 0x00, 0x00,
0x22, 0x22, 0x02, 0x1c, 0x08, 0x22, 0x14, 0x2a, 0x08, 0x22, 0x08, 0x08, 0x08, 0x10, 0x00, 0x00,
0x22, 0x22, 0x02, 0x20, 0x08, 0x22, 0x14, 0x2a, 0x08, 0x22, 0x04, 0x08, 0x08, 0x10, 0x00, 0x00,
0x22, 0x22, 0x02, 0x20, 0x08, 0x22, 0x08, 0x2a, 0x14, 0x22, 0x02, 0x08, 0x08, 0x10, 0x00, 0x00,
0x1e, 0x3c, 0x02, 0x1e, 0x30, 0x3c, 0x08, 0x1c, 0x22, 0x3c, 0x3e, 0x30, 0x08, 0x0c, 0x00, 0x00,
0x02, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x02, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

#endif /* !_GL_INTERNAL_FONT_H */
