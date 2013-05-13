#ifndef __EXPLOSION_H
#define __EXPLOSION_H

#include "Globals.h"
#include "R3Node.h"
#include "GameObject.h"
#include "Explosion.h"


class Explosion : public GameObject
{
    protected:
        static bool initialized;
    public:
        GameObjectType GetType(void) { return OBJ_EXPLOSION; }

        Explosion(R3Matrix transformation);
        ~Explosion();

        void Create(void);
        void Update(double dt);
        void Destroy();
        void Collide(GameObject *other)
        {
        }

        static void Initialize();
        R3Point GetPosition();

        R3Node *node;
        R3Matrix transform;
        static R3Material **mat;

        double lifetime;

};

#endif
