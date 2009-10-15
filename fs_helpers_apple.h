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

#ifndef _FS_HELPERS_APPLE_H
#define _FS_HELPERS_APPLE_H

#if defined(ENGINE_OS_APPLE) || defined(ENGINE_OS_IPHONE)

#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>

const char *fs_get_resource_path (const char *filename, mem_pool_t mempool);
CFURLRef fs_get_url (const char *name);
CGDataProviderRef fs_get_data_provider (const char *name);

#endif

bool fs_helpers_apple_init (void);
void fs_helpers_apple_shutdown (void);

#endif /* !_FS_HELPERS_APPLE_H */
