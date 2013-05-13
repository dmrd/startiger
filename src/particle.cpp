// Source file for the particle system



// Include files

#include "R2/R2.h"
#include "R3/R3.h"
#include "R3Material.h"
#include "R3Scene.h"
#include "particle.h"
#include <cmath>
using namespace std;
#ifdef _WIN32
#   include <windows.h>
#else
#   include <sys/time.h>
#endif

// Current trails and dynamic lifetime fading are on
#define TRAILS_ON
//#define FADE


////////////////////////////////////////////////////////////
// Random Number Generator
////////////////////////////////////////////////////////////

double RandomNumber(void)
{
#ifdef _WIN32
  // Seed random number generator
  static int first = 1;
  if (first) {
    srand(GetTickCount());
    first = 0;
  }

  // Return random number
  int r1 = rand();
  double r2 = ((double) rand()) / ((double) RAND_MAX);
  return (r1 + r2) / ((double) RAND_MAX);
#else 
  // Seed random number generator
  static int first = 1;
  if (first) {
    struct timeval timevalue;
    gettimeofday(&timevalue, 0);
    srand48(timevalue.tv_usec);
    first = 0;
  }

  // Return random number
  return drand48();
#endif
}

// Calculate the foce on the particle
R3Vector GetForce(R3Point position, R3Vector velocity, R3Scene *scene, R3Particle *particle, double time, bool *remove) {
    if (*remove)
        return R3Vector();

    // Tells update particle if this particle should be removed.
    *remove = false;

    // gravity
    R3Vector force = scene->gravity * particle->mass;

    // drag
    force -= particle->drag * velocity;

    // spring force
    for (int i = 0; i < particle->springs.size(); i++) {
        R3ParticleSpring *spring = particle->springs[i];
        R3Particle *other = spring->particles[0];
        if (other == particle)
            other = spring->particles[1];

        R3Vector direction = (other->position - position);
        double length = direction.Length();
        double magnitude = (length - spring->rest_length) * spring->ks;
        direction /= length;
        magnitude += spring->kd * (other->velocity - velocity).Dot(direction);
        force += magnitude * direction;
    }




    // Attempt at boids, not completed
    if (particle->boid != -1) {
        R3Point averageLocation;
        R3Vector averageVelocity;
        int count = 0;
        for (int i = 0; i < scene->NParticles(); i++) {
            R3Particle *other = scene->particles[i];
            if (other == particle || other->boid == -1)
                continue;

            R3Vector direction = (averageLocation - position);
            double length = direction.Length();
            force +=  -particle->mass * RandomNumber() * direction / 50 / length / length / length;

            count++;
            averageLocation += other->position;
            averageVelocity += other->velocity;
        }

        if (count > 0) {
            averageLocation /= count;
            averageVelocity /= count;

            force += (averageLocation - position) * particle->mass * RandomNumber() / 50;
            force += (averageVelocity - velocity) * particle->mass * RandomNumber() / 50;
        }
    }

    // Calculate force from sinks
    for (int i = 0; i < scene->NParticleSinks(); i++) {
        R3ParticleSink *sink = scene->ParticleSink(i);
        
        switch(sink->shape->GetType()) {
            case R3Shape::SPHERE:
                {
                    R3Sphere *sphere = (R3Sphere*) sink->shape;
                    R3Vector distance = sphere->Center() - position;
                    double length = distance.Length() - sphere->Radius();



                    distance.Normalize();
                    force += sink->intensity / (sink->constant_attenuation + sink->linear_attenuation * length + sink->quadratic_attenuation * length * length) * distance;
                }
                break;
            default:
                break;
        }
    }

    return force;
}

////////////////////////////////////////////////////////////
// Generating Particles
////////////////////////////////////////////////////////////

void GenerateParticles(R3Scene *scene, double current_time, double delta_time)
{
    GenerateParticlesForNode(scene, scene->root, R3identity_matrix, current_time, delta_time);
}

