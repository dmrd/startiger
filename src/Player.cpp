#include "Globals.h"
#include "R3Node.h"
#include "GameObject.h"
#include "Player.h"
#include "Shot.h"
#include "R3Scene.h"
#include <cmath>

#define MAX_ROTATION .5
#define ROTATION_RATE .5

#define FIRE_RATE .25


Player::Player(const Params &params_) :
    params(params_)
{
}

Player::~Player()
{
    delete mat;
}

void Player::Create(void)
{
    // let everyone know
    globals.player = this;

    // create material
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

    // create anode
    node = new R3Node(new R3Mesh("arwing.off"),
            mat, params.transform);
    globals.scene->root->AddChild(node);

    // camera targets
    globals.camerahandler->SetLookTarget(node);
    cameramovetarget = new R3Node(NULL, NULL, R3Matrix(R3Point(0, 1.5, 12)));
    node->AddChild(cameramovetarget);
    globals.camerahandler->SetMoveTarget(cameramovetarget);

    // initialize some values
    rotation = 0;
    lastFire = 0;
}

void Player::Update(double dt)
{
    // move
    node->transformation.ZRotate(-rotation);
    node->transformation.Translate(R3Vector(
                3 * (globals.keys['d'] - globals.keys['a']),
                3 * (globals.keys['w'] - globals.keys['s']),
                0
                ) * dt);

    // banking left and right
    double frameRotation = -(globals.keys['d'] - globals.keys['a']) * ROTATION_RATE * dt;
    if (frameRotation*rotation <= 0 && abs(rotation) > 0.01) {
        frameRotation = -rotation  / abs(rotation) * 2 * ROTATION_RATE * dt;
    }

    if (abs(rotation+frameRotation) > MAX_ROTATION)
        frameRotation = 0;
    rotation += frameRotation;
    node->transformation.ZRotate(rotation);

    lastFire -= dt;
    // shooting
    if (lastFire <= 0 && globals.keys['j']) {
        Shot::Params shotparams;
        shotparams.transform = GetPosition();
        globals.gomgr->Add(new Shot(shotparams));
        lastFire = FIRE_RATE;
    }
}

void Player::Destroy()
{
    globals.player = NULL;
}

R3Point Player::GetPosition()
{
    R3Point pos(R3null_point);
    pos.Transform(node->transformation);
    return pos;
}

