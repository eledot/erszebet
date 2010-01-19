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

#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/stat.h>

#include "common.h"
#include "engine.h"

double sys_time = 0.0;
static double oldtime = 0.0;

/*
=================
init_time
=================
*/
void init_time (void)
{
    struct timeval tp;

    /* start from 0.0 */
    gettimeofday(&tp, NULL);
    oldtime = (double)tp.tv_sec + tp.tv_usec / 1000000.0;
    srand(tp.tv_sec + tp.tv_usec);
}

/*
=================
sys_get_time
=================
*/
double sys_get_time (void)
{
    double         newtime;
    struct timeval tp;

    gettimeofday(&tp, NULL);
    newtime = (double)tp.tv_sec + tp.tv_usec / 1000000.0;

    if (newtime > oldtime)
    {
        sys_time += newtime - oldtime;
        oldtime = newtime;
    }

    sys_time += newtime - oldtime;
    oldtime = newtime;

    return sys_time;
}

/*
=================
sys_mkdir
=================
*/
erbool sys_mkdir (char *path)
{
    int i;

#define MKDIRMODE (S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH)

    if (NULL == path || !path[0])
    {
        sys_printf("bad pathname\n");
        return false;
    }

    for (i = 1; path[i] ;i++)
    {
        if ('/' == path[i])
        {
            path[i] = 0;

            if (-1 == mkdir(path, MKDIRMODE) && errno != EEXIST)
            {
                sys_printf("mkdir \"%s\" failed\n", path);
                return false;
            }

            path[i] = '/';
        }
    }

    if (-1 == mkdir(path, MKDIRMODE) && errno != EEXIST)
    {
        sys_printf("mkdir \"%s\" failed\n", path);
        return false;
    }

    errno = 0;

    return true;
}

/*
=================
sys_unlink
=================
*/
erbool sys_unlink (const char *filename)
{
    if (NULL == filename || !filename[0])
    {
        sys_printf("bad pathname\n");
        return false;
    }

    if (-1 != unlink(filename) || ENOENT == errno)
        return true;

    if (-1 != rmdir(filename) || ENOENT == errno)
        return true;

    return false;
}

/*
=================
signal_handler
=================
*/
static void signal_handler (GNUC_UNUSED int sig)
{
    static int in_signal = 0;

    if (in_signal)
    {
        /* something bad happened */
        exit(2);
    }

    in_signal = 1;

    sys_printf("caught signal %i\n", sig);

    /* set up us a bomb */
    signal(SIGALRM, signal_handler);
    alarm(5);

    engine_stop();
    exit(1);
}

/*
=================
grab_signals
=================
*/
static void grab_signals (void)
{
    int i;
    int signals[] = { SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGABRT, SIGSEGV };

    for (i = 0; i < STSIZE(signals) ;i++)
        signal(signals[i], signal_handler);
}

/*
=================
main
=================
*/
#ifndef ENGINE_OS_IPHONE
#ifndef ENGINE_OS_APPLE
#define SDL_main main
#endif /* !ENGINE_OS_APPLE */
int SDL_main (int argc, char **argv)
{
    int res;

    errno = 0;

    signal(SIGFPE, SIG_IGN);
    sys_arg_set(argc, argv);

    if (!sys_arg_find("-nocatch"))
        grab_signals();

    sys_printf("starting engine...\n");

    if ((res = engine_start()))
    {
        while (engine_frame());

        engine_stop();
    }

    sys_printf("exiting...\n");

    return res;
}
#endif /* !ENGINE_OS_IPHONE */