void GenerateParticlesForNode(R3Scene *scene, R3Node *node, R3Matrix transformation, double current_time, double delta_time) {
    R3ParticleSource *source = node->source;
    transformation.Transform(node->transformation);

    if (source != NULL) {
        int numParticles = (int)((current_time)*source->rate) - (int)((current_time - delta_time)*source->rate);

        for (int j = 0; j < numParticles; j++) {
            R3Particle *particle = new R3Particle();

            switch(source->shape->GetType()) {
                case R3Shape::SPHERE:
                    {
                        // Use rejection sampling to pick positions
                        double x1, x2, x1squared, x2squared;
                        do {
                            x1 = RandomNumber()*2-1;
                            x1squared = x1 * x1;
                            x2 = RandomNumber()*2-1;
                            x2squared = x2 * x2;
                        } while (x1squared + x2squared >= 1);

                        double squareRoot = sqrt(1 - x1squared - x2squared);
                        R3Point position(2*x1 * squareRoot, 2*x2 * squareRoot, 1 - 2*(x1squared + x2squared));
                        R3Sphere *sphere = (R3Sphere*) source->shape;
                        position *= sphere->Radius();
                        position += sphere->Center();

                        particle->position = position;  

                        // Find vectors perpindicular to the normal to allow
                        // correct velocity generation
                        R3Vector normal(position - sphere->Center());
                        R3Vector axis1 = R3posz_vector;

                        // Attempt crossing with the z vector, unless they're parallel
                        axis1.Cross(normal);
                        if (axis1.Length() < .0001) {
                            axis1 = R3posy_vector;
                            axis1.Cross(normal);
                        }
                        axis1.Normalize();
                        R3Vector axis2 = axis1;
                        axis2.Cross(normal);
                        axis2.Normalize();

                        double angle1, angle2;
                        angle1 = RandomNumber()*6.28318;
                        angle2 = RandomNumber()*sin(source->angle_cutoff);

                        R3Vector velocity = axis1*cos(angle1) + axis2*sin(angle1);
                        R3Vector perp = velocity;
                        perp.Cross(normal);

                        // Give the velocity the correct direction
                        velocity.Rotate(perp, acos(angle2));

                        particle->velocity = source->velocity * velocity;
                        particle->position.Transform(transformation);
                    }
                    break;

                case R3Shape::CIRCLE:
                    {
                        double r, theta;
                        R3Circle *circle = (R3Circle*) source->shape;

                        // Find vectors perpindicular to the normal to allow
                        // correct velocity generation
                        R3Vector normal = circle->Normal();
                        normal.Transform(transformation);
                        R3Vector axis1 = R3posz_vector;
                        axis1.Cross(normal);
                        // Attempt crossing with the z vector, unless they're parallel
                        if (axis1.Length() < .0001) {
                            axis1 = R3posy_vector;
                            axis1.Cross(normal);
                        }
                        axis1.Normalize();
                        R3Vector axis2 = axis1;
                        axis2.Cross(normal);
                        axis2.Normalize();

                        // Uniformly generate particles within the circle
                        r = sqrt(RandomNumber())*circle->Radius();
                        theta = RandomNumber()*6.28318;
                        R3Point position = circle->Center();
                        position += r*cos(theta)*axis1 + r*sin(theta)*axis2;

                        position.Transform(transformation);
                        particle->position = position;  

                        double angle1, angle2;
                        angle1 = RandomNumber()*6.28318;
                        angle2 = RandomNumber()*sin(source->angle_cutoff);

                        R3Vector velocity = axis1*cos(angle1) + axis2*sin(angle1);
                        R3Vector perp = velocity;
                        perp.Cross(normal);

                        velocity.Rotate(perp, acos(angle2));

                        // Give the velocity the correct direction
                        particle->velocity = source->velocity * velocity;

                    }
                    break;



                default:
                    {
                        delete particle;
                    }
                    continue;
            }

            // Set all of the trail points to the current location
            for (int k = 0; k < TRAIL_LENGTH; k++)
                particle->trail[k] = particle->position;

            particle->last_trail = 0;

            //set properties
            particle->mass = source->mass;
            particle->fixed = source->fixed;
            particle->drag = source->drag;
            particle->elasticity = source->elasticity;
            particle->lifetime = source->lifetime;
            particle->startLifetime = source->lifetime;
            particle->material = source->material;
            particle->materials = source->materials;
            particle->numMaterials = source->numMaterials;
            particle->size = source->size;


            if (particle->material == NULL)
                particle->material = particle->materials[0];

            scene->particles.push_back(particle);

        }
    }

    for (list<R3Node *>::const_iterator iter = node->children.begin();
            iter != node->children.end(); ++iter) 
        GenerateParticlesForNode(scene, (*iter), transformation, current_time, delta_time);

}



