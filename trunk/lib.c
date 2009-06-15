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

#include <dlfcn.h>

#include "common.h"

/*
=================
lib_get_funcs
=================
*/
int lib_get_funcs (lib_t lib, lib_func_t *funcs, int ignore_errors)
{
    lib_func_t *f;
    int         failed;

    if (NULL == lib || NULL == funcs)
    {
        sys_printf("bad args (lib=%p, funcs=%p, ignore_errors=%i)\n", lib, funcs, ignore_errors);
        return -1;
    }

    for (failed = 0, f = funcs; NULL != f && NULL != f->name && NULL != f->func ;f++)
    {
        *(f->func) = dlsym(lib, f->name);

        if (NULL == *(f->func))
        {
            failed++;

            if (!ignore_errors)
            {
                sys_printf("\"%s\" not found in library\n", f->name);
                break;
            }
        }
    }

    return failed;
}

/*
=================
lib_load
=================
*/
lib_t lib_open (const char **names, lib_func_t *funcs, int ignore_errors)
{
    const char **n;
    lib_t        lib;

    if (NULL == names)
    {
        sys_printf("NULL library names\n");
        return LIB_HANDLE_INVALID;
    }

    lib = NULL;

    for (n = names; NULL != *n ;n++)
    {
        if (NULL != (lib = dlopen(*n, RTLD_LAZY)))
            break;

/*
        sys_printf("failed to load \"%s\" (%s)\n", *n, dlerror());
*/
    }

    if (NULL == lib)
        return LIB_HANDLE_INVALID;

    if (NULL != funcs)
    {
        if (0 != lib_get_funcs(lib, funcs, ignore_errors) && !ignore_errors)
        {
            dlclose(lib);
            return LIB_HANDLE_INVALID;
        }
    }

    return lib;
}

/*
=================
lib_close
=================
*/
void lib_close (lib_t lib)
{
    if (NULL == lib)
    {
        sys_printf("NULL library\n");
        return;
    }

    dlclose(lib);
}
