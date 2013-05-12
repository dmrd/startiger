#ifndef __R3SCENE_H__
#define __R3SCENE_H__

#include "R3/R3.h"
#include "R3Node.h"
#include "R3Material.h"
#include "R3Light.h"
#include "R3Camera.h"

#define TRAIL_LENGTH 10

// Particle system definitions

struct R3Particle {
  R3Point position;
  R3Vector velocity;
  double mass;
  bool fixed;
  double drag;
  double elasticity;
  double lifetime;
  double startLifetime;
  R3Material *material;
  R3Material **materials;
  int numMaterials;
  vector<struct R3ParticleSpring *> springs;
  R3Point trail[TRAIL_LENGTH];
  int last_trail;
  int boid;
  double size;
};

struct R3ParticleSource {
  R3Shape *shape;
  R3Node *node;
  double rate;
  double velocity;
  double angle_cutoff;
  double mass;
  bool fixed;
  double drag;
  double elasticity;
  double lifetime;
  double size;
  R3Material *material;
  R3Material **materials;
  int numMaterials;

  ~R3ParticleSource() {
        delete material;
        for (int i = 0; i < numMaterials; i++)
            delete materials[i];
        delete materials;
        delete shape;
  }
};

struct R3ParticleSink
{
    R3Shape *shape;
    double intensity;
    double constant_attenuation;
    double linear_attenuation;
    double quadratic_attenuation;
};

struct R3ParticleSpring
{
    R3Particle *particles[2];
    double rest_length;
    double ks;
    double kd;
};



// Scene graph definition

class R3Node;

struct R3Scene
{
    public:
        // Constructor/destructor
        R3Scene(void);
        ~R3Scene();

        // Access functions
        R3Node *Root(void) const;
        int NLights(void) const;
        R3Light *Light(int k) const;
        R3Camera& Camera(void);

        // Particle stuff
        /*
        int NParticleSources(void) const;
        R3ParticleSource *ParticleSource(int k) const;
        */
        int NParticleSinks(void) const;
        R3ParticleSink *ParticleSink(int k) const;
        int NParticles(void) const;
        R3Particle *Particle(int k) const;

        // Bbox
        void UpdateBboxes(void);

        // Drawing
        void Draw(void);

        // I/O
        int Read(const string &name, R3Node *root = NULL);

        // Clear
        void ClearLights(void);

        // Collision
        void Collide(void);

    public:
        R3Node *root;
        vector<R3Particle *> particles;
        vector<R3ParticleSource *> particle_sources;
        vector<R3ParticleSink *> particle_sinks;
        vector<R3ParticleSpring *> particle_springs;
        vector<R3Light *> lights;
        R3Vector gravity;
        R3Camera camera;
        R2Pixel background;
        R2Pixel ambient;
        double time;

        struct
        {
            R3Mesh *mesh;
            R3Material *material;
        } skybox;
};



// Inline functions 

inline R3Node *R3Scene::Root(void) const
{
    // Return root node
    return root;
}



inline int R3Scene::NLights(void) const
{
    // Return number of lights
    return lights.size();
}



inline R3Light *R3Scene::Light(int k) const
{
    // Return kth light
    return lights[k];
}



inline R3Camera& R3Scene::Camera(void) 
{
    // Return camera
    return camera;
}


/*
inline int R3Scene::NParticleSources(void) const
{
    // Return number of particle sources
    return particle_sources.size();
}

inline int R3Scene::AddParticleSource(R3ParticleSource *source){
    // Return kth particle source
    particle_sources.push_back(source);
    return 0;
}


inline int R3Scene::RemoveParticleSource(R3ParticleSource *source)
{
    int i = 0;
    for (i = 0; i < NParticleSources(); i++) {
        if (particle_sources[i] == source)
            break;
    }
    if (i == NParticleSources())
        return -1;
    particle_sources[i] = particle_sources[NParticleSources() - 1];
    particle_sources.pop_back();
    return 0;
}

inline R3ParticleSource *R3Scene::ParticleSource(int k) const
{
    // Return kth particle source
    return particle_sources[k];
}
*/


inline int R3Scene::NParticleSinks(void) const
{
    // Return number of particle sinks
    return particle_sinks.size();
}



inline R3ParticleSink *R3Scene::ParticleSink(int k) const
{
    // Return kth particle sink
    return particle_sinks[k];
}



inline int R3Scene::NParticles(void) const
{
    // Return number of particles
    return particles.size();
}



inline R3Particle *R3Scene::Particle(int k) const
{
    // Return kth particle 
    return particles[k];
}



#endif