////////////////////////////////////////////////////////////
// Updating Particles
////////////////////////////////////////////////////////////

void UpdateParticles(R3Scene *scene, double current_time, double delta_time, int integration_type)
{
  // Update position for every particle

    for (int i = 0; i < scene->NParticles(); i++) {
        R3Particle *particle = scene->Particle(i);

        if (particle->fixed)
            continue;

        bool remove = false;

        if (particle->lifetime > 0) {
            if (particle->lifetime < delta_time)
                remove = true;
            else
                particle->lifetime -= delta_time;
        }

        const R3Point position = particle->position;
        const R3Vector velocity = particle->velocity;
        
        //Euler integration
        R3Point midPosition = position + delta_time/2.0 * velocity;
        R3Vector midVelocity = velocity + delta_time/2.0 * GetForce(position, velocity, scene, particle, current_time, &remove)/particle->mass;
        particle->position = position + delta_time * midVelocity;
        particle->velocity = velocity + delta_time * GetForce(midPosition, midVelocity, scene, particle, current_time, &remove)/particle->mass;

        // Get segment between current and next position to test for collisions
        R3Segment segment(position, particle->position);
        R3Point point;
        R3Vector normal;
        double t;

        // If collided, move out of surface
        /*
        if (NULL != IntersectWithScene(scene, segment, scene->root, &point, &normal, &t)){
            particle->velocity += -normal * (1 + particle->elasticity) * normal.Dot(particle->velocity);
            particle->position = point + normal/1000.0;
        }
        */

        // Update trails
        if (particle->last_trail < 0 || particle->last_trail >= TRAIL_LENGTH) {
            particle->last_trail = 0;
        }


        if (particle->numMaterials > 1) {
            int index = (int) (particle->numMaterials * (1-particle->lifetime/particle->startLifetime));
            particle->material = particle->materials[index];
        }
        

        // Update positions in the trails (trail is in a queue stored in an array)
        particle->trail[particle->last_trail] = particle->position;
        int diff = (int)(current_time*TRAIL_LENGTH) - (int)((current_time - delta_time)*TRAIL_LENGTH);
        while (diff > 0) {
            particle->last_trail++;
            particle->last_trail %= TRAIL_LENGTH;
            particle->trail[particle->last_trail] = particle->position;
            diff--;
        }

        // If we were told to remove the particle, or we've hit a sink, remove it
        
        if (remove){// || RemoveSinkIntersections(scene, &segment)) {
            delete scene->particles[i];
            scene->particles[i] = scene->particles.back();
            scene->particles.pop_back();
            i--;
        }

    }  
}


////////////////////////////////////////////////////////////
// Rendering Particles
////////////////////////////////////////////////////////////

