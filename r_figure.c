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

enum
{
    FIGURE_TYPE_CIRCLE = 0,
    FIGURE_TYPE_LINE
}r_figure_types_e;

typedef struct r_figure_s
{
    int type;

    r_texture_t *texture;
    double width;

    union
    {
        struct
        {
            double radius;
        }circle;

        struct
        {
            double coords_end[3];
        }line;
    }data;
}r_figure_t;

/*
=================
new_figure
=================
*/
static bool new_figure (const char *name, r_figure_t **f)
{
    r_texture_t *texture;

    if (NULL != *f)
    {
        if (strcmp((*f)->texture->name, name))
        {
            r_figure_unset(*f);
            *f = NULL;
        }
    }

    if (NULL == *f)
    {
        if (!r_texture_load(name, R_TEX_SCREEN_UI, &texture))
        {
            sys_printf("failed to load texture \"%s\"\n", name);
            return false;
        }

        if (NULL == ((*f) = mem_alloc_static(sizeof(**f))))
        {
            r_texture_unload(texture);
            return false;
        }

        (*f)->texture = texture;
    }

    return true;
}

/*
=================
r_figure_set_circle
=================
*/
bool r_figure_set_circle (const char *name,
                          double radius,
                          double width,
                          void **data)
{
    r_figure_t *f;

    if (NULL == name || radius <= 0.0 || width <= 0.0 || NULL == data)
    {
        sys_printf("bad args (name=%p, radius=%-2.2lf, width = %-2.2lf, data=%p)\n",
                   name, radius, width, data);
        return false;
    }

    if (!new_figure(name, (r_figure_t **)data))
        return false;

    f = *data;

    f->type = FIGURE_TYPE_CIRCLE;
    f->width = width;
    f->data.circle.radius = radius;

    return true;
}

/*
=================
r_figure_set_line
=================
*/
bool r_figure_set_line (const char *name,
                        const double *end,
                        double width,
                        void **data)
{
    r_figure_t *f;

    if (NULL == name || NULL == end || width <= 0.0 || NULL == data)
    {
        sys_printf("bad args (name=%p, end=%p, width = %-2.2lf, data=%p)\n",
                   name, end, width, data);
        return false;
    }

    if (!new_figure(name, (r_figure_t **)data))
        return false;

    f = *data;

    f->type = FIGURE_TYPE_LINE;
    f->width = width;
    f->data.line.coords_end[0] = end[0];
    f->data.line.coords_end[1] = end[1];

    *data = f;

    return true;
}

/*
=================
r_figure_unset
=================
*/
void r_figure_unset (void *data)
{
    r_figure_t *f;

    if (NULL == data)
    {
        sys_printf("bad args (data=%p)\n", data);
        return;
    }

    f = data;
    r_texture_unload(f->texture);
    mem_free(f);
}

/*
=================
r_figure_draw
=================
*/
void r_figure_draw (const void *data,
                    const double *origin,
                    const double *color,
                    double alpha,
                    double scale)
{
    const r_figure_t *f;
    double v[3];
    const double zero[3] = { 0.0, 0.0, 1.0 };

    if (NULL == data || NULL == origin || NULL == color || !scale)
    {
        sys_printf("bad args (data=%p, origin=%p, color=%p, alpha=%-2.2lf, scale=%-2.2lf\n",
                   data, origin, color, alpha, scale);
        return;
    }

    f = data;

    gl_color(color[0], color[1], color[2], alpha);

    if (FIGURE_TYPE_CIRCLE == f->type)
    {
        /* FIXME */
    }
    else if (FIGURE_TYPE_LINE == f->type)
    {
        double length;
        double n[3];
        double verts[9];
        double texcoords[8];

        vector_sub(f->data.line.coords_end, origin, v);
        length = vector_length(v);
        vector_normal(v, zero, n);
        vector_normalize(n);
        vector_mul(n, f->width / 2.0, n);

        vector_sub(origin, n, &verts[0]);
        vector_add(origin, n, &verts[2]);
        vector_add(f->data.line.coords_end, n, &verts[4]);
        vector_sub(f->data.line.coords_end, n, &verts[6]);

        texcoords[0] = 0.0; texcoords[1] = length / f->texture->w;
        texcoords[2] = 1.0; texcoords[3] = texcoords[1];
        texcoords[4] = 0.0; texcoords[5] = 0.0;
        texcoords[6] = 1.0; texcoords[7] = 0.0;

        gl_draw_quad(f->texture->gltex, verts, texcoords);
    }
}
