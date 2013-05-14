#include "App.h"
#include "glutfix.h"
#include "Globals.h"
#include "Util.h"
#include "R3/R3.h"
#include "R3Scene.h"
#include "Player.h"
#include "particle.h"
#include "Flock.h"
#include "Boid.h"

#ifndef __APPLE__
    #include "Sound.h"
#endif

#include "BasicLevel.h"
#include <string>
#include <sstream>

#define START_WIN_WIDTH    800
#define START_WIN_HEIGHT   600

R3Material *hud_mat;
double fps;
int changeFps = 0;

// --- main -----------------------------------------------------------------

void App::Init(int *argc, char **argv)
{
    // read command-line arguments
    //if (!App::ParseArgs(*argc, argv))
        //exit(1);

    /* Sound initialization */
#ifndef __APPLE__
    InitSound();
#endif

    // initialize GLUT
    glutInit(argc, argv);

    // window
    glutInitWindowPosition(100, 100);
    globals.window.width = START_WIN_WIDTH;
    globals.window.height = START_WIN_HEIGHT;
    glutInitWindowSize(globals.window.width, globals.window.height);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    globals.window.glutid = glutCreateWindow("StarTiger");

    // load scenclass Sound
    printf("Input scene: %s\n", globals.input_scene_name.c_str());
    globals.scene = new R3Scene();
    globals.scene->Read("loltest");

    // particles are chill
    globals.scene->gravity = R3null_vector;

    // default lights
    R3Light *light = new R3Light();
    R3Vector direction(-3,-4,-5);
    direction.Normalize();
    light->type = R3_DIRECTIONAL_LIGHT;
    light->color = R3Rgb(0.6,0.6,0.6,1);
    light->position = R3Point(0, 0, 0);
    light->direction = direction;
    light->radius = 0;
    light->constant_attenuation = 0;
    light->linear_attenuation = 0;
    light->quadratic_attenuation = 0;
    light->angle_attenuation = 0;
    light->angle_cutoff = M_PI;
    globals.scene->AddLight(light);

    light = new R3Light();
    direction = R3Vector(3,2,3);
    direction.Normalize();
    light->type = R3_DIRECTIONAL_LIGHT;
    light->color = R3Rgb(0.3, 0.3, 0.3, 1);
    light->position = R3Point(0, 0, 0);
    light->direction = direction;
    light->radius = 0;
    light->constant_attenuation = 0;
    light->linear_attenuation = 0;
    light->quadratic_attenuation = 0;
    light->angle_attenuation = 0;
    light->angle_cutoff = M_PI;
    globals.scene->AddLight(light);

    globals.scene->ambient = R3Rgb(0.2, 0.2, 0.2, 1);

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

    // initialize shaders
#if USE_SHADERS
    GLenum err = glewInit();
    if (err != GLEW_OK)
        exit(1); // or handle the error in a nicer way
    if (!GLEW_VERSION_2_1)  // check that the machine supports the 2.1 API.
        exit(1); // or handle the error in a nicer way
#endif

    // GameObjectManager
    globals.gomgr = new GameObjectManager();

    // GameStateManager
    globals.gsmgr = new GameStateManager();

    // CameraHandler
    globals.gomgr->Add(new CameraHandler(&globals.scene->camera));

    // Levels
    //globals.levelStatus = 1;
    globals.gsmgr->Add(new BasicLevel());
    //globals.gsmgr->Stop();

    // Sounds
#ifndef __APPLE__
    globals.sounds.explosion = new Sound("explosion.wav");
    globals.sounds.shot[0] = new Sound("shot1.wav");
    globals.sounds.shot[1] = new Sound("shot2.wav");
    globals.sounds.shot[2] = new Sound("shot3.wav");
    globals.sounds.shot[3] = new Sound("shot4.wav");
#endif

    // Run game!
    globals.gsmgr->Start();

    // HUD
    R3Material::Params hudParams; 
    hudParams.lit = false;
    hudParams.textureName = "ship.jpg";
    hud_mat = new R3Material(hudParams);//Util::GetTransparentTexture("ship.jpg", "ship_transparent.jpg");
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

    // cleanup
    delete globals.scene;
    delete globals.gomgr;



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

    if (globals.levelStatus == 0) {
        // draw scene
        globals.scene->Draw();

        RenderParticles(globals.scene, 0, 0);

    }
    // draw HUD
    HUD();

    glutSwapBuffers();
}

