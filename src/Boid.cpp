#include "Globals.h"
#include "R3Node.h"
#include "GameObject.h"
#include "Boid.h"
#include "Flock.h"
#include "R3Scene.h"

Boid::Boid(R3Point spawn_, R3Vector direction_, double speed_) :
    spawn(spawn_),
    direction(direction_),
    speed(speed_)
{
}

Boid::~Boid()
{
    delete mat;
}

void Boid::Create(void)
{
    // create material, node

    mat = new R3Material();
    mat->ka = R3Rgb(0.2, 0.2, 0.2, 1);
    mat->kd = R3Rgb(0.5, 0.5, 0.5, 1);
    mat->ks = R3Rgb(0.5, 0.5, 0.5, 1);
    mat->kt = R3Rgb(0.0, 0.0, 0.0, 1);
    mat->emission = R3Rgb(0, 0, 0, 1);
    mat->shininess = 10;
    mat->indexofrefraction = 1;
    mat->texture = NULL;
    mat->id = 0;

    node = new R3Node(new R3Sphere(R3null_point, 0.02),
            mat, R3Matrix(spawn));

    globals.scene->root->AddChild(node);
}

void Boid::Update(double dt)
{
    // move
    node->transformation.Translate(direction * speed * dt);
}

/* TODO: write delete function */
void Boid::Destroy()
{
    //delete node;
}
