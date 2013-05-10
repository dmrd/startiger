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
    public:
        struct Params { R3Point spawn; int swarmSize; double radius; };
        Flock(const Params &params_);

        ~Flock();

        void Create(void);
        void Update(double dt);
        void Destroy();

    protected:
        Params params;

        vector<Boid *> boids;
        double neighborhood;
        double repulsionArea;
        double vlim;
        vector<R3Point> targets;
        void UpdateBoidVelocity(int current);
};

#endif
