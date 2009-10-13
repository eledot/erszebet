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
#include "video_private.h"

static int keyboard_i = 0;

static mem_pool_t mempool;

typedef struct bind_s
{
    char *cmd;
}bind_t;

static const char *key_names[] =
{
    "",
    "",
    "backspace",
    "tab",
    "delete",
    "enter",
    "pause",
    "escape",
    "space",
    "quote",
    "plus",
    "comma",
    "minus",
    "period",
    "slash",
    "semicolon",
    "equals",
    "leftbracket",
    "backslash",
    "rightbracket",
    "backquote",
    "kp0",
    "kp1",
    "kp2",
    "kp3",
    "kp4",
    "kp5",
    "kp6",
    "kp7",
    "kp8",
    "kp9",
    "kp_period",
    "kp_slash",
    "kp_multiply",
    "kp_minus",
    "kp_plus",
    "kp_enter",
    "kp_equals",
    "up",
    "down",
    "right",
    "left",
    "insert",
    "home",
    "end",
    "pageup",
    "pagedown",
    "f1",
    "f2",
    "f3",
    "f4",
    "f5",
    "f6",
    "f7",
    "f8",
    "f9",
    "f10",
    "f11",
    "f12",
    "f13",
    "f14",
    "f15",
    "rshift",
    "lshift",
    "ralt",
    "lalt",
    "rctrl",
    "lctrl",
    "rmeta",
    "lmeta",
    "rsuper",
    "lsuper",
    "print",
    "break",
    "menu",
    "power",
    "euro",
    "numlock",
    "capslock",
    "scroll"
};

static bind_t key_binds['z' - '0'];
static bind_t key_special_binds[STSIZE(key_names)];

/*
=================
bind_f
=================
*/
static void bind_f (GNUC_UNUSED const struct cmd_s *cmd, GNUC_UNUSED int source, int argc, const char **argv)
{
    bind_t *bind = NULL;

    if (argc < 3)
        return;

    if (strlen(argv[1]) == 1)
    {
        int key = tolower(argv[1][0]);

        if ((key >= '0' && key <= '9') ||
            (key >= 'a' && key <= 'z'))
        {
            key -= '0';
            bind = &key_binds[key];
        }
    }
    else
    {
        int i;

        for (i = 2; i < STSIZE(key_names) ;i++)
        {
            if (!strcmp(key_names[i], argv[1]))
            {
                bind = &key_special_binds[i];
                break;
            }
        }
    }

    if (NULL == bind)
    {
        /* FIXME -- print to console */
        sys_printf("can\'t bind \"%s\"\n", argv[1]);
        return;
    }

    if (NULL != bind->cmd)
    {
        mem_free(bind->cmd);
        bind->cmd = NULL;
    }

    bind->cmd = mem_strdup_static(argv[2]);
}

/*
=================
key_set_repeat
=================
*/
void key_set_repeat (int repeat)
{
#ifdef ENGINE_VIDEO_SDL
    if (repeat)
        SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
    else
        SDL_EnableKeyRepeat(0, 0);
#endif
}

/*
=================
key_event
=================
*/
void key_event (int printable, GNUC_UNUSED int printable_shift, int normal, int mod, int down)
{
    static int repeat = 0;

    if (down)
    {
        if (KEY_ENTER == normal && (mod & KEYMOD_ALT))
        {
            video_fullscreen_toggle();
            return;
        }
        else if (KEY_BACKQUOTE == normal)
        {
            repeat = !repeat;
            key_set_repeat(repeat);
            video_grab_toggle();
            return;
        }
    }

    if (KEY_ALPHANUM == normal)
    {
        if (NULL != key_binds[printable - '0'].cmd)
            cmdbuf_add(key_binds[printable - '0'].cmd, down ? CMD_SRC_KEY_DOWN : CMD_SRC_KEY_UP);
    }
    else if (normal > 0 && normal < STSIZE(key_names))
    {
        if (NULL != key_special_binds[normal].cmd)
            cmdbuf_add(key_special_binds[normal].cmd, down ? CMD_SRC_KEY_DOWN : CMD_SRC_KEY_UP);
    }
}

/*
=================
keyboard_init
=================
*/
int keyboard_init (void)
{
    mem_alloc_static_pool("keyboard", 0);

    memset(key_binds,         0, sizeof(key_binds));
    memset(key_special_binds, 0, sizeof(key_special_binds));

    cmd_register("bind", NULL, &bind_f, 0);

    keyboard_i = 1;
    sys_printf("+keyboard\n");

    return 0;
}

/*
=================
keyboard_shutdown
=================
*/
void keyboard_shutdown (void)
{
    int i;
    fs_file_t f;

    if (!keyboard_i)
        return;

    if (NULL != (f = fs_open("config.cfg", FS_WRONLY, NULL, 1)))
    {
        for (i = 0; i < STSIZE(key_binds) ;i++)
        {
            if (NULL != key_binds[i].cmd)
                fs_printf(f, "bind %c \"%s\"\n", '0' + i, key_binds[i].cmd);
        }

        for (i = 2; i < STSIZE(key_special_binds) ;i++)
        {
            if (NULL != key_special_binds[i].cmd)
                fs_printf(f, "bind %s \"%s\"\n", key_special_binds, key_special_binds[i].cmd);
        }

        fs_close(f);
    }
    else
    {
        sys_printf("failed to save configuration\n");
    }

    mem_free_static_pool();

    sys_printf("-keyboard\n");
}
