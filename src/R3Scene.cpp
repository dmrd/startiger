// Source file for the R3 scene graph class 



// Include files 

#include "R3Scene.h"

#include <string>

#include "R3/R3.h"
#include "R3Camera.h"
#include "Dirs.h"


R3Scene::R3Scene(void) :
    bbox(R3null_box),
    background(0,0,0,1),
    ambient(0,0,0,1)
{
    // Setup default camera
    camera.eye = R3zero_point;
    camera.towards = R3negz_vector;
    camera.up = R3posy_vector;
    camera.right = R3posx_vector;
    camera.xfov = 0.0;
    camera.yfov = 0.0;
    camera.neardist = 0.01;
    camera.fardist = 100.0;

    // Create root node
    root = new R3Node();
}



// --- draw ----------------------------------------------------------------------------

void R3Scene::Draw() const
{
    // lights!
    static GLfloat glambient[4];
    glambient[0] = ambient[0];
    glambient[1] = ambient[1];
    glambient[2] = ambient[2];
    glambient[3] = 1;
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, glambient);
    for (int i = 0; i < lights.size(); ++i)
        lights[i]->Load(i);

    // camera!
    camera.Load();
    
    // action!
    root->Draw();
}



// --- I/O -----------------------------------------------------------------------------

static R3Shape *ReadShape(FILE *fp, int command_number, const char *filename)
{
    // Initialize result
    R3Shape *shape = NULL;

    // Read shape type
    char shape_type[1024];
    if (fscanf(fp, "%s", shape_type) != 1)
    {
        fprintf(stderr, "Unable to read shape type at command %d in file %s\n", command_number, filename);
        return NULL;
    }      

    // Read shape args
    if (!strcmp(shape_type, "box"))
    {
        // Read sphere args
        double x1, y1, z1, x2, y2, z2;
        if (fscanf(fp, "%lf%lf%lf%lf%lf%lf", &x1, &y1, &z1, &x2, &y2, &z2) != 6)
        {
            fprintf(stderr, "Unable to read sphere args at command %d in file %s\n", command_number, filename);
            return NULL;
        }

        // Create shape
        shape = new R3Box(x1, y1, z1, x2, y2, z2);
    }
    else if (!strcmp(shape_type, "sphere"))
    {
        // Read sphere args
        double center_x, center_y, center_z, radius;
        if (fscanf(fp, "%lf%lf%lf%lf", &center_x, &center_y, &center_z, &radius) != 4)
        {
            fprintf(stderr, "Unable to read sphere args at command %d in file %s\n", command_number, filename);
            return NULL;
        }

        // Create shape
        shape = new R3Sphere(R3Point(center_x, center_y, center_z), radius);
    }
    else if (!strcmp(shape_type, "cylinder"))
    {
        // Read cylinder args
        double center_x, center_y, center_z, radius, height;
        if (fscanf(fp, "%lf%lf%lf%lf%lf", &center_x, &center_y, &center_z, &radius, &height) != 5)
        {
            fprintf(stderr, "Unable to read cylinder args at command %d in file %s\n", command_number, filename);
            return NULL;
        }

        // Create shape
        shape = new R3Cylinder(R3Point(center_x, center_y, center_z), radius, height);
    }
    else if (!strcmp(shape_type, "cone"))
    {
        // Read cylinder args
        double center_x, center_y, center_z, radius, height;
        if (fscanf(fp, "%lf%lf%lf%lf%lf", &center_x, &center_y, &center_z, &radius, &height) != 5)
        {
            fprintf(stderr, "Unable to read cone args at command %d in file %s\n", command_number, filename);
            return NULL;
        }

        // Create shape
        shape = new R3Cone(R3Point(center_x, center_y, center_z), radius, height);
    }
    else if (!strcmp(shape_type, "mesh"))
    {
        // Read mesh args
        char meshname[1024];
        if (fscanf(fp, "%s", meshname) != 1)
        {
            fprintf(stderr, "Unable to read mesh args at command %d in file %s\n", command_number, filename);
            return NULL;
        }

        // Get mesh filename
        char buffer[2048];
        strcpy(buffer, filename);
        char *bufferp = strrchr(buffer, '/');
        if (bufferp) *(bufferp+1) = '\0';
        else buffer[0] = '\0';
        strcat(buffer, meshname);

        // Create mesh
        R3Mesh *mesh = new R3Mesh();
        if (!mesh)
        {
            fprintf(stderr, "Unable to allocate mesh\n");
            return 0;
        }

        // Read mesh file
        if (!mesh->Read(buffer))
        {
            fprintf(stderr, "Unable to read mesh: %s\n", buffer);
            return 0;
        }

        // Create shape
        shape = mesh;
    }
    else if (!strcmp(shape_type, "line"))
    {
        // Read sphere args
        double x1, y1, z1, x2, y2, z2;
        if (fscanf(fp, "%lf%lf%lf%lf%lf%lf", &x1, &y1, &z1, &x2, &y2, &z2) != 6)
        {
            fprintf(stderr, "Unable to read sphere args at command %d in file %s\n", command_number, filename);
            return NULL;
        }

        // Create shape
        shape = new R3Segment(R3Point(x1, y1, z1), R3Point(x2, y2, z2));
    }
    else if (!strcmp(shape_type, "circle"))
    {
        // Read circle args
        double center_x, center_y, center_z, normal_x, normal_y, normal_z, radius;
        if (fscanf(fp, "%lf%lf%lf%lf%lf%lf%lf", &center_x, &center_y, &center_z, &normal_x, &normal_y, &normal_z, &radius) != 7)
        {
            fprintf(stderr, "Unable to read circle args at command %d in file %s\n", command_number, filename);
            return NULL;
        }

        // Create shape
        shape = new R3Circle(R3Point(center_x, center_y, center_z), radius, R3Vector(normal_x, normal_y, normal_z));
    }
    else
    {
        // Unrecognized shape type
        fprintf(stderr, "Invalid shape type (%s) at command %d in file %s\n", shape_type, command_number, filename);
        return NULL;
    }

    // Return created shape
    return shape;
}

