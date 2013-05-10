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
    firingRate(0.2)
{
}

Boid::~Boid()
{
    delete mat;
}

void Boid::Create(void)
{
    // Initialize bullet to random number to prevent flock all firing at once
    bullets = Util::UnitRandom();

    // create material, node

    mat = new R3Material();
    mat->ka = R3Rgb(0.2, 0.2, 0.2, 1);
    mat->kd = R3Rgb(Util::UnitRandom(), Util::UnitRandom(), Util::UnitRandom(), 1.0);
    mat->kd = mat->kd * 0.5 + R3Rgb(0.5, 0.5, 0.5, 1.0);
    mat->ks = R3Rgb(0.5, 0.5, 0.5, 1);
    mat->kt = R3Rgb(0.0, 0.0, 0.0, 1);
    mat->emission = R3Rgb(0, 0, 0, 1);
    mat->shininess = 10;
    mat->indexofrefraction = 1;
    mat->texture = NULL;
    mat->id = 0;

    node = new R3Node(new R3Sphere(R3null_point, 0.2 + 0.3 * Util::UnitRandom()),
            mat, R3Matrix(spawn));

    globals.scene->root->AddChild(node);
}

void Boid::Update(double dt)
{
}

/* Manage accumulating bullet firing probabilities and act of firing*/
void Boid::ManageBullets(double dt) {
    bullets += dt * firingRate;
    if (bullets > 1) {
        bullets = Util::UnitRandom() - firingRate;
        Shot::Params shotparams;
        shotparams.transform = GetPosition();
        shotparams.direction = globals.player->Player::GetPosition() - GetPosition();
        globals.gomgr->Add(new Shot(shotparams));
    }
}

/* TODO: write delete function */
void Boid::Destroy()
{
    //delete node;
}


R3Point Boid::GetPosition()
{
    return node->getWorldTransform().getOrigin();
}
