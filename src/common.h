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

#ifndef _COMMON_H
#define _COMMON_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>

#undef assert
#define assert(unused) (void)0

#include "gnuc.h"
#include "misc.h"
#include "mem.h"
#include "cmd.h"
#include "cmdbuf.h"
#include "cvar.h"
#include "fs/fs.h"
#include "fs/fs_helpers.h"
#include "fs/fs_helpers_apple.h"
#include "gl/gl.h"
#include "image/image.h"
#include "input/keyboard.h"
#include "input/mouse.h"
#include "emath.h"
#include "render/r_main.h"
#include "sound/snd.h"
#include "strlcat.h"
#include "strlcpy.h"
#include "sys/sys_arg.h"
#include "sys/sys.h"
#include "video/video.h"
#include "game/g_main.h"

#ifdef __cplusplus
}
#endif

#endif /* !_COMMON_H */
