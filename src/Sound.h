#ifndef __SOUND_H__
#define __SOUND_H__
#include <stdio.h>
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#include "stdint.h"
#include <string>
#include "Globals.h"

static Mix_Music *music = NULL;

void mixaudio(void *unused, Uint8 *stream, int len);

bool InitSound();
bool closeSound();

static int audio_rate = 22050;
static Uint16 audio_format = AUDIO_S16; 
static int audio_channels = 2;
static int audio_buffers = 4096;

class Sound
{
    protected:
        Mix_Chunk *sample;
        bool loaded;


    public:
        Sound();
        Sound(std::string filename);
        ~Sound();

        void Play();
};
#endif
