#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include "Globals.h"
#include "R3Node.h"
#include "GameObject.h"

class Terrain : public GameObject
{
    public:
        typedef unsigned int ID;

        GameObjectType GetType(void) { return OBJ_TERRAIN; }

        struct Params { R2Image *heightMap; };
        Terrain();
        Terrain(const Params &params_);

        void Create(void);
        void Update(double dt);
        void Destroy(void);
        
    protected:
        R3Mesh *Patch(R3Point center, R2Point size, R2Point dps);

        Params params;
        R3Node *node;
        //std::map<location, R3Mesh> map;

};

#endif
