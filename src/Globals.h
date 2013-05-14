#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <string>

#include "GameObject.h"
#include "GameState.h"
#ifndef __APPLE__
    #include "SDL/SDL.h"
    #include "SDL/SDL_mixer.h"
#endif

/*
 * forward declarations
 */
class R3Scene;
class Player;
class CameraHandler;
#ifndef __APPLE__
class Sound;
#endif



/*
 * data accessible from everywhere
 *
 * probably not the cleanest in design terms, but works
 */

struct Globals
{
    struct Window
    {
        int glutid;       // window id given by GLUT
        int width;
        int height;
    } window;

    struct Settings
    {
        bool show_bboxes; // whether to draw node bounding boxes
    } settings;

    bool quit;            // if set to true, exit soon
    bool keys[256];

    std::string input_scene_name;
    R3Scene *scene;       // current scene

    GameObjectManager *gomgr;
    GameStateManager *gsmgr;
    Player *player;
    CameraHandler *camerahandler;
#ifndef __APPLE__
    struct Sounds
    {
        Sound *explosion;
        Sound *shot[4];

        int rate;
        Uint16 format; 
        int channels;
        int mix_channels; // number of channels to mix sounds in
        int buffers;
    } sounds;
#endif

    Globals()
    {
        // defaults

        settings.show_bboxes = false;

        quit = false;
        input_scene_name = "";

#ifndef __APPLE__
        sounds.rate = 22050;
        sounds.format = MIX_DEFAULT_FORMAT; 
        sounds.channels = 2;
        sounds.mix_channels = 32;
        sounds.buffers = 4096;
#endif
    }
};

extern Globals globals;



#endif
