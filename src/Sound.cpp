#include "Sound.h"
#include <string>
#include "Dirs.h"


bool InitSound() {
    printf("Sound initializing\n");
    SDL_Init(SDL_INIT_AUDIO);
    if(Mix_OpenAudio(globals.sounds.rate, globals.sounds.format, globals.sounds.channels, globals.sounds.buffers)) {
        printf("Unable to open audio!\n");
        //exit(1);
    }
    Mix_AllocateChannels(globals.sounds.mix_channels); 
    printf("Sound initialized\n");

    Mix_Music *mus = Mix_LoadMUS((DIR_SOUNDS + std::string("DST-Assembly.wav")).c_str());
    Mix_PlayMusic(mus, -1);

    return true;
}

bool CloseSound() {
    Mix_CloseAudio();
    SDL_Quit();
    printf("Sound shutdown\n");
    return true;
}

Sound::Sound() {
}

Sound::Sound(std::string filename) {
    filename = DIR_SOUNDS + filename;
    printf("Loading %s\n", filename.c_str()); 
    loaded = false;
    sample = Mix_LoadWAV(filename.c_str());
    loaded = true;
    printf("Loaded %s\n", filename.c_str()); 
}

Sound::~Sound() {

}

void Sound::Play() {
    if (!loaded) { return; }
    Mix_PlayChannel(-1, sample, 0);
}
