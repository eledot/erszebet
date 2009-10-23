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

#ifdef ENGINE_SND_WAV

#include "sound/snd_private.h"

#define STREAM_BUFFER_SIZE 16384

typedef struct
{
    int format;
    int channels;
    int rate;
    int bps;
    int data_start;
    int data_size;
}wav_info_t;

/*
=================
snd_wav_stream_unload
=================
*/
static void snd_wav_stream_unload (GNUC_UNUSED snd_stream_t *stream)
{
    /* FIXME */
}

/*
=================
snd_wav_stream_func
=================
*/
static int snd_wav_stream_func (GNUC_UNUSED snd_stream_t *stream,
                                GNUC_UNUSED int flags,
                                GNUC_UNUSED snd_feed_callback_t feed)
{
    /* FIXME */
    return SND_STREAM_RET_OK;
}

/*
=================
snd_wav_find_chunk
=================
*/
GNUC_NONNULL static bool snd_wav_find_chunk (const uint8_t **buffer, int *size, const char *chunk)
{
    int i;

    for (i = 0; i < *size - 4 ;i++)
    {
        if ((*buffer)[i + 0] == chunk[0] &&
            (*buffer)[i + 1] == chunk[1] &&
            (*buffer)[i + 2] == chunk[2] &&
            (*buffer)[i + 3] == chunk[3])
        {
            *buffer += i + 4;
            *size   -= i + 4;
            return true;
        }
    }

    return false;
}

/*
=================
snd_wav_get_wav_info
=================
*/
GNUC_NONNULL static bool snd_wav_get_wav_info (const uint8_t *buffer, int size, wav_info_t *wav)
{
    const uint8_t *b  = buffer;
    int            s  = size;
    const uint8_t *b2 = buffer;
    int            s2 = size;

    if (size < 40)
        return false;

    if (!snd_wav_find_chunk(&b, &s, "RIFF") ||
        !snd_wav_find_chunk(&b, &s, "WAVE") ||
        !snd_wav_find_chunk(&b, &s, "fmt "))
    {
        sys_printf("RIFF/WAVE/fmt chunks are missing\n");
        return false;
    }

    if (s < 32)
    {
        sys_printf("invalid wav file\n");
        return false;
    }

    b += 4;
    wav->format   = LITTLESHORT2(b);
    wav->channels = LITTLESHORT2(b);
    wav->rate     = LITTLELONG2(b);

    b += 6;
    wav->bps      = LITTLESHORT2(b);

    if (!snd_wav_find_chunk(&b2, &s2, "data"))
    {
        sys_printf("data chunk is missing\n");
        return false;
    }

    if (s2 < 8)
    {
        sys_printf("invalid wav file\n");
        return false;
    }

    wav->data_size  = LITTLELONG2(b2);
    wav->data_start = b2 - buffer;

    if (wav->data_size < 1)
    {
        sys_printf("wav data size < 1\n");
        return false;
    }

    return true;

}

/*
=================
snd_wav_load
=================
*/
GNUC_NONNULL static bool snd_wav_load (const char   *name,
                                       snd_stream_t *stream)
{
    fs_file_t  f;
    int        file_size, s, format;
    wav_info_t wav;
    uint8_t    tmp[4096];

    if (NULL == (f = fs_open(name, FS_RDONLY, &file_size, 0)))
        return false;

    s = fs_read(f, tmp, sizeof(tmp));

    if (!snd_wav_get_wav_info(tmp, s, &wav))
        goto error;

    if (-1 == (format = snd_get_stream_format(wav.bps, wav.channels)))
    {
        sys_printf("invalid/unsupported wav format\n");
        goto error;
    }

    if (wav.data_size > file_size - wav.data_start)
    {
        sys_printf("wrong data size\n");
        wav.data_size = file_size - wav.data_start;
    }

    stream->streaming = wav.data_size > PCM_MAX_NONSTREAMING;

    stream->stream_data_format = format;
    stream->stream_data_rate   = wav.rate;

    if (stream->streaming)
    {
        sys_printf("\"%s\" should be streamed\n", name);

        stream->unload = &snd_wav_stream_unload;
        stream->stream = &snd_wav_stream_func;

        /* FIXME */
        goto error;
    }
    else
    {
        stream->data_size = wav.data_size;

        if (NULL == (stream->data = mem_alloc(snd_mempool, stream->data_size)))
        {
            sys_printf("failed to allocate %i bytes\n", stream->data_size);
            goto error;
        }

        fs_seek(f, wav.data_start, FS_START);

        if (stream->data_size != fs_read(f, stream->data, stream->data_size))
        {
            sys_printf("failed to read %i bytes\n", stream->data_size);
            goto error;
        }

        fs_close(f);
    }

    return true;

error:
    sys_printf("failed to load \"%s\"\n", name);

    if (NULL != stream->data)
        mem_free(stream->data);

    fs_close(f);

    return false;
}

static const char * const snd_wav_extensions[] = { "wav", NULL };

const snd_plugin_t snd_plugin_wav =
{
    .name = "snd_wav",
    .extensions = snd_wav_extensions,
    .load = snd_wav_load,
};

#endif /* ENGINE_SND_WAV */
