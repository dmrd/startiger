#ifndef __BOID_H__
#define __BOID_H__

#include "Globals.h"
#include "R3Node.h"
#include "GameObject.h"

class Flock;

class Boid : public GameObject
{
    protected:
        R3Node *node;
        R3Point spawn;
        R3Material *mat;
        R3Vector direction;
        double speed;
        Flock *flock;

    public:
        Boid(R3Point spawn_, R3Vector direction_, double speed_);
        ~Boid();
        void Create(void);
        void Update(double dt);
        void Destroy();
};

#endif