void RenderParticles(R3Scene *scene, double current_time, double delta_time)
{
/* TEMPORARILY COMMENTED OUT
 * */
  // Draw every particle

  // REPLACE CODE HERE
  glPointSize(5);
  glDisable(GL_LIGHTING);

  // Allow alphas


  for (int i = 0; i < scene->NParticles(); i++) {
      R3Particle *particle = scene->Particle(i);

      
      // Dynamically chose alpha based on lifetime
      double alpha = 1;
      if (particle->startLifetime != 0)
          alpha = particle->lifetime/particle->startLifetime;
      const R3Point& position = particle->position;

      // If there is a texture, draw a square facing the camera and
      // add the texture to it.
//      if (particle->material->texture != NULL) {
          //glColor4d(particle->material->params.kd[0], particle->material->params.kd[1], particle->material->params.kd[2], alpha);

          //TODO: fix this to use R3Material::Load() with GL_LIGHTING
          //      disabled
          //glBindTexture(GL_TEXTURE_2D, particle->material->textureName);
          particle->material->Load();
          particle->material->SetColor(alpha);


          glBegin(GL_QUADS);

          glTexCoord2d(0.0, 0.0);

          R3Vector right = scene->camera.right * particle->size;
          R3Vector up = scene->camera.up * particle->size;
          glVertex3d(position[0] - right[0] - up[0], position[1] - right[1] - up[1], position[2] - right[2] - up[2]);
          glTexCoord2d(0.0, 1.0);
          glVertex3d(position[0] + right[0] - up[0], position[1] + right[1] - up[1], position[2] + right[2] - up[2]);
          glTexCoord2d(1.0, 1.0);
          glVertex3d(position[0] + right[0] + up[0], position[1] + right[1] + up[1], position[2] + right[2] + up[2]);
          glTexCoord2d(1.0, 0.0); 
          glVertex3d(position[0] - right[0] + up[0], position[1] - right[1] + up[1], position[2] - right[2] + up[2]);

          glEnd();

/*      } else {

#ifdef FADE
          glColor4d(particle->material->kd[0], particle->material->kd[1], particle->material->kd[2], alpha);
#else
          glColor4d(particle->material->kd[0], particle->material->kd[1], particle->material->kd[2], 1);
#endif
          glDepthMask(GL_TRUE);
          glBegin(GL_POINTS);
          glVertex3d(position[0], position[1], position[2]);
          glEnd();
      }*/
  }   


#ifdef TRAILS_ON
  // Draw the trails
/*  for (int i = 0; i < scene->NParticles(); i++) {
      R3Particle *particle = scene->Particle(i);
      glBegin(GL_LINE_STRIP);
      for (int j = 0; j < TRAIL_LENGTH; j++) {
          int index = (j + particle->last_trail + 1) % TRAIL_LENGTH;
          double alpha = 1.0 - ((particle->last_trail + TRAIL_LENGTH - index) % TRAIL_LENGTH) / (double)TRAIL_LENGTH;
          glColor3d(particle->material->kd[0]*alpha, particle->material->kd[1]*alpha, particle->material->kd[2]*alpha);
          const R3Point& position = particle->trail[index];
          glVertex3d(position[0], position[1], position[2]);
      }
      glEnd();
  }
  */
#endif

  glEnable(GL_LIGHTING);
  /**/
}



////////////////////////////////////////////////////////////
// Intersection Code
////////////////////////////////////////////////////////////

// Checks intersections with sinks
bool RemoveSinkIntersections(R3Scene *scene, R3Segment *segment)
{
    R3Ray ray = segment->Ray();//R3Ray::R3Ray(camera->eye, vector);
    double t = -1;
    int best = -1;

    for (int i = 0; i < scene->NParticleSinks(); i++) {
        R3ParticleSink *sink = scene->ParticleSink(i);
        double ret = IntersectWithObjects(sink->shape, &ray);
        if (ret != -1 && (t==-1 || ret < t)) {
            t = ret;
        }
    }

    if (t!=-1 && t < segment->Length()/ray.Vector().Length()) {
        return true;
    }

    return false;
}

// For dynamic control, if there is a sink where you clicked, return it
int FindSink(R3Scene *scene, R3Camera *camera, int x, int y, int width, int height)
{
    double angleX = camera->xfov*(x-width/2)/(double)width;
    double angleY = camera->yfov*(y-height/2)/(double)height;
    R3Vector vector =  2*tan(angleX)*camera->right + 2*tan(angleY)*camera->up + camera->towards;
    vector.Normalize();
    R3Ray ray = R3Ray(camera->eye, vector);
    double t = -1;
    int best = -1;

    for (int i = 0; i < scene->NParticleSinks(); i++) {
        R3ParticleSink *sink = scene->ParticleSink(i);
        double ret = IntersectWithObjects(sink->shape, &ray);
        if (ret != -1 && (t==-1 || ret < t)) {
            t = ret;
            best = i;
        }
    }

    return best;
}

// For dynamic control, if there is a source where you clicked, return it
int FindSource(R3Scene *scene, R3Camera *camera, int x, int y, int width, int height)
{
    double angleX = camera->xfov*(x-width/2)/(double)width;
    double angleY = camera->yfov*(y-height/2)/(double)height;
    R3Vector vector =  2*tan(angleX)*camera->right + 2*tan(angleY)*camera->up + camera->towards;
    vector.Normalize();
    R3Ray ray = R3Ray(camera->eye, vector);
    double t = -1;
    int best = -1;

    /*
    for (int i = 0; i < scene->NParticleSources(); i++) {
        R3ParticleSource *source = scene->ParticleSource(i);
        double ret = IntersectWithObjects(source->shape, &ray);
        if (ret != -1 && (t==-1 || ret < t)) {
            t = ret;
            best = i;
        }
    }
    */

    return best;
}

