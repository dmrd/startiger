#ifndef __SHOT_H__
#define __SHOT_H__

#include "Globals.h"
#include "R3Node.h"
#include "GameObject.h"
#include "CameraHandler.h"
#include "R3Scene.h"

class Shot : public GameObject
{
    public:
        struct Params { R3Matrix transform; };
        Shot(const Params &params_);
        ~Shot();

        void Create(void);
        void Update(double dt);
        void Destroy();

        R3Point GetPosition();

    protected:
        Params params;

        R3Node *node;
        R3Material *mat;
};

#endif