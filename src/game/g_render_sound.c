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

#include "game/g_render_private.h"
#include "sound/snd.h"

#ifdef ENGINE_SND

typedef struct ent_render_sound_s
{
    RENDER_PLUGIN_COMMON_DATA;

    erbool       loop;
    erbool       play;
    char        *sound_name;
    snd_sound_t *sound;
    unsigned int source;
}ent_render_sound_t;

/*
=================
sound_loop_callback
=================
*/
GNUC_NONNULL static void sound_loop_callback (g_entity_t *ent)
{
    ent_render_sound_t *r = ent->render_data;

    if (ent->render_valid)
        snd_source_set_looping(r->source, r->loop);
}

/*
=================
sound_reload_callback
=================
*/
GNUC_NONNULL static void sound_reload_callback (g_entity_t *ent)
{
    ent_render_sound_t *r = ent->render_data;

    if (ent->render_valid)
    {
        snd_unload(r->sound);
        snd_delete_source(r->source);
        ent->render_valid = false;
    }

    if (NULL == r->sound_name)
        return;

    if (NULL == (r->sound = snd_load(r->sound_name, r->loop ? SND_FL_LOOPING : 0)))
    {
        sys_printf("failed to load \"%s\"\n", r->sound_name);
        return;
    }

    r->source = snd_create_source(r->sound);

    ent->render_valid = true;
}

/*
=================
sound_play_callback
=================
*/
GNUC_NONNULL static void sound_play_callback (g_entity_t *ent)
{
    ent_render_sound_t *r = ent->render_data;

    if (!ent->render_valid)
        return;

    if (snd_source_is_playing(r->source))
        snd_source_stop(r->source);

    sys_printf("playing source %u\n", r->source);

    snd_source_play(r->source);
}

/*
=================
ent_render_sound_unset
=================
*/
GNUC_NONNULL static void ent_render_sound_unset (g_entity_t *ent)
{
    ent_render_sound_t *r = ent->render_data;

    if (!ent->render_valid)
        return;

    snd_delete_source(r->source);
    snd_unload(r->sound);
}

/*
=================
ent_render_sound_draw
=================
*/
GNUC_NONNULL static void ent_render_sound_draw (GNUC_UNUSED g_entity_t *ent)
{
    /* FIXME */
}

static g_field_t ent_fields_render_sound[] =
{
#define STRUCTURE_FOR_OFFSETS ent_render_sound_t
    G_FIELD("loop",  loop,       ERBOOL, false, &sound_loop_callback),
    G_FIELD("play",  play,       ERBOOL, false, &sound_play_callback),
    G_FIELD("sound", sound_name, STRING, NULL,  &sound_reload_callback),
    G_FIELD_NULL
};

const g_render_plugin_t g_render_plugin_sound =
{
    .name = "sound",
    .render_data_size = sizeof(ent_render_sound_t),
    .fields = ent_fields_render_sound,
    .unset = &ent_render_sound_unset,
    .draw = &ent_render_sound_draw
};

#endif
