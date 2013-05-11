#ifndef __BOID_H__
#define __BOID_H__

#include "Globals.h"
#include "R3Node.h"
#include "GameObject.h"

class Flock;

class Boid : public GameObject
{
    protected:
        R3Point spawn;
        R3Material *mat;
        Flock *flock;

    public:
        GameObjectType GetType(void) { return OBJ_BOID; }

        Boid(R3Point spawn_, R3Vector velocity_, Flock *flock_);
        ~Boid();
        void Create(void);
        void Update(double dt);
        void Destroy();
        void ManageBullets(double dt);
        R3Point GetPosition();

        R3Node *node;
        R3Vector velocity;
        R3Point position; // Used by Flock to calculate relative locations
        int targetID; // Current target index in flock targets
        double firingRate; // Max amount added to bullets per second
        double bullets; // Accumulate sum of random numbers.  Fire when greater than 1
        bool alive;
};

#endif
