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
#include "sglib.h"

#define MAX_FRAMES 64

#define SPRITE_NAME_COMPARATOR(f1, f2) strcmp(f1->name, f2->name)

SGLIB_DEFINE_SORTED_LIST_PROTOTYPES(r_sprite_t, SPRITE_NAME_COMPARATOR, next);
SGLIB_DEFINE_SORTED_LIST_FUNCTIONS(r_sprite_t, SPRITE_NAME_COMPARATOR, next);

static r_sprite_t *sprites;

/*
=================
r_sprite_get_align
=================
*/
GNUC_NONNULL static void r_sprite_get_align (PUV const char *name, const char *word, int *align)
{
    int a = *align;

    if (!strcmp(word, "left"))
        a |= SPRITE_ALIGN_LEFT;
    else if (!strcmp(word, "right"))
        a |= SPRITE_ALIGN_RIGHT;
    else if (!strcmp(word, "top"))
        a |= SPRITE_ALIGN_TOP;
    else if (!strcmp(word, "bottom"))
        a |= SPRITE_ALIGN_BOTTOM;

    if (!IS_PWROV2(a & (SPRITE_ALIGN_LEFT | SPRITE_ALIGN_RIGHT)))
    {
        sys_printf("%s: aligned both left and right\n", name);
        a -= a & (SPRITE_ALIGN_LEFT | SPRITE_ALIGN_RIGHT);
    }

    if (!IS_PWROV2(a & (SPRITE_ALIGN_TOP | SPRITE_ALIGN_BOTTOM)))
    {
        sys_printf("%s: aligned both top and bottom\n", name);
        a -= a & (SPRITE_ALIGN_TOP | SPRITE_ALIGN_BOTTOM);
    }

    *align = a;
}

/*
=================
r_sprite_load
=================
*/
bool r_sprite_load (const char  *name,
                    int          type,
                    r_sprite_t **sprite)
{
    char tmp[MISC_MAX_FILENAME];
    int i, nlen, num, size, align = 0;
    r_sprite_t *s = NULL, clone;
    void *data = NULL;
    char *d, *namecopy;
    const char *frames_names[MAX_FRAMES];

    if (NULL == name || NULL == frames_names || NULL == sprite)
    {
        sys_printf("bad args (name=%p, type=%i, sprite=%p)\n",
                   name,
                   type,
                   sprite);
        return false;
    }

    *sprite = NULL;

    /* check if it was loaded earlier */
    clone.name = name;
    s = sglib_r_sprite_t_find_member(sprites, &clone);

    if (NULL != s)
    {
        s->ref++;
        *sprite = s;
        return true;
    }

    snprintf(tmp, sizeof(tmp), "spr/%s.txt", name);

    if (1 > (size = fs_open_read_close(tmp, &data, 4096, r_mempool, true)))
    {
        sys_printf("sprite \"%s\" not found\n", name);
        return false;
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
        {
            if (*frames_names[num - 1] == '%')
                r_sprite_get_align(name, frames_names[--num] + 1, &align);

            frames_names[num++] = d;
        }
    }

    nlen = strlen(name) + 1;

    /* one-line sprite */
    if (num == 2 && *frames_names[0] == '*')
    {
        num = strtol(frames_names[0] + 1, NULL, 0);

        if (num < 1 || num > MAX_FRAMES)
        {
            sys_printf("bad number of frames (%i) in \"%s\"\n", num, name);
            goto error;
        }

        if (NULL == (s = mem_alloc(r_mempool, sizeof(r_sprite_t) + sizeof(r_texture_t *) + nlen)))
            goto error;

        if (!r_texture_load(frames_names[1], type, s->frames))
        {
            sys_printf("failed to load texture for sprite \"%s\"\n", name);
            goto error;
        }

        s->type = R_SPRITE_TYPE_LINE;
        namecopy = (char *)s + sizeof(r_sprite_t) + sizeof(r_texture_t *);
        s->inc = s->frames[0]->texw / (float)num;
    }
    else
    {
        if (NULL == (s = mem_alloc(r_mempool, sizeof(r_sprite_t) + num * sizeof(r_texture_t *) + nlen)))
            goto error;

        for (i = 0; i < num ;i++)
        {
            if (!r_texture_load(frames_names[i], type, &s->frames[i]))
            {
                sys_printf("failed to load frame %i (\"%s\")\n", i, frames_names[i]);

                for (i--; i >= 0 ;i--)
                    r_texture_unload(s->frames[i]);

                goto error;
            }
        }

        s->type = R_SPRITE_TYPE_TEXTURES;
        namecopy = (char *)s + sizeof(r_sprite_t) + num * sizeof(r_texture_t *);
        s->inc = 1.0f;
    }

    strlcpy(namecopy, name, nlen);
    s->name = namecopy;
    s->ref = 1;
    s->frames_num = num;
    s->align = align;

    *sprite = s;

    sglib_r_sprite_t_add(&sprites, s);

    mem_free(data);

    return true;

error:

    if (NULL != s)
        mem_free(s);

    if (NULL != data)
        mem_free(data);

    return false;
}

