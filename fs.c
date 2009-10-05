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

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "common.h"
#include "fs_private.h"

#define MAX_FILES 64

typedef struct file_s
{
    char name[MISC_MAX_FILENAME];
    int  mode;
    int  f;
    int  size;

    struct file_s *next;
    struct file_s *prev;
}file_t;

static int fs_i = 0;

static file_t *files;
static mem_pool_t mempool;

static cvar_t *fs_base;
static cvar_t *fs_home;
static cvar_t *fs_game;

path_t paths[FS_MAX_PATHS];

/*
=================
fs_open
=================
*/
fs_file_t fs_open (const char *name_, int mode, int *size, int shout)
{
    int         i, m, f;
    struct stat st;
    file_t     *file;
    char        name[MISC_MAX_FILENAME * 2];

    if (!filename_is_valid(name_))
        return NULL;

    switch (mode)
    {
    case FS_RDONLY:
        m = O_RDONLY;
        break;

    case FS_WRONLY:
        m = O_CREAT | O_TRUNC | O_WRONLY;
        break;

    case FS_APPEND:
        m = O_CREAT | O_APPEND | O_WRONLY;
        break;

    case FS_RDWR:
        m = O_RDWR;
        break;

    default:
        sys_printf("invalid mode (%i)\n", mode);
        return NULL;
    }

    file = NULL;

    for (i = 0; i < STSIZE(paths) ;i++)
    {
        if (!paths[i].valid)
            continue;

        if (paths[i].rdonly && FS_RDONLY != mode)
            continue;

        snprintf(name, sizeof(name), "%s/%s", paths[i].path, name_);

        if (-1 == (f = open(name, m, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH)))
            continue;

        if (-1 == fstat(f, &st))
        {
            if (shout)
            {
                sys_printf("fstat failed on \"%s\"\n", name);
            }

            continue;
        }

        if (!S_ISREG(st.st_mode))
        {
            if (shout)
            {
                sys_printf("not a regular file \"%s\"\n", name);
            }

            continue;
        }

        if (NULL != size)
            *size = st.st_size;

        if (NULL == (file = mem_alloc_static(sizeof(*file))))
        {
            sys_printf("not enough memory for file structure (\"%s\")\n", name);
            continue;
        }

        strlcpy(file->name, name, sizeof(file->name));

        file->mode = mode;
        file->size = st.st_size;
        file->f    = f;
        file->next = files;

        if (NULL != files)
            files->prev = file;

        files = file;

        break;
    }

    errno = 0;

    if (NULL == file && shout)
    {
        sys_printf("failed to open \"%s\"\n", name_);
    }

    return file;
}

/*
=================
fs_close
=================
*/
void fs_close (fs_file_t f)
{
    file_t *file = f;

    if (NULL == file)
    {
        sys_printf("NULL file\n");
        return;
    }

    if (-1 == close(file->f))
    {
        sys_printf("error while closing \"%s\"\n", file->name);
    }

    if (NULL != file->next)
        file->next->prev = file->prev;

    if (NULL != file->prev)
        file->prev->next = file->next;

    if (files == file)
        files = file->next;

    mem_free(file);
}

/*
=================
fs_read
=================
*/
int fs_read (fs_file_t f, void *buffer, int size)
{
    file_t *file = f;

    if (NULL == file || NULL == buffer || size < 1)
    {
        sys_printf("bad args (f=%p, buffer=%p, size=%i)\n", f, buffer, size);
        return -1;
    }

    return (int)read(file->f, buffer, size);
}

/*
=================
fs_write
=================
*/
int fs_write (fs_file_t f, const void *buffer, int size)
{
    file_t *file = f;

    if (NULL == file || NULL == buffer || size < 1)
    {
        sys_printf("bad args (f=%p, buffer=%p, size=%i)\n", f, buffer, size);
        return -1;
    }

    return (int)write(file->f, buffer, size);
}

/*
=================
fs_seek
=================
*/
int fs_seek (fs_file_t f, int offset, int origin)
{
    int     or;
    file_t *file = f;

    if (NULL == file)
    {
        sys_printf("NULL file\n");
        return -1;
    }

    switch (origin)
    {
    case FS_CURRENT:
        or = SEEK_CUR;
        break;

    case FS_START:
        or = SEEK_SET;
        break;

    case FS_END:
        or = SEEK_END;
        break;

    default:
        sys_printf("invalid origin (%i)\n", origin);
        return -1;
    }

    return (int)lseek(file->f, offset, or);
}

