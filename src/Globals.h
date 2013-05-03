#ifndef __GLOBALS_H__
#define __GLOBALS_H__

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



    Globals() :
        quit(false)
    {
    }
};

extern Globals globals;

#endif
