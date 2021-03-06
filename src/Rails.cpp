#include "Globals.h"
#include "Rails.h"
#include "Player.h"
#include "Terrain.h"
#include "R3Node.h"
#include "R3Scene.h"
#include "GameObject.h"
#include "Util.h"
#include <cmath>

// Always have 9
Rails::Rails()
{
}

Rails::Rails(const Params &params_) :
    params(params_)
{
}


void Rails::Create(void)
{

    for (int i = 0; i < params.railMap->Width(); i++) {
        for (int j = 0; j < params.railMap->Height(); j++) {
            if (params.railMap->Pixel(i,j).Luminance() > .7)
                currentLocation = R2Point(i, j);
        }
    }
    /*
    for (int i = 0; i < params.railMap->Width()-1; i++) {
        for (int j = 0; j < params.railMap->Height()-1; j++) {
            if (params.railMap->Pixel(i,j)[0] > .5) {
                if (params.railMap->Pixel(i,j+1)[0] > .5 && params.railMap->Pixel(i+1,j+1)[0] > .5) {
                    params.railMap->Pixel(i,j+1)[0] = 0;//, R2black_pixel);
                }
                if (params.railMap->Pixel(i+1,j)[0] > .5 && params.railMap->Pixel(i+1,j+1)[0] > .5) {
                    params.railMap->Pixel(i+1,j)[0] = 0;//, R2black_pixel);
                }
            }
            
        }
    }
    */
    oldLocation = currentLocation;
    oldoldLocation = currentLocation;
    targetDirection = 3.14159;
    currentDirection = 3.14159;

    time = 1.0/MOVE_SPEED*TERRAIN_DPS/(double)TERRAIN_SIZE;

    node = new R3Node();
    node->transformation = R3identity_matrix;

    globals.scene->root->AddChild(node);

}

void Rails::Update(double dt)
{
    time -= dt;
    if (time < 0) {
        int i = -1;
        int j = -1;
        time += 1.0/MOVE_SPEED*(double)GetWidth()/TERRAIN_DPS;//*GetWidth()/(double)TERRAIN_SIZE;//TERRAIN_SIZE/(double)TERRAIN_DPS;//*coeff;//(double)GetWidth()*coeff;

        float greatest = 0;
        R2Point previousLocation = oldLocation;
        R2Point firstLocation = currentLocation;
        R2Point secondLocation = currentLocation;
        for (int k = 0; k < 5; k++) {
            float greatest = 0;
            for (i = -1; i <= 1; i++) {
                for (j = -1; j <= 1; j++) {
                    if (i == 0 && j == 0)
                        continue;
                    if (params.railMap->Pixel(currentLocation.X()+i,currentLocation.Y()+j)[0] > greatest) {
                        R2Point tempLocation = R2Point(currentLocation.X()+i,currentLocation.Y()+j);
                        if (tempLocation.X() != oldLocation.X() || tempLocation.Y() != oldLocation.Y()) {
                            if (tempLocation.X() != oldoldLocation.X() || tempLocation.Y() != oldoldLocation.Y()) {
                                greatest = params.railMap->Pixel(currentLocation.X()+i,currentLocation.Y()+j)[0];
                                nextLocation = tempLocation;
                            }
                        }
                    }
                }
            } 

            if (k==0)
                secondLocation = nextLocation;
            oldoldLocation = oldLocation;
            oldLocation = currentLocation;
            currentLocation = nextLocation;

        }
        for (i = -1; i <= 1; i++) {
            for (j = -1; j <= 1; j++) {
                if (i == 0 && j == 0)
                    continue;
                if (params.railMap->Pixel(currentLocation.X()+i,currentLocation.Y()+j)[2] > .5) {
                    globals.levelStatus = 1;
                    globals.gsmgr->Stop();
                }
            }
        } 



        R3Vector v = R3Vector(-(currentLocation.X()-firstLocation.X()), 0, currentLocation.Y()-firstLocation.Y());
        targetDirection = acos(-R3zaxis_vector.Dot(v)/v.Length());
        targetDirection = targetDirection - (int)(targetDirection/6.28);
        if (v.X() < 0) {
            targetDirection = 6.28-targetDirection;
        }
        oldoldLocation = previousLocation;
        oldLocation = firstLocation;
        currentLocation = secondLocation;

    }
    //double coeff = (sqrt(2) - 1)*abs((firstLocation.X()-currentLocation.X())*(currentLocation.Y()-firstLocation.Y())) + 1;
    //printf("coeff %f\n", coeff);
    float closeness = 0;
    if (targetDirection < 0)
        closeness = (-currentDirection + targetDirection);
    else
        closeness = (-currentDirection + targetDirection);
    currentDirection += ROLL_SPEED*dt * closeness;//abs(closeness)*closeness;
    if (abs(currentDirection - targetDirection) > .05) {
        /*
           if (currentDirection < targetDirection)
           currentDirection += ROLL_SPEED*dt;
           if (currentDirection > targetDirection)
           currentDirection -= ROLL_SPEED*dt;
           */
    }
    //currentDirection = targetDirection;//abs(closeness)*closeness;
    globals.player->SetDirection(currentDirection);
}

void Rails::Destroy(void)
{
    
}

double Rails::GetWidth(void)
{
    return params.railMap->Width();
}

double Rails::GetHeight(void)
{
    return params.railMap->Height();
}
