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

/**
 * @file snd_ogg_funcs.h
 *
 * @brief snd_ogg.c private header with Ogg Vorbis funcs
 *
 * @author ftrvxmtrx
 */

#ifndef _SND_OGG_FUNCS_H
#define _SND_OGG_FUNCS_H

#ifndef OV_FALSE
#error This is internal header!
#endif

static int           (*eov_clear)          (OggVorbis_File *vf);
static vorbis_info * (*eov_info)           (OggVorbis_File *vf,int link);
static int           (*eov_open_callbacks) (void *datasource, OggVorbis_File *vf, char *initial, long ibytes, ov_callbacks callbacks);
static ogg_int64_t   (*eov_pcm_total)      (OggVorbis_File *vf, int i);
static int           (*eov_pcm_seek)       (OggVorbis_File *vf, ogg_int64_t pos);
static long          (*eov_read)           (OggVorbis_File *vf, char *buffer, int length, int bigendianp, int word, int sgned, int *bitstream);

static lib_func_t funcs[] =
{
    { "ov_clear",          (void**)&eov_clear          },
    { "ov_info",           (void**)&eov_info           },
    { "ov_open_callbacks", (void**)&eov_open_callbacks },
    { "ov_pcm_total",      (void**)&eov_pcm_total      },
    { "ov_pcm_seek",       (void**)&eov_pcm_seek       },
    { "ov_read",           (void**)&eov_read           },
    {NULL, NULL}
};

#endif /* !_SND_OGG_FUNCS_H */