// Most of this is copied from raytrace.cpp 
double IntersectWithObjects(R3Shape *shape, R3Ray *ray)
{

    double t = -1;
    R3Point point; 
    R3Vector normal;

    // Check what shape we have and intersect it
    switch(shape->GetType()) {
        case R3Shape::SPHERE:
            {
                SphereIntersect(ray, (R3Sphere *)shape, &point, &normal, &t);
            }
            break;
        case R3Shape::BOX:
            {
                BoxIntersect(ray, (R3Box *)shape, &point, &normal, &t);
            }
            break;
        case R3Shape::MESH:
            {
                R3Point bboxPoint = R3Point(); 
                R3Vector bboxNormal = R3Vector();
                double bboxT = -1;
                if (BoxIntersect(ray, &((R3Mesh *)shape)->bbox, &bboxPoint, &bboxNormal, &bboxT)) {
                    TriangleMeshIntersect(ray, (R3Mesh *)shape, &point, &normal, &t);
                }
            }
            break;
        case R3Shape::CYLINDER:
            {
                CylinderIntersect(ray, (R3Cylinder *)shape, &point, &normal, &t);
            }
            break;
        case R3Shape::CONE:
            {
                ConeIntersect(ray, (R3Cone *)shape, &point, &normal, &t);
            }
            break;
        default:
            break;
    }


    return t;
}

R3Node *IntersectWithScene(R3Scene *scene, R3Segment segment, R3Node *node, R3Point *point, R3Vector *normal, double *t)
{
    R3Ray realRay = segment.Ray();
    R3Ray *ray = &realRay;

    R3Node *retNode = IntersectWithScene(scene, ray, node, point, normal, t);
    if (*t > (segment.End() - segment.Start()).Length()/ray->Vector().Length()) {
        return NULL;
    }
    return retNode;
}

R3Node *IntersectWithScene(R3Scene *scene, R3Ray *ray, R3Node *node, R3Point *point, R3Vector *normal, double *t)
{

    // If we are at the end of the scene graph, return
    if (node == NULL)
        return NULL;

    double tmpT = -1;
    *t = -1;
    R3Node *tmpNode = node;

    R3Point tmpPoint = R3Point(); 
    R3Vector tmpNormal = R3Vector();
    R3Vector oldTmpNormal = R3Vector();

    R3Ray oldRay = *ray;

    ray->Transform(node->transformation.Inverse());


    // Check what shape we have and intersect it
    if (node->shape != NULL) {
        switch(node->shape->GetType()) {
            case R3Shape::SPHERE:
                {
                    SphereIntersect(ray, (R3Sphere *)node->shape, point, normal, t);
                }
                break;
            case R3Shape::BOX:
                {
                    BoxIntersect(ray, (R3Box *)node->shape, point, normal, t);
                }
                break;
            case R3Shape::MESH:
                {
                    R3Point bboxPoint = R3Point(); 
                    R3Vector bboxNormal = R3Vector();
                    double bboxT = -1;
                    if (BoxIntersect(ray, &((R3Mesh *)node->shape)->bbox, &bboxPoint, &bboxNormal, &bboxT)) {
                        TriangleMeshIntersect(ray, (R3Mesh *)node->shape, point, normal, t);
                    }
                }
                break;
            case R3Shape::CYLINDER:
                {
                    CylinderIntersect(ray, (R3Cylinder *)node->shape, point, normal, t);
                }
                break;
            case R3Shape::CONE:
                {
                    ConeIntersect(ray, (R3Cone *)node->shape, point, normal, t);
                }
                break;
            default:
                break;
        }
    } else {
        *t = -1;
    }


    if (*t != -1)
        *t = (*point - ray->Start()).Dot(ray->Vector());

    // Check intersections with the children
    for (list<R3Node *>::iterator iter = node->children.begin();
            iter != node->children.end(); ++iter)
    {
        R3Node *result = IntersectWithScene(scene, ray, *iter, &tmpPoint, &tmpNormal, &tmpT);
        if(result != NULL){
            tmpPoint.Transform(node->transformation);
            if (tmpT != -1)
                tmpT = (tmpPoint - oldRay.Start()).Dot(oldRay.Vector());
            if (tmpT != -1 && (*t == -1 || tmpT < *t)) {
                *t = tmpT;
                *point = tmpPoint;
                *normal = tmpNormal;
                *point = tmpPoint;
                *normal = tmpNormal;
                tmpNode = result;
            }
        }
    }



    if (*t != -1) { 
        normal->InverseTransform(node->transformation.Transpose());
        normal->Normalize();
    }

    *ray = oldRay;
    if (*t > 0) {
        return node;
    }

    return NULL;
}

