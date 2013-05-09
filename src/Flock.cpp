#include "Globals.h"
#include "Util.h"
#include "R3Node.h"
#include "GameObject.h"
#include "Flock.h"
#include "Boid.h"
#include <iostream>
using namespace std;

/* Simple test "impulse" in a certain direction */
R3Vector wind = R3Vector(0.5,0,0);
//R3Point target = R3Point(15,15,0);
//double targetStrength = 1.5;

Flock::Flock(const Params &params_) :
    params(params_),
    neighborhood(2),
    repulsionArea(1),
    vlim(2)
{
}

Flock::~Flock()
{
}

void Flock::Create(void)
{
    for (int boid = 0; boid < params.swarmSize; boid++)
    {
        R3Point location = params.spawn + Util::BallRandom(params.radius);
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
void Flock::UpdateBoidVelocity(int current)
{
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
        center /= numNeighbors;
        velocity /= numNeighbors;
        R3Vector centerV = (center - boids[current]->position) / 8;
        velocity /= 100;
        boids[current]->velocity += velocity + centerV + repulsion;
    }
    //R3Vector pull = (target - boids[current]->position);
    //pull.Normalize();
    //boids[current]->velocity += pull * targetStrength;
    boids[current]->velocity += wind;
    if (boids[current]->velocity.Length() >  vlim) {
        boids[current]->velocity.Normalize();
        boids[current]->velocity *= vlim;
    }
}

/*Update position of all flock members*/
void Flock::Update(double dt)
{

    /* Clear dead boids */
    int oSize = params.swarmSize;
    params.swarmSize = 0;
    for (int i = 0; i < boids.size(); i++) {
        if (boids[i]->alive) {
            // Cache R3Point location to avoid recalculating later
            boids[i]->position = boids[i]->node->getWorldTransform().getOrigin();
            boids[params.swarmSize] = boids[i];
            params.swarmSize++;
        } else {
            boids[i]->Destroy();
        }
    }
    if (oSize != params.swarmSize) {
        boids.resize(params.swarmSize);
    }
    /* Update velocities */
    for (int i = 0; i < boids.size(); i++) {
        UpdateBoidVelocity(i);
    }

    /* Update positions */
    for (int i = 0; i < boids.size(); i++) {
        boids[i]->node->transformation.Translate((boids[i]->velocity) * dt);
        //boids[i]-
    }

}

void Flock::Destroy()
{
}



