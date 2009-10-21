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

#ifdef ENGINE_SND_FLAC

#include <stream_decoder.h>

#include "snd_private.h"

#define STREAM_BUFFER_SIZE 65536

/*
=================
eflac_decoder_read
=================
*/
static FLAC__StreamDecoderReadStatus eflac_decoder_read (GNUC_UNUSED const FLAC__StreamDecoder *decoder,
                                                         FLAC__byte buffer[],
                                                         size_t *bytes,
                                                         void *client_data)
{
    snd_stream_t *s = client_data;

    if (NULL != bytes && *bytes > 0)
    {
        *bytes = fs_read(s->file, buffer, *bytes);

        if (!*bytes)
            return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
        else if (*bytes > 0)
            return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
    }

    return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
}

/*
=================
eflac_decoder_seek
=================
*/
static FLAC__StreamDecoderSeekStatus eflac_decoder_seek (GNUC_UNUSED const FLAC__StreamDecoder *decoder,
                                                         FLAC__uint64 abs_offset,
                                                         void *client_data)
{
    snd_stream_t *s = client_data;
    int           offset = (int)abs_offset;

    if (offset == fs_seek(s->file, offset, FS_START))
        return FLAC__STREAM_DECODER_SEEK_STATUS_OK;

    return FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;
}

/*
=================
eflac_decoder_tell
=================
*/
static FLAC__StreamDecoderTellStatus eflac_decoder_tell (GNUC_UNUSED const FLAC__StreamDecoder *decoder,
                                                         FLAC__uint64 *abs_offset,
                                                         void *client_data)
{
    snd_stream_t *s = client_data;
    int           offset;

    if (0 >= (offset = fs_tell(s->file)))
    {
        *abs_offset = offset;
        return FLAC__STREAM_DECODER_TELL_STATUS_OK;
    }

    return FLAC__STREAM_DECODER_TELL_STATUS_ERROR;
}

/*
=================
eflac_decoder_length
=================
*/
static FLAC__StreamDecoderLengthStatus eflac_decoder_length (GNUC_UNUSED const FLAC__StreamDecoder *decoder,
                                                             FLAC__uint64 *stream_length,
                                                             void *client_data)
{
    snd_stream_t *s = client_data;
    int           size;

    if (-1 != (size = fs_get_size(s->file)))
    {
        *stream_length = size;
        return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
    }

    return FLAC__STREAM_DECODER_LENGTH_STATUS_ERROR;
}

/*
=================
eflac_decoder_eof
=================
*/
static FLAC__bool eflac_decoder_eof (GNUC_UNUSED const FLAC__StreamDecoder *decoder, void *client_data)
{
    snd_stream_t *s = client_data;

    return fs_eof(s->file);
}

/*
=================
flac_decoder_write
=================
*/
static FLAC__StreamDecoderWriteStatus eflac_decoder_write (GNUC_UNUSED const FLAC__StreamDecoder *decoder,
                                                           const FLAC__Frame *frame,
                                                           const FLAC__int32 *const buffer[],
                                                           void *client_data)
{
    snd_stream_t *s = client_data;
    unsigned int  i, j;

    if (1 == frame->header.channels)
    {
        switch (frame->header.bits_per_sample)
        {
        case 8:
            /* 8-bit mono */
            j = s->data_size;

            for (i = 0; i < frame->header.blocksize ;i++, j++, s->data_size++)
                ((uint8_t *)s->data)[j] = 128 + buffer[0][i];

            return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;

        case 16:
            /* 16-bit mono */
            j = s->data_size >> 1;

            for (i = 0; i < frame->header.blocksize ;i++, j++, s->data_size += 2)
                ((int16_t *)s->data)[j] = buffer[0][i];

            return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;

        default:
            break;
        }
    }
    else if (2 == frame->header.channels)
    {
        switch (frame->header.bits_per_sample)
        {
        case 8:
            /* 8-bit stereo */
            j = s->data_size;

            for (i = 0; i < frame->header.blocksize ;i++, j += 2, s->data_size += 2)
            {
                ((uint8_t *)s->data)[j + 0] = 128 + buffer[0][i];
                ((uint8_t *)s->data)[j + 1] = 128 + buffer[1][i];
            }

            return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;

        case 16:
            /* 16-bit stereo */
            j = s->data_size >> 1;

            for (i = 0; i < frame->header.blocksize ;i++, j += 2, s->data_size += 4)
            {
                ((int16_t *)s->data)[j + 0] = buffer[0][i];
                ((int16_t *)s->data)[j + 1] = buffer[1][i];
            }

            return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;

        default:
            break;
        }
    }

        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
}

