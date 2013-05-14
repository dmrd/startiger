#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <string>

#include "glutfix.h"
#include "GameObject.h"
#include "GameState.h"
#ifndef __NOSOUND__
    #include "SDL/SDL.h"
    #include "SDL/SDL_mixer.h"
#endif

/*
 * forward declarations
 */
class R3Scene;
class Player;
class CameraHandler;
#ifndef __NOSOUND__
class Sound;
#endif

class Terrain;
class Rails;

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

    Terrain* terrain;
    Rails* rails;

    int levelStatus;

#ifndef __APPLE__
#ifndef __NOSOUND__
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

#ifndef __NOSOUND__
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
