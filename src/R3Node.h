#ifndef __R3NODE_H__
#define __R3NODE_H__

#include "R3/R3.h"
#include "Globals.h"
#include "R3Material.h"

struct R3Node
{
    struct R3Node *parent;
    vector<R3Node *> children;
    R3Shape *shape;
    R3Matrix transformation;
    R3Material *material;
    R3Box bbox;

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
        bbox(shape_ ? shape_->BBox() : R3null_box),
        material(material_),
        transformation(transformation_)
    {
    }

    void AddChild(R3Node *node)
    {
        bbox.Union(node->bbox);
        children.push_back(node);
        node->parent = this;
    }

    void Draw(void) const
    {
        // push transform
        transformation.Push();

        // draw this, children
        if (material)
            material->Load();
        if (shape)
            shape->Draw();
        for (vector<R3Node *>::const_iterator iter = children.begin();
                iter != children.end(); ++iter) 
            (*iter)->Draw();

        // pop transform
        transformation.Pop();

        // bbox?
        if (globals.settings.show_bboxes)
        {
            GLboolean lighting = glIsEnabled(GL_LIGHTING);
            glDisable(GL_LIGHTING);
            bbox.Outline();
            if (lighting)
                glEnable(GL_LIGHTING);
        }
    }

    R3Matrix getWorldTransform(void) const
    {
        if (parent)
            return parent->getWorldTransform() * transformation;
        return transformation;
    }
};

#endif