/*
=================
eflac_decoder_metadata
=================
*/
static void eflac_decoder_metadata (GNUC_UNUSED const FLAC__StreamDecoder *decoder,
                                    const FLAC__StreamMetadata *metadata,
                                    void *client_data)
{
    snd_stream_t                          *s = client_data;
    const FLAC__StreamMetadata_StreamInfo *si = &metadata->data.stream_info;

    switch (metadata->type)
    {
    case FLAC__METADATA_TYPE_STREAMINFO:
        s->stream_data_format = snd_get_stream_format(si->bits_per_sample, si->channels);
        s->stream_data_rate   = si->sample_rate;
        s->stream_data_size   = si->total_samples * si->channels * si->bits_per_sample >> 3;
        break;

    case FLAC__METADATA_TYPE_PADDING:
        sys_printf("padding\n");
        break;

    case FLAC__METADATA_TYPE_APPLICATION:
        sys_printf("application\n");
        break;

    case FLAC__METADATA_TYPE_SEEKTABLE:
        sys_printf("seektable\n");
        break;

    case FLAC__METADATA_TYPE_VORBIS_COMMENT:
        sys_printf("vorbis comment\n");
        break;

    case FLAC__METADATA_TYPE_CUESHEET:
        sys_printf("cue sheet\n");
        break;

    case FLAC__METADATA_TYPE_PICTURE:
        sys_printf("picture\n");
        break;

    case FLAC__METADATA_TYPE_UNDEFINED:
        break;

    default:
        break;
    }
}

/*
=================
eflac_decoder_error
=================
*/
static void eflac_decoder_error (GNUC_UNUSED const FLAC__StreamDecoder *decoder,
                                 FLAC__StreamDecoderErrorStatus status,
                                 GNUC_UNUSED void *client_data)
{
    switch (status)
    {
    case FLAC__STREAM_DECODER_ERROR_STATUS_LOST_SYNC:
        sys_printf("lost sync\n");
        break;

    case FLAC__STREAM_DECODER_ERROR_STATUS_BAD_HEADER:
        sys_printf("bad header\n");
        break;

    case FLAC__STREAM_DECODER_ERROR_STATUS_FRAME_CRC_MISMATCH:
        sys_printf("crc mismatch\n");
        break;

    case FLAC__STREAM_DECODER_ERROR_STATUS_UNPARSEABLE_STREAM:
        sys_printf("unparsable stream\n");
        break;

    default:
        break;
    }
}

/*
=================
eflac_decoder_print_init_status
=================
*/
static void eflac_decoder_print_init_status (FLAC__StreamDecoderInitStatus s)
{
    switch (s)
    {
    case FLAC__STREAM_DECODER_INIT_STATUS_UNSUPPORTED_CONTAINER:
        sys_printf("unsupported container\n");
        break;

    case FLAC__STREAM_DECODER_INIT_STATUS_INVALID_CALLBACKS:
        sys_printf("invalid callback(s)\n");
        break;

    case FLAC__STREAM_DECODER_INIT_STATUS_MEMORY_ALLOCATION_ERROR:
        sys_printf("memory allocation error\n");
        break;

    case FLAC__STREAM_DECODER_INIT_STATUS_ALREADY_INITIALIZED:
        sys_printf("already initialized\n");
        break;

    case FLAC__STREAM_DECODER_INIT_STATUS_ERROR_OPENING_FILE:
        sys_printf("error opening file\n");
        break;

    case FLAC__STREAM_DECODER_INIT_STATUS_OK:
        break;

    default:
        sys_printf("unknown error\n");
        break;
    }
}