/*
 * Does the given ray hit the given sphere. Return the intersection and normal.
 */
bool SphereIntersect(R3Ray *ray, R3Sphere *sphere, R3Point *point, R3Vector *normal, double *t)
{
    R3Vector L = sphere->Center() - ray->Start();
    double tca = L.Dot(ray->Vector());
    //if (tca < 0)
    //    return false;
    double dsquared = L.Dot(L) - tca*tca;
    double rsquared = sphere->Radius()*sphere->Radius();
    if (dsquared > rsquared)
        return false;

    double thc = sqrt(rsquared - dsquared);
    // Take the closer t value, unless you're inside the object.
    if (tca > thc){
        *t = tca - thc;
    }else{
        *t = tca + thc;
    }
    *point = ray->Start() + *t * ray->Vector();

    *normal = *point - sphere->Center();
    normal->Normalize();
    //if (tca < thc)
    //    *normal *= -1;
    return true;
}

/*
 * Does the given ray hit the given cylinder. Return the intersection and normal.
 */
bool CylinderIntersect(R3Ray *ray, R3Cylinder *cylinder, R3Point *point, R3Vector *normal, double *t)
{
    // To determine intersection through the lateral face
    // use code similar to sphere intersection. Just project the ray
    // to be perpendicular to the cylinder's face.
    
    R3Vector L = cylinder->Center() - ray->Start();
    R3Vector axis = cylinder->Axis().Vector();
    axis.Normalize();

    L = L - axis.Dot(L)*axis;


    double factor = ray->Vector().Length();
    R3Vector perpRay = ray->Vector() - ray->Vector().Dot(axis)*axis;
    factor = factor/perpRay.Length();
    perpRay.Normalize();

    double tca = L.Dot(perpRay);
    if (tca >= 0) {
        double dsquared = L.Dot(L) - tca*tca;
        double rsquared = cylinder->Radius()*cylinder->Radius();
        if (dsquared <= rsquared) {

            double thc = sqrt(rsquared - dsquared);
            if (tca > thc)
                *t = (tca - thc)*factor;
            else
                *t = (tca + thc)*factor;
            *point = ray->Start() + *t * ray->Vector();

            if (abs(point->Y()-cylinder->Center().Y()) > cylinder->Height()/2) {
                *t = -1;
            }
            R3Vector hold = (*point - cylinder->Center());
            *normal = hold - axis * hold.Dot(axis);
            normal->Normalize();
        }
    }

    // Now check intersections through the endcaps
    double dot;
    double holdT;
    R3Point P;
    R3Vector tmpNormal;
    dot = ray->Vector().Dot(axis); 
    if (dot < 0) {
        holdT = -(ray->Start().Vector().Dot(axis)-(cylinder->Center()+axis*cylinder->Height()/2.0).Vector().Dot(axis))/(ray->Vector().Dot(axis));
        P = ray->Start() + holdT*ray->Vector();

        tmpNormal = axis;
        if ((P - (cylinder->Center()+axis*cylinder->Height()/2.0)).Length() > cylinder->Radius()) {
            holdT = -1;
        }
    }
    if (dot > 0) {
        holdT = -(ray->Start().Vector().Dot(-axis)-(cylinder->Center()-axis*cylinder->Height()/2.0).Vector().Dot(-axis))/(ray->Vector().Dot(-axis));
        P = ray->Start() + holdT*ray->Vector();
        tmpNormal = -axis;
        R3Vector hold = (P - (cylinder->Center()-axis*cylinder->Height()/2.0));
        if (hold.Dot(hold) > cylinder->Radius()*cylinder->Radius()) {
            holdT = -1;
        }
    }
    // Only update t if we found a close intersection
    if (holdT != -1 && (*t == -1 || *t > holdT)) {
        *t = holdT;
        *point = P;
        *normal = tmpNormal;
    }
    return *t != -1;
}

