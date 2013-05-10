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

        struct
        {
            R3Node *yawpos;
            R3Node *pitch;
            R3Node *roll;

            R3Node *cameraMove;
        } nodes;

        struct
        {
            double yaw;
            double pitch;
            double roll;
        } rotation;

        R3Point position;

        // move rotvar toward target
        inline void RotAnim(double &rotvar, double target, double speed, double dt)
        {
            if (rotvar < target)
            {
                rotvar += speed * dt;
                if (rotvar > target)
                    rotvar = target;
            }
            else if (rotvar > target)
            {
                rotvar -= speed * dt;
                if (rotvar < target)
                    rotvar = target;
            }
        }

        R3Material *mat;

        double fireTimer;
};

#endif
