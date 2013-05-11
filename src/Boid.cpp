#include "Globals.h"
#include "R3Node.h"
#include "GameObject.h"
#include "Util.h"
#include "Boid.h"
#include "Flock.h"
#include "Player.h"
#include "Shot.h"
#include "R3Scene.h"
#include <iostream>

Boid::Boid(R3Point spawn_, R3Vector velocity_, Flock *flock_) :
    spawn(spawn_),
    velocity(velocity_),
    flock(flock_),
    alive(true),
    firingRate(0.08)
{
}

Boid::~Boid()
{
}

void Boid::Create(void)
{
    // Initialize bullet to random number to prevent flock all firing at once
    bullets = Util::UnitRandom();

    // create material, node

    R3Material::Params matParams;
    matParams.kd = R3Rgb(Util::UnitRandom(), Util::UnitRandom(), Util::UnitRandom(), 1.0);
    mat = new R3Material(matParams);

    node = new R3Node(NULL, mat, R3Matrix(spawn));
    globals.scene->root->AddChild(node);

    child = new R3Node(this, new R3Mesh("arwing.off"), mat, R3identity_matrix);
    node->AddChild(child);
}

void Boid::Update(double dt)
{
    // face player -- will have to fix to yaw/pitch/roll later

    R3Vector dir = globals.player->GetPosition() - GetPosition();
    dir.Normalize();

    child->transformation = R3identity_matrix;
    child->transformation.Rotate(R3negz_vector, dir);
    child->transformation.Scale(0.5);
}

/* Manage accumulating bullet firing probabilities and act of firing*/
void Boid::ManageBullets(double dt) {
    bullets += dt * firingRate;
    if (bullets > 1) {
        bullets = Util::UnitRandom() - firingRate;
        Shot::Params shotparams;
        shotparams.transform = GetPosition();
        shotparams.direction = globals.player->GetPosition() - GetPosition();
        shotparams.playershot = false;
        globals.gomgr->Add(new Shot(shotparams));
    }
}

void Boid::Destroy()
{
    node->Destroy();
    delete mat;
}


R3Point Boid::GetPosition()
{
    return node->getWorldTransform().getOrigin();
}
