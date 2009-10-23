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

#include "g_private.h"
#include "g_render.h"

extern const ent_render_plugin_t ent_render_plugin_sprite;
extern const ent_render_plugin_t ent_render_plugin_text;
extern const ent_render_plugin_t ent_render_plugin_model;
extern const ent_render_plugin_t ent_render_plugin_circle;
extern const ent_render_plugin_t ent_render_plugin_line;

static const ent_render_plugin_t * const ent_render_plugins[] =
{
    &ent_render_plugin_sprite,
    &ent_render_plugin_circle,
    &ent_render_plugin_line
/*
    &ent_render_plugin_model,
    &ent_render_plugin_text,
*/
};

enum { ent_render_plugins_num = STSIZE(ent_render_plugins) };

static bool ent_render_plugins_usable[ent_render_plugins_num];

/*
=================
g_render_entity
=================
*/
void g_render_entity (GNUC_UNUSED const g_entity_t *ent)
{
    /* FIXME */
}

/*
=================
g_render_init
=================
*/
void g_render_init (void)
{
    int i;

    for (i = 0; i < ent_render_plugins_num ;i++)
    {
        ent_render_plugins_usable[i] = (NULL == ent_render_plugins[i]->init ||
                                        ent_render_plugins[i]->init());
    }

    sys_printf("+g_render\n");
}

/*
=================
g_render_shutdown
=================
*/
void g_render_shutdown (void)
{
    sys_printf("-g_render\n");
}
