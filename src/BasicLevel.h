#ifndef __BASICLEVEL_H__
#define __BASICLEVEL_H__

#include "GameState.h"

class BasicLevel : public GameState
{
    public:
        BasicLevel();
        ~BasicLevel();

        void Update(double dt);
        void Start(void);
        void Stop(void);

    protected:
};

#endif
