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

#include "common.h"

#ifdef ENGINE_SND

#include <al.h>
#include <alc.h>

#include "snd_wav.h"
#include "snd_ogg.h"
#include "snd_flac.h"

typedef struct sound_s
{
    char   name[MISC_MAX_FILENAME];
    ALuint al_buffer;
    ALuint al_buffer2;

    int          streaming : 1;
    snd_stream_t stream;

    struct sound_s *next;
    struct sound_s *prev;
}sound_t;

static int snd_i = 0;
static int alc_done = 0;

static ALCdevice  *snd_device;
static ALCcontext *snd_context;

static cvar_t *s_volume;

static sound_t *sounds;
static sound_t *sounds_streaming;
static mem_pool_t mempool;

static const char *extensions;

static const struct
{
    char  ext[8];
    int (*func) (const char   *name,
                 snd_stream_t *stream,
                 int          *streaming,
                 mem_pool_t    pool);
}loaders[] =
{
    { "wav",  (void *)&snd_wav_load  },
    { "ogg",  (void *)&snd_ogg_load  },
    { "flac", (void *)&snd_flac_load }
};

#ifdef ENGINE_SND_DEBUG

static int al_error;

/*
=================
snd_check_error
=================
*/
static void snd_check_error (const char *func, const char *file, int line)
{
    const char *msg;
    int         err;

    al_error = 0;

    if (alc_done)
    {
        err = alGetError();

        if (AL_NO_ERROR != err)
        {
            switch (err)
            {
            case AL_INVALID_NAME:      msg = "invalid name";      break;
            case AL_INVALID_ENUM:      msg = "invalid enum";      break;
            case AL_INVALID_VALUE:     msg = "invalid value";     break;
            case AL_INVALID_OPERATION: msg = "invalid operation"; break;
            case AL_OUT_OF_MEMORY:     msg = "out of memory";     break;
            default:                   msg = "unknown";           break;
            }

            al_error = err;

            sys_printf("\"%s\" al error in %s (%s:%i)\n", msg, func, file, line);
        }
    }

    err = alcGetError(snd_device);

    if (ALC_NO_ERROR != err)
    {
        switch (err)
        {
        case ALC_INVALID_DEVICE:   msg = "invalid device";  break;
        case ALC_INVALID_CONTEXT:  msg = "invalid context"; break;
        case ALC_INVALID_ENUM:     msg = "invalid enum";    break;
        case ALC_INVALID_VALUE:    msg = "invalid value";   break;
        case ALC_OUT_OF_MEMORY:    msg = "out of memory";   break;
        default:                   msg = "unknown";         break;
        }

        al_error = err;

        sys_printf("\"%s\" alc error in %s (%s:%i)\n", msg, func, file, line);
    }
}

#define ALERROR() snd_check_error(__FUNCTION__, __FILE__, __LINE__)

#else /* !ENGINE_SND_DEBUG */

#define ALERROR()
#define al_error alGetError()

#endif /* ENGINE_SND_DEBUG */

/*
=================
s_volume_callback
=================
*/
static void s_volume_callback (const cvar_t *cvar)
{
    if (!snd_i)
        return;

    alListenerf(AL_GAIN, cvar->f);
    ALERROR();
}

/*
=================
snd_set_listener_pos
=================
*/
void snd_set_listener_pos (const float *pos)
{
    if (!snd_i)
        return;

    if (NULL == pos)
    {
        sys_printf("NULL listener position\n");
        return;
    }

    alListener3f(AL_POSITION, pos[0], pos[1], pos[2]);
    ALERROR();
}

/*
=================
snd_set_listener_orientation
=================
*/
void snd_set_listener_orientation (const float *ori)
{
    if (!snd_i)
        return;

    if (NULL == ori)
    {
        sys_printf("NULL listener orientation\n");
        return;
    }

    alListenerfv(AL_ORIENTATION, ori);
    ALERROR();
}

/*
=================
snd_set_listener_velocity
=================
*/
void snd_set_listener_velocity (const float *vel)
{
    if (!snd_i)
        return;

    if (NULL == vel)
    {
        sys_printf("NULL listener velocity\n");
        return;
    }

    alListenerfv(AL_VELOCITY, vel);
    ALERROR();
}

/*
=================
snd_get_stream_format
=================
*/
int snd_get_stream_format (int bps, int channels)
{
    if (16 == bps)
    {
        switch (channels)
        {
        case 1:  return AL_FORMAT_MONO16;
        case 2:  return AL_FORMAT_STEREO16;
        default: break;
        }
    }
    else if (8 == bps)
    {
        switch (channels)
        {
        case 1:  return AL_FORMAT_MONO8;
        case 2:  return AL_FORMAT_STEREO8;
        default: break;
        }
    }

    return -1;
}

