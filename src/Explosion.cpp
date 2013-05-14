#include "Globals.h"
#include "R3Node.h"
#include "GameObject.h"
#include "Util.h"
#include "Explosion.h"
#include "R3Scene.h"
#include <iostream>
#ifndef __NOSOUND__
#include "Sound.h"
#endif

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
    fireParams.lit = false;
    fireParams.kd = R2Pixel(.5, .5, .25, 0);
    fireParams.additive = true;
    fireParams.textureName = "smoke_transparent.jpg";
    mat[0] = new R3Material(fireParams);

    fireParams.kd = R2Pixel(.5, .25, .25, 0);
    mat[1] = new R3Material(fireParams);

    fireParams.kd = R2Pixel(.3, .25, .2, 0);
    mat[2] = new R3Material(fireParams);

    fireParams.kd = R2Pixel(.2, .2, 0.2, 0);
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
    source->shape = new R3Sphere(R3Point(0, 0, 0), 0.8);
    source->rate = 50;
    source->velocity = 2;
    source->angle_cutoff = .5;
    source->mass = 1;
    source->fixed = false;
    source->drag = 0.15;
    source->elasticity = 0;
    source->lifetime = 1.2;
    source->size = 1.3;

    source->numMaterials = 4;
    source->materials = Explosion::mat;

    node->AttachSource(source);

#ifndef __NOSOUND__
    globals.sounds.explosion->Play();
#endif
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
