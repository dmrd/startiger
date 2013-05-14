#ifndef __RAILS_H__
#define __RAILS_H__

#include "Globals.h"
#include "R3Node.h"
#include "GameObject.h"



class Rails : public GameObject
{
    public:
        typedef unsigned int ID;

        GameObjectType GetType(void) { return OBJ_RAILS; }

        struct Params { R2Image *railMap; };
        Rails();
        Rails(const Params &params_);

        void Create(void);
        void Update(double dt);
        void Destroy(void);
        
        R2Point currentLocation;
        double GetWidth(void);
        double GetHeight(void);

    protected:


        Params params;
        R3Node *node;
        R2Point oldLocation;
        R2Point oldoldLocation;
        R2Point nextLocation;
        float currentDirection;
        float targetDirection;
        double time;
        //std::map<location, R3Mesh> map;

};

#endif