int R3Scene::Read(const string &name, R3Node *node)
{
    string path = DIR_SCENES + name + ".scn";

    // Open file
    FILE *fp;
    if (!(fp = fopen(path.c_str(), "r")))
    {
        fprintf(stderr, "Unable to open file %s", path.c_str());
        return 0;
    }

    // Create array of materials
    vector<R3Material *> materials;

    // Create default material
    R3Material *default_material = new R3Material();
    default_material->ka = R3Rgb(0.2, 0.2, 0.2, 1);
    default_material->kd = R3Rgb(0.5, 0.5, 0.5, 1);
    default_material->ks = R3Rgb(0.5, 0.5, 0.5, 1);
    default_material->kt = R3Rgb(0.0, 0.0, 0.0, 1);
    default_material->emission = R3Rgb(0, 0, 0, 1);
    default_material->shininess = 10;
    default_material->indexofrefraction = 1;
    default_material->texture = NULL;
    default_material->id = 0;

    // Create stack of group information
    const int max_depth = 1024;
    R3Node *group_nodes[max_depth] = { NULL };
    R3Material *group_materials[max_depth] = { NULL };
    group_nodes[0] = (node) ? node : root;
    group_materials[0] = default_material;
    int depth = 0;

    // Read body
    char cmd[128];
    int command_number = 1;
    vector<R3Particle *> particles_for_springs;
    while (fscanf(fp, "%s", cmd) == 1)
    {
        if (cmd[0] == '#')
        {
            // Comment -- read everything until end of line
            do { cmd[0] = fgetc(fp); } while ((cmd[0] >= 0) && (cmd[0] != '\n'));
        }

        // --- particles -----------------------------------------------------------------------

        else if (!strcmp(cmd, "particle"))
        {
            // Read position and velocity
            R3Point position;
            R3Vector velocity;
            double mass, drag, elasticity, lifetime;
            int fixed, m;
            if (fscanf(fp, "%lf%lf%lf%lf%lf%lf%lf%d%lf%lf%lf%d", 
                        &position[0], &position[1], &position[2], 
                        &velocity[0], &velocity[1], &velocity[2], 
                        &mass, &fixed, &drag, &elasticity, &lifetime, &m) != 12)
            {
                fprintf(stderr, "Unable to read particle at command %d in file %s\n", command_number, path.c_str());
                return 0;
            }

            // Get material
            R3Material *material = group_materials[depth];
            if (m >= 0)
            {
                if (m < (int) materials.size())
                {
                    material = materials[m];
                }
                else
                {
                    fprintf(stderr, "Invalid material id at particle command %d in file %s\n", command_number, path.c_str());
                    return 0;
                }
            }

            // Create particle
            R3Particle *particle = new R3Particle();
            particle->position = position;
            particle->velocity = velocity;
            particle->mass = mass;
            particle->fixed = (fixed) ? true : false;
            particle->drag = drag;
            particle->elasticity = elasticity;
            particle->lifetime = lifetime;
            particle->material = material;   
            particle->dead = false;
            particle->forces = R3Vector(0.0,0.0,0.0);

            // Add particle to scene
            particles.push_back(particle);

            // Update scene bounding box
            bbox.Union(position);

            // Add to list of particles available for springs
            particles_for_springs.push_back(particle);
        }
        else if (!strcmp(cmd, "particle_source"))
        {
            // Read particle source parameters 
            double mass, drag, elasticity, lifetime;
            double rate, velocity, angle_cutoff;
            int fixed, m;
            if (fscanf(fp, "%lf%d%lf%lf%lf%d%lf%lf%lf", &mass, &fixed, &drag, &elasticity, &lifetime, &m, &rate, &velocity, &angle_cutoff) != 9)
            {
                fprintf(stderr, "Unable to read particle source at command %d in file %s\n", command_number, path.c_str());
                return 0;
            }

            // Read shape
            R3Shape *shape = ReadShape(fp, command_number, path.c_str());
            if (!shape)
            {
                fprintf(stderr, "Unable to read particle source at command %d in file %s\n", command_number, path.c_str());
                return 0;
            }

            // Get material
            R3Material *material = group_materials[depth];
            if (m >= 0)
            {
                if (m < (int) materials.size())
                {
                    material = materials[m];
                }
                else
                {
                    fprintf(stderr, "Invalid material id at particle source command %d in file %s\n", command_number, path.c_str());
                    return 0;
                }
            }

            // Create particle source
            R3ParticleSource *source = new R3ParticleSource();
            source->mass = mass;
            source->fixed = (fixed) ? true : false;
            source->drag = drag;
            source->elasticity = elasticity;
            source->lifetime = lifetime;
            source->material = material;   
            source->rate = rate;
            source->velocity = velocity;
            source->angle_cutoff = angle_cutoff;
            source->shape = shape;   
            source->residual = 0.0;   

            // Add particle source to scene
            particle_sources.push_back(source);

            // Update scene bounding box
            bbox.Union(shape->BBox());
        }
        else if (!strcmp(cmd, "particle_sink"))
        {
            // Read sink parameters 
            double intensity, ca, la, qa;
            if (fscanf(fp, "%lf%lf%lf%lf", &intensity, &ca, &la, &qa) != 4)
            {
                fprintf(stderr, "Unable to read particle sink at command %d in file %s\n", command_number, path.c_str());
                return 0;
            }

            // Read shape
            R3Shape *shape = ReadShape(fp, command_number, path.c_str());
            if (!shape)
            {
                fprintf(stderr, "Unable to read particle source at command %d in file %s\n", command_number, path.c_str());
                return 0;
            }

            // Create particle sink
            R3ParticleSink *sink = new R3ParticleSink();
            sink->intensity = intensity;
            sink->constant_attenuation = ca;
            sink->linear_attenuation = la;
            sink->quadratic_attenuation = qa;
            sink->shape = shape;

            // Add particle sink to scene
            particle_sinks.push_back(sink);

            // Update scene bounding box
            bbox.Union(shape->BBox());
        }
        else if (!strcmp(cmd, "particle_spring"))
        {
            // Read gravity parameters 
            int id1, id2;
            double rest_length, ks, kd;
            if (fscanf(fp, "%d%d%lf%lf%lf", &id1, &id2, &rest_length, &ks, &kd) != 5)
            {
                fprintf(stderr, "Unable to read particle spring at command %d in file %s\n", command_number, path.c_str());
                return 0;
            }

            // Get particles
            R3Particle *particle1 = particles_for_springs[id1];
            R3Particle *particle2 = particles_for_springs[id2];

            // Create particle spring
            R3ParticleSpring *spring = new R3ParticleSpring();
            spring->particles[0] = particle1;
            spring->particles[1] = particle2;
            spring->rest_length = rest_length;
            spring->ks = ks;
            spring->kd = kd;

            // Insert spring into particles
            particle1->springs.push_back(spring);
            particle2->springs.push_back(spring);

            // Insert spring into scene
            particle_springs.push_back(spring);
        }
        else if (!strcmp(cmd, "particle_gravity"))
        {
            // Read gravity parameters 
            if (fscanf(fp, "%lf%lf%lf", &gravity[0], &gravity[1], &gravity[2]) != 3)
            {
                fprintf(stderr, "Unable to read particle gravity at command %d in file %s\n", command_number, path.c_str());
                return 0;
            }
        }

        // --- shapes --------------------------------------------------------------------------

#define READ_SHAPE(func, num, type) \
        do \
        { \
            if (func != num) \
            { \
                fprintf(stderr, "Bad " type " at command %d in file %s\n", command_number, path.c_str()); \
                return 0; \
            } \
            material = group_materials[depth]; \
            if (m >= 0) \
            { \
                if (m < (int) materials.size()) \
                    material = materials[m]; \
                else \
                { \
                    fprintf(stderr, "Invalid material id at box command %d in file %s\n", command_number, path.c_str()); \
                    return 0; \
                } \
            } \
        } \
        while (0)

        else if (!strcmp(cmd, "box"))
        {
            R3Point p1, p2;
            int m; R3Material *material;
            READ_SHAPE(fscanf(fp, "%d%lf%lf%lf%lf%lf%lf", &m, &p1[0], &p1[1], &p1[2], &p2[0], &p2[1], &p2[2]), 7, "box");
            group_nodes[depth]->AddChild(new R3Node(new R3Box(p1, p2), material));
        }
        else if (!strcmp(cmd, "sphere"))
        {
            R3Point c; double r;
            int m; R3Material *material;
            READ_SHAPE(fscanf(fp, "%d%lf%lf%lf%lf", &m, &c[0], &c[1], &c[2], &r), 5, "sphere");
            group_nodes[depth]->AddChild(new R3Node(new R3Sphere(c, r), material));
        }
        else if (!strcmp(cmd, "cylinder"))
        {
            R3Point c; double r, h;
            int m; R3Material *material;
            READ_SHAPE(fscanf(fp, "%d%lf%lf%lf%lf%lf", &m, &c[0], &c[1], &c[2], &r, &h), 6, "cyldiner");
            group_nodes[depth]->AddChild(new R3Node(new R3Cylinder(c, r, h), material));
        }
        else if (!strcmp(cmd, "mesh"))
        {
            char meshname[256];
            int m; R3Material *material;
            READ_SHAPE(fscanf(fp, "%d%s", &m, meshname), 2, "mesh");

            R3Mesh *mesh = new R3Mesh();
            if (!mesh->Read(meshname))
            {
                fprintf(stderr, "Unable to read mesh: %s\n", meshname);
                return 0;
            }

            group_nodes[depth]->AddChild(new R3Node(mesh, material));
        }
        else if (!strcmp(cmd, "cone"))
        {
            R3Point c;
            double r, h;
            int m; R3Material *material;
            READ_SHAPE(fscanf(fp, "%d%lf%lf%lf%lf%lf", &m, &c[0], &c[1], &c[2], &r, &h), 6, "cone");
            group_nodes[depth]->AddChild(new R3Node(new R3Cone(c, r, h), material));
        }
        else if (!strcmp(cmd, "line"))
        {
            R3Point p1, p2;
            int m; R3Material *material;
            READ_SHAPE(fscanf(fp, "%d%lf%lf%lf%lf%lf%lf", &m, &p1[0], &p1[1], &p1[2], &p2[0], &p2[1], &p2[2]), 7, "line");
            group_nodes[depth]->AddChild(new R3Node(new R3Segment(p1, p2), material));
        }

        // --- scenegraph ----------------------------------------------------------------------

        else if (!strcmp(cmd, "begin"))
        {
            // Read data
            double matrix[16];
            int m; R3Material *material;
            READ_SHAPE(fscanf(fp, "%d%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf", &m, 
                        &matrix[0], &matrix[1], &matrix[2], &matrix[3], 
                        &matrix[4], &matrix[5], &matrix[6], &matrix[7], 
                        &matrix[8], &matrix[9], &matrix[10], &matrix[11], 
                        &matrix[12], &matrix[13], &matrix[14], &matrix[15]), 17, "begin");
            R3Node *node = new R3Node(R3Matrix(matrix));
            group_nodes[depth]->AddChild(node);
            group_nodes[++depth] = node;
            group_materials[depth] = material;
        }
        else if (!strcmp(cmd, "end"))
        {
            // Pop node and transform bounding box
            R3Node *node = group_nodes[depth];
            depth--;
            node->bbox.Transform(node->transformation);
        }

        // --- material ------------------------------------------------------------------------

        else if (!strcmp(cmd, "material"))
        {
            // Read data
            R3Rgb ka, kd, ks, kt, e;
            double n, ir;
            char texture_name[256];
            if (fscanf(fp, "%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%s", 
                        &ka[0], &ka[1], &ka[2], &kd[0], &kd[1], &kd[2], &ks[0], &ks[1], &ks[2], &kt[0], &kt[1], &kt[2], 
                        &e[0], &e[1], &e[2], &n, &ir, texture_name) != 18)
            {
                fprintf(stderr, "Unable to read material at command %d in file %s\n", command_number, path.c_str());
                return 0;
            }

            // Create material
            R3Material *material = new R3Material();
            material->ka = ka;
            material->kd = kd;
            material->ks = ks;
            material->kt = kt;
            material->emission = e;
            material->shininess = n;
            material->indexofrefraction = ir;
            material->texture = NULL;

            // Read texture
            if (strcmp(texture_name, "0"))
            {
                // Read texture image
                material->texture = new R2Image();
                if (!material->texture->Read(texture_name))
                {
                    fprintf(stderr, "Unable to read texture from %s at command %d in file %s\n", texture_name, command_number, path.c_str());
                    return 0;
                }
            }

            // Insert material
            materials.push_back(material);
        }
        else if (!strcmp(cmd, "background"))
        {
            // Read data
            double r, g, b;
            if (fscanf(fp, "%lf%lf%lf", &r, &g, &b) != 3)
            {
                fprintf(stderr, "Unable to read background at command %d in file %s\n", command_number, path.c_str());
                return 0;
            }

            // Assign background color
            background = R3Rgb(r, g, b, 1);
        }

        // --- lights --------------------------------------------------------------------------

        else if (!strcmp(cmd, "dir_light"))
        {
            // Read data
            R3Rgb c;
            R3Vector d;
            if (fscanf(fp, "%lf%lf%lf%lf%lf%lf", 
                        &c[0], &c[1], &c[2], &d[0], &d[1], &d[2]) != 6)
            {
                fprintf(stderr, "Unable to read directional light at command %d in file %s\n", command_number, path.c_str());
                return 0;
            }

            // Normalize direction
            d.Normalize();

            // Create light
            R3Light *light = new R3Light();
            light->type = R3_DIRECTIONAL_LIGHT;
            light->color = c;
            light->position = R3Point(0, 0, 0);
            light->direction = d;
            light->radius = 0;
            light->constant_attenuation = 0;
            light->linear_attenuation = 0;
            light->quadratic_attenuation = 0;
            light->angle_attenuation = 0;
            light->angle_cutoff = M_PI;

            // Insert light
            lights.push_back(light);
        }
        else if (!strcmp(cmd, "point_light"))
        {
            // Read data
            R3Rgb c;
            R3Point p;
            double ca, la, qa;
            if (fscanf(fp, "%lf%lf%lf%lf%lf%lf%lf%lf%lf", &c[0], &c[1], &c[2], &p[0], &p[1], &p[2], &ca, &la, &qa) != 9)
            {
                fprintf(stderr, "Unable to read point light at command %d in file %s\n", command_number, path.c_str());
                return 0;
            }

            // Create light
            R3Light *light = new R3Light();
            light->type = R3_POINT_LIGHT;
            light->color = c;
            light->position = p;
            light->direction = R3Vector(0, 0, 0);
            light->radius = 0;
            light->constant_attenuation = ca;
            light->linear_attenuation = la;
            light->quadratic_attenuation = qa;
            light->angle_attenuation = 0;
            light->angle_cutoff = M_PI;

            // Insert light
            lights.push_back(light);
        }
        else if (!strcmp(cmd, "spot_light"))
        {
            // Read data
            R3Rgb c;
            R3Point p;
            R3Vector d;
            double ca, la, qa, sc, sd;
            if (fscanf(fp, "%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf", 
                        &c[0], &c[1], &c[2], &p[0], &p[1], &p[2], &d[0], &d[1], &d[2], &ca, &la, &qa, &sc, &sd) != 14)
            {
                fprintf(stderr, "Unable to read point light at command %d in file %s\n", command_number, path.c_str());
                return 0;
            }

            // Normalize direction
            d.Normalize();

            // Create light
            R3Light *light = new R3Light();
            light->type = R3_SPOT_LIGHT;
            light->color = c;
            light->position = p;
            light->direction = d;
            light->radius = 0;
            light->constant_attenuation = ca;
            light->linear_attenuation = la;
            light->quadratic_attenuation = qa;
            light->angle_attenuation = sd;
            light->angle_cutoff = sc;

            // Insert light
            lights.push_back(light);
        }
        else if (!strcmp(cmd, "area_light"))
        {
            // Read data
            R3Rgb c;
            R3Point p;
            R3Vector d;
            double radius, ca, la, qa;
            if (fscanf(fp, "%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf", 
                        &c[0], &c[1], &c[2], &p[0], &p[1], &p[2], &d[0], &d[1], &d[2], &radius, &ca, &la, &qa) != 13)
            {
                fprintf(stderr, "Unable to read area light at command %d in file %s\n", command_number, path.c_str());
                return 0;
            }

            // Normalize direction
            d.Normalize();

            // Create light
            R3Light *light = new R3Light();
            light->type = R3_AREA_LIGHT;
            light->color = c;
            light->position = p;
            light->direction = d;
            light->radius = radius;
            light->constant_attenuation = ca;
            light->linear_attenuation = la;
            light->quadratic_attenuation = qa;
            light->angle_attenuation = 0;
            light->angle_cutoff = M_PI;

            // Insert light
            lights.push_back(light);
        }
        else if (!strcmp(cmd, "ambient"))
        {
            // Read data
            double r, g, b;
            if (fscanf(fp, "%lf%lf%lf", &r, &g, &b) != 3)
            {
                fprintf(stderr, "Unable to read ambient at command %d in file %s\n", command_number, path.c_str());
                return 0;
            }

            // Assign ambient color
            ambient = R3Rgb(r, g, b, 1);
        }

        // --- camera --------------------------------------------------------------------------

        else if (!strcmp(cmd, "camera"))
        {
            // Read data
            double px, py, pz, dx, dy, dz, ux, uy, uz, xfov, neardist, fardist;
            if (fscanf(fp, "%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf", &px, &py, &pz, &dx, &dy, &dz, &ux, &uy, &uz, &xfov, &neardist, &fardist) != 12)
            {
                fprintf(stderr, "Unable to read camera at command %d in file %s\n", command_number, path.c_str());
                return 0;
            }

            // Assign camera
            camera.eye = R3Point(px, py, pz);
            camera.towards = R3Vector(dx, dy, dz);
            camera.towards.Normalize();
            camera.up = R3Vector(ux, uy, uz);
            camera.up.Normalize();
            camera.right = camera.towards % camera.up;
            camera.right.Normalize();
            camera.up = camera.right % camera.towards;
            camera.up.Normalize();
            camera.xfov = xfov;
            camera.yfov = xfov;
            camera.neardist = neardist;
            camera.fardist = fardist;
        }

        // --- scene ---------------------------------------------------------------------------

        else if (!strcmp(cmd, "include"))
        {
            // Read data
            char scenename[256];
            if (fscanf(fp, "%s", scenename) != 1)
            {
                fprintf(stderr, "Unable to read include command %d in file %s\n", command_number, path.c_str());
                return 0;
            }

            // Read scene from included file
            if (!Read(scenename, group_nodes[depth]))
            {
                fprintf(stderr, "Unable to read included scene: %s\n", scenename);
                return 0;
            }
        }
        else
        {
            fprintf(stderr, "Unrecognized command %d in file %s: %s\n", command_number, path.c_str(), cmd);
            return 0;
        }

        // Increment command number
        command_number++;
    }

    // Update bounding box
    bbox.Union(root->bbox);

    // Provide default camera
    if (camera.xfov == 0)
    {
        double scene_radius = bbox.DiagonalRadius();
        R3Point scene_center = bbox.Centroid();
        camera.towards = R3Vector(0, 0, -1);
        camera.up = R3Vector(0, 1, 0);
        camera.right = R3Vector(1, 0, 0);
        camera.eye = scene_center - 3 * scene_radius * camera.towards;
        camera.xfov = 0.25;
        camera.yfov = 0.25;
        camera.neardist = 0.01 * scene_radius;
        camera.fardist = 100 * scene_radius;
    }

    // Provide default lights
    if (lights.size() == 0)
    {
        // Create first directional light
        R3Light *light = new R3Light();
        R3Vector direction(-3,-4,-5);
        direction.Normalize();
        light->type = R3_DIRECTIONAL_LIGHT;
        light->color = R3Rgb(1,1,1,1);
        light->position = R3Point(0, 0, 0);
        light->direction = direction;
        light->radius = 0;
        light->constant_attenuation = 0;
        light->linear_attenuation = 0;
        light->quadratic_attenuation = 0;
        light->angle_attenuation = 0;
        light->angle_cutoff = M_PI;
        lights.push_back(light);

        // Create second directional light
        light = new R3Light();
        direction = R3Vector(3,2,3);
        direction.Normalize();
        light->type = R3_DIRECTIONAL_LIGHT;
        light->color = R3Rgb(0.5, 0.5, 0.5, 1);
        light->position = R3Point(0, 0, 0);
        light->direction = direction;
        light->radius = 0;
        light->constant_attenuation = 0;
        light->linear_attenuation = 0;
        light->quadratic_attenuation = 0;
        light->angle_attenuation = 0;
        light->angle_cutoff = M_PI;
        lights.push_back(light);
    }

    // Close file
    fclose(fp);

    // Return success
    return 1;
}



