#include "Draw.h"
#include "Globals.h"
#include "particle.h"

#ifdef _WIN32
#  include <windows.h>
#else
#  include <sys/time.h>
#endif

static double GetTime(void)
{
#ifdef _WIN32
  // Return number of seconds since start of execution
  static int first = 1;
  static LARGE_INTEGER timefreq;
  static LARGE_INTEGER start_timevalue;

  // Check if this is the first time
  if (first) {
    // Initialize first time
    QueryPerformanceFrequency(&timefreq);
    QueryPerformanceCounter(&start_timevalue);
    first = 0;
    return 0;
  }
  else {
    // Return time since start
    LARGE_INTEGER current_timevalue;
    QueryPerformanceCounter(&current_timevalue);
    return ((double) current_timevalue.QuadPart - 
            (double) start_timevalue.QuadPart) / 
            (double) timefreq.QuadPart;
  }
#else
  // Return number of seconds since start of execution
  static int first = 1;
  static struct timeval start_timevalue;

  // Check if this is the first time
  if (first) {
    // Initialize first time
    gettimeofday(&start_timevalue, NULL);
    first = 0;
    return 0;
  }
  else {
    // Return time since start
    struct timeval current_timevalue;
    gettimeofday(&current_timevalue, NULL);
    int secs = current_timevalue.tv_sec - start_timevalue.tv_sec;
    int usecs = current_timevalue.tv_usec - start_timevalue.tv_usec;
    return (double) (secs + 1.0E-6F * usecs);
  }
#endif
}


void LoadCamera(R3Camera *camera)
{
  // Set projection transformation
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(2*180.0*camera->yfov/M_PI,
          (GLdouble) globals.window.width/(GLdouble) globals.window.height,
          0.01, 10000);

  // Set camera transformation
  R3Vector t = -(camera->towards);
  R3Vector& u = camera->up;
  R3Vector& r = camera->right;
  GLdouble camera_matrix[16] = { r[0], u[0], t[0], 0, r[1], u[1], t[1], 0, r[2], u[2], t[2], 0, 0, 0, 0, 1 };
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMultMatrixd(camera_matrix);
  glTranslated(-(camera->eye[0]), -(camera->eye[1]), -(camera->eye[2]));
}

void LoadLights(R3Scene *scene)
{
  GLfloat buffer[4];

  // Load ambient light
  static GLfloat ambient[4];
  ambient[0] = scene->ambient[0];
  ambient[1] = scene->ambient[1];
  ambient[2] = scene->ambient[2];
  ambient[3] = 1;
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

  // Load scene lights
  for (int i = 0; i < (int) scene->lights.size(); i++) {
    R3Light *light = scene->lights[i];
    int index = GL_LIGHT0 + i;

    // Temporarily disable light
    glDisable(index);

    // Load color
    buffer[0] = light->color[0];
    buffer[1] = light->color[1];
    buffer[2] = light->color[2];
    buffer[3] = 1.0;
    glLightfv(index, GL_DIFFUSE, buffer);
    glLightfv(index, GL_SPECULAR, buffer);

    // Load attenuation with distance
    buffer[0] = light->constant_attenuation;
    buffer[1] = light->linear_attenuation;
    buffer[2] = light->quadratic_attenuation;
    glLightf(index, GL_CONSTANT_ATTENUATION, buffer[0]);
    glLightf(index, GL_LINEAR_ATTENUATION, buffer[1]);
    glLightf(index, GL_QUADRATIC_ATTENUATION, buffer[2]);

    // Load spot light behavior
    buffer[0] = 180.0 * light->angle_cutoff / M_PI;
    buffer[1] = light->angle_attenuation;
    glLightf(index, GL_SPOT_CUTOFF, buffer[0]);
    glLightf(index, GL_SPOT_EXPONENT, buffer[1]);

    // Load positions/directions
    if (light->type == R3_DIRECTIONAL_LIGHT) {
      // Load direction
      buffer[0] = -(light->direction.X());
      buffer[1] = -(light->direction.Y());
      buffer[2] = -(light->direction.Z());
      buffer[3] = 0.0;
      glLightfv(index, GL_POSITION, buffer);
    }
    else if (light->type == R3_POINT_LIGHT) {
      // Load position
      buffer[0] = light->position.X();
      buffer[1] = light->position.Y();
      buffer[2] = light->position.Z();
      buffer[3] = 1.0;
      glLightfv(index, GL_POSITION, buffer);
    }
    else if (light->type == R3_SPOT_LIGHT) {
      // Load position
      buffer[0] = light->position.X();
      buffer[1] = light->position.Y();
      buffer[2] = light->position.Z();
      buffer[3] = 1.0;
      glLightfv(index, GL_POSITION, buffer);

      // Load direction
      buffer[0] = light->direction.X();
      buffer[1] = light->direction.Y();
      buffer[2] = light->direction.Z();
      buffer[3] = 1.0;
      glLightfv(index, GL_SPOT_DIRECTION, buffer);
    }
    else if (light->type == R3_AREA_LIGHT) {
      // Load position
      buffer[0] = light->position.X();
      buffer[1] = light->position.Y();
      buffer[2] = light->position.Z();
      buffer[3] = 1.0;
      glLightfv(index, GL_POSITION, buffer);

      // Load direction
      buffer[0] = light->direction.X();
      buffer[1] = light->direction.Y();
      buffer[2] = light->direction.Z();
      buffer[3] = 1.0;
      glLightfv(index, GL_SPOT_DIRECTION, buffer);
    }
     else {
      fprintf(stderr, "Unrecognized light type: %d\n", light->type);
      return;
    }

    // Enable light
    glEnable(index);
  }
}

