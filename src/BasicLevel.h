#ifndef __BASICLEVEL_H__
#define __BASICLEVEL_H__

#include "GameState.h"

class R3Node;

class BasicLevel : public GameState
{
    public:
        BasicLevel();
        ~BasicLevel();

        void Update(double dt);
        void Start(void);
        void Stop(void);

    protected:
        R3Node *treasure;
        void SpawnTreasure();
};

#endif
