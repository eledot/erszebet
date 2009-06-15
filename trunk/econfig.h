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

#ifndef _ECONFIG_H
#define _ECONFIG_H

#define GAME_NAME "erszebet"

#ifdef DEBUG
#undef DEBUG
#undef RELEASE
#define DEBUG   1
#else
#undef DEBUG
#undef RELEASE
#define RELEASE 1
#endif

#define ENGINE_SYS_PRINTF
#define ENGINE_IMAGE_PNG
#define ENGINE_IMAGE_JPEG
#define ENGINE_SND_WAV
#define ENGINE_SND_OGG
#define ENGINE_SND_FLAC

/* debugging */
#ifdef DEBUG
#define ENGINE_SYS_PRINTF_LOCATION
#define ENGINE_MEM_DEBUG
/* #define ENGINE_MEM_FREE_DEBUG_ALLOCS */
#define ENGINE_SND_DEBUG
#define ENGINE_GL_DEBUG
/* #define ENGINE_SYS_PRINTF_TIMESTAMP */
#endif

#endif /* !_ECONFIG_H */
