// Include file for the particle system



// Particle system integration types

enum {
  EULER_INTEGRATION,
  MIDPOINT_INTEGRATION,
  ADAPTIVE_STEP_SIZE_INTEGRATION,
  RK4_INTEGRATION
};



// Particle system functions

R3Vector GetForce(R3Point position, R3Vector velocity, R3Scene *scene, R3Particle *particle, double time, bool *remove);
void UpdateParticles(R3Scene *scene, double current_time, double delta_time, int integration_type);
void GenerateParticles(R3Scene *scene, double current_time, double delta_time);
void GenerateParticlesForNode(R3Scene *scene, R3Node *node, R3Matrix transformation, double current_time, double delta_time);
void RenderParticles(R3Scene *scene, double current_time, double delta_time);

// Intersection Code
bool RemoveSinkIntersections(R3Scene *scene, R3Segment *segment);
int FindSink(R3Scene *scene, R3Camera *camera, int x, int y, int width, int height);
int FindSource(R3Scene *scene, R3Camera *camera, int x, int y, int width, int height);
double IntersectWithObjects(R3Shape *shape, R3Ray *ray);
R3Node *IntersectWithScene(R3Scene *scene, R3Segment segment, R3Node *node, R3Point *point, R3Vector *normal, double *t);
R3Node *IntersectWithScene(R3Scene *scene, R3Ray *ray, R3Node *node, R3Point *point, R3Vector *normal, double *t);
bool SphereIntersect(R3Ray *ray, R3Sphere *sphere, R3Point *point, R3Vector *normal, double *t);
bool CylinderIntersect(R3Ray *ray, R3Cylinder *cylinder, R3Point *point, R3Vector *normal, double *t);
bool ConeIntersect(R3Ray *ray, R3Cone *cone, R3Point *point, R3Vector *normal, double *t);
bool OnFace(R3Point *P, R3Box *box);
bool BoxIntersect(R3Ray *ray, R3Box *box, R3Point *point, R3Vector *normal, double *t);
bool TriangleMeshIntersect(R3Ray *ray, R3Mesh *mesh, R3Point *point, R3Vector *normal, double *t);

