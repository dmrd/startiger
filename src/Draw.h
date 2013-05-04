#ifndef __DRAW_H__
#define __DRAW_H__
#include "R3Scene.h"

void LoadCamera(R3Camera *camera);
void LoadLights(R3Scene *scene);
void LoadMaterial(R3Material *material);
void LoadMatrix(R3Matrix *matrix);
void DrawNode(R3Scene *scene, R3Node *node);
void DrawShape(R3Shape *shape);
void DrawScene(R3Scene *scene);

#endif
