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

#define FORWARD_VELCOITY 1

#define BOUNDARY 10 

Player::Player(const Params &params_) :
    params(params_)
{
}

Player::~Player()
{
}

void Player::Create(void)
{
    // let everyone know
    globals.player = this;

    // create material
    R3Material::Params matParams;
    mat = new R3Material(matParams);

    // create nodes

    //R3Cylinder *debugshape = new R3Cylinder(R3null_point, 0.2, 3);

    nodes.yawpos = new R3Node(NULL, NULL, params.transform);
    nodes.pitch = new R3Node(NULL, NULL, R3identity_matrix);
    nodes.roll = new R3Node(this, new R3Mesh("arwing.off"), mat, R3identity_matrix);

    R3ParticleSource *source = new R3ParticleSource();
    source->shape = new R3Sphere(R3Point(0, 0, 0), 1);
    source->rate = 10;
    source->velocity = 1;
    source->angle_cutoff = 0;
    source->mass = 1;
    source->fixed = false;
    source->drag = 0;
    source->elasticity = 0;
    source->lifetime = 10;
    source->material = mat;
    nodes.roll->AttachSource(source);

    globals.scene->root->AddChild(nodes.yawpos);
    nodes.yawpos->AddChild(nodes.pitch);
    nodes.pitch->AddChild(nodes.roll);

    rotation.yaw = 0;
    rotation.pitch = 0;
    rotation.roll = 0;

    position = params.transform.getOrigin();

    // camera targets

    globals.camerahandler->SetLookTarget(nodes.yawpos);

    nodes.camMove = new R3Node(NULL, NULL, R3Matrix(R3Point(0, 6, 21)));
    nodes.pitch->AddChild(nodes.camMove);
    globals.camerahandler->SetMoveTarget(nodes.camMove);

    nodes.camLook = new R3Node(NULL, NULL, R3Matrix(R3Point(0, 3.5, 0)));
    nodes.yawpos->AddChild(nodes.camLook);
    globals.camerahandler->SetLookTarget(nodes.camLook);

    // initialize some values
    fireTimer = 0;
    health = 1;
    lives = 3;
    score = 0;
}

void Player::Update(double dt)
{
    //health -= dt*.01;
    // yaw/pitch/roll

    rotation.yaw += (globals.keys['z'] - globals.keys['c']) * ROLL_SPEED * dt;
    RotAnim(rotation.pitch, ROLL_MAX * (globals.keys['r'] - globals.keys['f']), ROLL_SPEED, dt);
    RotAnim(rotation.roll,  ROLL_MAX * (globals.keys['a'] - globals.keys['d']), ROLL_SPEED, dt);

    // move

    R3Vector dx(
            globals.keys['d'] - globals.keys['a'],
            globals.keys['w'] - globals.keys['s'],
            globals.keys['k'] - globals.keys['i']
            );

    dx.Transform(R3Matrix::XRotation(rotation.pitch));     // direction given by yaw, pitch
    dx.Transform(R3Matrix::YRotation(rotation.yaw));

    position += MOVE_SPEED * dx * dt;

    // actually set the transform

    //if (position.X() < -BOUNDARY)
        //position.SetX(-BOUNDARY);
    //if (position.X() > BOUNDARY)
        //position.SetX(BOUNDARY);
    //if (position.Y() < -BOUNDARY)
        //position.SetY(-BOUNDARY);
    //if (position.Y() > BOUNDARY)
        //position.SetY(BOUNDARY);

    nodes.yawpos->transformation = R3Matrix(position);     // have to translate first then rotate

    nodes.yawpos->transformation.YRotate(rotation.yaw);    // else it gets weird
    nodes.pitch->transformation = R3Matrix::XRotation(rotation.pitch);
    nodes.roll->transformation = R3Matrix::ZRotation(rotation.roll);

    // shoot

    if (fireTimer > 0)
        fireTimer -= dt;
    else if (globals.keys['j'])
    {
        Shot::Params shotparams;
        shotparams.transform = GetPosition();
        shotparams.direction = nodes.pitch->getWorldTransform() * R3negz_vector;
        shotparams.playershot = true;
        globals.gomgr->Add(new Shot(shotparams));
        fireTimer = FIRE_PERIOD;
    }
}

void Player::Destroy()
{
    globals.player = NULL;
    delete mat;
}

R3Point Player::GetPosition()
{
    return position;
}

float Player::GetHealth()
{
    return health;
}
