#ifndef __R3SHAPE_H__
#define __R3SHAPE_H__

#include "RefCounted.h"

/*
 * base class for all shape types
 */

struct R3Box;

struct R3Shape : public RefCounted
{
    enum Type
    {
        NONE,
        BOX,
        SPHERE,
        CYLINDER,
        CONE,
        MESH,
        SEGMENT,
        CIRCLE,
        NUM_TYPES
    };

    virtual ~R3Shape() { } 

    virtual void Draw(void) const { }
    virtual const R3Box BBox(void) const = 0;
    virtual Type GetType(void) { return NONE; }
};

#endif
