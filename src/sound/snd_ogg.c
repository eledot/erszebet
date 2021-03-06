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

#define OV_EXCLUDE_STATIC_CALLBACKS
#include <vorbisfile.h>

#include "sound/snd_private.h"

#define STREAM_BUFFER_SIZE 65536

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
static void snd_ogg_stream_unload (GNUC_UNUSED snd_stream_t *stream)
{
    /* FIXME */
}

/*
=================
snd_ogg_stream_func
=================
*/
static int snd_ogg_stream_func (GNUC_UNUSED snd_stream_t *stream,
                                GNUC_UNUSED int flags,
                                GNUC_UNUSED snd_feed_callback_t feed)
{
    /* FIXME */

    return SND_STREAM_RET_OK;
}

/*
=================
snd_ogg_load
=================
*/
GNUC_NONNULL static erbool snd_ogg_load (const char   *name,
                                       snd_stream_t *stream)
{
    fs_file_t       f;
    int             file_size, length, buffer_size, add, cursec;
    OggVorbis_File *vorbisfile = NULL;
    vorbis_info    *vorbisinfo = NULL;

    if (NULL == (f = fs_open(name, FS_RDONLY, &file_size, false)))
        return false;

    if (NULL == (vorbisfile = mem_alloc(snd_mempool, sizeof(*vorbisfile))))
    {
        sys_printf("failed to allocate %zu bytes\n", sizeof(*vorbisfile));
        goto error;
    }

    stream->file    = f;
    stream->private = vorbisfile;

    if (ov_open_callbacks(stream, vorbisfile, NULL, 0, callbacks) != 0)
    {
        sys_printf("bad ogg file\n");
        goto error;
    }

    if (!(vorbisinfo = ov_info(vorbisfile, -1)))
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

    length = ov_pcm_total(vorbisfile, -1) * vorbisinfo->channels * 2;

    stream->streaming = length > PCM_MAX_NONSTREAMING;

    if (stream->streaming)
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

        if (NULL == (stream->data = mem_alloc(snd_mempool, stream->data_size)))
        {
            sys_printf("failed to allocate %i bytes\n", stream->data_size);
            goto error;
        }

        buffer_size = 0;

        while (0 < (add = ov_read(vorbisfile,
                                  (char *)stream->data + buffer_size,
                                  length - buffer_size,
                                  0,
                                  2,
                                  1,
                                  &cursec)))
        {
            buffer_size += add;
        }

        ov_clear(vorbisfile);
        mem_free(stream->private);
    }

    return true;

error:
    sys_printf("failed to load \"%s\"\n", name);

    if (NULL != vorbisfile)
        ov_clear(vorbisfile);
    else
        fs_close(f);

    if (NULL != stream->private)
        mem_free(stream->private);

    if (NULL != stream->data)
        mem_free(stream->data);

    return false;
}

static const char * const snd_ogg_extensions[] = { "ogg", NULL };

const snd_plugin_t snd_plugin_ogg =
{
    .name = "snd_ogg",
    .extensions = snd_ogg_extensions,
    .load = snd_ogg_load,
};

#endif /* ENGINE_SND_OGG */
