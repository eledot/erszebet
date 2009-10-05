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

#include "common.h"

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

static int snd_wav_i = 0;

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
static int snd_wav_find_chunk (const uint8_t **buffer, int *size, const char *chunk)
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
            return 0;
        }
    }

    return -1;
}

/*
=================
snd_wav_get_wav_info
=================
*/
static int snd_wav_get_wav_info (const uint8_t *buffer, int size, wav_info_t *wav)
{
    const uint8_t *b  = buffer;
    int            s  = size;
    const uint8_t *b2 = buffer;
    int            s2 = size;

    if (size < 40)
        return -1;

    if (0 != snd_wav_find_chunk(&b, &s, "RIFF") ||
        0 != snd_wav_find_chunk(&b, &s, "WAVE") ||
        0 != snd_wav_find_chunk(&b, &s, "fmt "))
    {
        sys_printf("RIFF/WAVE/fmt chunks are missing\n");
        return -2;
    }

    if (s < 32)
    {
        sys_printf("invalid wav file\n");
        return -3;
    }

    b += 4;
    wav->format   = LITTLESHORT2(b);
    wav->channels = LITTLESHORT2(b);
    wav->rate     = LITTLELONG2(b);

    b += 6;
    wav->bps      = LITTLESHORT2(b);

    if (0 != snd_wav_find_chunk(&b2, &s2, "data"))
    {
        sys_printf("data chunk is missing\n");
        return -4;
    }

    if (s2 < 8)
    {
        sys_printf("invalid wav file\n");
        return -5;
    }

    wav->data_size  = LITTLELONG2(b2);
    wav->data_start = b2 - buffer;

    if (wav->data_size < 1)
    {
        sys_printf("wav data size < 1\n");
        return -6;
    }

    return 0;

}

/*
=================
snd_wav_load
=================
*/
int snd_wav_load (const char   *name,
                  snd_stream_t *stream,
                  int          *streaming,
                  mem_pool_t    pool)
{
    fs_file_t  f;
    int        file_size, s, format;
    wav_info_t wav;
    uint8_t    tmp[4096];

    if (NULL == name || NULL == stream || NULL == pool || NULL == streaming)
    {
        sys_printf("bad args (name=%p, pool=%p, stream=%p, streaming=%p)\n",
                   name,
                   pool,
                   stream,
                   streaming);

        return -1;
    }

    memset(stream, 0, sizeof(*stream));

    if (NULL == (f = fs_open(name, FS_RDONLY, &file_size, 0)))
        return -2;

    s = fs_read(f, tmp, sizeof(tmp));

    if (0 != snd_wav_get_wav_info(tmp, s, &wav))
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

    *streaming = wav.data_size > PCM_MAX_NONSTREAMING;

    stream->stream_data_format = format;
    stream->stream_data_rate   = wav.rate;

    if (*streaming)
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

        if (NULL == (stream->data = mem_alloc(pool, stream->data_size)))
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

    return 0;

error:
    sys_printf("failed to load \"%s\"\n", name);

    if (NULL != stream->data)
        mem_free(stream->data);

    fs_close(f);

    return -3;
}

/*
=================
snd_wav_init
=================
*/
int snd_wav_init (void)
{
    snd_wav_i = 1;

    sys_printf("+snd_wav\n");

    return 0;
}

/*
=================
snd_wav_shutdown
=================
*/
void snd_wav_shutdown (void)
{
    if (!snd_wav_i)
        return;

    sys_printf("-snd_wav\n");
}

#else /* !ENGINE_SND_WAV */

#include "common.h"

int snd_wav_load (GNUC_UNUSED const char *name,
                  GNUC_UNUSED snd_stream_t *stream,
                  GNUC_UNUSED int *streaming,
                  GNUC_UNUSED mem_pool_t pool)
{
    return -1;
}

int snd_wav_init (void) { return -1; }
void snd_wav_shutdown (void) { }

#endif /* ENGINE_SND_WAV */
