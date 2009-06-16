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
#include "engine.h"

#define INIT(a)                                 \
    if (0 != a##_init())                        \
        goto error;

#define SHUTDOWN(a)                             \
        a##_shutdown();

static int engine_quit_flag = 0;

/*
=================
quit_f
=================
*/
static void quit_f (const struct cmd_s *cmd UV, int source UV, int argc UV, const char **argv UV)
{
    engine_quit_flag = 1;
}

/*
=================
echo_f
=================
*/
static void echo_f (const struct cmd_s *cmd UV, int source UV, int argc, const char **argv)
{
    if (argc < 2)
        return;

    sys_printf("%s\n", argv[1]);
}

/*
=================
exec_f
=================
*/
static void exec_f (const struct cmd_s *cmd UV, int source UV, int argc, const char **argv)
{
    if (argc < 2)
        return;

    cmdbuf_add_file(argv[1]);
}

/*
=================
engine_frame
=================
*/
static void engine_frame (void)
{
    sys_get_time();

    snd_frame();
    r_frame();
    video_frame();
    g_frame();
    cmdbuf_frame();
}

/*
=================
engine_run
=================
*/
int engine_run (void)
{
    INIT(cvar);
    INIT(cmd);
    INIT(cmdbuf);

    cmd_register("quit", NULL, &quit_f, 0);
    cmd_register("echo", NULL, &echo_f, 0);

    /*
      put sys args into cmdbuf (which begin with '+')
      before some modules (this affects RO cvars like fs_game)
    */
    sys_arg_to_cmdbuf('+');
    cmdbuf_frame();

    INIT(fs);
    INIT(mouse);
    INIT(keyboard);
    INIT(image);

    INIT(video);
    INIT(gl);
    INIT(r);
    INIT(snd);

    cmd_register("exec", NULL, &exec_f, 0);

    cmdbuf_add("exec config.cfg\n", CMD_SRC_ENGINE);
    cmdbuf_frame();
    cmdbuf_add("exec autoexec.cfg\n", CMD_SRC_ENGINE);
    cmdbuf_frame();

    /*
      put sys args into cmdbuf (which begin with '+')
      once again (after all cmds like 'map' were registered)
    */
    sys_arg_to_cmdbuf('+');
    cmdbuf_frame();

    INIT(g);

    while (!engine_quit_flag)
    {
        engine_frame();
    }

error:
    return engine_stop();
}

/*
=================
engine_stop
=================
*/
int engine_stop (void)
{
    static int stopping = 0;

    if (stopping)
    {
        /* already stopping */
        return 1;
    }

    stopping = 1;

    SHUTDOWN(g);
    SHUTDOWN(snd);
    SHUTDOWN(r);
    SHUTDOWN(gl);
    SHUTDOWN(video);
    SHUTDOWN(mouse);
    SHUTDOWN(keyboard);
    SHUTDOWN(image);
    SHUTDOWN(cvar);
    SHUTDOWN(cmdbuf);
    SHUTDOWN(cmd);
    SHUTDOWN(fs);

    return 0;
}
