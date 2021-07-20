#line 2 "audio.cpp"
/*  audio
 * 
 *  Created for Ludum Dare #24
 *
 *  Created on: 29/08/2012 time8:32:30 PM
 *      Author: Lasty
 */

#include "audio.h"

#include "globals.h"

#include <SDL2/SDL_audio.h>

#include <string>
#include <list>


Wave::Wave(const char* filename)
{
        std::string FN = std::string(DATA_DIR) + std::string(filename);

        if (SDL_LoadWAV( FN.c_str(), &spec, &buffer, &length) == nullptr)
        {
                THROW(Exception, "Could not load wav");
        }
        LOGf("WAV '%s' Loaded:  Buf: %p  Len: %d", filename, buffer, length);
}

Wave::~Wave()
{
        SDL_FreeWAV(buffer);
        buffer = nullptr;
}


Channel::Channel(Wave* sample, float volume)
: waveref(sample), volume(volume), audio_pos(sample->buffer), audio_len(sample->length)
{

}

void Channel::Mix(Uint8* stream, int len)
{
        if (audio_len <= 0)
        {
                done = true;
                return;
        }
        len = len > audio_len ? audio_len : len;
        //SDL_memcpy (stream, audio_pos, len);
        SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME * volume);
        audio_pos += len;
        audio_len -= len;
}


Audio::Audio()
{
        //Copy wavspec from one of the sounds
        SDL_AudioSpec wav_spec;

        //wav_spec = startup.spec;

        wav_spec.freq = 44100;
        wav_spec.format = AUDIO_U16SYS;
        wav_spec.channels = 2;
        wav_spec.callback = audio_callback;
        wav_spec.userdata = nullptr;
        wav_spec.samples = 512;

        SDL_AudioSpec got;

        //SDL_Init(SDL_INIT_AUDIO);

        if ( SDL_OpenAudio(&wav_spec, &got) < 0) {
                THROW(Exception, "Could not open audio");
        }

        SDL_PauseAudio(0);
}

Audio::~Audio()
{
        SDL_PauseAudio(1);

        SDL_CloseAudio();
}


std::list<Channel> Mixer;


void Audio::PlaySound(Wave *sample, float volume)
{
    SDL_LockAudio();
        Mixer.push_back(Channel(sample, volume));
    SDL_UnlockAudio();
}

void Audio::audio_callback(void *unused, Uint8 *stream, int len)
{
        SDL_memset(stream, 0, len);

        //LOGf("Mixer: Channels: %d", Mixer.size());

        for( auto &chan : Mixer )
        {
                chan.Mix(stream, len);
        }

        for(auto i = Mixer.begin(); i != Mixer.end(); )
        {
                if (i->done)
                {
                        Mixer.erase(i);
                        i = Mixer.begin();
                }
                else
                {
                        i++;
                }
        }
}

