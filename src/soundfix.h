#ifndef __SOUNDFIX_H__
#define __SOUNDFIX_H__

/*
 * hax to get sound to work
 */

#if defined(_WIN32) || defined(__CYGWIN__)
#    define __NOSOUND__
#elif defined(__APPLE__)
#    define __NOSOUND__
#endif

#endif
