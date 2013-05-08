#include "Util.h"

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

}



