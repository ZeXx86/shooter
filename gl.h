#ifndef __gl_h__
#define __gl_h__

#include <math.h>

#include <SDL2/SDL.h>
#ifdef ANDROID
# include <SDL2/SDL_opengles.h>
#else
# include <SDL2/SDL_opengl.h>
#endif

#define FPS_MAX		120.0f
#define WALL_DIM	2.0f

extern bool gl_init ();
extern void gl_resize (int width, int height);
extern void gl_render ();

#endif

