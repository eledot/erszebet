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

#define MAX_TOKARGS 16

int         tok_argc = 0;
const char *tok_argv[MAX_TOKARGS];

static char tok_buffer[MISC_MAX_TOKLEN];
static char tok_token[MISC_MAX_TOKLEN];
static int  tok_bufferpos = 0;

/*
=================
parse_token
=================
*/
static int parse_token (const char **datapointer)
{
    int         len;
    const char *data = *datapointer;

    len = 0;
    tok_token[0] = 0;

    if (NULL == data)
    {
        *datapointer = NULL;
        return 0;
    }

skipwhite:
    for (; *data <= ' ' ;data++)
    {
        if (!*data)
        {
            *datapointer = NULL;
            return 0;
        }
    }

    if (*data == '/' && data[1] == '/')
    {
        while (*data && *data != '\n' && *data != '\r')
            data++;

        goto skipwhite;
    }
    else if (*data == '\"')
    {
        for (data++; *data != '\"' ;data++)
        {
            if (!*data || len >= (int)sizeof(tok_token) - 1)
            {
                tok_token[0] = 0;
                *datapointer = NULL;

                return 0;
            }

            tok_token[len++] = *data;
        }

        tok_token[len] = 0;
        *datapointer = data + 1;

        return 1;
    }
    else
    {
        for (;*data > ' ';data++)
        {
            if (len >= (int)sizeof(tok_token) - 1)
            {
                tok_token[0] = 0;
                *datapointer = NULL;
                return 0;
            }

            tok_token[len++] = *data;
        }

        tok_token[len] = 0;
        *datapointer = data;

        return 1;
    }
}

/*
=================
string_tokenize
=================
*/
int string_tokenize (const char *text)
{
    int l, k;

    tok_argc = tok_bufferpos = 0;

    for (k = tok_argc + 1; --k >= 0 ;)
        tok_argv[k] = 0;

    while (1)
    {
        while (*text && *text <= ' ' && *text != '\r' && *text != '\n')
            text++;

        if (*text == '\n' || *text == '\r')
        {
            if (*text == '\r' && text[1] == '\n')
                text++;

            text++;
            break;
        }

        if (!*text)
            break;

        if (!parse_token(&text))
            return 0;

        if (tok_argc < MAX_TOKARGS)
        {
            l = strlen(tok_token) + 1;

            if (tok_bufferpos + l > MISC_MAX_TOKLEN)
                break;

            strcpy(tok_buffer + tok_bufferpos, tok_token);

            tok_argv[tok_argc] = tok_buffer + tok_bufferpos;
            tok_bufferpos += l;
            tok_argc++;
        }
    }

    return tok_argc;
}

/*
=================
filename_is_valid
=================
*/
int filename_is_valid (const char *name)
{
    int len, i, d;

    if (NULL == name || name[0] < 'a' || name[0] > 'z')
    {
        sys_printf("invalid filename\n");
        return 0;
    }

    len = strlen(name);

    if (len >= MISC_MAX_FILENAME)
    {
        sys_printf("invalid filename (too long)\n");
        return 0;
    }

    d = 0;

    for (i = 1; i < len ;i++)
    {
        if ((name[i] < 'a' || name[i] > 'z') &&
            (name[i] != '_' && name[i] != '.' && name[i] != '/' &&
             (name[i] < '0' || name[i] > '9')))
        {
            sys_printf("invalid name (contains invalid chars)\n");
            return 0;
        }

        if ('.' == name[i])
        {
            d++;

            if (d > 1)
            {
                sys_printf("invalid name (probably relative path)\n");
                return 0;
            }
        }
        else
        {
            d = 0;
        }
    }

    return 1;
}

/*
=================
varname_is_valid
=================
*/
int varname_is_valid (const char *name)
{
    int len, i;

    if (NULL == name || name[0] < 'a' || name[0] > 'z')
    {
        sys_printf("invalid name\n");
        return 0;
    }

    len = strlen(name);

    if (len >= MISC_MAX_VARNAME)
    {
        sys_printf("invalid name (too long)\n");
        return 0;
    }

    for (i = 1; i < len ;i++)
    {
        if ((name[i] < 'a' || name[i] > 'z') &&
            (name[i] != '_' && (name[i] < '0' || name[i] > '9')))
        {
            sys_printf("invalid name (contains invalid chars)\n");
            return 0;
        }
    }

    return 1;
}