void LoadMaterial(R3Material *material) 
{
    GLfloat c[4];

    // Check if same as current
    static R3Material *current_material = NULL;
    if (material == current_material) return;
    current_material = material;

    // Compute "opacity"
    double opacity = 1 - material->kt.Luminance();

    // Load ambient
    c[0] = material->ka[0];
    c[1] = material->ka[1];
    c[2] = material->ka[2];
    c[3] = opacity;
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, c);

    // Load diffuse
    c[0] = material->kd[0];
    c[1] = material->kd[1];
    c[2] = material->kd[2];
    c[3] = opacity;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c);

    // Load specular
    c[0] = material->ks[0];
    c[1] = material->ks[1];
    c[2] = material->ks[2];
    c[3] = opacity;
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);

    // Load emission
    c[0] = material->emission.Red();
    c[1] = material->emission.Green();
    c[2] = material->emission.Blue();
    c[3] = opacity;
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, c);

    // Load shininess
    c[0] = material->shininess;
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, c[0]);

    // Load texture
    if (material->texture) {
        if (material->texture_index <= 0) {
            // Create texture in OpenGL
            GLuint texture_index;
            glGenTextures(1, &texture_index);
            material->texture_index = (int) texture_index;
            glBindTexture(GL_TEXTURE_2D, material->texture_index); 
            R2Image *image = material->texture;
            int npixels = image->NPixels();
            R2Pixel *pixels = image->Pixels();
            GLfloat *buffer = new GLfloat [ 4 * npixels ];
            R2Pixel *pixelsp = pixels;
            GLfloat *bufferp = buffer;
            for (int j = 0; j < npixels; j++) { 
                *(bufferp++) = pixelsp->Red();
                *(bufferp++) = pixelsp->Green();
                *(bufferp++) = pixelsp->Blue();
                *(bufferp++) = pixelsp->Alpha();
                pixelsp++;
            }
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glTexImage2D(GL_TEXTURE_2D, 0, 4, image->Width(), image->Height(), 0, GL_RGBA, GL_FLOAT, buffer);
            delete [] buffer;
        }

        // Select texture
        glBindTexture(GL_TEXTURE_2D, material->texture_index); 
        glEnable(GL_TEXTURE_2D);
    }
    else {
        glDisable(GL_TEXTURE_2D);
    }

    // Enable blending for transparent surfaces
    if (opacity < 1) {
        glDepthMask(false);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
    }
    else {
        glDisable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ZERO);
        glDepthMask(true);
    }
}

void LoadMatrix(R3Matrix *matrix)
{
    // Multiply matrix by top of stack
    // Take transpose of matrix because OpenGL represents vectors with 
    // column-vectors and R3 represents them with row-vectors
    R3Matrix m = matrix->Transpose();
    glMultMatrixd((double *) &m);
}

void DrawNode(R3Scene *scene, R3Node *node)
{
    // Push transformation onto stack
    glPushMatrix();
    LoadMatrix(&node->transformation);

    // Load material
    if (node->material) LoadMaterial(node->material);

    // Draw shape
    if (node->shape) node->shape->Draw();

    // Draw children nodes
    for (int i = 0; i < (int) node->children.size(); i++) 
        DrawNode(scene, node->children[i]);

    // Restore previous transformation
    glPopMatrix();

    // Show bounding box
    /*
     *if (show_bboxes) {
     *  GLboolean lighting = glIsEnabled(GL_LIGHTING);
     *  glDisable(GL_LIGHTING);
     *  node->bbox.Outline();
     *  if (lighting) glEnable(GL_LIGHTING);
     *}
     */
}

void DrawScene(R3Scene *scene) 
{
    // Draw nodes recursively
    DrawNode(scene, scene->root);
}

void DrawParticles(R3Scene *scene)
{
  // Get current time (in seconds) since start of execution
  double current_time = GetTime();
  static double previous_time = 0;


  // program just started up?
  if (previous_time == 0) previous_time = current_time;

  // time passed since starting
  double delta_time = current_time - previous_time;

  //scene->camera = camera;

  // Update particles
  UpdateParticles(scene, current_time, delta_time, 0);

  // Generate new particles
  GenerateParticles(scene, current_time, delta_time);

  // Render particles
  RenderParticles(scene, current_time, delta_time);

  // Remember previous time
  previous_time = current_time;
}


