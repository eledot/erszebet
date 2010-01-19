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

#ifdef ENGINE_SND

#ifdef ENGINE_OS_IPHONE
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <al.h>
#include <alc.h>
#endif

#include "sound/snd_private.h"

typedef struct sound_s
{
    char name[MISC_MAX_FILENAME];

    ALuint al_buffer;
    ALuint al_buffer2;

    erbool streaming;
    snd_stream_t stream;

    struct sound_s *next;
    struct sound_s *prev;
}sound_t;

mem_pool_t snd_mempool;

static erbool snd_i = false;
static erbool alc_done = false;

static ALCdevice *snd_device;
static ALCcontext *snd_context;

static cvar_t *s_volume;

static sound_t *sounds;
static sound_t *sounds_streaming;

static const char *extensions;

/* sound plugins */
extern const snd_plugin_t snd_plugin_wav;
extern const snd_plugin_t snd_plugin_ogg;
extern const snd_plugin_t snd_plugin_flac;

static const snd_plugin_t * const snd_plugins[] =
{
#ifdef ENGINE_SND_WAV
    &snd_plugin_wav,
#endif
#ifdef ENGINE_SND_OGG
    &snd_plugin_ogg,
#endif
#ifdef ENGINE_SND_FLAC
    &snd_plugin_flac
#endif
};

enum { snd_plugins_num = STSIZE(snd_plugins) };

static erbool snd_plugins_usable[snd_plugins_num];

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
snd_load_internal
=================
*/
static GNUC_NONNULL erbool snd_load_internal (const char *name, sound_t *sound)
{
    int i;
    const char * const *exts;
    char tmp[MISC_MAX_FILENAME];

    memset(&sound->stream, 0, sizeof(sound->stream));

    for (i = 0; i < snd_plugins_num ;i++)
    {
        if (!snd_plugins_usable[i] || NULL == snd_plugins[i]->load)
            continue;

        for (exts = snd_plugins[i]->extensions; NULL != exts && NULL != *exts ;exts++)
        {
            snprintf(tmp, sizeof(tmp), "%s.%s", name, *exts);

            memset(&sound->stream, 0, sizeof(sound->stream));

            if (snd_plugins[i]->load(tmp, &sound->stream))
            {
                strlcpy(sound->name, name, sizeof(sound->name));
                return true;
            }
        }
    }

    return false;
}

/*
=================
snd_load
=================
*/
snd_sound_t snd_load (const char *name, GNUC_UNUSED int flags)
{
    sound_t sound, *sound_allocated = NULL;

    if (!snd_i)
        return NULL;

    if (!snd_load_internal(name, &sound))
        goto error;

    alGenBuffers(1, &sound.al_buffer);
    ALERROR();

    if (al_error)
    {
        sys_printf("alGenBuffers failed\n");
        goto error;
    }

    if (!sound.streaming)
    {
        alBufferData(sound.al_buffer,
                     sound.stream.stream_data_format,
                     sound.stream.data,
                     sound.stream.data_size,
                     sound.stream.stream_data_rate);
        ALERROR();

        if (al_error)
        {
            sys_printf("alBufferData failed\n");
            goto error;
        }

        mem_free(sound.stream.data);
        sound.stream.data = NULL;
    }
    else
    {
        /* FIXME -- streaming sounds */
    }

    if (NULL == (sound_allocated = mem_alloc(snd_mempool, sizeof(sound))))
    {
        sys_printf("failed to allocate sound\n");
        goto error;
    }

    memcpy(sound_allocated, &sound, sizeof(sound));

    return sound_allocated;

error:
    sys_printf("failed to load \"%s\"\n", name);

    if (NULL != sound.stream.data)
    {
        mem_free(sound.stream.data);
        sound.stream.data = NULL;

        if (sound.streaming)
            sound.stream.unload(&sound.stream);
    }

    if (NULL != sound_allocated)
        mem_free(sound_allocated);

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
erbool snd_init (void)
{
    const char *s;
    char tmp[16];
    int i;

    sounds = sounds_streaming = NULL;

    /* sound volume (0.0 - 1.0) */
    s_volume = cvar_get("s_volume", "1", CVAR_FL_SAVE);
    cvar_set_minmax(s_volume, 0.0, 1.0f);
    s_volume->callback = &s_volume_callback;

    if (sys_arg_find("-nosound"))
        return true;

    /* open default device */
    snd_device = alcOpenDevice(NULL);
    ALERROR();

    if (NULL == snd_device)
    {
        sys_printf("failed to open default sound device\n");
        return false;
    }

    /* create default context */
    snd_context = alcCreateContext(snd_device, NULL);
    ALERROR();

    if (NULL == snd_context)
    {
        sys_printf("failed to create sound context\n");
        return false;
    }

    /* set current context. alGetError becomes available, btw */
    alcMakeContextCurrent(snd_context);
    alc_done = true;
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

    snd_mempool = mem_alloc_pool("snd", 0);

    for (i = 0; i < snd_plugins_num ;i++)
    {
        snprintf(tmp, sizeof(tmp), "-no%s", snd_plugins[i]->name);

        snd_plugins_usable[i] = !sys_arg_find(tmp) && (NULL == snd_plugins[i]->init ||
                                                       snd_plugins[i]->init());

        if (snd_plugins_usable[i])
        {
            sys_printf("+%s\n", snd_plugins[i]->name);
        }
    }

    snd_i = true;

    /* set volume */
    s_volume_callback(s_volume);

    sys_printf("+snd\n");

    return true;
}

/*
=================
snd_shutdown
=================
*/
void snd_shutdown (void)
{
    int i;

    if (!snd_i)
        return;

    alcMakeContextCurrent(NULL);
    alc_done = false; /* no context here -- can't use alGetError */
    ALERROR();
    alcDestroyContext(snd_context);
    ALERROR();
    alcCloseDevice(snd_device);
    ALERROR();

    mem_free_pool(&snd_mempool);

    for (i = 0; i < snd_plugins_num ;i++)
    {
        if (snd_plugins_usable[i])
        {
            if (NULL != snd_plugins[i]->shutdown)
                snd_plugins[i]->shutdown();

            sys_printf("-%s\n", snd_plugins[i]->name);
        }
    }

    snd_i = false;

    sys_printf("-snd\n");
}

#else /* !ENGINE_SND */

#include "snd_private.h"

void snd_set_listener_pos (GNUC_UNUSED const float *pos) { }
void snd_set_listener_velocity (GNUC_UNUSED const float *vel) { }
int snd_get_stream_format (GNUC_UNUSED int bps, GNUC_UNUSED int channels) { return -1; }
snd_sound_t snd_load (GNUC_UNUSED const char *name, GNUC_UNUSED int flags) { return NULL; }
void snd_unload (GNUC_UNUSED snd_sound_t *sound) { }
void snd_frame (void) { }

erbool snd_init (void) { return true; }
void snd_shutdown (void) { }

#endif /* ENGINE_SND */
