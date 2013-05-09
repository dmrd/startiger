#include "Globals.h"
#include "R3Node.h"
#include "GameObject.h"
#include "Shot.h"
#include "R3Scene.h"
#include <cmath>


#define BULLET_SPEED -40

Shot::Shot(const Params &params_) :
    params(params_)
{
}

Shot::~Shot()
{
    delete mat;
}

void Shot::Create(void)
{
    // create material, node

    mat = new R3Material();
    mat->ka = R3Rgb(0.0, 0.5, 0.0, 1);
    mat->kd = R3Rgb(0.0, 0.5, 0.0, 1);
    mat->ks = R3Rgb(0.0, 0.5, 0.0, 1);
    mat->kt = R3Rgb(0.0, 0.0, 0.0, 1);
    mat->emission = R3Rgb(0, 0, 0, 1);
    mat->shininess = 10;
    mat->indexofrefraction = 1;
    mat->texture = NULL;
    mat->id = 0;

    node = new R3Node(new R3Cylinder(R3Point(0, 0, 0), .05, 1),
            mat);

    node->transformation.Transform(params.transform);
    node->transformation.ZTranslate(-1);
    node->transformation.XRotate(1.57);
    globals.scene->root->AddChild(node);

}

void Shot::Update(double dt)
{
    // move
    node->transformation.Translate(R3Vector(
                0,
                BULLET_SPEED,
                0
                ) * dt);


}

void Shot::Destroy()
{
}

R3Point Shot::GetPosition()
{
    return node->getWorldTransform().getOrigin();
}

