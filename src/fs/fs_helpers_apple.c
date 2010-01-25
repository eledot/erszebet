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
const char *fs_get_resource_path (const char *filename)
{
    static char path[MISC_MAX_FILENAME];

    snprintf(path, sizeof(path), "%s/%s", paths[1].path, filename);

    return path;
}

/*
=================
fs_get_resource_url
=================
*/
CFURLRef fs_get_resource_url (const char *filename)
{
    CFStringRef cffilename;
    CFURLRef    url;

    if (NULL == (cffilename = CFStringCreateWithCString(NULL, fs_get_resource_path(filename), kCFStringEncodingUTF8)))
        return NULL;

    url = CFBundleCopyResourceURL(cg_main_bundle, cffilename, NULL, NULL);
    CFRelease(cffilename);

    return url;
}

/*
=================
fs_get_data_provider
=================
*/
CGDataProviderRef fs_get_data_provider (const char *filename)
{
    return CGDataProviderCreateWithFilename(fs_get_resource_path(filename));
}

/*
=================
fs_helpers_apple_init
=================
*/
erbool fs_helpers_apple_init (void)
{
    CFURLRef url;

    if (NULL == (cg_main_bundle = CFBundleGetMainBundle()))
    {
        sys_printf("CFBundleGetMainBundle failed\n");
        return false;
    }

    if (NULL == (url = CFBundleCopyResourcesDirectoryURL(cg_main_bundle)))
    {
        sys_printf("CFBundleCopyResourcesDirectoryURL failed\n");
        CFRelease(cg_main_bundle);
        return false;
    }

    CFStringRef home_dir = (CFStringRef)NSHomeDirectory();

    CFStringGetCString(home_dir, paths[0].path, sizeof(paths[0].path), kCFStringEncodingASCII);
    paths[0].valid = 1;
    paths[0].rdonly = 0;

    CFURLGetFileSystemRepresentation(url, true, (unsigned char *)paths[1].path, sizeof(paths[1].path));
    CFRelease(url);
    paths[1].valid = 1;
    paths[1].rdonly = 1;

    paths[2].valid = paths[3].valid = 0;

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

erbool fs_helpers_apple_init (void) { return true; }
void fs_helpers_apple_shutdown (void) { }

#endif /* _FS_HELPERS_APPLE_H */
