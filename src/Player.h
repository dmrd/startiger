#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Globals.h"
#include "R3Node.h"
#include "GameObject.h"

class Player : public GameObject
{
    protected:
        R3Node *node;
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

            globals.scene->root->AddChild(new R3Node(new R3Sphere(R3null_point, 2),
                        mat, starttransform));
        }

        void Update(double dt)
        {
        }

        void Destroy()
        {
        }
};

#endif
