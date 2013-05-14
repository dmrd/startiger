#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Globals.h"
#include "R3Node.h"
#include "GameObject.h"
#include "CameraHandler.h"
#include "R3Scene.h"
#ifndef __NOSOUND__
#include "Sound.h"
#endif

#define MOVE_SPEED 25.0
#define ROLL_SPEED 2.5
class Player : public GameObject
{
    public:
        GameObjectType GetType(void) { return OBJ_PLAYER; }

        struct Params { R3Matrix transform; };
        Player(const Params &params_);
        ~Player();

        // events
        void Create(void);
        void Update(double dt);
        void Destroy();
        void Collide(GameObject *other)
        {
            if (other && other->GetType() == OBJ_ENEMYSHOT)
            {
                globals.gomgr->Destroy(other->GetID());
                health -= 0.01;
            }
        }

        // public API
        R3Point GetPosition();
        float GetHealth();

        void SetDirection(float angle);
        void SetPosition(R3Point p);
        int lives;
        int score;

    protected:
        Params params;

        struct
        {
            R3Node *yawpos;
            R3Node *pitch;
            R3Node *roll;

            R3Node *camMove;   // camera position target
            R3Node *camLook;   // camera look target

            R3Node *reticleNear;
            R3Node *reticleFar;

            R3Node *jet;
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
        R3Material *reticleMat;

        float health;
        double fireTimer;
        float xVal;
        float yVal;
};

#endif
