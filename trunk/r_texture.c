/*
   Copyright (C) 2009 Serge "ftrvxmtrx" Ziryukin

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General
   Public License along with this library; if not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301 USA
*/

#include "r_private.h"
#include "gl_texture.h"

/* FIXME -- 'notexture' with mask=0 in r_texture_load on fail */

static const int types[] =
{
    0,
    GL_TEX_FL_UI
};

static r_texture_t *textures;

/*
=================
r_texture_load
=================
*/
int r_texture_load (const char *name, int mask, int type, r_texture_t **tex)
{
    int     gltex;
    char    tmp[MISC_MAX_FILENAME];
    image_t image;

    if (NULL == name || type < 0 || type >= STSIZE(types) || NULL == tex)
    {
        sys_printf("bad args (name=%p, tex=%p)\n");
        return -1;
    }

    *tex = NULL;

    if (!filename_is_valid(name))
        return -2;

    snprintf(tmp, sizeof(tmp), "tex/%s", name);

    if (0 != image_load(tmp, &image))
        return -3;

    if (0 != gl_texture_create(&image, types[type], &gltex))
        return -4;

    mem_free(image.data);

    if (NULL == (*tex = mem_alloc_static(sizeof(r_texture_t))))
    {
        gl_texture_delete(gltex);
        return -5;
    }

    (*tex)->mask  = mask;
    (*tex)->type  = type;
    (*tex)->gltex = gltex;
    (*tex)->next  = textures;

    if (NULL != textures)
        textures->prev = *tex;

    return 0;
}

/*
=================
r_texture_unload
=================
*/
void r_texture_unload (r_texture_t *tex)
{
    if (NULL == tex)
    {
        sys_printf("NULL tex\n");
        return;
    }

    /* internal textures shouldn't be unloaded */
    if (!tex->mask)
        return;

    if (NULL != tex->next)
        tex->next->prev = tex->prev;

    if (NULL != tex->prev)
        tex->prev->next = tex->next;
    else
        textures = tex->next;

    gl_texture_delete(tex->gltex);

    mem_free(tex);
}

/*
=================
r_texture_mask_unload
=================
*/
void r_texture_mask_unload (int mask)
{
    r_texture_t *t, *next = NULL;

    if (!mask)
        return;

    for (t = textures; NULL != t ;t = next)
    {
        next = t->next;

        if (t->mask & mask)
            r_texture_unload(t);
    }
}

/*
=================
r_texture_init
=================
*/
int r_texture_init (void)
{
    textures = NULL;

    return 0;
}

/*
=================
r_texture_shutdown
=================
*/
void r_texture_shutdown (void)
{
    while (NULL != textures)
        r_texture_unload(textures);
}
