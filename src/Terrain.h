#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include "Globals.h"
#include "R3Node.h"
#include "GameObject.h"

class Patch
{
    public:
        typedef unsigned int ID;

    private:
        ID id;
};

class Terrain : public GameObject
{
    protected:
        R3Node *node;
        //std::map<location, R3Mesh> map;

    public:
        typedef unsigned int ID;

        GameObjectType GetType(void) { return OBJ_TERRAIN; }

        Patch::ID nextID;
        typedef std::map<Patch::ID, Patch*> PatchMap;


        struct Params { };
        Terrain() { };
        Terrain(const Params &params_);

        void Create(void);
        void Update(double dt);
        void Destroy(void);
};

#endif
