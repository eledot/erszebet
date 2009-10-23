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

#if defined(ENGINE_OS_APPLE) || defined(ENGINE_OS_IPHONE)

#include "common.h"
#include "fs/fs_private.h"
#include "fs/fs_helpers_apple.h"

static CFBundleRef cg_main_bundle;

/*
=================
fs_get_resource_path
=================
*/
const char *fs_get_resource_path (const char *filename, mem_pool_t mempool)
{
    char       *buffer;
    CFStringRef path;
    CFURLRef    url;

    if (NULL == (path = CFStringCreateWithCString(NULL, filename, kCFStringEncodingUTF8)))
    {
        sys_printf("CFStringCreateWithCString failed\n");
        return NULL;
    }

    if (NULL == (url = CFBundleCopyResourceURL(cg_main_bundle, path, NULL, NULL)))
    {
        sys_printf("CFBundleCopyResourceURL failed\n");
        CFRelease(path);
        return NULL;
    }

    buffer = mem_alloc_static(MISC_MAX_FILENAME);
    CFURLGetFileSystemRepresentation(url, true, buffer, MISC_MAX_FILENAME);
    CFRelease(path);
    CFRelease(url);

    return buffer;
}

/*
=================
fs_get_url
=================
*/
CFURLRef fs_get_url (const char *name_)
{
    char        name[MISC_MAX_FILENAME * 2];
    CFStringRef filename;
    CFURLRef    url;
    int         i;

    for (i = 0; i < STSIZE(paths) ;i++)
    {
        if (!paths[i].valid)
            continue;

        snprintf(name, sizeof(name), "%s/%s", paths[i].path, name_);

        if (NULL == (filename = CFStringCreateWithCString(NULL, name, kCFStringEncodingUTF8)))
            continue;

        url = CFBundleCopyResourceURL(cg_main_bundle, filename, NULL, NULL);

        CFRelease(filename);

        if (NULL != url)
            return url;
    }

    return NULL;
}

/*
=================
fs_get_data_provider
=================
*/
CGDataProviderRef fs_get_data_provider (const char *name_)
{
    CGDataProviderRef provider;
    char              name[MISC_MAX_FILENAME * 2];
    int               i;

    for (i = 0; i < STSIZE(paths) ;i++)
    {
        if (!paths[i].valid)
            continue;

        snprintf(name, sizeof(name), "%s/%s", paths[i].path, name_);

        if (NULL != (provider = CGDataProviderCreateWithFilename(name)))
            return provider;
    }

    return NULL;
}

/*
=================
fs_helpers_apple_init
=================
*/
bool fs_helpers_apple_init (void)
{
    if (NULL == (cg_main_bundle = CFBundleGetMainBundle()))
    {
        sys_printf("CFBundleGetMainBundle failed\n");
        return false;
    }

    return true;
}

/*
=================
fs_helpers_apple_shutdown
=================
*/
void fs_helpers_apple_shutdown (void)
{
    CFRelease(cg_main_bundle);
}

#else /* !_FS_HELPERS_APPLE_H */

#include "common.h"

bool fs_helpers_apple_init (void) { return true; }
void fs_helpers_apple_shutdown (void) { }

#endif /* _FS_HELPERS_APPLE_H */
