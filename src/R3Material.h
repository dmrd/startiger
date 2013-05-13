#ifndef __R3MATERIAL_H__
#define __R3MATERIAL_H__

#include "R3/R3.h"

#include <string>

class R3Material
{
    public:
        struct Params
        {
            R2Pixel ka;
            R2Pixel kd;
            R2Pixel ks;
            R2Pixel kt;
            R2Pixel emission;
            double shininess;
            bool lit; // Disable lighting for this material
            //double indexofrefraction;
            string textureName;

            Params() :
                ka(R3Rgb(0.2, 0.2, 0.2, 1)),
                kd(R3Rgb(0.5, 0.5, 0.5, 1)),
                ks(R3Rgb(0.5, 0.5, 0.5, 1)),
                kt(R3Rgb(0.0, 0.0, 0.0, 1)),
                emission(R3Rgb(0.0, 0.0, 0.0, 1)),
                shininess(10),
                lit(true),
                textureName("")
            {
            }
        };

        R3Material(const Params &params_) :
            params(params_),
            textureImage(NULL),
            initialized(false)
        {
            Initialize();
        }

        void Load(void) const
        {
            if (params.lit) {
                glEnable(GL_LIGHTING);
            } else {
                glDisable(GL_LIGHTING);
            }
            // skip if previous
            if (lastMaterial == this)
                return;
            lastMaterial = this;

            // ambient, diffuse, specular, emmision, shininess
            GLfloat c[4];
            double opacity = 1 - params.kt.Luminance();
            c[0] = params.ka[0]; c[1] = params.ka[1]; c[2] = params.ka[2]; c[3] = opacity;
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, c);
            c[0] = params.kd[0]; c[1] = params.kd[1]; c[2] = params.kd[2]; c[3] = opacity;
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c);
            c[0] = params.ks[0]; c[1] = params.ks[1]; c[2] = params.ks[2]; c[3] = opacity;
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);
            c[0] = params.emission.Red(); c[1] = params.emission.Green(); c[2] = params.emission.Blue(); c[3] = opacity;
            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, c);
            c[0] = params.shininess;
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, c[0]);

            // texture
            if (textured)
            {
                glBindTexture(GL_TEXTURE_2D, textureIndex); 
                glEnable(GL_TEXTURE_2D);
            }
            else
                glDisable(GL_TEXTURE_2D);

            // transparency
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

        void SetColor(void) {
            glColor4d(params.kd[0], params.kd[1], params.kd[2], params.kd[3]);
        }

        void SetColor(double alpha) {
            glColor4d(params.kd[0], params.kd[1], params.kd[2], params.kd[3]*alpha);
        }
    private:
        void Initialize(void)
        {
            if (params.textureName == "")
                textured = false;
            else
            {
                textured = true;

                glGenTextures(1, &textureIndex);
                glBindTexture(GL_TEXTURE_2D, textureIndex); 

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

                textureImage = new R2Image(params.textureName.c_str());
                int npixels = textureImage->NPixels();
                R2Pixel *pixels = textureImage->Pixels();
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

                glTexImage2D(GL_TEXTURE_2D, 0, 4, textureImage->Width(), textureImage->Height(), 0, GL_RGBA, GL_FLOAT, buffer);
                delete [] buffer;
            }
        }

        bool initialized;

        Params params;

        static const R3Material *lastMaterial;   // the last loaded material

        bool textured;
        R2Image *textureImage;
        GLuint textureIndex;
};

#endif
