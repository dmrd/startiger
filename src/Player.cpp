#include "Globals.h"
#include "R3Node.h"
#include "GameObject.h"
#include "Player.h"
#include "Rails.h"
#include "Shot.h"
#include "R3Scene.h"
#include "soundfix.h"
#ifndef __NOSOUND__
#include "Sound.h"
#endif
#include "Terrain.h"
#include <cmath>

#define ROLL_MAX 0.5


#define FIRE_PERIOD .05

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

    R3Material::Params reticleMatParams;
    reticleMatParams.textureName = "reticle.jpg";
    reticleMatParams.lit = false;
    reticleMat = new R3Material(reticleMatParams);

    // create nodes
    //
    nodes.reticleFar = new R3Node(new R3Mesh("reticleFar.off", true), reticleMat, R3Matrix(R3Point(0,0,-10)));
    nodes.reticleNear = new R3Node(new R3Mesh("reticle.off", true), reticleMat, R3Matrix(R3Point(0,0,-5)));

    nodes.yawpos = new R3Node(NULL, NULL, params.transform);
    nodes.pitch = new R3Node(NULL, NULL, R3identity_matrix);

    nodes.pitch->AddChild(nodes.reticleFar);
    nodes.pitch->AddChild(nodes.reticleNear);

    R3Light *light = new R3Light(R3_POINT_LIGHT, R3Point(0, 0, -5), R3null_vector,
            0, R3Rgb(0.1, 0.4, 0.9, 1), 0.2, 0, 0.008, 0, M_PI);
    globals.scene->AddLight(light);
    nodes.yawpos->AttachLight(light);

    nodes.roll = new R3Node(this, new R3Mesh("arwing.off"), mat, R3identity_matrix);

    R3ParticleSource *source = new R3ParticleSource();
    source->shape = new R3Circle(R3Point(0, 0, 0), .3, R3posz_vector);
    source->rate = 0;
    source->velocity = 10;
    source->angle_cutoff = M_PI/7;
    source->mass = 1;
    source->fixed = false;
    source->drag = 0.2;
    source->elasticity = 0;
    source->lifetime = .2;
    source->size = .8;

    source->numMaterials = 4;
    source->materials = new R3Material*[4];

    R3Material::Params fireParams; 
    fireParams.lit = false;
    fireParams.kd = R2Pixel(.0, .1, .8, 0);
    fireParams.additive = true;
    fireParams.textureName = "flare.jpg";
    source->materials[0] = new R3Material(fireParams);

    fireParams.kd = R2Pixel(.5, .5, 0, 0);
    source->materials[1] = new R3Material(fireParams);

    fireParams.kd = R2Pixel(.5, .25, 0, 0);
    source->materials[2] = new R3Material(fireParams);

    fireParams.kd = R2Pixel(.5, 0, 0, 0);
    source->materials[3] = new R3Material(fireParams);

    nodes.jet = new R3Node(NULL, NULL, R3Matrix(R3Point(0, 0, 0.5)));
    nodes.pitch->AddChild(nodes.jet);
    nodes.jet->AttachSource(source);

    globals.scene->root->AddChild(nodes.yawpos);
    nodes.yawpos->AddChild(nodes.pitch);
    nodes.pitch->AddChild(nodes.roll);


    rotation.yaw = 3.14159;
    rotation.pitch = 0;
    rotation.roll = 0;

//    nodes.yawpos->transformation = 
    //R2Point size = R2Point(TERRAIN_SIZE,TERRAIN_SIZE);
    //R2Point dps = R2Point(TERRAIN_DPS,TERRAIN_DPS);
    //double dx = (double)globals.rails->currentLocation.X()/globals.rails->GetWidth() * size.X() - size.X() / 2;
    //double dz = (double)globals.rails->currentLocation.Y()/globals.rails->GetHeight() * size.Y() - size.Y() / 2;
    //position = R3Point(dx, 1, dz);
//params.transform.getOrigin();

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
    xVal = 0;
    yVal = 0;
}

void Player::SetPosition(R3Point p) {
}

