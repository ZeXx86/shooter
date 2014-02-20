#ifndef __tex_h__
#define __tex_h__

#ifdef ANDROID
# include <SDL/SDL_opengles.h>
#else
# include <SDL/SDL_opengl.h>
#endif
#include <SDL/SDL_image.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern GLuint tex_get (unsigned id);
extern bool tex_init ();
extern void tex_deinit ();

#endif

