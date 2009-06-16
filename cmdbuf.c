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

#define CMDBUF_SIZE 32768

static int cmdbuf_i = 0;

static char *cmdbuf;
static int   cmdbuf_len;

static mem_pool_t mempool;

/*
=================
cmdbuf_exec
=================
*/
void cmdbuf_exec (const char *c, int source)
{
    cmd_t  *cmd;
    cvar_t *cvar;

    if (!string_tokenize(c))
        return;

    if (NULL != (cmd = cmd_find(tok_argv[0])))
    {
        if (NULL == cmd->action)
            cmdbuf_exec(cmd->alias, source);
        else
            cmd->action(cmd, source, tok_argc, tok_argv);
    }
    else if (NULL != (cvar = cvar_find(tok_argv[0])) && !(cvar->flags & CVAR_FL_HIDDEN))
    {
        if (tok_argc != 2)
        {
            /* FIXME -- print help message */
            //if (CMD_SRC_USER == source)
            //    sys_printf(cvar->help);
        }
        else if (!(cvar->flags & CVAR_FL_RDONLY))
        {
            cvar_set(cvar, tok_argv[1]);
        }
        else
        {
            if (CMD_SRC_USER == source)
            {
                sys_printf("cvar \"%s\" is read-only\n", cvar->name);
            }
        }
    }
    else
    {
        if (CMD_SRC_USER == source)
        {
            sys_printf("unknown cmd or cvar \"%s\"\n", tok_argv[0]);
        }
    }
}

/*
=================
cmdbuf_add
=================
*/
void cmdbuf_add (const char *c, int source)
{
    int i, len, noend = 0;

    if (NULL == c)
    {
        sys_printf("NULL or empty cmd\n");
        return;
    }

    if (!*c)
        return;

    if (source < 0 || source >= CMD_SRC_INVALID)
    {
        sys_printf("invalid source %i\n", source);
        return;
    }

    len = strlen(c);

    if (cmdbuf_len + len >= CMDBUF_SIZE - 1)
    {
        sys_printf("cmdbuf overflow\n");
        return;
    }

    if (c[len - 1] != '\n' && c[len - 1] != ';')
        noend = 1;

    for (i = 0; i < len ;i++)
    {
        if ((c[i] < ' ' && c[i] != '\n') || c[i] > 'z')
        {
            sys_printf("invalid char (0x%02x) in cmd at %i\n", (uint8_t)c[i], i);
            return;
        }
    }

    cmdbuf[cmdbuf_len] = source + 1;
    strlcpy(cmdbuf + cmdbuf_len + 1, c, CMDBUF_SIZE - cmdbuf_len - 1);
    cmdbuf_len += len + 1;

    if (noend)
    {
        strlcpy(cmdbuf + cmdbuf_len + 1, "\n", CMDBUF_SIZE - cmdbuf_len - 1);
        cmdbuf_len++;
    }
}

/*
=================
cmdbuf_add_file
=================
*/
void cmdbuf_add_file (const char *filename)
{
    int   size;
    void *buf;

    if (NULL == filename || !filename[0])
    {
        sys_printf("NULL filename\n");
        return;
    }

    size = fs_open_read_close(filename, &buf, CMDBUF_SIZE - cmdbuf_len - 1, mempool, 0);

    if (size < 0)
    {
        sys_printf("failed to read \"%s\"\n", filename);
        return;
    }

    if (!size)
        return;

    ((char *)buf)[size - 1] = 0;

    sys_printf("executing \"%s\"\n", filename);
    cmdbuf_add(buf, CMD_SRC_ENGINE);
    mem_free(buf);
}

/*
=================
cmdbuf_frame
=================
*/
void cmdbuf_frame (void)
{
    char *c, *start;
    int   in_quotes = 0, end = 0, source = CMD_SRC_ENGINE;

    if (cmdbuf_len < 1)
        return;

    for (c = start = cmdbuf; *c ;)
    {
        /* set source */
        if (*c >= 1 && *c < CMD_SRC_INVALID + 1)
        {
            source = *c - 1;
            c++;
            start++;
        }

        while (*c)
        {
            if ('\"' == *c)
                in_quotes = !in_quotes;

            if ('\n' == *c || (';' == *c && !in_quotes))
                break;

            c++;
        }

        if (!*c)
            end = 1;

        if (c != start)
        {
            *c = 0;
            cmdbuf_exec(start, source);

            if (end)
                break;
        }

        c++;
        start = c;
    }

    cmdbuf_len = 0;
}

/*
=================
set_f
=================
*/
static void set_f (const struct cmd_s *cmd, int source, int argc, const char **argv)
{
    cvar_t *c;
    int     flags;

    if (argc < 3)
        return;

    if (NULL != (c = cvar_find(argv[1])) && (c->flags & CVAR_FL_RDONLY))
    {
        if (CMD_SRC_USER == source)
        {
            sys_printf("\"%s\" is read-only\n", argv[1]);
        }

        return;
    }

    if ('a' == cmd->name[0])
        flags = CVAR_FL_SAVE;
    else
        flags = 0;

    cvar_get(argv[1], argv[2], flags);
}

/*
=================
cmdbuf_init
=================
*/
int cmdbuf_init (void)
{
    mem_alloc_static_pool("cmdbuf", CMDBUF_SIZE * 2);

    if (NULL == (cmdbuf = mem_alloc_static(CMDBUF_SIZE)))
    {
        sys_printf("failed to allocate %i bytes for cmd buffer\n", CMDBUF_SIZE);
        return -1;
    }

    cmd_register("set",  NULL, &set_f, 0); /* normal set              */
    cmd_register("aset", NULL, &set_f, 0); /* set with CVAR_FL_SAVE   */

    cmdbuf_i = 1;

    sys_printf("+cmdbuf\n");

    return 0;
}

/*
=================
cmdbuf_shutdown
=================
*/
void cmdbuf_shutdown (void)
{
    if (!cmdbuf_i)
        return;

    mem_free_static_pool();

    cmdbuf_i = 0;

    sys_printf("-cmdbuf\n");
}
