#ifndef __SCENE_H__
#define __SCENE_H__

// Include file for the R3 scene stuff
#include "R3/R3.h"

#define R3Rgb R2Pixel

// Constant definitions

typedef enum {
    R3_DIRECTIONAL_LIGHT,
    R3_POINT_LIGHT,
    R3_SPOT_LIGHT,
    R3_AREA_LIGHT,
    R3_NUM_LIGHT_TYPES
} R3LightType;



// Scene element definitions

struct R3Material {
    R2Pixel ka;
    R2Pixel kd;
    R2Pixel ks;
    R2Pixel kt;
    R2Pixel emission;
    double shininess;
    double indexofrefraction;
    R2Image *texture;
    int texture_index;
    int id;
};

struct R3Light {
    R3LightType type;
    R3Point position;
    R3Vector direction;
    double radius;
    R2Pixel color;
    double constant_attenuation;
    double linear_attenuation;
    double quadratic_attenuation;
    double angle_attenuation;
    double angle_cutoff;
};

struct R3Camera {
    R3Point eye;
    R3Vector towards;
    R3Vector right;
    R3Vector up;
    double xfov, yfov;
    double neardist, fardist;
};

struct R3Node {
    struct R3Node *parent;
    vector<struct R3Node *> children;
    R3Shape *shape;
    R3Matrix transformation;
    R3Material *material;
    R3Box bbox;
};



// Particle system definitions

struct R3Particle {
    R3Point position;
    R3Point oPos;
    R3Vector velocity;
    R3Vector oV;
    double mass;
    bool fixed;
    double drag;
    double elasticity;
    double lifetime;
    R3Material *material;
    double birth;
    vector<struct R3ParticleSpring *> springs;
    bool dead;
    R3Vector forces;
};

struct R3ParticleSource {
    R3Shape *shape;
    double rate;
    double velocity;
    double angle_cutoff;
    double mass;
    bool fixed;
    double drag;
    double elasticity;
    double lifetime;
    R3Material *material;
    double residual;
};

struct R3ParticleSink {
    R3Shape *shape;
    double intensity;
    double constant_attenuation;
    double linear_attenuation;
    double quadratic_attenuation;
};

struct R3ParticleSpring {
    R3Particle *particles[2];
    double rest_length;
    double ks;
    double kd;
};



// Scene graph definition

struct R3Scene {
    public:
        // Constructor functions
        R3Scene(void);

        // Access functions
        R3Node *Root(void) const;
        int NLights(void) const;
        R3Light *Light(int k) const;
        R3Camera& Camera(void);
        R3Box& BBox(void);

        // Particle stuff
        int NParticleSources(void) const;
        R3ParticleSource *ParticleSource(int k) const;
        int NParticleSinks(void) const;
        R3ParticleSink *ParticleSink(int k) const;
        int NParticles(void) const;
        R3Particle *Particle(int k) const;

        // I/O functions
        int Read(const char *filename, R3Node *root = NULL);

    public:
        R3Node *root;
        vector<R3Particle *> particles;
        vector<R3ParticleSource *> particle_sources;
        vector<R3ParticleSink *> particle_sinks;
        vector<R3ParticleSpring *> particle_springs;
        vector<R3Light *> lights;
        R3Vector gravity;
        R3Camera camera;
        R3Box bbox;
        R2Pixel background;
        R2Pixel ambient;
        double time;
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



inline R3Box& R3Scene::BBox(void) 
{
    // Return bounding box 
    return bbox;
}



inline int R3Scene::NParticleSources(void) const
{
    // Return number of particle sources
    return particle_sources.size();
}



inline R3ParticleSource *R3Scene::ParticleSource(int k) const
{
    // Return kth particle source
    return particle_sources[k];
}



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
