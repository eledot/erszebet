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
#include "gl_texture.h"

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
int r_texture_load (const char *name, int type, r_texture_t **tex)
{
    int     gltex, nlen, w, h;
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

    /* check if it was loaded earlier */
    for (*tex = textures; NULL != *tex ;*tex = (*tex)->next)
    {
        if (!strcmp((*tex)->name, name))
        {
            (*tex)->ref++;
            return 0;
        }
    }

    snprintf(tmp, sizeof(tmp), "tex/%s", name);

    if (0 != image_load(tmp, &image))
        return -3;

    w = image.width;
    h = image.height;

    if (0 != gl_texture_create(&image, types[type], &gltex))
        return -4;

    mem_free(image.data);

    nlen = strlen(name) + 1;

    if (NULL == (*tex = mem_alloc_static(sizeof(r_texture_t) + nlen)))
    {
        gl_texture_delete(gltex);
        return -5;
    }

    (*tex)->type  = type;
    (*tex)->gltex = gltex;
    (*tex)->ref = 1;
    (*tex)->w = w;
    (*tex)->h = h;
    (*tex)->next  = textures;
    strlcpy((*tex)->name, name, nlen);

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

    /* check the reference counter */
    if (--tex->ref > 0)
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
