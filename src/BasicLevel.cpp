#include "BasicLevel.h"
#include "Globals.h"
#include "Util.h"
#include "Player.h"
#include "Flock.h"
#include "Terrain.h"
#include "Rails.h"

Flock *flock;

#define FLOCK_TIME 30
BasicLevel::BasicLevel()
{
}

BasicLevel::~BasicLevel()
{
}

static double spawnFlockTime = FLOCK_TIME;

R3Point SpawnLocation() {
    R3Point playerPos = globals.player->GetPosition();
    R3Vector playerDir = globals.player->GetDirection();
    R3Vector playerLeft = R3Vector(R3posy_vector);
    playerLeft.Cross(playerDir);
    return playerPos + Util::SymmetricRandom() * 15.0 * playerLeft + 
                        Util::SymmetricRandom() * 15.0 * R3posy_vector + 
                        (Util::UnitRandom() + 0.3) * 100.0 * playerDir;
}

void BasicLevel::SpawnTreasure() {
    double x = Util::SymmetricRandom() * TERRAIN_SIZE / 2.0;
    double y = Util::SymmetricRandom() * TERRAIN_SIZE / 2.0;
    double height = globals.terrain->Height(R3Point(x,0,y));
    
    treasure->transformation = R3Matrix(R3Point(x, height + 2, y));
}

void BasicLevel::Start()
{
    Terrain::Params terrainparams;
    terrainparams.heightMap = new R2Image("heightmap.jpg");
    globals.terrain = new Terrain(terrainparams);
    globals.gomgr->Add(globals.terrain);

    Player::Params playerparams;
    playerparams.transform = R3identity_matrix;
    globals.gomgr->Add(new Player(playerparams));

    Flock::Params flockparams;
    flockparams.spawn = SpawnLocation();
    flockparams.swarmSize = 20;
    flockparams.radius = 20;
    flock = new Flock(flockparams);
    globals.gomgr->Add(flock);

    R3Material **mat = new R3Material*[4];

    R3Material::Params fireParams; 
    fireParams.lit = false;
    fireParams.kd = R2Pixel(0.1, .5, .8, 0);
    fireParams.additive = true;
    fireParams.textureName = "smoke_transparent.jpg";
    mat[0] = new R3Material(fireParams);

    fireParams.kd = R2Pixel(0.1, .25, .6, 0);
    mat[1] = new R3Material(fireParams);

    fireParams.kd = R2Pixel(0.1, .25, .4, 0);
    mat[2] = new R3Material(fireParams);

    fireParams.kd = R2Pixel(0.1, .2, 0.2, 0);
    mat[3] = new R3Material(fireParams);

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
    source->materials = mat;

    treasure = new R3Node(new R3Sphere(R3null_point, 3), mat[0], R3Point(0, 10, 0));
    globals.scene->root->AddChild(treasure);
    treasure->AttachSource(source);

    SpawnTreasure();
}

void BasicLevel::Stop()
{
}

void BasicLevel::Update(double dt)
{
    static double timeSinceClear = 0;
    //if (flock->NBoids() == 0) {
    spawnFlockTime -= dt;

    if (spawnFlockTime < 0) {
        spawnFlockTime = FLOCK_TIME;
        //timeSinceClear += dt;

        //if (timeSinceClear > 5) {
            //timeSinceClear = 0;

            Flock::Params flockparams;
            flockparams.spawn = SpawnLocation();
            flockparams.swarmSize = 10;
            flockparams.radius = 20;
            flock = new Flock(flockparams);
            globals.gomgr->Add(flock);
        //}
    }

    if ((globals.player->GetPosition() - treasure->getWorldTransform().getOrigin()).Length() < 10) {
        globals.player->score += 256;
        SpawnTreasure();
    }

}