/*
 * Does the given ray hit the given cone. Return the intersection and normal.
 */
bool ConeIntersect(R3Ray *ray, R3Cone *cone, R3Point *point, R3Vector *normal, double *t)
{
    // Determine intersection through a lateral face by algebraically
    // solving for the parameter t.
    R3Vector axis = cone->Axis().Vector();
    axis.Normalize();

    R3Vector v = ray->Vector();
    R3Point S = ray->Start();
    double c = cone->Radius()/cone->Height();
    double y0 = cone->Height()/2 + cone->Center().Y();
    double csquared = c*c;
    double a = (v.X()*v.X() + v.Z()*v.Z() - v.Y()*v.Y()*csquared);
    double b = (2*v.X()*S.X() + 2*v.Z()*S.Z() - 2*v.Y()*S.Y()*csquared + 2*y0*v.Y()*csquared);
    double d = (S.X()*S.X() + S.Z()*S.Z() - S.Y()*S.Y()*csquared + 2*y0*S.Y()*csquared - y0*y0*csquared);
    double descriminant = b*b - 4*a*d;
    if (descriminant >= 0) {
        *t = (-b-sqrt(descriminant))/2/a;
        if (*t < 0)
            *t = (-b+sqrt(descriminant))/2/a;
        if (*t >= 0) {
            *point = ray->Start() + *t * ray->Vector();

            if (abs(point->Y()-cone->Center().Y()) > cone->Height()/2) {//abs((*point - cone->Center()).Dot(axis)) > cone->Height()/2.0) {
                *t = -1;
            }
            //R3Vector hold = (*point - cone->Center());
            R3Point top = R3Point(0, cone->Height()/2, 0) + cone->Center();
            R3Vector hold = (top - *point);
            R3Vector hold2 = (*point - cone->Center());
            hold2.SetY(0);
            hold2.Cross(cone->Axis().Vector());
            hold2.Cross(-hold);
            //*normal = hold - axis * hold.Dot(axis);
            *normal = hold2;
            //normal->SetY(c*normal->Length());
            normal->Normalize();
            }
    }

    // Check intersection through the endcap
    double dot;
    double holdT = -1;
    R3Point P;
    R3Vector tmpNormal;
    dot = ray->Vector().Dot(axis); 
    if (dot > 0) {
        holdT = -(ray->Start().Vector().Dot(-axis)-(cone->Center()-axis*cone->Height()/2.0).Vector().Dot(-axis))/(ray->Vector().Dot(-axis));
        P = ray->Start() + holdT*ray->Vector();
        tmpNormal = -axis;
        R3Vector hold = (P - (cone->Center()-axis*cone->Height()/2.0));
        if (hold.Dot(hold) > cone->Radius()*cone->Radius()) {
            holdT = -1;
        }
    }

    // Only update t if we found a close intersection
    if (holdT != -1 && (*t == -1 || *t > holdT)) {
        *t = holdT;
        *point = P;
        *normal = tmpNormal;
    }
    return *t != -1;
}

/*
 * Check if give point lies within the box
 */ 
bool OnFace(R3Point *P, R3Box *box) {
    if(P->X() >= box->XMin() && P->X() <= box->XMax()) {
        if(P->Y() >= box->YMin() && P->Y() <= box->YMax()) {
            if (P->Z() >= box->ZMin() && P->Z() <= box->ZMax()) {
                return true;
            }
        }
    }
    return false;
}

/*
 * Check if give ray intersects any of the 6 faces of a box. 
 * Normally we could just check the three faces facing you, but
 * this fails if the box is transparent.
 */ 
