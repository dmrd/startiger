#include "Globals.h"
#include "R3Node.h"
#include "GameObject.h"
#include "Util.h"
#include "Explosion.h"
#include "R3Scene.h"
#include <iostream>

bool Explosion::initialized = false;
R3Material **Explosion::mat = NULL;

Explosion::Explosion(R3Matrix transform)
{
    this->transform = transform;
}

Explosion::~Explosion()
{
}


void Explosion::Initialize() {
    Explosion::initialized = true;
    Explosion::mat = new R3Material*[4];

    R3Material::Params fireParams; 
    fireParams.kd = R2Pixel(.5, .5, .25, 0);
    fireParams.textureName = "smoke.jpg";
    mat[0] = new R3Material(fireParams);

    fireParams.kd = R2Pixel(.5, .5, 0, 0);
    mat[1] = new R3Material(fireParams);

    fireParams.kd = R2Pixel(.5, .25, 0, 0);
    mat[2] = new R3Material(fireParams);

    fireParams.kd = R2Pixel(.5, 0, 0, 0);
    mat[3] = new R3Material(fireParams);
}

void Explosion::Create(void)
{
    if (!Explosion::initialized) {
        Initialize();
    }
    lifetime = .25;

    node = new R3Node(transform);
    globals.scene->root->AddChild(node);

    R3ParticleSource *source = new R3ParticleSource();
    source->shape = new R3Sphere(R3Point(0, 0, 0), .05);
    source->rate = 300;
    source->velocity = 20;
    source->angle_cutoff = .5;
    source->mass = 1;
    source->fixed = false;
    source->drag = 0;
    source->elasticity = 0;
    source->lifetime = .25;
    source->size = .5;

    source->numMaterials = 4;
    source->materials = Explosion::mat;

    node->AttachSource(source);

}

void Explosion::Update(double dt)
{
    lifetime -= dt;
    if (lifetime < 0) {
        globals.gomgr->Destroy(this->GetID());
    }
}

/* Manage accumulating bullet firing probabilities and act of firing*/
void Explosion::Destroy()
{
    node->Destroy();
}


R3Point Explosion::GetPosition()
{
    return node->getWorldTransform().getOrigin();
}
