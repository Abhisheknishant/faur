/*
    Copyright 2010, 2016-2019 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "f_sdl_sound.v.h"
#include <faur.v.h>

#if F_CONFIG_SOUND_ENABLED && F_CONFIG_LIB_SDL
#if F_CONFIG_LIB_SDL == 1
    #include <SDL/SDL.h>
    #include <SDL/SDL_mixer.h>
#elif F_CONFIG_LIB_SDL == 2
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_mixer.h>
#endif

#if F_CONFIG_SYSTEM_EMSCRIPTEN && F_CONFIG_LIB_SDL == 1
    #define F__SOUND_NO_VOLUME_ADJUSTMENT 1
#endif

static bool g_enabled;
static bool g_mute = F_CONFIG_SOUND_MUTE;
static int g_numSampleChannels;
static int g_numSampleChannelsReserved;
static int g_currentSampleChannel;

void f_platform_sdl_sound__init(void)
{
    if(SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        F__FATAL("SDL_InitSubSystem: %s", SDL_GetError());
    }

    if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 256) != 0) {
        f_out__error("Mix_OpenAudio: %s", Mix_GetError());
        return;
    }

    g_enabled = true;

    g_numSampleChannels =
        Mix_AllocateChannels(F_CONFIG_SOUND_SAMPLE_CHANNELS_TOTAL);

    if(g_numSampleChannels < 1) {
        f_out__error("Mix_AllocateChannels: %s", Mix_GetError());
    }

    g_numSampleChannelsReserved =
        Mix_ReserveChannels(F_CONFIG_SOUND_SAMPLE_CHANNELS_RESERVED);

    if(g_numSampleChannelsReserved < 1) {
        f_out__error("Mix_ReserveChannels: %s", Mix_GetError());
    }

    f_out__info("Allocated %d sample channels, reserved %d",
                g_numSampleChannels,
                g_numSampleChannelsReserved);
}

void f_platform_sdl_sound__uninit(void)
{
    if(g_enabled) {
        Mix_CloseAudio();
    }

    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

bool f_platform_api__soundMuteGet(void)
{
    return g_enabled ? g_mute : true;
}

void f_platform_api__soundMuteFlip(void)
{
    if(!g_enabled) {
        return;
    }

    g_mute = !g_mute;

    if(g_mute) {
        Mix_PauseMusic();
    } else if(Mix_PausedMusic()) {
        Mix_ResumeMusic();
    }
}

int f_platform_api__soundVolumeGetMax(void)
{
    return MIX_MAX_VOLUME;
}

APlatformMusic* f_platform_api__soundMusicNew(const char* Path)
{
    if(!g_enabled) {
        return NULL;
    }

    Mix_Music* m = Mix_LoadMUS(Path);

    if(m == NULL) {
        f_out__error("Mix_LoadMUS(%s): %s", Path, Mix_GetError());
    }

    return m;
}

void f_platform_api__soundMusicFree(APlatformMusic* Music)
{
    Mix_FreeMusic(Music);
}

void f_platform_api__soundMusicVolumeSet(int Volume)
{
    if(!g_enabled) {
        return;
    }

    #if F__SOUND_NO_VOLUME_ADJUSTMENT
        F_UNUSED(Volume);
    #else
        Mix_VolumeMusic(Volume);
    #endif
}

void f_platform_api__soundMusicPlay(APlatformMusic* Music)
{
    if(Music == NULL) {
        return;
    }

    if(Mix_PlayMusic(Music, -1) == -1) {
        f_out__error("Mix_PlayMusic: %s", Mix_GetError());
    }
}

void f_platform_api__soundMusicStop(void)
{
    if(!g_enabled) {
        return;
    }

    Mix_HaltMusic();
}

APlatformSample* f_platform_api__soundSampleNewFromFile(const char* Path)
{
    if(!g_enabled) {
        return NULL;
    }

    Mix_Chunk* chunk = Mix_LoadWAV(Path);

    if(chunk == NULL) {
        f_out__error("Mix_LoadWAV(%s): %s", Path, Mix_GetError());
    }

    return chunk;
}

APlatformSample* f_platform_api__soundSampleNewFromData(const uint8_t* Data, int Size)
{
    if(!g_enabled) {
        return NULL;
    }

    SDL_RWops* rw = SDL_RWFromMem((void*)Data, Size);

    if(rw == NULL) {
        f_out__error("SDL_RWFromMem: %s", SDL_GetError());
        return NULL;
    }

    Mix_Chunk* chunk = Mix_LoadWAV_RW(rw, 0);

    if(chunk == NULL) {
        f_out__error("Mix_LoadWAV_RW: %s", Mix_GetError());
    }

    SDL_FreeRW(rw);

    return chunk;
}

void f_platform_api__soundSampleFree(APlatformSample* Sample)
{
    Mix_FreeChunk(Sample);
}

void f_platform_api__soundSampleVolumeSet(APlatformSample* Sample, int Volume)
{
    if(Sample == NULL) {
        return;
    }

    #if F__SOUND_NO_VOLUME_ADJUSTMENT
        F_UNUSED(Sample);
        F_UNUSED(Volume);
    #else
        Mix_VolumeChunk(Sample, Volume);
    #endif
}

void f_platform_api__soundSampleVolumeSetAll(int Volume)
{
    if(!g_enabled) {
        return;
    }

    #if F__SOUND_NO_VOLUME_ADJUSTMENT
        F_UNUSED(Volume);
    #else
        Mix_Volume(-1, Volume);
    #endif
}

void f_platform_api__soundSamplePlay(APlatformSample* Sample, int Channel, bool Loop)
{
    if(Sample == NULL) {
        return;
    }

    if(Mix_PlayChannel(Channel, Sample, Loop ? -1 : 0) == -1) {
        #if F_CONFIG_BUILD_DEBUG
            f_out__error("Mix_PlayChannel(%d): %s", Channel, Mix_GetError());
        #endif
    }
}

void f_platform_api__soundSampleStop(int Channel)
{
    if(!g_enabled) {
        return;
    }

    Mix_HaltChannel(Channel);
}

bool f_platform_api__soundSampleIsPlaying(int Channel)
{
    if(!g_enabled) {
        return false;
    }

    return Mix_Playing(Channel);
}

int f_platform_api__soundSampleChannelGet(void)
{
    if(!g_enabled || g_numSampleChannelsReserved < 1) {
        return -1;
    }

    return g_currentSampleChannel++ % g_numSampleChannelsReserved;
}
#endif // F_CONFIG_SOUND_ENABLED && F_CONFIG_LIB_SDL