bool BoxIntersect(R3Ray *ray, R3Box *box, R3Point *point, R3Vector *normal, double *t)
{

    double holdT;
    *t = -1;
    R3Point P;

    // To get transmission with refraction right, try all 6 sides
    
    // Check +x side
    holdT = -(ray->Start().Vector().Dot(R3negx_vector)+box->XMin())/(ray->Vector().Dot(R3negx_vector));
    P = ray->Start() + holdT*ray->Vector();
    P.SetX(box->XMin());
    if (OnFace(&P, box)) {
        if (*t == -1 || *t > holdT) {
            *t = holdT;
            *point = P;
            *normal = R3negx_vector;
        }
    }

    // Check -x side
    holdT = -(ray->Start().Vector().Dot(R3posx_vector)-box->XMax())/(ray->Vector().Dot(R3posx_vector));
    P = ray->Start() + holdT*ray->Vector();
    P.SetX(box->XMax());
    if (OnFace(&P, box)) {
        if (*t == -1 || *t > holdT) {
            *t = holdT;
            *point = P;
            *normal = R3posx_vector;
        }
    }

    // Check +y side
    holdT = -(ray->Start().Vector().Dot(R3negy_vector)+box->YMin())/(ray->Vector().Dot(R3negy_vector));
    P = ray->Start() + holdT*ray->Vector();
    P.SetY(box->YMin());
    if (OnFace(&P, box)) {
        if (*t == -1 || *t > holdT) {
            *t = holdT;
            *point = P;
            *normal = R3negy_vector;
        }
    }

    // Check -y side
    holdT = -(ray->Start().Vector().Dot(R3posy_vector)-box->YMax())/(ray->Vector().Dot(R3posy_vector));
    P = ray->Start() + holdT*ray->Vector();
    P.SetY(box->YMax());
    if (OnFace(&P, box)) {
        if (*t == -1 || *t > holdT) {
            *t = holdT;
            *point = P;
            *normal = R3posy_vector;
        }
    }


    // Check +z side
    holdT = -(ray->Start().Vector().Dot(R3negz_vector)+box->ZMin())/(ray->Vector().Dot(R3negz_vector));
    P = ray->Start() + holdT*ray->Vector();
    P.SetZ(box->ZMin());
    if (OnFace(&P, box)) {
        if (*t == -1 || *t > holdT) {
            *t = holdT;
            *point = P;
            *normal = R3negz_vector;
        }
    }

    // Check -z side
    holdT = -(ray->Start().Vector().Dot(R3posz_vector)-box->ZMax())/(ray->Vector().Dot(R3posz_vector));
    P = ray->Start() + holdT*ray->Vector();
    P.SetZ(box->ZMax());
    if (OnFace(&P, box)) {
        if (*t == -1 || *t > holdT) {
            *t = holdT;
            *point = P;
            *normal = R3posz_vector;
        }
    }

    return *t != -1;
}

/*
 * Check if give ray intersects any of the faces of a mesh. 
 */ 
bool TriangleMeshIntersect(R3Ray *ray, R3Mesh *mesh, R3Point *point, R3Vector *normal, double *t)
{
    double holdT;
    *t = -1;
    R3Point P;

    // Test all faces
    for(unsigned int i = 0; i < mesh->faces.size(); i++) {
        R3Plane plane = mesh->faces[i]->plane;

        if (plane.Normal().Dot(ray->Vector()) > 0)
            continue;
        holdT = -(ray->Start().Vector().Dot(plane.Normal())+plane.D())/(ray->Vector().Dot(plane.Normal()));
        P = ray->Start() + holdT*ray->Vector();
        bool inside = true;

        // Does this ray intersect this face
        for (unsigned int j = 0; j < mesh->faces[i]->vertices.size(); j++) {
            R3Vector V1 = mesh->faces[i]->vertices[j]->position - P;
            R3Vector V2 = mesh->faces[i]->vertices[(j+1)%mesh->faces[i]->vertices.size()]->position - P;
            V2.Cross(V1);
            if (ray->Vector().Dot(V2) < 0) {
                inside = false;
                break;
            }
        }
        if (inside) {
            if (*t == -1 || *t > holdT) {
                *t = holdT;
                *point = P;
                *normal = plane.Normal();
            }
        }
    }
    return *t != -1;
}

