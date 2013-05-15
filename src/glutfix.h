#ifndef __GLUTFIX_H__
#define __GLUTFIX_H__

/* 
 * hax to get glut to work
 */


#if defined(_WIN32) || defined(__CYGWIN__)
#    define USE_SHADERS 0
#    ifndef USE_OPENGL32
#        define USE_OPENGL32
#    endif
#    ifndef NOMINMAX
#        define NOMINMAX
#    endif
#    include <windows.h>
#    include <GL/glut.h>
#elif defined(__APPLE__)
#    define USE_SHADERS 1
#    include <GL/glew.h>
#    include <GLUT/glut.h>
#else 
#    define USE_SHADERS 1
#    include <GL/glew.h>
#    include <GL/glut.h>
#endif

#endif
