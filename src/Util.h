#ifndef __UTIL_H__
#define __UTIL_H__

#ifdef _WIN32
#  include <windows.h>
#else
#  include <sys/time.h>
#endif
#include "stdlib.h"

#include "R3/R3.h"

namespace Util {

    double GetTime(void);            // time in seconds
    double UnitRandom(void);         // random in [0, 1]
    double SymmetricRandom(void);    // random in [-1, 1]
    R3Vector BallRandom(double r);   // random in ball of radius r
    R3Point PointFromMatrix(const R3Matrix &mat);
    GLuint GetTransparentTexture(char *image, char *trans);
}

#endif
