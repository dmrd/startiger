#ifndef __FLOCK_H__
#define __FLOCK_H__

#include "Globals.h"
#include "R3Node.h"
#include "GameObject.h"
#include "R3/R3.h"
#include "Boid.h"
#include <vector>

class Flock : public GameObject
{
    protected:
        R3Point spawn;
        int swarmSize;
        vector<Boid *> boids;
        double radius; // Area to spawn in
        double neighborhood;
        double repulsionArea;
        double vlim;
        void UpdateBoidVelocity(int current);

    public:
        Flock(R3Point spawn_, int swarmSize_, double radius_);
        ~Flock();
        void Create(void);
        void Update(double dt);
        void Destroy();
};

#endif
