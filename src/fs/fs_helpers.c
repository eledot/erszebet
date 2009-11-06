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

#include "common.h"

/*
=================
fs_open_read_close
=================
*/
int fs_open_read_close (const char *name, void **buffer, int max_size, mem_pool_t pool, int shout)
{
    int        size;
    fs_file_t *f;

    if (NULL == buffer)
    {
        sys_printf("NULL buffer\n");
        return -1;
    }

    if (NULL == (f = fs_open(name, FS_RDONLY, &size, shout)))
        return -1;

    if (size < 1 || !max_size)
    {
        fs_close(f);
        return 0;
    }

    if (max_size > 0)
        size = MIN(max_size, size);

    if (NULL == (*buffer = mem_alloc(pool, size)))
    {
        sys_printf("failed to allocate %i bytes to read \"%s\"\n", size, name);
        fs_close(f);
        return -1;
    }

    size = fs_read(f, *buffer, size);

    fs_close(f);

    return size;
}

/*
=================
fs_open_write_close
=================
*/
int fs_open_write_close (const char *name, const void *buffer, int size, int shout)
{
    fs_file_t *f;

    if (NULL == buffer)
    {
        sys_printf("NULL buffer\n");
        return -1;
    }

    if (NULL == (f = fs_open(name, FS_WRONLY, NULL, shout)))
        return -1;

    if (size < 1)
    {
        fs_close(f);
        return 0;
    }

    size = fs_write(f, buffer, size);

    fs_close(f);

    return size;
}

/*
=================
fs_get_size
=================
*/
int fs_get_size (fs_file_t f)
{
    int pos, size;

    if (-1 == (pos = fs_tell(f)))
        return -1;

    if (-1 == (size = fs_seek(f, 0, FS_END)))
        return -1;

    if (pos != fs_seek(f, pos, FS_START))
        return -1;

    return size;
}

/*
=================
fs_eof
=================
*/
int fs_eof (fs_file_t f)
{
    int  res;
    char tmp;

    if (-1 == (res = fs_read(f, &tmp, 1)))
        return -1;
    else if (!res)
        return 1;

    fs_seek(f, -1, FS_CURRENT);

    return 0;
}

/*
=================
fs_file_exists
=================
*/
int fs_file_exists (const char *name)
{
    fs_file_t *f;

    if (NULL == (f = fs_open(name, FS_RDONLY, NULL, 0)))
        return 0;

    fs_close(f);

    return 1;
}

/*
=================
fs_printf
=================
*/
int fs_printf (fs_file_t f, const char *format, ...)
{
    va_list argptr;
    char    msg[512];

    va_start(argptr, format);
    vsnprintf(msg, sizeof(msg), format, argptr);
    va_end(argptr);

    return fs_write(f, msg, strlen(msg));
}