void App::HUD()
{

    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, globals.window.width, globals.window.height, 0.0, -1.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    glUseProgram(0);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);
    if (globals.levelStatus == 0) {
        /*glEnable(GL_BLEND);
          glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

          glEnable(GL_TEXTURE_2D);
          */
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        //glBindTexture(GL_TEXTURE_2D, hud_img);

        hud_mat->Load();

        glBegin(GL_QUADS);

        glTexCoord2d(1.0, 0.0); 
        glVertex2d(globals.window.width - 150.0, 15.0);

        glTexCoord2d(0.0, 0.0);
        glVertex2d(globals.window.width - 150.0, 65.0);

        glTexCoord2d(0.0, 1.0);
        glVertex2d(globals.window.width - 100.0, 65.0);

        glTexCoord2d(1.0, 1.0);
        glVertex2d(globals.window.width - 100.0, 15.0);

        glEnd();
        glDisable(GL_TEXTURE_2D);
        
        // Health Bar
        // White outline
        glBegin(GL_QUADS);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glVertex2f(47.0, 27.0);
        glVertex2f(47.0, 53.0);
        glVertex2f(253.0, 53.0);
        glVertex2f(253.0, 27.0);
        glEnd();


        // Black Background
        glBegin(GL_QUADS);
        glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
        glVertex2f(50.0, 30.0);
        glVertex2f(50.0, 50.0);
        glVertex2f(250.0, 50.0);
        glVertex2f(250.0, 30.0);
        glEnd();

        // Red Health Bar
        glBegin(GL_QUADS);
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        glVertex2f(50.0, 30.0);
        glVertex2f(50.0, 50.0);
        glVertex2f(250.0*globals.player->GetHealth(), 50.0);
        glVertex2f(250.0*globals.player->GetHealth(), 30.0);
        glEnd();




        // lives
        stringstream ss;
        ss << "x" << globals.player->lives;
        glColor3f(0.8f, 0.8f, 0.3f);
        glRasterPos2f(globals.window.width - 90.0, 45.0);

        for (int i = 0; i < ss.str().length(); i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ss.str().at(i));
        }

        // score
        stringstream ss2;
        ss2 << "Score: " << globals.player->score;
        glColor3f(0.8f, 0.8f, 0.3f);
        glRasterPos2f(globals.window.width/2, 45.0);

        for (int i = 0; i < ss2.str().length(); i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ss2.str().at(i));
        }

        // score
        stringstream ss3;

        ss3 << "FPS: " << fps;
        glColor3f(0.8f, 0.8f, 0.3f);
        glRasterPos2f(globals.window.width/2, 90.0);

        for (int i = 0; i < ss3.str().length(); i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ss3.str().at(i));
        }

    } else if (globals.levelStatus == 1) {
        stringstream ss3;

        ss3 << "YOU WIN! PRESS Q TO QUIT";
        glColor3f(0.8f, 0.8f, 0.3f);
        glRasterPos2f(globals.window.width/2-150, 200.0);

        for (int i = 0; i < ss3.str().length(); i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ss3.str().at(i));
        }


    } else if (globals.levelStatus == 2) {
        stringstream ss3;

        ss3 << "YOU LOOSE! PRESS Q TO QUIT";
        glColor3f(0.8f, 0.8f, 0.3f);
        glRasterPos2f(globals.window.width/2-150, 200.0);

        for (int i = 0; i < ss3.str().length(); i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ss3.str().at(i));
        }

    }


    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

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

    if (changeFps > 200) {
        fps = 1/delta_time;
        changeFps = 0;
    } else {
        changeFps++;
    }

    // update objects
    globals.gomgr->Update(delta_time);
    globals.gsmgr->Update(delta_time);

    // update particles
    UpdateParticles(globals.scene, current_time, delta_time, 0);
    GenerateParticles(globals.scene, current_time, delta_time);

    // collide
    globals.scene->Collide();

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

