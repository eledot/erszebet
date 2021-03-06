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

#include "render/r_private.h"
#include "gl/gl_texture.h"
#include "sglib.h"

#define TEXTURE_NAME_COMPARATOR(f1, f2) strcmp(f1->name, f2->name)

SGLIB_DEFINE_SORTED_LIST_PROTOTYPES(r_texture_t, TEXTURE_NAME_COMPARATOR, next);
SGLIB_DEFINE_SORTED_LIST_FUNCTIONS(r_texture_t, TEXTURE_NAME_COMPARATOR, next);

static const int types[] =
{
    GL_TEX_FL_UI - GL_TEX_FL_NOSCALE,
    GL_TEX_FL_UI
};

static r_texture_t *textures;

/*
=================
r_texture_load
=================
*/
erbool r_texture_load (const char *name, int type, image_t *out_image, r_texture_t **tex)
{
    int gltex, nlen, w, h, texw, texh;
    char tmp[MISC_MAX_FILENAME], *namecopy;
    image_t image;
    r_texture_t *t, clone;

    if (NULL == name || type < 0 || type >= STSIZE(types) || NULL == tex)
    {
        sys_printf("bad args (name=%p, type=%i, tex=%p)\n", name, type, tex);
        return false;
    }

    *tex = NULL;

    if (!filename_is_valid(name))
        return false;

    /* check if it was loaded earlier */
    clone.name = name;
    t = sglib_r_texture_t_find_member(textures, &clone);

    if (NULL != t)
    {
        t->ref++;
        *tex = t;
        return true;
    }

    snprintf(tmp, sizeof(tmp), "tex/%s", name);

    if (!image_load(tmp, &image))
        goto error;

    w = image.width;
    h = image.height;

    if (NULL != out_image)
    {
        memcpy(out_image, &image, sizeof(image));
        out_image->data = mem_alloc(r_mempool, w * h << 2);
        memcpy(out_image->data, image.data, w * h << 2);
    }

    if (!gl_texture_create(&image, types[type], &gltex, &texw, &texh))
    {
        sys_printf("failed to create gl texture \"%s\" (%ix%i)\n", name, w, h);
        goto error;
    }

    mem_free(image.data);

    nlen = strlen(name) + 1;

    if (NULL == (t = mem_alloc(r_mempool, sizeof(r_texture_t) + nlen)))
    {
        gl_texture_delete(gltex);
        goto error;
    }

    namecopy = (char *)t + sizeof(r_texture_t);
    strlcpy(namecopy, name, nlen);

    t->name = namecopy;
    t->type = type;
    t->gltex = gltex;
    t->ref = 1;
    t->w = w;
    t->h = h;
    t->texw = (float)w / (float)texw;
    t->texh = (float)h / (float)texh;

    *tex = t;

    sglib_r_texture_t_add(&textures, t);

    return true;

error:

    if (NULL != image.data)
        mem_free(image.data);

    return false;
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
r_list_textures_f
=================
*/
static void r_list_textures_f (GNUC_UNUSED const struct cmd_s *cmd,
                               int source,
                               GNUC_UNUSED int argc,
                               GNUC_UNUSED const char **argv)
{
    struct sglib_r_texture_t_iterator it;
    const r_texture_t *tex;

    if (source == CMD_SRC_KEY_UP)
        return;

    sys_printf("----------- textures list -----------\n");

    for (tex = sglib_r_texture_t_it_init(&it, textures);
         NULL != tex;
         tex = sglib_r_texture_t_it_next(&it))
    {
        sys_printf("texture: %p name=%s type=%i ref=%i w=%i h=%i texw=%-2.2lf texh=%-2.2lf\n",
                   tex,
                   tex->name,
                   tex->type,
                   tex->ref,
                   tex->w,
                   tex->h,
                   tex->texw,
                   tex->texh);
    }
}

/*
=================
r_texture_init
=================
*/
erbool r_texture_init (void)
{
    textures = NULL;

    cmd_register("r_list_textures", NULL, &r_list_textures_f, 0);

    return true;
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
