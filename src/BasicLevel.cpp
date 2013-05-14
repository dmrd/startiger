#include "BasicLevel.h"
#include "Globals.h"
#include "Util.h"
#include "Player.h"
#include "Flock.h"
#include "Terrain.h"

Flock *flock;

BasicLevel::BasicLevel()
{
}

BasicLevel::~BasicLevel()
{
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
    flockparams.spawn = R3Point(-5,0,-10);
    flockparams.swarmSize = 20;
    flockparams.radius = 20;
    flock = new Flock(flockparams);
    globals.gomgr->Add(flock);
}

void BasicLevel::Stop()
{
}

void BasicLevel::Update(double dt)
{
    static double timeSinceClear = 0;
    if (flock->NBoids() == 0) {
        timeSinceClear += dt;

        if (timeSinceClear > 5) {
            timeSinceClear = 0;

            printf("new flock \n");
            Flock::Params flockparams;
            flockparams.spawn = globals.player->GetPosition();
            flockparams.swarmSize = 10;
            flockparams.radius = 20;
            flock = new Flock(flockparams);
            globals.gomgr->Add(flock);
        }
    }
}
