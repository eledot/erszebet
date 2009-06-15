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
 * @file snd_openal_funcs.h
 *
 * @brief snd.c private header with OpenAL funcs
 *
 * @author ftrvxmtrx
 */

#ifndef _SND_OPENAL_FUNCS_H
#define _SND_OPENAL_FUNCS_H

#ifndef OPENAL
#error This is internal header!
#endif

static void            (*eal_enable)                 (ALenum capability);
static void            (*eal_disable)                (ALenum capability);
static ALboolean       (*eal_is_enabled)             (ALenum capability);
static const ALchar *  (*eal_get_string)             (ALenum param);
static ALboolean       (*eal_get_boolean)            (ALenum param);
static ALint           (*eal_get_integer)            (ALenum param);
static ALfloat         (*eal_get_float)              (ALenum param);
static ALdouble        (*eal_get_double)             (ALenum param);
static ALenum          (*eal_get_error)              (void);
static ALboolean       (*eal_is_extension_present)   (const ALchar *extname);
static void *          (*eal_get_proc_address)       (const ALchar *fname);
static ALenum          (*eal_get_enum_value)         (const ALchar *ename);
static void            (*eal_listenerf)              (ALenum param, ALfloat value);
static void            (*eal_listener3f)             (ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
static void            (*eal_listenerfv)             (ALenum param, const ALfloat *values);
static void            (*eal_get_listenerf)          (ALenum param, ALfloat *value);
static void            (*eal_get_listener3f)         (ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3);
static void            (*eal_get_listenerfv)         (ALenum param, ALfloat *values);
static void            (*eal_gen_sources)            (ALsizei n, ALuint* sources);
static void            (*eal_delete_sources)         (ALsizei n, const ALuint* sources);
static ALboolean       (*eal_is_source)              (ALuint sid);
static void            (*eal_sourcef)                (ALuint sid, ALenum param, ALfloat value);
static void            (*eal_source3f)               (ALuint sid, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
static void            (*eal_sourcefv)               (ALuint sid, ALenum param, const ALfloat *values);
static void            (*eal_sourcei)                (ALuint sid, ALenum param, ALint value);
static void            (*eal_get_sourcef)            (ALuint sid, ALenum param, ALfloat *value);
static void            (*eal_get_source3f)           (ALuint sid, ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3);
static void            (*eal_source_playv)           (ALsizei ns, const ALuint *sids);
static void            (*eal_source_stopv)           (ALsizei ns, const ALuint *sids);
static void            (*eal_source_rewindv)         (ALsizei ns, const ALuint *sids);
static void            (*eal_source_pausev)          (ALsizei ns, const ALuint *sids);
static void            (*eal_source_play)            (ALuint sid);
static void            (*eal_source_stop)            (ALuint sid);
static void            (*eal_source_rewind)          (ALuint sid);
static void            (*eal_source_pause)           (ALuint sid);
static void            (*eal_source_queue_buffers)   (ALuint sid, ALsizei numEntries, const ALuint *bids);
static void            (*eal_source_unqueue_buffers) (ALuint sid, ALsizei numEntries, ALuint *bids);
static void            (*eal_gen_buffers)            (ALsizei n, ALuint *buffers);
static void            (*eal_delete_buffers)         (ALsizei n, const ALuint *buffers);
static ALboolean       (*eal_is_buffer)              (ALuint bid);
static void            (*eal_buffer_data)            (ALuint bid, ALenum format, const ALvoid *data, ALsizei size, ALsizei freq);
static void            (*eal_bufferf)                (ALuint bid, ALenum param, ALfloat value);
static void            (*eal_buffer3f)               (ALuint bid, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
static void            (*eal_get_bufferf)            (ALuint bid, ALenum param, ALfloat* value);
static void            (*eal_get_buffer3f)           (ALuint bid, ALenum param, ALfloat* value1, ALfloat* value2, ALfloat* value3);
static void            (*eal_doppler_factor)         (ALfloat value);
static void            (*eal_doppler_velocity)       (ALfloat value);
static void            (*eal_speed_of_sound)         (ALfloat value);
static void            (*eal_distance_model)         (ALenum distanceModel);
static ALCcontext *    (*ealc_create_context)        (ALCdevice *device, const ALCint* attrlist);
static ALCboolean      (*ealc_make_context_current)  (ALCcontext *context);
static void            (*ealc_process_context)       (ALCcontext *context);
static void            (*ealc_suspend_context)       (ALCcontext *context);
static void            (*ealc_destroy_context)       (ALCcontext *context);
static ALCcontext *    (*ealc_get_current_context)   (void);
static ALCdevice *     (*ealc_get_contexts_device)   (ALCcontext *context);
static ALCdevice *     (*ealc_open_device)           (const ALCchar *devicename);
static ALCboolean      (*ealc_close_device)          (ALCdevice *device);
static ALCenum         (*ealc_get_error)             (ALCdevice *device);
static ALCboolean      (*ealc_is_extension_present)  (ALCdevice *device, const ALCchar *extname);
static void *          (*ealc_get_proc_address)      (ALCdevice *device, const ALCchar *funcname);
static ALCenum         (*ealc_get_enum_value)        (ALCdevice *device, const ALCchar *enumname);
static const ALCchar * (*ealc_get_string)            (ALCdevice *device, ALCenum param);
static void            (*ealc_get_integerv)          (ALCdevice *device, ALCenum param, ALCsizei size, ALCint *data);
static ALCdevice *     (*ealc_capture_open_device)   (const ALCchar *devicename, ALCuint frequency, ALCenum format, ALCsizei buffersize);
static ALCboolean      (*ealc_capture_close_device)  (ALCdevice *device);
static void            (*ealc_capture_start)         (ALCdevice *device);
static void            (*ealc_capture_stop)          (ALCdevice *device);
static void            (*ealc_capture_samples)       (ALCdevice *device, ALCvoid *buffer, ALCsizei samples);

static lib_func_t funcs[] =
{
    { "alEnable",               (void **)&eal_enable                 },
    { "alDisable",              (void **)&eal_disable                },
    { "alIsEnabled",            (void **)&eal_is_enabled             },
    { "alGetString",            (void **)&eal_get_string             },
    { "alGetBoolean",           (void **)&eal_get_boolean            },
    { "alGetInteger",           (void **)&eal_get_integer            },
    { "alGetFloat",             (void **)&eal_get_float              },
    { "alGetDouble",            (void **)&eal_get_double             },
    { "alGetError",             (void **)&eal_get_error              },
    { "alIsExtensionPresent",   (void **)&eal_is_extension_present   },
    { "alGetProcAddress",       (void **)&eal_get_proc_address       },
    { "alGetEnumValue",         (void **)&eal_get_enum_value         },
    { "alListenerf",            (void **)&eal_listenerf              },
    { "alListener3f",           (void **)&eal_listener3f             },
    { "alListenerfv",           (void **)&eal_listenerfv             },
    { "alGetListenerf",         (void **)&eal_get_listenerf          },
    { "alGetListener3f",        (void **)&eal_get_listener3f         },
    { "alGetListenerfv",        (void **)&eal_get_listenerfv         },
    { "alGenSources",           (void **)&eal_gen_sources            },
    { "alDeleteSources",        (void **)&eal_delete_sources         },
    { "alIsSource",             (void **)&eal_is_source              },
    { "alSourcef",              (void **)&eal_sourcef                },
    { "alSource3f",             (void **)&eal_source3f               },
    { "alSourcefv",             (void **)&eal_sourcefv               },
    { "alSourcei",              (void **)&eal_sourcei                },
    { "alGetSourcef",           (void **)&eal_get_sourcef            },
    { "alGetSource3f",          (void **)&eal_get_source3f           },
    { "alSourcePlayv",          (void **)&eal_source_playv           },
    { "alSourceStopv",          (void **)&eal_source_stopv           },
    { "alSourceRewindv",        (void **)&eal_source_rewindv         },
    { "alSourcePlayv",          (void **)&eal_source_pausev          },
    { "alSourcePlay",           (void **)&eal_source_play            },
    { "alSourceStop",           (void **)&eal_source_stop            },
    { "alSourceRewind",         (void **)&eal_source_rewind          },
    { "alSourcePause",          (void **)&eal_source_pause           },
    { "alSourceQueueBuffers",   (void **)&eal_source_queue_buffers   },
    { "alSourceUnqueueBuffers", (void **)&eal_source_unqueue_buffers },
    { "alGenBuffers",           (void **)&eal_gen_buffers            },
    { "alDeleteBuffers",        (void **)&eal_delete_buffers         },
    { "alIsBuffer",             (void **)&eal_is_buffer              },
    { "alBufferData",           (void **)&eal_buffer_data            },
    { "alBufferf",              (void **)&eal_bufferf                },
    { "alBuffer3f",             (void **)&eal_buffer3f               },
    { "alGetBufferf",           (void **)&eal_get_bufferf            },
    { "alGetBuffer3f",          (void **)&eal_get_buffer3f           },
    { "alDopplerFactor",        (void **)&eal_doppler_factor         },
    { "alDopplerVelocity",      (void **)&eal_doppler_velocity       },
    { "alSpeedOfSound",         (void **)&eal_speed_of_sound         },
    { "alDistanceModel",        (void **)&eal_distance_model         },
    { "alcCreateContext",       (void **)&ealc_create_context        },
    { "alcMakeContextCurrent",  (void **)&ealc_make_context_current  },
    { "alcProcessContext",      (void **)&ealc_process_context       },
    { "alcSuspendContext",      (void **)&ealc_suspend_context       },
    { "alcDestroyContext",      (void **)&ealc_destroy_context       },
    { "alcGetCurrentContext",   (void **)&ealc_get_current_context   },
    { "alcGetContextsDevice",   (void **)&ealc_get_contexts_device   },
    { "alcOpenDevice",          (void **)&ealc_open_device           },
    { "alcCloseDevice",         (void **)&ealc_close_device          },
    { "alcGetError",            (void **)&ealc_get_error             },
    { "alcIsExtensionPresent",  (void **)&ealc_is_extension_present  },
    { "alcGetProcAddress",      (void **)&ealc_get_proc_address      },
    { "alcGetEnumValue",        (void **)&ealc_get_enum_value        },
    { "alcGetString",           (void **)&ealc_get_string            },
    { "alcGetIntegerv",         (void **)&ealc_get_integerv          },
    { "alcCaptureOpenDevice",   (void **)&ealc_capture_open_device   },
    { "alcCaptureCloseDevice",  (void **)&ealc_capture_close_device  },
    { "alcCaptureStart",        (void **)&ealc_capture_start         },
    { "alcCaptureStop",         (void **)&ealc_capture_stop          },
    { "alcCaptureSamples",      (void **)&ealc_capture_samples       },
    { NULL, NULL }
};

#endif /* !_SND_OPENAL_FUNCS_H */
