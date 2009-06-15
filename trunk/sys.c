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

#include "common.h"

#ifdef ENGINE_SYS_PRINTF

#ifdef ENGINE_SYS_PRINTF_TIMESTAMP
#define FMTTIME "%.5lf "
#define ARGSTIME sys_get_time(),
#else
#define FMTTIME
#define ARGSTIME
#endif

#ifdef ENGINE_SYS_PRINTF_LOCATION
#define FMTLOC "[ %s (%s:%i) ]: "
#define ARGSLOC func, file, line,
#else
#define FMTLOC
#define ARGSLOC
#endif

/*
=================
sys_printf_real
=================
*/
void sys_printf_real (const char *file, int line, const char *func, const char *format, ...)
{
    va_list argptr;
    char    msg[4096];

    va_start(argptr, format);
    vsnprintf(msg, sizeof(msg), format, argptr);
    va_end(argptr);

    if (errno)
    {
        fprintf(stderr, FMTTIME FMTLOC "(%s) %s", ARGSTIME ARGSLOC strerror(errno), msg);
        fflush(stderr);
    }
    else
    {
        fprintf(stderr, FMTTIME FMTLOC "%s", ARGSTIME ARGSLOC msg);
        fflush(stdout);
    }

    errno = 0;
}

#endif /* ENGINE_SYS_PRINTF */
