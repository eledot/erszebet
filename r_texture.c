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
#include "sglib.h"

#define TEXTURE_NAME_COMPARATOR(f1, f2) strcmp(f1->name, f2->name)

SGLIB_DEFINE_SORTED_LIST_PROTOTYPES(r_texture_t, TEXTURE_NAME_COMPARATOR, next);
SGLIB_DEFINE_SORTED_LIST_FUNCTIONS(r_texture_t, TEXTURE_NAME_COMPARATOR, next);

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
    int gltex, nlen, w, h;
    char tmp[MISC_MAX_FILENAME], *namecopy;
    image_t image;
    r_texture_t *t, clone;

    if (NULL == name || type < 0 || type >= STSIZE(types) || NULL == tex)
    {
        sys_printf("bad args (name=%p, type=%i, tex=%p)\n", name, type, tex);
        return -1;
    }

    *tex = NULL;

    if (!filename_is_valid(name))
        return -2;

    /* check if it was loaded earlier */
    clone.name = name;
    t = sglib_r_texture_t_find_member(textures, &clone);

    if (NULL != t)
    {
        t->ref++;
        *tex = t;
        return 0;
    }

    snprintf(tmp, sizeof(tmp), "tex/%s", name);

    if (0 != image_load(tmp, &image))
        goto error;

    w = image.width;
    h = image.height;

    if (0 != gl_texture_create(&image, types[type], &gltex))
        goto error;

    mem_free(image.data);
    image.data = NULL;

    nlen = strlen(name) + 1;

    if (NULL == (t = mem_alloc_static(sizeof(r_texture_t) + nlen)))
    {
        gl_texture_delete(gltex);
        goto error;
    }

    namecopy = (char *)t + sizeof(r_texture_t);
    strlcpy(namecopy, name, nlen);

    t->name = namecopy;
    t->type  = type;
    t->gltex = gltex;
    t->ref = 1;
    t->w = w;
    t->h = h;
    t->next  = textures;

    *tex = t;

    sglib_r_texture_t_add(&textures, t);

    return 0;

error:

    if (NULL != image.data)
        mem_free(image.data);

    return -3;
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

    gl_texture_delete(tex->gltex);
    sglib_r_texture_t_delete(&textures, tex);

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
