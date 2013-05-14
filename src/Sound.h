#ifndef __APPLE__
#ifndef __SOUND_H__
#define __SOUND_H__
#include <stdio.h>
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#include "stdint.h"
#include <string>
#include "Globals.h"

bool InitSound();
bool CloseSound();

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
#endif
