#ifndef __R3NODE_H__
#define __R3NODE_H__

#include "R3/R3.h"
#include "Globals.h"
#include "R3Material.h"
#include "R3Scene.h"

#include <algorithm>
#include <list>

struct R3Node
{
    struct R3Node *parent;
    list<R3Node *> children;
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
        if (shape)
            REFCOUNT_UP(shape);
    }

    ~R3Node()
    {
        if (shape)
            REFCOUNT_DOWN(shape);
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

        for (list<R3Node *>::iterator iter = children.begin();
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

    void Destroy(void)
    {
        if (parent)
            parent->RemoveChild(this);
        _Destroy();
    }

    protected:

    // destroy without removing self from parent
    void _Destroy(void)
    {
        for (list<R3Node *>::iterator iter = children.begin();
                iter != children.end(); ++iter) 
            (*iter)->_Destroy();
        delete this;
    }

    void RemoveChild(R3Node *node)
    {
        list<R3Node *>::iterator found = find(children.begin(), children.end(), node);
        if (found != children.end())
            children.erase(found);
    }
};

#endif
