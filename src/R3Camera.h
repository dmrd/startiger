#ifndef __R3CAMERA_H__
#define __R3CAMERA_H__

#include "R3/R3.h"

/*
 * a viewpoint for scene rendering
 */

struct R3Camera
{
    R3Point eye;
    R3Vector towards;
    R3Vector right;
    R3Vector up;
    double xfov, yfov;
    double neardist, fardist;

    void Load(void) const
    {
        // Set projection transformation
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(2*180.0*yfov/M_PI,
                (GLdouble) globals.window.width/(GLdouble) globals.window.height,
                0.01, 10000);

        // Set camera transformation
        R3Vector t = -(towards);
        const R3Vector& u = up;
        const R3Vector& r = right;
        GLdouble camera_matrix[16] = { r[0], u[0], t[0], 0, r[1], u[1], t[1], 0, r[2], u[2], t[2], 0, 0, 0, 0, 1 };
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMultMatrixd(camera_matrix);
        glTranslated(-(eye[0]), -(eye[1]), -(eye[2]));
    }
};

#endif
