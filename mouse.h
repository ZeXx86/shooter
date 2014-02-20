#ifndef __mouse_h__
#define __mouse_h__

#include <iostream>

typedef struct {
	int x;
	int y;

	int abs_x;
	int abs_y;

	unsigned char state;
} mouse_t;

extern mouse_t *mouse_get ();

#endif

