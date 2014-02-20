#ifndef __tex_h__
#define __tex_h__

#ifdef ANDROID
# include <SDL2/SDL_opengles.h>
#else
# include <SDL2/SDL_opengl.h>
#endif
#include <SDL2/SDL_image.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern GLuint tex_get (unsigned id);
extern bool tex_init ();
extern void tex_deinit ();

#endif

