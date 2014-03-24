#ifndef __particle_h__
#define __particle_h__

typedef struct {
	float x, y, z;
	float l;
} particle_t;



extern void particle_system_render ();
extern bool particle_init ();

#endif
