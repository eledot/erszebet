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

#define HASH_SIZE ('z' - 'a')
#define MAX_CMDS 256

static bool cmd_i = false;

static cmd_t *cmds[HASH_SIZE];
static mem_pool_t mempool;

/*
=================
cmd_insert
=================
*/
static void cmd_insert (cmd_t *ins)
{
    cmd_t *c, *prev;
    int    first, res;

    first = ins->name[0] - 'a';

    prev = NULL;

    for (c = cmds[first]; NULL != c ;c = c->next)
    {
        res = strcmp(ins->name, c->name);

        if (!res)
            return;

        if (res < 0)
        {
            if (NULL != prev)
            {
                ins->next = prev->next;
                prev->next = ins;
            }
            else
            {
                cmds[first] = ins;
                ins->next = c;
            }

            break;
        }

        prev = c;
    }

    if (NULL != prev)
        prev->next = ins;
    else
        cmds[first] = ins;
}

/*
=================
cmd_find
=================
*/
cmd_t *cmd_find (const char *name)
{
    cmd_t *c;
    int    first, res;

    if (NULL == name || name[0] < 'a' || name[0] > 'z')
    {
        sys_printf("invalid cmd name\n");
        return NULL;
    }

    first = name[0] - 'a';

    for (c = cmds[first]; NULL != c ;c = c->next)
    {
        res = strcmp(name, c->name);

        if (!res)
            return c;

        if (res < 0)
            break;
    }

    return NULL;
}

/*
=================
cmd_register
=================
*/
cmd_t *cmd_register (const char *name, const char *alias, cmd_action_t action, int flags)
{
    cmd_t *c;

    if (NULL != action && NULL != alias)
    {
        sys_printf("both alias and action are set\n");
        return NULL;
    }

    if (!varname_is_valid(name))
        return NULL;

    if (NULL == (c = cmd_find(name)))
    {
        if (NULL == (c = mem_alloc_static(sizeof(*c))))
        {
            sys_printf("not enough memory for cmd \"%s\"\n", name);
            return NULL;
        }

        strlcpy(c->name, name, sizeof(c->name));
        c->flags = flags;

        if (NULL != alias)
            strlcpy(c->alias, alias, sizeof(c->alias));
        else
            c->action = action;

        cmd_insert(c);
    }
    else
    {
        if (NULL != c->action)
        {
            sys_printf("cmd \"%s\" already registered\n", name);
            return NULL;
        }

        strlcpy(c->alias, alias, sizeof(c->alias));
    }

    return c;
}

/*
=================
cmd_init
=================
*/
bool cmd_init (void)
{
    memset(cmds, 0, sizeof(cmds));

    mem_alloc_static_pool("cmd", MAX_CMDS * sizeof(cmd_t));

    cmd_i = true;

    sys_printf("+cmd\n");

    return true;
}

/*
=================
cmd_shutdown
=================
*/
void cmd_shutdown (void)
{
    int    i;
    cmd_t *c;

    if (!cmd_i)
        return;

    for (i = 0; i < STSIZE(cmds) ;i++)
        for (c = cmds[i]; NULL != c ;c = c->next)
            if (NULL != c->delete)
                c->delete(c);

    mem_free_static_pool();

    cmd_i = false;

    sys_printf("-cmd\n");
}
