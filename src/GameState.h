#ifndef __GAMESTATE_H__
#define __GAMESTATE_H__

#include <string>
#include <map>
#include <vector>

/*
 * base class for all game logic types
 */

class GameState
{
    public:
        friend class GameStateManager;

        // unique identifier for each GameState instance
        typedef unsigned int ID;

        // custom properties for each object
        typedef std::pair< std::string, std::vector<std::string> > Property;
        struct Properties : public std::map< std::string, std::vector<std::string> >
        {
        };


        // constructor, destructor
        GameState() { }
        virtual ~GameState() { }

        // events
        virtual void Update(double dt) { }    // logic update
        // virtual void Create(void) { }         // logic create (independent of malloc)
        // virtual void Destroy(void) { }        // logic destroy (independent of free)

        virtual void Start(void) { }
        virtual void Stop(void) { }

        // public API
        ID GetID(void) { return id; }

    private:
        ID id;
};




/*
 * manages GameState creation/destruction, event broadcast
 */

class GameStateManager
{
    private:
        GameState::ID nextID;
        typedef std::map<GameState::ID, GameState *> GameStateMap;
        GameStateMap gameStates;

        GameState::ID currentID;

    public:
        GameStateManager() :
            nextID(0),
            currentID(0)
        {
        }

        void Jump(GameState::ID id) {
            currentID = id;
        }

        void Add(GameState *obj)
        {
            // assign next free ID
            while (gameStates.find(nextID) != gameStates.end())
                ++nextID;
            obj->id = nextID++;
            gameStates[obj->id] = obj;
            currentID = obj->id;
        }

        GameState* Current() {
            return gameStates[currentID];
        }

        void Start() {
            gameStates[currentID]->Start();
        }

        void Stop() {
            gameStates[currentID]->Stop();
        }

        void Update(double dt)
        {
            // notify
            if (gameStates[currentID])
                gameStates[currentID]->Update(dt);
        }
};

#endif
