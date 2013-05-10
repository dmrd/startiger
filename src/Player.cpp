#include "Globals.h"
#include "R3Node.h"
#include "GameObject.h"
#include "Player.h"
#include "Shot.h"
#include "R3Scene.h"
#include <cmath>

#define ROLL_MAX 0.5
#define ROLL_SPEED 2.5

#define MOVE_SPEED 20

#define FIRE_PERIOD .25

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

    // create nodes

    R3Cylinder *debugshape = new R3Cylinder(R3null_point, 0.2, 3);

    nodes.yawpos = new R3Node(debugshape, mat, params.transform);
    nodes.pitch = new R3Node(debugshape, mat, R3identity_matrix);
    nodes.roll = new R3Node(new R3Mesh("arwing.off"), mat, R3identity_matrix);

    globals.scene->root->AddChild(nodes.yawpos);
    nodes.yawpos->AddChild(nodes.pitch);
    nodes.pitch->AddChild(nodes.roll);

    rotation.yaw = 0;
    rotation.pitch = 0;
    rotation.roll = 0;

    position = params.transform.getOrigin();

    // camera targets

    globals.camerahandler->SetLookTarget(nodes.yawpos);

    nodes.cameraMove = new R3Node(NULL, NULL, R3Matrix(R3Point(0, 5, 25)));
    nodes.pitch->AddChild(nodes.cameraMove);
    globals.camerahandler->SetMoveTarget(nodes.cameraMove);

    // initialize some values
    fireTimer = 0;
}

void Player::Update(double dt)
{
    // yaw (easy)

    rotation.yaw += ROLL_SPEED * dt * (globals.keys['z'] - globals.keys['c']);
    
    // pitch

    if (globals.keys['r'])
    {
        if (rotation.pitch < ROLL_MAX)
            rotation.pitch += ROLL_SPEED * dt;
        if (rotation.pitch > ROLL_MAX)
            rotation.pitch = ROLL_MAX;
    }
    else if (globals.keys['f'])
    {
        if (rotation.pitch > -ROLL_MAX)
            rotation.pitch -= ROLL_SPEED * dt;
        if (rotation.pitch < -ROLL_MAX)
            rotation.pitch = -ROLL_MAX;
    }
    else
    {
        if (rotation.pitch < 0)
        {
            rotation.pitch += ROLL_SPEED * dt;
            if (rotation.pitch > 0)
                rotation.pitch = 0;
        }
        if (rotation.pitch > 0)
        {
            rotation.pitch -= ROLL_SPEED * dt;
            if (rotation.pitch < 0)
                rotation.pitch = 0;
        }
    }

    // roll

    if (globals.keys['a'])
    {
        if (rotation.roll < ROLL_MAX)
            rotation.roll += ROLL_SPEED * dt;
        if (rotation.roll > ROLL_MAX)
            rotation.roll = ROLL_MAX;
    }
    else if (globals.keys['d'])
    {
        if (rotation.roll > -ROLL_MAX)
            rotation.roll -= ROLL_SPEED * dt;
        if (rotation.roll < -ROLL_MAX)
            rotation.roll = -ROLL_MAX;
    }
    else
    {
        if (rotation.roll < 0)
        {
            rotation.roll += ROLL_SPEED * dt;
            if (rotation.roll > 0)
                rotation.roll = 0;
        }
        if (rotation.roll > 0)
        {
            rotation.roll -= ROLL_SPEED * dt;
            if (rotation.roll < 0)
                rotation.roll = 0;
        }
    }

    // move

    R3Vector dx(
            globals.keys['d'] - globals.keys['a'],
            globals.keys['w'] - globals.keys['s'],
            globals.keys['k'] - globals.keys['i']
            );

    dx.Transform(R3Matrix::XRotation(rotation.pitch));
    dx.Transform(R3Matrix::YRotation(rotation.yaw));

    position += MOVE_SPEED * dx * dt;

    // actually set the transform

    nodes.yawpos->transformation = R3Matrix(position);
    nodes.yawpos->transformation.YRotate(rotation.yaw);
    nodes.pitch->transformation = R3Matrix::XRotation(rotation.pitch);
    nodes.roll->transformation = R3Matrix::ZRotation(rotation.roll);

    // shoot

    if (fireTimer > 0)
        fireTimer -= dt;
    else if (globals.keys['j'])
    {
        Shot::Params shotparams;
        shotparams.transform = GetPosition();
        shotparams.direction = R3negz_vector;
        globals.gomgr->Add(new Shot(shotparams));
        fireTimer = FIRE_PERIOD;
    }
}

void Player::Destroy()
{
    globals.player = NULL;
}

R3Point Player::GetPosition()
{
    return position;
}

