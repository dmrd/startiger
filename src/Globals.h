#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <string>


/*
 * forward declarations
 */
class R3Scene;



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

    bool quit;            // if set to true, exit soon
    bool keys[256];

    std::string input_scene_name;
    R3Scene *scene;       // The currently displayed scene file

    Globals() :
        quit(false),
        input_scene_name("")
    {
    }
};

extern Globals globals;



#endif
