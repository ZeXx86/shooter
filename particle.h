#ifndef __particle_h__
#define __particle_h__

typedef struct {
	float x, y, z;	// pozice castice
	float l;	// lifetime castice

	float u, v;	// smer letu castice [uhel]
	float s; // rychlost
	float t; //cas
} particle_t;

extern void particle_reset (float x, float y, float z, float u, float v);
extern void particle_update_ballistic ();
extern void particle_system_render ();
extern bool particle_init ();


#endif