/*
=================
snd_flac_stream_unload
=================
*/
static void snd_flac_stream_unload (GNUC_UNUSED snd_stream_t *stream)
{
    /* FIXME */
}

/*
=================
snd_flac_stream_func
=================
*/
static int snd_flac_stream_func (GNUC_UNUSED snd_stream_t *stream, GNUC_UNUSED int flags, GNUC_UNUSED snd_feed_callback_t feed)
{
    /* FIXME */
    return SND_STREAM_RET_OK;
}

/*
=================
snd_flac_load
=================
*/
GNUC_NONNULL static bool snd_flac_load (const char   *name,
                                        snd_stream_t *stream)
{
    fs_file_t                     f;
    int                           file_size;
    FLAC__StreamDecoder          *dec = NULL;
    FLAC__StreamDecoderInitStatus status;

    if (NULL == (f = fs_open(name, FS_RDONLY, &file_size, 0)))
        return false;

    if (NULL == (dec = FLAC__stream_decoder_new()))
    {
        sys_printf("eflac_decoder_new failed\n");
        goto error;
    }

    stream->file    = f;
    stream->private = dec;

    status = FLAC__stream_decoder_init_stream(dec,
                                              &eflac_decoder_read,
                                              &eflac_decoder_seek,
                                              &eflac_decoder_tell,
                                              &eflac_decoder_length,
                                              &eflac_decoder_eof,
                                              &eflac_decoder_write,
                                              &eflac_decoder_metadata,
                                              &eflac_decoder_error,
                                              stream);

    if (FLAC__STREAM_DECODER_INIT_STATUS_OK != status)
    {
        eflac_decoder_print_init_status(status);
        goto error;
    }

    if (!FLAC__stream_decoder_process_until_end_of_metadata(dec))
    {
        sys_printf("eflac_decoder_process_until_eom failed\n");
        goto error;
    }

    if (!stream->stream_data_size || -1 == stream->stream_data_format)
    {
        sys_printf("invalid or unsupported flac file\n");
        goto error;
    }

    stream->streaming = stream->stream_data_size > PCM_MAX_NONSTREAMING;

    if (stream->streaming)
    {
        sys_printf("\"%s\" should be streamed\n", name);

        stream->unload = &snd_flac_stream_unload;
        stream->stream = &snd_flac_stream_func;

        /* FIXME */
        goto error;
    }
    else
    {
        stream->data_size = 0;

        if (NULL == (stream->data = mem_alloc(snd_mempool, stream->stream_data_size)))
        {
            sys_printf("failed to allocate %i bytes\n", stream->data_size);
            goto error;
        }

        if (!FLAC__stream_decoder_process_until_end_of_stream(dec))
        {
            sys_printf("eflac_decoder_process_until_eos failed\n");
            goto error;
        }

        if (!FLAC__stream_decoder_finish(dec))
        {
            sys_printf("eflac_decoder_finish failed\n");
            //goto error;
        }

        FLAC__stream_decoder_delete(dec);
    }

    return true;

error:
    sys_printf("failed to load \"%s\"\n", name);

    if (NULL != stream->data)
        mem_free(stream->data);

    if (NULL != stream->file)
        fs_close(stream->file);

    if (NULL != dec)
        FLAC__stream_decoder_delete(dec);

    return false;
}

static const char * const snd_flac_extensions[] = { "flac", NULL };

const snd_plugin_t snd_plugin_flac =
{
    .name = "snd_flac",
    .extensions = snd_flac_extensions,
    .load = snd_flac_load,
};

#endif /* ENGINE_SND_FLAC */
