#include "App.h"

#include "Globals.h"
#include "Util.h"
#include "glutfix.h"
#include "R3/R3.h"
#include "R3Scene.h"
#include "Player.h"
#include "Flock.h"
#include "Boid.h"

#define START_WIN_WIDTH    800
#define START_WIN_HEIGHT   600


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

    // GameObjectManager
    globals.gomgr = new GameObjectManager();

    // test Player, CameraHandler

    globals.gomgr->Add(new CameraHandler(&globals.scene->camera));

    Player::Params playerparams;
    playerparams.transform = R3identity_matrix;
    globals.gomgr->Add(new Player(playerparams));

    Flock::Params flockparams;
    flockparams.spawn = R3Point(-5,0,-10);
    flockparams.swarmSize = 10;
    flockparams.radius = 1;
    globals.gomgr->Add(new Flock(flockparams));
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

    double wbh = ((double) w) / h;
    globals.scene->camera.xfov = wbh * globals.scene->camera.yfov;

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

    // draw HUD
    HUD();

    glutSwapBuffers();
}

void App::HUD() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, globals.window.width, globals.window.height, 0.0, -1.0, 10.0);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    //glPushMatrix();        ----Not sure if I need this
    glLoadIdentity();
    glDisable(GL_CULL_FACE);

    glClear(GL_DEPTH_BUFFER_BIT);

    glBegin(GL_QUADS);
    glColor3f(1.0f, 0.0f, 0.0);
    glVertex2f(0.0, 0.0);
    glVertex2f(100.0, 0.0);
    glVertex2f(100.0, 100.0);
    glVertex2f(0.0, 100.0);
    glEnd();

    // Making sure we can render 3d again
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    //glPopMatrix();        ----and this?

}

void App::Update()
{    
    double current_time = Util::GetTime();
    static double previous_time = 0;

    // program just started up?
    if (previous_time == 0) previous_time = current_time;

    // time passed since starting
    double delta_time = current_time - previous_time;
    previous_time = current_time;
    
    // fps
    printf("fps: %f\n", 1/delta_time);

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



