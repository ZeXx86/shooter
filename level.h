#ifndef __level_h__
#define __level_h__

#include <iostream>
#include <fstream>
#include <string>

#ifdef ANDROID
# define PATH_LEVEL	"/sdcard/shooter/level/"
#else
# define PATH_LEVEL	"level/"
#endif

typedef struct {
	unsigned dim_x;
	unsigned dim_y;

	unsigned char *data;
} level_t;

extern level_t *level_get ();
extern bool level_init ();
extern void level_deinit ();

#endif

