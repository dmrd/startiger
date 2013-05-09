#ifndef __R3LIGHT_H__
#define __R3LIGHT_H__

#include "R3/R3.h"
#include "stdio.h"

/*
 * light types
 */

typedef enum
{
    R3_DIRECTIONAL_LIGHT,
    R3_POINT_LIGHT,
    R3_SPOT_LIGHT,
    R3_AREA_LIGHT,
    R3_NUM_LIGHT_TYPES
} R3LightType;



/*
 * represents light of different types
 */

struct R3Light
{
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

    // load light at index i
    void Load(int i) const
    {
        GLfloat buffer[4];
        int index = GL_LIGHT0 + i;

        // Temporarily disable light
        glDisable(index);

        // Load color
        buffer[0] = color[0];
        buffer[1] = color[1];
        buffer[2] = color[2];
        buffer[3] = 1.0;
        glLightfv(index, GL_DIFFUSE, buffer);
        glLightfv(index, GL_SPECULAR, buffer);

        // Load attenuation with distance
        buffer[0] = constant_attenuation;
        buffer[1] = linear_attenuation;
        buffer[2] = quadratic_attenuation;
        glLightf(index, GL_CONSTANT_ATTENUATION, buffer[0]);
        glLightf(index, GL_LINEAR_ATTENUATION, buffer[1]);
        glLightf(index, GL_QUADRATIC_ATTENUATION, buffer[2]);

        // Load spot light behavior
        buffer[0] = 180.0 * angle_cutoff / M_PI;
        buffer[1] = angle_attenuation;
        glLightf(index, GL_SPOT_CUTOFF, buffer[0]);
        glLightf(index, GL_SPOT_EXPONENT, buffer[1]);

        // Load positions/directions
        switch (type)
        {
            case R3_DIRECTIONAL_LIGHT: 
                // Load direction
                buffer[0] = -(direction.X());
                buffer[1] = -(direction.Y());
                buffer[2] = -(direction.Z());
                buffer[3] = 0.0;
                glLightfv(index, GL_POSITION, buffer);
                break;

            case R3_POINT_LIGHT: 
                // Load position
                buffer[0] = position.X();
                buffer[1] = position.Y();
                buffer[2] = position.Z();
                buffer[3] = 1.0;
                glLightfv(index, GL_POSITION, buffer);
                break;

            case R3_SPOT_LIGHT: 
                // Load position
                buffer[0] = position.X();
                buffer[1] = position.Y();
                buffer[2] = position.Z();
                buffer[3] = 1.0;
                glLightfv(index, GL_POSITION, buffer);

                // Load direction
                buffer[0] = direction.X();
                buffer[1] = direction.Y();
                buffer[2] = direction.Z();
                buffer[3] = 1.0;
                glLightfv(index, GL_SPOT_DIRECTION, buffer);
                break;

            case R3_AREA_LIGHT: 
                // Load position
                buffer[0] = position.X();
                buffer[1] = position.Y();
                buffer[2] = position.Z();
                buffer[3] = 1.0;
                glLightfv(index, GL_POSITION, buffer);

                // Load direction
                buffer[0] = direction.X();
                buffer[1] = direction.Y();
                buffer[2] = direction.Z();
                buffer[3] = 1.0;
                glLightfv(index, GL_SPOT_DIRECTION, buffer);
                break;

            default:
                fprintf(stderr, "Unkown light type!\n");
                break;
        }

        // Enable light
        glEnable(index);
    }
};

#endif
