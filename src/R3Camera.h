#ifndef __R3CAMERA_H__
#define __R3CAMERA_H__

#include "R3/R3.h"
#include "Globals.h"

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

    R3Plane frustumplanes[4];

    void UpdateFrustumPlanes(void)
    {
        const double radius = 10;

        R3Point org = eye + towards * radius;
        R3Vector dx = right * radius * tan(xfov);
        R3Vector dy = up * radius * tan(yfov);

        R3Point ur = org + dx + dy;
        R3Point lr = org + dx - dy;
        R3Point ul = org - dx + dy;
        R3Point ll = org - dx - dy;

        frustumplanes[0] = R3Plane(eye, ur, lr);
        frustumplanes[1] = R3Plane(eye, ul, ur);
        frustumplanes[2] = R3Plane(eye, ll, ul);
        frustumplanes[3] = R3Plane(eye, lr, ll);
    }

    // whether completely outside frustum plane of given index
    bool OutsideFrustumPlane(int index, const R3Box &box)
    {
        // if any point is inside, box is inside
        if (R3SignedDistance(frustumplanes[index], box.Corner(0,0,0)) > 0) return false;
        if (R3SignedDistance(frustumplanes[index], box.Corner(0,0,1)) > 0) return false;
        if (R3SignedDistance(frustumplanes[index], box.Corner(0,1,0)) > 0) return false;
        if (R3SignedDistance(frustumplanes[index], box.Corner(0,1,1)) > 0) return false;
        if (R3SignedDistance(frustumplanes[index], box.Corner(1,0,0)) > 0) return false;
        if (R3SignedDistance(frustumplanes[index], box.Corner(1,0,1)) > 0) return false;
        if (R3SignedDistance(frustumplanes[index], box.Corner(1,1,0)) > 0) return false;
        if (R3SignedDistance(frustumplanes[index], box.Corner(1,1,1)) > 0) return false;
        return true;
    }

    bool InFrustum(const R3Box &box)
    {
        // box is outside if it is completely outside any one plane
        if (OutsideFrustumPlane(0, box)) return false;
        if (OutsideFrustumPlane(1, box)) return false;
        if (OutsideFrustumPlane(2, box)) return false;
        if (OutsideFrustumPlane(3, box)) return false;
        return true;
    }

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

    void LookAt(const R3Point &target, const R3Vector &globalup = R3posy_vector)
    {
        towards = target - eye;
        towards.Normalize();

        right = towards;
        right.Cross(globalup);
        right.Normalize();

        up = right;
        up.Cross(towards);
        up.Normalize();
    }
};

#endif
