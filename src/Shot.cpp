#include "Globals.h"
#include "R3Node.h"
#include "GameObject.h"
#include "Shot.h"
#include "R3Scene.h"
#include <cmath>
#ifndef __NOSOUND__
#include "Sound.h"
#endif


#define BULLET_SPEED 80
#define LIFETIME 1

Shot::Shot(const Params &params_) :
    params(params_)
{
    params.direction.Normalize();
}

Shot::~Shot()
{
}

void Shot::Create(void)
{
    // need unit-length direction
    params.direction.Normalize();

    // create material
    R3Material::Params matParams;
    matParams.kd = params.playershot ? R3Rgb(0.9, 0.8, 0.0, 1) : R3Rgb(0.8, 0.1, 0.1, 1);
    matParams.emission = params.playershot ? R3Rgb(0.9, 0.8, 0.0, 1) : R3Rgb(0.8, 0.1, 0.1, 1);
    mat = new R3Material(matParams);

    // create node
    node = new R3Node(this, new R3Box(-0.05, -0.05, -0.5, 0.05, 0.05, 0.5), mat);
    node->transformation = params.transform;
    node->transformation.Rotate(R3negz_vector, params.direction);
    globals.scene->root->AddChild(node);

    // apply transformation to basevel
    params.basevel.Transform(node->transformation.Inverse());
    params.basevel *= 0.2;

    // set shot type
    type = params.playershot ? OBJ_PLAYERSHOT : OBJ_ENEMYSHOT;

    // initialize some stuff
    timeleft = LIFETIME;
}

void Shot::Update(double dt)
{
    // die?
    timeleft -= dt;
    if (globals.keys['g'] || timeleft <= 0)
        globals.gomgr->Destroy(GetID());
    
    // move
    node->transformation.Translate((params.basevel + R3Vector(0, 0, -BULLET_SPEED)) * dt);
}

void Shot::Destroy()
{
    node->Destroy();
    delete mat;
}

R3Point Shot::GetPosition()
{
    return node->getWorldTransform().getOrigin();
}

