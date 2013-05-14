#include "glutfix.h"

#include "R3Material.h"

const R3Material *R3Material::lastMaterial = NULL;

void R3Material::Load(void) const
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
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureIndex); 
    }
    else
        glDisable(GL_TEXTURE_2D);


    // transparency
    if (params.additive)
    {
        glDepthMask(false);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }
    else if (opacity < 1 || transparent)
    {
        glDepthMask(false);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
    {
        glDepthMask(true);
        glDisable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ZERO);
    }

    // shader
#if USE_SHADERS
    glUseProgram(shader);
#endif
}

void R3Material::Initialize(void)
{
    // texture
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

        string transparentName = params.textureName.c_str();
        transparentName = transparentName.insert(transparentName.find_last_of("."), "_transparent");
        R2Image transparentImage = R2Image();
        transparent = transparentImage.Read(transparentName);

        int npixels = textureImage->NPixels();
        R2Pixel *pixels = textureImage->Pixels();
        GLfloat *buffer = new GLfloat [ 4 * npixels ];
        R2Pixel *pixelsp = pixels;

        R2Pixel *transPixelsp = pixelsp;
        if (transparent)
            transPixelsp = transparentImage.Pixels();

        cout << transparentName << " " << transparent << endl;

        GLfloat *bufferp = buffer;
        for (int j = 0; j < npixels; j++)
        {
            *(bufferp++) = pixelsp->Red();
            *(bufferp++) = pixelsp->Green();
            *(bufferp++) = pixelsp->Blue();
            if (transparent) {
                *(bufferp++) = transPixelsp->Red();
            } else {
                *(bufferp++) = pixelsp->Alpha();
            }

            pixelsp++;
            transPixelsp++;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, 4, textureImage->Width(), textureImage->Height(), 0, GL_RGBA, GL_FLOAT, buffer);
        //gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, textureImage->Width(), textureImage->Height(), GL_RGBA, GL_FLOAT, buffer);
        delete [] buffer;
    }

    // shaders
#if USE_SHADERS
    if (params.vertShaderName != "" && params.fragShaderName != "")
    {
        GLuint v = glCreateShader(GL_VERTEX_SHADER);
        GLuint f = glCreateShader(GL_FRAGMENT_SHADER);

        const char *vs = Util::textFileRead((DIR_SHADERS + params.vertShaderName).c_str());
        const char *fs = Util::textFileRead((DIR_SHADERS + params.fragShaderName).c_str());

        if (vs && fs)
        {
            glShaderSource(v, 1, &vs, NULL);
            glShaderSource(f, 1, &fs, NULL);

            glCompileShader(v);
            glCompileShader(f);

            delete vs;
            delete fs;

            shader = glCreateProgram();
            glAttachShader(shader, v);
            glAttachShader(shader, f);

            glLinkProgram(shader);
        }
    }
#endif
}
