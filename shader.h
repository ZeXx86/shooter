#ifndef __shader_h__
#define __shader_h__

#include <iostream>
#include <string.h>
#include <fstream>

extern void shader_getuniform_light (GLuint prog, light_t *l);
extern GLuint shader_init (char *file);

#endif

