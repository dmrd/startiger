#include "R3Node.h"
#include "Globals.h"
#include "R3Scene.h"
#include "R3Light.h"

void R3Node::Draw(void) const
{
    // skip?
    if (!globals.scene->camera.InFrustum(bbox))
        return;

    // push transform
    transformation.Push();

    // draw this, children
    if (material)
        material->Load();
    if (shape)
        shape->Draw();
    for (list<R3Node *>::const_iterator iter = children.begin();
            iter != children.end(); ++iter) 
        (*iter)->Draw();

    // bbox?
    if (globals.settings.show_bboxes)
    {
        GLboolean lighting = glIsEnabled(GL_LIGHTING);
        glDisable(GL_LIGHTING);

        R3Matrix invworldtransform = getWorldTransform().Inverse();
        invworldtransform.Push();
        bbox.Outline();
        invworldtransform.Pop();

        if (lighting)
            glEnable(GL_LIGHTING);
    }

    // pop transform
    transformation.Pop();
}

void R3Node::Collide(void)
{
    // skip?
    if (!intracollide)
        return;

    // intra-child
    for (list<R3Node *>::iterator iter = children.begin();
            iter != children.end(); ++iter)
        (*iter)->Collide();

    // inter-child
    for (list<R3Node *>::iterator iter1 = children.begin();
            iter1 != children.end(); ++iter1)
    {
        list<R3Node *>::iterator iter2 = iter1;
        while (++iter2 != children.end())
            (*iter1)->Collide(*iter2);
    }
}

void R3Node::Collide(R3Node *other)
{
    // skip?
    if (!bbox.Intersects(other->bbox))
        return;

    // notify (TODO: do narrowphase here?)
    if (object)
        object->Collide(other->object);
    if (other->object)
        other->object->Collide(object);

    // our children vs. other
    for (list<R3Node *>::iterator iter = children.begin();
            iter != children.end(); ++iter)
        (*iter)->Collide(other);

    // other children vs. this
    for (list<R3Node *>::iterator iter = other->children.begin();
            iter != other->children.end(); ++iter)
        (*iter)->Collide(this);

    // our children vs. other children
    for (list<R3Node *>::iterator iter1 = children.begin();
            iter1 != children.end(); ++iter1)
        for (list<R3Node *>::iterator iter2 = other->children.begin();
                iter2 != other->children.end(); ++iter2)
            (*iter1)->Collide(*iter2);
}



// destroy without removing self from parent
void R3Node::_Destroy(void)
{
    for (list<R3Node *>::iterator iter = children.begin();
            iter != children.end(); ++iter) 
        (*iter)->_Destroy();

    if (source != NULL) {
        //globals.scene->RemoveParticleSource(source);
        delete source;
    }

    delete this;
}

void R3Node::AttachSource(R3ParticleSource *sourceToAdd) {
    if (source != NULL)
        return;

    source = sourceToAdd;
    source->node = this;
    //globals.scene->AddParticleSource(sourceToAdd);
}

void R3Node::AttachLight(R3Light *light_)
{
    light = light_;
    light->parent = this;
}



