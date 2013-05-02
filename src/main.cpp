/**************************************
 * StarTiger main
 * COS426 - Princeton University
 * Team:
 * Mark Benjamin
 * David Bieber
 * David Dohan
 * Nikhilesh Sigatapu
 ***************************************/

// Import GLUT
#if defined(_WIN32) || defined(__CYGWIN__)
# ifndef USE_OPENGL32
#  define USE_OPENGL32
# endif
# ifndef NOMINMAX
#  define NOMINMAX
# endif
# include <windows.h>
# include <GL/glut.h>
#elif defined(__APPLE__)
# include <GLUT/glut.h>
#else 
# include <GL/glut.h>
#endif
#include "R3/R3.h"
#include "globals.h"

// Display globals
static Camera camera;

// State globals
static int quit = 0;

// GLUT Variables
static int GLUTwindow = 0;
static int GLUTwindow_height = 800;
static int GLUTwindow_width = 800;

/* GLUT callback functions.  Logic placed in these */

void GLUTIdle(void)
{
    // Set current window
    if ( glutGetWindow() != GLUTwindow ) 
        glutSetWindow(GLUTwindow);  

    // Redraw
    glutPostRedisplay();
}

void GLUTStop(void) {
    glutDestroyWindow(GLUTwindow);
    exit(0);
}

void GLUTResize(int w, int h)
{
    // Resize window
    glViewport(0, 0, w, h);

    // Resize camera vertical field of view to match aspect ratio of viewport
    camera.yfov = atan(tan(camera.xfov) * (double) h/ (double) w); 

    // Remember window size 
    GLUTwindow_width = w;
    GLUTwindow_height = h;

    // Redraw
    glutPostRedisplay();
}

//
void GLUTRedraw(void) {


    if (quit) { GLUTStop(); }
}

void GLUTMotion(int x, int y)
{

}

void GLUTMouse(int button, int state, int x, int y)
{

}

void GLUTSpecial(int key, int x, int y)
{
}

void GLUTKeyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 'Q':
        case 'q':
        case 27:
            quit = 1;
            break;
    }
}

void GLUTInit(int *argc, char **argv) {
    // Open window 
    glutInit(argc, argv);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(GLUTwindow_width, GLUTwindow_height);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // | GLUT_STENCIL
    GLUTwindow = glutCreateWindow("StarTiger");


    // Register callback functions
    glutIdleFunc(GLUTIdle);
    glutReshapeFunc(GLUTResize);
    glutDisplayFunc(GLUTRedraw);
    glutKeyboardFunc(GLUTKeyboard);
    glutSpecialFunc(GLUTSpecial);
    glutMouseFunc(GLUTMouse);
    glutMotionFunc(GLUTMotion);

}

// Game event loop
void GLUTMainLoop(void) {
    // Run main loop - never returns - runs registered callbacks
    glutMainLoop();
}

int main(int argc, char **argv) {
    // Initialize GLUT
    GLUTInit(&argc, argv);

    // Read in level

    // Enter game logic loop
    GLUTMainLoop();
}
