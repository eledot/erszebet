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

#ifndef _CMDBUF_H
#define _CMDBUF_H

void cmdbuf_exec (const char *c, int source) GNUC_NONNULL;
void cmdbuf_add (const char *c, int source) GNUC_NONNULL;
void cmdbuf_add_file (const char *filename) GNUC_NONNULL;

void cmdbuf_frame (void);

erbool cmdbuf_init (void);
void cmdbuf_shutdown (void);

#endif /* !_CMDBUF_H */
