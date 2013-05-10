#include "Globals.h"
#include "R3Node.h"
#include "GameObject.h"
#include "Shot.h"
#include "R3Scene.h"
#include <cmath>


#define BULLET_SPEED 40
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
    mat = new R3Material();
    mat->ka = R3Rgb(0.2, 0.2, 0.2, 1);
    mat->kd = R3Rgb(0.8, 0.2, 0.1, 1);
    mat->ks = R3Rgb(0.8, 0.2, 0.1, 1);
    mat->kt = R3Rgb(0.0, 0.0, 0.0, 1);
    mat->emission = R3Rgb(0, 0, 0, 1);
    mat->shininess = 10;
    mat->indexofrefraction = 1;
    mat->texture = NULL;
    mat->id = 0;

    // create node
    node = new R3Node(new R3Box(-0.05, -0.05, -0.5, 0.05, 0.05, 0.5), mat);
    node->transformation = params.transform;
    node->transformation.Rotate(R3negz_vector, params.direction);
    globals.scene->root->AddChild(node);

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
    node->transformation.Translate(R3Vector(0, 0, -BULLET_SPEED * dt));
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

