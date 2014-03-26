#ifndef __particle_h__
#define __particle_h__

typedef struct {
	float x, y, z;	// pozice castice
	float l;	// lifetime castice

	float u, v;	// smer letu castice [uhel]
} particle_t;


extern void particle_update_ballistic ();
extern void particle_system_render ();
extern bool particle_init ();

#endif
