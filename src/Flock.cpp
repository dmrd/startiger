#include "Globals.h"
#include "Util.h"
#include "R3Node.h"
#include "R3/R3.h"
#include "GameObject.h"
#include "Player.h"
#include "Flock.h"
#include "Boid.h"

/* Simple test "impulse" in a certain direction */
double targetDist = 3;
double targetIntensity = 8;
double targetConstant = 1;
double targetLinear = 0.5;
double targetQuad = 0.1;
vector<R3Point> relativeTargets;
//R3Point target = R3Point(15,15,0);
//double targetStrength = 1.5;

Flock::Flock(const Params &params_) :
    params(params_),
    neighborhood(4),
    repulsionArea(3),
    vlim(10)
{
}

Flock::~Flock()
{
}

void Flock::Create(void)
{
    relativeTargets.push_back(R3Point(0,0,-10));
    relativeTargets.push_back(R3Point(-5,-5,-15));
    relativeTargets.push_back(R3Point(-5,5,-15));
    relativeTargets.push_back(R3Point(5,5,-15));
    relativeTargets.push_back(R3Point(5,-5,-15));
    targets = vector<R3Point>(relativeTargets);
    for (int boid = 0; boid < params.swarmSize; boid++)
    {
        R3Point location = params.spawn + Util::BallRandom(params.radius);
        Boid *newBoid = new Boid(location, R3Vector(1,0,0), this);
        globals.gomgr->Add(newBoid);
        boids.push_back(newBoid);
        newBoid->velocity = R3Vector(1.0,0,0);
        newBoid->targetID = 0;
    }
}

/*
 * Update velocity of a single boid based on its neighborhood
 * Basic rules based on http://www.kfish.org/boids/pseudocode.html
 * Fly to center of boids
 * Avoid getting too close to other boids
 * Align velocity with nearby boids
 */
void Flock::UpdateBoidVelocity(int current)
{
    Boid *boid = boids[current];
    /* Update boid target if it is near current target*/
    if (R3Distance(boid->position, targets[boid->targetID]) < targetDist) {
        boid->targetID = (boid->targetID + 1) % targets.size();
    }

    /* Calculate center of mass & average velocity*/
    R3Point center = R3null_point;
    R3Vector velocity = R3null_vector;
    R3Vector repulsion = R3null_vector;
    int numNeighbors = 0;
    for (int id = 0; id < boids.size(); id++) {
        if (id == current) { continue; }
        //Only include nearby boids
        R3Vector vec = boid->position - boids[id]->position;
        double dist = vec.Length();
        //Can only see ahead of self within a small area
        if (dist > neighborhood || boid->velocity.Dot(vec) < 0) { continue; }
        numNeighbors++;

        center += boids[id]->position;
        velocity += boids[id]->velocity;
        if (dist < repulsionArea) {
            repulsion += (boid->position - boids[id]->position);
        }
    }

    if (numNeighbors > 0) {
        center /= numNeighbors;
        R3Vector centerV = (center - boid->position) / 100;
        velocity /= numNeighbors;
        velocity -= boid->velocity;
        velocity /= 100;
        boid->velocity += velocity + centerV + repulsion;
    }
    // Make relative to player for now
    R3Vector tVector = (targets[boid->targetID] - boid->position);
    double tDist = tVector.Length();
    // Falloff with distance
    boid->velocity += tVector / (targetConstant + targetLinear * tDist
                                                + targetQuad * tDist * tDist);
    if (boid->velocity.Length() >  vlim) {
        boid->velocity.Normalize();
        boid->velocity *= vlim;
    }
}

/*Update position of all flock members*/
void Flock::Update(double dt)
{
    // Update absolute targets relative to player
    R3Point playerPos = globals.player->GetPosition();
    for (int i = 0; i < targets.size(); i++) {
        targets[i] = relativeTargets[i] + playerPos;
    }
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
            //boids[i]->Destroy();
            globals.gomgr->Destroy(boids[i]->GetID());
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
        boids[i]->ManageBullets(dt);
    }

}

void Flock::Destroy()
{
}

unsigned int Flock::NBoids()
{
    return boids.size();
}



