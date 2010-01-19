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

#ifndef _CMD_H
#define _CMD_H

#define CMD_MAX_ALIAS 128

typedef enum
{
    CMD_FL_SERVER = (1 << 0),  /**< this is server-side cmd    */
    CMD_FL_THREAD = (1 << 1),  /**< execute in separate thread */
    CMD_FL_CHEATS = (1 << 2)   /**< this is a cheat cmd        */
}cmd_flags_e;

typedef enum
{
    CMD_SRC_KEY_DOWN = 0,
    CMD_SRC_KEY_UP,
    CMD_SRC_CONSOLE,
    CMD_SRC_ENGINE,
    CMD_SRC_GAME,
    CMD_SRC_REMOTE,

    CMD_SRC_INVALID
}cmd_sources_e;

struct cmd_s;

typedef void (*cmd_action_t) (const struct cmd_s *cmd, int source, int argc, const char **argv);
typedef void (*cmd_delete_t) (struct cmd_s *cmd);

typedef struct cmd_s
{
    char name[MISC_MAX_VARNAME];
    char alias[CMD_MAX_ALIAS];

    cmd_action_t action;
    cmd_delete_t delete;

    int flags;
    int userdata;

    struct cmd_s *next;
}cmd_t;

cmd_t *cmd_find (const char *name)
    GNUC_NONNULL GNUC_WARN_UNUSED_RES;
cmd_t *cmd_register (const char *name, const char *alias, cmd_action_t action, int flags)
    GNUC_NONNULL_ARGS(1, 3);

erbool cmd_init (void);
void cmd_shutdown (void);

#endif /* !_CMD_H */