/*
=================
r_sprite_unload
=================
*/
void r_sprite_unload (r_sprite_t *spr)
{
    int i;

    if (NULL == spr)
    {
        sys_printf("NULL sprite\n");
        return;
    }

    /* check the reference counter */
    if (--spr->ref > 0)
        return;

    r_texture_unload(spr->frames[0]);

    if (spr->type == R_SPRITE_TYPE_TEXTURES)
    {
        for (i = 1; i < spr->frames_num ;i++)
            r_texture_unload(spr->frames[i]);
    }

    sglib_r_sprite_t_delete(&sprites, spr);

    mem_free(spr);
}

/*
=================
r_sprite_draw
=================
*/
void r_sprite_draw (const r_sprite_t *sprite,
                    int frame,
                    double width,
                    double height)
{
    const r_texture_t *tex;
    float vt[8], verts[8];
    double originx, originy;

    if (NULL == sprite)
    {
        sys_printf("bad args (sprite=%p, frame=%i, width=%-2.2lf, height=%-2.2lf)\n",
                   sprite, frame, width, height);
        return;
    }

    if (frame >= sprite->frames_num || frame < 0)
        return;

    originx = 0.0;
    originy = 0.0;

    tex = sprite->frames[0];

    if (!(int)width)
        width = tex->w / sprite->frames_num;

    if (!(int)height)
        height = tex->h;

    if (sprite->align & SPRITE_ALIGN_TOP)
        originy -= height;
    else if (!(sprite->align & SPRITE_ALIGN_BOTTOM))
        originy -= height / 2.0;

    if (sprite->align & SPRITE_ALIGN_RIGHT)
        originx -= width;
    else if (!(sprite->align & SPRITE_ALIGN_LEFT))
        originx -= width / 2.0;

    verts[0] = verts[4] = originx;
    verts[1] = verts[3] = originy;
    verts[5] = verts[7] = originy + height;
    verts[6] = verts[2] = originx + width;


    if (R_SPRITE_TYPE_TEXTURES == sprite->type)
    {
        tex = sprite->frames[frame];

        vt[0] = vt[4] = 0.0;
        vt[1] = vt[3] = tex->texh;
        vt[5] = vt[7] = 0.0f;
        vt[6] = vt[2] = tex->texw;

        gl_draw_quad(tex->gltex, verts, vt);
    }
    else
    {
        tex = sprite->frames[0];

        vt[0] = vt[4] = frame * sprite->inc;
        vt[1] = vt[3] = tex->texh;
        vt[5] = vt[7] = 0.0f;
        vt[6] = vt[2] = vt[0] + sprite->inc;

        gl_draw_quad(tex->gltex, verts, vt);
    }
}

/*
=================
r_list_sprites_f
=================
*/
static void r_list_sprites_f (GNUC_UNUSED const struct cmd_s *cmd,
                              int source,
                              GNUC_UNUSED int argc,
                              GNUC_UNUSED const char **argv)
{
    struct sglib_r_sprite_t_iterator it;
    const r_sprite_t *spr;

    if (source == CMD_SRC_KEY_UP)
        return;

    sys_printf("----------- sprites list -----------\n");

    for (spr = sglib_r_sprite_t_it_init(&it, sprites);
         NULL != spr;
         spr = sglib_r_sprite_t_it_next(&it))
    {
        sys_printf("sprite: %p name=%s ref=%i frames=%i\n",
                   spr,
                   spr->name,
                   spr->ref,
                   spr->frames_num);
    }
}

/*
=================
r_sprite_init
=================
*/
bool r_sprite_init (void)
{
    sprites = NULL;

    cmd_register("r_list_sprites", NULL, &r_list_sprites_f, 0);

    return true;
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
