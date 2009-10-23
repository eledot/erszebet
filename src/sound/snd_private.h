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

#ifndef _SND_PRIVATE_H
#define _SND_PRIVATE_H

#include "common.h"

typedef enum
{
    SND_STREAM_RET_OK = 0,
    SND_STREAM_RET_STOPPED,
}snd_stream_func_ret_e;

typedef void (*snd_feed_callback_t) (struct snd_stream_s *stream, const char *data, int size);

typedef struct snd_stream_s
{
    fs_file_t file;
    int       stream_data_start;
    int       stream_data_size;
    int       stream_data_format;
    int       stream_data_rate;
    bool      streaming;

    int       data_size;
    uint8_t  *data;

    void     *private;

    void    (*unload) (struct snd_stream_s *stream);
    int     (*stream) (struct snd_stream_s *stream, int flags, snd_feed_callback_t feed);
}snd_stream_t;

typedef struct snd_plugin_s
{
    const char *name;
    const char * const *extensions;
    bool (*init) (void);
    void (*shutdown) (void);
    bool (*load) (const char *name, snd_stream_t *stream);
}snd_plugin_t;

extern mem_pool_t snd_mempool;

#endif /* !_SND_PRIVATE_H */
