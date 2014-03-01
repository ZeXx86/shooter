#ifndef __gl_h__
#define __gl_h__

#include <math.h>

#include <SDL2/SDL.h>
#ifdef ANDROID
# include <SDL2/SDL_opengles.h>
#else
# include <SDL2/SDL_opengl.h>

# ifdef __WIN32__
#  pragma comment (lib,"opengl32.lib")
#  pragma comment (lib,"glu32.lib")
# endif
#endif

#define FPS_MAX		60.0f
#define NEAR_PLANE 	0.15
#define FAR_PLANE 	100.0
#define WALL_DIM	2.0f

extern bool gl_init ();
extern void gl_resize (int width, int height);
extern void gl_render ();

#endif

