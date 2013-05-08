#ifdef _WIN32
#  include <windows.h>
#else
#  include <sys/time.h>
#endif

#include "App.h"

#include "Globals.h"
#include "glutfix.h"
#include "R3/R3.h"
#include "R3Scene.h"
#include "Player.h"

#define START_WIN_WIDTH    800
#define START_WIN_HEIGHT   600



// --- GetTime() ------------------------------------------------------------

static double GetTime(void)
{
#ifdef _WIN32
    // Return number of seconds since start of execution
    static int first = 1;
    static LARGE_INTEGER timefreq;
    static LARGE_INTEGER start_timevalue;

    // Check if this is the first time
    if (first)
    {
        // Initialize first time
        QueryPerformanceFrequency(&timefreq);
        QueryPerformanceCounter(&start_timevalue);
        first = 0;
        return 0;
    }
    else
    {
        // Return time since start
        LARGE_INTEGER current_timevalue;
        QueryPerformanceCounter(&current_timevalue);
        return ((double) current_timevalue.QuadPart - 
                (double) start_timevalue.QuadPart) / 
            (double) timefreq.QuadPart;
    }
#else
    // Return number of seconds since start of execution
    static int first = 1;
    static struct timeval start_timevalue;

    // Check if this is the first time
    if (first)
    {
        // Initialize first time
        gettimeofday(&start_timevalue, NULL);
        first = 0;
        return 0;
    }
    else
    {
        // Return time since start
        struct timeval current_timevalue;
        gettimeofday(&current_timevalue, NULL);
        int secs = current_timevalue.tv_sec - start_timevalue.tv_sec;
        int usecs = current_timevalue.tv_usec - start_timevalue.tv_usec;
        return (double) (secs + 1.0E-6F * usecs);
    }
#endif
}



// --- main -----------------------------------------------------------------

void App::Init(int *argc, char **argv)
{
    // read command-line arguments
    if (!App::ParseArgs(*argc, argv))
        exit(1);

    // load scene
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

    // test Player
    globals.gomgr = new GameObjectManager();
    globals.gomgr->Add(new Player(R3identity_matrix));
}

int App::ParseArgs(int argc, char **argv)
{
    int print_usage = 0;

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
        glutSetWindow(globals.window.glutid);  
    Update();
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
    // initialize OpenGL drawing modes
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);
    glDepthMask(true);

    // clear window 
    R3Rgb background = globals.scene->background;
    glClearColor(background[0], background[1], background[2], background[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw scene
    globals.scene->Draw();

    // swap buffers 
    glutSwapBuffers();
}

void App::Update()
{    
    double current_time = GetTime();
    static double previous_time = 0;

    // program just started up?
    if (previous_time == 0) previous_time = current_time;

    // time passed since starting
    double delta_time = current_time - previous_time;
    previous_time = current_time;

    // update objects
    globals.gomgr->Update(delta_time);

    // quit?
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



