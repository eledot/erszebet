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
#include "gl/gl.h"

typedef struct g_render_common_s
{
    RENDER_PLUGIN_COMMON_DATA;
}g_render_common_t;

extern const g_render_plugin_t g_render_plugin_sprite;
extern const g_render_plugin_t g_render_plugin_circle;
extern const g_render_plugin_t g_render_plugin_line;
extern const g_render_plugin_t g_render_plugin_text;
extern const g_render_plugin_t g_render_plugin_scissor;
extern const g_render_plugin_t g_render_plugin_text2;
extern const g_render_plugin_t g_render_plugin_fade;

static const g_render_plugin_t * const g_render_plugins[] =
{
    &g_render_plugin_sprite,
    &g_render_plugin_circle,
    &g_render_plugin_line,
    &g_render_plugin_text,
    &g_render_plugin_scissor,
    &g_render_plugin_text2,
    &g_render_plugin_fade
};

enum { g_render_plugins_num = STSIZE(g_render_plugins) };

static erbool g_render_plugins_usable[g_render_plugins_num];

static g_field_t ent_fields_render_common[] =
{
#define STRUCTURE_FOR_OFFSETS g_render_common_t
    G_FIELD("color",   color,    VECTOR, NULL, NULL),
    G_FIELD("color_r", color[0], DOUBLE, 1.0,  NULL),
    G_FIELD("color_g", color[1], DOUBLE, 1.0,  NULL),
    G_FIELD("color_b", color[2], DOUBLE, 1.0,  NULL),
    G_FIELD("alpha",   alpha,    DOUBLE, 1.0,  NULL),
    G_FIELD("scale",   scale,    DOUBLE, 1.0,  NULL),
    G_FIELD_NULL
};

static void render_set_callback (g_entity_t *ent) GNUC_NONNULL;

static g_field_t ent_field_render =
    G_FIELD("render", color, CUSTOM_CALLBACK, NULL, &render_set_callback);

/*
=================
render_set_callback
=================
*/
static void render_set_callback (g_entity_t *ent)
{
    int index_new = lua_tonumber(lua_state, lua_entity_value_index) - 1;
    const g_field_t *f;
    const g_render_plugin_t *plugin_old = NULL;
    const g_render_plugin_t *plugin_new = NULL;

    if (NULL != ent->render_data)
    {
        plugin_old = g_render_plugins[ent->render_index - 1];

        lua_getref(lua_state, ent->lua_dataref);
        g_fields_push(ent->render_data, plugin_old->fields);
        lua_pop(lua_state, 1);

        g_render_mem_free(ent);
    }

    if (index_new < 0 || index_new >= g_render_plugins_num)
        return;

    ent->render_index = index_new + 1;
    plugin_new = g_render_plugins[index_new];
    ent->render_data = mem_alloc(g_mempool, plugin_new->render_data_size);

    /* set defaults */
    g_fields_set_default_values(ent->render_data, ent_fields_render_common);
    g_fields_set_default_values(ent->render_data, plugin_new->fields);

    /* update from ent.__data */
    lua_getref(lua_state, ent->lua_dataref);
    g_fields_pop(ent->render_data, ent_fields_render_common, true);
    g_fields_pop(ent->render_data, plugin_new->fields, true);
    lua_pop(lua_state, 1);

    lua_entity_value_index = -1;

    /* execute field callbacks */
    for (f = plugin_new->fields; NULL != f->name ;f++)
    {
        if (NULL == f->callback)
            continue;

        if (G_FIELD_TYPE_CUSTOM_CALLBACK == f->type)
        {
            lua_getref(lua_state, ent->lua_dataref);
            lua_pushstring(lua_state, f->name);
            lua_gettable(lua_state, -2);
            f->callback(ent);
            lua_pop(lua_state, 2);
        }
        else
        {
            f->callback(ent);
        }
    }

    if (NULL != plugin_new->set)
        plugin_new->set(ent);
}

/*
=================
g_render_mem_free
=================
*/
void g_render_mem_free (g_entity_t *ent)
{
    const g_render_plugin_t *plugin;

    if (NULL == ent->render_data)
        return;

    plugin = g_render_plugins[ent->render_index - 1];

    if (NULL != plugin->unset)
        plugin->unset(ent);

    g_fields_free_values(ent->render_data, plugin->fields);

    mem_free(ent->render_data);
    ent->render_index = 0;
    ent->render_valid = false;
    ent->render_data = NULL;
}

/*
=================
g_render_entity
=================
*/
void g_render_entity (GNUC_UNUSED g_entity_t *ent)
{
    if (ent->render_valid)
    {
        const g_render_plugin_t *plugin = g_render_plugins[ent->render_index - 1];
        const g_render_common_t *common = ent->render_data;
        gl_matrix_push();
        gl_translate_rotate(ent->origin[0], ent->origin[1], ent->angle);
        gl_scale(common->scale, common->scale);
        gl_color(common->color[0], common->color[1], common->color[2], common->alpha);
        plugin->draw(ent);
        gl_matrix_pop();
    }
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
    const g_field_t *field;

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

        for (field = plugin->fields; NULL != field->name ;field++)
        {
            sys_printf("     field: %s type=%s\n",
                       field->name,
                       g_field_type_to_string(field->type));
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
    char tmp[64];

    g_entity_add_field_list(ent_fields_render_common, G_ENT_FIELD_INDEX_RENDER_COMMON);
    ent_field_render.index = G_ENT_FIELD_INDEX_BASE;
    g_entity_add_field(&ent_field_render);

    for (i = 0; i < g_render_plugins_num ;i++)
    {
        g_render_plugins_usable[i] = (NULL == g_render_plugins[i]->init ||
                                        g_render_plugins[i]->init());

        if (g_render_plugins_usable[i])
        {
            g_entity_add_field_list(g_render_plugins[i]->fields, i + 1);
            snprintf(tmp, sizeof(tmp), "g_render_%s", g_render_plugins[i]->name);
            g_set_integer(tmp, i + 1);
            sys_printf("+%s\n", tmp);
        }
        else
        {
            sys_printf("failed to initialize %s renderer\n", g_render_plugins[i]->name);
        }
    }

    g_set_integer("G_TEXT_ALIGN_LEFT", G_TEXT_ALIGN_LEFT);
    g_set_integer("G_TEXT_ALIGN_RIGHT", G_TEXT_ALIGN_RIGHT);
    g_set_integer("G_TEXT_ALIGN_TOP", G_TEXT_ALIGN_TOP);
    g_set_integer("G_TEXT_ALIGN_BOTTOM", G_TEXT_ALIGN_BOTTOM);

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
            g_entity_delete_field_list(g_render_plugins[i]->fields);
            sys_printf("-g_render_%s\n", g_render_plugins[i]->name);
        }
    }

    sys_printf("-g_render\n");
}