/*
=================
snd_load
=================
*/
snd_sound_t snd_load (const char *name, int flags UV)
{
    int      i, streaming;
    sound_t *sound;
    char     tmp[MISC_MAX_FILENAME];

    if (!snd_i)
        return NULL;

    if (NULL == (sound = mem_alloc_static(sizeof(*sound))))
    {
        sys_printf("failed to allocate %i bytes for sound\n", sizeof(*sound));
        return NULL;
    }

    for (i = 0; i < STSIZE(loaders) ;i++)
    {
        snprintf(tmp, sizeof(tmp), "%s.%s", name, loaders[i].ext);

        if (0 == loaders[i].func(tmp, &sound->stream, &streaming, mempool))
        {
            sys_printf("found \"%s\"\n", name);

            sound->streaming = streaming;

            alGenBuffers(1, &sound->al_buffer);
            ALERROR();

            if (al_error)
            {
                sys_printf("alGenBuffers failed\n");
                goto error;
            }

            if (!streaming)
            {
                alBufferData(sound->al_buffer,
                             sound->stream.stream_data_format,
                             sound->stream.data,
                             sound->stream.data_size,
                             sound->stream.stream_data_rate);
                ALERROR();

                if (al_error)
                {
                    sys_printf("alBufferData failed\n");
                    goto error;
                }

                mem_free(sound->stream.data);
                sound->stream.data = NULL;
            }
            else
            {
            }

            return sound;
        }
    }

error:
    sys_printf("failed to load \"%s\"\n", name);

    if (NULL != sound)
    {
        if (NULL != sound->stream.data)
            mem_free(sound->stream.data);

        if (sound->streaming)
            sound->stream.unload(&sound->stream);

        mem_free(sound);
    }

    return NULL;
}

/*
=================
snd_unload
=================
*/
void snd_unload (snd_sound_t *sound)
{
    if (NULL == sound)
    {
        sys_printf("NULL sound\n");
        return;
    }
}

/*
=================
snd_frame
=================
*/
void snd_frame (void)
{
    sound_t *s;

    if (!snd_i)
        return;

    for (s = sounds_streaming; NULL != s ;s = s->next)
    {
        /* FIXME */
    }
}

/*
=================
snd_init
=================
*/
int snd_init (void)
{
    const char *s;

    alc_done = 0;
    sounds = sounds_streaming = NULL;

    /* sound volume (0.0 - 1.0) */
    s_volume = cvar_get("s_volume", "1", CVAR_FL_SAVE);
    cvar_set_minmax(s_volume, 0.0, 1.0f);
    s_volume->callback = &s_volume_callback;

    if (sys_arg_find("-nosound"))
        return 0;

    /* open default device */
    snd_device = alcOpenDevice(NULL);
    ALERROR();

    if (NULL == snd_device)
    {
        sys_printf("failed to open default sound device\n");
        return -2;
    }

    /* create default context */
    snd_context = alcCreateContext(snd_device, NULL);
    ALERROR();

    if (NULL == snd_context)
    {
        sys_printf("failed to create sound context\n");
        return -3;
    }

    /* set current context. alGetError becomes available, btw */
    alcMakeContextCurrent(snd_context);
    alc_done = 1;
    ALERROR();

    errno = 0;

    /* print some info about OpenAL */
    s = alGetString(AL_VENDOR);
    sys_printf("openal vendor: %s\n", NULL != s ? s : "undefined");
    ALERROR();
    s = alGetString(AL_VERSION);
    sys_printf("openal version: %s\n", NULL != s ? s : "undefined");
    ALERROR();
    s = alGetString(AL_RENDERER);
    sys_printf("openal renderer: %s\n", NULL != s ? s : "undefined");
    ALERROR();
    s = extensions = alGetString(AL_EXTENSIONS);
    sys_printf("openal extensions: %s\n", NULL != s ? s : "undefined");
    ALERROR();

    /* initialize sound formats support */
    snd_wav_init();
    snd_ogg_init();
    snd_flac_init();

    mem_alloc_static_pool("snd", 0);

    snd_i = 1;

    /* set volume */
    s_volume_callback(s_volume);

    sys_printf("+snd\n");

    return 0;
}

/*
=================
snd_shutdown
=================
*/
void snd_shutdown (void)
{
    if (!snd_i)
        return;

    alcMakeContextCurrent(NULL);
    alc_done = 0; /* no context here -- can't use alGetError */
    ALERROR();
    alcDestroyContext(snd_context);
    ALERROR();
    alcCloseDevice(snd_device);
    ALERROR();

    mem_free_static_pool();

    snd_wav_shutdown();
    snd_ogg_shutdown();
    snd_flac_shutdown();

    snd_i = 0;

    sys_printf("-snd\n");
}

#else

void snd_set_listener_pos (const float *pos UV) { }
void snd_set_listener_velocity (const float *vel UV) { }
int snd_get_stream_format (int bps UV, int channels UV) { return -1; }
snd_sound_t snd_load (const char *name UV, int flags UV) { return NULL; }
void snd_unload (snd_sound_t *sound UV) { }
void snd_frame (void) { }

int snd_init (void) { return 0; }
void snd_shutdown (void) { }

#endif /* ENGINE_SND */
