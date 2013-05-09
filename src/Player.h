#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Globals.h"
#include "R3Node.h"
#include "GameObject.h"
#include "CameraHandler.h"
#include "R3Scene.h"

class Player : public GameObject
{
    public:
        struct Params { R3Matrix transform; };
        Player(const Params &params_);
        ~Player();

        void Create(void);
        void Update(double dt);
        void Destroy();

        R3Point GetPosition();

    protected:
        Params params;

        R3Node *node;
        R3Node *cameramovetarget;
        R3Material *mat;

        double rotation;
        double lastFire;
};

#endif
