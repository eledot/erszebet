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

#ifdef ENGINE_SND_OGG

#include <vorbisfile.h>

#include "common.h"
#include "snd_ogg_funcs.h"

#define STREAM_BUFFER_SIZE 65536

static int snd_ogg_i = 0;

static lib_t libvorbis;
static lib_t libvorbisfile;

/*
=================
eovcb_read
=================
*/
static size_t eovcb_read (void *ptr, size_t size, size_t nmemb, snd_stream_t *datasource)
{
    return fs_read(datasource->file, ptr, size * nmemb);
}

/*
=================
eovcb_seek
=================
*/
static int eovcb_seek (snd_stream_t *datasource, ogg_int64_t offset, int whence)
{
    int origin;

    switch (whence)
    {
    case SEEK_CUR: origin = FS_CURRENT; break;
    case SEEK_END: origin = FS_END;     break;
    default:       origin = FS_START;   break;
    }

    return fs_seek(datasource->file, offset, origin);
}

/*
=================
eovcb_close
=================
*/
static int eovcb_close (snd_stream_t *datasource)
{
    fs_close(datasource->file);

    return 0;
}

/*
=================
eovcb_tell
=================
*/
static long eovcb_tell (snd_stream_t *datasource)
{
    return fs_tell(datasource->file);
}

static const ov_callbacks callbacks =
{
    (void *)&eovcb_read,
    (void *)&eovcb_seek,
    (void *)&eovcb_close,
    (void *)&eovcb_tell
};

/*
=================
snd_ogg_stream_unload
=================
*/
static void snd_ogg_stream_unload (snd_stream_t *stream)
{
}

/*
=================
snd_ogg_stream_func
=================
*/
static int snd_ogg_stream_func (snd_stream_t *stream, int flags, snd_feed_callback_t feed)
{
    return SND_STREAM_RET_OK;
}

/*
=================
snd_ogg_load
=================
*/
int snd_ogg_load (const char   *name,
                  snd_stream_t *stream,
                  int          *streaming,
                  mem_pool_t    pool)
{
    fs_file_t       f;
    int             file_size, length, buffer_size, add, cursec;
    OggVorbis_File *vorbisfile = NULL;
    vorbis_info    *vorbisinfo = NULL;

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

    if (NULL == (vorbisfile = mem_alloc(pool, sizeof(*vorbisfile))))
    {
        sys_printf("failed to allocate %i bytes\n", sizeof(*vorbisfile));
        goto error;
    }

    stream->file    = f;
    stream->private = vorbisfile;

    if (eov_open_callbacks(stream, vorbisfile, NULL, 0, callbacks) != 0)
    {
        sys_printf("bad ogg file\n");
        goto error;
    }

    if (!(vorbisinfo = eov_info(vorbisfile, -1)))
    {
        sys_printf("ov_info failed\n");
        goto error;
    }

    if (-1 == (stream->stream_data_format = snd_get_stream_format(16, vorbisinfo->channels)))
    {
        sys_printf("unsupported format\n");
        goto error;
    }

    stream->stream_data_rate = vorbisinfo->rate;

    switch (vorbisinfo->channels)
    {
    case 1:
        buffer_size = vorbisinfo->rate >> 2;
        buffer_size -= buffer_size % 2;
        break;

    case 2:
        buffer_size = vorbisinfo->rate >> 1;
        buffer_size -= buffer_size % 4;
        break;

    default:
        sys_printf("unsupported number of channels (%i)\n", vorbisinfo->channels);
        goto error;
    }

    length = eov_pcm_total(vorbisfile, -1) * vorbisinfo->channels * 2;

    *streaming = length > PCM_MAX_NONSTREAMING;

    if (*streaming)
    {
        sys_printf("\"%s\" should be streamed\n", name);

        stream->stream_data_size = length;

        stream->unload = &snd_ogg_stream_unload;
        stream->stream = &snd_ogg_stream_func;

        /* FIXME */
        goto error;
    }
    else
    {
        stream->data_size = length;

        if (NULL == (stream->data = mem_alloc(pool, stream->data_size)))
        {
            sys_printf("failed to allocate %i bytes\n", stream->data_size);
            goto error;
        }

        buffer_size = 0;

        while (0 < (add = eov_read(vorbisfile,
                                   (char *)stream->data + buffer_size,
                                   length - buffer_size,
                                   0,
                                   2,
                                   1,
                                   &cursec)))
        {
            buffer_size += add;
        }

        eov_clear(vorbisfile);
        mem_free(stream->private);
    }

    return 0;

error:
    sys_printf("failed to load \"%s\"\n", name);

    if (NULL != vorbisfile)
        eov_clear(vorbisfile);
    else
        fs_close(f);

    if (NULL != stream->private)
        mem_free(stream->private);

    if (NULL != stream->data)
        mem_free(stream->data);

    return -3;
}

/*
=================
snd_ogg_init
=================
*/
int snd_ogg_init (void)
{
    const char *names[] =
    {
        "libvorbisfile.so.3.3.0",
        "libvorbisfile.so.3",
        "libvorbisfile.so",
        NULL
    };

    const char *names2[] =
    {
        "libvorbis.so.0.4.1",
        "libvorbis.so.0",
        "libvorbis.so",
        NULL
    };

    if (sys_arg_find("-nolibvorbis"))
        return 0;

    if (LIB_HANDLE_INVALID == (libvorbisfile = lib_open(names, funcs, 0)))
    {
        sys_printf("ogg support disabled\n");
        lib_close(libvorbis);
        return -1;
    }

    if (LIB_HANDLE_INVALID == (libvorbis = lib_open(names2, NULL, 0)))
    {
        sys_printf("ogg support disabled\n");
        return -2;
    }

    snd_ogg_i = 1;

    sys_printf("+snd_ogg\n");

    return 0;
}

/*
=================
snd_ogg_shutdown
=================
*/
void snd_ogg_shutdown (void)
{
    if (!snd_ogg_i)
        return;

    lib_close(libvorbisfile);
    lib_close(libvorbis);

    sys_printf("-snd_ogg\n");
}

#else /* !ENGINE_SND_OGG */

#include "common.h"

int snd_ogg_load (const char   *name UV,
                  snd_stream_t *stream UV,
                  int          *streaming UV,
                  mem_pool_t    pool UV)
{
    return -1;
}

int snd_ogg_init (void) { return -1; }
void snd_ogg_shutdown (void) { }

#endif /* ENGINE_SND_OGG */
