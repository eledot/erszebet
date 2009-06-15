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

static int    argc = 0;
static char **argv = NULL;

/*
=================
sys_arg_find
=================
*/
int sys_arg_find (const char *name)
{
    int i;

    if (NULL == name)
    {
        sys_printf("bad args (name=%p)\n", name);
        return 0;
    }

    if (NULL == argv)
        return 0;

    for (i = 1; i < argc && NULL != argv[i] ;i++)
        if (!strcmp(name, argv[i]))
            return i;

    return 0;
}

/*
=================
sys_arg_to_cmdbuf
=================
*/
void sys_arg_to_cmdbuf (char start)
{
    char tmp[MISC_MAX_TOKLEN];
    int  i, adding = 0;

    if (NULL == argv)
        return;

    for (i = 1; i < argc && NULL != argv[i] ;i++)
    {
        if (adding && ('-' == argv[i][0] || start == argv[i][0]))
        {
            adding = 0;

            strlcat(tmp, "\n", sizeof(tmp));
            cmdbuf_add(tmp, CMD_SRC_ENGINE);
        }
        else if (adding)
        {
            strlcat(tmp, " ", sizeof(tmp));
            strlcat(tmp, argv[i], sizeof(tmp));
        }

        if (start == argv[i][0] && argv[i][1])
        {
            strlcpy(tmp, argv[i] + 1, sizeof(tmp));
            adding = 1;
        }
    }

    if (adding)
    {
        strlcat(tmp, "\n", sizeof(tmp));
        cmdbuf_add(tmp, CMD_SRC_ENGINE);
    }
}

/*
=================
sys_arg_set
=================
*/
void sys_arg_set (int c, char **v)
{
    argc = c;
    argv = v;
}
