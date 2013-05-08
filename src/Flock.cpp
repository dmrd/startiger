#include "Globals.h"
#include "Util.h"
#include "R3Node.h"
#include "GameObject.h"
#include "Flock.h"
#include "Boid.h"


Flock::Flock(R3Point spawn_, int swarmSize_, double radius_) :
    spawn(spawn_),
    swarmSize(swarmSize_),
    radius(radius_)
{
}

Flock::~Flock()
{
}

void Flock::Create(void)
{
    for (int boid = 0; boid < swarmSize; boid++)
    {
        R3Point location = spawn + Util::BallRandom(radius);
        globals.gomgr->Add(new Boid(location, R3Vector(1,0,0), 1.0));
    }
}

void Flock::Update(double dt)
{
}

void Flock::Destroy()
{
}
