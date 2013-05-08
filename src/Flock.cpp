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
    for (int boid = 0; boid < swarmSize; boid++) {
        double x, y, z;
        do {
            x = 2 * (UnitRandom() - 0.5);
            y = 2 * (UnitRandom() - 0.5);
            z = 2 * (UnitRandom() - 0.5);
        } while(x*x + y*y + z*z > 1.0);
        R3Point location = R3Point(0,0,0) + radius * (x * R3posx_vector
                                            + y * R3posy_vector
                                            + z * R3posz_vector);
        globals.gomgr->Add(new Boid(location, R3Vector(1,0,0), 1.0));
    }
}

void Flock::Update(double dt)
{
}

void Flock::Destroy()
{
}
