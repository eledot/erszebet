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

#include "game/g_private.h"
#include "game/g_render_private.h"

typedef struct g_render_common_s
{
    RENDER_PLUGIN_COMMON_DATA;
}g_render_common_t;

extern const g_render_plugin_t g_render_plugin_sprite;
extern const g_render_plugin_t g_render_plugin_text;
extern const g_render_plugin_t g_render_plugin_model;
extern const g_render_plugin_t g_render_plugin_circle;
extern const g_render_plugin_t g_render_plugin_line;

static const g_render_plugin_t * const g_render_plugins[] =
{
    &g_render_plugin_sprite,
    &g_render_plugin_circle,
    &g_render_plugin_line
/*
    &g_render_plugin_text,
*/
};

enum { g_render_plugins_num = STSIZE(g_render_plugins) };

static bool g_render_plugins_usable[g_render_plugins_num];

static g_entity_field_t ent_fields_render_common[] =
{
#define STRUCTURE_FOR_OFFSETS g_render_common_t
    ENTITY_FIELD("color",   color, VECTOR, NULL),
    ENTITY_FIELD("color_r", color, DOUBLE, NULL),
    ENTITY_FIELD("color_g", color, DOUBLE, NULL),
    ENTITY_FIELD("color_b", color, DOUBLE, NULL),
    ENTITY_FIELD("alpha",   alpha, DOUBLE, NULL),
    ENTITY_FIELD("scale",   scale, DOUBLE, NULL),
    ENTITY_FIELD_NULL
};

static void render_set_callback (g_entity_t *ent) GNUC_NONNULL;

static g_entity_field_t ent_field_render =
    ENTITY_FIELD("render", color, CUSTOM_CALLBACK, &render_set_callback);

/*
=================
render_set_callback
=================
*/
static void render_set_callback (g_entity_t *ent)
{
    /* FIXME */
    sys_printf("FIXME ent=%p\n", ent);
}

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
g_list_render_plugins_f
=================
*/
GNUC_NONNULL static void g_list_render_plugins_f (GNUC_UNUSED const struct cmd_s *cmd,
                                           int source,
                                           GNUC_UNUSED int argc,
                                           GNUC_UNUSED const char **argv)
{
    int i;
    const g_render_plugin_t *plugin;
    const g_entity_field_t *field;

    if (source == CMD_SRC_KEY_UP)
        return;

    sys_printf("----------- entity render plugins list -----------\n");

    for (i = 0; i < g_render_plugins_num ;i++)
    {
        plugin = g_render_plugins[i];

        sys_printf("render plugin: %p name=%s usable=%s data_size=%i\n",
                   plugin,
                   plugin->name,
                   g_render_plugins_usable[i] ? "yes" : "no",
                   plugin->render_data_size);

        for (field = plugin->entity_fields; NULL != field->name ;field++)
        {
            sys_printf("     field: %s type=%s\n",
                       field->name,
                       g_entity_field_type_string(field->type));
        }
    }
}

/*
=================
g_render_init
=================
*/
void g_render_init (void)
{
    int i;

    g_entity_add_field_list(ent_fields_render_common, ENT_FIELD_INDEX_RENDER_COMMON);
    ent_field_render.index = ENT_FIELD_INDEX_BASE;
    g_entity_add_field(&ent_field_render);

    for (i = 0; i < g_render_plugins_num ;i++)
    {
        g_render_plugins_usable[i] = (NULL == g_render_plugins[i]->init ||
                                        g_render_plugins[i]->init());

        if (g_render_plugins_usable[i])
        {
            g_entity_add_field_list(g_render_plugins[i]->entity_fields, i + 1);
            sys_printf("+g_render_%s\n", g_render_plugins[i]->name);
        }
        else
        {
            sys_printf("failed to initialize %s renderer\n", g_render_plugins[i]->name);
        }
    }

    cmd_register("g_list_render_plugins", NULL, &g_list_render_plugins_f, 0);

    sys_printf("+g_render\n");
}

/*
=================
g_render_shutdown
=================
*/
void g_render_shutdown (void)
{
    int i;

    g_entity_delete_field_list(ent_fields_render_common);
    g_entity_delete_field(&ent_field_render);

    for (i = 0; i < g_render_plugins_num ;i++)
    {
        if (g_render_plugins_usable[i])
        {
            g_entity_delete_field_list(g_render_plugins[i]->entity_fields);
            sys_printf("-g_render_%s\n", g_render_plugins[i]->name);
        }
    }

    sys_printf("-g_render\n");
}
