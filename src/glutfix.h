#ifndef __GLUTFIX_H__
#define __GLUTFIX_H__

/* 
 * hax to get glut to work
 */

#if defined(_WIN32) || defined(__CYGWIN__)
#    ifndef USE_OPENGL32
#        define USE_OPENGL32
#    endif
#    ifndef NOMINMAX
#        define NOMINMAX
#    endif
#    include <windows.h>
#    include <GL/glut.h>
#elif defined(__APPLE__)
#    include <GLUT/glut.h>
#else 
#    include <GL/glut.h>
#endif

#endif
