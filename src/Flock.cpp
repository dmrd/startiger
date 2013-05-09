#include "Globals.h"
#include "Util.h"
#include "R3Node.h"
#include "GameObject.h"
#include "Flock.h"
#include "Boid.h"
#include <iostream>
using namespace std;

/* Simple test "impulse" in a certain direction */
R3Vector leader = R3Vector(1,0,0);

Flock::Flock(R3Point spawn_, int swarmSize_, double radius_) :
    spawn(spawn_),
    swarmSize(swarmSize_),
    radius(radius_),
    neighborhood(1.5),
    repulsionArea(0.4),
    vlim(1)
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
        Boid *newBoid = new Boid(location, R3Vector(1,0,0), this);
        globals.gomgr->Add(newBoid);
        boids.push_back(newBoid);
        newBoid->velocity = R3Vector(1.0,0,0);
    }
}
/*
 * Update velocity of a single boid based on its neighborhood
 * Basic rules based on http://www.kfish.org/boids/pseudocode.html
 */
void Flock::UpdateBoidVelocity(int current) {
    /* Calculate center of mass & average velocity*/
    R3Point center = R3null_point;
    R3Vector velocity = R3null_vector;
    R3Vector repulsion = R3null_vector;
    int numNeighbors = 0;
    for (int id = 0; id < boids.size(); id++) {
        if (id == current) { continue; }
        //Only include nearby boids
        R3Vector vec = boids[current]->position - boids[id]->position;
        double dist = vec.Length();
        //Can only see ahead of self within a small area
        if (dist > neighborhood || boids[current]->velocity.Dot(vec) < 0) { continue; }
        numNeighbors++;
        center += boids[id]->position;
        velocity += boids[id]->velocity;
        if (dist < repulsionArea) {
            repulsion += (boids[current]->position - boids[id]->position);
        }
    }
    if (numNeighbors > 0) {
        center /= swarmSize - 1;
        velocity /= swarmSize - 1;
        R3Vector centerV = (center - boids[current]->position) / 8;
        velocity /= 100;
        boids[current]->velocity += velocity + centerV + repulsion;
    }
    boids[current]->velocity += leader;
    if (boids[current]->velocity.Length() >  vlim) {
        boids[current]->velocity.Normalize();
        boids[current]->velocity *= vlim;
    }
}

/*Update position of all flock members*/
void Flock::Update(double dt)
{

    /* Clear dead boids */
    int oSize = swarmSize;
    swarmSize = 0;
    for (int i = 0; i < boids.size(); i++) {
        if (boids[i]->alive) {
            // Cache R3Point location to avoid recalculating later
            boids[i]->position = boids[i]->node->transformation.getOrigin();
            boids[swarmSize] = boids[i];
            swarmSize++;
        } else {
            boids[i]->Destroy();
        }
    }
    if (oSize != swarmSize) {
        boids.resize(swarmSize);
    }
    /* Update velocities */
    for (int i = 0; i < boids.size(); i++) {
        UpdateBoidVelocity(i);
    }

    /* Update positions */
    for (int i = 0; i < boids.size(); i++) {
        boids[i]->node->transformation.Translate((boids[i]->velocity) * dt);
    }

}

void Flock::Destroy()
{
}
