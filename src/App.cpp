#include "App.h"

#include "Globals.h"
#include "glutfix.h"
#include "R3/R3.h"
#include "R3Scene.h"
#include "Draw.h"

#define START_WIN_WIDTH    800
#define START_WIN_HEIGHT   600


////////////////////////////////////////////////////////////
// TIMER CODE
////////////////////////////////////////////////////////////




// --- main -----------------------------------------------------------------

void App::Init(int *argc, char **argv)
{
    // Read in scene from command line - Works until we have a main menu
    if (!App::ParseArgs(*argc, argv))
        exit(1);

    // Read in scene
    printf("Input scene: %s\n", globals.input_scene_name.c_str());
    globals.scene = new R3Scene();
    globals.scene->Read(globals.input_scene_name);


    // initialize GLUT
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
    glutKeyboardUpFunc(App::KeyReleased);
    glutSpecialUpFunc(App::KeyReleasedSpecial);

    // OpenGL modes
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
}

int App::ParseArgs(int argc, char **argv)
{
    int print_usage = 0;

    // Parse arguments
    argc--; argv++;
    while (argc > 0)
    {
        if ((*argv)[0] == '-')
        {
            if (!strcmp(*argv, "-help"))
                print_usage = 1;
            else
            {
                fprintf(stderr, "Invalid program argument: %s", *argv);
                exit(1);
            }
            argv++; argc--;
        }
        else
        {
            if (globals.input_scene_name == "")
                globals.input_scene_name = *argv;
            else
            {
                fprintf(stderr, "Invalid program argument: %s", *argv);
                exit(1);
            }
            argv++; argc--;
        }
    }

    // Check input_scene_name
    if (globals.input_scene_name == "" || print_usage)
    {
        printf("Usage: startiger <input.scn> [glut options]\n");
        return 0;
    }

    // Return OK status 
    return 1;
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
    {
        glutSetWindow(globals.window.glutid);  
    }
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

    // Initialize OpenGL drawing modes
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);
    glDepthMask(true);

    // Clear window 
    R3Rgb background = globals.scene->background;
    glClearColor(background[0], background[1], background[2], background[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Load camera
    LoadCamera(&(globals.scene->camera));

    // Load scene lights
    LoadLights(globals.scene);



    // Draw particles
    DrawParticles(globals.scene);

    glEnable(GL_LIGHTING);
    // Draw scene surfaces
    DrawScene(globals.scene);


    // Swap buffers 
    glutSwapBuffers();
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
    globals.keys[key] = true;
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

void App::KeyReleased(unsigned char key, int x, int y)
{
    globals.keys[key] = false;
}

void App::KeyReleasedSpecial(int key, int x, int y)
{
}


