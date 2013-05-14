#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include "Globals.h"
#include "R3Node.h"
#include "GameObject.h"

#define TERRAIN_SIZE 300
#define TERRAIN_DPS 200



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
        double Height(R3Point pos);
        
    protected:
        double _Height(int x, int y)
        {
            if (x < 0) x = 0;
            if (y < 0) y = 0;
            if (x > params.heightMap->Width() - 1) x = params.heightMap->Width() - 1;
            if (y > params.heightMap->Width() - 1) y = params.heightMap->Width() - 1;
            return 25 * params.heightMap->Pixel(round(x), round(y)).Luminance();
        }

        R3Mesh *Patch(R3Point center, R2Point size, R2Point dps);

        Params params;
        R3Node *node;
        //std::map<location, R3Mesh> map;

};

#endif
