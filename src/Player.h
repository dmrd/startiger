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

            node = new R3Node(new R3Sphere(R3null_point, 0.5),
                        mat, starttransform);

            globals.scene->root->AddChild(node);
        }

        void Update(double dt)
        {
            // move
            node->transformation.Translate(R3Vector(
                        5 * (globals.keys['d'] - globals.keys['a']),
                        5 * (globals.keys['w'] - globals.keys['s']),
                        0
                        ) * dt);
        }

        void Destroy()
        {
        }
};

#endif
