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

#include "r_private.h"

#define MAX_FRAMES 64

static r_sprite_t *sprites;

/*
=================
r_sprite_load
=================
*/
int r_sprite_load (const char *name,
                   int mask,
                   int type,
                   r_sprite_t **sprite)
{
    char tmp[MISC_MAX_FILENAME];
    int i, nlen, num, size;
    r_sprite_t *s;
    void *data;
    char *d;
    const char *frames_names[MAX_FRAMES];

    if (NULL == name || NULL == frames_names || NULL == sprite)
    {
        sys_printf("bad args (name=%p, mask=%i, type=%i, sprite=%p)\n",
                   name,
                   mask,
                   type,
                   sprite);
        return -1;
    }

    /* check if it was loaded earlier */
    for (s = sprites; NULL != s ;s = s->next)
    {
        if (!strcmp(s->name, name))
        {
            s->ref++;
            *sprite = s;
            return 0;
        }
    }

    snprintf(tmp, sizeof(tmp), "spr/%s.txt", name);

    if (1 > (size = fs_open_read_close(tmp, &data, 4096, mempool, 1)))
    {
        sys_printf("sprite \"%s\" not found\n", name);
        return -2;
    }

    frames_names[0] = data;

    for (i = 0, d = data, num = 1; num < MAX_FRAMES && i < size ;d++, i++)
    {
        while (*d == '\n' || *d == ' ' || *d == '\t')
        {
            *d++ = 0;
            i++;
        }

        if (i >= size)
            break;

        if (i > 0 && !*(d - 1))
            frames_names[num++] = d;
    }

    nlen = strlen(name) + 1;

    if (NULL == (s = mem_alloc_static(sizeof(r_sprite_t) + num * sizeof(r_texture_t *) + nlen)))
    {
        mem_free(data);
        return -3;
    }

    for (i = 0; i < num ;i++)
    {
        if (0 != r_texture_load(frames_names[i], mask, type, &s->frames[i]))
        {
            sys_printf("failed to load frame %i (\"%s\")\n", i, frames_names[i]);

            for (i--; i >= 0 ;i--)
                r_texture_unload(s->frames[i]);

            mem_free(data);
            mem_free(s);
            return -4;
        }
    }

    s->name = (char *)s + sizeof(r_sprite_t) + num * sizeof(r_texture_t *);
    strlcpy(s->name, name, nlen);
    s->ref = 1;
    s->frames_num = num;
    s->next = sprites;

    *sprite = s;

    if (NULL != sprites)
        sprites->prev = s;

    mem_free(data);

    return 0;
}

/*
=================
r_sprite_unload
=================
*/
void r_sprite_unload (r_sprite_t *sprite)
{
    if (NULL == sprite)
    {
        sys_printf("NULL sprite\n");
        return;
    }

    /* internal sprites shouldn't be unloaded */
    if (!sprite->frames[0]->mask)
        return;

    /* check the reference counter */
    if (--sprite->ref > 0)
        return;

    if (NULL != sprite->next)
        sprite->next->prev = sprite->prev;

    if (NULL != sprite->prev)
        sprite->prev->next = sprite->next;
    else
        sprites = sprite->next;

    mem_free(sprite);
}

/*
=================
r_sprite_init
=================
*/
int r_sprite_init (void)
{
    sprites = NULL;

    return 0;
}

/*
=================
r_sprite_shutdown
=================
*/
void r_sprite_shutdown (void)
{
    while (NULL != sprites)
        r_sprite_unload(sprites);
}
