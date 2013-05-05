#ifndef __R3MATERIAL_H__
#define __R3MATERIAL_H__

#include "R3/R3.h"

class R3Material
{
    private:
        static const R3Material *current_material;   // the currently applied material

    public:
        R2Pixel ka;
        R2Pixel kd;
        R2Pixel ks;
        R2Pixel kt;
        R2Pixel emission;
        double shininess;
        double indexofrefraction;
        R2Image *texture;
        int texture_index;
        int id;

        void Load(void) const
        {
            GLfloat c[4];

            // skip if current
            if (current_material == this)
                return;
            current_material = this;

            // load ambient, diffuse, specular, emmision, shininess

            double opacity = 1 - kt.Luminance();

            c[0] = ka[0]; c[1] = ka[1]; c[2] = ka[2]; c[3] = opacity;
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, c);

            c[0] = kd[0]; c[1] = kd[1]; c[2] = kd[2]; c[3] = opacity;
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c);

            c[0] = ks[0]; c[1] = ks[1]; c[2] = ks[2]; c[3] = opacity;
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);

            c[0] = emission.Red(); c[1] = emission.Green(); c[2] = emission.Blue(); c[3] = opacity;
            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, c);

            c[0] = shininess;
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, c[0]);

            // Load texture
            if (texture)
            {
                if (texture_index <= 0)
                {
                    // Create texture in OpenGL
                    GLuint texture_index;
                    glGenTextures(1, &texture_index);
                    texture_index = (int) texture_index;
                    glBindTexture(GL_TEXTURE_2D, texture_index); 
                    R2Image *image = texture;
                    int npixels = image->NPixels();
                    R2Pixel *pixels = image->Pixels();
                    GLfloat *buffer = new GLfloat [ 4 * npixels ];
                    R2Pixel *pixelsp = pixels;
                    GLfloat *bufferp = buffer;
                    for (int j = 0; j < npixels; j++)
                    {
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
                glBindTexture(GL_TEXTURE_2D, texture_index); 
                glEnable(GL_TEXTURE_2D);
            }
            else
            {
                glDisable(GL_TEXTURE_2D);
            }

            // Enable blending for transparent surfaces
            if (opacity < 1)
            {
                glDepthMask(false);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glEnable(GL_BLEND);
            }
            else
            {
                glDisable(GL_BLEND);
                glBlendFunc(GL_ONE, GL_ZERO);
                glDepthMask(true);
            }
        }
};

#endif
