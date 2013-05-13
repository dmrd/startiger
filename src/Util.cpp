#include "Util.h"
#include "R2/R2Image.h"
#include "R2/R2Pixel.h"

#include <fstream>

namespace Util {

    double GetTime(void)
    {
#ifdef _WIN32
        // Return number of seconds since start of execution
        static int first = 1;
        static LARGE_INTEGER timefreq;
        static LARGE_INTEGER start_timevalue;

        // Check if this is the first time
        if (first)
        {
            // Initialize first time
            QueryPerformanceFrequency(&timefreq);
            QueryPerformanceCounter(&start_timevalue);
            first = 0;
            return 0;
        }
        else
        {
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
        if (first)
        {
            // Initialize first time
            gettimeofday(&start_timevalue, NULL);
            first = 0;
            return 0;
        }
        else
        {
            // Return time since start
            struct timeval current_timevalue;
            gettimeofday(&current_timevalue, NULL);
            int secs = current_timevalue.tv_sec - start_timevalue.tv_sec;
            int usecs = current_timevalue.tv_usec - start_timevalue.tv_usec;
            return (double) (secs + 1.0E-6F * usecs);
        }
#endif
    }

    // random in [0, 1]
    double UnitRandom(void)
    {
#ifdef _WIN32
        // Seed random number generator
        static int first = 1;
        if (first)
        {
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
        if (first)
        {
            struct timeval timevalue;
            gettimeofday(&timevalue, 0);
            srand48(timevalue.tv_usec);
            first = 0;
        }

        // Return random number
        return drand48();
#endif
    }

    // return random in [-1, 1]
    double SymmetricRandom(void)
    {
        return 2 * UnitRandom() - 1;
    }

    // return random vector in unit ball of radius r
    R3Vector BallRandom(double r)
    {
        R3Vector V;
        do
            V.Reset(SymmetricRandom(),
                    SymmetricRandom(),
                    SymmetricRandom());
        while (V.Dot(V) > 1.0);
        return r * V;
    }

    R3Point PointFromMatrix(const R3Matrix &mat) {
        return R3Point(mat[0][3], mat[1][3], mat[2][3]);
    }


    GLuint GetTransparentTexture(const char *imageName, const char *transName) {

        // Read texture image
        R2Image img = R2Image();
        if (!img.Read(imageName)) {
            return -1;

        }
        R2Image transparent_part = R2Image();
        // Get texture filename

        // Read texture image
        if (!transparent_part.Read(transName)) {
            return -1;
        }
        GLuint texName;

        glGenTextures(1, &texName);
        glBindTexture(GL_TEXTURE_2D, texName);
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

        // when texture area is small, bilinear filter the closest mipmap
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                GL_LINEAR_MIPMAP_NEAREST );
        // when texture area is large, bilinear filter the first mipmap
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        bool wrap = true;
        // if wrap is true, the texture wraps over at the edges (repeat)
        //       ... false, the texture ends at the edges (clamp)
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                wrap ? GL_REPEAT : GL_CLAMP );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                wrap ? GL_REPEAT : GL_CLAMP );

        // build our texture mipmaps
        unsigned char *data = new unsigned char[img.Width()*img.Height()*4];
        for (int i = 0; i < img.Width(); i++) {
            for (int j = 0; j < img.Height(); j++) {
                for (int k = 0; k < 4; k++) {
                    if (k == 3)
                        data[i * img.Height()*4 + j * 4 + k] = transparent_part.Pixel(i,j)[0]*255;
                    else
                        data[i * img.Height()*4 + j * 4 + k] = img.Pixel(i,j)[k]*255;
                }
            }
        }
        gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(),
                GL_RGBA, GL_UNSIGNED_BYTE, data );

        delete data;

        return texName;

    }

}


char *Util::textFileRead(const char *fn) {
    FILE *fp;
    char *content = NULL;

    int count=0;

    if (fn != NULL) {
        fp = fopen(fn,"rt");

        if (fp != NULL) {

            fseek(fp, 0, SEEK_END);
            count = ftell(fp);
            rewind(fp);

            if (count > 0) {
                content = new char[count + 1];//(char *)malloc(sizeof(char) * (count+1));
                count = fread(content,sizeof(char),count,fp);
                content[count] = '\0';
            }
            fclose(fp);
        }
    }
    return content;
}

