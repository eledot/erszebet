/*
   Copyright (C) 2009 Serge "ftrvxmtrx" Ziryukin

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General
   Public License along with this library; if not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301 USA
*/

/**
 * @file snd_flac_funcs.h
 *
 * @brief snd_flac.c private header with FLAC funcs
 *
 * @author ftrvxmtrx
 */

#ifndef _SND_FLAC_FUNCS_H
#define _SND_FLAC_FUNCS_H

#ifndef FLAC_API_VERSION_CURRENT
#error This is internal header!
#endif

static FLAC__StreamDecoder *         (*eflac_decoder_new)                 (void);
static void                          (*eflac_decoder_delete)              (FLAC__StreamDecoder *decoder);
static FLAC__StreamDecoderState      (*eflac_decoder_get_state)           (const FLAC__StreamDecoder *decoder);
static FLAC__uint64                  (*eflac_decoder_get_total_samples)   (const FLAC__StreamDecoder *decoder);
static unsigned                      (*eflac_decoder_get_channels)        (const FLAC__StreamDecoder *decoder);
static unsigned                      (*eflac_decoder_get_bps)             (const FLAC__StreamDecoder *decoder);
static unsigned                      (*eflac_decoder_get_sample_rate)     (const FLAC__StreamDecoder *decoder);
static unsigned                      (*eflac_decoder_get_blocksize)       (const FLAC__StreamDecoder *decoder);
static FLAC__bool                    (*eflac_decoder_get_decode_position) (const FLAC__StreamDecoder *decoder, FLAC__uint64 *position);
static FLAC__StreamDecoderInitStatus (*eflac_decoder_init_stream)         (FLAC__StreamDecoder *decoder,
                                                                           FLAC__StreamDecoderReadCallback read_callback,
                                                                           FLAC__StreamDecoderSeekCallback seek_callback,
                                                                           FLAC__StreamDecoderTellCallback tell_callback,
                                                                           FLAC__StreamDecoderLengthCallback length_callback,
                                                                           FLAC__StreamDecoderEofCallback eof_callback,
                                                                           FLAC__StreamDecoderWriteCallback write_callback,
                                                                           FLAC__StreamDecoderMetadataCallback metadata_callback,
                                                                           FLAC__StreamDecoderErrorCallback error_callback,
                                                                           void *client_data);
static FLAC__bool                    (*eflac_decoder_finish)              (FLAC__StreamDecoder *decoder);
static FLAC__bool                    (*eflac_decoder_flush)               (FLAC__StreamDecoder *decoder);
static FLAC__bool                    (*eflac_decoder_reset)               (FLAC__StreamDecoder *decoder);
static FLAC__bool                    (*eflac_decoder_process_until_eom)   (FLAC__StreamDecoder *decoder);
static FLAC__bool                    (*eflac_decoder_process_until_eos)   (FLAC__StreamDecoder *decoder);
static FLAC__bool                    (*eflac_decoder_seek_absolute)       (FLAC__StreamDecoder *decoder, FLAC__uint64 sample);

static lib_func_t funcs[] =
{
    { "FLAC__stream_decoder_new",                           (void **)&eflac_decoder_new                 },
    { "FLAC__stream_decoder_delete",                        (void **)&eflac_decoder_delete              },
    { "FLAC__stream_decoder_get_state",                     (void **)&eflac_decoder_get_state           },
    { "FLAC__stream_decoder_get_total_samples",             (void **)&eflac_decoder_get_total_samples   },
    { "FLAC__stream_decoder_get_channels",                  (void **)&eflac_decoder_get_channels        },
    { "FLAC__stream_decoder_get_bits_per_sample",           (void **)&eflac_decoder_get_bps             },
    { "FLAC__stream_decoder_get_sample_rate",               (void **)&eflac_decoder_get_sample_rate     },
    { "FLAC__stream_decoder_get_blocksize",                 (void **)&eflac_decoder_get_blocksize       },
    { "FLAC__stream_decoder_get_decode_position",           (void **)&eflac_decoder_get_decode_position },
    { "FLAC__stream_decoder_init_stream",                   (void **)&eflac_decoder_init_stream         },
    { "FLAC__stream_decoder_finish",                        (void **)&eflac_decoder_finish              },
    { "FLAC__stream_decoder_flush",                         (void **)&eflac_decoder_flush               },
    { "FLAC__stream_decoder_reset",                         (void **)&eflac_decoder_reset               },
    { "FLAC__stream_decoder_process_until_end_of_metadata", (void **)&eflac_decoder_process_until_eom   },
    { "FLAC__stream_decoder_process_until_end_of_stream",   (void **)&eflac_decoder_process_until_eos   },
    { "FLAC__stream_decoder_seek_absolute",                 (void **)&eflac_decoder_seek_absolute       },
    {NULL, NULL}
};

#endif /* !_SND_FLAC_FUNCS_H */
