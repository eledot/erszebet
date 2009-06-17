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

#ifdef ENGINE_OS_APPLE

#include "common.h"
#include "fs_private.h"

static CFBundleRef cg_main_bundle;

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
int fs_helpers_apple_init (void)
{
    if (NULL == (cg_main_bundle = CFBundleGetMainBundle()))
    {
        sys_printf("CFBundleGetMainBundle failed\n");
        return -1;
    }

    return 0;
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

int fs_helpers_apple_init (void) { return 0; }
void fs_helpers_apple_shutdown (void) { }

#endif /* _FS_HELPERS_APPLE_H */
