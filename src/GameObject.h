#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include <string>
#include <map>
#include <vector>

/*
 * base class for all game logic types
 */

class GameObject
{
    public:
        // unique identifier for each GameObject instance
        typedef unsigned int ID;

        // custom properties for each object
        typedef std::pair< std::string, std::vector<std::string> > Property;
        struct Properties : public std::map< std::string, std::vector<std::string> >
        {
        };

        // events
        void Update(double dt);

    private:
        ID id;
};

#endif

