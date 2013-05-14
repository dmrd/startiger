#include "Sound.h"
#include <string>
#include "Dirs.h"


bool InitSound() {
    printf("Sound initializing\n");
    SDL_Init(SDL_INIT_AUDIO);
    if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)) {
        printf("Unable to open audio!\n");
        exit(1);
    }
    printf("Sound initialized\n");
    return true;
}

bool closeSound() {
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