/*
=================
fs_tell
=================
*/
int fs_tell (fs_file_t f)
{
    file_t *file = f;

    if (NULL == file)
    {
        sys_printf("NULL file\n");
        return -1;
    }

    return (int)lseek(file->f, 0, SEEK_CUR);
}

/*
=================
fs_mkdir
=================
*/
int fs_mkdir (const char *name)
{
    int  i;
    char tmp[MISC_MAX_FILENAME * 2];

    if (!filename_is_valid(name))
        return -1;

    for (i = 0; i < STSIZE(paths) ;i++)
    {
        if (paths[i].valid && !paths[i].rdonly)
        {
            snprintf(tmp, sizeof(tmp), "%s/%s", paths[i].path, name);
            return sys_mkdir(tmp);
        }
    }

    return -1;
}

/*
=================
fs_unlink
=================
*/
int fs_unlink (const char *name)
{
    int  i;
    char tmp[MISC_MAX_FILENAME * 2];

    if (!filename_is_valid(name))
        return -1;

    for (i = 0; i < STSIZE(paths) ;i++)
    {
        if (paths[i].valid && !paths[i].rdonly)
        {
            snprintf(tmp, sizeof(tmp), "%s/%s", paths[i].path, name);
            return sys_unlink(tmp);
        }
    }

    return -1;
}

/*
=================
fs_add_paths
=================
*/
static void fs_add_paths (const char *base, const char *home)
{
    unsigned int size;
    path_t      *p;

    memset(paths, 0, sizeof(paths));
    size = sizeof(paths[0].path);

#define ADDPATH(i, d1, d2, rd)                                          \
    {                                                                   \
        p = &paths[i];                                                  \
        p->valid = 1;                                                   \
        snprintf(p->path, size, "%s/%s", d1, d2);                       \
        p->rdonly = rd;                                                 \
        if (!rd)                                                        \
            sys_mkdir(p->path);                                         \
        sys_printf("added \"%s\" to paths\n", p->path);                 \
    }

    ADDPATH(0, home, fs_game->s, 0);
    ADDPATH(1, base, fs_game->s, 1);

    if (!strcmp(FS_DEFAULT_GAME, fs_game->s))
        return;

    ADDPATH(2, home, FS_DEFAULT_GAME, 1);
    ADDPATH(3, base, FS_DEFAULT_GAME, 1);
}

/*
=================
fs_init
=================
*/
int fs_init (void)
{
    const char *home, *s;
    char        tmp[MISC_MAX_FILENAME];

    if (0 != fs_helpers_apple_init())
    {
        sys_printf("fs_helpers_apple_init failed\n");
        return -1;
    }

    if (NULL == (home = getenv("HOME")) || !home[0])
    {
        sys_printf("$HOME env var hasn\'t been set\n");
        return -2;
    }

    snprintf(tmp, sizeof(tmp), "%s/%s", home, FS_HOME_BASE);

    files = NULL;

    fs_base = cvar_get("fs_base", FS_DEFAULT_BASE, CVAR_FL_RDONLY);
    fs_home = cvar_get("fs_home", tmp, CVAR_FL_RDONLY);
    fs_game = cvar_get("fs_game", FS_DEFAULT_GAME, CVAR_FL_RDONLY);

    for (s = fs_game->s; *s ;s++)
    {
        if (!(*s >= '0' && *s <= '9') && !(*s >= 'a' && *s <= 'z') && *s != '_')
        {
            sys_printf("%s contains invalid char\n", fs_game->name);
            return -3;
        }
    }

    if (!fs_home->s[0])
    {
        sys_printf("fs_home cvar is empty\n");
        return -4;
    }

    fs_add_paths(fs_base->s, fs_home->s);

    mem_alloc_static_pool("fs", MAX_FILES * sizeof(file_t));

    fs_i = 1;

    sys_printf("+fs\n");

    return 0;
}

/*
=================
fs_shutdown
=================
*/
void fs_shutdown (void)
{
    file_t *f;

    if (!fs_i)
        return;

    fs_helpers_apple_shutdown();

    for (f = files; NULL != f ;f = f->next)
    {
        if (-1 == close(f->f))
        {
            sys_printf("error while closing \"%s\"\n", f->name);
        }
    }

    mem_free_static_pool();

    fs_i = 0;

    sys_printf("-fs\n");
}
