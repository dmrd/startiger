#ifndef __R3MATERIAL_H__
#define __R3MATERIAL_H__

#include "R3/R3.h"

#include <string>
#include <iostream>

#include "Util.h"
#include "Dirs.h"

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
            bool additive;
            bool defaultShaders;
            //double indexofrefraction;
            string textureName;
            string vertShaderName;
            string fragShaderName;

            Params() :
                ka(R3Rgb(0.2, 0.2, 0.2, 1)),
                kd(R3Rgb(0.5, 0.5, 0.5, 1)),
                ks(R3Rgb(0.5, 0.5, 0.5, 1)),
                kt(R3Rgb(0.0, 0.0, 0.0, 1)),
                emission(R3Rgb(0.0, 0.0, 0.0, 1)),
                shininess(10),
                lit(true),
                textureName(""),
                vertShaderName(""),
                fragShaderName(""),
                additive(false),
                defaultShaders(true)
            {
            }
        };

        R3Material(const Params &params_) :
            params(params_),
            textureImage(NULL),
            initialized(false),
            shader(0)
        {
            Initialize();
        }

        void Load(void) const;

        void SetColor(void) {
            glColor4d(params.kd[0], params.kd[1], params.kd[2], 1 - params.kt.Luminance());
        }

        void SetColor(double alpha) {
            glColor4d(params.kd[0], params.kd[1], params.kd[2], alpha);
        }
    private:
        void Initialize(void);

        bool initialized;

        Params params;

        static const R3Material *lastMaterial;   // the last loaded material

        bool textured;
        bool transparent;
        R2Image *textureImage;
        GLuint textureIndex;
        GLuint shader;
};

#endif
