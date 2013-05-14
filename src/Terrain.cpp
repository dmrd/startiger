#include "Globals.h"
#include "Terrain.h"
#include "R3Node.h"
#include "R3Scene.h"
#include "GameObject.h"
#include "Util.h"

// Always have 9
Terrain::Terrain()
{
}

Terrain::Terrain(const Params &params_) :
    params(params_)
{
  printf("terrain\n");
}

R3Mesh* Terrain::Patch(R3Point center, R2Point size, R2Point dps) { // dps: dots per side
  R3Mesh *patch = new R3Mesh();
  for (unsigned int r = 0; r < dps.Y(); r++) {
    for (unsigned int c = 0; c < dps.X(); c++) {
      double dx = (double)c / dps.X() * size.X() - size.X() / 2;
      double dy = (r + c) % 2 * Util::UnitRandom(); // default

      if (params.heightMap) {
        int x = ((double)c / dps.X()) * params.heightMap->Width();
        int y = ((double)r / dps.Y()) * params.heightMap->Height();
        dy = 25 * params.heightMap->Pixel(x,y).Luminance();
      }

      double dz = (double)r / dps.Y() * size.Y() - size.Y() / 2;
      R3Point position(center.X() + dx, center.Y() + dy, center.Z() + dz);

      double tx = position.X() / 80;
      double ty = position.Z() / 80;

      //R2Point texcoord = R2Point(tx - ((int) tx), ty - ((int) ty));
      //if (texcoord.X() < 0)
          //texcoord.SetX(1 + texcoord.X());
      //if (texcoord.Y() < 0)
          //texcoord.SetY(1 + texcoord.Y());

      patch->CreateVertex(position, R3Vector(0,1,0), R2Point(tx, ty)); // id's go up along x axis, then along y
    }
  }
  
  for (unsigned int r = 0; r < dps.Y() - 1; r++) {
    for (unsigned int c = 0; c < dps.X() - 1; c++) {
      unsigned int cols = (unsigned int)dps.X();
      unsigned int k = c + r * cols;
      vector<R3MeshVertex*> vertices;
      vertices.push_back(patch->Vertex(k));
      vertices.push_back(patch->Vertex(k+1));
      vertices.push_back(patch->Vertex(k+cols));
      patch->CreateFace(vertices);

      vector<R3MeshVertex*> vertices2;
      vertices2.push_back(patch->Vertex(k+1));
      vertices2.push_back(patch->Vertex(k+1+cols));
      vertices2.push_back(patch->Vertex(k+cols));
      patch->CreateFace(vertices2);
    }
  }

  patch->Update();

  return patch;
}

void Terrain::Create(void)
{
    node = new R3Node();
    node->transformation = R3identity_matrix;

    R3Material::Params matParams;
    matParams.textureName = "mars.jpg";
    matParams.kd = R3Rgb(1, 1, 1, 1);
    //matParams.vertShaderName = "toon.vert";
    //matParams.fragShaderName = "toon.frag";
    R3Material *mat = new R3Material(matParams);

    node->AddChild(new R3Node(Patch(R3Point(0,-3, 0),
                    R2Point(300,300), R2Point(200,200)),
                mat, R3identity_matrix));
    //node->AddChild(new R3Node(Patch(R3Point(10,-3, 0), R2Point(10,10), R2Point(60,60)), NULL, R3identity_matrix));
    globals.scene->root->AddChild(node);
}

void Terrain::Update(double dt)
{
    
}

void Terrain::Destroy(void)
{
    
}
