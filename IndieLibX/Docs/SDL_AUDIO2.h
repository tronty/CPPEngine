/*  audio

 * 
 *  Created for Ludum Dare #24
 *
 *  Created on: 29/08/2012 time8:32:30 PM
 *      Author: Lasty
 */

#include "globals.h"

#include <SDL2/SDL_audio.h>

#include <list>

struct Wave
{
        Uint32 length;
        Uint8 *buffer;
        SDL_AudioSpec spec;

        Wave(const char* filename);
        virtual ~Wave();
};

struct Channel
{
        Wave *waveref;
        float volume;

        Uint8 *audio_pos;
        int audio_len;

        bool done = false;

        Channel(Wave* sample, float volume);
        void Mix(Uint8* stream, int len);
};



class Audio
{
public:
        Audio();
        virtual ~Audio();

        void PlaySound(Wave *sample, float volume = 1.0f);
        void PlaySound(Wave &sample, float volume = 1.0f) { PlaySound(&sample, volume); }
        static void audio_callback(void *unused, Uint8 *stream, int len);
};

