#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Globals.h"
#include "R3Node.h"
#include "GameObject.h"
#include "CameraHandler.h"
#include "R3Scene.h"

class Player : public GameObject
{
    protected:
        R3Node *node;
        R3Node *cameramovetarget;
        R3Matrix starttransform;
        R3Material *mat;

    public:
        Player(R3Matrix transform) :
            starttransform(transform)
        {
        }

        ~Player()
        {
            delete mat;
        }

        void Create(void)
        {
            // let everyone know
            globals.player = this;

            // create material, node

            mat = new R3Material();
            mat->ka = R3Rgb(0.2, 0.2, 0.2, 1);
            mat->kd = R3Rgb(0.5, 0.5, 0.5, 1);
            mat->ks = R3Rgb(0.5, 0.5, 0.5, 1);
            mat->kt = R3Rgb(0.0, 0.0, 0.0, 1);
            mat->emission = R3Rgb(0, 0, 0, 1);
            mat->shininess = 10;
            mat->indexofrefraction = 1;
            mat->texture = NULL;
            mat->id = 0;

            node = new R3Node(new R3Mesh("arwing.off"),
                        mat, starttransform);

            globals.scene->root->AddChild(node);

            // camera targets
            globals.camerahandler->SetLookTarget(node);
            cameramovetarget = new R3Node(NULL, NULL, R3Matrix(R3Point(0, 1.5, 12)));
            node->AddChild(cameramovetarget);
            globals.camerahandler->SetMoveTarget(cameramovetarget);
        }

        void Update(double dt)
        {
            // move
            node->transformation.Translate(R3Vector(
                        3 * (globals.keys['d'] - globals.keys['a']),
                        3 * (globals.keys['w'] - globals.keys['s']),
                        0
                        ) * dt);
        }

        void Destroy()
        {
            globals.player = NULL;
        }

        R3Point GetPosition()
        {
            R3Point pos(R3null_point);
            pos.Transform(node->transformation);
            return pos;
        }
};

#endif
