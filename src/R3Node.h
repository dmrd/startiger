#ifndef __R3NODE_H__
#define __R3NODE_H__

#include "R3/R3.h"
#include "Globals.h"
#include "R3Material.h"
#include "R3Scene.h"

struct R3Node
{
    struct R3Node *parent;
    vector<R3Node *> children;
    R3Shape *shape;
    R3Matrix transformation;
    R3Material *material;
    R3Box bbox; // stored in world space!

    R3Node(const R3Matrix &transformation_ = R3identity_matrix) :
        parent(NULL),
        shape(NULL),
        transformation(transformation_),
        material(NULL),
        bbox(R3null_box)
    {
    }

    R3Node(R3Shape *shape_, R3Material *material_,
            const R3Matrix &transformation_ = R3identity_matrix) :
        parent(NULL),
        shape(shape_),
        material(material_),
        transformation(transformation_)
    {
    }

    void UpdateBbox(const R3Matrix &parentworldtransform)
    {
        R3Matrix worldtransform = parentworldtransform * transformation;

        if (shape)
        {
            bbox = shape->BBox();
            bbox.Transform(worldtransform);
        }
        else
            bbox = R3null_box;

        for (vector<R3Node *>::iterator iter = children.begin();
                iter != children.end(); ++iter) 
        {
            (*iter)->UpdateBbox(worldtransform);
            bbox.Union((*iter)->bbox);
        }
    }

    void AddChild(R3Node *node)
    {
        children.push_back(node);
        node->parent = this;

        node->UpdateBbox(getWorldTransform());
        bbox.Union(node->bbox);
    }

    void Draw(void) const;

    R3Matrix getWorldTransform(void) const
    {
        if (parent)
            return parent->getWorldTransform() * transformation;
        return transformation;
    }
};

#endif
