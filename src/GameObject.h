#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include <string>
#include <map>
#include <vector>

/*
 * GameObject types (maybe a better way to do this?)
 */

enum GameObjectType
{
    OBJ_UNDEF,
    OBJ_PLAYER,
    OBJ_PLAYERSHOT,
    OBJ_ENEMYSHOT,
    OBJ_FLOCK,
    OBJ_BOID
};



/*
 * base class for all game logic types
 */

class GameObject
{
    public:
        friend class GameObjectManager;

        // unique identifier for each GameObject instance
        typedef unsigned int ID;

        // custom properties for each object
        typedef std::pair< std::string, std::vector<std::string> > Property;
        struct Properties : public std::map< std::string, std::vector<std::string> >
        {
        };


        // constructor, destructor
        GameObject() { }
        virtual ~GameObject() { }


        // events
        virtual void Update(double dt) { }            // logic update
        virtual void Create(void) { }                 // logic create (independent of malloc)
        virtual void Destroy(void) { }                // logic destroy (independent of free)
        virtual void Collide(GameObject *other) { }   // physical collision with `other'


        // public API
        ID GetID(void) { return id; }
        virtual GameObjectType GetType(void) { return OBJ_UNDEF; }


    private:
        ID id;
};




/*
 * manages GameObject creation/destruction, event broadcast
 */

class GameObjectManager
{
    private:
        GameObject::ID nextID;
        typedef std::map<GameObject::ID, GameObject *> GameObjectMap;
        GameObjectMap gameObjects;

        std::vector<GameObject::ID> toDestroy;

        // actually destroy an object
        void _Destroy(GameObject::ID id)
        {
            GameObjectMap::iterator found = gameObjects.find(id);

            if (found != gameObjects.end())
            {
                gameObjects.erase(found);
                found->second->Destroy();
                //delete found->second;
            }
        }

    public:
        GameObjectManager()
            : nextID(0)
        {
        }

        void Add(GameObject *obj)
        {
            // assign next free ID
            while (gameObjects.find(nextID) != gameObjects.end())
                ++nextID;
            obj->id = nextID++;
            gameObjects[obj->id] = obj;

            // notify
            obj->Create();
        }

        void Destroy(GameObject::ID id)
        {
            toDestroy.push_back(id);
        }

        void Update(double dt)
        {
            // notify
            for (GameObjectMap::reverse_iterator iter = gameObjects.rbegin();
                    iter != gameObjects.rend(); ++iter)
                iter->second->Update(dt);

            // destroy marked
            for (std::vector<GameObject::ID>::iterator iter = toDestroy.begin();
                    iter != toDestroy.end(); ++iter)
                _Destroy(*iter);
        }
};



#endif
