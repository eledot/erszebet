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

#ifndef _SND_OPENAL_H
#define _SND_OPENAL_H

#define PCM_MAX_NONSTREAMING (8 * 1024 * 1024)

struct snd_stream_s;

typedef void * snd_sound_t;

typedef enum
{
    SND_FL_LOOPING = (1 << 0)
}snd_flags_e;

int snd_get_stream_format (int bps, int channels) GNUC_CONST GNUC_WARN_UNUSED_RES;

void snd_set_listener_pos (const float *pos) GNUC_NONNULL;
void snd_set_listener_orientation (const float *ori) GNUC_NONNULL;
void snd_set_listener_velocity (const float *vel) GNUC_NONNULL;

snd_sound_t snd_load (const char *name, int flags);
void snd_unload (snd_sound_t *sound);

unsigned int snd_create_source (const snd_sound_t *sound);
void snd_delete_source (unsigned int source);
void snd_source_play (unsigned int source);
void snd_source_stop (unsigned int source);
void snd_source_set_looping (unsigned int source, erbool loop);
erbool snd_source_is_playing (unsigned int source);

void snd_frame (void);

erbool snd_init (void);
void snd_shutdown (void);

#endif /* !_SND_OPENAL_H */