void Player::SetDirection(float angle) {
    //rotation.yaw = angle;
}

R3Vector Player::GetDirection() {
    R3Vector dir = R3Vector(R3negz_vector);
    dir.Rotate(R3posy_vector, rotation.yaw);
    printf("%f %f %f\n", dir.X(), dir.Y(), dir.Z());
    printf("%f %f %f\n", position.X(), position.Y(), position.Z());
    return dir;
}


void Player::Update(double dt)
{
    if (health < 0) {
        globals.levelStatus = 2;
    }
    //health -= dt*.01;
    // yaw/pitch/roll

    rotation.yaw += (globals.keys['z'] - globals.keys['c']) * ROLL_SPEED * dt;
    RotAnim(rotation.pitch, ROLL_MAX * (globals.keys['r'] - globals.keys['f']), ROLL_SPEED, dt);
    RotAnim(rotation.roll,  ROLL_MAX * (globals.keys['a'] - globals.keys['d']), ROLL_SPEED, dt);

    // move

    int rightleft = (globals.keys['d'] - globals.keys['a']);
    int updown = (globals.keys['w'] - globals.keys['s']);
    double horizontalMove =  rightleft * MOVE_SPEED * dt;
    double verticalMove = updown * MOVE_SPEED * dt;
    //if (xVal + horizontalMove > BOUNDARY) {
        //horizontalMove = 0;
        //rightleft = 0;
    //}
    //if (xVal + horizontalMove < -BOUNDARY) {
        //horizontalMove = 0;
        //rightleft = 0;
    //}

    xVal += horizontalMove;
    yVal += verticalMove;
    R3Vector dx(
            rightleft,
            updown,
            //-1
            globals.keys['k'] - globals.keys['i']
            );

#define JET_RATE 190
    if (dx.IsZero())
    {
        nodes.jet->source->rate = 0.6*JET_RATE;
        nodes.jet->source->size = 0.3;
        nodes.jet->source->velocity = 2;
        nodes.jet->source->lifetime = 0.12;
    }
    else
    {
        nodes.jet->source->rate = JET_RATE;
        nodes.jet->source->size = 1;
        nodes.jet->source->velocity = 10;
        nodes.jet->source->lifetime = 0.2;
    }

    dx.Transform(R3Matrix::XRotation(rotation.pitch));     // direction given by yaw, pitch
    dx.Transform(R3Matrix::YRotation(rotation.yaw));

    dx *= MOVE_SPEED;

    position += dx * dt;

    // Keep player above terrain
    double height = globals.terrain->Height(GetPosition()); 
    if (position.Y() < height) {
        position.SetY(height);
    }
    //if (position.Y() > height+10) {
        //position.SetY(height+10);
    //}

    // actually set the transform

    //if (position.X() < -BOUNDARY)
        //position.SetX(-BOUNDARY);
    //if (position.X() > BOUNDARY)
        //position.SetX(BOUNDARY);
    //if (position.Y() < -2)
        //position.SetY(-2);
    //if (position.Y() > BOUNDARY)
        //position.SetY(BOUNDARY);

    nodes.yawpos->transformation = R3Matrix(position);     // have to translate first then rotate

    nodes.yawpos->transformation.YRotate(rotation.yaw);    // else it gets weird
    nodes.pitch->transformation = R3Matrix::XRotation(rotation.pitch);
    nodes.roll->transformation = R3Matrix::ZRotation(rotation.roll);
    nodes.roll->transformation.Scale(1.3);

    // shoot

    if (fireTimer > 0)
        fireTimer -= dt;
    else if (globals.keys['j'])
    {
#ifndef __NOSOUND__
        globals.sounds.shot[(int) (2.9 * Util::UnitRandom())]->Play();
#endif
        Shot::Params shotparams;
        shotparams.transform = GetPosition();
        shotparams.direction = nodes.pitch->getWorldTransform() * R3negz_vector;
        shotparams.playershot = true;
        shotparams.basevel = dx;
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
    if (health < 0) { return 0; }
    return health;
}
