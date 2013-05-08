#ifndef __CAMERAHANDLER_H__
#define __CAMERAHANDLER_H__

#include "Globals.h"
#include "R3Node.h"
#include "R3Camera.h"

class CameraHandler : public GameObject
{
    protected:
        R3Node *movetarget;
        R3Node *looktarget;

        R3Camera *camera;

    public:
        CameraHandler(R3Camera *camera_) :
            camera(camera_),
            movetarget(NULL),
            looktarget(NULL)
        {
            globals.camerahandler = this;
        }

        ~CameraHandler()
        {
            globals.camerahandler = NULL;
        }

        void Update(double dt)
        {
            // move
            if (movetarget)
            {
                const double FACTOR = 7;
                R3Vector delta = movetarget->getWorldTransform().getOrigin()
                    - camera->eye;
                camera->eye += delta * dt * FACTOR;
            }

            // look
            if (looktarget)
                camera->LookAt(looktarget->getWorldTransform().getOrigin());
        }

        void SetMoveTarget(R3Node *node)
        {
            movetarget = node;
        }

        void SetLookTarget(R3Node *node)
        {
            looktarget = node;
        }
};

#endif
