#include "R3Node.h"

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
    for (vector<R3Node *>::const_iterator iter = children.begin();
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
