#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "GameState.h"

class Level : public GameState
{
    public:
        Level() { };
        ~Level() { };

        void Update(double dt);
        void Start(void);
        void Stop(void);

    protected:
};

#endif
