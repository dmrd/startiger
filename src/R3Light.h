#ifndef __R3LIGHT_H__
#define __R3LIGHT_H__

#include "R3/R3.h"
#include "stdio.h"

#include "R3Node.h"
#include "Globals.h"
#include "Player.h"


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

    R3Node *parent;

    R3Light(
        R3LightType type_ = R3_POINT_LIGHT,
        R3Point position_ = R3null_point,
        R3Vector direction_ = R3null_vector,
        double radius_ = 0,
        R2Pixel color_ = R3Rgb(1, 1, 1, 1),
        double constant_attenuation_ = 1,
        double linear_attenuation_ = 0,
        double quadratic_attenuation_ = 0,
        double angle_attenuation_ = 0,
        double angle_cutoff_ = M_PI
        ) :
        type(type_),
        position(position_),
        direction(direction_),
        radius(radius_),
        color(color_),
        constant_attenuation(constant_attenuation_),
        linear_attenuation(linear_attenuation_),
        quadratic_attenuation(quadratic_attenuation_),
        angle_attenuation(angle_attenuation_),
        angle_cutoff(angle_cutoff_),
        parent(NULL)
    {
    }

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
        R3Point transpos = position;    // nothing to do with 'transpose'...
        R3Vector transdir = direction;

        if (parent)
        {
            R3Matrix trans = parent->getWorldTransform();
            transpos.Transform(trans);
            //transdir.Transform(trans);
        }

        switch (type)
        {
            case R3_DIRECTIONAL_LIGHT: 
                // Load direction
                buffer[0] = -(transdir.X());
                buffer[1] = -(transdir.Y());
                buffer[2] = -(transdir.Z());
                buffer[3] = 0.0;
                glLightfv(index, GL_POSITION, buffer);
                break;

            case R3_POINT_LIGHT: 
                // Load transpos
                buffer[0] = transpos.X();
                buffer[1] = transpos.Y();
                buffer[2] = transpos.Z();
                buffer[3] = 1.0;
                glLightfv(index, GL_POSITION, buffer);
                break;

            case R3_SPOT_LIGHT: 
                // Load transpos
                buffer[0] = transpos.X();
                buffer[1] = transpos.Y();
                buffer[2] = transpos.Z();
                buffer[3] = 1.0;
                glLightfv(index, GL_POSITION, buffer);

                // Load transdir
                buffer[0] = transdir.X();
                buffer[1] = transdir.Y();
                buffer[2] = transdir.Z();
                buffer[3] = 1.0;
                glLightfv(index, GL_SPOT_DIRECTION, buffer);
                break;

            case R3_AREA_LIGHT: 
                // Load transpos
                buffer[0] = transpos.X();
                buffer[1] = transpos.Y();
                buffer[2] = transpos.Z();
                buffer[3] = 1.0;
                glLightfv(index, GL_POSITION, buffer);

                // Load transdir
                buffer[0] = transdir.X();
                buffer[1] = transdir.Y();
                buffer[2] = transdir.Z();
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
