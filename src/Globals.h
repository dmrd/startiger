#ifndef __GLOBALS_H__
#define __GLOBALS_H__
#include "R3Scene.h"

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

    char *input_scene_name = NULL;
    R3Scene *scene;       // The currently displayed scene file

    Globals() :
        quit(false)
    {
    }
};

extern Globals globals;

#endif
