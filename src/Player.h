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

        float GetHealth();

    protected:
    public:
        Params params;

        struct
        {
            R3Node *yawpos;
            R3Node *pitch;
            R3Node *roll;

            R3Node *camMove;   // camera position target
            R3Node *camLook;   // camera look target
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

        float health;
        double fireTimer;
        int lives;
};

#endif
