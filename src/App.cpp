#include "App.h"

#include "Globals.h"
#include "glutfix.h"

#define START_WIN_WIDTH    800
#define START_WIN_HEIGHT   600

// --- main -----------------------------------------------------------------

void App::Init(int *argc, char **argv)
{
    glutInit(argc, argv);

    // window
    glutInitWindowPosition(100, 100);
    globals.window.width = START_WIN_WIDTH;
    globals.window.height = START_WIN_HEIGHT;
    glutInitWindowSize(globals.window.width, globals.window.height);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    globals.window.glutid = glutCreateWindow("StarTiger");

    // callbacks
    glutIdleFunc(App::Idle);
    glutReshapeFunc(App::WindowResized);
    glutDisplayFunc(App::Draw);
    glutMouseFunc(App::MouseClicked);
    glutMotionFunc(App::MouseMoved);
    glutKeyboardFunc(App::KeyPressed);
    glutSpecialFunc(App::KeyPressedSpecial);
}

void App::Loop(void)
{
    // never returns, runs callbacks
    glutMainLoop();
}

void App::Quit(void)
{
    glutDestroyWindow(globals.window.glutid);
    exit(0);
}


// --- window ---------------------------------------------------------------

void App::Idle(void)
{
    if (glutGetWindow() != globals.window.glutid) 
        glutSetWindow(globals.window.glutid);  
    glutPostRedisplay();
}

void App::WindowResized(int w, int h)
{
    glViewport(0, 0, w, h);
    globals.window.width = w;
    globals.window.height = h;
    glutPostRedisplay();
}



// --- draw -----------------------------------------------------------------

void App::Draw(void)
{
    if (globals.quit)
        App::Quit();
}


// --- mouse ----------------------------------------------------------------

void App::MouseMoved(int x, int y)
{
}

void App::MouseClicked(int button, int state, int x, int y)
{
}



// --- keyboard -------------------------------------------------------------

void App::KeyPressed(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'Q':
        case 'q':
        case 27:
            globals.quit = true;
            break;
    }
}

void App::KeyPressedSpecial(int key, int x, int y)
{
}



