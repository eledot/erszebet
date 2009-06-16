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
#define MAX_CVARS 256

static int cvar_i = 0;

static cvar_t *cvars[HASH_SIZE];
static mem_pool_t mempool;

/*
=================
cvar_insert
=================
*/
static void cvar_insert (cvar_t *ins)
{
    cvar_t *c, *prev;
    int     first, res;

    first = ins->name[0] - 'a';

    prev = NULL;

    for (c = cvars[first]; NULL != c ;c = c->next)
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
                cvars[first] = ins;
                ins->next = c;
            }

            break;
        }

        prev = c;
    }

    if (NULL != prev)
        prev->next = ins;
    else
        cvars[first] = ins;
}

/*
=================
cvar_find
=================
*/
cvar_t *cvar_find (const char *name)
{
    cvar_t *c;
    int     first, res;

    if (NULL == name || name[0] < 'a' || name[0] > 'z')
    {
        sys_printf("invalid cvar name\n");
        return NULL;
    }

    first = name[0] - 'a';

    for (c = cvars[first]; NULL != c ;c = c->next)
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
cvar_set
=================
*/
void cvar_set (cvar_t *c, const char *str)
{
    if (NULL == c)
        return;

    if (NULL == str)
    {
        sys_printf("NULL value\n");
        return;
    }

    strlcpy(c->s, str, sizeof(c->s));

    errno = 0;
    c->f = strtof(str, NULL);

    if (errno)
        c->f = 0;

    errno = 0;
    c->i = strtol(str, NULL, 0);

    if (errno)
        c->i = 0;

    if (c->flags & CVAR_FL_MIN)
        c->f = MAX(c->f, c->min);

    if (c->flags & CVAR_FL_MAX)
        c->f = MIN(c->f, c->max);

    if (NULL != c->callback)
        c->callback(c);
}

/*
=================
cvar_set_min
=================
*/
void cvar_set_min (cvar_t *c, float min)
{
    if (NULL == c)
    {
        sys_printf("NULL cvar\n");
        return;
    }

    c->flags |= CVAR_FL_MIN;
    c->min = min;

    if (c->f < min)
        cvar_set(c, c->s);
}

/*
=================
cvar_set_max
=================
*/
void cvar_set_max (cvar_t *c, float max)
{
    if (NULL == c)
    {
        sys_printf("NULL cvar\n");
        return;
    }

    c->flags |= CVAR_FL_MAX;
    c->max = max;

    if (c->f > max)
        cvar_set(c, c->s);
}

/*
=================
cvar_get
=================
*/
cvar_t *cvar_get (const char *name, const char *str, int flags)
{
    cvar_t *c;

    if (NULL == str)
    {
        sys_printf("NULL value\n");
        return NULL;
    }

    if (!varname_is_valid(name))
        return NULL;

    if (NULL == (c = cvar_find(name)))
    {
        if (NULL == (c = mem_alloc_static(sizeof(*c))))
        {
            sys_printf("not enough memory for cvar \"%s\"\n", name);
            return NULL;
        }

        strlcpy(c->name, name, sizeof(c->name));
        c->flags = flags;

        cvar_set(c, str);

        cvar_insert(c);
    }
    else
    {
        c->flags = flags;

        cvar_set(c, c->s);
    }

    return c;
}

/*
=================
cvar_init
=================
*/
int cvar_init (void)
{
    memset(cvars, 0, sizeof(cvars));

    mem_alloc_static_pool("cvar", MAX_CVARS * sizeof(cvar_t));

    cvar_i = 1;

    sys_printf("+cvar\n");

    return 0;
}

/*
=================
cvar_shutdown
=================
*/
void cvar_shutdown (void)
{
    int       i;
    cvar_t   *cv;
    fs_file_t f;

    if (!cvar_i)
        return;

    if (NULL != (f = fs_open("config.cfg", FS_WRONLY, NULL, 1)))
    {
        for (i = 0; i < HASH_SIZE ;i++)
        {
            for (cv = cvars[i]; NULL != cv ;cv = cv->next)
            {
                if (cv->flags & CVAR_FL_SAVE)
                {
                    fs_printf(f, "%s \"%s\"\n", cv->name, cv->s);
                }
            }
        }

        fs_close(f);
    }
    else
    {
        sys_printf("failed to save configuration\n");
    }

    mem_free_static_pool();

    cvar_i = 0;

    sys_printf("-cvar\n");
}
